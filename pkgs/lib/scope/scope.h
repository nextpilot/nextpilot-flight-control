/******************************************************************
 *      _   __             __   ____   _  __        __
 *     / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *    /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *   / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 *  /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#ifndef __SCOPE_H__
#define __SCOPE_H__

#include <stdint.h>
#include <rtdbg.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

#define DEBUG_MODE DEBUG_INS_SENSOR

typedef enum {
    DEBUG_NONE,
    DEBUG_INS_SENSOR,
    DEBUG_INS_EKF,
    DEBUG_FCS_EKF,
    DEBUG_FCS_CMDR,
    DEBUG_FCS_FMM,
    DEBUG_FCS_CTR,
    DEBUG_FCS_NAV,
    DEBUG_FCS_RC,
    DEBUG_FCS_MIXER,
    DEBUG_FCS_SIM,
    DEBUG_COUNT
} debug_type_e;

#define DEBUG_VALUE_COUNT 16
extern const char *_debug_name[DEBUG_VALUE_COUNT];
extern float       _debug_value[DEBUG_VALUE_COUNT];

#define DEBUG_SET(mode, index, name, value)                \
    {                                                      \
        if (DEBUG_MODE == (mode) &&                        \
            (index >= 0) && (index < DEBUG_VALUE_COUNT)) { \
            _debug_name[(index)]  = (name);                \
            _debug_value[(index)] = (value);               \
        }                                                  \
    }

///////////////////////////////////////////////////////////////

#define SCOPE_MSG_HEADER_LEN      (3)
#define SCOPE_MSG_CHECK_LEN       (1)
#define SCOPE_MSG_PAYLOAD_MAX_LEN (255)
#define SCOPE_MSG_EXTRAL_LEN      (SCOPE_MSG_HEADER_LEN + SCOPE_MSG_CHECK_LEN)
#define SCOPE_MSG_FRAME_MAX_LEN   (SCOPE_MSG_PAYLOAD_MAX_LEN + SCOPE_MSG_EXTRAL_LEN)

// 新版本的消息
#define SCOPE_MSG_ID_TEXT   (1)
#define SCOPE_MSG_ID_VECT   (2)
#define SCOPE_MSG_ID_UUID   (4)
#define SCOPE_MSG_ID_FIELD  (4)
#define SCOPE_MSG_ID_CUSTOM (99)
// 老版的消息
#define SCOPE_MSG_ID_VECT_OLD (101)
#define SCOPE_MSG_ID_TEXT_OLD (102)

#pragma pack(push, 1)

typedef struct {
    uint32_t timestamp;
    uint8_t  severity;
    char     message[127];
} scope_msg_text_t;

typedef struct {
    uint32_t timestamp;
    uint8_t  name[8];
    float    data[16];
} scope_msg_vector_t;

typedef struct {
    uint32_t timestamp;
    uint16_t uuid;
    float    data[16];
} scope_msg_uuid_t;

typedef struct {
    uint16_t uuid;
    uint8_t  filed[127]; // topic:field1;field2;
} scope_msg_field_t;

typedef struct {
    // 帧头
    union {
        struct {
            uint8_t sync;
            uint8_t msgid;
            uint8_t length;
        };
        uint8_t header[SCOPE_MSG_HEADER_LEN];
    };
    // 内容
    union {
        scope_msg_text_t   text;
        scope_msg_vector_t vect;
        scope_msg_vector_t field;
        uint8_t            payload[SCOPE_MSG_PAYLOAD_MAX_LEN];
    };
    // 校验
    union {
        uint8_t crc8;
        uint8_t check[1];
    };
} scope_msg_t;
#pragma pack(pop)

typedef struct {
    uint32_t msgid;

    uint8_t format_send_timestamp;

    // 上一次发送的时间
    uint32_t last_send_timestamp;

    // 上一次统计的时间
    uint32_t last_meseasure_timestmp;
    uint32_t last_meseasure_sendcount;
} scope_status_t;

// 老版本的示波器
int scope_printf_old(const char *fmt, ...);
int scope_send_vector_old(float data[16], int interval_ms);
int scope_send_debug_vector(int interval_ms);

// 新版本示波器
int scope_printf(const char *fmt, ...);
int scope_send_vector(const char *name, float data[16], int interval_ms);

#ifdef __cplusplus
}
#endif // __cplusplus
#endif // __SCOPE_H__
