/******************************************************************
 *      _   __             __   ____   _  __        __
 *     / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *    /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *   / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 *  /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2023 NextPilot Development Team
 *******************************************************************/
#define LOG_TAG "version"
#define LOG_LVL LOG_LVL_INFO

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <rtdbg.h>
#include "board_version.h"

int show_version_info(void) {
    LOG_RAW("\n");
    LOG_RAW("=================================================================\n");
    LOG_RAW("         _   __             __   ____   _  __        __ \n");
    LOG_RAW("        / | / /___   _  __ / /_ / __ \\ (_)/ /____   / /_\n");
    LOG_RAW("       /  |/ // _ \\ | |/_// __// /_/ // // // __ \\ / __/\n");
    LOG_RAW("      / /|  //  __/_>  < / /_ / ____// // // /_/ // /_  \n");
    LOG_RAW("     /_/ |_/ \\___//_/|_| \\__//_/    /_//_/ \\____/ \\__/ \n");
    LOG_RAW("\n");
    const char *build_date = __DATE__;
    const char *build_year = build_date + 7;
    LOG_RAW(" Copyright All Reserved (C) 2015-%s NextPilot Development Team\n", build_year);
    LOG_RAW("=================================================================\n");

    // 硬件版本信息
    LOG_RAW("HW ARCH: %s(ver %d, rev %d)\n", get_board_name(), board_get_hw_version(), board_get_hw_revision());
    mcu_info_t info;
    if (board_get_mcu_info(&info) == 0) {
        LOG_RAW("MCU IDC: %s(dev 0x%X, rev 0x%X, flash %dkB)\n", info.name, info.devid, info.revid, info.flash_size);
        LOG_RAW("MCU UID: 0x%X%X%X\n", info.guid[2], info.guid[1], info.guid[0]);
    } else {
        LOG_RAW("MCU IDC: %s\n", "Unkown");
        LOG_RAW("MCU UID: %s\n", "Unkown");
    }
    // 软件版本信息
    LOG_RAW("FW  TAG: %s\n", FW_GIT_TAG_STR);
    LOG_RAW("FW HASH: %s(%s)\n", get_firmware_version_string(), FW_GIT_BRANCH_STR);
    // 系统版本信息
    uint32_t fwver = get_os_version();
    uint32_t major = (fwver >> (8 * 3)) & 0xFF;
    uint32_t minor = (fwver >> (8 * 2)) & 0xFF;
    uint32_t patch = (fwver >> (8 * 1)) & 0xFF;
    // uint32_t type  = (fwver >> (8 * 0)) & 0xFF;
    LOG_RAW("OS  VER: %s V%d.%d.%d\n", get_os_name(), major, minor, patch);
    LOG_RAW("OS HASH: %s(%s)\n", get_os_version_string(), OS_GIT_BRANCH_STR);
    // 工具版本信息
    LOG_RAW("Toolchain: %s %s\n", get_toolchain_name(), get_toolchain_version());
    LOG_RAW("Build  URI: %s\n", get_build_uri());
    LOG_RAW("Build TIME: %s %s\n", __DATE__, __TIME__);
    LOG_RAW("--------------------------------------------------------------\n");
    // 强制先输出
    ulog_flush();
    return 0;
}
INIT_EXPORT(show_version_info, "3.1");

static int version_main(int argc, char *argv[]) {
    return show_version_info();
}
MSH_CMD_EXPORT_ALIAS(version_main, ver, version);
