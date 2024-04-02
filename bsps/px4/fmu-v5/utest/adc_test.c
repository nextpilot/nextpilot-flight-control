/******************************************************************
 *      _   __             __   ____   _  __        __
 *     / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *    /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *   / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 *  /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2024 NextPilot Development Team
 ******************************************************************/

/*
 * 程序清单： ADC 设备使用例程
 * 例程导出了 adc_sample 命令到控制终端
 * 命令调用格式：adc_sample
 * 程序功能：通过 ADC 设备采样电压值并转换为数值。
 *           示例代码参考电压为3.3V,分辨率为12位。
 */
#include <rtthread.h>
#include <rtdevice.h>
#include <string.h>

#define ADC_DEV_NAME    "adc1" /* ADC 设备名称 */
#define ADC_DEV_CHANNEL 14     /* ADC 通道 */
static int adc_vol_sample(int argc, char *argv[]) {
    rt_adc_device_t adc_dev;
    rt_uint32_t     value, vol;
    rt_err_t        ret = RT_EOK;

    /* 查找设备 */
    adc_dev = (rt_adc_device_t)rt_device_find(ADC_DEV_NAME);
    if (adc_dev == RT_NULL) {
        rt_kprintf("adc sample run failed! can't find %s device!\n", ADC_DEV_NAME);
        return RT_ERROR;
    }

    /* 使能设备 */
    ret = rt_adc_enable(adc_dev, ADC_DEV_CHANNEL);
    /* 读取采样值 */
    value = rt_adc_read(adc_dev, ADC_DEV_CHANNEL);
    rt_kprintf("the value is :%d \n", value);
    /* 转换为对应电压值,3.3V对应12位最大值4096,数据精度乘以100保留2位小数 */
    vol = value * 330 / 4096;
    rt_kprintf("the voltage is :%d.%02d \n", vol / 100, vol % 100);
    /* 关闭通道 */
    ret = rt_adc_disable(adc_dev, ADC_DEV_CHANNEL);
    return ret;
}
/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(adc_vol_sample, adc voltage convert sample);

