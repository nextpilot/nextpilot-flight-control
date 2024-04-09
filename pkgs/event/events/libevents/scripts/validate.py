#!/usr/bin/env python

"""
    Script to validate event definition files
"""

import argparse
import json
import os
import sys
import re

import common

try:
    from jsonschema import validate
except ImportError as e:
    print("Failed to import jsonschema: " + str(e))
    print("")
    print("You may need to install it using:")
    print("    pip3 install --user jsonschema")
    print("")
    sys.exit(1)

def dict_raise_on_duplicates(ordered_pairs):
    """Reject duplicate keys"""
    return_dict = {}
    for key, value in ordered_pairs:
        if key in return_dict:
            raise ValueError("duplicate key: {:}".format(key))
        return_dict[key] = value
    return return_dict

def main():
    """ main method """
    # Parse command line arguments
    parser = argparse.ArgumentParser(description="Validate event definition files.")
    parser.add_argument("files",
                        default=[],
                        metavar="EVENT.JSON",
                        nargs='+',
                        help="one or more event definition files")

    parser.add_argument('-v', '--verbose',
                        action='store_true',
                        help='Verbose Output')

    args = parser.parse_args()
    input_files = args.files
    verbose = args.verbose

    cur_dir = os.path.dirname(os.path.realpath(__file__))
    schema_file = os.path.join(cur_dir, '../validation/schema.json')
    with open(schema_file, 'r', encoding='utf-8') as stream:
        schema = json.load(stream)

    # read configuration
    config = common.read_config()

    events = {}
    for input_file in input_files:
        if verbose:
            print("Validating {:}".format(input_file))
        with open(input_file, 'r', encoding='utf-8') as json_file:
            events = json.load(json_file, object_pairs_hook=dict_raise_on_duplicates)

        try:
            validate(instance=events, schema=schema)
            extra_validation(events, config)
        except:
            print("Error: validation for {:} failed.\nschema: {:}".format(input_file, schema_file))
            raise

