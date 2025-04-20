#! /usr/bin/env python3

""" run parsing tests """

import os
import sys

from helper import create_args

#pylint: disable=wrong-import-position
sys.path.append(os.path.join(os.path.dirname(os.path.realpath(__file__)), '..'))
from libevents_parse.parser import Parser

#pylint: disable=invalid-name


cur_dir = os.path.dirname(os.path.realpath(__file__))
json_file = os.path.join(cur_dir, "../../test.json")

p = Parser(debug=True)
p.load_definition_file(json_file)
p.set_profile('dev')
p.set_url_formatter(lambda content, url: '#'+content+'<link='+url+'>#')

args = None
event_id = 2306236 | (1 << 24)
parsed_event = p.parse(event_id, args)
assert parsed_event
assert parsed_event.event_id() == event_id
assert parsed_event.name() == "test1"
assert parsed_event.group() == "default"
assert parsed_event.num_arguments() == 0
print("message={}".format(parsed_event.message()))
print("description={}".format(parsed_event.description()))
assert parsed_event.message() == "Test"
assert parsed_event.description() == ""
print("")


a = -3
b = -9238
c = 832223
d = -9277377287318721
e = 3423423.3423
args = create_args([(a, 'int8_t'), (b, 'int16_t'), (c, 'int32_t'), (d, 'int64_t'), (e, 'float'), ])
event_id = 2307445 | (1 << 24)
parsed_event = p.parse(event_id, args)

assert parsed_event
assert parsed_event.event_id() == event_id
assert parsed_event.name() == "test2"
assert parsed_event.group() == "default"
assert parsed_event.num_arguments() == 5

print("message={}".format(parsed_event.message()))
print("description={}".format(parsed_event.description()))
assert parsed_event.message() == "Arguments: -3 -9238 832223 -9277377287318721 3423423.25"
assert parsed_event.description() == "test\n<{}PARAM#www.test.com<link=www.test.com>#"
print("")


a = 42
b = 321.3134554
c = 9182
d = 663281
e = 2834873473267162
args = create_args([(a, 'uint8_t'), (b, 'float'), (c, 'uint16_t'),
                    (d, 'uint32_t'), (e, 'uint64_t'), ])
event_id = 2307042 | (1 << 24)
parsed_event = p.parse(event_id, args)

assert parsed_event
assert parsed_event.event_id() == event_id
assert parsed_event.name() == "test3"
assert parsed_event.group() == "default"

assert parsed_event.num_arguments() == 5

print("message={}".format(parsed_event.message()))
print("description={}".format(parsed_event.description()))
assert parsed_event.message() == "Arguments: 42 321.3 9182 663281 2834873473267162"
assert parsed_event.description() == "#url<link=www.test.com/a/b/c?x=1>#\nkeep this. P"
print("")


a = (1 << 1) | (1 << 2)
args = create_args([(a, 'uint8_t')])
event_id = 2307043 | (1 << 24)
parsed_event = p.parse(event_id, args)

assert parsed_event
assert parsed_event.event_id() == event_id
assert parsed_event.name() == "test4"

assert parsed_event.num_arguments() == 1

print("message={}".format(parsed_event.message()))
print("description={}".format(parsed_event.description()))
assert parsed_event.message() == "Bitfield value: Bit 2/ Bit 3"
assert parsed_event.description() == ""
print("")


a = 16.423
b = 9472.3245894327
c = -54
d = 4613686018427387904
args = create_args([(a, 'float'), (b, 'float'), (c, 'int8_t'), (d, 'uint64_t')])
event_id = 2307044 | (1 << 24)
parsed_event = p.parse(event_id, args)

assert parsed_event
assert parsed_event.event_id() == event_id
assert parsed_event.name() == "test5"

assert parsed_event.num_arguments() == 4

print("message={}".format(parsed_event.message()))
print("description={}".format(parsed_event.description()))
assert parsed_event.message() == "Testing units: 16.42300033569336 m"
assert parsed_event.description() == """16.423 m 16.42300033569336 C
9472.324219 m^2
-54 m
4613686018427387904 m/s"""
print("")


a = 1
b = 4611686018427387904
args = create_args([(a, 'uint64_t'), (b, 'uint64_t')])
event_id = 1531103 | (1 << 24)
parsed_event = p.parse(event_id, args)

assert parsed_event
assert parsed_event.event_id() == event_id
assert parsed_event.name() == "test_enum"
assert parsed_event.group() == "custom_group"

assert parsed_event.num_arguments() == 2

print("message={}".format(parsed_event.message()))
print("description={}".format(parsed_event.description()))
assert parsed_event.message() == "Event using enums. Enum values: One value, Large value"
assert parsed_event.description() == ""
print("")


