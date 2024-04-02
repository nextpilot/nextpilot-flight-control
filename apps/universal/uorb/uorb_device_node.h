/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#ifndef __UROB_DEVICE_NODE_H__
#define __UROB_DEVICE_NODE_H__

#include "rtthread.h"
#include <stdint.h>
#include <stdbool.h>
#include <uORB.h>

struct uorb_device_node {
    rt_list_t                  _list;
    const struct orb_metadata *_meta;
    uint8_t                   *_data;       // 数据指针，只有publish的时候才会去分配
    int                        _data_valid; // data是否已经发布过一次
    long /*rt_atomic_t */      _generation; // 发布数据次数
    uint8_t                    _instance;   // 主题的第instance个实例
    // advertised是占位符，只有公告的主题才能进行copy和publish
    // 因为有些特殊情况，需要订阅某个还没有创建的主题，且必须现在就拿到主题的地址
    // 这个时候会新建这个主题，并标记为advertised=false，表示我先占坑
    // 等下次调用orb_advertise函数的时候，使用这个坑就可以，advertised=true
    bool    _advertised;       // placeholder
    uint8_t _queue_size;       // data中保存多少拍的数据
    int8_t  _subscriber_count; // 订阅者数量
};

typedef struct uorb_device_node *uorb_device_t;
typedef struct uorb_device_node *orb_advert_t;

bool          uorb_device_is_exist(ORB_ID orb_id, uint8_t instance);
void          uorb_device_mark_exist(ORB_ID orb_id, uint8_t instance);
void          uorb_device_clear_exist(ORB_ID orb_id, uint8_t instance);
uorb_device_t uorb_device_find_node(const struct orb_metadata *meta, uint8_t instance);

void uorb_device_add_node(uorb_device_t node);
void uorb_device_remove_node(uorb_device_t node);

uint8_t uorb_device_get_queue_size(const uorb_device_t node);
int     urob_device_set_queue_size(uorb_device_t node, uint32_t queue_size);

uint8_t uorb_device_get_instance(const uorb_device_t node);
bool    uorb_device_is_advertised(const uorb_device_t node);
void    uorb_device_mark_advertised(uorb_device_t node);

unsigned uorb_device_updates_available(const uorb_device_t node, unsigned last_generation);

uorb_device_t uorb_device_add_internal_subscriber(ORB_ID orb_id, uint8_t instance, unsigned *initial_generation);
void          uorb_device_remove_internal_subscriber(uorb_device_t node);

uorb_device_t uorb_device_create(const struct orb_metadata *meta, const uint8_t instance, uint8_t queue_size);
int           uorb_device_delete(uorb_device_t node);
uorb_device_t uorb_device_advertise(const struct orb_metadata *meta, const void *data, int *instance,
                                    unsigned int queue_size);
int           uorb_device_unadvertise(orb_advert_t node);

int uorb_device_write(uorb_device_t node, const void *data);
int uorb_device_read(uorb_device_t node, void *dst, uint32_t *generation);

// Determine the data range
static inline bool is_in_range(unsigned left, unsigned value, unsigned right) {
    if (right > left) {
        return (left <= value) && (value <= right);

    } else { // Maybe the data overflowed and a wraparound occurred
        return (left <= value) || (value <= right);
    }
}

// round up to nearest power of two
// Such as 0 => 1, 1 => 1, 2 => 2 ,3 => 4, 10 => 16, 60 => 64, 65...255 => 128
// Note: When the input value > 128, the output is always 128
static inline uint8_t round_pow_of_two_8(uint8_t n) {
    if (n == 0) {
        return 1;
    }

    // Avoid is already a power of 2
    uint8_t value = n - 1;

    // Fill 1
    value |= value >> 1U;
    value |= value >> 2U;
    value |= value >> 4U;

    // Unable to round-up, take the value of round-down
    if (value == UINT8_MAX) {
        value >>= 1U;
    }

    return value + 1;
}

#endif // __UROB_DEVICE_NODE_H__
