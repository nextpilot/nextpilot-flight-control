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
    rt_list_t            list;
    struct orb_metadata *meta;
    uint8_t             *data;
    bool                 data_valid;
    rt_atomic_t          generation;
    uint8_t              instance;
    bool                 advertised;
    uint8_t              queue_size;
    int8_t               subscriber_count;
};

typedef struct uorb_device_node *uorb_device_t;
typedef struct uorb_device_node *orb_advert_t;

uorb_device_t uorb_device_get_node(const struct orb_metadata *meta, uint8_t instance);
bool          uorb_device_is_exist(ORB_ID orb_id, uint8_t instance);

int  uorb_device_unadvertise(uorb_device_t node);
bool uorb_device_publish(uorb_device_t node, const void *data);

#ifdef __cplusplus
namespace nextpilot::uORB {
class DeviceNode : public uorb_device_node {
};

} // namespace nextpilot::uORB

#endif //__cplusplus

#endif // __UROB_DEVICE_NODE_H__
