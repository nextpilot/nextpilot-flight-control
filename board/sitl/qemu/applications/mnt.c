/******************************************************************
 *      _   __             __   ____   _  __        __
 *     / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *    /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *   / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 *  /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2023 NextPilot Development Team
 ******************************************************************/

#define LOG_TAG "SDMNT"
#define LOG_LVL LOG_LVL_INFO

#include <rtthread.h>
#include <rtdbg.h>

#ifdef RT_USING_DFS
#include <dfs_fs.h>
#include <unistd.h>

#ifdef RT_USING_DFS_ROMFS
#define SDCARD_MOUNT_POINT "/mnt/microsd"
#else
#define SDCARD_MOUNT_POINT "/"
#endif // RT_USING_DFS_ROMFS

#define SDCARD_DEVICE_NAME "sd0"

int mnt_init(void) {
    // 查找sd
    int       found = 0;
    rt_tick_t start = rt_tick_get();
    while (rt_tick_get() - start < 1000) {
        if (rt_device_find(SDCARD_DEVICE_NAME)) {
            found = 1;
            break;
        }
        rt_thread_mdelay(10);
    }
    if (!found) {
        LOG_E("can't find %s", SDCARD_DEVICE_NAME);
        return -RT_ERROR;
    }

    // 挂载sd
    if (dfs_mount(SDCARD_DEVICE_NAME, SDCARD_MOUNT_POINT, "elm", 0, 0) == RT_EOK) {
        LOG_I("mount SD to %s", SDCARD_MOUNT_POINT);
        goto __mkdir;
    }

    // 格式化sd
    if (dfs_mkfs("elm", SDCARD_DEVICE_NAME) != RT_EOK) {
        LOG_E("mkfs SD fail");
        return -RT_ERROR;
    }

    // 再次挂载
    if (dfs_mount(SDCARD_DEVICE_NAME, SDCARD_MOUNT_POINT, "elm", 0, 0) == RT_EOK) {
        LOG_I("mount SD to %s", SDCARD_MOUNT_POINT);
        goto __mkdir;
    }

    LOG_E("mount SD fail");
    return -RT_ERROR;

__mkdir:
    // 创建etc目录
    if (access(SDCARD_MOUNT_POINT "/etc", 0) < 0) {
        mkdir(SDCARD_MOUNT_POINT "/etc", 0);
    }

    // 创建log目录
    if (access(SDCARD_MOUNT_POINT "/log", 0) < 0) {
        mkdir(SDCARD_MOUNT_POINT "/log", 0);
    }
    return RT_EOK;
}
INIT_ENV_EXPORT(mnt_init);
#endif // RT_USING_DFS
