""" helper code for tests """

import struct


def create_args(arg_list):
    """ create byte array arguments from list of (value, type) arguments """
    base_types = {
        "uint8_t": {"size": 1, "pack": 'B'},
        "int8_t": {"size": 1, "pack": 'b'},
        "uint16_t": {"size": 2, "pack": 'H'},
        "int16_t": {"size": 2, "pack": 'h'},
        "uint32_t": {"size": 4, "pack": 'I'},
        "int32_t": {"size": 4, "pack": 'i'},
        "uint64_t": {"size": 8, "pack": 'Q'},
        "int64_t": {"size": 8, "pack": 'q'},
        "float": {"size": 4, "pack": 'f'},
        }
    pack_str = '<'
    values = []
    for val, type_name in arg_list:
        values.append(val)
        pack_str += base_types[type_name]['pack']
    return struct.pack(pack_str, *values)
