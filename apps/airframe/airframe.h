/******************************************************************
 *      _   __             __   ____   _  __        __
 *     / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *    /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *   / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 *  /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#ifndef __AIRFRAME_H__
#define __AIRFRAME_H__

#include <stdint.h>
#include "rtthread.h"
#include "param.h"

typedef int (*airframe_custom_init_fcn)();

#pragma pack(push, 4)

typedef struct {
    const char   *name; // 机架名称
    const int32_t id;   // 机架编号

    // 自定义的初始化函数
    // airframe_custom_init_fcn init;
    // 构型参数
    struct {
        uint8_t count;
        void   *point;
    } params;

    // 分配矩阵
    struct {
        uint8_t count;
        void   *point;
    } mixers;
} airframe_info_t;

#pragma pack(pop)

typedef struct param_config_s {
    const char         *name; // 变量名
    const param_type_t  type;
    const param_value_t value;
} param_config_t;

#define PARAM_CONFIG_FLOAT(_name, _value) \
    {                                     \
        .name  = #_name,                  \
        .type  = PARAM_TYPE_FLOAT,        \
        .value = {.f32 = _value},         \
    }

#define PARAM_CONFIG_INT32(_name, _value) \
    {                                     \
        .name  = #_name,                  \
        .type  = PARAM_TYPE_INT32,        \
        .value = {.i32 = _value},         \
    }

#if defined(_MSC_VER)
#   pragma section("NextpilotAirframeTab$p", read)
#   define AIRFRAME_EXPORT __declspec(allocate("NextpilotAirframeTab$p")) static const airframe_info_t
#elif defined(__TI_COMPILER_VERSION__)
#   define __TI_FINSH_EXPORT_FUNCTION(f) PRAGMA(DATA_SECTION(f, "NextpilotAirframeTab"))
#   define AIRFRAME_EXPORT
#else
#   define AIRFRAME_EXPORT RT_USED const airframe_info_t RT_SECTION("NextpilotAirframeTab")
#endif

#define AIRFRAME_DEFINE_ALL(_name, _id, _param)          \
    AIRFRAME_EXPORT __airframe_##_name = {               \
        .name   = #_name,                                \
        .id     = _id,                                   \
        .params = {                                      \
            .count = sizeof(_param) / sizeof(_param[0]), \
            .point = _param,                             \
        },                                               \
    }

#define AIRFRAME_DEFINE_PARAM(_name, _id, _param)        \
    AIRFRAME_EXPORT __airframe_##_name = {               \
        .name   = #_name,                                \
        .id     = _id,                                   \
        .params = {                                      \
            .count = sizeof(_param) / sizeof(_param[0]), \
            .point = _param,                             \
        },                                               \
        .mixers = {                                      \
            .count = 0,                                  \
            .point = NULL,                               \
        },                                               \
    }

#define AIRFRAME_DEFINE_MIXER(_name, _id, _mixer)        \
    AIRFRAME_EXPORT __airframe_##_name = {               \
        .name   = #_name,                                \
        .id     = _id,                                   \
        .params = {                                      \
            .count = 0,                                  \
            .point = NULL,                               \
        },                                               \
        .mixers = {                                      \
            .count = sizeof(_mixer) / sizeof(_mixer[0]), \
            .point = _mixer,                             \
        },                                               \
    }

airframe_info_t *airframe_get_info();
void            *airframe_get_mixer(uint8_t *count);
void            *airframe_get_param(uint8_t *count);
void             airframe_print_info();

#endif // __AIRFRAME_H__
