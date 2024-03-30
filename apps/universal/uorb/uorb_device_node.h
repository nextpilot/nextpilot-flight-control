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
    // rt_device_t   parent;
    rt_list_t     list;
    orb_metadata *meta;
    uint8_t      *data;
    bool          data_valid;
    rt_atomic_t   generation;
    uint8_t       instance;
    bool          advertised;
    uint8_t       queue_size;
    int8_t        subscriber_count;

    struct uorb_device_node *_head;
};

typedef struct uorb_device_node *uorb_device_t;

#ifdef __cplusplus
namespace nextpilot::uORB {
class DeviceNode : public uorb_device_node {
};

} // namespace nextpilot::uORB

#endif //__cplusplus

#endif // __UROB_DEVICE_NODE_H__
