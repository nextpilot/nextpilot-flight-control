
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
#include <rtdevice.h>
#include <string.h>
#include <stdio.h>

#define AHT10_I2C_BUS_NAME "i2c1"
static struct rt_i2c_bus_device *i2c_bus     = RT_NULL;  /* I2C?????? */
static rt_bool_t                 initialized = RT_FALSE; /* ???????? */
#define AT24CXX_MAX_MEM_ADDRESS 8192
struct {
    char *fmu_name;
    char *dev_name;
    int   i2c_addr;
} fmu_i2c_list[3] = {
    {"I2C1", "i2c1", 0x50},
    {"I2C2", "i2c2", 0x50},
    {"I2C3", "i2c3", 0x0e}};

static rt_err_t write_reg16(struct rt_i2c_bus_device *bus, rt_uint16_t reg, rt_uint8_t val, int i) {
    rt_size_t         ret;
    rt_uint8_t        buf[3];
    struct rt_i2c_msg msgs;

    buf[0] = (uint8_t)(reg >> 8);
    buf[1] = (uint8_t)reg;
    buf[2] = val;

    msgs.addr  = fmu_i2c_list[i].i2c_addr;
    msgs.flags = RT_I2C_WR;
    msgs.buf   = buf;
    msgs.len   = 3;

    ret = rt_i2c_transfer(bus, &msgs, 1);

    return ret == 1 ? RT_EOK : RT_ERROR;
}
static rt_err_t write_reg(struct rt_i2c_bus_device *bus, rt_uint8_t reg, rt_uint8_t val, int i) {
    rt_size_t         ret;
    rt_uint8_t        buf[2];
    struct rt_i2c_msg msgs;

    buf[0] = reg;
    buf[1] = val;

    msgs.addr  = fmu_i2c_list[i].i2c_addr;
    msgs.flags = RT_I2C_WR;
    msgs.buf   = buf;
    msgs.len   = 2;

    ret = rt_i2c_transfer(bus, &msgs, 1);

    return ret == 1 ? RT_EOK : RT_ERROR;
}
static rt_err_t read_reg16(struct rt_i2c_bus_device *bus, uint16_t reg, uint8_t *buf, int i) {
    rt_uint8_t buffer[2];
    rt_uint8_t temp;

    buf[0] = (uint8_t)(reg >> 8);
    buf[1] = (uint8_t)reg;
    rt_i2c_master_send(bus, fmu_i2c_list[i].i2c_addr, 0, buffer, 2);

    struct rt_i2c_msg msgs;
    msgs.addr  = fmu_i2c_list[i].i2c_addr;
    msgs.flags = RT_I2C_RD;
    msgs.buf   = buf;
    msgs.len   = 1;

    uint8_t size = rt_i2c_transfer(bus, &msgs, 1);

    return size == 1 ? RT_EOK : RT_ERROR;
}
static rt_err_t read_reg(struct rt_i2c_bus_device *bus, uint8_t reg, uint8_t *buf, int i) {
    struct rt_i2c_msg msgs[2];
    msgs[0].addr  = fmu_i2c_list[i].i2c_addr;
    msgs[0].flags = RT_I2C_WR;
    msgs[0].buf   = &reg;
    msgs[0].len   = 1;

    msgs[1].addr  = fmu_i2c_list[i].i2c_addr;
    msgs[1].flags = RT_I2C_RD;
    msgs[1].buf   = buf;
    msgs[1].len   = 1;

    uint8_t size = rt_i2c_transfer(bus, msgs, 2);

    return size == 2 ? RT_EOK : RT_ERROR;
}

static int i2c_test(int argc, char *argv[]) {
    rt_uint32_t value, vol;
    rt_err_t    ret = RT_EOK;
    int         num = 6, close_chan = 0;
    uint8_t     data = 0xff;

    if (argc < 2) {
        rt_kprintf("\n");
        rt_kprintf("I2C1        TEST I2C1_START \n");
        rt_kprintf("I2C2        TEST I2C2_START \n");
        rt_kprintf("I2C3        TEST I2C3_START \n");

        return 0;
    }
    if (argc >= 2) {
        if (!strcmp(argv[1], "I2C1")) num = 0;
        if (!strcmp(argv[1], "I2C2")) num = 1;
        if (!strcmp(argv[1], "I2C3")) num = 2;

        if (num <= 3) {
            i2c_bus = (struct rt_i2c_bus_device *)rt_device_find(fmu_i2c_list[num].dev_name);
            if (i2c_bus == RT_NULL) {
                rt_kprintf("can't find %s device!\n", fmu_i2c_list[num].dev_name);
                return RT_ERROR;
            }
            if (num == 0) {
                ret = write_reg16(i2c_bus, AT24CXX_MAX_MEM_ADDRESS, 0x54, num);
                rt_thread_mdelay(10);
                ret = read_reg16(i2c_bus, AT24CXX_MAX_MEM_ADDRESS, &data, num);
                rt_kprintf("test_Success read addr= 0X%02X\n", data);
                if (ret != RT_EOK) {
                    rt_kprintf("TEST %s failed!\n", fmu_i2c_list[num].dev_name);
                }
            }
            if (num == 1) {
                ret = write_reg16(i2c_bus, AT24CXX_MAX_MEM_ADDRESS, 0x54, num);
                rt_thread_mdelay(10);
                ret = read_reg16(i2c_bus, AT24CXX_MAX_MEM_ADDRESS, &data, num);
                rt_kprintf("test_Success read addr= 0X%02X\n", data);
                if (ret != RT_EOK) {
                    rt_kprintf("TEST %s failed!\n", fmu_i2c_list[num].dev_name);
                }
            }
            if (num == 2) {
                ret = read_reg(i2c_bus, 0X00, &data, num);
                rt_kprintf("test_Success read addr= 0X%02X\n", data);
                if (ret != RT_EOK) {
                    rt_kprintf("TEST %s failed!\n", fmu_i2c_list[num].dev_name);
                }
            }
        }
    }
    return 0;
}
MSH_CMD_EXPORT(i2c_test, write i2c cmd);