def validate_event_description(description: str, num_args: int):
    """ validate event description or message
     * Supported parsing:
     * - characters can be escaped with \\, e.g. '\\<', '\\{'
     * - tags:
     *   - <profile name="[!]NAME">CONTENT</profile>
     *   - <a [href="URL"]>CONTENT</a>
     *     if href is not found, use CONTENT as url
     *   - <param>PARAM_NAME</param>
     *   - unknown tags are ignored (including content)
     *   - no nested tags of the same type
     * - arguments: following python syntax, with 1-based indexing (instead of 0)
     *   and custom types (units)
     *   - general form: {ARG_IDX[:.NUM_DECIMAL_DIGITS][UNIT]}
     *     UNIT:
     *     - m: horizontal distance in meters
     *     - m_v: vertical distance in meters
     *     - m^2: area in m^2
     *     - m/s: speed in m/s
     *     - C: temperature in degrees celcius
    """

    # remove escaped characters to simplify parsing
    check_str = description
    backslash_idx = check_str.find('\\')
    while backslash_idx != -1:
        check_str = check_str[0:backslash_idx]+check_str[backslash_idx+2:]
        backslash_idx = check_str.find('\\')

    i = 0
    url_allowed = False
    while i < len(check_str):

        # enforce using tags for url's
        if not url_allowed and (check_str[i:i+7] == 'http://' or check_str[i:i+8] == 'https://'):
            raise ValueError("freestanding url found in:\n\n{:}\n\n" \
                    "Use a tag in one of these formats:\n" \
                    "- <a>LINK</a>\n" \
                    "- <a href=\"LINK\">DESCRIPTION</a>".format(description))
        url_allowed = False

        if check_str[i] == '<':
            # extract tag with 1 optional argument. Be strict with spacing to
            # simplify library implementations
            m = re.match(r"^<([a-z]+)(?: ([a-z]+)=\"([^\"]*)\")?>(.)", check_str[i:], re.DOTALL)
            if not m:
                raise ValueError("Invalid tag format in:\n\n{:}\n\n" \
                        "General form: <TAG[ ARG=\"VAL\"]>CONTENT</TAG>\n"
                        "Use \\< to escape a single '<'".format(description))
            #print(m.groups())

            tag_name = m.group(1)

            # "unknown" is for the tests
            known_tags = ["a", "param", "profile", "unknown"]
            if not tag_name in known_tags:
                raise ValueError("Unknown tag '<{:}>' in:\n\n{:}\n\n" \
                        "Known tags: {:}".format(tag_name, description, known_tags))

            if tag_name == "profile":
                known_profiles = ["dev", "normal"]
                if m.group(3) is None:
                    raise ValueError("Missing profile name in:\n\n{:}\n\n" \
                            "".format(description))
                profile = m.group(3)
                if profile.startswith('!'):
                    profile = profile[1:]
                if m.group(2) != "name" or not profile in known_profiles:
                    raise ValueError("Unknown profile '{:}={:}' in:\n\n{:}\n\n" \
                            "Known profiles: {:}".format(
                                m.group(2), profile, description, known_profiles))
            elif tag_name == 'a':
                url_allowed = True

            content_start_idx = m.start(4)
            end_tag_idx = check_str.find('</'+tag_name+'>', i+content_start_idx)
            if end_tag_idx == -1:
                raise ValueError("Ending tag for '<{:}>' not found in:\n\n{:}\n\n" \
                        "".format(tag_name, description))
            tag_content = check_str[i+content_start_idx:end_tag_idx]

            # check for nested tags
            if '<'+tag_name in tag_content:
                raise ValueError("Unsupported nested tag found for '<{:}>' in:\n\n{:}\n\n" \
                        "".format(tag_name, description))

            # continue with checking the tag content
            check_str = check_str[:i] + tag_content + check_str[end_tag_idx+len(tag_name)+3:]

        elif check_str[i] == '{':
            arg_end_idx = check_str.find('}', i)
            if arg_end_idx == -1:
                raise ValueError("Invalid argument, no '}}' found in:\n\n{:}\n\n" \
                        "Use escaping for a literal output: '\\{{'".format(description))
            arg = check_str[i+1:arg_end_idx]
            m = re.match(r"^(\d+)(?::(?:\.(\d+))?)?(m|m_v|m/s|m\^2|C)?$", arg)
            if not m:
                raise ValueError("Invalid argument ('{{{:}}}') in:\n\n{:}\n\n" \
                        "General form: {{ARG_IDX[:.NUM_DECIMAL_DIGITS][UNIT]}}" \
                        .format(arg, description))
            #print(m.groups())
            arg_idx = int(m.group(1)) - 1
            if arg_idx < 0 or arg_idx >= num_args:
                raise ValueError("Invalid argument index ({:}) in:\n\n{:}\n\n" \
                        "Valid range: [1..{:}]".format(arg_idx+1, description, num_args))

            i += len(arg) + 2
        elif check_str[i] in ('}', '>'):
            raise ValueError("Found stray '{:}' in:\n\n{:}\n\n" \
                    "You might want to escape it with '\\'".format(check_str[i], description))
        else:
            i += 1

def validate_group(event, group_name, event_name, arguments):
    """ rules for certain groups """

    def validate_arg_name(event, arguments, index, name):
        assert event['arguments'][index]['name'] == name, \
            "event '{:}' argument {:} name must " \
            "be '{:}', but is '{:}'".format(
            event_name, index, name, event['arguments'][index]['name'])

    def validate_arg_type(event, arguments, index, type_name):
        assert arguments[index] == type_name, \
            "event '{:}' argument {:} type must " \
            "be '{:}', but is '{:}'".format(
            event_name, index, type_name, arguments[index])

    if group_name in ('arming_check', 'health'):
        event_type = event.get('type', '')
        if event_type == 'summary':
            assert len(arguments) >= 4, \
                "missing arguments for health/arming_check summary event {:}".format(event_name)

            validate_arg_name(event, arguments, 0, 'chunk_idx')
            validate_arg_type(event, arguments, 0, 'uint8_t')

            assert arguments[1] not in common.base_types, \
                "health/arming_check event {:} argument 1 must " \
                "be a mode group enum, but is {:}".format(
                    event_name, arguments[1])

            if group_name == 'arming_check':
                validate_arg_name(event, arguments, 1, 'error')
                validate_arg_name(event, arguments, 2, 'warning')
                validate_arg_type(event, arguments, 2, arguments[1])
                validate_arg_name(event, arguments, 3, 'can_arm')
                validate_arg_name(event, arguments, 4, 'can_run')
            else: # 'health'
                validate_arg_name(event, arguments, 1, 'is_present')
                validate_arg_name(event, arguments, 2, 'error')
                validate_arg_type(event, arguments, 2, arguments[1])
                validate_arg_name(event, arguments, 3, 'warning')
                validate_arg_type(event, arguments, 3, arguments[1])
        else:
            assert len(arguments) >= 2, \
                "missing arguments for health/arming_check event {:}".format(event_name)
            assert arguments[0] not in common.base_types, \
                "first health/arming_check event {:} argument must " \
                "be a mode group enum, but is {:}".format(
                    event_name, arguments[0])

            validate_arg_type(event, arguments, 1, 'uint8_t')
            validate_arg_name(event, arguments, 1, 'health_component_index')

