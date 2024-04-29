/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#include <hrtimer.h>

#include <uORB/Publication.hpp>
#include <uORB/topics/gripper.h>

using namespace time_literals;

/**
 * Griper class to handle functionality of using a Gripper
 */

typedef struct GripperConfig {
    enum class GripperType {
        UNDEFINED = -1,
        SERVO     = 0,
    };

    GripperType type{GripperType::UNDEFINED};

    // Gripper state feedback sensor type
    enum class GripperSensorType {
        NONE = -1,
        ENCODER,
    };

    GripperSensorType sensor{GripperSensorType::NONE};

    hrt_abstime timeout_us{0};

} GripperConfig;

enum class GripperState {
    IDLE = 0,
    GRABBING,
    GRABBED,
    RELEASING,
    RELEASED
};

class Gripper {
public:
    Gripper() = default;

    // Initialize the gripper
    void init(const GripperConfig &config);

    // De-initialize the gripper
    void deinit();

    // Actuate the gripper to the grab position
    void grab();

    // Actuate the gripper to release position
    void release();

    // Update gripper state
    void update();

    // Return griper's state in string format
    const char *get_state_str() const;

    // Returns true if in grabbed position either sensed or timeout based
    bool grabbed() {
        return _state == GripperState::GRABBED;
    }

    // Returns true if in grabbing position either sensed or timeout based
    bool grabbing() {
        return _state == GripperState::GRABBING;
    }

    // Returns true if in released position either sensed or timeout based
    bool released() {
        return _state == GripperState::RELEASED;
    }

    /**
     * @brief Returns true once after gripper succeededs in releasing the payload
     *
     * Note, that this is a single-read for a one successful release operation. Meaning that
     * if you command `release()` and then call this function multiple times in the future,
     * it will only return `true` only once, and then it will return `false` after it returns
     * `true` once.
     *
     * This is used to detect a single event of successful releasing of a gripper in `payload_deliverer`
     * module, which then would send `vehicle_command_ack` message, indicating the successful release.
     */
    bool released_read_once() {
        if (_released_state_cache) {
            _released_state_cache = false;
            return true;

        } else {
            return false;
        }
    }

    /**
     * @brief Returns true once after gripper succeededs in grabbing the payload
     */
    bool grabbed_read_once() {
        if (_grabbed_state_cache) {
            _grabbed_state_cache = false;
            return true;

        } else {
            return false;
        }
    }

    // Returns true if Gripper output function is assigned properly
    bool is_valid() const {
        return _valid;
    }

private:
    // Internal function to send gripper command via uORB
    void publish_gripper_command(const int8_t gripper_command);

    // Internal settings
    bool        _valid{false};
    bool        _has_feedback_sensor{false};
    hrt_abstime _timeout_us{0};

    // Internal state
    GripperState _state{GripperState::IDLE};
    hrt_abstime  _last_command_time{0};

    // Cached flag that is set once gripper release/grab was successful, it must get reset when read.
    bool _released_state_cache{false};
    bool _grabbed_state_cache{false};

    uORB::Publication<gripper_s> _gripper_pub{ORB_ID(gripper)};
};
