/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#ifndef MAG_CAL_REPORT_HPP
#define MAG_CAL_REPORT_HPP

#include <lib/sensor_calibration/Magnetometer.hpp>

#include <uORB/topics/parameter_update.h>
#include <uORB/topics/sensor_mag.h>

class MavlinkStreamMagCalReport : public MavlinkStream {
public:
    static MavlinkStream *new_instance(Mavlink *mavlink) {
        return new MavlinkStreamMagCalReport(mavlink);
    }

    static constexpr const char *get_name_static() {
        return "MAG_CAL_REPORT";
    }
    static constexpr uint16_t get_id_static() {
        return MAVLINK_MSG_ID_MAG_CAL_REPORT;
    }

    const char *get_name() const override {
        return get_name_static();
    }
    uint16_t get_id() override {
        return get_id_static();
    }

    unsigned get_size() override {
        return _parameter_update_sub.advertised() ? MAVLINK_MSG_ID_MAG_CAL_REPORT_LEN + MAVLINK_NUM_NON_PAYLOAD_BYTES : 0;
    }

private:
    explicit MavlinkStreamMagCalReport(Mavlink *mavlink) :
        MavlinkStream(mavlink) {
    }

    static constexpr int MAX_SENSOR_COUNT = 4;

    uORB::SubscriptionMultiArray<sensor_mag_s, MAX_SENSOR_COUNT> _sensor_mag_subs{ORB_ID::sensor_mag};
    uORB::Subscription                                           _parameter_update_sub{ORB_ID(parameter_update)};

    bool send() override {
        bool               sent = false;
        parameter_update_s parameter_update;

        if (_parameter_update_sub.update(&parameter_update)) {
            for (int mag = 0; mag < MAX_SENSOR_COUNT; mag++) {
                sensor_mag_s sensor_mag;

                if (_sensor_mag_subs[mag].update(&sensor_mag) && (sensor_mag.device_id != 0)) {
                    calibration::Magnetometer calibration{sensor_mag.device_id};

                    if (calibration.calibrated()) {
                        mavlink_mag_cal_report_t msg{};

                        msg.compass_id = mag;
                        msg.cal_mask   = 0;               // TODO: Bitmask of compasses being calibrated.
                        msg.cal_status = MAG_CAL_SUCCESS; // TODO: Calibration Status.
                        msg.fitness    = 0;               // TODO: RMS milligauss residuals.

                        msg.ofs_x = calibration.offset()(0);
                        msg.ofs_y = calibration.offset()(1);
                        msg.ofs_z = calibration.offset()(2);

                        msg.diag_x = calibration.scale()(0, 0);
                        msg.diag_y = calibration.scale()(1, 1);
                        msg.diag_z = calibration.scale()(2, 2);

                        msg.offdiag_x = calibration.scale()(0, 1);
                        msg.offdiag_y = calibration.scale()(0, 2);
                        msg.offdiag_z = calibration.scale()(1, 2);

                        msg.orientation_confidence = 1.f;                         // TODO: orientation_confidence
                        msg.old_orientation        = calibration.rotation_enum(); // TODO: old orientation
                        msg.new_orientation        = calibration.rotation_enum();
                        msg.scale_factor           = 1.f;

                        mavlink_msg_mag_cal_report_send_struct(_mavlink->get_channel(), &msg);
                        sent = true;
                    }
                }
            }
        }

        return sent;
    }
};

#endif // MAG_CAL_REPORT_HPP
