/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#ifndef EXTENDED_SYS_STATE_HPP
#define EXTENDED_SYS_STATE_HPP

#include <uORB/topics/position_setpoint_triplet.h>
#include <uORB/topics/vehicle_control_mode.h>
#include <uORB/topics/vehicle_status.h>
#include <uORB/topics/vehicle_land_detected.h>

class MavlinkStreamExtendedSysState : public MavlinkStream {
public:
    static MavlinkStream *new_instance(Mavlink *mavlink) {
        return new MavlinkStreamExtendedSysState(mavlink);
    }

    static constexpr const char *get_name_static() {
        return "EXTENDED_SYS_STATE";
    }
    static constexpr uint16_t get_id_static() {
        return MAVLINK_MSG_ID_EXTENDED_SYS_STATE;
    }

    const char *get_name() const override {
        return get_name_static();
    }
    uint16_t get_id() override {
        return get_id_static();
    }

    unsigned get_size() override {
        return MAVLINK_MSG_ID_EXTENDED_SYS_STATE_LEN + MAVLINK_NUM_NON_PAYLOAD_BYTES;
    }

private:
    explicit MavlinkStreamExtendedSysState(Mavlink *mavlink) :
        MavlinkStream(mavlink) {
        _msg.vtol_state   = MAV_VTOL_STATE_UNDEFINED;
        _msg.landed_state = MAV_LANDED_STATE_ON_GROUND;
    }

    uORB::Subscription           _status_sub{ORB_ID(vehicle_status)};
    uORB::Subscription           _landed_sub{ORB_ID(vehicle_land_detected)};
    uORB::Subscription           _pos_sp_triplet_sub{ORB_ID(position_setpoint_triplet)};
    uORB::Subscription           _control_mode_sub{ORB_ID(vehicle_control_mode)};
    mavlink_extended_sys_state_t _msg{};

    bool send() override {
        bool updated = false;

        vehicle_status_s status;

        if (_status_sub.copy(&status)) {
            updated = true;

            if (status.is_vtol) {
                if (!status.in_transition_mode && status.vehicle_type == vehicle_status_s::VEHICLE_TYPE_ROTARY_WING) {
                    _msg.vtol_state = MAV_VTOL_STATE_MC;

                } else if (!status.in_transition_mode) {
                    _msg.vtol_state = MAV_VTOL_STATE_FW;

                } else if (status.in_transition_mode && status.in_transition_to_fw) {
                    _msg.vtol_state = MAV_VTOL_STATE_TRANSITION_TO_FW;

                } else if (status.in_transition_mode) {
                    _msg.vtol_state = MAV_VTOL_STATE_TRANSITION_TO_MC;
                }
            }
        }

        vehicle_land_detected_s land_detected;

        if (_landed_sub.copy(&land_detected)) {
            updated = true;

            if (land_detected.landed) {
                _msg.landed_state = MAV_LANDED_STATE_ON_GROUND;

            } else if (!land_detected.landed) {
                _msg.landed_state = MAV_LANDED_STATE_IN_AIR;

                vehicle_control_mode_s      control_mode;
                position_setpoint_triplet_s pos_sp_triplet;

                if (_control_mode_sub.copy(&control_mode) && _pos_sp_triplet_sub.copy(&pos_sp_triplet)) {
                    if (control_mode.flag_control_auto_enabled && pos_sp_triplet.current.valid) {
                        if (pos_sp_triplet.current.type == position_setpoint_s::SETPOINT_TYPE_TAKEOFF) {
                            _msg.landed_state = MAV_LANDED_STATE_TAKEOFF;

                        } else if (pos_sp_triplet.current.type == position_setpoint_s::SETPOINT_TYPE_LAND) {
                            _msg.landed_state = MAV_LANDED_STATE_LANDING;
                        }
                    }
                }
            }
        }

        if (updated) {
            mavlink_msg_extended_sys_state_send_struct(_mavlink->get_channel(), &_msg);
        }

        return updated;
    }
};

#endif // EXTENDED_SYS_STATE_HPP
