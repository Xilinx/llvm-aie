# This file is licensed under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
# (c) Copyright 2024 Advanced Micro Devices, Inc. or its affiliates


def trim_instr_str(instr: str) -> str:
    """Reduce "bloat" from instructions so they are shorter."""

    # Remove operand qualifiers
    bloat = [
        "renamable ",
        "dead ",
        "killed ",
        "implicit ",
        "implicit-def ",
        "(tied-def 0)",
        "(tied-def 1)",
    ]
    for substr in bloat:
        instr = instr.replace(substr, "")

    # Remove MMOs and leading/trailing spaces
    return instr.split(":")[0].strip()
