/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

/**
 * @author Pavel Kirienko <pavel.kirienko@gmail.com>
 */

#pragma once

#include <containers/List.hpp>
#include <uavcan/uavcan.hpp>
#include <drivers/drv_orb_dev.h>
#include <drivers/device/ Device.hpp>
#include <uORB/uORB.h>

/**
 * A sensor bridge class must implement this interface.
 */
class IUavcanSensorBridge : uavcan::Noncopyable, public ListNode<IUavcanSensorBridge *> {
public:
    static constexpr unsigned MAX_NAME_LEN = 20;

    virtual ~IUavcanSensorBridge() = default;

    /**
     * Returns ASCII name of the bridge.
     */
    virtual const char *get_name() const = 0;

    /**
     * Starts the bridge.
     * @return Non-negative value on success, negative on error.
     */
    virtual int init() = 0;

    /**
     * Returns number of active redundancy channels.
     */
    virtual unsigned get_num_redundant_channels() const = 0;

    /**
     * Prints current status in a human readable format to stdout.
     */
    virtual void print_status() const = 0;

    virtual void update(){};

    /**
     * Sensor bridge factory.
     * Creates all known sensor bridges and puts them in the linked list.
     */
    static void make_all(uavcan::INode &node, List<IUavcanSensorBridge *> &list);
};

namespace uavcan_bridge {
struct Channel {
    int          node_id{-1};
    orb_advert_t orb_advert{nullptr};
    int          instance{-1};
    void        *h_driver{nullptr};
};
} // namespace uavcan_bridge

/**
 * This is the base class for redundant sensors with an independent ORB topic per each redundancy channel.
 * For example, sensor_mag0, sensor_mag1, etc.
 */
class UavcanSensorBridgeBase : public IUavcanSensorBridge, public device::Device {
    const orb_id_t                _orb_topic;
    uavcan_bridge::Channel *const _channels;
    bool                          _out_of_channels = false;

protected:
    static constexpr unsigned DEFAULT_MAX_CHANNELS = 4;
    const unsigned            _max_channels;

    UavcanSensorBridgeBase(const char *name, const orb_id_t orb_topic_sensor,
                           const unsigned max_channels = DEFAULT_MAX_CHANNELS) :
        Device(name),
        _orb_topic(orb_topic_sensor),
        _channels(new uavcan_bridge::Channel[max_channels]),
        _max_channels(max_channels) {
        set_device_bus_type(DeviceBusType_UAVCAN);
        set_device_bus(0);
    }

    /**
     * Sends one measurement into appropriate ORB topic.
     * New redundancy channels will be registered automatically.
     * @param node_id Sensor's Node ID
     * @param report  Pointer to ORB message object
     */
    void publish(const int node_id, const void *report);

    /**
     * Init the sensor driver for this channel.
     * Implementation depends on sensor type being constructed.
     * @param channel Channel pointer for which h_driver should be initialized.
     */
    virtual int init_driver(uavcan_bridge::Channel *channel) {
        return PX4_OK;
    }

    uavcan_bridge::Channel *get_channel_for_node(int node_id);

public:
    virtual ~UavcanSensorBridgeBase();

    unsigned get_num_redundant_channels() const override;

    int8_t get_channel_index_for_node(int node_id);

    void print_status() const override;
};
