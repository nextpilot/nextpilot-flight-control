#!/usr/bin/env python
# -*- coding: UTF-8 -*-
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
# parameters processor (main executable file)
#
# This tool scans the source code for declarations of tunable parameters
# and outputs the list in various formats.
#
# Currently supported formats are:
#   * XML for the parametric UI generator
#   * Human-readable description in Markdown page format for the dev guide
#

from __future__ import print_function
import sys
import os
import argparse
from px4params import srcscanner, srcparser, injectxmlparams, xmlout, markdownout, jsonout

import json


def save_compressed(filename):
    import lzma  # to create .xz file

    # create lzma compressed version
    xz_filename = filename + ".xz"
    with lzma.open(xz_filename, "wt", preset=9, encoding="utf-8") as f:
        with open(filename, "r", encoding="utf-8") as content_file:
            f.write(content_file.read())


def main():
    # Parse command line arguments
    parser = argparse.ArgumentParser(description="Process parameter documentation.")
    parser.add_argument(
        "-s",
        "--src-path",
        default=["../src"],
        metavar="PATH",
        nargs="*",
        help="one or more paths to source files to scan for parameters",
    )
    parser.add_argument(
        "-x",
        "--xml",
        nargs="?",
        const="parameters.xml",
        metavar="FILENAME",
        help="Create XML file" " (default FILENAME: parameters.xml)",
    )
    parser.add_argument(
        "-i",
        "--inject-xml",
        nargs="?",
        const="parameters_injected.xml",
        metavar="FILENAME",
        help="Inject additional param XML file" " (default FILENAME: parameters_injected.xml)",
    )
    parser.add_argument("-b", "--board", nargs="?", const="", metavar="BOARD", help="Board to create xml parameter xml for")
    parser.add_argument(
        "-m",
        "--markdown",
        nargs="?",
        const="parameters.md",
        metavar="FILENAME",
        help="Create Markdown file" " (default FILENAME: parameters.md)",
    )
    parser.add_argument(
        "-j",
        "--json",
        nargs="?",
        const="parameters.json",
        metavar="FILENAME",
        help="Create Json file" " (default FILENAME: parameters.json)",
    )
    parser.add_argument("-v", "--verbose", action="store_true", help="verbose output")
    parser.add_argument("-c", "--compress", action="store_true", help="compress parameter file")
    parser.add_argument(
        "-o", "--overrides", default="{}", metavar="OVERRIDES", help="a dict of overrides in the form of a json string"
    )

    args = parser.parse_args()

    # Check for valid command
    if not (args.xml or args.markdown or args.json):
        print("Error: You need to specify at least one output method!\n")
        parser.print_usage()
        sys.exit(1)

    # Initialize source scanner and parser
    scanner = srcscanner.SourceScanner()
    parser = srcparser.SourceParser()

    # Scan directories, and parse the files
    if args.verbose:
        print("Scanning source path " + str(args.src_path))

    if not scanner.ScanDir(args.src_path, parser):
        sys.exit(1)

    if not parser.Validate():
        sys.exit(1)
    param_groups = parser.GetParamGroups()

    if len(param_groups) == 0:
        print("Warning: no parameters found")

    # inject parameters at front of set
    cur_dir = os.path.dirname(os.path.realpath(__file__))
    groups_to_inject = injectxmlparams.XMLInject(os.path.join(cur_dir, args.inject_xml)).injected()
    param_groups = groups_to_inject + param_groups

    override_dict = json.loads(args.overrides)
    if len(override_dict.keys()) > 0:
        for group in param_groups:
            for param in group.GetParams():
                name = param.GetName()
                if name in override_dict.keys():
                    val = str(override_dict[param.GetName()])
                    param.default = val
                    print("OVERRIDING {:s} to {:s}!!!!!".format(name, val))

    output_files = []

    # Output to XML file
    if args.xml:
        if args.verbose:
            print("Creating XML file " + args.xml)
        out = xmlout.XMLOutput(param_groups, args.board)
        out.Save(args.xml)
        output_files.append(args.xml)

    # Output to Markdown/HTML tables
    if args.markdown:
        if args.verbose:
            print("Creating markdown file " + args.markdown)
        out = markdownout.MarkdownTablesOutput(param_groups)
        out.Save(args.markdown)
        output_files.append(args.markdown)

    # Output to JSON file
    if args.json:
        if args.verbose:
            print("Creating Json file " + args.json)
        cur_dir = os.path.dirname(os.path.realpath(__file__))
        out = jsonout.JsonOutput(param_groups, args.board, os.path.join(cur_dir, args.inject_xml))
        out.Save(args.json)
        output_files.append(args.json)

    if args.compress and sys.version_info >= (3, 3):
        for f in output_files:
            if args.verbose:
                print("Compressing file " + f)
            save_compressed(f)


if __name__ == "__main__":
    main()
