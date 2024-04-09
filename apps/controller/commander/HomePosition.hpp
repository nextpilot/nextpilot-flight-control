/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#pragma once

#include <uORB/Publication.hpp>
#include <uORB/Subscription.hpp>
#include <uORB/topics/home_position.h>
#include <uORB/topics/sensor_gps.h>
#include <uORB/topics/vehicle_global_position.h>
#include <uORB/topics/vehicle_local_position.h>
#include <uORB/topics/failsafe_flags.h>

static constexpr int   kHomePositionGPSRequiredFixType = 2;
static constexpr float kHomePositionGPSRequiredEPH     = 5.f;
static constexpr float kHomePositionGPSRequiredEPV     = 10.f;
static constexpr float kHomePositionGPSRequiredEVH     = 1.f;

class HomePosition {
public:
    HomePosition(const failsafe_flags_s &failsafe_flags);
    ~HomePosition() = default;

    bool setHomePosition(bool force = false);
    void setInAirHomePosition();
    bool setManually(double lat, double lon, float alt, float yaw);

    void update(bool set_automatically, bool check_if_changed);

    bool valid() const {
        return _valid;
    }

private:
    bool hasMovedFromCurrentHomeLocation();
    void setHomePosValid();
    void updateHomePositionYaw(float yaw);

    static void fillLocalHomePos(home_position_s &home, const vehicle_local_position_s &lpos);
    static void fillLocalHomePos(home_position_s &home, float x, float y, float z, float heading);
    static void fillGlobalHomePos(home_position_s &home, const vehicle_global_position_s &gpos);
    static void fillGlobalHomePos(home_position_s &home, double lat, double lon, float alt);

    uORB::Subscription _vehicle_gps_position_sub{ORB_ID(vehicle_gps_position)};

    uORB::SubscriptionData<vehicle_global_position_s> _global_position_sub{ORB_ID(vehicle_global_position)};
    uORB::SubscriptionData<vehicle_local_position_s>  _local_position_sub{ORB_ID(vehicle_local_position)};

    uORB::PublicationData<home_position_s> _home_position_pub{ORB_ID(home_position)};

    uint8_t                 _heading_reset_counter{0};
    bool                    _valid{false};
    const failsafe_flags_s &_failsafe_flags;
    bool                    _gps_position_for_home_valid{false};
    double                  _gps_lat{0};
    double                  _gps_lon{0};
    float                   _gps_alt{0.f};
    float                   _gps_eph{0.f};
    float                   _gps_epv{0.f};
};
