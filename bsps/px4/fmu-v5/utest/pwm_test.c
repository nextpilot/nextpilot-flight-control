/******************************************************************
 *      _   __             __   ____   _  __        __
 *     / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *    /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *   / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 *  /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2023 NextPilot Development Team
 ******************************************************************/

#include "rtthread.h"
#include <rtdevice.h>

#define PWM_DEV_NAME    "pwm1" /* PWM设备名称 */
#define PWM_DEV_CHANNEL 4      /* PWM通道 */
struct rt_device_pwm *pwm_dev; /* PWM设备句柄 */

static int pwm_sample(int argc, char *argv[]) {
    struct {
        char *fmu_name;
        char *dev_name;
        int   chan;
    } fmu_pwm_list[8] = {
        {"FMU_CH1", "pwm1", 4}, {"FMU_CH2", "pwm1", 3}, {"FMU_CH3", "pwm1", 2}, {"FMU_CH4", "pwm1", 1}, {"FMU_CH5", "pwm4", 2}, {"FMU_CH6", "pwm4", 3}, {"FMU_CH7", "pwm12", 1}, {"FMU_CH8", "pwm12", 2}};

    rt_uint32_t period, pulse, dir;
    period = 500000; /* 周期为0.5ms，单位为纳秒ns */
    dir    = 1;      /* PWM脉冲宽度值的增减方向 */
    pulse  = 0;      /* PWM脉冲宽度值，单位为纳秒ns */

    while (1) {
        rt_thread_mdelay(50);
        for (int i = 0; i < sizeof(fmu_pwm_list) / sizeof(fmu_pwm_list[0]); i++) {
            period = 10000000; /* 周期为0.5ms，单位为纳秒ns */
            dir    = 1;        /* PWM脉冲宽度值的增减方向 */
            pulse  = 0;        /* PWM脉冲宽度值，单位为纳秒ns */

            /* 查找设备 */
            pwm_dev = (struct rt_device_pwm *)rt_device_find(fmu_pwm_list[i].dev_name);
            if (pwm_dev == RT_NULL) {
                rt_kprintf("pwm sample run failed! can't find %s device!\n", fmu_pwm_list[i].dev_name);
                return RT_ERROR;
            }
            /* 设置PWM周期和脉冲宽度默认值 */
            rt_pwm_set(pwm_dev, fmu_pwm_list[i].chan, period, pulse);
            /* 使能设备 */
            rt_pwm_enable(pwm_dev, fmu_pwm_list[i].chan);
            while (1) {
                rt_thread_mdelay(50);
                if (dir) {
                    pulse += 50000; /* 从0值开始每次增加5000ns */
                } else {
                    pulse -= 50000; /* 从最大值开始每次减少5000ns */
                }
                if (pulse >= period) {
                    dir = 0;
                }
                if (0 == pulse) {
                    dir = 1;
                    rt_pwm_disable(pwm_dev, fmu_pwm_list[i].chan);
                    break;
                }
                /* 设置PWM周期和脉冲宽度 */

                rt_pwm_set(pwm_dev, fmu_pwm_list[i].chan, period, pulse);
            }
        }
    }
}
/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(pwm_sample, pwm sample);

