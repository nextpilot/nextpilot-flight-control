/******************************************************************
 *      _   __             __   ____   _  __        __
 *     / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *    /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *   / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 *  /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

#define LOG_TAG "board"
#define LOG_LVL LOG_LVL_INFO

#include <rtthread.h>
#include <rtdbg.h>
#include <dfs_fs.h>
#include <dfs_elm.h>
#include <unistd.h>

#ifndef SDIO_DEVICE_NAME
#   define SDIO_DEVICE_NAME "sd0"
#endif // SDIO_DEVICE_NAME

#ifndef SDIO_MOUNT_POINT
#   ifdef RT_USING_ROMFS
#      define SDIO_MOUNT_POINT "/mnt/microsd"
#   else
#      define SDIO_MOUNT_POINT "/"
#   endif // RT_USING_ROMFS
#endif    //SDIO_MOUNT_POINT

#ifdef BSP_USING_SDIO
static int rt_bsp_microsd_mount() {
    int       found = 0;
    rt_tick_t start = rt_tick_get();
    while (rt_tick_get() - start < 3000) {
        if (rt_device_find(SDIO_DEVICE_NAME)) {
            found = 1;
            break;
        }
        rt_thread_mdelay(10);
    }

    if (!found) {
        LOG_E("can't find %s", SDIO_DEVICE_NAME);
        return -RT_ERROR;
    }

    // 挂载sd，挂载失败则格式化
    if (dfs_mount(SDIO_DEVICE_NAME, SDIO_MOUNT_POINT, "elm", 0, 0) != RT_EOK) {
        // 格式化sd
        if (dfs_mkfs("elm", SDIO_DEVICE_NAME) != RT_EOK) {
            LOG_E("mkfs SD fail");
            return -RT_ERROR;
        }

        // 再次挂载
        if (dfs_mount(SDIO_DEVICE_NAME, SDIO_MOUNT_POINT, "elm", 0, 0) != RT_EOK) {
            LOG_I("mount SD fail", SDIO_MOUNT_POINT);
            return -RT_ERROR;
        }
    }


    // 创建etc目录和log目录
    if (access(SDIO_MOUNT_POINT "/etc", 0) < 0) {
        mkdir(SDIO_MOUNT_POINT "/etc", 0);
    }
    if (access(SDIO_MOUNT_POINT "/log", 0) < 0) {
        mkdir(SDIO_MOUNT_POINT "/log", 0);
    }

    LOG_E("microsd init ok");
    return RT_EOK;
}

INIT_ENV_EXPORT(rt_bsp_microsd_mount);
#endif // BSP_USING_SDIO
