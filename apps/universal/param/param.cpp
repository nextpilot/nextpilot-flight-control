/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#define DBG_ENABLE
#define DBG_LEVEL DBG_LOG

#include "param.h"
#include "param_global_autogen.h"
#include "rtthread.h"
#include "rtdbg.h"

uint16_t param_get_count() {
    uint16_t count = 0;
#ifdef PARAM_USING_GLOBAL_AUTOGEN
    count += nextpilot::global_params::param_get_count();
#endif // PARAM_USING_GLOBAL_AUTOGEN

#ifdef PARAM_USING_LINKER_SECTION
    count += section_param_get_count();
#endif // PARAM_USING_LINKER_SECTION

#ifdef PARAM_USING_SIMULINK_CAPI
    count += simulink_param_get_count();
#endif // PARAM_USING_SIMULINK_CAPI

    return count;
}

bool param_in_range(param_t idx) {
    return idx < param_get_count();
}

param_interface_t param_get_whois(param_t *idx) {
    uint16_t offset = 0;
#ifdef PARAM_USING_GLOBAL_AUTOGEN
    if ((*idx) < offset + nextpilot::global_params::param_get_count()) {
        (*idx) -= offset;
        return PARAM_INTERFACE_AUTOGEN;
    }
    offset += nextpilot::global_params::param_get_count();
#endif // PARAM_USING_GLOBAL_AUTOGEN

#ifdef PARAM_USING_LINKER_SECTION
    if ((*idx) < offset + section_param_get_count()) {
        (*idx) -= offset;
        return PARAM_INTERFACE_SECTION;
    }
    offset += section_param_get_count();
#endif // PARAM_USING_LINKER_SECTION

#ifdef PARAM_USING_SIMULINK_CAPI
    if ((*idx) < offset + simulink_param_get_count()) {
        (*idx) -= offset;
        return PARAM_INTERFACE_SIMULINK;
    }
#endif // PARAM_USING_SIMULINK_CAPI

    return PARAM_INTERFACE_UNKONWN;
}

int param_find_internal(const char *name, bool mark_used) {
    param_t idx    = PARAM_INVALID;
    int     offset = 0;

#ifdef PARAM_USING_GLOBAL_AUTOGEN
    idx = nextpilot::global_params::param_find(name, mark_used);
    if (idx != PARAM_INVALID) {
        return idx;
    }
    offset += nextpilot::global_params::param_get_count();
#endif // PARAM_USING_GLOBAL_AUTOGEN

#ifdef PARAM_USING_LINKER_SECTION
    idx = section_param_find_internal(name, mark_used);
    if (idx != PARAM_INVALID) {
        return idx + offset;
    }
    offset += section_param_get_count();
#endif // PARAM_USING_LINKER_SECTION

#ifdef PARAM_USING_SIMULINK_CAPI
    idx = simulink_param_find_internal(name, mark_used);
    if (idx != PARAM_INVALID) {
        return idx + offset;
    }
#endif // PARAM_USING_SIMULINK_CAPI

    return PARAM_INVALID;
}

// int param_find(const char *name) {
//     return param_find_internal(name, true);
// }

const char *param_get_name(param_t idx) {
    switch (param_get_whois(&idx)) {
#ifdef PARAM_USING_GLOBAL_AUTOGEN
    case PARAM_INTERFACE_AUTOGEN:
        return nextpilot::global_params::param_get_name(idx);
#endif // PARAM_USING_GLOBAL_AUTOGEN
#ifdef PARAM_USING_LINKER_SECTION
    case PARAM_INTERFACE_SECTION:
        return section_param_get_name(idx);
#endif // PARAM_USING_LINKER_SECTION
#ifdef PARAM_USING_SIMULINK_CAPI
    case case PARAM_INTERFACE_SIMULINK:
        return simulink_param_get_name(idx);
#endif // PARAM_USING_SIMULINK_CAPI
    default:
        return nullptr;
    }
}

param_type_t param_get_type(param_t idx) {
    switch (param_get_whois(&idx)) {
#ifdef PARAM_USING_GLOBAL_AUTOGEN
    case PARAM_INTERFACE_AUTOGEN:
        return nextpilot::global_params::param_get_type(idx);
#endif // PARAM_USING_GLOBAL_AUTOGEN
#ifdef PARAM_USING_LINKER_SECTION
    case PARAM_INTERFACE_SECTION:
        return section_param_get_type(idx);
#endif // PARAM_USING_LINKER_SECTION
#ifdef PARAM_USING_SIMULINK_CAPI
    case PARAM_INTERFACE_SIMULINK:
        return simulink_param_get_type(idx);
#endif // PARAM_USING_SIMULINK_CAPI
    default:
        return PARAM_TYPE_UNKNOWN;
    }
}

const char *param_get_type_cstr(param_t idx) {
    return param_type_cstr(param_get_type(idx));
}

int param_get_default_value(param_t idx, void *val) {
    switch (param_get_whois(&idx)) {
#ifdef PARAM_USING_GLOBAL_AUTOGEN
    case PARAM_INTERFACE_AUTOGEN:
        return nextpilot::global_params::param_get_default_value(idx, val);
#endif // PARAM_USING_GLOBAL_AUTOGEN
#ifdef PARAM_USING_LINKER_SECTION
    case PARAM_INTERFACE_SECTION:
        return section_param_get_type(idx);
#endif // PARAM_USING_LINKER_SECTION
#ifdef PARAM_USING_SIMULINK_CAPI
    case PARAM_INTERFACE_SIMULINK:
        return simulink_param_get_type(idx);
#endif // PARAM_USING_SIMULINK_CAPI
    default:
        return PARAM_TYPE_UNKNOWN;
    }
}

