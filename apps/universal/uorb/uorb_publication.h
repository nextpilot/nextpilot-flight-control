/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#ifndef __UORB_PUBLICATION_H__
#define __UORB_PUBLICATION_H__

#include "uorb_device_node.h"

orb_advert_t orb_advertise_multi_queue(const struct orb_metadata *meta, const void *data, int *instance, unsigned int queue_size);
orb_advert_t orb_advertise_multi(const struct orb_metadata *meta, const void *data, int *instance);
orb_advert_t orb_advertise_queue(const struct orb_metadata *meta, const void *data, unsigned int queue_size);
orb_advert_t orb_advertise(const struct orb_metadata *meta, const void *data);
int          orb_unadvertise(orb_advert_t node);
int          orb_publish(const struct orb_metadata *meta, orb_advert_t node, const void *data);
int          orb_publish_auto(const struct orb_metadata *meta, orb_advert_t *node, const void *data, int *instance);

#ifdef __cplusplus

namespace nextpilot::uORB {
template <typename U>
class DefaultQueueSize {
private:
    template <typename T>
    static constexpr uint8_t get_queue_size(decltype(T::ORB_QUEUE_LENGTH) *) {
        return T::ORB_QUEUE_LENGTH;
    }

    template <typename T>
    static constexpr uint8_t get_queue_size(...) {
        return 1;
    }

public:
    static constexpr unsigned value = get_queue_size<U>(nullptr);
};

class PublicationBase {
public:
    bool advertised() const {
        return _handle != nullptr;
    }

    bool unadvertise() {
        return (uorb_device_unadvertise(_handle) == 0);
    }

    orb_id_t get_topic() const {
        return get_orb_meta(_orb_id);
    }

protected:
    PublicationBase(ORB_ID id) :
        _orb_id(id) {
    }

    ~PublicationBase() {
        if (_handle != nullptr) {
            // don't automatically unadvertise queued publications (eg vehicle_command)
            if (uorb_device_get_queue_size(_handle) == 1) {
                unadvertise();
            }
        }
    }

    orb_advert_t _handle{nullptr};
    const ORB_ID _orb_id;
};

} // namespace nextpilot::uORB

#endif //__cplusplus

#endif // __UORB_PUBLICATION_H__
