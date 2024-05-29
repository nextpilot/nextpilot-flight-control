/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

// Global timestamp generator


#include "hrtimer.h"
#include "board.h"

#define TSG_CNTCR  *(volatile unsigned int *)0x5C005000 // bit0 用于使能
#define TSG_CNTCVL *(volatile unsigned int *)0x5C005008 // 低32bit
#define TSG_CNTCVU *(volatile unsigned int *)0x5C00500C // 高32bit

// TSG时间戳的频率为Debug APB的240MHz,即1s累加240M次,64位资源完全不怕溢出,该功能为H7的专用功能,F1/F4/F7不支持
uint64_t hrt_absolute_time(void) {
    static int _tsg_inited = 0;

    if (_tsg_inited == 0) {
        TSG_CNTCR   = 0x00000000; /* 禁止计数器，必须禁止状态才可以设置其它寄存器 */
        TSG_CNTCVL  = 0;          /* 清零低16bit，也可以初始时间戳 */
        TSG_CNTCVU  = 0;          /* 清零高16bit，也可以初始时间戳 */
        TSG_CNTCR   = 0x00000001; /* 使能计数器并递增计数 */
        _tsg_inited = 1;
    }

    return ((uint64_t)TSG_CNTCVL | ((uint64_t)TSG_CNTCVU << 32)) / 240; // 输出us
}
