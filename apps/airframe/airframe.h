/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#include <stdint.h>

#ifndef __AIRFRAME_H__
#   define __AIRFRAME_H__

#   include "nextpilot.h"

#   ifdef __cplusplus
extern "C" {
#   endif

#   pragma pack(push, 4)

typedef struct {
    const char *name;

    union {
        int32_t i32;
        float   f32;
    } value;
} param_list_t;

typedef struct {
    const char *name; // 机架名称
    int32_t     id;   // 机架编号

    // 构型参数
    struct {
        uint8_t       count;
        param_list_t *list;
    } params;

    // 分配矩阵
    // struct {
    //     uint8_t       count;
    //     mixer_list_t *list;
    // } mixers;
} airframe_info_t;

#   pragma pack(pop)

// param
#   define PARAM_CONFIG_FLOAT(_name, _value)         \
       {                                             \
           .name = #_name, .value = {.f32 = _value } \
       }

#   define PARAM_CONFIG_INT32(_name, _value)         \
       {                                             \
           .name = #_name, .value = {.i32 = _value } \
       }

#   define MIXER_DEFINE_NULL()      \
       {                            \
           .type = MIXER_TYPE_NULL, \
       }

// mc_mixer
#   define MIXER_DEFINE_MC(_count, ...)        \
       {                                       \
           .type = MIXER_TYPE_MC,              \
           .mc   = {                           \
                 .rotor_count  = _count,       \
                 .rotor_matrix = {__VA_ARGS__} \
           }                                   \
       }

#   define MC_MIXER_COUNT(_count) _count

#   define MC_MIXER_SCALE(_roll, _pitch, _yaw, _throttle) \
       {                                                  \
           .roll_scale   = _roll,                         \
           .pitch_scale  = _pitch,                        \
           .yaw_scale    = _yaw,                          \
           .thrust_scale = _throttle                      \
       }

// simple_mixer
#   define MIXER_DEFINE_SIMPLE(_count, _scale, ...)  \
       {                                             \
           .type   = MIXER_TYPE_SIMPLE,              \
           .simple = {                               \
               .control_count       = _count,        \
               .output_scaler       = _scale,        \
               .slew_rate_rise_time = 0.0f,          \
               .controls            = {__VA_ARGS__}, \
           }                                         \
       }

#   define SIMPLE_MIXER_COUNT(_count) _count

#   define SIMPLE_MIXER_SCALE(_neg_scale, _pos_scale, _offset, _min, _max) \
       {                                                                   \
           .negative_scale = _neg_scale,                                   \
           .positive_scale = _pos_scale,                                   \
           .offset         = _offset,                                      \
           .min_output     = _min,                                         \
           .max_output     = _max                                          \
       }

#   define SIMPLE_MIXER_CONTROL(_group, _index, _neg_scale, _pos_scale, _offset, _min, _max) \
       {                                                                                     \
           .control_group = _group,                                                          \
           .control_index = _index,                                                          \
           .scaler        = {                                                                \
                      .negative_scale = _neg_scale,                                          \
                      .positive_scale = _pos_scale,                                          \
                      .offset         = _offset,                                             \
                      .min_output     = _min,                                                \
                      .max_output     = _max                                                 \
           }                                                                                 \
       }

#   if defined(_MSC_VER)
#      pragma section("CetcspilotAirframeTab$p", read)
#      define AIRFRAME_EXPORT __declspec(allocate("CetcspilotAirframeTab$p")) static const airframe_info_t
#   elif defined(__TI_COMPILER_VERSION__)
#      define __TI_FINSH_EXPORT_FUNCTION(f) PRAGMA(DATA_SECTION(f, "CetcspilotAirframeTab"))
#      define AIRFRAME_EXPORT
#   else
#      define AIRFRAME_EXPORT RT_USED const airframe_info_t RT_SECTION("CetcspilotAirframeTab")
#   endif

#   define AIRFRAME_DEFINE(_name, _id, _param, _mixer)         \
       AIRFRAME_EXPORT __airframe_##_name = {                  \
           .name   = #_name,                                   \
           .id     = _id,                                      \
           .params = {                                         \
               .count = sizeof(_param) / sizeof(param_list_t), \
               .list  = _param,                                \
           },                                                  \
           .mixers = {                                         \
               .count = sizeof(_mixer) / sizeof(mixer_list_t), \
               .list  = _mixer,                                \
           },                                                  \
       }

const airframe_info_t *get_airframe_info();
// mixer_list_t          *get_airframe_mixer(uint8_t *count);
param_list_t *get_airframe_param(uint8_t *count);
void          print_airframe_info();

#   ifdef __cplusplus
}
#   endif
#endif // __AIRFRAME_H__
