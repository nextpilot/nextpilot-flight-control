#define LOG_TAG "scope"
#define LOG_LVL LOG_LVL_INFO

#define SCOPE_META_MAX_COUNT  (16)
#define SCOPE_NAME_MAX_LENGTH (16)
#define SCOPE_FIELD_MAX_COUNT (16)

#include <stdint.h>
#include <stdarg.h>
#include <stdlib.h>
#include <rtthread.h>
#include <rtdbg.h>

#include "scope.h"

enum {
    SCOPE_TYPE_INT8,
    SCOPE_TYPE_INT16,
    SCOPE_TYPE_INT32,
    SCOPE_TYPE_INT64,
    SCOPE_TYPE_UINT8,
    SCOPE_TYPE_UINT16,
    SCOPE_TYPE_UINT32,
    SCOPE_TYPE_UINT64,
    SCOPE_TYPE_FLOAT32,
    SCOPE_TYPE_FLOAT64,
};

typedef struct {
    // 是否有效
    uint8_t valid;
    // 字段名
    char name[SCOPE_NAME_MAX_LENGTH];
    // 字段类型
    uint8_t type;
    // 字段维度（数组长度，如果是标量则为0）
    uint8_t dims;
    // 字段字节数
    uint8_t size;
} scope_field_t;

typedef struct {
    // 是否有效
    uint8_t valid;

    // 消息id，采用crc16计算
    uint16_t msgid;
    // 主题名称
    char topic[SCOPE_NAME_MAX_LENGTH];
    // 字段情况
    scope_field_t field[SCOPE_FIELD_MAX_COUNT];

    // 上一次发送的时间
    uint32_t last_send_timestamp;
    // 上一次统计的时间
    uint32_t last_meseasure_timestmp;
    uint32_t last_meseasure_sendcount;
} scope_meta_t;

static scope_meta_t _scope_meta_table[SCOPE_META_MAX_COUNT];
static int          _scope_meta_count = 0;

int scope_vsnprintf_getmsgid(const char *fmt) {
    if (!fmt) {
        return -1;
    }

    int id = 0;
    for (; *fmt; ++fmt) {
        id += (*fmt);
    }

    return id;
}

scope_field_t scope_vsnprintf_getfield(const char *name, const char *type, const char *dims) {
    scope_field_t info = {0};

    // 字段name
    memcpy(info.name, name, 16);
    info.name[15] = '\0';

    // 字段dimension
    info.dims = atoi(dims);

    // 字段type和sizeof
    if ((strcmp(type, "float") == 0) ||
        (strcmp(type, "f32") == 0) ||
        (strcmp(type, "foat32") == 0)) {
        info.type = SCOPE_TYPE_FLOAT32;
        info.size = 4;

    } else if ((strcmp(type, "double") == 0) ||
               (strcmp(type, "f64") == 0) ||
               (strcmp(type, "float64") == 0)) {
        info.type = SCOPE_TYPE_FLOAT64;
        info.size = 8;

    } else if ((strcmp(type, "int32") == 0) ||
               (strcmp(type, "i32") == 0) ||
               (strcmp(type, "int32_t") == 0)) {
        info.type = SCOPE_TYPE_INT32;
        info.size = 4;

    } else if ((strcmp(type, "uint32") == 0) ||
               (strcmp(type, "u32") == 0) ||
               (strcmp(type, "uint32_t") == 0)) {
        info.type = SCOPE_TYPE_UINT32;
        info.size = 4;

    } else if ((strcmp(type, "int16") == 0) ||
               (strcmp(type, "i16") == 0) ||
               (strcmp(type, "int16_t") == 0)) {
        info.type = SCOPE_TYPE_INT16;
        info.size = 2;

    } else if ((strcmp(type, "uint16") == 0) ||
               (strcmp(type, "u16") == 0) ||
               (strcmp(type, "uint16_t") == 0)) {
        info.type = SCOPE_TYPE_UINT16;
        info.size = 2;

    } else if (strcmp(type, "int8") == 0) {
        info.type = SCOPE_TYPE_INT8;
        info.size = 1;

    } else if (strcmp(type, "uint8") == 0) {
        info.type = SCOPE_TYPE_UINT8;
        info.size = 1;
    }

    info.valid = 1;
    return info;
}

