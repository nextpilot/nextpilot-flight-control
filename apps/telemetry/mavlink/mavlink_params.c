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
 * MAVLink system ID
 * @group MAVLink
 * @min 1
 * @max 250
 * @reboot_required true
 */
PARAM_DEFINE_INT32(MAV_SYS_ID, 1);

/**
 * MAVLink component ID
 * @group MAVLink
 * @min 1
 * @max 250
 * @reboot_required true
 */
PARAM_DEFINE_INT32(MAV_COMP_ID, 1);

/**
 * MAVLink protocol version
 * @group MAVLink
 * @value 0 Default to 1, switch to 2 if GCS sends version 2
 * @value 1 Always use version 1
 * @value 2 Always use version 2
 */
PARAM_DEFINE_INT32(MAV_PROTO_VER, 0);

/**
 * MAVLink SiK Radio ID
 *
 * When non-zero the MAVLink app will attempt to configure the
 * SiK radio to this ID and re-set the parameter to 0. If the value
 * is negative it will reset the complete radio config to
 * factory defaults. Only applies if this mavlink instance is going through a SiK radio
 *
 * @group MAVLink
 * @min -1
 * @max 240
 */
PARAM_DEFINE_INT32(MAV_SIK_RADIO_ID, 0);

/**
 * MAVLink airframe type
 *
 * @min 0
 * @max 22
 * @value 0 Generic micro air vehicle
 * @value 1 Fixed wing aircraft
 * @value 2 Quadrotor
 * @value 3 Coaxial helicopter
 * @value 4 Normal helicopter with tail rotor
 * @value 7 Airship, controlled
 * @value 8 Free balloon, uncontrolled
 * @value 10 Ground rover
 * @value 11 Surface vessel, boat, ship
 * @value 12 Submarine
 * @value 13 Hexarotor
 * @value 14 Octorotor
 * @value 15 Tricopter
 * @value 19 VTOL Two-rotor Tailsitter
 * @value 20 VTOL Quad-rotor Tailsitter
 * @value 21 VTOL Tiltrotor
 * @value 22 VTOL Standard (separate fixed rotors for hover and cruise flight)
 * @value 23 VTOL Tailsitter
 * @group MAVLink
 */
PARAM_DEFINE_INT32(MAV_TYPE, 0);

/**
 * Use/Accept HIL GPS message even if not in HIL mode
 *
 * If set to 1 incoming HIL GPS messages are parsed.
 *
 * @boolean
 * @group MAVLink
 */
PARAM_DEFINE_INT32(MAV_USEHILGPS, 0);

/**
 * Forward external setpoint messages
 *
 * If set to 1 incoming external setpoint messages will be directly forwarded
 * to the controllers if in offboard control mode
 *
 * @boolean
 * @group MAVLink
 */
PARAM_DEFINE_INT32(MAV_FWDEXTSP, 1);

/**
 * Parameter hash check.
 *
 * Disabling the parameter hash check functionality will make the mavlink instance
 * stream parameters continuously.
 *
 * @boolean
 * @group MAVLink
 */
PARAM_DEFINE_INT32(MAV_HASH_CHK_EN, 1);

/**
 * Heartbeat message forwarding.
 *
 * The mavlink heartbeat message will not be forwarded if this parameter is set to 'disabled'.
 * The main reason for disabling heartbeats to be forwarded is because they confuse dronekit.
 *
 * @boolean
 * @group MAVLink
 */
PARAM_DEFINE_INT32(MAV_HB_FORW_EN, 1);

/**
 * Timeout in seconds for the RADIO_STATUS reports coming in
 *
 * If the connected radio stops reporting RADIO_STATUS for a certain time,
 * a warning is triggered and, if MAV_X_RADIO_CTL is enabled, the software-flow
 * control is reset.
 *
 * @group MAVLink
 * @unit s
 * @min 1
 * @max 250
 */
PARAM_DEFINE_INT32(MAV_RADIO_TOUT, 5);
