# This file is licensed under the Apache License v2.0 with LLVM Exceptions.
# See https://llvm.org/LICENSE.txt for license information.
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
# (c) Copyright 2024 Advanced Micro Devices, Inc. or its affiliates

import argparse
from mischedutils import schedlogparser


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("sched_log")
    parser.add_argument("-d", "--dump", action="store_true")
    args = parser.parse_args()

    regions = []
    with open(args.sched_log, "r") as file:
        regions = schedlogparser.process_log(file.read())

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

        app = interactive_sched.InteractiveMISchedApp(regions)
        app.run()


if __name__ == "__main__":
    main()
