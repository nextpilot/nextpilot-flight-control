/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/


#define LOG_TAG "mavlink.shell"
#define LOG_LVL LOG_LVL_INFO

#include "mavlink_main.h"
#include "mavlink_shell.h"
#include <defines.h>
#include <ulog/log.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>

MavlinkShell::~MavlinkShell() {
    stop();
}

int MavlinkShell::start() {
    // 创建一个虚拟串口
    snprintf(_name, RT_NAME_MAX, "mavlink%d_shell", _mavlink->get_instance_id());
    _vconsole = vconsole_create(_name, 128, 2048, RT_DEVICE_FLAG_RDWR);
    if (!_vconsole) {
        LOG_E("create vconsole fail");
        return -RT_ERROR;
    }
    // 将终端切换到虚拟串口
    _old_console = rt_console_change_device(_name);
    // 判断是否切换成功
    if (rt_console_get_device() == (rt_device_t)_vconsole) {
        LOG_I("change console device to %s", _name);
        return 0;
    }

    return -RT_ERROR;
}

int MavlinkShell::stop() {
#ifdef RT_CONSOLE_DEVICE_NAME
    rt_console_change_device(RT_CONSOLE_DEVICE_NAME);
#endif // RT_CONSOLE_DEVICE_NAME

    if (_vconsole) {
        vconsole_delete(_vconsole);
    }

    return RT_EOK;
}

size_t MavlinkShell::write(uint8_t *buffer, size_t len) {
    buffer[len] = '\0';
    return vconsole_input(_vconsole, buffer, len);
}

size_t MavlinkShell::read(uint8_t *buffer, size_t len) {
    return vconsole_output(_vconsole, buffer, len);
}

size_t MavlinkShell::available() {
    return rt_ringbuffer_data_len(_vconsole->tx_buff);
}
