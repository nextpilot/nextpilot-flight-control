/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#ifndef ESTIMATOR_STATUS_HPP
#define ESTIMATOR_STATUS_HPP

#include <uORB/topics/estimator_selector_status.h>
#include <uORB/topics/estimator_status.h>

class MavlinkStreamEstimatorStatus : public MavlinkStream {
public:
    static MavlinkStream *new_instance(Mavlink *mavlink) {
        return new MavlinkStreamEstimatorStatus(mavlink);
    }

    static constexpr const char *get_name_static() {
        return "ESTIMATOR_STATUS";
    }
    static constexpr uint16_t get_id_static() {
        return MAVLINK_MSG_ID_ESTIMATOR_STATUS;
    }

    const char *get_name() const override {
        return get_name_static();
    }
    uint16_t get_id() override {
        return get_id_static();
    }

    unsigned get_size() override {
        return _estimator_status_sub.advertised() ? MAVLINK_MSG_ID_ESTIMATOR_STATUS_LEN + MAVLINK_NUM_NON_PAYLOAD_BYTES : 0;
    }

private:
    explicit MavlinkStreamEstimatorStatus(Mavlink *mavlink) :
        MavlinkStream(mavlink) {
    }

    uORB::Subscription _estimator_selector_status_sub{ORB_ID(estimator_selector_status)};
    uORB::Subscription _estimator_status_sub{ORB_ID(estimator_status)};

    bool send() override {
        // use primary estimator_status
        if (_estimator_selector_status_sub.updated()) {
            estimator_selector_status_s estimator_selector_status;

            if (_estimator_selector_status_sub.copy(&estimator_selector_status)) {
                if (estimator_selector_status.primary_instance != _estimator_status_sub.get_instance()) {
                    _estimator_status_sub.ChangeInstance(estimator_selector_status.primary_instance);
                }
            }
        }

        estimator_status_s est;

        if (_estimator_status_sub.update(&est)) {
            mavlink_estimator_status_t est_msg{};
            est_msg.time_usec          = est.timestamp;
            est_msg.vel_ratio          = est.vel_test_ratio;
            est_msg.pos_horiz_ratio    = est.pos_test_ratio;
            est_msg.pos_vert_ratio     = est.hgt_test_ratio;
            est_msg.mag_ratio          = est.mag_test_ratio;
            est_msg.hagl_ratio         = est.hagl_test_ratio;
            est_msg.tas_ratio          = est.tas_test_ratio;
            est_msg.pos_horiz_accuracy = est.pos_horiz_accuracy;
            est_msg.pos_vert_accuracy  = est.pos_vert_accuracy;
            est_msg.flags              = est.solution_status_flags;
            mavlink_msg_estimator_status_send_struct(_mavlink->get_channel(), &est_msg);

            return true;
        }

        return false;
    }
};

#endif // ESTIMATOR_STATUS_HPP
