# This file is licensed under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
# (c) Copyright 2024 Advanced Micro Devices, Inc. or its affiliates

from typing import List

from mischedutils import schedlogparser
from textual.app import App, ComposeResult
from textual.widgets import Header, Footer, OptionList


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

    def action_toggle_dark(self) -> None:
        """An action to toggle dark mode."""
        self.dark = not self.dark
