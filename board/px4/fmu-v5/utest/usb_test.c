/******************************************************************
 *      _   __             __   ____   _  __        __
 *     / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
 *    /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
 *   / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
 *  /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
 *
 * Copyright All Reserved © 2015-2023 NextPilot Development Team
 ******************************************************************/
/*
 * 程序清单：这是一个 USB CDC设备使用例程
 * 例程导出了 usb_cdc_sample 命令到控制终端
 * 命令调用格式：usb_cdc_sample
 * 程序功能：每隔500ms向终端输出"hello RT-Thread!\r\n"(输出50次),读取接受到的数据
 */
#include <rtthread.h>
#include <rtdevice.h>
#define USB_CDC_DEV_NAME  "vcom" /* usbd虚拟串口设备名称 */
#define USBD_CDC_RLEN_MAX 1024

/* 用于接收消息的信号量 */
static struct rt_semaphore rx_sem;
rt_device_t                cdc_dev = RT_NULL; /* usb cdc设备句柄 */

/* 接收数据回调函数 */
static rt_err_t usb_cdc_input(rt_device_t dev, rt_size_t size) {
    /* 串口接收到数据后产生中断，调用此回调函数，然后发送接收信号量 */
    rt_sem_release(&rx_sem);

    return RT_EOK;
}

static void usb_cdc_thread_entry(void *parameter) {
    char ch;

    while (1) {
        /* 从串口读取一个字节的数据，没有读取到则等待接收信号量 */
        while (rt_device_read(cdc_dev, -1, &ch, 1) != 1) {
            /* 阻塞等待接收信号量，等到信号量后再次读取数据 */
            rt_sem_take(&rx_sem, RT_WAITING_FOREVER);
        }
        /* 读取到的数据通过串口输出 */
        rt_device_write(cdc_dev, 0, &ch, 1);
    }
}

static int usb_cdc_sample(int argc, char *argv[]) {
    char wBuf[] = "hello RT-Thread!\r\n";

    cdc_dev = rt_device_find(USB_CDC_DEV_NAME);
    if (!cdc_dev) {
        rt_kprintf("vcom find failed\n");
        return -RT_ERROR;
    }

    /* 初始化信号量 */
    rt_sem_init(&rx_sem, "rx_sem", 0, RT_IPC_FLAG_FIFO);

    // rt_device_open(cdc_dev, RT_DEVICE_FLAG_RDWR);
    /* 以中断接收及轮询发送方式打开串口设备 */
    rt_device_open(cdc_dev, RT_DEVICE_FLAG_INT_RX);
    /* 设置接收回调函数 */
    rt_device_set_rx_indicate(cdc_dev, usb_cdc_input);

    rt_device_write(cdc_dev, 0, wBuf, rt_strlen(wBuf));

    /* 创建 usb 线程 */
    rt_thread_t thread = rt_thread_create("usb_cdc", usb_cdc_thread_entry, RT_NULL, 1024, 25, 10);
    if (thread != RT_NULL) {
        rt_thread_startup(thread);
    } else
        return -RT_ERROR;

    // rt_device_close(cdc_dev);

    return RT_EOK;
}
/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(usb_cdc_sample, usbd cdc sample);