static int adc_test(int argc, char *argv[]) {
    rt_adc_device_t adc_dev;
    rt_uint32_t     value, vol;
    rt_err_t        ret = RT_EOK;
    int             num = 6, close_chan = 0;
    struct {
        char *fmu_name;
        char *dev_name;
        int   chan;
    } fmu_adc_list[6] = {
        {"BAT1_V", "adc1", 0}, {"BAT1_I", "adc1", 1}, {"ADC1_SPARE_1", "adc1", 14}, {"ADC1_SPARE_2", "adc1", 4}, {"SCALED_V5", "adc1", 10}, {"SCALED_VDD_3V3_SENSORS", "adc1", 11}};
    /* If the number of arguments less than 2 */
    if (argc < 2) {
        rt_kprintf("\n");
        rt_kprintf("ADC_ALL       Open all adc \n");
        rt_kprintf("BAT1_V        The acquisition voltage is 0-60V \n");
        rt_kprintf("BAT1_I        The acquisition voltage is 0-60V\n");
        rt_kprintf("ADC1_SPARE_1  The acquisition voltage is 0-12V \n");
        rt_kprintf("ADC1_SPARE_2  The acquisition voltage is 0-12V\n");
        rt_kprintf("SCALED_V5     The acquisition voltage is 0-5V \n");
        rt_kprintf("SCALED_VDD_3V3_SENSORS   The acquisition voltage is 0-5V\n");
        rt_kprintf("ADC_CLOSE      Close all chan\n");
        return 0;
    }
    if (argc >= 2) {
        if (!strcmp(argv[1], "BAT1_V")) num = 0;
        if (!strcmp(argv[1], "BAT1_I")) num = 1;
        if (!strcmp(argv[1], "ADC1_SPARE_1")) num = 2;
        if (!strcmp(argv[1], "ADC1_SPARE_2")) num = 3;
        if (!strcmp(argv[1], "SCALED_V5")) num = 4;
        if (!strcmp(argv[1], "SCALED_VDD_3V3_SENSORS")) num = 5;
        if (num <= 5) {
            adc_dev = (rt_adc_device_t)rt_device_find(fmu_adc_list[num].dev_name);
            if (adc_dev == RT_NULL) {
                rt_kprintf("adc sample run failed! can't find %s device!\n", fmu_adc_list[num].dev_name);
                return RT_ERROR;
            }
            /* 使能设备 */
            ret = rt_adc_enable(adc_dev, fmu_adc_list[num].chan);
            /* 读取采样值 */
            value = rt_adc_read(adc_dev, fmu_adc_list[num].chan);
            rt_kprintf("the %s value is :%d \n", fmu_adc_list[num].fmu_name, value);
            if (num == 0 || num == 1) {
                /* 转换为对应电压值,3.3V对应12位最大值4096,数据精度乘以100保留2位小数；
                电压分压系数 = （300K + 15K）/15K = 21
                */
                vol = 21 * value * 330 / 4096;
                rt_kprintf("the voltage is :%d.%02dV\n", vol / 100, vol % 100);
            }
            if (num == 2 || num == 3) {
                /* 转换为对应电压值,3.3V对应12位最大值4096,数据精度乘以100保留2位小数；
                电压分压系数 = （30.1K + 10K）/10K = 4.01
                */
                vol = 4.01 * value * 330 / 4096;
                rt_kprintf("the voltage is :%d.%02dV \n", vol / 100, vol % 100);
            }
            if (num == 4 || num == 5) {
                /* 转换为对应电压值,3.3V对应12位最大值4096,数据精度乘以100保留2位小数；
                电压分压系数 = （10K + 10K）/10K = 2
                */
                vol = 2 * value * 330 / 4096;
                rt_kprintf("the voltage is :%d.%02dV \n", vol / 100, vol % 100);
            }
        }

        if (!strcmp(argv[1], "ADC_ALL")) {
            for (int i = 0; i < sizeof(fmu_adc_list) / sizeof(fmu_adc_list[0]); i++) {
                adc_dev = (rt_adc_device_t)rt_device_find(fmu_adc_list[i].dev_name);
                if (adc_dev == RT_NULL) {
                    rt_kprintf("adc sample run failed! can't find %s device!\n", fmu_adc_list[i].dev_name);
                    return RT_ERROR;
                }
                /* 使能设备 */
                ret = rt_adc_enable(adc_dev, fmu_adc_list[i].chan);
                /* 读取采样值 */
                value = rt_adc_read(adc_dev, fmu_adc_list[i].chan);
                rt_kprintf("the %s value is :%d \n", fmu_adc_list[i].fmu_name, value);
                if (i == 0 || i == 1) {
                    /* 转换为对应电压值,3.3V对应12位最大值4096,数据精度乘以100保留2位小数；
                    电压分压系数 = （300K + 15K）/15K = 21
                    */
                    vol = 21 * value * 330 / 4096;
                    rt_kprintf("the voltage is :%d.%02dV \n", vol / 100, vol % 100);
                }
                if (i == 2 || i == 3) {
                    /* 转换为对应电压值,3.3V对应12位最大值4096,数据精度乘以100保留2位小数；
                    电压分压系数 = （30.1K + 10K）/10K = 4.01
                    */
                    vol = 4.01 * value * 330 / 4096;
                    rt_kprintf("the voltage is :%d.%02dV \n", vol / 100, vol % 100);
                }
                if (i == 4 || i == 5) {
                    /* 转换为对应电压值,3.3V对应12位最大值4096,数据精度乘以100保留2位小数；
                    电压分压系数 = （10K + 10K）/10K = 2
                    */
                    vol = 2 * value * 330 / 4096;
                    rt_kprintf("the voltage is :%d.%02dV \n", vol / 100, vol % 100);
                }
            }
        }
        if (!strcmp(argv[1], "ADC_CLOSE")) {
            for (int i = 0; i < sizeof(fmu_adc_list) / sizeof(fmu_adc_list[0]); i++) {
                adc_dev = (rt_adc_device_t)rt_device_find(fmu_adc_list[i].dev_name);
                if (adc_dev == RT_NULL) {
                    rt_kprintf("adc sample run failed! can't find %s device!\n", fmu_adc_list[i].dev_name);
                    return RT_ERROR;
                }

                rt_adc_disable(adc_dev, fmu_adc_list[i].chan);
                rt_kprintf("adc %s closed \n", fmu_adc_list[i].fmu_name);
            }
        }
    }

    return 0;
}
/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(adc_test, adc voltage convert sample);
