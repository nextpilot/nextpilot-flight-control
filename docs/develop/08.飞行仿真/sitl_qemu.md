
# QEMU软件在环

NextPilot支持Qemu和Simulation两种软件在环（SITL）测试方式

嵌入式软件开发离不开开发板，在没有物理开发板的情况下，可以使用 QEMU 等类似的虚拟机来模拟开发板。QEMU 是一个支持跨平台虚拟化的虚拟机，它可以虚拟很多开发板。为了方便大家在没有开发板的情况下体验 NextPilot，NextPilot，RT 提供了 QEMU 模拟的 ARM vexpress A9 开发板的板级支持包 (BSP)。

本文主要介绍在 Window 平台使用 QEMU 运行 NextPilot 工程，并介绍了如何使用虚拟网卡连接 QEMU 到网络。


## 准备工作

请先完成[开发环境的安装和配置](../A.%E5%BF%AB%E9%80%9F%E5%85%A5%E9%97%A8/1-toolchain.md)，尤其是ENV编译环境 + Tap虚拟网卡。

## 编译和运行工程

使用Env打开$nextpilot/bsp/qemu/qemu-vexpress-a9，并输入以下`scons -j8`编译工程
```
scons -j8
```
![](../A.%E5%BF%AB%E9%80%9F%E5%85%A5%E9%97%A8/image/env-build-qemu-vexpress-v9.png)

编译完成之后在终端继续输入`qemu.bat`，启动qemu模拟器中，并在模拟器中运行sitl-qemu-default.bin程序，Env 命令行界面显示 Nextpilot 系统启动过程中打印的初始化信息及版本号信息等。
```bat
:: qemu.bat 是 Window 批处理文件，主要包括 QEMU 的执行指令，

@echo off
:: 首次运行工程会创建一份空白的 sd.bin 文件，这是虚拟的 sd 卡，大小为 64M。
if exist sd.bin goto run
qemu-img create -f raw sd.bin 64M

:run
:: 用双核vexpress-a9运行sitl-qemu-default.bin程序，网卡为tap
qemu-system-arm -M vexpress-a9 -smp cpus=2
                 -kernel build/sitl-qemu-default.bin
                 -display none -serial stdio -sd sd.bin
                 -net nic -net tap,ifname=tap
```

![](../A.%E5%BF%AB%E9%80%9F%E5%85%A5%E9%97%A8/image/env-start-qemu-vexpress-v9.png)

注意事项：若电脑安装有 360 安全卫士会有警告，请点击允许程序运行。

## 运行 Finsh 控制台

Nextpilot 支持 Finsh，用户可以在命令行模式使用命令操作。输入 `help` 或按 `tab` 键可以查看所有支持的命令。如下图所示，左边为命令，右边为命令描述。

![](./image/finsh-cmd.png)

如下图所示，比如输入`list_thread`命令可以查看当前运行的线程，以及线程状态和堆栈大小等信息。输入list_timer可以查看定时器的状态。

![](./image/finsh-thread.png)

输入 `list_device` 可以查看注册到系统的所有设备。

![](./image/finsh-device.png)

## 运行网络功能

**注意事项**

[请一定先安装好虚拟网卡Tap](../A.%E5%BF%AB%E9%80%9F%E5%85%A5%E9%97%A8/1-toolchain.md#tap)，并确定qemu.bat文件中包含 `-net nic -net tap,ifname=tap`

### 查询IP地址

在 shell 中输入 ifconfig命令查看网络状态，正常获取到 IP 即表示网络驱动正常，配置工作完成，效果如下图所示：

![](./image/ifconfig.png)

**注意事项**

- 当出现获取不到 IP 地址的情况时，先将以太网的共享关闭，然后再次打开即可。
- 如果获取到 IP 是 10.0.x,x，是因为没有为 QEMU 添加启动参数 -net nic -net tap,ifname=tap 。
- 虚拟机刚开始运行的时候并不会立刻获取到 IP 地址，有时需要等待几秒钟才会获取到 IP。
- 关闭虚拟机可以按 Ctrl + 'C' 来结束程序运行。

### 运行PING工具

在 shell 中输入 ifconfig命令查看网络状态，正常获取到 IP 即表示网络驱动正常：

![](./image/ping.png)
