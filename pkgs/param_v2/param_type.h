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

// 参数取值类型
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

// 参数值联合体
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

// 参数功能标志
typedef union {
    uint8_t value;

    struct {
        uint8_t system_required : 1; // 只有系统可以修改
        uint8_t reboot_required : 1; // 修改，重启生效
        uint8_t disarm_required : 1; // 加锁状态才能修改
    };
} param_flag_t;

// 参数当前状态
typedef union {
    uint8_t value;

    struct {
        uint8_t actived         : 1;
        uint8_t changed         : 1;
        uint8_t unsaved         : 1;
        uint8_t unsaved_to_ulog : 1;
    };
} param_status_t;

// 参数meta信息
typedef struct param_info_s {
    const char   *name;
    param_type_t  type;
    param_value_t value;
    param_flag_t  flag;
} param_info_t;

// 存到文件的头部格式
typedef struct {
    uint32_t check;
    char     magic[9]; // "nextpilot"
    uint64_t utc;      // 保存时间戳
    uint16_t count;    // 参数个数
} param_header_t;

// 存储到文件的参数格式
typedef struct {
    char          name[16];
    param_type_t  type;
    param_value_t value;
} param_payload_t;

// 参数值字节数
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

// 参数类型字符串
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

typedef bool (*param_filter_func)(param_t handle);

#endif // __PARAM_COMMON_H__
