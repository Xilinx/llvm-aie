# This file is licensed under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
# (c) Copyright 2024 Advanced Micro Devices, Inc. or its affiliates

import re
from dataclasses import dataclass
from typing import Dict, List, Optional

SU_STR = r"SU\((?P<su_num>[0-9]+)\):\s+(?P<su_mi>.+)\n"
SU_DESC_RE = re.compile(
    rf"^{SU_STR}"
    r"(.*\n)+?"
    r"^\s+Latency\s+: (?P<su_latency>[0-9]+)\n"
    r"^\s+Depth\s+: (?P<su_depth>[0-9]+)\n"
    r"^\s+Height\s+: (?P<su_height>[0-9]+)\n"
    r"(.*\n)+?",
    re.M,
)


@dataclass
class ScheduleUnit:
    """Corresponds to an LLVM SUnit, i.e. a node of the scheduling DAG."""

    su_num: int
    instr: str
    latency: int
    depth: int
    height: int


SCHED_ACTION_RE = re.compile(
    r"^\*\* ScheduleDAGMI::schedule picking next node\n"
    r"(.*\n)+?"
    r"(^Cycle: (?P<new_cycle>[0-9]+) (?P<bumped_zone>\w+)Q\.A\n)?"
    r"^Queue BotQ\.P: (?P<pending>[0-9 ]*)\n"
    r"^Queue BotQ\.A: (?P<available>[0-9 ]*)\n"
    r"(?P<pick_details>(.*\n)*?)"
    r"^Pick (?P<picked_zone>\S+) (?P<picked_reason>\S+).*?\n"
    r"^Scheduling SU\((?P<picked_su>[0-9]+)\).+?\n"
    r"^\s+Ready @(?P<ready_cycle>[0-9]+)c\n",
    re.M,
)


@dataclass
class ScheduleAction:
    """A decision of the scheduler, i.e. which node was just scheduled."""

    new_cycle: Optional[int]
    pending: List[int]
    available: List[int]
    pick_details: str
    picked_zone: str
    picked_reason: str
    picked_su: int
    picked_cycle: int


# Note "ScheduleDAGMI::schedule" makes sure we only match regions for the
# postmisched. The pre-RA ScheduleDAGMILive is not supported yet.
SCHED_REGION_RE = re.compile(
    r"^[*]+ MI Scheduling [*]+\n"
    r"^(?P<sched_fn>[\w.]+):%(?P<sched_bb>[\w.]+).*\n"
    r"^\s+From:\s+(?P<begin_mi>.+)\n"
    r"^\s+To:\s+(?P<end_mi>.+)\n"
    r"^\s+RegionInstrs:\s+(?P<num_instrs>[0-9]+)\n"
    r"^ScheduleDAGMI::schedule starting\n"
    r"(.*\n)+?"
    r"^[*]+ Final schedule for %(?P=sched_bb) [*]+\n"
    rf"(^{SU_STR})+",
    re.M,
)


@dataclass
class RegionSchedule:
    """A region of the code and its corresponding schedule."""

    sched_fn: str
    sched_bb: str
    begin_mi: str
    end_mi: str
    sched_units: Dict[int, ScheduleUnit]
    sched_actions: List[ScheduleAction]

    def get_sched_units(self, su_nums: List[int]) -> List[ScheduleUnit]:
        """Return the SUnits that have the given numbers."""
        return [self.sched_units[su_num] for su_num in su_nums]


def int_or_none(s: str) -> Optional[int]:
    """Convert the string to an int, or return None if it is empty."""
    if s:
        return int(s)
    return None


def process_region(sched_fn, sched_bb, begin_mi, end_mi, region_str) -> RegionSchedule:
    """Process the scheduling log of a region into a RegionSchedule object."""
    sched_units = dict()
    for su in re.finditer(SU_DESC_RE, region_str):
        su_num = int(su["su_num"])
        sched_units[su_num] = ScheduleUnit(
            su_num,
            su["su_mi"],
            int(su["su_latency"]),
            int(su["su_depth"]),
            int(su["su_height"]),
        )

    sched_actions: List[ScheduleAction] = []
    for action in re.finditer(SCHED_ACTION_RE, region_str):
        assert not action["bumped_zone"] or action["bumped_zone"] == "Bot"
        pending = [int(num) for num in action["pending"].split()]
        available = [int(num) for num in action["available"].split()]
        sched_actions.append(
            ScheduleAction(
                int_or_none(action["new_cycle"]),
                pending,
                available,
                action["pick_details"],
                action["picked_zone"],
                action["picked_reason"],
                int(action["picked_su"]),
                int(action["ready_cycle"]),
            )
        )

    if len(sched_actions) != region_str.count("Scheduling SU"):
        for sched_action in sched_actions:
            print(sched_action)
        print(
            f"Error: Actions={len(sched_actions)} "
            f"Schedules={region_str.count('Scheduling SU')}"
        )
        exit(1)

    return RegionSchedule(
        sched_fn, sched_bb, begin_mi, end_mi, sched_units, sched_actions
    )


def process_log(log_str) -> List[RegionSchedule]:
    """Process the scheduling log into a list of RegionSchedule."""
    regions = []
    for region in re.finditer(SCHED_REGION_RE, log_str):
        r = process_region(
            region["sched_fn"],
            region["sched_bb"],
            region["begin_mi"],
            region["end_mi"],
            region.group(),
        )
        regions.append(r)
    return regions
