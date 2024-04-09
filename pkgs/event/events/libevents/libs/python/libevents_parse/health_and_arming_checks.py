""" python events health and arming checks protocol libary """
import math

from .parser import ParsedEvent

# pylint: disable=too-few-public-methods, protected-access


class HealthAndArmingChecks:
    """ health and arming checks protocol parser """

    class Health:
        """ Health bits for a single component """
        def __init__(self):
            self.is_present: bool = False
            self.error: bool = False
            self.warning: bool = False

    class ArmingCheck:
        """ Arming check bits for a single component """
        def __init__(self):
            self.error: bool = False
            self.warning: bool = False

    class HealthComponent:
        """ Flags for a single component """
        def __init__(self):
            self.label: str = ''
            self.name: str = ''
            self.description: str = ''
            self.bitmask: int = 0
            self.health: HealthAndArmingChecks.Health = HealthAndArmingChecks.Health()
            self.arming_check: HealthAndArmingChecks.ArmingCheck =\
                HealthAndArmingChecks.ArmingCheck()

    class ModeGroup:
        """ Flags for a mode group """
        def __init__(self):
            self.name: str = ''
            self.can_arm: bool = False
            self.can_run: bool = False

    class Check:
        """ single check (associated with an event) """
        def __init__(self):
            self.message: str = ''
            self.description: str = ''
            self.affected_mode_groups: int = 0
            self.affected_health_component_index: int = 0
            self.health_component: HealthAndArmingChecks.HealthComponent = None
            self.event: ParsedEvent = None

    class Results:
        """ Contains all results """
        def __init__(self):
            self._mode_groups: list[HealthAndArmingChecks.ModeGroup] = []
            self._health_components: dict[str, HealthAndArmingChecks.HealthComponent] = {}
            self._checks: list[HealthAndArmingChecks.Check] = []

        def can_arm(self, mode_group_index: int):
            """ Whether arming is possible for a mode group """
            if mode_group_index >= len(self._mode_groups):
                return False
            return self._mode_groups[mode_group_index].can_arm

        def can_run(self, mode_group_index: int):
            """ Whether switching to a mode is possible for a mode group (relevant while armed) """
            if mode_group_index >= len(self._mode_groups):
                return False
            return self._mode_groups[mode_group_index].can_run

        @property
        def checks(self):
            """ list of Checks """
            return self._checks

        @property
        def health_components(self):
            """ dict of HealthComponents """
            return self._health_components


    def __init__(self, debug: bool):
        self._debug = debug

        self._expected_event = "arming_check"
        self._current_chunk: int = 0
        self._chunks: dict[int, HealthAndArmingChecks.Results] = {}
        self._results: HealthAndArmingChecks.Results = None

    def _debug_print(self, msg: str):
        """ debug output """
        if self._debug:
            print(msg)

    @property
    def results(self):
        """ Results """
        return self._results

    def handle_event(self, event: ParsedEvent):
        """ handle new incoming event
            :return: tuple[bool has_update, list[new events], list[removed events]]
         """
        ret = False, [], []
        if event.group() == 'arming_check' and event.type() == 'summary':
            event_type = 'arming_check'
        elif event.group() == 'health' and event.type() == 'summary':
            event_type = 'health'
        elif (event.group() == 'arming_check' or event.group() == 'health') and event.type() == '':
            event_type = 'other'
        else:
            # not interested in this event
            return ret

        # the expected order of receiving is:
        # - arming_check summary
        # - N Other
        # - health summary

        if event_type != self._expected_event:
            if self._expected_event == 'other' and event_type == 'health':
                # all good
                pass
            elif event_type == 'arming_check':
                # accept and reset
                self._debug_print('Unexpected arming_check summary, resetting. Expected: ' +
                                  self._expected_event)
            else:
                self._expected_event = 'arming_check'
                self._debug_print('Unexpected event, resetting. Expected: '+self._expected_event +
                                  ', got: ' + event_type)
                return ret

        if event_type == 'arming_check':
            self._reset()
            if event.num_arguments() >= 5 and event.argument_enum(1) is not None and \
                    event.argument_enum(3) is not None:
                self._current_chunk = event.argument_value(0)
                self._chunks[self._current_chunk] = self.Results()
                health_enum = event.argument_enum(1)
                error = event.argument_value(1)
                warning = event.argument_value(2)
                mode_group_enum = event.argument_enum(3)
                can_arm = event.argument_value(3)
                can_run = event.argument_value(4)

                current_results = self._chunks[self._current_chunk]
                for entry_key in mode_group_enum['entries']:
                    entry_key_int = int(entry_key)
                    index = int(math.log2(entry_key_int))
                    entry = mode_group_enum['entries'][entry_key]
                    while index >= len(current_results._mode_groups):
                        # The groups might have gaps, ensure to fit the largest index
                        current_results._mode_groups.append(HealthAndArmingChecks.ModeGroup())
                    mode_group = HealthAndArmingChecks.ModeGroup()
                    mode_group.name = entry['name']
                    mode_group.can_arm = (can_arm & entry_key_int) != 0
                    mode_group.can_run = (can_run & entry_key_int) != 0
                    current_results._mode_groups[index] = mode_group

                for entry_key in health_enum['entries']:
                    entry_key_int = int(entry_key)
                    entry = health_enum['entries'][entry_key]
                    health_component = HealthAndArmingChecks.HealthComponent()
                    health_component.name = entry['name']
                    health_component.description = entry.get('description', '')
                    health_component.bitmask = entry_key_int
                    health_component.arming_check.error = (error & health_component.bitmask) != 0
                    health_component.arming_check.warning = \
                        (warning & health_component.bitmask) != 0
                    current_results._health_components[health_component.name] = health_component
                self._expected_event = 'other'

        elif event_type == 'other':
            check = HealthAndArmingChecks.Check()
            check.message = event.message()
            check.description = event.description()
            check.affected_mode_groups = event.argument_value(0)
            check.affected_health_component_index = event.argument_value(1)
            check.event = event
            self._chunks[self._current_chunk]._checks.append(check)

        elif event_type == 'health':
            chunk_id = event.argument_value(0)
            if event.num_arguments() >= 4 and self._current_chunk == chunk_id:
                is_present = event.argument_value(1)
                error = event.argument_value(2)
                warning = event.argument_value(3)

                current_results = self._chunks[self._current_chunk]
                for entry in current_results._health_components:
                    health_component = current_results._health_components[entry]
                    health_component.health.is_present = \
                        (is_present & health_component.bitmask) != 0
                    health_component.health.error = (error & health_component.bitmask) != 0
                    health_component.health.warning = (warning & health_component.bitmask) != 0
                ret = self._update_results_from_chunks()
            else:
                self._debug_print('Unexpected chunk id, resetting. Expected: {:}, got {:}'.format(
                    self._current_chunk, chunk_id))
            self._reset()

        return ret

    def _update_results_from_chunks(self):
        prev_results = self._results
        self._results = HealthAndArmingChecks.Results()

        # mode groups
        for _, chunk in self._chunks.items():
            if len(self._results._mode_groups) == 0:
                self._results._mode_groups = chunk._mode_groups
            elif len(self._results._mode_groups) == len(chunk._mode_groups):
                for mode_group_idx, mode_group in enumerate(chunk._mode_groups):
                    # combine with AND
                    self._results._mode_groups[mode_group_idx].can_arm &= \
                        mode_group.can_arm
                    self._results._mode_groups[mode_group_idx].can_run &= \
                        mode_group.can_run
            else:
                self._debug_print('unexpected different number of modes. Expected: {:}, got {:}'
                                  .format(len(self._results._mode_groups), len(chunk._mode_groups)))

            # health
            if len(self._results._health_components) == 0:
                self._results._health_components = chunk._health_components
            else:
                health_components = self._results._health_components
                for health_component_key in chunk._health_components:
                    if health_component_key in health_components:
                        # combine, use OR (assuming it's only set in one, or equal in both cases),
                        # bitmask becomes invalid
                        health_component = health_components[health_component_key]
                        chunk_health_component = chunk._health_components[health_component_key]
                        health_component.arming_check.warning |= \
                            chunk_health_component.arming_check.warning
                        health_component.arming_check.error |= \
                            chunk_health_component.arming_check.error
                        health_component.health.is_present |= \
                            chunk_health_component.health.is_present
                        health_component.health.warning |= chunk_health_component.health.warning
                        health_component.health.error |= chunk_health_component.health.error
                        health_component.bitmask = 0
                    else:
                        health_components[health_component_key] = \
                            chunk._health_components[health_component_key]

            # checks
            for check in chunk._checks:
                self._results._checks.append(check)
                if check.affected_health_component_index != 0: # 0 == none
                    for health_component_key in chunk._health_components:
                        if chunk._health_components[health_component_key].bitmask == \
                                (1 << check.affected_health_component_index):
                            self._results._checks[-1].health_component = \
                                chunk._health_components[health_component_key]
                            break

        # get the diff to the previous results
        added_events = []
        removed_events = []
        if prev_results is not None:
            for check in prev_results._checks:
                found = False
                for new_check in self._results._checks:
                    if self._compare_events(check.event, new_check.event):
                        found = True
                        break
                if not found:
                    removed_events.append(check.event)
        for check in self._results._checks:
            found = False
            if prev_results is not None:
                for prev_check in prev_results._checks:
                    if self._compare_events(check.event, prev_check.event):
                        found = True
                        break
            if not found:
                added_events.append(check.event)

        self._debug_print('Got update, added events: {:}, removed events: {:}'
                          .format(len(added_events), len(removed_events)))
        return True, added_events, removed_events

    @staticmethod
    def _compare_events(event_a: ParsedEvent, event_b: ParsedEvent):
        if event_a.event_id() != event_b.event_id() or \
                event_a.num_arguments() != event_b.num_arguments():
            return False

        for i in range(event_a.num_arguments()):
            if event_a.argument_value(i) != event_b.argument_value(i):
                return False
        return True

    def _reset(self):
        self._expected_event = 'arming_check'

