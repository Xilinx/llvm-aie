# This file is licensed under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
# (c) Copyright 2024 Advanced Micro Devices, Inc. or its affiliates

# RUN: %python %s

import unittest
from mischedutils import miutils


class TestMIUtils(unittest.TestCase):

    def test_trim_instr(self):
        instr1 = "renamable $v0, $p0 = LD_vec $p0(tied-def 1), killed $m0 :: (load (<8 x s32>) from %ir.ptr_in)"
        self.assertEqual(miutils.trim_instr_str(instr1), "$v0, $p0 = LD_vec $p0, $m0")

        instr2 = "ST_vec $p0, 32, $v0, $m0, implicit-def $status_0, implicit $cr_0, implicit $cr_1"
        self.assertEqual(
            miutils.trim_instr_str(instr2),
            "ST_vec $p0, 32, $v0, $m0, $status_0, $cr_0, $cr_1",
        )


if __name__ == "__main__":
    unittest.main()
