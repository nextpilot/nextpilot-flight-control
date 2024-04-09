#include "health_and_arming_checks.h"

#ifndef LIBEVENTS_DEBUG_PRINTF
#define LIBEVENTS_DEBUG_PRINTF(...)
#endif

namespace events
{

bool HealthAndArmingChecks::handleEvent(const events::parser::ParsedEvent& event)
{
    Type type;
    if (event.group() == "arming_check" && event.type() == "summary") {
        type = Type::ArmingCheckSummary;
    } else if (event.group() == "health" && event.type() == "summary") {
        type = Type::HealthSummary;
    } else if ((event.group() == "arming_check" || event.group() == "health") && event.type() == "") {
        type = Type::Other;
    } else {
        // not interested in this event
        return false;
    }

    // the expected order of receiving is:
    // - ArmingCheckSummary
    // - N Other
    // - HealthSummary

    if (type != _expectedEvent) {
        if (_expectedEvent == Type::Other && type == Type::HealthSummary) {
            // all good
        } else if (type == Type::ArmingCheckSummary) {
            LIBEVENTS_DEBUG_PRINTF("Unexpected ArmingCheckSummary event, resetting. Expected: %i", (int)_expectedEvent);
            // accept & reset
        } else {
            LIBEVENTS_DEBUG_PRINTF("Unexpected event, resetting. Expected: %i, got %i", (int)_expectedEvent, (int)type);
            _expectedEvent = Type::ArmingCheckSummary;
            return false;
        }
    }

    bool ret = false;
    switch (type) {
        case Type::ArmingCheckSummary:
            reset();
            if (event.numArguments() >= 5 && event.argument(1).isEnum() && event.argument(3).isEnum()) {
                _current_chunk = event.argumentValueInt(0);
                _chunks[_current_chunk] = {};
                parser::EnumDefinition* health_enum = event.argument(1).enum_def;
                uint64_t error = event.argumentValueInt(1);
                uint64_t warning = event.argumentValueInt(2);
                parser::EnumDefinition* mode_group_enum = event.argument(3).enum_def;
                uint64_t can_arm = event.argumentValueInt(3);
                uint64_t can_run = event.argumentValueInt(4);

                Results& current_results = _chunks[_current_chunk];
                for (auto entry_iter : mode_group_enum->entries) {
                    unsigned index = log2(entry_iter.first);
                    if (index >= current_results._mode_groups.groups.size()) {
                        // The groups might have gaps, ensure to fit the largest index
                        current_results._mode_groups.groups.resize(index + 1);
                    }
                    ModeGroup mode_group{};
                    mode_group.name = entry_iter.second.name;
                    mode_group.can_arm = can_arm & entry_iter.first;
                    mode_group.can_run = can_run & entry_iter.first;
                    current_results._mode_groups.groups[index] = mode_group;
                }
                for (auto entry_iter : health_enum->entries) {
                    HealthComponent health_component{};
                    health_component.name = entry_iter.second.name;
                    health_component.label = entry_iter.second.description;
                    health_component.bitmask = entry_iter.first;
                    health_component.arming_check.error = error & health_component.bitmask;
                    health_component.arming_check.warning = warning & health_component.bitmask;
                    current_results._health_components.health_components.emplace(health_component.name,
                                                                                 health_component);
                }
                _expectedEvent = Type::Other;
            }
            break;
        case Type::Other: {
            Check check{};
            check.type = event.group() == "health" ? CheckType::Health : CheckType::ArmingCheck;
            check.message = event.message();
            check.description = event.description();
            check.affected_mode_groups = event.argumentValueInt(0);
            check.affected_health_component_index = event.argumentValueInt(1);
            check.log_levels = event.eventData().log_levels;
            _chunks[_current_chunk]._checks.push_back(check);
        } break;
        case Type::HealthSummary:
            int chunk_id = event.argumentValueInt(0);
            if (event.numArguments() >= 4 && _current_chunk == chunk_id) {
                uint64_t is_present = event.argumentValueInt(1);
                uint64_t error = event.argumentValueInt(2);
                uint64_t warning = event.argumentValueInt(3);

                Results& current_results = _chunks[_current_chunk];
                for (auto& entry_iter : current_results._health_components.health_components) {
                    entry_iter.second.health.is_present = is_present & entry_iter.second.bitmask;
                    entry_iter.second.health.error = error & entry_iter.second.bitmask;
                    entry_iter.second.health.warning = warning & entry_iter.second.bitmask;
                }

                updateResultsFromChunks();

                ret = true;

            } else {
                LIBEVENTS_DEBUG_PRINTF("Unexpected chunk id, resetting. Expected: %i, got %i", _current_chunk,
                                       chunk_id);
            }
            reset();
            break;
    }
    return ret;
}

void HealthAndArmingChecks::updateResultsFromChunks()
{
    _results = {};
    // mode groups must be the same among different chunks, but health components might differ
    for (auto chunk_iter : _chunks) {
        // mode groups
        const Results& chunk = chunk_iter.second;
        if (_results._mode_groups.groups.size() == 0) {
            _results._mode_groups.groups = chunk._mode_groups.groups;
        } else if (_results._mode_groups.groups.size() == chunk._mode_groups.groups.size()) {
            for (size_t mode_group_idx = 0; mode_group_idx < chunk._mode_groups.groups.size(); ++mode_group_idx) {
                const ModeGroup& mode_group = chunk._mode_groups.groups[mode_group_idx];
                // combine with AND
                _results._mode_groups.groups[mode_group_idx].can_arm &= mode_group.can_arm;
                _results._mode_groups.groups[mode_group_idx].can_run &= mode_group.can_run;
            }
        } else {
            LIBEVENTS_DEBUG_PRINTF("Unexpected different number of modes. Expected: %i, got %i",
                                   _results._mode_groups.groups.size(), chunk._mode_groups.groups.size());
        }

        // health
        if (_results._health_components.health_components.size() == 0) {
            _results._health_components.health_components = chunk._health_components.health_components;
        } else {
            auto& health_components = _results._health_components.health_components;
            for (auto health_component_iter : chunk._health_components.health_components) {
                if (health_components.find(health_component_iter.first) == health_components.end()) {
                    health_components.insert(std::make_pair(health_component_iter.first, health_component_iter.second));
                } else {
                    // combine, use OR (assuming it's only set in one, or equal in both cases), bitmask becomes invalid
                    auto& health_component = health_components[health_component_iter.first];
                    health_component.arming_check.warning |= health_component_iter.second.arming_check.warning;
                    health_component.arming_check.error |= health_component_iter.second.arming_check.error;
                    health_component.health.is_present |= health_component_iter.second.health.is_present;
                    health_component.health.warning |= health_component_iter.second.health.warning;
                    health_component.health.error |= health_component_iter.second.health.error;
                    health_component.bitmask = 0;
                }
            }
        }

        // checks
        for (const auto& check : chunk._checks) {
            _results._checks.push_back(check);
            if (check.affected_health_component_index != 0) {  // 0 == none
                for (auto health_component_iter : chunk._health_components.health_components) {
                    if (health_component_iter.second.bitmask == (1ull << check.affected_health_component_index)) {
                        _results._checks.back().health_component = &health_component_iter.second;
                        break;
                    }
                }
            }
        }
    }
}

void HealthAndArmingChecks::reset()
{
    _expectedEvent = Type::ArmingCheckSummary;
}

bool HealthAndArmingChecks::Results::canArm(int mode_group_index) const
{
    if (mode_group_index >= (int)_mode_groups.groups.size()) {
        return false;
    }
    return _mode_groups.groups[mode_group_index].can_arm;
}

bool HealthAndArmingChecks::Results::canRun(int mode_group_index) const
{
    if (mode_group_index >= (int)_mode_groups.groups.size()) {
        return false;
    }
    return _mode_groups.groups[mode_group_index].can_run;
}

std::vector<HealthAndArmingChecks::Check> HealthAndArmingChecks::Results::checks(int mode_group_index) const
{
    std::vector<HealthAndArmingChecks::Check> ret;
    for (const auto& check : _checks) {
        if (check.affected_mode_groups & (1ull << mode_group_index)) {
            ret.push_back(check);
        }
    }

    return ret;
}

} /* namespace events */
