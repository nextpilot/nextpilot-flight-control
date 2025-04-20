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
 * @file camera_interface.h
 */

#pragma once

#include <param/param.h>
#include <px4_platform_common/log.h>

#define arraySize(a) (sizeof((a)) / sizeof(((a)[0])))

class CameraInterface {
public:
    CameraInterface()          = default;
    virtual ~CameraInterface() = default;

    /**
	 * trigger the camera
	 * @param enable
	 */
    virtual void trigger(bool trigger_on_true) {
    }

    /**
	 * send command to turn the camera on/off
	 * @param enable
	 */
    virtual void send_toggle_power(bool enable) {
    }

    /**
	 * send command to prevent the camera from sleeping
	 * @param enable
	 */
    virtual void send_keep_alive(bool enable) {
    }

    /**
	 * Display info.
	 */
    virtual void info() {
    }

    /**
	 * Checks if the interface has support for
	 * camera power control
	 * @return true if power control is supported
	 */
    virtual bool has_power_control() {
        return false;
    }

    /**
	 * Checks if the camera connected to the interface
	 * is turned on.
	 * @return true if camera is on
	 */
    virtual bool is_powered_on() {
        return true;
    }

protected:
    /**
	 * setup the interface
	 */
    virtual void setup() {
    }

    /**
	 * get the hardware configuration
	 */
    void get_pins();

    int _pins[32]{};
};
