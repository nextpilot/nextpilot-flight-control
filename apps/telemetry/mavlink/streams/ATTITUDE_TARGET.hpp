/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#ifndef ATTITUDE_TARGET_HPP
#define ATTITUDE_TARGET_HPP

#include <uORB/topics/vehicle_attitude_setpoint.h>
#include <uORB/topics/vehicle_rates_setpoint.h>

class MavlinkStreamAttitudeTarget : public MavlinkStream {
public:
    static MavlinkStream *new_instance(Mavlink *mavlink) {
        return new MavlinkStreamAttitudeTarget(mavlink);
    }

    static constexpr const char *get_name_static() {
        return "ATTITUDE_TARGET";
    }
    static constexpr uint16_t get_id_static() {
        return MAVLINK_MSG_ID_ATTITUDE_TARGET;
    }

    const char *get_name() const override {
        return get_name_static();
    }
    uint16_t get_id() override {
        return get_id_static();
    }

    unsigned get_size() override {
        return _att_sp_sub.advertised() ? MAVLINK_MSG_ID_ATTITUDE_TARGET_LEN + MAVLINK_NUM_NON_PAYLOAD_BYTES : 0;
    }

private:
    explicit MavlinkStreamAttitudeTarget(Mavlink *mavlink) :
        MavlinkStream(mavlink) {
    }

    uORB::Subscription _att_sp_sub{ORB_ID(vehicle_attitude_setpoint)};
    uORB::Subscription _att_rates_sp_sub{ORB_ID(vehicle_rates_setpoint)};
    hrt_abstime        _last_att_sp_update{0};

    bool send() override {
        vehicle_attitude_setpoint_s att_sp;

        bool updated = false;

        if (_att_sp_sub.update(&att_sp)) {
            _last_att_sp_update = att_sp.timestamp;
            updated             = true;

        } else if (hrt_elapsed_time(&_last_att_sp_update) > 500_ms) {
            if (!_att_sp_sub.copy(&att_sp)) {
                att_sp = {};
            }

            updated = _att_rates_sp_sub.updated();
        }

        if (updated) {
            mavlink_attitude_target_t msg{};

            msg.time_boot_ms = att_sp.timestamp / 1000;
            matrix::Quatf(att_sp.q_d).copyTo(msg.q);

            vehicle_rates_setpoint_s att_rates_sp{};
            _att_rates_sp_sub.copy(&att_rates_sp);

            msg.body_roll_rate  = att_rates_sp.roll;
            msg.body_pitch_rate = att_rates_sp.pitch;
            msg.body_yaw_rate   = att_rates_sp.yaw;

            msg.thrust = matrix::Vector3f(att_sp.thrust_body).norm();

            mavlink_msg_attitude_target_send_struct(_mavlink->get_channel(), &msg);

            return true;
        }

        return false;
    }
};

#endif // ATTITUDE_TARGET_HPP
