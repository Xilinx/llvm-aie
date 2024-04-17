# This file is licensed under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
# (c) Copyright 2024 Advanced Micro Devices, Inc. or its affiliates

# RUN: %python %s

import unittest
from mischedutils import schedlogparser


SCHED_REGION = r"""
********** MI Scheduling **********
double_bb:%bb.1 
  From: BeginInstr foo bar
    To: End
 RegionInstrs: 2
ScheduleDAGMI::schedule starting
#blah
#blah
SU(0):   renamable $r0 = FOO_INSTR_0 renamable $r0, implicit-def $status, implicit $control0, implicit $control1
  # Extra things...
  Latency            : 3
  Depth              : 0
  Height             : 5
  Successors:
    SU(1): Out  Latency=3
    ExitSU: Ord  Latency=2 Artificial
SU(1):   renamable $r1 = FOO_INSTR_1 renamable $r1, implicit-def $status, implicit $control0, implicit $control1
  # Extra things...
  Latency            : 2
  Depth              : 3
  Height             : 2
  Predecessors:
    SU(0): Out  Latency=3
  Successors:
    ExitSU: Ord  Latency=2 Artificial
#blah
#blah
** ScheduleDAGMI::schedule picking next node
#blah
#blah
Queue BotQ.P: 
Queue BotQ.A: 1 
Pick Bot Reason-1 Reason-details
Scheduling SU(1) renamable $r1 = FOO_INSTR_1 renamable $r1, implicit-def $status, implicit $control0, implicit $control1
  Ready @1c
#blah
#blah
** ScheduleDAGMI::schedule picking next node
#blah
#blah
Cycle: 4 BotQ.A
Queue BotQ.P: 
Queue BotQ.A: 0 
Pick Bot Reason Reason-details     
Scheduling SU(0) renamable $r0 = FOO_INSTR_0 renamable $r0, implicit-def $status, implicit $control0, implicit $control1
  Ready @4c
#blah
#blah
** ScheduleDAGMI::schedule picking next node
#blah
#blah
*** Final schedule for %bb.1 ***
SU(0):   renamable $r0 = FOO_INSTR_0 renamable $r0, implicit-def $status, implicit $control0, implicit $control1
SU(1):   renamable $r1 = FOO_INSTR_1 renamable $r1, implicit-def $status, implicit $control0, implicit $control1
"""


class TestLogParser(unittest.TestCase):

    def setUp(self) -> None:
        self.maxDiff = None

    def test_one_region(self):
        r = schedlogparser.process_region(
            "double_bb", "bb.1", "BeginMI", "EndMI", SCHED_REGION
        )

        # Quickly verify that the numbers of SUs and actions match
        self.assertEqual(len(r.sched_units), 2)
        self.assertEqual(len(r.sched_actions), 2)

        exp_su = {
            0: schedlogparser.ScheduleUnit(
                su_num=0,
                instr="renamable $r0 = FOO_INSTR_0 renamable $r0, implicit-def $status, implicit $control0, implicit $control1",
                latency=3,
                depth=0,
                height=5,
            ),
            1: schedlogparser.ScheduleUnit(
                su_num=1,
                instr="renamable $r1 = FOO_INSTR_1 renamable $r1, implicit-def $status, implicit $control0, implicit $control1",
                latency=2,
                depth=3,
                height=2,
            ),
        }
        self.assertEqual(r.sched_units, exp_su)

        exp_actions = [
            schedlogparser.ScheduleAction(
                new_cycle=None,
                pending=[],
                available=[1],
                picked_zone="Bot",
                picked_reason="Reason-1",
                picked_su=1,
                picked_cycle=1,
            ),
            schedlogparser.ScheduleAction(
                new_cycle=4,
                pending=[],
                available=[0],
                picked_zone="Bot",
                picked_reason="Reason",
                picked_su=0,
                picked_cycle=4,
            ),
        ]
        self.assertEqual(r.sched_actions, exp_actions)

        self.assertEqual(
            r,
            schedlogparser.RegionSchedule(
                "double_bb", "bb.1", "BeginMI", "EndMI", exp_su, exp_actions
            ),
        )

    def test_whole_log(self):
        sched_log = f"{SCHED_REGION}foo\nbar\n{SCHED_REGION}"
        regs = schedlogparser.process_log(sched_log)
        self.assertEqual(len(regs), 2)
        self.assertEqual(regs[0], regs[1])


if __name__ == "__main__":
    unittest.main()
