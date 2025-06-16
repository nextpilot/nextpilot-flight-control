#include <rtthread.h>
#include <rtdevice.h>

#define UART_NAME "uart3"

static rt_thread_t thread_handle_tx = RT_NULL;
static rt_thread_t thread_handle_rx = RT_NULL;

static rt_device_t            uart_dev = RT_NULL;
static struct rt_messagequeue uart_rx_mq;
static char                   rx_msg_pool[256];

static uint8_t should_exit = 0;

/**
 * @brief 串口接收消息结构
 *
 */
struct uart_rx_msg {
    rt_device_t dev;
    rt_size_t   size;
};

/**
 * @brief 串口接收回调
 *
 * @param dev
 * @param size
 * @return rt_err_t
 */
static rt_err_t callback_uart_rx_done(rt_device_t dev, rt_size_t size) {
    struct uart_rx_msg rx_msg;
    rt_err_t           result;
    // 发送消息队列，通知接收线程读取数据
    rx_msg.dev  = dev;
    rx_msg.size = size;
    if (rt_mq_send(&(uart_rx_mq), &rx_msg, sizeof(rx_msg)) != RT_EOK) {
        return RT_ERROR;
    }
    return RT_EOK;
}

/**
 * @brief 串口发送完成回调
 *
 * @param dev
 * @param buffer
 * @return rt_err_t
 */
static rt_err_t callback_uart_tx_done(rt_device_t dev, void *buffer) {
    return RT_EOK;
}

static void uart_thread_tx_entry(void *parameter) {
    rt_err_t result;
    uint16_t cnt = 0;
    while (!should_exit) {
        rt_device_write(uart_dev, 0, "Hello UART!\n", 12);
        rt_kprintf("UART write count: %d\n", cnt);
        cnt++;
        rt_thread_mdelay(1000);
    }
}

static rt_err_t parse(uint8_t data) {
    return RT_EOK;
}

static void uart_thread_rx_entry(void *parameter) {
    uint8_t            recv_buf[128] = {0x00};
    uint8_t            rx_length, i;
    struct uart_rx_msg msg;
    rt_err_t           result;

    while (!should_exit) {
        // Wait for a message from the queue
        result = rt_mq_recv(&uart_rx_mq, &msg, sizeof(msg), RT_WAITING_FOREVER);
        if (result == RT_EOK) {
            if (msg.size > 128) {
                msg.size = 128;
            }
            // 接收数据
            rx_length = rt_device_read(uart_dev, 0, recv_buf, msg.size);
            if (rx_length > 0) {
                for (i = 0; i < rx_length; i++) {
                    if (parse(recv_buf[i])) {
                    }
                }
            }
            rt_kprintf("UART received %d bytes\n", rx_length);
        } //
    }
}

static int test_uart_device_start(int argc, char **argv) {
    rt_err_t result;

    char uart_name[RT_NAME_MAX];
    if (argc == 3) {
        rt_strncpy(uart_name, argv[2], RT_NAME_MAX);
    } else {
        rt_strncpy(uart_name, UART_NAME, RT_NAME_MAX);
    }

    uart_dev = rt_device_find(uart_name);
    if (uart_dev == RT_NULL) {
        rt_kprintf("Failed to open UART device\n");
        return result;
    } else {
        rt_kprintf("Successfully found UART device: %s\n", uart_name);
    }

    // 配置
    struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT;
    config.baud_rate               = 115200;
    config.data_bits               = DATA_BITS_8;
    config.stop_bits               = STOP_BITS_1;
    config.bufsz                   = 128 * 2;
    config.parity                  = PARITY_NONE;

    if (rt_device_control(uart_dev, RT_DEVICE_CTRL_CONFIG, &config) != RT_EOK) {
        rt_kprintf("can't config serial device: %s\r\n", uart_name);
        return RT_ERROR;
    }

    // 初始化消息队列
    rt_mq_init(&uart_rx_mq, "uart_rx_mq",  // 消息队列名称
               rx_msg_pool,                // 存放消息的缓冲区
               sizeof(struct uart_rx_msg), // 一条消息的最大长度
               sizeof(rx_msg_pool),        // 存放消息的缓冲区大小
               RT_IPC_FLAG_FIFO);          // 如果有多个线程等待，按照先来先得到的方法分配消息

    // 添加数据接收完成回调
    rt_device_set_rx_indicate(uart_dev, callback_uart_rx_done);
    // 添加数据发送完成回调（DMA发送模式下，必须使用）
    rt_device_set_tx_complete(uart_dev, callback_uart_tx_done);
    // 以DMA方式打开串口
    if (rt_device_open(uart_dev, RT_DEVICE_FLAG_DMA_RX | RT_DEVICE_FLAG_DMA_TX) != RT_EOK) {
        rt_kprintf("Can't open serial device: %s\n", uart_name);
        return RT_ERROR;
    }

    // 创建发送和接收线程
    thread_handle_tx = rt_thread_create("test_uart_tx", uart_thread_tx_entry, RT_NULL, 1024, 20, 100);
    if (thread_handle_tx == RT_NULL) {
        rt_kprintf("Failed to create UART tx thread!\n");
        return RT_ERROR;
    }
    thread_handle_rx = rt_thread_create("test_uart_rx", uart_thread_rx_entry, RT_NULL, 1024, 20, 100);
    if (thread_handle_rx == RT_NULL) {
        rt_kprintf("Failed to create UART rx thread!\n");
        return RT_ERROR;
    }

    rt_thread_startup(thread_handle_tx);
    rt_thread_startup(thread_handle_rx);
    return RT_EOK;
}

static int test_uart_device_main(int argc, char **argv) {
    if (strcmp(argv[1], "help") == 0) {
        rt_kprintf("test uart help\r\n");
        rt_kprintf("utest_uart start uart2 - start uart test\r\n");
        rt_kprintf("utest_uart stop - stop uart test\r\n");
    } else if (strcmp(argv[1], "start") == 0) {
        return test_uart_device_start(argc, argv);
    } else if (strcmp(argv[1], "stop") == 0) {
        if (uart_dev != RT_NULL) {
            should_exit = 1;
            rt_thread_mdelay(2000); // 等待线程退出
            rt_kprintf("Stopping UART test...\r\n");
            rt_device_close(uart_dev);
            uart_dev = RT_NULL;
        }
        return RT_EOK;
    }
    return RT_EOK;
}

MSH_CMD_EXPORT_ALIAS(test_uart_device_main, utest_uart, test uart device);
