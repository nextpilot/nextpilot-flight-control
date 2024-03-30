/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#include "uorb_subscription.h"
#include <stdbool.h>

int uorb_sbuscription_init(uorb_subscription_t sub, uint16_t id, uint8_t instance) {
    sub->_orb_id   = id;
    sub->_instance = instance;
    return 0;
}

int uorb_subscription_subscribe(uorb_subscription_t sub) {
    return 0;
}

// bool uorb_subscription_advertied(uorb_subscription_t sub) {
//     if (sub->_node) {
//         return uorb_device_is_advertised(sub->_node);
//     }

//     if (urob_subcription_subcribe(sub)) {
//         if (sub->_node) {
//             return uorb_device_is_advertised(sub->_node);
//         }
//     }
//     return false;
// }

bool uorb_subcription_updated(uorb_subscription_t sub) {
    if (!sub->_node) {
        uorb_subscription_subscribe(sub);
    }

    // return valid() ? Manager::updates_available(_node, _last_generation) : false;

    return (sub->_node != NULL);
}

bool uorb_subscription_update(uorb_subscription_t sub, void *dst) {
    if (!sub->_node) {
        uorb_subscription_subscribe(sub);
    }

    // return valid() ? Manager::orb_data_copy(_node, dst, _last_generation, true) : false;

    return (sub->_node != NULL) ? 1 : 0;
}

bool uorb_subscription_copy() {
    return 0;
}

bool uorb_subscription_change_instance(uorb_subscription_t sub, uint8_t instance) {
    return 0;
}
