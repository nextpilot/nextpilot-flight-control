/******************************************************************
 *      _   __             __   ____   _  __        __
 *     / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *    /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *   / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 *  /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

// 根据不同的板子选择调试串口
// #if defined(BSP_CETCS_FCS_V3)
// #define SCOPE_UART_NAME "uart2" // shell是uart7，mavlink是uart3
// #elif defined(BSP_CETCS_FCS_V4) && defined(DRV_USING_SNC200A)
// #define SCOPE_UART_NAME "uart4" // shell是串口6，mavlink是uart1
// #elif defined(BSP_CETCS_FCS_V4)
// #define SCOPE_UART_NAME "uart5" // shell是uart6，mavlink是uart1
// #elif defined(BSP_CETCS_INS_V4)
// #define SCOPE_UART_NAME "uart3" // shell是uart4，mavlink是uart1
// #else
// #define SCOPE_UART_NAME "uart1"
// #endif

#define LOG_TAG "scope"
#define LOG_LVL LOG_LVL_INFO

#include "rtthread.h"
#include "rtdevice.h"
#include "ipc/ringbuffer.h"
#include "scope.h"

#ifndef SCOPE_UART_NAME
#error "please define SCOPE_UART_NAME"
#endif // SCOPE_UART_NAME

float       _debug_value[DEBUG_VALUE_COUNT];
const char *_debug_name[DEBUG_VALUE_COUNT];

// #define SCOPE_UART_NAME     "uart2"
// #define SCOPE_UART_BAUDRATE 460800
// #define ULOG_USING_SCOPE_BACKEND
// #define SCOPE_USING_INSIDE_CRC8
// #define SCOPE_USING_ASYNC_OUTPUT
// #define SCOPE_ASYNC_BUFFER_SIZE      2048
// #define SCOPE_ASYNC_THREAD_PRIORITY  20
// #define SCOPE_ASYNC_THREAD_STACKSIZE 1024

static rt_device_t _scope_device = RT_NULL;

#ifdef SCOPE_USING_ASYNC_OUTPUT
static struct rt_ringbuffer *_scope_buffer = RT_NULL;
static struct rt_mutex       _scope_buffer_locker;
static rt_thread_t           _scope_thread = RT_NULL;
static struct rt_semaphore   _scope_thread_locker;
#endif // SCOPE_USING_ASYNC_OUTPUT

#ifdef SCOPE_USING_INSIDE_CRC8
static const uint8_t crc8_tab[256] =
    {
        0x00, 0x3e, 0x7c, 0x42, 0xf8, 0xc6, 0x84, 0xba,
        0x95, 0xab, 0xe9, 0xd7, 0x6d, 0x53, 0x11, 0x2f,
        0x4f, 0x71, 0x33, 0x0d, 0xb7, 0x89, 0xcb, 0xf5,
        0xda, 0xe4, 0xa6, 0x98, 0x22, 0x1c, 0x5e, 0x60,
        0x9e, 0xa0, 0xe2, 0xdc, 0x66, 0x58, 0x1a, 0x24,
        0x0b, 0x35, 0x77, 0x49, 0xf3, 0xcd, 0x8f, 0xb1,
        0xd1, 0xef, 0xad, 0x93, 0x29, 0x17, 0x55, 0x6b,
        0x44, 0x7a, 0x38, 0x06, 0xbc, 0x82, 0xc0, 0xfe,
        0x59, 0x67, 0x25, 0x1b, 0xa1, 0x9f, 0xdd, 0xe3,
        0xcc, 0xf2, 0xb0, 0x8e, 0x34, 0x0a, 0x48, 0x76,
        0x16, 0x28, 0x6a, 0x54, 0xee, 0xd0, 0x92, 0xac,
        0x83, 0xbd, 0xff, 0xc1, 0x7b, 0x45, 0x07, 0x39,
        0xc7, 0xf9, 0xbb, 0x85, 0x3f, 0x01, 0x43, 0x7d,
        0x52, 0x6c, 0x2e, 0x10, 0xaa, 0x94, 0xd6, 0xe8,
        0x88, 0xb6, 0xf4, 0xca, 0x70, 0x4e, 0x0c, 0x32,
        0x1d, 0x23, 0x61, 0x5f, 0xe5, 0xdb, 0x99, 0xa7,
        0xb2, 0x8c, 0xce, 0xf0, 0x4a, 0x74, 0x36, 0x08,
        0x27, 0x19, 0x5b, 0x65, 0xdf, 0xe1, 0xa3, 0x9d,
        0xfd, 0xc3, 0x81, 0xbf, 0x05, 0x3b, 0x79, 0x47,
        0x68, 0x56, 0x14, 0x2a, 0x90, 0xae, 0xec, 0xd2,
        0x2c, 0x12, 0x50, 0x6e, 0xd4, 0xea, 0xa8, 0x96,
        0xb9, 0x87, 0xc5, 0xfb, 0x41, 0x7f, 0x3d, 0x03,
        0x63, 0x5d, 0x1f, 0x21, 0x9b, 0xa5, 0xe7, 0xd9,
        0xf6, 0xc8, 0x8a, 0xb4, 0x0e, 0x30, 0x72, 0x4c,
        0xeb, 0xd5, 0x97, 0xa9, 0x13, 0x2d, 0x6f, 0x51,
        0x7e, 0x40, 0x02, 0x3c, 0x86, 0xb8, 0xfa, 0xc4,
        0xa4, 0x9a, 0xd8, 0xe6, 0x5c, 0x62, 0x20, 0x1e,
        0x31, 0x0f, 0x4d, 0x73, 0xc9, 0xf7, 0xb5, 0x8b,
        0x75, 0x4b, 0x09, 0x37, 0x8d, 0xb3, 0xf1, 0xcf,
        0xe0, 0xde, 0x9c, 0xa2, 0x18, 0x26, 0x64, 0x5a,
        0x3a, 0x04, 0x46, 0x78, 0xc2, 0xfc, 0xbe, 0x80,
        0xaf, 0x91, 0xd3, 0xed, 0x57, 0x69, 0x2b, 0x15};

static uint8_t crc8part(const uint8_t *src, size_t len, uint8_t crc8val) {
    size_t i;

    crc8val ^= 0xff;
    for (i = 0; i < len; i++) {
        crc8val = crc8_tab[crc8val ^ src[i]];
    }

    return crc8val ^ 0xff;
}
#else
uint8_t crc8part(const uint8_t *src, size_t len, uint8_t crc8val);
#endif // SCOPE_USING_INSIDE_CRC8

static inline uint8_t trim_payload_tail_zero(const uint8_t *payload, uint8_t length) {
    while (length > 1 && payload[length - 1] == 0) {
        length--;
    }
    return length;
}

static inline uint8_t trim_string_tail_byte(const char *payload, uint8_t length) {
    uint8_t len = 0;
    while (len < length && payload[len] != '\0') {
        len++;
    }
    return len;
}

scope_status_t *scope_get_status(const char *name) {
    static scope_status_t status[16];
    int                   count = 0;

    uint8_t msgid = crc8part((uint8_t *)name, rt_strlen(name), 0);

    for (int i = 0; i < 16; i++) {
        if (status[i].msgid == msgid) {
            return status + i;
        }
    }

    if (count < 16) {
        rt_memset(status + count, 0, sizeof(scope_status_t));
        return status + (count++);
    }

    return NULL;
}

int scope_send_internal(uint8_t msgid, void *payload, int length) {
    uint8_t buff[SCOPE_MSG_FRAME_MAX_LEN] = {0};
    int     size                          = 0;

    if (msgid == SCOPE_MSG_ID_VECT_OLD) {
        // #float_data[16]C
        // #是帧头，float_data[16]是16个float数据，C是data的和校验

        if (length != 64) {
            length = 64;
        }

        // header
        buff[0] = '#';
        // payload
        rt_memcpy(buff + 1, payload, length);
        // check
        uint8_t check = 0;
        for (int i = 0; i < length; i++) {
            check += ((uint8_t *)payload)[i];
        }
        buff[1 + length] = check;

        size = length + 2;
    } else if (msgid == SCOPE_MSG_ID_TEXT_OLD) {
        // $Lxxxxxxxx~~C
        // $是帧头，L是长度，xxxx是字符串内容，~~是帧尾，C是xxx的和校验
        if (length > UINT8_MAX) {
            length = UINT8_MAX;
        }

        // header
        buff[0] = '$';
        buff[1] = length;
        // payload
        rt_memcpy(buff + 2, payload, length);
        // 校验
        uint8_t check = 0;
        for (int i = 0; i < length; i++) {
            check += ((uint8_t *)payload)[i];
        }
        buff[2 + length] = check;

        size = length + 3;
    } else {
        if (length > SCOPE_MSG_PAYLOAD_MAX_LEN) {
            LOG_E("payload too long");
            length = SCOPE_MSG_PAYLOAD_MAX_LEN;
        }

        // 去掉尾部的0
        // length = trim_payload_tail_zero(payload, length);

        // 帧头
        buff[0] = '#';
        buff[1] = msgid;
        buff[2] = length;
        // payload
        rt_memcpy(buff + SCOPE_MSG_HEADER_LEN, payload, length);

        // 校验
        uint8_t check                       = 0;
        check                               = crc8part(buff, length + SCOPE_MSG_HEADER_LEN, 0);
        buff[length + SCOPE_MSG_HEADER_LEN] = check;
        size                                = length + SCOPE_MSG_EXTRAL_LEN;
    }

    if (size <= 0) {
        return -1;
    }

#ifdef SCOPE_USING_ASYNC_OUTPUT
    if (!_scope_buffer) {
        return 0;
    }

    // 将消息压到rb中
    rt_mutex_take(&_scope_buffer_locker, RT_WAITING_FOREVER);
    if (rt_ringbuffer_data_len(_scope_buffer) + size > SCOPE_ASYNC_BUFFER_SIZE) {
        rt_mutex_release(&_scope_buffer_locker);
        return -1;
    }
    rt_ringbuffer_put(_scope_buffer, buff, size);
    rt_mutex_release(&_scope_buffer_locker);

    if (_scope_thread_locker.value <= 0) {
        rt_sem_release(&_scope_thread_locker);
    }
#else
    rt_device_write(_scope_device, 0, buff, size);
#endif // SCOPE_USING_ASYNC_OUTPUT

    return size;
}

int scope_printf_old(const char *fmt, ...) {
    if (!fmt) {
        return 0;
    }

    char buffer[255];
    int  length = 0;

    va_list ap;
    va_start(ap, fmt);
    // 预留1个字节用来保存\0
    length = rt_vsnprintf(buffer, sizeof(buffer) - 1, fmt, ap);
    va_end(ap);

    return scope_send_internal(SCOPE_MSG_ID_TEXT_OLD, buffer, length);
}

int scope_send_vector_old(float data[16], int interval_ms) {
    static rt_tick_t last_send_timestamp = 0;

    if (rt_tick_get() - last_send_timestamp < interval_ms) {
        return -1;
    }

    last_send_timestamp = rt_tick_get();

    return scope_send_internal(SCOPE_MSG_ID_VECT_OLD, data, 64);
}

int scope_send_debug_vector(int interval_ms) {
    int len = scope_send_vector_old(_debug_value, interval_ms);

    static uint8_t   index     = 0;
    static uint8_t   init_flag = 0;
    static rt_tick_t init_msec = 0;

    if (!init_flag) {
        init_msec = rt_tick_get();
        init_flag = 1;
    }

    if (index < 16 && len >= 0 &&
        (rt_tick_get() - init_msec > 2 * 1e3)) {
        scope_printf_old("ch%02d: %s\n", index, _debug_name[index]);
        index++;
    }

    return len;
}

int scope_printf(const char *fmt, ...) {
    if (!fmt) {
        return 0;
    }

    scope_msg_text_t txt;

    txt.timestamp = rt_tick_get();
    txt.severity  = LOG_LVL_INFO;

    va_list ap;
    va_start(ap, fmt);
    // 预留1个字节用来保存\0
    int length = rt_vsnprintf(txt.message, sizeof(txt.message) - 1, fmt, ap);
    va_end(ap);

    RT_UNUSED(length);

    return scope_send_internal(SCOPE_MSG_ID_TEXT, &txt, sizeof(txt));
}

int scope_send_vector(const char *name, float data[16], int interval_ms) {
    RT_ASSERT(name != RT_NULL);

    // uint8_t         uuid   = crc8part(name, rt_strlen(name), 0);
    scope_status_t *status = scope_get_status(name);

    if (!status) {
        return 0;
    }

    if (rt_tick_get() - status->last_send_timestamp < interval_ms) {
        return 0;
    }

    // if (status->format_have_send < 3) {
    //   scope_msg_field_t field = {0};
    //  field.uuid              = uuid;
    //  rt_memcpy(field.filed, name, 127);
    //  scope_send_internal(SCOPE_MSG_ID_FIELD, &field, sizeof(field));
    //}

    status->last_send_timestamp = rt_tick_get();

    scope_msg_vector_t vect = {0};

    vect.timestamp = rt_tick_get();
    rt_memcpy(vect.name, name, sizeof(vect.name));
    rt_memcpy(vect.data, data, sizeof(vect.data));

    return scope_send_internal(SCOPE_MSG_ID_VECT, &vect, sizeof(vect));
}

#ifdef SCOPE_USING_ASYNC_OUTPUT
static void scope_thread_entry(void *param) {
    static uint8_t buff[512];
    while (1) {
        rt_sem_take(&_scope_thread_locker, RT_WAITING_FOREVER);

        while (1) {
            rt_mutex_take(&_scope_buffer_locker, RT_WAITING_FOREVER);
            if (rt_ringbuffer_data_len(_scope_buffer) <= 0) {
                rt_mutex_release(&_scope_buffer_locker);
                break;
            }
            // rt_memset(buff, 0, sizeof(buff));
            int len = rt_ringbuffer_get(_scope_buffer, buff, sizeof(buff));
            rt_mutex_release(&_scope_buffer_locker);

            rt_device_write(_scope_device, 0, buff, len);
        }
    }
}
#endif // SCOPE_USING_ASYNC_OUTPUT

static int scope_init() {
    // 初始化串口
    _scope_device = rt_device_find(SCOPE_UART_NAME);
    if (!_scope_device) {
        LOG_E("find %s fail", SCOPE_UART_NAME);
        return -1;
    }

    // 配置串口
    struct serial_configure config = ((struct rt_serial_device *)_scope_device)->config;
    config.baud_rate               = SCOPE_UART_BAUDRATE;

#if defined(RT_USING_SERIAL_V1)
    config.bufsz = 1024;
#else
    config.tx_bufsz = 1024;
    config.rx_bufsz = 512;
#endif // RT_USING_SERIAL_V1

    if (rt_device_control(_scope_device, RT_DEVICE_CTRL_CONFIG, &config) != 0) {
        LOG_E("config %s fail", SCOPE_UART_NAME);
        return -1;
    }

    // 打开串口，注意串口是以阻塞模式发送的数据的
    // 如果采用同步，将会影响主线程(类似rt_kprintf)
    // 如果采用异步，则不会影响主线程(类似ulog)
    uint16_t openflag = 0;
#if defined(RT_USING_SERIAL_V1)
    if (_scope_device->flag & RT_DEVICE_FLAG_DMA_RX)
        openflag |= RT_DEVICE_FLAG_DMA_RX;
    else if (_scope_device->flag & RT_DEVICE_FLAG_INT_RX) {
        openflag |= RT_DEVICE_FLAG_INT_RX;
    }

    if (_scope_device->flag & RT_DEVICE_FLAG_DMA_TX)
        openflag |= RT_DEVICE_FLAG_DMA_TX;
    else if (_scope_device->flag & RT_DEVICE_FLAG_INT_TX) {
        openflag |= RT_DEVICE_FLAG_INT_TX;
    }
#else
#ifdef SCOPE_USING_ASYNC_OUTPUT
    openflag = RT_DEVICE_FLAG_TX_BLOCKING | RT_DEVICE_FLAG_RX_NON_BLOCKING;
#else
    openflag = RT_DEVICE_FLAG_TX_NON_BLOCKING | RT_DEVICE_FLAG_RX_NON_BLOCKING;
#endif // SCOPE_USING_ASYNC_OUTPUT
#endif // RT_USING_SERIAL_V1

    if (rt_device_open(_scope_device, openflag) != 0) {
        LOG_E("open %s fail", SCOPE_UART_NAME);
        return -1;
    }

#ifdef SCOPE_USING_ASYNC_OUTPUT
    rt_mutex_init(&_scope_buffer_locker, "scope", RT_IPC_FLAG_PRIO);
    rt_sem_init(&_scope_thread_locker, "scope", 0, RT_IPC_FLAG_FIFO);

    // 创建ringbuffer
    _scope_buffer = rt_ringbuffer_create(SCOPE_ASYNC_BUFFER_SIZE);
    if (!_scope_buffer) {
        LOG_E("create ringbuffer fail");
        return -1;
    }

    // 创建线程
    _scope_thread = rt_thread_create("scope", scope_thread_entry, RT_NULL, SCOPE_ASYNC_THREAD_STACKSIZE, SCOPE_ASYNC_THREAD_PRIORITY, 20);
    if (!_scope_thread) {
        LOG_E("create thread fail");
        return -1;
    }

    // 启动线程
    if (rt_thread_startup(_scope_thread) != 0) {
        LOG_E("starup thread fail");
        return -1;
    }
#endif // SCOPE_USING_ASYNC_OUTPUT

    LOG_I("start over %s", SCOPE_UART_NAME);

    return 0;
}

INIT_APP_EXPORT(scope_init);

#ifdef ULOG_USING_SCOPE_BACKEND
static void ulog_scope_backend_output(struct ulog_backend *backend,
                                      rt_uint32_t          level,
                                      const char          *tag,
                                      rt_bool_t            is_raw,
                                      const char          *log,
                                      rt_size_t            len) {
    scope_msg_text_t txt;
    txt.timestamp = rt_tick_get();
    txt.severity  = level;
    if (len > sizeof(txt.message)) {
        len = sizeof(txt.message);
    }
    rt_memcpy(txt.message, log, len);

    scope_send_internal(SCOPE_MSG_ID_TEXT, &txt, sizeof(txt));
}

static struct ulog_backend _scope_be = {0};

static int ulog_scope_backend_init() {
    _scope_be.output = ulog_scope_backend_output;
    // 由于ulog的字符串后续需要通过uorb进行处理，故不要添加”颜色“，否则字符串头与尾会增加额外表示颜色的字节
    ulog_backend_register(&_scope_be, "scope", RT_FALSE);
    return 0;
}

// INIT_APP_EXPORT(ulog_scope_backend_init);
#endif // ULOG_USING_SCOPE_BACKEND
