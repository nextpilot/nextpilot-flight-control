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

#include "airframe.h"

// 获取section的首位地址
#if defined(__ARMCC_VERSION) /* ARM C Compiler */
// 如果不在sct文件定义section，则根据代码自动创建
extern const int        nextpilotAirframeTab$$Base;
extern const int        nextpilotAirframeTab$$Limit;
static airframe_info_t *__airframe_section_start_ptr = (airframe_info_t *)&nextpilotAirframeTab$$Base;
static airframe_info_t *__airframe_section_end_ptr   = (airframe_info_t *)&nextpilotAirframeTab$$Limit;
static airframe_info_t *__airframe_table__           = NULL;
static uint16_t         __airframe_count__           = UINT16_MAX; // nextpilotAirframeTab$$Length
#elif defined(__ICCARM__) || defined(__ICCRX__)                    /* for IAR Compiler */
#   pragma section = "nextpilotAirframeTab"
static airframe_info_t *__airframe_section_start_ptr = (airframe_info_t *)__section_begin("nextpilotAirframeTab");
static airframe_info_t *__airframe_section_end_ptr   = (airframe_info_t *)__section_end("nextpilotAirframeTab");
static airframe_info_t *__airframe_table__           = NULL;
static uint16_t         __airframe_count__           = UINT16_MAX;
#elif defined(__GNUC__) || defined(__TI_COMPILER_VERSION__) || defined(__TASKING__)
// 请在lds文件中定义好nextpilot_airframe_tab section
extern const int        __nextpilot_airframe_tab_start;
extern const int        __nextpilot_airframe_tab_end;
static airframe_info_t *__airframe_section_start_ptr = (airframe_info_t *)&__nextpilot_airframe_tab_start;
static airframe_info_t *__airframe_section_end_ptr   = (airframe_info_t *)&__nextpilot_airframe_tab_end;
static airframe_info_t *__airframe_table__           = NULL;
static uint16_t         __airframe_count__           = UINT16_MAX;
#elif defined(__ADSPBLACKFIN__) /* for VisaulDSP++ Compiler*/
// 如何定义section
extern "asm" int        __nextpilot_airframe_tab_start;
extern "asm" int        __nextpilot_airframe_tab_end;
static airframe_info_t *__airframe_section_start_ptr = (airframe_info_t *)&__nextpilot_airframe_tab_start;
static airframe_info_t *__airframe_section_end_ptr   = (airframe_info_t *)&__nextpilot_airframe_tab_end;
static airframe_info_t *__airframe_table__           = NULL;
static uint16_t         __airframe_count__           = UINT16_MAX;
#elif defined(_MSC_VER)
#   pragma section("nextpilotAirframeTab$a", read)
#   pragma section("nextpilotAirframeTab$p", read)
#   pragma section("nextpilotAirframeTab$z", read)
__declspec(allocate("nextpilotAirframeTab$a")) static airframe_info_t __nextpilot_airframe_tab_start = {
    .name = "airframe_section_start",
};
__declspec(allocate("nextpilotAirframeTab$z")) static airframe_info_t __nextpilot_airframe_tab_end = {
    .name = "airframe_section_end",
};
static airframe_info_t *__airframe_section_start_ptr = &__nextpilot_airframe_tab_start;
static airframe_info_t *__airframe_section_end_ptr   = &__nextpilot_airframe_tab_end;
static airframe_info_t  __airframe_table__[100];
static uint16_t         __airframe_count__ = UINT16_MAX;
#endif

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

static inline uint16_t get_airframe_count() {
    if (__airframe_count__ == UINT16_MAX) {
        airframe_section_init();
    }

    return __airframe_count__;
}

static inline airframe_info_t *get_airframe_table() {
    if (__airframe_count__ == UINT16_MAX) {
        airframe_section_init();
    }

    return __airframe_table__;
}

static airframe_info_t *_airframe_info = NULL;

const airframe_info_t *get_airframe_info() {
    if (!_airframe_info) {
        int32_t sys_autostart = 0;
        param_get(param_find("SYS_AUTOSTART"), &sys_autostart);
        for (int i = 0; i < get_airframe_count(); i++) {
            if (__airframe_table__[i].id == sys_autostart) {
                return &__airframe_table__[i];
            }
        }
        LOG_W("find airframe(id=%d) fail", sys_autostart);
        return NULL;
    } else {
        return _airframe_info;
    }
}

// mixer_list_t *get_airframe_mixer(uint8_t *count) {
//     const airframe_info_t *info = get_airframe_info();
//     if (info) {
//         if (count) {
//             *count = info->mixers.count;
//         }
//         if (info->mixers.list) {
//             return info->mixers.list;
//         }
//     }
//     return NULL;
// }

param_list_t *get_airframe_param(uint8_t *count) {
    const airframe_info_t *info = get_airframe_info();
    if (info) {
        if (count) {
            *count = info->params.count;
        }
        if (info->params.list) {
            return info->params.list;
        }
    }
    return NULL;
}

void print_airframe_info() {
    const airframe_info_t *info = get_airframe_info();
    if (info) {
        rt_kprintf("airfame: %s(%d)\n", info->name, info->id);
    }
}

static int airframe_param_init() {
    airframe_section_init();

    // // TODO: 当前情况下只要选择仿真，就只使用1102这一个机架，根据需要，后续再修改这部分逻辑
    // #ifdef BSP_SITL_QEMU
    //     int32_t sys_autoconfig = 1;
    //     param_set_int32(param_find("SYS_AUTOSTART"), 1102);
    // #else
    int32_t sys_autoconfig = 0;
    param_get(param_find("SYS_AUTOCONFIG"), &sys_autoconfig);
    // #endif // BSP_SITL_QEMU

    if (sys_autoconfig != 0) {
        // reset all param to default, exclued the next
        const char *excludes[] = {"SYS_AUTO*", "RC*", "COM_FLTMODE*", "LND_FLIGHT*", "TC_*", "CAL_ACC*", "CAL_GYRO*", "COM_FLIGHT*"};
        param_reset_excludes(excludes, sizeof(excludes) / sizeof(char *));
        // set param to airframe value
        const airframe_info_t *info = get_airframe_info();
        if (info && info->params.list) {
            for (uint8_t i = 0; i < info->params.count; i++) {
                param_set(param_find(info->params.list[i].name), &info->params.list[i].value);
                // param_print(param_find(info->params.list[i].name), i == 0);
            }
            LOG_W("reset airframe(id=%d) parameters", info->id);
        }
        param_set_int32(param_find("SYS_AUTOCONFIG"), 0);
    }

#ifdef BSP_SITL_QEMU
    // param_set_int32(param_find("SYS_HITL"), 2);
#endif

    int32_t sys_autostart = 0;
    param_get(param_find("SYS_AUTOSTART"), &sys_autostart);
    if (sys_autostart == 0) {
        LOG_E("please set SYS_AUTOSTART first!!!");
    } else {
        LOG_I("init ok");
    }
    return 0;
}

INIT_EXPORT(airframe_param_init, "5.0.4");