def validate_type(event, group_name, event_name, arguments):
    """ rules for certain types """

    if not 'type' in event:
        return

    valid_types = []
    if group_name == 'default':
        valid_types = ['append_health_and_arming_messages']
        if event['type'] == valid_types[0]:
            # arg0 == mode
            assert arguments[0] == 'uint32_t', \
                "event '{:}' argument {:} must " \
                "be of type '{:}', but is '{:}'".format(
                event_name, 0, 'uint32_t', arguments[0])
    elif group_name in ('arming_check', 'health'):
        valid_types = ['summary']

    assert event['type'] in valid_types, \
            "event '{:}' type must be one of '{:}', but is '{:}'".format(
            event_name, valid_types, event['type'])


def validate_event_arguments(config, event, events, namespace):
    """ ensure all enums exist
        :return: list of argument types (normalized)
    """
    if not "arguments" in event:
        return []
    arguments = []
    arguments_size = 0
    for arg in event["arguments"]:
        if arg["type"] in common.base_types:
            base_type = arg["type"]
            arguments.append(base_type)
        else:
            try:
                (base_type, normalized_type) = \
                    common.base_type_from_enum(events, namespace, arg["type"])
            except:
                print("Exception trying to get enum type " \
                    "for event: {:}".format(event["name"]))
                raise
            arguments.append(normalized_type)
        arguments_size += common.base_types[base_type]['size']

    if arguments_size > int(config['max_arguments_size']):
        raise ValueError("Argument size exceeded for event {:} ({:} > {:})" \
                .format(namespace+'::'+event["name"], arguments_size, config['max_arguments_size']))
    return arguments

def extra_validation(events, config):
    """ Additional validation not possible with the schema.
        Includes:
        - unique names (enum, events, namespaces) & ID's (components & events)
        - special rules for certain event groups
        - event description & message
        - event argument types (base type or enum)
    """

    if not "components" in events:
        return
    all_namespaces = set()
    for comp_id in events["components"]:
        icomp_id = int(comp_id)
        assert 0 <= icomp_id <= 255, "component id out of range: {}".format(icomp_id)
        comp = events["components"][comp_id]
        namespace = comp["namespace"]

        if namespace in all_namespaces:
            raise ValueError("Duplicate namespace: {:}".format(namespace))
        all_namespaces.add(namespace)


        if "event_groups" in comp:
            all_event_id = set()
            all_event_names = set()
            for group_name in comp["event_groups"]:
                group = comp["event_groups"][group_name]
                if not "events" in group:
                    continue
                for event_sub_id in group["events"]:
                    sub_id = int(event_sub_id)
                    assert 0 <= sub_id <= 16777215, "event id out of range: {}".format(sub_id)
                    event = group["events"][event_sub_id]
                    event_name = event["name"]

                    if event_sub_id in all_event_id:
                        raise ValueError("Duplicate event id: {:} ({:})".format(
                            event_sub_id, event['name']))
                    all_event_id.add(event_sub_id)
                    if event_name in all_event_names:
                        raise ValueError("Duplicate event name: {:}".format(event_name))
                    all_event_names.add(event_name)

                    arguments = validate_event_arguments(config, event, events, namespace)

                    # rules for certain groups & types
                    validate_group(event, group_name, event_name, arguments)
                    validate_type(event, group_name, event_name, arguments)

                    validate_event_description(event["message"], len(arguments))
                    if "description" in event:
                        validate_event_description(event["description"], len(arguments))



if __name__ == "__main__":
    main()


