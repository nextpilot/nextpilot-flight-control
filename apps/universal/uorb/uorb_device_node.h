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
    bool                       _data_valid; // data是否已经发布过一次
    rt_atomic_t                _generation; // 发布数据次数
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

uorb_device_t uorb_device_get_node(const struct orb_metadata *meta, uint8_t instance);
bool          uorb_device_is_exist(ORB_ID orb_id, uint8_t instance);

uorb_device_t uorb_device_create(const struct orb_metadata *meta, const uint8_t instance, uint8_t queue_size);
int           uorb_device_delete(uorb_device_t node);
bool          uorb_device_publish(uorb_device_t node, const void *data);
bool          uorb_device_copy(uorb_device_t node, void *dst, uint32_t *generation);

#endif // __UROB_DEVICE_NODE_H__
