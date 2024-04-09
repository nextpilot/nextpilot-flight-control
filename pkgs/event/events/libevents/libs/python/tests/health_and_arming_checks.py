#! /usr/bin/env python3

""" run health and arming check tests """

import math
import os
import sys

from helper import create_args

#pylint: disable=wrong-import-position
sys.path.append(os.path.join(os.path.dirname(os.path.realpath(__file__)), '..'))
from libevents_parse.parser import Parser
from libevents_parse.health_and_arming_checks import HealthAndArmingChecks

#pylint: disable=invalid-name


cur_dir = os.path.dirname(os.path.realpath(__file__))
json_file = os.path.join(cur_dir, "../../test.json")

p = Parser()
p.load_definition_file(json_file)
p.set_profile('dev')

health_and_arming_checks = HealthAndArmingChecks(debug=True)

# this matches json definition
absolute_pressure_mask = 2
differential_pressure_mask = 4
gps_mask = 8
manual_mask = 1
pos_ctrl_mask = 4
mission_mask = 8
rtl_mask = 32


print('No Events')
chunk_id = 0
error = 0
warning = gps_mask
can_arm = pos_ctrl_mask | rtl_mask
can_run = 0
args = create_args([(chunk_id, 'int8_t'), (error, 'uint32_t'), (warning, 'uint32_t'),
                    (can_arm, 'uint32_t'), (can_run, 'uint32_t')])
event_id = 11047904 | (0 << 24) # arming check summary
parsed_event = p.parse(event_id, args)
assert parsed_event
arming_check_summary_event = parsed_event
updated, _, _ = health_and_arming_checks.handle_event(parsed_event)
assert not updated

is_present = gps_mask | absolute_pressure_mask
error = 0
warning = absolute_pressure_mask
args = create_args([(chunk_id, 'int8_t'), (is_present, 'uint32_t'), (error, 'uint32_t'),
                    (warning, 'uint32_t')])
event_id = 1914663 | (0 << 24) # health summary
parsed_event = p.parse(event_id, args)
assert parsed_event
health_summary_event = parsed_event
updated, _, _ = health_and_arming_checks.handle_event(parsed_event)
assert updated

assert health_and_arming_checks.results is not None
assert len(health_and_arming_checks.results.checks) == 0
assert health_and_arming_checks.results.health_components['gps'].name == 'gps'
assert not health_and_arming_checks.results.health_components['gps'].arming_check.error
assert health_and_arming_checks.results.health_components['gps'].arming_check.warning
assert not health_and_arming_checks.results.health_components['gps'].health.error
assert not health_and_arming_checks.results.health_components['gps'].health.warning
assert health_and_arming_checks.results.health_components['gps'].health.is_present
assert not health_and_arming_checks.results.health_components['absolute_pressure'].health.error
assert health_and_arming_checks.results.health_components['absolute_pressure'].health.warning
assert health_and_arming_checks.results.health_components['absolute_pressure'].health.is_present
assert not health_and_arming_checks.results.can_arm(int(math.log2(manual_mask)))
assert health_and_arming_checks.results.can_arm(int(math.log2(pos_ctrl_mask)))
assert not health_and_arming_checks.results.can_arm(int(math.log2(mission_mask)))
assert health_and_arming_checks.results.can_arm(int(math.log2(rtl_mask)))
print("")


print("Some Events")
updated, _, _ = health_and_arming_checks.handle_event(arming_check_summary_event)
assert not updated

modes = pos_ctrl_mask
health_component_index = int(math.log2(differential_pressure_mask))
arg0 = 4.324
arg1 = 7374.43
args = create_args([(modes, 'uint32_t'), (health_component_index, 'uint8_t'), (arg0, 'float'),
                    (arg1, 'float')])
event_id = 1802995 | (0 << 24) # check_airspeed_too_high
event_id_airspeed = event_id
parsed_event = p.parse(event_id, args)
assert parsed_event
diff_pressure_event = parsed_event
updated, _, _ = health_and_arming_checks.handle_event(parsed_event)
assert not updated

health_component_index = 0
arg0_mag0 = 0
args = create_args([(modes, 'uint32_t'), (health_component_index, 'uint8_t'),
                    (arg0_mag0, 'uint8_t')])
event_id = 16716537 | (0 << 24) # check_mag_not_calibrated
event_id_mag = event_id
parsed_event = p.parse(event_id, args)
assert parsed_event
mag0_event = parsed_event
updated, _, _ = health_and_arming_checks.handle_event(parsed_event)
assert not updated

updated, added_events, removed_events = health_and_arming_checks.handle_event(health_summary_event)
assert updated
assert len(removed_events) == 0
assert len(added_events) == 2
assert added_events[0].event_id() == event_id_airspeed
assert math.fabs(added_events[0].argument_value(2) - arg0) < 0.0001
assert math.fabs(added_events[0].argument_value(3) - arg1) < 0.001
assert added_events[1].event_id() == event_id_mag
assert added_events[1].argument_value(2) == arg0_mag0
assert len(health_and_arming_checks.results.checks) == 2
print("")


