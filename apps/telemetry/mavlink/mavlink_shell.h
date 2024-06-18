/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/


#include <stddef.h>
#include <stdint.h>
#include <atomic/atomic.hpp>

#pragma once

class MavlinkShell {
public:
    MavlinkShell() = default;

    ~MavlinkShell();

    /**
     * Start the mavlink shell.
     *
     * @return		0 on success.
     */
    int start();

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
    atomic<uint8_t> _target_sysid{};
    atomic<uint8_t> _target_compid{};

    int        _to_shell_fd   = -1;       /** fd to write to the shell */
    int        _from_shell_fd = -1;       /** fd to read from the shell */
    int        _shell_fds[2]  = {-1, -1}; /** stdin & out used by the shell */
    px4_task_t _task;

    static int shell_start_thread(int argc, char *argv[]);

    /* do not allow copying this class */
    MavlinkShell(const MavlinkShell &)           = delete;
    MavlinkShell operator=(const MavlinkShell &) = delete;
};
