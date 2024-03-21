/******************************************************************
 *      _   __             __   ____   _  __        __
 *     / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *    /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *   / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 *  /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2023 NextPilot Development Team
 ******************************************************************/

#include <rtthread.h>

#if defined(RT_USING_RTGUI) || defined(PKG_USING_GUIENGINE)

#include <rtgui/driver.h>
int lcd_init(void) {
    struct rt_device *device;
    device = rt_device_find("lcd");
    if (device) {
        rtgui_graphic_set_device(device);
    }

    return 0;
}
INIT_APP_EXPORT(lcd_init);

#endif
