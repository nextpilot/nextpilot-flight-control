/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#pragma once

#include <stddef.h>
#include <stdint.h>
#include <atomic/atomic.hpp>
#include <vconsole/vconsole.h>

class Mavlink;

class MavlinkShell {
public:
    MavlinkShell(Mavlink *mavlink) :
        _mavlink(mavlink) {

        };

    ~MavlinkShell();

    /**
     * Start the mavlink shell.
     *
     * @return		0 on success.
     */
    int start();

    int stop();

    /**
     * Write to the shell
     * @return number of written bytes
     */
    size_t write(uint8_t *buffer, size_t len);

    /**
     * Read from the shell. This is blocking, if 0 bytes are available, this will block.
     * @param len buffer length
     * @return number of bytes read.
     */
    size_t read(uint8_t *buffer, size_t len);

    /**
     * Get the number of bytes that can be read.
     */
    size_t available();

    void setTargetID(uint8_t sysid, uint8_t compid) {
        _target_sysid.store(sysid);
        _target_compid.store(compid);
    }

    uint8_t targetSysid() const {
        return _target_sysid.load();
    }

    uint8_t targetCompid() const {
        return _target_compid.load();
    }

private:
    Mavlink *_mavlink;

    atomic<uint8_t> _target_sysid{};
    atomic<uint8_t> _target_compid{};


    char                       _name[RT_NAME_MAX];
    struct rt_vconsole_device *_vconsole{nullptr};
    rt_device_t                _old_console{nullptr};

    /* do not allow copying this class */
    MavlinkShell(const MavlinkShell &)           = delete;
    MavlinkShell operator=(const MavlinkShell &) = delete;
};
