/*****************************************************************
 *     _   __             __   ____   _  __        __
 *    / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *   /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *  / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 * /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2025 NextPilot Development Team
 ******************************************************************/

#define LOG_TAG "param.fm25v02"
#define LOG_LVL LOG_LVL_INFO

#include <rtdbg.h>
#include "param_device_fm25v02.h"

// SPI 设备名称
#define FM25V02_DEVICE_NAME "fm25v02"


// 板载fm25v02,容量256k bit
#define FM25V02_CMD_WREN   0x06 // 写使能
#define FM25V02_CMD_WRDI   0x04 // 写失效
#define FM25V02_CMD_RDSR   0x05 // 读状态
#define FM25V02_CMD_WRSR   0x01 // 写状态
#define FM25V02_CMD_READ   0x03 // 读
#define FM25V02_CMD_FSTRD  0x0b // 快速读取
#define FM25V02_CMD_WRITE  0x02 // 写
#define FM25V02_CMD_SLEEEP 0xb9 // 睡眠模式
#define FM25V02_CMD_RDID   0x9f // 读设备ID
#define FM25V02_CMD_SNR    0xc3 // 读S/N

static int _init(param_device_t *dev) {
    struct rt_spi_device *spi_dev = (struct rt_spi_device *)dev->dev;
    // 配置设备接口
    struct rt_spi_configuration cfg;
    cfg.data_width = 8;
    cfg.mode       = RT_SPI_MASTER | RT_SPI_MODE_0 | RT_SPI_MSB;
    cfg.max_hz     = 20 * 1000 * 1000;
    if (rt_spi_configure(spi_dev, &cfg) != RT_EOK) {
        LOG_E("config spi fail");
        return -RT_ERROR;
    }

    // read device id
    // 0x7f 0x7f 0x7f 0x7f 0x7f 0x7f 0xc2
    // 0x22 0x00
    uint8_t cmd[1] = {0x9f};
    uint8_t devid[9];
    if (rt_spi_send_then_recv(spi_dev, cmd, 1, devid, sizeof(devid)) != RT_EOK) {
        LOG_E("read device id fail");
        return -RT_ERROR;
    }

    LOG_I("got devid %02x%02x%02x%02x%02x%02x%02x-%02x-%02x",
          devid[0], devid[1], devid[2], devid[3], devid[4],
          devid[5], devid[6], devid[7], devid[8]);

    dev->inited = true;
    return RT_EOK;
}

static int _open(param_device_t *dev, const char *name, uint8_t flag) {
    if (!dev) {
        return -RT_ERROR;
    }

    if (name) {
        dev->dev = rt_device_find(FM25V02_DEVICE_NAME);
    }

    if (!dev->dev) {
        LOG_E("can't find %s", FM25V02_DEVICE_NAME);
        return -RT_ERROR;
    }

    if (!dev->inited) {
        return _init(dev);
    }

    return RT_EOK;
}

static int _read(param_device_t *dev, int32_t offset, void *buff, uint32_t size) {
    if (!dev && !dev->dev) {
        return -RT_ERROR;
    }

    struct rt_spi_device *spi_dev = (struct rt_spi_device *)dev->dev;

    uint8_t cmd[3];
    cmd[0] = FM25V02_CMD_READ; // 读指令
    cmd[1] = offset >> 8;
    cmd[2] = offset & 0xff;
    // 发送读指令并接收数据
    if (rt_spi_send_then_recv(spi_dev, cmd, 3, buff, size) != RT_EOK) {
        LOG_E("read param fail");
        return -RT_ERROR;
    }
    return RT_EOK;
}

static int _write(param_device_t *dev, int32_t offset, void *buff, uint32_t size) {
    if (!dev && !dev->dev) {
        return -RT_ERROR;
    }

    struct rt_spi_device *spi_dev = (struct rt_spi_device *)dev->dev;

    uint8_t cmd[3];
    // 写使能
    cmd[0] = FM25V02_CMD_WREN;               // 写使能
    if (rt_spi_send(spi_dev, cmd, 1) != 1) { // 发送写使能
        LOG_E("enable write fail");
        return -RT_ERROR;
    }
    // 写入数据，先写入偏移，然后写入数据
    cmd[0] = FM25V02_CMD_WRITE;
    cmd[1] = offset >> 8;
    cmd[2] = offset & 0xff;
    // 发送写指令及数据
    if (rt_spi_send_then_send(spi_dev, cmd, 3, buff, size) != RT_EOK) {
        LOG_E("write param fail");
        return -RT_ERROR;
    }
    return RT_EOK;
}

static int _close(param_device_t *dev) {
    return RT_EOK;
}

static param_device_ops_t _ops = {
    .init  = _init,
    .open  = _open,
    .read  = _read,
    .write = _write,
    .close = _close,
};

param_device_t _param_fm25v02_device = {
    .name = "fm25v02",
    .type = 2,
    .ops  = &_ops,
};
