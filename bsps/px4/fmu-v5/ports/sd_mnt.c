/******************************************************************
 *      _   __             __   ____   _  __        __
 *     / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *    /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *   / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 *  /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2023 NextPilot Development Team
 ******************************************************************/

#define LOG_TAG "board"
#define LOG_LVL LOG_LVL_INFO

#include <rtthread.h>
#include <rtdbg.h>
#include <dfs_fs.h>
#include <dfs_elm.h>
#include <unistd.h>

#define SDCARD_DEVICE_NAME "sd0"

#ifdef SYS_USING_ROMFS
#define SDCARD_MOUNT_POINT "/mnt/microsd"
#else
#define SDCARD_MOUNT_POINT "/"
#endif // SYS_USING_ROMFS

#ifdef RT_USING_SDIO
static int rt_bsp_microsd_mount() {
    // 最多尝试3次
    for (int attempt = 0; attempt < 3; attempt++) {
        if (rt_device_find(SDCARD_DEVICE_NAME) != RT_NULL) {
            if (dfs_mount(SDCARD_DEVICE_NAME, SDCARD_MOUNT_POINT, "elm", 0, 0) == RT_EOK) {
                LOG_I("mount microsd to '%s'", SDCARD_MOUNT_POINT);
                return RT_EOK;
            } else if (dfs_mkfs("elm", SDCARD_DEVICE_NAME) == RT_EOK) {
                LOG_I("mkfs microsd ok");
            } else {
                LOG_W("mount/mkfs microsd fail, try again");
            }
        } else {
            // LOG_D("can't find microsd device, try again");
        }
        // usleep(500000);
        rt_thread_mdelay(500);
    }
    LOG_E("mount microsd fail");
    return -RT_ERROR;
}
INIT_ENV_EXPORT(rt_bsp_microsd_mount);
#endif // RT_USING_SDIO
