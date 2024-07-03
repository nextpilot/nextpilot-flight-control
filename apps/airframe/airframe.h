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

// struct param_config_s {
//     const uint16_t id;
//     const char    *name;

//     const union {
//         uint32_t u32;
//         int32_t  i32;
//         float    f32;
//         double   f64;
//     } value;
// };

// #define PARAM_CONFIG_INT32(_name, _val)    \
//     {                                      \
//         .id    = nextpilot::params::_name, \
//         .name  = #_name,                   \
//         .value = {.f32 = _val},            \
//     }

// #define MIXER_TYPE_NULL   0
// #define MIXER_TYPE_MC     1
// #define MIXER_TYPE_SIMPLE 2

// // null_mixer
// #define MIXER_DEFINE_NULL()      \
//     {                            \
//         .type = MIXER_TYPE_NULL, \
//     }

// // mc_mixer
// #define MIXER_DEFINE_MC(_count, ...)        \
//     {                                       \
//         .type = MIXER_TYPE_MC,              \
//         .mc   = {                           \
//               .rotor_count  = _count,       \
//               .rotor_matrix = {__VA_ARGS__} \
//         }                                   \
//     }

// #define MIXER_MC_COUNT(_count) (_count)

// #define MIXER_MC_SCALE(_roll, _pitch, _yaw, _throttle) \
//     {                                                  \
//         .roll_scale   = _roll,                         \
//         .pitch_scale  = _pitch,                        \
//         .yaw_scale    = _yaw,                          \
//         .thrust_scale = _throttle                      \
//     }

// // simple_mixer
// #define MIXER_DEFINE_SIMPLE(_count, _scale, ...)  \
//     {                                             \
//         .type   = MIXER_TYPE_SIMPLE,              \
//         .simple = {                               \
//             .control_count       = _count,        \
//             .output_scaler       = _scale,        \
//             .slew_rate_rise_time = 0.0f,          \
//             .controls            = {__VA_ARGS__}, \
//         }                                         \
//     }

// #define MIXER_SIMPLE_COUNT(_count) (_count)

// #define MIXER_SIMPLE_SCALE(_neg_scale, _pos_scale, _offset, _min, _max) \
//     {                                                                   \
//         .negative_scale = _neg_scale,                                   \
//         .positive_scale = _pos_scale,                                   \
//         .offset         = _offset,                                      \
//         .min_output     = _min,                                         \
//         .max_output     = _max                                          \
//     }

// #define MIXER_SIMPLE_CONTROL(_group, _index, _neg_scale, _pos_scale, _offset, _min, _max) \
//     {                                                                                     \
//         .control_group = _group,                                                          \
//         .control_index = _index,                                                          \
//         .scaler        = {                                                                \
//                    .negative_scale = _neg_scale,                                          \
//                    .positive_scale = _pos_scale,                                          \
//                    .offset         = _offset,                                             \
//                    .min_output     = _min,                                                \
//                    .max_output     = _max                                                 \
//         }                                                                                 \
//     }