scope_meta_t scope_vsnprintf_getmeta(const char *fmt) {
    scope_meta_t meta = {0};

    if (!fmt) {
        return meta;
    }

    char *topic = meta.topic;
    char  name[16];
    char  dims[16];
    char  type[16];
    int   idx = 0;
    int   pos = 0;

    enum {
        STAGE_MSG_NAME,
        STAGE_FIELD_TYPE,
        STAGE_FIELD_DIMS,
        STAGE_FIELD_NAME,
    } stage = STAGE_MSG_NAME;

    for (; *fmt; ++fmt) {
        if (stage == STAGE_MSG_NAME) {
            if (*fmt == ':') {
                topic[pos++] = '\0';
                pos          = 0;
                stage        = STAGE_FIELD_TYPE;
            } else {
                topic[pos++] = *fmt;
            }
        } else if (stage == STAGE_FIELD_TYPE) { // 查找类型
            if (*fmt == '[') {
                type[pos++] = '\0';
                pos         = 0;
                stage       = STAGE_FIELD_DIMS;
            } else if (*fmt == ' ') {
                type[pos++] = '\0';
                pos         = 0;
                dims[0]     = '0';
                dims[1]     = '\0';
                stage       = STAGE_FIELD_NAME;
            } else {
                type[pos++] = *fmt;
            }
        } else if (stage == STAGE_FIELD_DIMS) { // 查找维度
            if (*fmt == ']') {
                dims[pos++] = '\0';
                pos         = 0;
                fmt++; // 因为变量名之前有一个空格
                stage = STAGE_FIELD_NAME;
            } else {
                dims[pos++] = *fmt;
            }
        } else if (stage == STAGE_FIELD_NAME) { // 查找名称
            if (*fmt == ';') {
                name[pos++] = '\0';
                pos         = 0;
                stage       = STAGE_FIELD_TYPE;

                meta.field[idx++] = scope_vsnprintf_getfield(name, type, dims);

            } else if (!(*(fmt + 1))) { // 最后一个字符
                name[pos++] = *fmt;
                name[pos++] = '\0';

                meta.field[idx++] = scope_vsnprintf_getfield(name, type, dims);
            } else {
                name[pos++] = *fmt;
            }
        }
    }

    meta.valid = 1;
    meta.msgid = scope_vsnprintf_getmsgid(fmt);
    return meta;
}

void scope_vsnprintf_printdata(char *buff, scope_field_t *info) {
    int dims = (info->dims == 0) ? 1 : info->dims;
    rt_kprintf("%8s = ", info->name);
    switch (info->type) {
    case SCOPE_TYPE_FLOAT32:
        for (int i = 0; i < dims; i++) {
            rt_kprintf("%f, ", *((float *)buff));
            buff += info->size;
        }
        break;
    case SCOPE_TYPE_FLOAT64:
        for (int i = 0; i < dims; i++) {
            rt_kprintf("%f, ", *((double *)buff));
            buff += info->size;
        }
        break;
    case SCOPE_TYPE_UINT32:
        for (int i = 0; i < dims; i++) {
            rt_kprintf("%d, ", *((uint32_t *)buff));
            buff += info->size;
        }
        break;
    case SCOPE_TYPE_INT32:
        for (int i = 0; i < dims; i++) {
            rt_kprintf("%d, ", *((int32_t *)buff));
            buff += info->size;
        }
        break;
    case SCOPE_TYPE_UINT16:
        for (int i = 0; i < dims; i++) {
            rt_kprintf("%d, ", *((uint16_t *)buff));
            buff += info->size;
        }
        break;
    case SCOPE_TYPE_INT16:
        for (int i = 0; i < dims; i++) {
            rt_kprintf("%d, ", *((int16_t *)buff));
            buff += info->size;
        }
        break;
    case SCOPE_TYPE_UINT8:
        for (int i = 0; i < dims; i++) {
            rt_kprintf("%d, ", *((uint8_t *)buff));
            buff += info->size;
        }
        break;
    case SCOPE_TYPE_INT8:
        for (int i = 0; i < dims; i++) {
            rt_kprintf("%d, ", *((int8_t *)buff));
            buff += info->size;
        }
        break;
    default:
        break;
    }
    rt_kprintf("\n");
}

char *scope_vsnprintf_internal(char *buff, char *end, scope_field_t *info, va_list *args) {
    // 当前字段占用的字节数
    int bytes = 0;
    if (info->dims == 0) { // 标量
        bytes = info->size;
    } else { // 数组
        bytes = info->size * info->dims;
    }

    // 判断buff长度是否够
    if (buff + bytes > end) {
        LOG_E("buff length not enough");
        return buff;
    }

    // 拷贝数据
    if (info->dims == 0) { // 标量
        // 直接拷贝n个字节
        memcpy(buff, *args, bytes);
        (*args) += bytes;
    } else { // 数组
        // 取出指针，然后拷贝
        char *data = va_arg((*args), char *);
        memcpy(buff, data, bytes);
    }

    // 打印结果
    scope_vsnprintf_printdata(buff, info);

    buff += bytes;

    return buff;
}

int scope_vsnprintf_by_meta(char *buff, int size, scope_meta_t *meta, va_list args) {
    return 0;
}

// fmt格式如下
//
// topic:type1[dims1] name1;type2[dims2] name2;
//
// topic 消息主题名字，使用冒号分割
//
// type 字段的类型，支持float/double, uint64/int64, int32/uint32, int16/uint16, int8/uint8
// dims 字段的长度，使用方括号分割，如[3]表示长度3，目前只支持1维数组
// name 字段的名字，使用分号分割，最后一个分号可以省略
//
// float[5] a ：表示a是float数组，长度为5
// float[1] b ：表示b是float数组，长度为1
// float c ：表示c是float类型，但不是数组

