#ifndef __PARAM_H__
#define __PARAM_H__

#include <stdint.h>

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
        uint8_t system_required : 1;   // 只有系统可以修改
        uint8_t restart_required : 1;  // 修改，重启生效
        uint8_t disarmed_required : 1; // 加锁状态才能修改
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

#pragma pack(push, 4)
typedef struct param_info_s {
    const char         *name;
    const param_type_t  type;
    const param_value_t value;
    const param_flag_t  flag;

    // param_value_t       current_value;
    // param_status_t      current_status;
} param_info_t;
#pragma pack(pop)

typedef struct param_curr_s {
    param_value_t  value;
    param_status_t status;
} param_curr_t;
#endif //__PARAM_H__
