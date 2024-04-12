#!/usr/bin/env python
# /*****************************************************************
#  *     _   __             __   ____   _  __        __
#  *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
#  *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
#  *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
#  * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
#  *
#  * Copyright All Reserved © 2015-2024 NextPilot Development Team
#  ******************************************************************/

#
# events processor (main executable file)
#
# This tool scans the source code for definitions of events.
#

import sys
import os
import argparse
from px4events import srcscanner, srcparser, jsonout

import re
import codecs


def main():
    # Parse command line arguments
    parser = argparse.ArgumentParser(description="Process events definitions.")
    parser.add_argument(
        "-s",
        "--src-path",
        default=["../src"],
        metavar="PATH",
        nargs="*",
        help="one or more paths/files to source files to scan for events",
    )
    parser.add_argument(
        "-j",
        "--json",
        nargs="?",
        const="events.json",
        metavar="FILENAME",
        help="Create Json output file" " (default FILENAME: events.json)",
    )
    parser.add_argument("-v", "--verbose", action="store_true", help="verbose output")

    args = parser.parse_args()

    # Check for valid command
    if not (args.json):
        print("Error: You need to specify at least one output method!")
        parser.print_usage()
        sys.exit(1)

    # Initialize source scanner and parser
    scanner = srcscanner.SourceScanner()
    parser = srcparser.SourceParser()

    # Scan directories, and parse the files
    if args.verbose:
        print("Scanning source path/files " + str(args.src_path))

    # canonicalize + remove duplicates
    src_paths = set()
    for path in args.src_path:
        src_paths.add(os.path.realpath(path))

    if not scanner.ScanDir(src_paths, parser):
        sys.exit(1)

    events = parser.events

    # Output to JSON file
    if args.json:
        if args.verbose:
            print("Creating Json file " + args.json)
        cur_dir = os.path.dirname(os.path.realpath(__file__))
        out = jsonout.JsonOutput(events)
        out.save(args.json)


if __name__ == "__main__":
    main()
