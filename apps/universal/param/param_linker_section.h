/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#ifndef __PARAM_INTERFACE_SECTION_H__
#define __PARAM_INTERFACE_SECTION_H__

#include "param_common.h"

typedef struct {
    const char         *name;
    const param_type_t  type;
    const param_value_t value;
    const param_flag_t  flag;
    param_value_t       data;
    param_status_t      status;
} section_param_data_t;

#pragma pack(push, 4)
typedef struct {
    section_param_data_t *ptr;
} section_param_info_t;
#pragma pack(pop)

#if defined(_MSC_VER)
#pragma section("NextpilotParamTab$p", read)
#pragma comment(linker, "/merge:NextpilotParamTab=myparam")
#define PARAM_EXPORT __declspec(allocate("NextpilotParamTab$p")) static section_param_info_t
#elif defined(__TI_COMPILER_VERSION__)
#define __TI_FINSH_EXPORT_FUNCTION(f) PRAGMA(DATA_SECTION(f, "NextpilotParamTab"))
#define PARAM_EXPORT
#else
#define PARAM_EXPORT RT_USED const section_param_info_t RT_SECTION("NextpilotParamTab")
#endif

#define PARAM_DEFINE_FLOAT(_name, _value, _flag)          \
    RT_USED section_param_data_t __param_data_##_name = { \
        .name   = #_name,                                 \
        .type   = PARAM_TYPE_FLOAT,                       \
        .value  = {.f32 = _value},                        \
        .data   = {.f32 = _value},                        \
        .status = {.value = 0},                           \
    };                                                    \
    PARAM_EXPORT __param_info_##_name = {                 \
        .ptr = &__param_data_##_name,                     \
    }

#ifdef __cplusplus
#define PARAM_DECLARE(_name) extern "C" param_data_t __param_data_##_name
#else
#define PARAM_DECLARE(_name) extern param_data_t __param_data_##_name
#endif //__cplusplus

#ifdef __cplusplus
namespace nextpilot::section_params {

} // namespace nextpilot::section_params
#endif //__cplusplus

#endif // __PARAM_INTERFACE_SECTION_H__
