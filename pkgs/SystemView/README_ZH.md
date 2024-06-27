# segger_debug

中文页 | [English](README.md)

segger 调试工具，例如 SEGGER 的实时传输，SEGGER SystemView 移植。

## 如何使用 Segger SystemView 跟踪组件

作者 Arda.Fu 2016.8.10

### 1. 复制描述文件

将 'description\SYSVIEW_RTThread.txt' 复制到文件夹 'SystemView_Install_Folder\Description\'。

### 2. 复制源文件

将文件夹 “trace” 复制到文件夹 “RTT_ROOT\components\trace”。

### 3. 启用跟踪组件
在您的 bsp 的 rtconfig.h 中启用 `RT_USING_HOOK` 并添加 `RT_USING_TRACE` 定义

```c
// <bool name="RT_USING_TRACE" description="Enable trace" default="true" />
#define RT_USING_TRACE

```
### 4. 连接到目标板调试接口

Segger SystemView 仅支持 Segger J-Link 和 Segger J-Link OB。如果使用供应商的原始开发板，则可以将板载调试器升级到 J-Link OB。

* 对于 ST ST-Link，这是 [ST-Link 升级实用程序下载页面]（https://www.segger.com/jlink-st-link.html）
* 对于 Atmel EDBG，这是 [EDBG 升级实用程序下载页面]（https://www.segger.com/jlink-edbg.html）
* 对于飞思卡尔（NXP）OpenSDA/OpenSDA-V2，这是 [OpenSDA 升级实用程序下载页面]（https://www.segger.com/opensda.html）
* 对于 NXP LPC Link 2，这是 [LPC link 2 升级实用程序下载页面]（https://www.segger.com/lpc-link-2.html）
* NXP LPCXpresso，这是 [LPCXpresso 升级实用程序下载页面]（https://www.segger.com/jlink-lpcxpresso-ob.html）

### 5. 集成（移植）限制

* 只能跟踪线程，调度程序和中断。
* 不支持跟踪 RT-Thread IPC 组件。
