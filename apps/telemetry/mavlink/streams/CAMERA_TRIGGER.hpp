/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#ifndef CAMERA_TRIGGER_HPP
#define CAMERA_TRIGGER_HPP

#include <uORB/topics/camera_status.h>
#include <uORB/topics/camera_trigger.h>

class MavlinkStreamCameraTrigger : public MavlinkStream {
public:
    static MavlinkStream *new_instance(Mavlink *mavlink) {
        return new MavlinkStreamCameraTrigger(mavlink);
    }

    static constexpr const char *get_name_static() {
        return "CAMERA_TRIGGER";
    }
    static constexpr uint16_t get_id_static() {
        return MAVLINK_MSG_ID_CAMERA_TRIGGER;
    }

    const char *get_name() const override {
        return get_name_static();
    }
    uint16_t get_id() override {
        return get_id_static();
    }

    bool const_rate() override {
        return true;
    }

    unsigned get_size() override {
        if (_camera_trigger_sub.advertised()) {
            return MAVLINK_MSG_ID_CAMERA_TRIGGER_LEN + MAVLINK_NUM_NON_PAYLOAD_BYTES + MAVLINK_MSG_ID_COMMAND_LONG_LEN + MAVLINK_NUM_NON_PAYLOAD_BYTES; // TODO: MAV_CMD_DO_DIGICAM_CONTROL
        }

        return 0;
    }

private:
    explicit MavlinkStreamCameraTrigger(Mavlink *mavlink) :
        MavlinkStream(mavlink) {
    }

    uORB::Subscription _camera_trigger_sub{ORB_ID(camera_trigger)};
    uORB::Subscription _camera_status_sub{ORB_ID(camera_status)};
    camera_status_s    _camera_status = {
        0,                 // timestamp
        0,                 // target_sys_id
        MAV_COMP_ID_CAMERA // active_comp_id
    };
    int _sequence{1};

    bool send() override {
        camera_trigger_s camera_trigger;

        if ((_mavlink->get_free_tx_buf() >= get_size()) && _camera_trigger_sub.update(&camera_trigger)) {
            /* ensure that only active trigger events are sent and ignore camera capture feedback messages*/
            if (camera_trigger.timestamp > 0 && !camera_trigger.feedback) {
                mavlink_camera_trigger_t msg{};
                msg.time_usec = camera_trigger.timestamp;
                msg.seq       = camera_trigger.seq;
                mavlink_msg_camera_trigger_send_struct(_mavlink->get_channel(), &msg);

                _camera_status_sub.update(&_camera_status);

                vehicle_command_s vcmd{};
                vcmd.timestamp        = hrt_absolute_time();
                vcmd.param1           = 0.0f; // all cameras
                vcmd.param2           = 0.0f; // duration 0 because only taking one picture
                vcmd.param3           = 1.0f; // only take one
                vcmd.param4           = (float)_sequence++;
                vcmd.param5           = (double)NAN;
                vcmd.param6           = (double)NAN;
                vcmd.param7           = NAN;
                vcmd.command          = MAV_CMD_IMAGE_START_CAPTURE;
                vcmd.target_system    = mavlink_system.sysid;
                vcmd.target_component = _camera_status.active_comp_id;

                MavlinkCommandSender::instance().handle_vehicle_command(vcmd, _mavlink->get_channel());

                // TODO: move this camera_trigger and publish as a vehicle_command
                /* send MAV_CMD_DO_DIGICAM_CONTROL*/
                mavlink_command_long_t command_long_msg{};

                command_long_msg.target_system    = _camera_status.active_sys_id;
                command_long_msg.target_component = _camera_status.active_comp_id;
                command_long_msg.command          = MAV_CMD_DO_DIGICAM_CONTROL;
                command_long_msg.confirmation     = 0;
                command_long_msg.param1           = NAN;
                command_long_msg.param2           = NAN;
                command_long_msg.param3           = NAN;
                command_long_msg.param4           = NAN;
                command_long_msg.param5           = 1; // take 1 picture
                command_long_msg.param6           = NAN;
                command_long_msg.param7           = NAN;

                mavlink_msg_command_long_send_struct(_mavlink->get_channel(), &command_long_msg);

                return true;
            }
        }

        return false;
    }
};

#endif // CAMERA_TRIGGER_HPP
