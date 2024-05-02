# This file is licensed under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
# (c) Copyright 2024 Advanced Micro Devices, Inc. or its affiliates

from typing import List

from mischedutils import miutils, schedlogparser
from textual.app import App, ComposeResult
from textual.containers import VerticalScroll
from textual.screen import Screen
from textual.widgets import Header, Footer, OptionList, Static

INSTR_CELL_SIZE = 48


class CycleLine(Static):
    """A single-line widget representing a VLIW packet."""

    def __init__(self, cycle: int) -> None:
        super().__init__()
        self.cycle = cycle

    def compose(self) -> ComposeResult:
        """Create the initial widget, showing the cycle number."""
        yield Static(f"c{self.cycle}", classes="cycle_num")

    def add_su(self, su: schedlogparser.ScheduleUnit):
        """Add a SUnit to this cycle."""
        new_inst = Static(miutils.trim_instr_str(su.instr)[:INSTR_CELL_SIZE])
        new_inst.styles.width = INSTR_CELL_SIZE
        self.mount(new_inst)
        self.scroll_visible()


class RegionSchedScreen(Screen):
    """A screen representing the schedule for one region."""

    BINDINGS = [("escape", "app.pop_screen", "Regions"), ("n", "schedule_next", "Next")]
    CSS = """
    CycleLine {
        layout: horizontal;
    }
    CycleLine > Static {
        height: 1;
        margin-right: 2;
        background: $boost;
    }
    .cycle_num {
        width: 4;
    }
    VerticalScroll {
        border: dodgerblue;
    }
    """

    def __init__(self, region: schedlogparser.RegionSchedule):
        super().__init__()
        self.sched_region = region
        self.title = (
            f"Region: {self.sched_region.sched_fn}:{self.sched_region.sched_bb}"
        )
        self.next_action_idx = 0

    def compose(self) -> ComposeResult:
        """Create the screen with VLIW cycles and various info widgets."""
        yield Header()

        actions = self.sched_region.sched_actions
        max_cycle = max(actions, key=lambda a: a.picked_cycle).picked_cycle
        self.cycles = [CycleLine(c) for c in range(max_cycle + 1)]
        cycle_container = VerticalScroll(*self.cycles[::-1], id="cycles")
        cycle_container.border_title = "Schedule"
        yield cycle_container

        yield Footer()

    def action_schedule_next(self):
        """Add the next instruction to the schedule."""
        if self.next_action_idx >= len(self.sched_region.sched_actions):
            return
        action = self.sched_region.sched_actions[self.next_action_idx]
        picked_su = self.sched_region.sched_units[action.picked_su]
        self.cycles[action.picked_cycle].add_su(picked_su)
        self.next_action_idx += 1


class InteractiveMISchedApp(App):
    """A Textual app to visualize MachineScheduler decisions."""

    BINDINGS = [("d", "toggle_dark", "Dark mode")]

    def __init__(self, regions: List[schedlogparser.RegionSchedule]):
        super().__init__()
        self.regions = regions

    def compose(self) -> ComposeResult:
        """Create a home screen with the list of parsed regions."""
        yield Header()
        options = [f"{r.sched_fn}:{r.sched_bb}" for r in self.regions]
        yield OptionList(*options)
        yield Footer()

    def on_option_list_option_selected(self, opt: OptionList.OptionSelected):
        """Push a screen to interact with the schedule of the selected region."""
        selected_region = self.regions[opt.option_index]
        self.push_screen(RegionSchedScreen(selected_region))

    def action_toggle_dark(self) -> None:
        """An action to toggle dark mode."""
        self.dark = not self.dark