param_flag_t param_get_flag(param_t idx) {
    param_flag_t flag = {.value = 0};
    switch (param_get_whois(&idx)) {
#ifdef PARAM_USING_GLOBAL_AUTOGEN
    case PARAM_INTERFACE_AUTOGEN:
        return nextpilot::global_params::param_get_flag(idx);
#endif // PARAM_USING_GLOBAL_AUTOGEN
#ifdef PARAM_USING_LINKER_SECTION
    case PARAM_INTERFACE_SECTION:
        return section_param_get_flag(idx);
#endif // PARAM_USING_LINKER_SECTION
#ifdef PARAM_USING_SIMULINK_CAPI
    case case PARAM_INTERFACE_SIMULINK:
        return simulink_param_get_flag(idx);
#endif // PARAM_USING_SIMULINK_CAPI
    default:
        return flag;
    }
}

int param_set_internal(param_t idx, const void *val, bool mark_saved, bool notify_changes) {
    switch (param_get_whois(&idx)) {
#ifdef PARAM_USING_GLOBAL_AUTOGEN
    case PARAM_INTERFACE_AUTOGEN:
        return nextpilot::global_params::param_set_value(idx, val, mark_saved, notify_changes);
#endif // PARAM_USING_GLOBAL_AUTOGEN
#ifdef PARAM_USING_LINKER_SECTION
    case PARAM_INTERFACE_SECTION:
        return section_param_set_value(idx, mark_saved, notify_changes);
#endif // PARAM_USING_LINKER_SECTION
#ifdef PARAM_USING_SIMULINK_CAPI
    case case PARAM_INTERFACE_SIMULINK:
        return simulink_param_set_value(idx, mark_saved, notify_changes);
#endif // PARAM_USING_SIMULINK_CAPI
    default:
        return -1;
    }
}

int param_get_internal(param_t idx, void *val, bool mark_used) {
    // perf_count(param_get_perf);

    if (!val) {
        return -1;
    }

    switch (param_get_whois(&idx)) {
#ifdef PARAM_USING_GLOBAL_AUTOGEN
    case PARAM_INTERFACE_AUTOGEN:
        return nextpilot::global_params::param_get_value(idx, val, mark_used);
#endif // PARAM_USING_GLOBAL_AUTOGEN
#ifdef PARAM_USING_LINKER_SECTION
    case PARAM_INTERFACE_SECTION:
        return section_param_get_value(idx, val);
#endif // PARAM_USING_LINKER_SECTION
#ifdef PARAM_USING_SIMULINK_CAPI
    case case PARAM_INTERFACE_SIMULINK:
        return simulink_param_get_value(idx, val);
#endif // PARAM_USING_SIMULINK_CAPI
    default:
        return -1;
    }
}

int param_get(param_t idx, void *val) {
    return param_get_internal(idx, val, true);
}

param_status_t param_get_status(param_t idx) {
    param_status_t status = {.value = 10};
    return status;
}

int param_reset_internal(param_t idx, bool mark_saved, bool notify) {
    //     switch (param_get_whois(&idx)) {
    // #ifdef PARAM_USING_GLOBAL_AUTOGEN
    //     case PARAM_INTERFACE_AUTOGEN:
    //         return autogen_param_reset_internal(idx, mark_saved, notify);
    // #endif // PARAM_USING_GLOBAL_AUTOGEN
    // #ifdef PARAM_USING_LINKER_SECTION
    //     case PARAM_INTERFACE_SECTION:
    //         return section_param_reset_internal(idx, mark_saved, notify);
    // #endif // PARAM_USING_LINKER_SECTION
    // #ifdef PARAM_USING_SIMULINK_CAPI
    //     case case PARAM_INTERFACE_SIMULINK:
    //         return simulink_param_reset_internal(idx, mark_saved, notify);
    // #endif // PARAM_USING_SIMULINK_CAPI
    //     default:
    //         return -1;
    //     }
    return 0;
}

void param_print(param_t idx, bool show_header) {
    if (!param_in_range(idx)) {
        return;
    }

    if (show_header) {
        LOG_RAW("  flag: BIT2=disarm,  BIT1=restart, BIT0=system\n");
        LOG_RAW("status: BIT2=unsaved, BIT1=changed, BIT0=actived\n");
        LOG_RAW("index    type         name            default       current     flag   status\n");
        LOG_RAW("-----  --------  ----------------  ------------  ------------  ------  ------\n");
    }

    param_value_t def_val;
    param_value_t now_val;

    param_get_default_value(idx, &def_val);
    param_get(idx, &now_val);

    switch (param_get_type(idx)) {
    case PARAM_TYPE_INT32:
        LOG_RAW("%5d  %8s  %16s  %12d  %12d  0x%04x  0x%04x\n", idx,
                param_get_type_cstr(idx),
                param_get_name(idx),
                def_val.i32,
                now_val.i32,
                param_get_flag(idx).value,
                param_get_status(idx).value);
        break;
    case PARAM_TYPE_FLOAT:
        LOG_RAW("%5d  %8s  %16s  %12d  %12d  0x%04x  0x%04x\n", idx,
                param_get_type_cstr(idx),
                param_get_name(idx),
                def_val.f32,
                now_val.f32,
                param_get_flag(idx).value,
                param_get_status(idx).value);
        break;
    default:
        break;
    }
}

int param_test() {
    const char *name = "BB";

    param_t idx = param_find(name);
    rt_kprintf("\n==============\n name = %s,  idx = %d\n", name, idx);

    param_print(2, true);
    return 0;
}
INIT_APP_EXPORT(param_test);
