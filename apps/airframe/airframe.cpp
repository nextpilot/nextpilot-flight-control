/******************************************************************
 *      _   __             __   ____   _  __        __
 *     / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *    /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *   / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 *  /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#define LOG_TAG "airframe"
#define LOG_LVL LOG_LVL_INFO

#include <rtdbg.h>
#include "airframe.h"
#include "stdlib.h"
#include "param_global_autogen.hpp"

// 获取section的首位地址
#if defined(__ARMCC_VERSION) /* ARM C Compiler */
// 如果不在sct文件定义section，则根据代码自动创建
extern const int        NextpilotAirframeTab$$Base;
extern const int        NextpilotAirframeTab$$Limit;
static airframe_info_t *__airframe_section_start_ptr = (airframe_info_t *)&NextpilotAirframeTab$$Base;
static airframe_info_t *__airframe_section_end_ptr   = (airframe_info_t *)&NextpilotAirframeTab$$Limit;
static airframe_info_t *__airframe_table__           = nullptr;
static uint16_t         __airframe_count__           = UINT16_MAX; // NextpilotAirframeTab$$Length
#elif defined(__ICCARM__) || defined(__ICCRX__)                    /* for IAR Compiler */
#   pragma section = "NextpilotAirframeTab"
static airframe_info_t *__airframe_section_start_ptr = (airframe_info_t *)__section_begin("NextpilotAirframeTab");
static airframe_info_t *__airframe_section_end_ptr   = (airframe_info_t *)__section_end("NextpilotAirframeTab");
static airframe_info_t *__airframe_table__           = nullptr;
static uint16_t         __airframe_count__           = UINT16_MAX;
#elif defined(__GNUC__) || defined(__TI_COMPILER_VERSION__) || defined(__TASKING__)
// 请在lds文件中定义好nextpilot_airframe_tab section
extern const int        __nextpilot_airframe_tab_start;
extern const int        __nextpilot_airframe_tab_end;
static airframe_info_t *__airframe_section_start_ptr = (airframe_info_t *)&__nextpilot_airframe_tab_start;
static airframe_info_t *__airframe_section_end_ptr   = (airframe_info_t *)&__nextpilot_airframe_tab_end;
static airframe_info_t *__airframe_table__           = nullptr;
static uint16_t         __airframe_count__           = UINT16_MAX;
#elif defined(__ADSPBLACKFIN__) /* for VisaulDSP++ Compiler*/
// 如何定义section
extern "asm" int        __nextpilot_airframe_tab_start;
extern "asm" int        __nextpilot_airframe_tab_end;
static airframe_info_t *__airframe_section_start_ptr = (airframe_info_t *)&__nextpilot_airframe_tab_start;
static airframe_info_t *__airframe_section_end_ptr   = (airframe_info_t *)&__nextpilot_airframe_tab_end;
static airframe_info_t *__airframe_table__           = nullptr;
static uint16_t         __airframe_count__           = UINT16_MAX;
#elif defined(_MSC_VER)
#   pragma section("NextpilotAirframeTab$a", read)
#   pragma section("NextpilotAirframeTab$p", read)
#   pragma section("NextpilotAirframeTab$z", read)
__declspec(allocate("NextpilotAirframeTab$a")) static airframe_info_t __nextpilot_airframe_tab_start = {
    .name = "airframe_section_start",
};
__declspec(allocate("NextpilotAirframeTab$z")) static airframe_info_t __nextpilot_airframe_tab_end = {
    .name = "airframe_section_end",
};
static airframe_info_t *__airframe_section_start_ptr = &__nextpilot_airframe_tab_start;
static airframe_info_t *__airframe_section_end_ptr   = &__nextpilot_airframe_tab_end;
static airframe_info_t  __airframe_table__[100];
static uint16_t         __airframe_count__ = UINT16_MAX;
#endif

using namespace nextpilot::global_params;

bool airframe_section_init() {
#if defined(_MSC_VER)
    unsigned int *ptr_begin = (unsigned int *)(__airframe_section_start_ptr + 1);
    unsigned int *ptr_end   = (unsigned int *)__airframe_section_end_ptr;
    uint16_t      count     = 0;

    // 由于msv的section是不连续的，因此先将所有的内容拷贝到一个table里面
    // table数组的size是1000
    while (ptr_begin < ptr_end && count <= 1000) {
        if (*ptr_begin != 0) {
            rt_memcpy(__airframe_table__ + count, ptr_begin, sizeof(airframe_info_t));
            count++;
            ptr_begin += sizeof(airframe_info_t) / sizeof(unsigned int);
        } else {
            ptr_begin++;
        }
    }
    __airframe_count__ = count;
#else
    __airframe_table__ = __airframe_section_start_ptr;
    __airframe_count__ = __airframe_section_end_ptr - __airframe_section_start_ptr;
#endif
    return true;
}

static inline uint16_t airframe_get_count() {
    if (__airframe_count__ == UINT16_MAX) {
        airframe_section_init();
    }

    return __airframe_count__;
}

static inline airframe_info_t *airframe_get_table() {
    if (__airframe_count__ == UINT16_MAX) {
        airframe_section_init();
    }

    return __airframe_table__;
}

