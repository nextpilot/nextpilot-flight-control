# 高精度时钟定时器

hrtimer主要实现以下两个功能：

- 全局时间时间戳（global timestamp）
- 高精度定时器，能够`微秒级周期性`调用，也可以延时`指定微秒`后调用

## 全局时间戳

全局时间戳功能，主要包含以下几个函数：

```c
typedef uint64_t hrt_abstime;

// 获取全局时间戳，微秒级
hrt_abstime hrt_absolute_time(void);

// 相对then已过了多少微秒
hrt_abstime hrt_elapsed_time(const hrt_abstime *then)
{
    return hrt_absolute_time() - *then;
}

/**
 * Convert absolute time to a timespec.
 */
void abstime_to_ts(struct timespec *ts, hrt_abstime abstime)
{
    ts->tv_sec   = (typeof(ts->tv_sec))(abstime / 1000000);
    abstime     -= (hrt_abstime)(ts->tv_sec) * 1000000;
    ts->tv_nsec  = (typeof(ts->tv_nsec))(abstime * 1000);
}

/**
 * Store the absolute time in an interrupt-safe fashion.
 *
 * This function ensures that the timestamp cannot be seen half-written by an interrupt handler.
 */
void hrt_store_absolute_time(volatile hrt_abstime *t)
{
    rt_base_t flags = rt_hw_interrupt_disable();
    *t              = hrt_absolute_time();
    rt_hw_interrupt_enable(flags);
}
```

根据不同的芯片或者应用场景，主要是实现`hrt_absolute_time()`函数。通常获取全局时间错有以下方法：

（1）RT-Thread的os tick

rt-thread根据`RT_TICK_PER_SECOND`自动配置systick的装载量（LOAD），然后调用`rt_tick_increase()`来增加os tick，因此os tick的精度是 `1/RT_TICK_PER_SECOND` 秒，典型的情况RT_TICK_PER_SECOND=1000，那么os tick的精度就是1ms。

> 为了系统调度性能rt-thread限制RT_TICK_PER_SECOND最大为1000，因此os tick最高精度也就是1ms了。

（2）STM32的 DWT 寄存器

 在 Cortex-M 里面`DWT(Data Watchpoint and Trace)`，是用于系统调试及跟踪，它有一个32位的寄存器叫CYCCNT， 它是一个向上的计数器，内核时钟跳动一次，该计数器就加1，当CYCCNT溢出之后， 会清0重新开始向上计数。

例如STM32F103系列，内核时钟是72M，那计时精度就是1/72M=14ns，最长能记录的时间为60s=2^32/72M；而STM32H7主频一般为400M，则计时精度高达1/400M=2.5ns，最长记录时间为10.74s=2^32/400M。而i.MX RT1052主频高达528M，内核跳一次的时间大概为1/528M=1.9ns，最长能记录的时间8.13s=2^32/528M 。

（3）STM32的 TIM 定时器

（4）STM32的 RTC 实时时钟

STM32H7的 RTC（Real Time Clock）支持亚秒，但是大部分STM32的RTC都不支持亚秒。

> 如果RTC用于嵌入式调度，精度太低了。且需要额外的硬件资源，因此 **不推荐** 。

（5）STM32H7的 TSG 寄存器

STM32H743/H750 系列的芯片有一个64位的全局时间戳发生器（Global timestamp generator）

```c
// 全局时间戳发生器相关寄存器
#define TSG_CNTCR *(volatile unsigned int *)0x5C005000  // 计数器控制寄存器
#define TSG_CNTCVL *(volatile unsigned int *)0x5C005008 // 当前计数器计数值低字寄存器
#define TSG_CNTCVU *(volatile unsigned int *)0x5C00500C // 当前计数器计数值高字寄存器

// 初始化并启动全局时间戳发生器
TSG_CNTCR = 0x00000000; // 停止计数器
TSG_CNTCVL = 0;         // 清空计数器计数值低字
TSG_CNTCVU = 0;         // 清空计数器计数值高字
TSG_CNTCR = 0x00000001; // 启动计数器
```

## 高精度定时器

高精度定时器，要求实现以下几个函数：

```c
// 按照period_us定周期调用hrt_call_isr
int hrt_call_isr_every(uint32_t period_us);

// 延时timeout_us后调用hrt_call_isr
int hrt_call_isr_after(uint32_t timeout_us);
```
