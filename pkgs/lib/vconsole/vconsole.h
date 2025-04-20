/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#ifndef __VCONSOLE_H__
#define __VCONSOLE_H__


#include <rtthread.h>
#include <rtdevice.h>
#include <ipc/ringbuffer.h>

#ifdef __cplusplus
extern "C" {
#endif //__cplusplus

#ifndef VCONSOLE_MAGIC_NUM
#   define VCONSOLE_MAGIC_NUM (0x11223344)
#endif //VCONSOLE_MAGIC_NUM

struct rt_vconsole_device {
    struct rt_device      parent;
    rt_uint32_t           magic;
    struct rt_ringbuffer *rx_buff;
    struct rt_ringbuffer *tx_buff;
};

typedef struct rt_vconsole_device *rt_vconsole_device_t;

rt_vconsole_device_t vconsole_create(const char *name, rt_size_t rx_buf_size, rt_size_t tx_buf_size, rt_uint8_t flag);

rt_err_t vconsole_delete(rt_vconsole_device_t vcom);

rt_size_t vconsole_input(rt_vconsole_device_t ring, rt_uint8_t *buffer, rt_size_t size);

rt_size_t vconsole_output(rt_vconsole_device_t ring, rt_uint8_t *buffer, rt_size_t size);

rt_device_t rt_console_change_device(const char *name);

#ifdef __cplusplus
}
#endif //__cplusplus

#endif // __VCONSOLE_H__