print("Adding an event with the same event id")
updated, _, _ = health_and_arming_checks.handle_event(arming_check_summary_event)
assert not updated

updated, _, _ = health_and_arming_checks.handle_event(diff_pressure_event)
assert not updated

updated, _, _ = health_and_arming_checks.handle_event(mag0_event)
assert not updated

health_component_index = 0
arg0_mag1 = 1
args = create_args([(modes, 'uint32_t'), (health_component_index, 'uint8_t'),
                    (arg0_mag1, 'uint8_t')])
event_id = 16716537 | (0 << 24) # check_mag_not_calibrated
event_id_mag = event_id
parsed_event = p.parse(event_id, args)
assert parsed_event
mag1_event = parsed_event
updated, _, _ = health_and_arming_checks.handle_event(parsed_event)
assert not updated

updated, added_events, removed_events = health_and_arming_checks.handle_event(health_summary_event)
assert updated
assert len(removed_events) == 0
assert len(added_events) == 1
assert added_events[0].event_id() == event_id_mag
assert added_events[0].argument_value(2) == arg0_mag1
assert len(health_and_arming_checks.results.checks) == 3
print("")

print("Removing events")
updated, _, _ = health_and_arming_checks.handle_event(arming_check_summary_event)
assert not updated

updated, _, _ = health_and_arming_checks.handle_event(mag0_event)
assert not updated

updated, added_events, removed_events = health_and_arming_checks.handle_event(health_summary_event)
assert updated
assert len(removed_events) == 2
assert len(added_events) == 0
assert removed_events[0].event_id() == event_id_airspeed
assert removed_events[1].event_id() == event_id_mag
assert removed_events[1].argument_value(2) == arg0_mag1
assert len(health_and_arming_checks.results.checks) == 1
print("")


print("Adding chunk")
chunk_id = 1
error = 0
warning = differential_pressure_mask
can_arm = pos_ctrl_mask
can_run = pos_ctrl_mask
args = create_args([(chunk_id, 'int8_t'), (error, 'uint32_t'), (warning, 'uint32_t'),
                    (can_arm, 'uint32_t'), (can_run, 'uint32_t')])
event_id = 11047904 | (0 << 24) # arming check summary
parsed_event = p.parse(event_id, args)
assert parsed_event
updated, _, _ = health_and_arming_checks.handle_event(parsed_event)
assert not updated

health_component_index = 0
args = create_args([(modes, 'uint32_t'), (health_component_index, 'uint8_t')])
event_id = 10011251 | (0 << 24) # check_modes_global_pos
event_id_global_pos = event_id
parsed_event = p.parse(event_id, args)
assert parsed_event
updated, _, _ = health_and_arming_checks.handle_event(parsed_event)
assert not updated

is_present = gps_mask | absolute_pressure_mask
error = 0
warning = absolute_pressure_mask
args = create_args([(chunk_id, 'int8_t'), (is_present, 'uint32_t'), (error, 'uint32_t'),
                    (warning, 'uint32_t')])
event_id = 1914663 | (0 << 24) # health summary
parsed_event = p.parse(event_id, args)
assert parsed_event
updated, added_events, removed_events = health_and_arming_checks.handle_event(parsed_event)
assert updated

assert health_and_arming_checks.results.health_components['gps'].name == 'gps'
assert not health_and_arming_checks.results.health_components['gps'].arming_check.error
assert health_and_arming_checks.results.health_components['gps'].arming_check.warning
assert not health_and_arming_checks.results.health_components['gps'].health.error
assert not health_and_arming_checks.results.health_components['gps'].health.warning
assert health_and_arming_checks.results.health_components['gps'].health.is_present
assert not health_and_arming_checks.results.health_components['absolute_pressure'].health.error
assert health_and_arming_checks.results.health_components['absolute_pressure'].health.warning
assert health_and_arming_checks.results.health_components['absolute_pressure'].health.is_present
assert health_and_arming_checks.results.health_components['differential_pressure']\
    .arming_check.warning
assert not health_and_arming_checks.results.can_arm(int(math.log2(manual_mask)))
assert health_and_arming_checks.results.can_arm(int(math.log2(pos_ctrl_mask)))
assert not health_and_arming_checks.results.can_arm(int(math.log2(mission_mask)))
assert not health_and_arming_checks.results.can_arm(int(math.log2(rtl_mask)))

assert len(removed_events) == 0
assert len(added_events) == 1
assert added_events[0].event_id() == event_id_global_pos
assert len(health_and_arming_checks.results.checks) == 2

print("")
