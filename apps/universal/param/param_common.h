/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#ifndef __PARAM_COMMON_H__
#define __PARAM_COMMON_H__

#include <stdint.h>
#include <stdbool.h>

#define PARAM_HASH        ((uint16_t)INT16_MAX)
#define PARAM_HASH_NAME   "_HASH_CHECK"
#define PARAM_INVALID     UINT16_MAX
#define PARAM_NAME_LENGTH 16

typedef uint16_t param_t;

typedef enum {
    PARAM_INTERFACE_UNKONWN = 0,
    PARAM_INTERFACE_AUTOGEN,
    PARAM_INTERFACE_SECTION,
    PARAM_INTERFACE_SIMULINK,
} param_interface_t;

typedef enum {
    PARAM_TYPE_UNKNOWN = 0,
    PARAM_TYPE_INT8,
    PARAM_TYPE_UINT8,
    PARAM_TYPE_INT16,
    PARAM_TYPE_UINT16,
    PARAM_TYPE_INT32,
    PARAM_TYPE_UINT32,
    PARAM_TYPE_INT64,
    PARAM_TYPE_UINT64,
    PARAM_TYPE_FLOAT,
    PARAM_TYPE_DOUBLE,
    PARAM_TYPE_MAX
} param_type_t;

typedef union {
    int8_t   i8;
    uint8_t  u8;
    int16_t  i16;
    uint16_t u16;
    int32_t  i32;
    uint32_t u32;
    // int64_t  i64;
    // uint64_t u64;
    float f32;
    // double      f64;
} param_value_t;

typedef union {
    uint8_t value;
    struct {
        uint8_t system_required : 1; // 只有系统可以修改
        uint8_t reboot_required : 1; // 修改，重启生效
        uint8_t disarm_required : 1; // 加锁状态才能修改
    };
} param_flag_t;

typedef union {
    uint8_t value;
    struct {
        uint8_t actived : 1;
        uint8_t changed : 1;
        uint8_t unsaved : 1;
        uint8_t unsaved_to_ulog : 1;
    };
} param_status_t;

typedef struct param_info_s {
    const char   *name;
    param_type_t  type;
    param_value_t value;
    param_flag_t  flag;
} param_info_t;

typedef struct param_data_s {
    param_value_t  value;
    param_status_t status;
} param_data_t;

static inline uint8_t param_type_size(param_type_t type) {
    switch (type) {
    case PARAM_TYPE_INT8:
    case PARAM_TYPE_UINT8:
        return 1;
    case PARAM_TYPE_INT16:
    case PARAM_TYPE_UINT16:
        return 2;
    case PARAM_TYPE_INT32:
    case PARAM_TYPE_UINT32:
    case PARAM_TYPE_FLOAT:
        return 4;
    case PARAM_TYPE_INT64:
    case PARAM_TYPE_UINT64:
    case PARAM_TYPE_DOUBLE:
        return 8;
    default:
        return 0;
    }
}

static inline const char *param_type_cstr(param_type_t type) {
    switch (type) {
    case PARAM_TYPE_INT8:
        return "INT8";
    case PARAM_TYPE_UINT8:
        return "UINT8";
    case PARAM_TYPE_INT16:
        return "INT16";
    case PARAM_TYPE_UINT16:
        return "UINT16";
    case PARAM_TYPE_INT32:
        return "INT32";
    case PARAM_TYPE_UINT32:
        return "UINT32";
    case PARAM_TYPE_INT64:
        return "INT64";
    case PARAM_TYPE_UINT64:
        return "UINT64";
    case PARAM_TYPE_FLOAT:
        return "FLOAT";
    case PARAM_TYPE_DOUBLE:
        return "DOUBLE";
    default:
        return "UNKONWN";
    }
}

void param_notify_changes();

#endif // __PARAM_COMMON_H__
