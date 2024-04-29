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
 * @file gnss.hpp
 *
 * UAVCAN <--> ORB bridge for GNSS messages:
 *     uavcan.equipment.gnss.Fix (deprecated, but still supported for backward compatibility)
 *     uavcan.equipment.gnss.Fix2
 *
 * @author Pavel Kirienko <pavel.kirienko@gmail.com>
 * @author Andrew Chambers <achamber@gmail.com>
 */

#pragma once

#include <uORB/Subscription.hpp>
#include <uORB/SubscriptionMultiArray.hpp>
#include <uORB/PublicationMulti.hpp>
#include <uORB/topics/sensor_gps.h>
#include <uORB/topics/gps_inject_data.h>

#include <uavcan/uavcan.hpp>
#include <uavcan/equipment/gnss/Auxiliary.hpp>
#include <uavcan/equipment/gnss/Fix.hpp>
#include <uavcan/equipment/gnss/Fix2.hpp>
#include <ardupilot/gnss/MovingBaselineData.hpp>
#include <uavcan/equipment/gnss/RTCMStream.hpp>

#include <perf/perf_counter.h>

#include "sensor_bridge.hpp"

class UavcanGnssBridge : public UavcanSensorBridgeBase {
public:
    static const char *const NAME;

    UavcanGnssBridge(uavcan::INode &node);
    ~UavcanGnssBridge();

    const char *get_name() const override {
        return NAME;
    }

    int  init() override;
    void update() override;
    void print_status() const override;

private:
    /**
     * GNSS fix message will be reported via this callback.
     */
    void gnss_auxiliary_sub_cb(const uavcan::ReceivedDataStructure<uavcan::equipment::gnss::Auxiliary> &msg);
    void gnss_fix_sub_cb(const uavcan::ReceivedDataStructure<uavcan::equipment::gnss::Fix> &msg);
    void gnss_fix2_sub_cb(const uavcan::ReceivedDataStructure<uavcan::equipment::gnss::Fix2> &msg);

    template <typename FixType>
    void process_fixx(const uavcan::ReceivedDataStructure<FixType> &msg,
                      uint8_t                                       fix_type,
                      const float (&pos_cov)[9], const float (&vel_cov)[9],
                      const bool valid_pos_cov, const bool valid_vel_cov,
                      const float heading, const float heading_offset,
                      const float heading_accuracy);

    void handleInjectDataTopic();
    bool PublishRTCMStream(const uint8_t *data, size_t data_len);
    bool PublishMovingBaselineData(const uint8_t *data, size_t data_len);

    typedef uavcan::MethodBinder<UavcanGnssBridge *,
                                 void (UavcanGnssBridge::*)(const uavcan::ReceivedDataStructure<uavcan::equipment::gnss::Auxiliary> &)>
        AuxiliaryCbBinder;

    typedef uavcan::MethodBinder<UavcanGnssBridge *,
                                 void (UavcanGnssBridge::*)(const uavcan::ReceivedDataStructure<uavcan::equipment::gnss::Fix> &)>
        FixCbBinder;

    typedef uavcan::MethodBinder<UavcanGnssBridge *,
                                 void (UavcanGnssBridge::*)(const uavcan::ReceivedDataStructure<uavcan::equipment::gnss::Fix2> &)>
        Fix2CbBinder;

    typedef uavcan::MethodBinder<UavcanGnssBridge *,
                                 void (UavcanGnssBridge::*)(const uavcan::TimerEvent &)>
        TimerCbBinder;

    uavcan::INode &_node;

    uavcan::Subscriber<uavcan::equipment::gnss::Auxiliary, AuxiliaryCbBinder> _sub_auxiliary;
    uavcan::Subscriber<uavcan::equipment::gnss::Fix, FixCbBinder>             _sub_fix;
    uavcan::Subscriber<uavcan::equipment::gnss::Fix2, Fix2CbBinder>           _sub_fix2;

    uavcan::Publisher<ardupilot::gnss::MovingBaselineData> _pub_moving_baseline_data;
    uavcan::Publisher<uavcan::equipment::gnss::RTCMStream> _pub_rtcm_stream;

    uint64_t _last_gnss_auxiliary_timestamp{0};
    float    _last_gnss_auxiliary_hdop{0.0f};
    float    _last_gnss_auxiliary_vdop{0.0f};

    uORB::SubscriptionMultiArray<gps_inject_data_s, gps_inject_data_s::MAX_INSTANCES> _orb_inject_data_sub{ORB_ID::gps_inject_data};
    hrt_abstime                                                                       _last_rtcm_injection_time{0}; ///< time of last rtcm injection
    uint8_t                                                                           _selected_rtcm_instance{0};   ///< uorb instance that is being used for RTCM corrections

    bool _system_clock_set{false};                                                                                  ///< Have we set the system clock at least once from GNSS data?

    bool *_channel_using_fix2;                                                                                      ///< Flag for whether each channel is using Fix2 or Fix msg

    bool _publish_rtcm_stream{false};
    bool _publish_moving_baseline_data{false};

    perf_counter_t _rtcm_stream_pub_perf{nullptr};
    perf_counter_t _moving_baseline_data_pub_perf{nullptr};
};