airframe_info_t *airframe_get_info() {
    int32_t sys_autostart = param_get_int32((param_t)params_id::SYS_AUTOSTART);

    for (int i = 0; i < airframe_get_count(); i++) {
        if (__airframe_table__[i].id == sys_autostart) {
            return &__airframe_table__[i];
        }
    }
    return nullptr;
}

void *airframe_get_mixer(uint8_t *count) {
    airframe_info_t *info = airframe_get_info();

    if (!info) {
        return nullptr;
    }

    if (count) {
        *count = info->mixers.count;
    }

    return info->mixers.point;
}

void *airframe_get_param(uint8_t *count) {
    airframe_info_t *info = airframe_get_info();

    if (!info) {
        return nullptr;
    }

    if (count) {
        *count = info->params.count;
    }

    return info->params.point;
}

void airframe_print_info() {
    const airframe_info_t *info = airframe_get_info();
    if (info) {
        LOG_RAW("airfame: %s(%d)\n", info->name, info->id);
    }
}

static int reset_airframe(int argc, char *argv[]) {
    int32_t sys_autoconfig = 1;
    param_set((param_t)params_id::SYS_AUTOCONFIG, &sys_autoconfig);
    LOG_I("please reboot to rest airframe param");
    return 0;
}

MSH_CMD_EXPORT(reset_airframe, reset airframe param);

static int change_airframe(int argc, char *argv[]) {
    int32_t sys_autostart;
    param_get((param_t)params_id::SYS_AUTOSTART, &sys_autostart);

    if (argc < 2) {
        LOG_I("current airframe = %d", sys_autostart);
        return 0;
    }

    int32_t       id             = atoi(argv[1]);
    const int32_t sys_autoconfig = 1;
    // param_get(param_find("SYS_AUTOCONFIG"), &sys_autoconfig);
    if (id == sys_autostart) {
        LOG_W("set the same airframe id, will not effect");
    } else {
        param_set((param_t)params_id::SYS_AUTOSTART, &id);
        param_set((param_t)params_id::SYS_AUTOCONFIG, &sys_autoconfig);
        LOG_I("airframe changed from %d to %d, please reboot", sys_autostart, id);
    }

    return 0;
}

MSH_CMD_EXPORT(change_airframe, change airframe);

static int airframe_param_init() {
    airframe_section_init();

    int32_t sys_autostart  = 0;
    int32_t sys_autoconfig = 0;
    param_get((param_t)params_id::SYS_AUTOSTART, &sys_autostart);
    param_get((param_t)params_id::SYS_AUTOCONFIG, &sys_autoconfig);

    // 判断机架设置是否合理
#ifdef BSP_USING_QEMU
    // qemu模式如果不是仿真机架，重置为1102
    if ((sys_autostart < 1100) || (sys_autostart > 1104)) {
        param_set_int32((param_t)params_id::SYS_AUTOSTART, 1103);
        sys_autostart  = 1103;
        sys_autoconfig = 1;
        LOG_W("wrong airframe for qemu, change to 1103_sih_standard_vtol");
    }
#endif // BSP_USING_QEMU

    if (sys_autostart == 0) {
        LOG_E("please set SYS_AUTOSTART first!");
        return -1;
    }

    // 判断是否需要重置参数
    if (sys_autoconfig != 0) {
        // reset all param to default, exclued the next
        const char *excludes[] = {"SYS_AUTO*", "RC*", "COM_FLTMODE*", "LND_FLIGHT*", "TC_*", "CAL_ACC*", "CAL_GYRO*", "COM_FLIGHT*"};
        param_reset_excludes(excludes, sizeof(excludes) / sizeof(char *));
        // set param to airframe value
        uint8_t         count  = 0;
        param_config_t *params = (param_config_t *)airframe_get_param(&count);
        if (params && count > 0) {
            LOG_I("reset airframe %d params", sys_autostart);

            for (param_t i = 0; i < count; i++) {
                param_t index = param_find(params[i].name);
                if (index == PARAM_INVALID) {
                    continue;
                }

                if (param_get_type(index) != params[i].type) {
                    continue;
                }

                param_set(index, &params[i].value);

                // char *ptr;
                // switch (param_get_type(index)) {
                // case PARAM_TYPE_FLOAT: {
                //     float val = (float)(strtod(airframe->params.sets[i].value, &ptr));
                //     param_set_float(index, val);
                //     break;
                // }
                // case PARAM_TYPE_INT32: {
                //     float val = strtol(airframe->params.sets[i].value, &ptr, 10);
                //     param_set_int32(index, val);
                //     break;
                // }
                // default:
                //     break;
                // }

                // param_print(index, i == 0);
            }
        }
        param_set_int32((param_t)params_id::SYS_AUTOCONFIG, 0);
    }


    LOG_I("start id: %d, hitl: %d, vehicle: %d, est: %d",
          param_get_int32((param_t)params_id::SYS_AUTOSTART),
          param_get_int32((param_t)params_id::SYS_HITL),
          param_get_int32((param_t)params_id::SYS_VEHICLE_TYPE),
          param_get_int32((param_t)params_id::SYS_MC_EST_GROUP));

    return 0;
}

INIT_EXPORT(airframe_param_init, "5.0.4");
