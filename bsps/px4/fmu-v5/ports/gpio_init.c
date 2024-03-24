/******************************************************************
 *      _   __             __   ____   _  __        __
 *     / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *    /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *   / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 *  /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2023 NextPilot Development Team
 ******************************************************************/

#include "drv_gpio.h"
#include "rtthread.h"

static int rt_bsp_gpio_init() {
    rt_pin_mode(GET_PIN(E, 3), PIN_MODE_OUTPUT); // 使能传感器电源
    rt_pin_write(GET_PIN(E, 3), PIN_HIGH);

    rt_pin_mode(GET_PIN(F, 12), PIN_MODE_OUTPUT); // 使能外部5V输出电源
    rt_pin_write(GET_PIN(F, 12), PIN_HIGH);

    rt_pin_mode(GET_PIN(G, 7), PIN_MODE_OUTPUT); // 使能SD卡电源
    rt_pin_write(GET_PIN(G, 7), PIN_HIGH);

    return 0;
}
INIT_BOARD_EXPORT(rt_bsp_gpio_init);