int scope_vsnprintf_by_fmt(char *buff, int size, const char *fmt, va_list args) {
    char *ptr = buff;
    char *end = buff + size;

    enum {
        STAGE_MSG_NAME,
        STAGE_FIELD_TYPE,
        STAGE_FIELD_DIMS,
        STAGE_FIELD_NAME,
    } stage = STAGE_MSG_NAME;

    int pos = 0;

    char topic[16];
    char name[16];
    char dims[16];
    char type[16];

    scope_meta_t meta = {0};
    memset(&meta, 0, sizeof(meta));

    for (; *fmt; ++fmt) {
        if (stage == STAGE_MSG_NAME) {
            if (*fmt == ':') {
                topic[pos++] = '\0';
                pos          = 0;
                stage        = STAGE_FIELD_TYPE;
            } else {
                topic[pos++] = *fmt;
            }
        } else if (stage == STAGE_FIELD_TYPE) { // 查找类型
            if (*fmt == '[') {
                type[pos++] = '\0';
                pos         = 0;
                stage       = STAGE_FIELD_DIMS;
            } else if (*fmt == ' ') {
                type[pos++] = '\0';
                pos         = 0;
                dims[0]     = '0';
                dims[1]     = '\0';
                stage       = STAGE_FIELD_NAME;
            } else {
                type[pos++] = *fmt;
            }
        } else if (stage == STAGE_FIELD_DIMS) { // 查找维度
            if (*fmt == ']') {
                dims[pos++] = '\0';
                pos         = 0;
                fmt++; // 因为变量名之前有一个空格
                stage = STAGE_FIELD_NAME;
            } else {
                dims[pos++] = *fmt;
            }
        } else if (stage == STAGE_FIELD_NAME) { // 查找名称
            if (*fmt == ';') {
                name[pos++] = '\0';
                pos         = 0;
                stage       = STAGE_FIELD_TYPE;

                scope_field_t field = scope_vsnprintf_getfield(name, type, dims);
                ptr                 = scope_vsnprintf_internal(ptr, end, &field, &args);

            } else if (!(*(fmt + 1))) { // 最后一个字符
                name[pos++] = *fmt;
                name[pos++] = '\0';

                scope_field_t field = scope_vsnprintf_getfield(name, type, dims);
                ptr                 = scope_vsnprintf_internal(ptr, end, &field, &args);
            } else {
                name[pos++] = *fmt;
            }
        }
    }

    return ptr - buff;
}

int scope_send_custom(int interval_ms, const char *fmt, ...) {
    char buff[SCOPE_MSG_PAYLOAD_MAX_LEN];

    // 根据fmt计算出msgid
    int msgid = scope_vsnprintf_getmsgid(fmt);

#ifdef SCOPE_USING_META
    scope_meta_t *meta = NULL;
    // 在table中查找是否有对应的msgid
    for (int i = 0; i < _scope_meta_count; i++) {
        if (_scope_meta_table[i].msgid == msgid) {
            meta = &_scope_meta_table[i];
        }
    }

    if (!meta) {
        scope_meta_t tmp = scope_vsnprintf_getmeta(fmt);
        if (tmp.valid) {
            _scope_meta_table[_scope_meta_count++] = tmp;
            meta                                   = &_scope_meta_table[_scope_meta_count];
        } else {
            LOG_E("get meta fail");
            return 0;
        }
    }

    // 时间没有到
#ifdef __RTTHREAD__
    uint32_t now = rt_tick_get();
#else
    uint32_t now = time(NULL);
#endif
    if (now - meta->last_send_timestamp < interval_ms) {
        return 0;
    }

#endif

    // 解析数据
    int     len = 0;
    va_list args;
    va_start(args, fmt);
#ifdef SCOPE_USING_META
    len = scope_vsnprintf_by_fmt(buff, SCOPE_MSG_PAYLOAD_MAX_LEN - 1, meta, args);
#else
    len = scope_vsnprintf_by_fmt(buff, SCOPE_MSG_PAYLOAD_MAX_LEN - 1, fmt, args);
#endif
    va_end(args);

    int ret = 0;
    // 发送字符串

    // 发送数据
    ret += scope_send_internal(1, buff, len);

#ifdef SCOPE_USING_META
    meta->last_send_timestamp = now;
    meta->last_meseasure_sendcount++;
#endif

    return ret;
}

int test() {
    const char fmt[]    = "mc_att_control:float[3] att;int32[2] count;double gg";
    float      att[3]   = {4199.32f, 54336.96f, 666.3f};
    int32_t    count[2] = {56788, 724};
    double     gg       = 965433.26;
    scope_send_custom(50, fmt, att, count, gg);
    return 0;
}