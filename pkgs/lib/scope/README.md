# SCOPE调试示波器

scope是一个简单的嵌入式调试工具，将嵌入式系统的变量通过**uart**发送给上位机，上位机通过曲线的方式展示出来。

## 基本原理

scope上位机通信协议只有2条消息：

```c
// 数值消息
typedef struct {
    uint32_t timestamp;
    uint8_t  name[8];
    float    data[16];
} scope_msg_vector_t;

// 文本消息
typedef struct {
    uint32_t timestamp;
    uint8_t  severity;
    uint8_t  message[127];
} scope_msg_text_t;
```

通过menuconfig可以配置scope，主要包含以下设置：

```c
// scope使用的串口
#define SCOPE_UART_NAME     "uart2"
// scope串口波特率
#define SCOPE_UART_BAUDRATE 460800

// 是否启用ULOG的scope后端，这样LOG_xxx等消息可以通过scope发送到上位机
#define ULOG_USING_SCOPE_BACKEND

// 是否使用内部的crc8校验算法
#define SCOPE_USING_INSIDE_CRC8

// 是否启用异步输出，强烈建议，否则scope的发送会阻塞主线程
#define SCOPE_USING_ASYNC_OUTPUT
// 异步输出ringbuffer的size
#define SCOPE_ASYNC_BUFFER_SIZE      2048
// 异步输出线程优先级
#define SCOPE_ASYNC_THREAD_PRIORITY  20
//异步输出线程堆栈大小
#define SCOPE_ASYNC_THREAD_STACKSIZE 1024
```

## 使用方法

```c
// 包含头文件
#include "scope.h"

void module_a(){
    // 发送数据，绘制曲线
    float value[16] ={1,2,3,4};
    scope_send_vector("module_a", value);

    // 发送文本消息
    // #define LOG_LVL_ASSERT                 0
    // #define LOG_LVL_ERROR                  3
    // #define LOG_LVL_WARNING                4
    // #define LOG_LVL_INFO                   6
    // #define LOG_LVL_DBG                    7
    scope_send_string(LOG_LVL_INFO, "call scope_send_string in module_a");

    // 另外通过LOG_E/LOG_W/LOG_D的消息会发送到调试终端，同时也会发送给上位机
    LOG_E("call LOG_E in module_a ")
}


void module_b(){
    float value[16] ={1,2,3,4};
    scope_send_vector("module_b", value);
}

int main(){
    // module_a
    module_a();

    // module_b
    module_b();

}

```
