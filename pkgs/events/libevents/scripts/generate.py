#!/usr/bin/env python

"""
    Script to generate code from jinja2 templates and an event file
"""

import argparse
import json
import sys
import os

import common

try:
    from jinja2 import Environment, FileSystemLoader
except ImportError as e:
    print("Failed to import jinja2: " + str(e))
    print("")
    print("You may need to install it using:")
    print("    pip3 install --user jinja2")
    print("")
    sys.exit(1)

def main():
    """ main method """
    # Parse command line arguments
    parser = argparse.ArgumentParser(description="Code generation from event definitions.")
    parser.add_argument("file",
                        metavar="EVENT.JSON",
                        help="event definition file")

    req = parser.add_mutually_exclusive_group(required=True)
    req.add_argument('-l', '--language',
                     type=str,
                     action='store',
                     help='Output language (if template not provided)',
                     choices=['cpp'])

    req.add_argument('-t', '--template',
                     type=str,
                     action='store',
                     help='Template file (if language not provided)')

    parser.add_argument('-o', '--output',
                        type=str,
                        action='store',
                        help='Output directory',
                        required=True)

    args = parser.parse_args()
    input_file = args.file
    output_dir = args.output

    template_files = []
    if args.template:
        template_files = [args.template]
    else:
        cur_dir = os.path.dirname(os.path.realpath(__file__))
        if args.language == 'cpp':
            template_files = \
                [os.path.join(cur_dir, '../libs/cpp/templates/events_generated.h.jinja')]

    # read configuration
    config = common.read_config()

    constants = {}
    constants["base_types"] = common.base_types

    with open(input_file, 'r', encoding='utf-8') as json_file:
        events = json.load(json_file)
        assert "version" in events
        assert events["version"] == 1 or events["version"] == 2

        add_info(events, constants["base_types"])

        generate_output(events, config, constants, template_files, output_dir)

def add_info(events, base_types):
    """ add additional information to events:
        - the real ID (combined component + sub_id)
        - for each arguments:
          - base_type: underlying base type (for enums)
          - is_enum boolean
          - normalize enum names to <namespace>::<name>
    """
    if not "components" in events:
        return
    for comp_id in events["components"]:
        comp = events["components"][comp_id]
        if not "event_groups" in comp:
            continue
        namespace = comp["namespace"]
        for group_name in comp["event_groups"]:
            group = comp["event_groups"][group_name]
            if not "events" in group:
                continue
            for event_sub_id in group["events"]:
                event = group["events"][event_sub_id]

                event["id"] = (int(comp_id) << 24) | int(event_sub_id)

                if "arguments" in event:
                    for arg in event["arguments"]:
                        if arg["type"] in base_types:
                            arg["base_type"] = arg["type"]
                            arg["is_enum"] = False
                        else:
                            arg["is_enum"] = True
                            (arg["base_type"], arg["type"]) = \
                                common.base_type_from_enum(events, namespace, arg["type"])

def jinja_cpp_namespace(arg_type: str):
    """ jinja filter to convert an enum type to a C++ namespace
    """
    if arg_type in common.base_types:
        return arg_type
    return arg_type.replace('::', '::enums::')

def generate_output(events, config, constants, template_files, output_dir):
    """ generate outputs from a list of template files and event definitions """
    for template_file in template_files:
        jinja_env = Environment(loader=FileSystemLoader(
            os.path.dirname(template_file)))

        jinja_env.filters['cpp_namespace'] = jinja_cpp_namespace

        template_basename = os.path.basename(template_file)
        template = jinja_env.get_template(template_basename)
        output_file = os.path.join(output_dir, os.path.splitext(template_basename)[0])

        with open(output_file, 'w', encoding='utf-8') as f:
            f.write(template.render(events=events, config=config,
                                    constants=constants))

if __name__ == "__main__":
    main()


