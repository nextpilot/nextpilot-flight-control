/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#ifndef WIND_COV_HPP
#define WIND_COV_HPP

#include <uORB/topics/vehicle_local_position.h>
#include <uORB/topics/wind.h>

class MavlinkStreamWindCov : public MavlinkStream {
public:
    static MavlinkStream *new_instance(Mavlink *mavlink) {
        return new MavlinkStreamWindCov(mavlink);
    }

    static constexpr const char *get_name_static() {
        return "WIND_COV";
    }
    static constexpr uint16_t get_id_static() {
        return MAVLINK_MSG_ID_WIND_COV;
    }

    const char *get_name() const override {
        return get_name_static();
    }
    uint16_t get_id() override {
        return get_id_static();
    }

    unsigned get_size() override {
        return _wind_sub.advertised() ? MAVLINK_MSG_ID_WIND_COV_LEN + MAVLINK_NUM_NON_PAYLOAD_BYTES : 0;
    }

private:
    explicit MavlinkStreamWindCov(Mavlink *mavlink) :
        MavlinkStream(mavlink) {
    }

    uORB::Subscription _wind_sub{ORB_ID(wind)};
    uORB::Subscription _local_pos_sub{ORB_ID(vehicle_local_position)};

    bool send() override {
        wind_s wind;

        if (_wind_sub.update(&wind)) {
            mavlink_wind_cov_t msg{};

            msg.time_usec = wind.timestamp;

            msg.wind_x = wind.windspeed_north;
            msg.wind_y = wind.windspeed_east;
            msg.wind_z = 0.0f;

            msg.var_horiz = wind.variance_north + wind.variance_east;
            msg.var_vert  = 0.0f;

            vehicle_local_position_s lpos{};
            _local_pos_sub.copy(&lpos);
            msg.wind_alt = (lpos.z_valid && lpos.z_global) ? (-lpos.z + lpos.ref_alt) : (float)NAN;

            msg.horiz_accuracy = 0.0f;
            msg.vert_accuracy  = 0.0f;

            mavlink_msg_wind_cov_send_struct(_mavlink->get_channel(), &msg);

            return true;
        }

        return false;
    }
};

#endif // WIND_COV
