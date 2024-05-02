# This file is licensed under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
# (c) Copyright 2024 Advanced Micro Devices, Inc. or its affiliates

import argparse
import io
import os
import sys

from mischedutils import schedlogparser


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("sched_log", help="Log file to parse, or '-' to read stdin")
    parser.add_argument(
        "-d", "--dump", action="store_true", help="dump the sched regions"
    )
    args = parser.parse_args()

    regions = []
    if args.sched_log != "-":
        if not sys.__stdin__.isatty():
            print(
                "Error: cannot get data from stdin and a file at the same time",
                file=sys.stderr,
            )
            exit(1)
        print(f"Parsing '{args.sched_log}'...")
        with open(args.sched_log, "r") as file:
            regions = schedlogparser.process_log(file.read())
    else:
        print("Parsing stdin...")
        regions = schedlogparser.process_log(sys.__stdin__.read())

    if args.dump:
        for region in regions:
            print(f"Region: {region.sched_fn}:{region.sched_bb}")
            for action in region.sched_actions:
                if action.new_cycle:
                    print(f"** Bump to c{action.new_cycle}")
                print(f"* {action}")
            print("")
    else:
        from mischedutils import interactive_sched

        # Make sure Textual gets a fresh stdin for user keyboard inputs.
        sys.__stdin__.flush()
        sys.__stdin__ = io.TextIOWrapper(io.FileIO(os.open(os.ctermid(), os.O_RDONLY)))

        app = interactive_sched.InteractiveMISchedApp(regions)
        app.run()


if __name__ == "__main__":
    main()
