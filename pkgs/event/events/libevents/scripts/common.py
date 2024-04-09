""" Shared code and data """
import configparser
import os

base_types = {
    "uint8_t": {"size": 1},
    "int8_t": {"size": 1},
    "uint16_t": {"size": 2},
    "int16_t": {"size": 2},
    "uint32_t": {"size": 4},
    "int32_t": {"size": 4},
    "uint64_t": {"size": 8},
    "int64_t": {"size": 8},
    "float": {"size": 4},
    }

def base_type_from_enum(events, default_namespace: str, enum: str):
    """ returns teh base type given an enum
        enum is expected in the form: [<namespace>::]<name>
        :return: tuple of (base_type, normalized enum)
    """
    if '::' in enum:
        [namespace, enum_type] = enum.split('::')
    else:
        namespace = default_namespace
        enum_type = enum
    for comp_id in events["components"]:
        comp = events["components"][comp_id]
        if comp["namespace"] != namespace:
            continue
        if not "enums" in comp:
            continue
        for enum_name in comp["enums"]:
            e = comp["enums"][enum_name]
            if enum_name == enum_type:
                return (e["type"], namespace+'::'+enum_type)
    raise ValueError("enum '{:}' definition not found.\nSupported base types: {}" \
                    .format(enum, list(base_types.keys())))

def read_config():
    """ read configuration file
        :return: dict of 'events' section
    """
    conf = configparser.ConfigParser()
    cur_dir = os.path.dirname(os.path.realpath(__file__))
    with open(os.path.join(cur_dir, '../config.ini'), encoding='utf-8') as config_file:
        conf.read_file(config_file)
    return dict(conf.items("events"))