static int pwm_test(int argc, char *argv[]) {
    rt_uint32_t period, pulse, dir, res;
    int         num = 10, close_chan = 0;
    period = 10000000; /* 周期为0.5ms，单位为纳秒ns */
    dir    = 1;        /* PWM脉冲宽度值的增减方向 */
    pulse  = 5000000;  /* PWM脉冲宽度值，单位为纳秒ns */

    struct {
        char *fmu_name;
        char *dev_name;
        int   chan;
    } fmu_pwm_list[8] = {
        {"FMU_CH1", "pwm1", 4}, {"FMU_CH2", "pwm1", 3}, {"FMU_CH3", "pwm1", 2}, {"FMU_CH4", "pwm1", 1}, {"FMU_CH5", "pwm4", 2}, {"FMU_CH6", "pwm4", 3}, {"FMU_CH7", "pwm12", 1}, {"FMU_CH8", "pwm12", 2}};
    /* If the number of arguments less than 2 */
    if (argc < 2) {
        rt_kprintf("\n");
        rt_kprintf("FMU_CHx[1-8]  period[10K-50M] duty_cycle[period%(10-90)] \n");
        rt_kprintf("FMU_CH_ALL    period[10K-50M] duty_cycle[period%(10-90)]\n");
        rt_kprintf("FMU_CLOSE     chan[1-8]\n");
        return 0;
    }
    if (argc >= 3) {
        if (!rt_strcmp(argv[1], "FMU_CH1")) num = 0;
        if (!rt_strcmp(argv[1], "FMU_CH2")) num = 1;
        if (!rt_strcmp(argv[1], "FMU_CH3")) num = 2;
        if (!rt_strcmp(argv[1], "FMU_CH4")) num = 3;
        if (!rt_strcmp(argv[1], "FMU_CH5")) num = 4;
        if (!rt_strcmp(argv[1], "FMU_CH6")) num = 5;
        if (!rt_strcmp(argv[1], "FMU_CH7")) num = 6;
        if (!rt_strcmp(argv[1], "FMU_CH8")) num = 7;
        if (num <= 7) {
            pwm_dev = (struct rt_device_pwm *)rt_device_find(fmu_pwm_list[num].dev_name);
            if (pwm_dev == RT_NULL) {
                rt_kprintf("pwm sample run failed! can't find %s device!\n", fmu_pwm_list[num].dev_name);
                return RT_ERROR;
            }
            /* 使能设备 */
            rt_pwm_enable(pwm_dev, fmu_pwm_list[num].chan);
            /* 设置PWM周期和脉冲宽度默认值 */
            rt_pwm_set(pwm_dev, fmu_pwm_list[num].chan, period, pulse);
            if (argc == 4) {
                period = atoi(argv[2]);
                pulse  = atoi(argv[3]);
            }
            while (1) {
                rt_pwm_set(pwm_dev, fmu_pwm_list[num].chan, period, pulse); //
                num = 10;
                break;
            }
        }
        if (!rt_strcmp(argv[1], "FMU_CH_ALL")) {
            for (int i = 0; i < sizeof(fmu_pwm_list) / sizeof(fmu_pwm_list[0]); i++) {
                pwm_dev = (struct rt_device_pwm *)rt_device_find(fmu_pwm_list[i].dev_name);
                if (pwm_dev == RT_NULL) {
                    rt_kprintf("pwm sample run failed! can't find %s device!\n", fmu_pwm_list[i].dev_name);
                    return RT_ERROR;
                }
                /* 使能设备 */
                rt_pwm_enable(pwm_dev, fmu_pwm_list[i].chan);
                /* 设置PWM周期和脉冲宽度默认值 */
                rt_pwm_set(pwm_dev, fmu_pwm_list[i].chan, period, pulse); //

                if (argc == 4) {
                    period = atoi(argv[2]);
                    pulse  = atoi(argv[3]);
                }
                while (1) {
                    rt_pwm_set(pwm_dev, fmu_pwm_list[i].chan, period, pulse); //
                    break;
                }
            }
        }
        if (!rt_strcmp(argv[1], "FMU_CLOSE")) {
            close_chan = atoi(argv[2]) - 1;
            pwm_dev    = (struct rt_device_pwm *)rt_device_find(fmu_pwm_list[close_chan].dev_name);
            if (pwm_dev == RT_NULL) {
                rt_kprintf("pwm sample run failed! can't find %s device!\n", fmu_pwm_list[num].dev_name);
                return RT_ERROR;
            }
            close_chan = atoi(argv[2]) - 1;
            rt_pwm_disable(pwm_dev, fmu_pwm_list[close_chan].chan);
        }
    } else {
        rt_kprintf("Unknown command, please enter 'pwm_test' get help information!\n");
    }
    return 0;
}
/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(pwm_test, pwm sample);
