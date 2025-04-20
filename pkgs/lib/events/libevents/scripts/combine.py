#!/usr/bin/env python

"""
    Script to combine several event definition files
"""

import argparse
import json

def main():
    """ main method """
    # Parse command line arguments
    parser = argparse.ArgumentParser(description="Combine event definition files.")
    parser.add_argument("files",
                        default=[],
                        metavar="EVENT.JSON",
                        nargs='+',
                        help="one or more event definition files")

    parser.add_argument('-o', '--output',
                        type=str,
                        action='store',
                        help='Output file',
                        required=True)

    args = parser.parse_args()
    input_files = args.files
    output_file = args.output

    events = {}
    for input_file in input_files:
        with open(input_file, 'r', encoding='utf-8') as json_file:
            new_events = json.load(json_file)
            assert "version" in new_events
            assert new_events["version"] == 1 or new_events["version"] == 2

            if not events:
                events = new_events
                continue

            # translation: just need to make sure we pick one
            if "translation" in new_events and "translation" not in events:
                events["translation"] = new_events["translation"]

            # merge new_events into events
            for comp_id in new_events["components"]:
                component = new_events["components"][comp_id]

                if comp_id in events["components"]:
                    matching_comp = events["components"][comp_id]

                    # same component already exists: try to merge

                    assert matching_comp["namespace"] == component["namespace"], \
                        "Namespaces with equal component ID's must match"

                    # enums
                    for enum_name in component.get("enums", []):
                        enum = component['enums'][enum_name]
                        if not "enums" in matching_comp:
                            matching_comp["enums"] = {}
                        assert not enum_name in matching_comp["enums"], \
                            "enum collision: {:}".format(enum_name)
                        matching_comp["enums"][enum_name] = enum

                    # event groups
                    for group_name in component.get("event_groups", []):
                        group = component['event_groups'][group_name]
                        if not "event_groups" in matching_comp:
                            matching_comp["event_groups"] = {}

                        if group_name in matching_comp["event_groups"]:
                            matching_group = matching_comp["event_groups"][group_name]
                            # events
                            for event_sub_id in group["events"]:
                                event = group["events"][event_sub_id]
                                assert not event_sub_id in matching_group["events"], \
                                    "ID collision: {:}".format(event_sub_id)
                                assert not any(event["name"] == \
                                    matching_group["events"][e]["name"] \
                                    for e in matching_group["events"]), \
                                    "event name collision: {:}".format(event["name"])
                                matching_group["events"][event_sub_id] = event
                        else:
                            matching_comp["event_groups"][group_name] = group

                    if "supported_protocols" in component:
                        matching_comp["supported_protocols"] = \
                            list(set(matching_comp.get("supported_protocols", []) +
                            component["supported_protocols"]))

                    if "navigation_mode_groups" in component:
                        assert "navigation_mode_groups" not in matching_comp
                        matching_comp["navigation_mode_groups"] = \
                            component["navigation_mode_groups"]
                else:
                    events["components"][comp_id] = component

    with open(output_file, 'w', encoding='utf-8') as f:
        f.write(json.dumps(events, indent=2, sort_keys=True))

if __name__ == "__main__":
    main()

