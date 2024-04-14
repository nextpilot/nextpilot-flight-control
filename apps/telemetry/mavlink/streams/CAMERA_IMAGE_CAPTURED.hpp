/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#ifndef CAMERA_IMAGE_CAPTURED_HPP
#define CAMERA_IMAGE_CAPTURED_HPP

#include <uORB/topics/camera_capture.h>

class MavlinkStreamCameraImageCaptured : public MavlinkStream {
public:
    static MavlinkStream *new_instance(Mavlink *mavlink) {
        return new MavlinkStreamCameraImageCaptured(mavlink);
    }

    static constexpr const char *get_name_static() {
        return "CAMERA_IMAGE_CAPTURED";
    }
    static constexpr uint16_t get_id_static() {
        return MAVLINK_MSG_ID_CAMERA_IMAGE_CAPTURED;
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
        return _capture_sub.advertised() ? MAVLINK_MSG_ID_CAMERA_IMAGE_CAPTURED_LEN + MAVLINK_NUM_NON_PAYLOAD_BYTES : 0;
    }

private:
    explicit MavlinkStreamCameraImageCaptured(Mavlink *mavlink) :
        MavlinkStream(mavlink) {
    }

    uORB::Subscription _capture_sub{ORB_ID(camera_capture)};

    bool send() override {
        camera_capture_s capture;

        if ((_mavlink->get_free_tx_buf() >= get_size()) && _capture_sub.update(&capture)) {
            mavlink_camera_image_captured_t msg{};

            msg.time_boot_ms   = capture.timestamp / 1000;
            msg.time_utc       = capture.timestamp_utc;
            msg.camera_id      = 1; // FIXME : get this from uORB
            msg.lat            = capture.lat * 1e7;
            msg.lon            = capture.lon * 1e7;
            msg.alt            = capture.alt * 1e3f;
            msg.relative_alt   = capture.ground_distance * 1e3f;
            msg.q[0]           = capture.q[0];
            msg.q[1]           = capture.q[1];
            msg.q[2]           = capture.q[2];
            msg.q[3]           = capture.q[3];
            msg.image_index    = capture.seq;
            msg.capture_result = capture.result;
            msg.file_url[0]    = '\0';

            mavlink_msg_camera_image_captured_send_struct(_mavlink->get_channel(), &msg);

            return true;
        }

        return false;
    }
};

#endif // CAMERA_IMAGE_CAPTURED_HPP
