#include "param.h"
#include "parameters_autogen.hpp"

static param_curr_t __param_table__[nextpilot::params::MAX_COUNT] = {};

static constexpr bool param_in_range(uint16_t idx) {
    return idx < nextpilot::params::MAX_COUNT;
}

uint16_t param_get_count() {
    return (uint16_t)nextpilot::params::MAX_COUNT;
}

uint8_t param_get_type(uint16_t idx) {
    if (param_in_range(idx){
        return nextpilot::parameters[idx].type
    }
    return 0;
}

int param_find(const char *name) {
}

int param_get(uint16_t idx, void *val) {
    if (!val || !param_in_range(idx)) {
        LOG_E("param idx %d out range %d", idx, param_get_count());
        return -1;
    }

    rt_enter_critical();
    if (__param_table__[idx].status.changed) {
        memcpy(val, &__param_table__[idx].value, 4)
    } else {
        memcpy(val, &nextpilot::parameters[idx].value, 4)
    }
    rt_exit_critical();

    return 0;
}
