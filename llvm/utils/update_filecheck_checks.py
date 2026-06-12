# This file is licensed under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
# (c) Copyright 2026 Advanced Micro Devices, Inc. or its affiliates
"""Apply FileCheck --show-diff patch blocks to test source files.

Usage:
  FILECHECK_OPTS="--show-diff" ninja check-llvm-codegen-aie 2>&1 | tee run.log
  llvm/utils/update_filecheck_checks.py run.log          # interactive
  llvm/utils/update_filecheck_checks.py --auto run.log   # automatic
  llvm/utils/update_filecheck_checks.py --dry-run run.log
  llvm/utils/update_filecheck_checks.py --file llvm/test/foo.mir run.log
"""
import argparse
import os
import re
import sys
from typing import List, Set

_RED = '\033[31m'
_GREEN = '\033[32m'
_YELLOW = '\033[33m'
_CYAN = '\033[36m'
_BOLD = '\033[1m'
_DIM = '\033[2m'
_RESET = '\033[0m'


def _c(text, code):
    return (code + text + _RESET) if sys.stdout.isatty() else text


def _hr():
    return _c('─' * 72, _DIM)


class Patch:
    def __init__(self):
        self.file = ''
        self.old = ''
        self.new = ''
        self.actual = ''
        self.check_type = ''
        self.check_line = 0

    def is_valid(self):
        return bool(self.file and self.check_line > 0 and self.old and self.new)


def _unquote(s):
    s = s.strip()
    return s[1:-1] if len(s) >= 2 and s[0] == s[-1] == "'" else s


def parse_log(text: str) -> List[Patch]:
    patches = []
    for m in re.finditer(
        r'--- FILECHECK-PATCH ---\n(.*?)--- END PATCH ---', text, re.DOTALL
    ):
        p = Patch()
        for line in m.group(1).split('\n'):
            if line.startswith('file: '):
                p.file = line[6:].strip()
            elif line.startswith('check-line: '):
                try:
                    p.check_line = int(line[12:].strip())
                except ValueError:
                    pass
            elif line.startswith('type: '):
                p.check_type = line[6:].strip()
            elif line.startswith('old: '):
                p.old = _unquote(line[5:])
            elif line.startswith('new: '):
                p.new = _unquote(line[5:])
            elif line.startswith('actual: '):
                p.actual = _unquote(line[8:])
        if p.is_valid():
            patches.append(p)
    return patches


def _resolve(filepath):
    if os.path.exists(filepath):
        return filepath
    root = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    cand = os.path.join(root, filepath)
    return cand if os.path.exists(cand) else None


def display(p: Patch, idx: int, total: int):
    print()
    print(_hr())
    print(
        f'{_c(f"Patch {idx}/{total}", _BOLD)}: '
        f'{_c(p.file + ":" + str(p.check_line), _CYAN)}  [{p.check_type}]'
    )
    print(f'\n  {_c("OLD", _RED)}: {p.old}')
    print(f'  {_c("NEW", _GREEN)}: {p.new}')
    if p.actual:
        print(f'  {_c("ACT", _CYAN)}: {p.actual}')


def apply_patch(p: Patch, dry_run=False) -> bool:
    fp = _resolve(p.file)
    if not fp:
        print(f'  {_c("ERROR", _RED)}: not found: {p.file}')
        return False
    try:
        lines = open(fp).readlines()
        actual = (
            lines[p.check_line - 1].rstrip('\n\r')
            if p.check_line <= len(lines)
            else ''
        )
    except OSError as e:
        print(f'  {_c("ERROR", _RED)}: {e}')
        return False
    if actual != p.old:
        print(f'  {_c("SKIP", _YELLOW)}: mismatch at {fp}:{p.check_line}')
        print(f'    Expected: {p.old!r}')
        print(f'    Found   : {actual!r}')
        return False
    if dry_run:
        print(f'  {_c("DRY-RUN", _CYAN)}: would update {fp}:{p.check_line}')
        return True
    try:
        orig = lines[p.check_line - 1]
        end = '\r\n' if orig.endswith('\r\n') else '\n' if orig.endswith('\n') else ''
        lines[p.check_line - 1] = p.new + end
        open(fp, 'w').writelines(lines)
        print(f'  {_c("APPLIED", _GREEN)}: {fp}:{p.check_line}')
        return True
    except OSError as e:
        print(f'  {_c("ERROR", _RED)}: {e}')
        return False


def _prompt(p: Patch, auto_files: Set[str]) -> str:
    if p.file in auto_files:
        return 'y'
    while True:
        try:
            ans = input('Apply? [y/n/e/s/a/q/?] ').strip().lower()
        except (EOFError, KeyboardInterrupt):
            print()
            return 'q'
        if ans in ('y', 'n', 'e', 's', 'a', 'q'):
            return ans
        if ans == '?':
            print('\n  y=apply n=skip e=editor s=auto-file a=auto-all q=quit\n')
        else:
            print('Enter y/n/e/s/a/q/?')


def main(argv=None):
    p = argparse.ArgumentParser(
        description='Apply FileCheck --show-diff patches.'
    )
    p.add_argument(
        'log', nargs='?', type=argparse.FileType('r'), default=sys.stdin
    )
    p.add_argument('--auto', action='store_true')
    p.add_argument('--dry-run', action='store_true')
    p.add_argument('--file', metavar='PATH', action='append', default=[])
    args = p.parse_args(argv)

    patches = parse_log(args.log.read())
    if not patches:
        print('No FILECHECK-PATCH blocks found.')
        return 0
    print(f'Found {len(patches)} patch(es).')
    if args.file:
        fs = set(args.file)
        patches = [
            x for x in patches
            if x.file in fs or any(x.file.endswith(f) for f in fs)
        ]
        print(f'Filtered to {len(patches)} patch(es).')

    applied = skipped = failed = 0
    auto_files: Set[str] = set()
    quit_now = False

    for i, patch in enumerate(patches, 1):
        if quit_now:
            skipped += 1
            continue
        display(patch, i, len(patches))
        action = 'y' if (args.auto or args.dry_run) else _prompt(patch, auto_files)
        if action == 'q':
            print('Quitting.')
            quit_now = True
            skipped += 1
            continue
        elif action == 'n':
            print('  Skipped.')
            skipped += 1
            continue
        elif action == 'e':
            editor = os.environ.get('EDITOR', 'vi')
            fp = _resolve(patch.file)
            if fp:
                print(f'  Proposed: {patch.new}')
                os.system(f'{editor} +{patch.check_line} {fp}')
            action = _prompt(patch, auto_files)
            if action not in ('y', 's', 'a'):
                skipped += 1
                continue
        if action == 's':
            auto_files.add(patch.file)
        elif action == 'a':
            args.auto = True
        if apply_patch(patch, dry_run=args.dry_run):
            applied += 1
        else:
            failed += 1

    print(f'\n{_hr()}')
    print(
        f'Summary: {_c(str(applied), _GREEN)} applied, '
        f'{_c(str(skipped), _YELLOW)} skipped, {_c(str(failed), _RED)} failed.'
    )
    return 0 if failed == 0 else 1


if __name__ == '__main__':
    sys.exit(main())
