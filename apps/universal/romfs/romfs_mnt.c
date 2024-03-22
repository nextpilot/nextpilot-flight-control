/******************************************************************
 *      _   __             __   ____   _  __        __
 *     / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *    /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *   / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 *  /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2023 NextPilot Development Team
 ******************************************************************/

#define LOG_TAG "romfs"
#define LOG_LVL LOG_LVL_INFO

#include <rtthread.h>
#include <rtdbg.h>

#ifdef RT_USING_DFS_ROMFS
#include <dfs_romfs.h>
#include <dfs_file.h>

int rt_bsp_romfs_mount() {
    if (dfs_mount(RT_NULL, "/", "rom", 0, &(romfs_root)) == 0) {
        LOG_I("mount romfs to '/'");
        return RT_EOK;
    } else {
        LOG_E("mount romfs failed");
        return -RT_ERROR;
    }
}

// INIT_ENV_EXPORT(rt_bsp_romfs_mount);
INIT_EXPORT(rt_bsp_romfs_mount, "4.1");

#endif // RT_USING_DFS_ROMFS