
# BSP 板级支持包

本目录包含了 NextPilot 支持的各种硬件。

|序号|目录|作用|说明|
|---|---|---|---|
|1|ndt|Nextpilot团队的硬件产品|包括飞控、空速计、惯导等产品|
|2|px4|pixhawk标准定义的硬件产品|目前只支持FMUv5系列|
|3|sitl|[基于qemu虚拟飞行仿真](sitl/qemu-vexpress-a9/README.md)|支持udp链接地面站，日志存储、参数管理等，与真实飞行是同一套代码|

## 常用编译指令

进入到指定BSP目录，比如 `bsps/sitl/qemu`，通过右键启动 `env`，并在 env 终端里面运行：

```shell
# 显示帮助
scons -h

# 清理工程
scons -c

# 编译工程
scons             # 编译默认目标，等效scons default
scons default     # 根据config/default.config编译工程
scons bootloader  # 根据config/bootloader.config编译工程

# 生成mdk5/vs code/vs/cmake等工程
scons --target=mdk5  # 生成mdk5工程
scons --target=vsc   # 生成vsc工程
scons bootloader --target=mdk5  # 根据config/bootloader.config生成mdk5工程

# 根据xx.config文件生成rtconfig.h
scons --useconfig=xx.config

# 根据rtconfig.h生成.config，如果缺失.config文件，运行时menuconfig所有配置将使用默认值，因此此时需要先生成.config再运行menuconfig
scons --genconfig

# 启动menuconfig gui界面配置工程
scons --pyconfig

```

## 如何制作BSP

> 本教程只支持STM32系列芯片，其它系列请参考[RT-Thread官方文档](https://www.rt-thread.org/document/site/#/rt-thread-version/rt-thread-standard/README)：比如 [瑞萨RA系列](https://www.rt-thread.org/document/site/#/rt-thread-version/rt-thread-standard/tutorial/make-bsp/renesas-ra/RA%E7%B3%BB%E5%88%97%E9%A9%B1%E5%8A%A8%E4%BB%8B%E7%BB%8D)，[英飞凌PSoC™6系列](https://www.rt-thread.org/document/site/#/rt-thread-version/rt-thread-standard/tutorial/make-bsp/ifx-psoc6/PSoC6%E7%B3%BB%E5%88%97BSP%E5%88%B6%E4%BD%9C%E6%95%99%E7%A8%8B)。

STM32 BSP框架引入了**CubeMX**工具，可以使用该工具来对 BSP 中使用的外设引脚进行配置。CubeMX 工具提供了图形化的配置界面，这种图形化的配置方式对开发者来说更加直观，不仅可以让开发者灵活地配置 BSP 中使用的资源，并且可以让开发者对资源的使用情况一目了然。

BSP 文件夹中**不会包含固件库、外设驱动等**可以被多个 BSP 引用的代码文件。而是将这些通用的文件统一存放在 `rtos/platform/stm32` 文件夹中，通过在特定 BSP 中引用这些文件的方式，来包含 BSP 中所需的库文件或者驱动文件。这种方式不仅大大提高了代码复用率，降低了 BSP 的维护成本，而且可以更方便地给开发者提供更丰富的驱动文件，让开发者可以更容易地找到自己需要的资源。

bsp使用了cubemx生成的文件有：

- Core/Inc/main.h
- Core/Inc/stm32f1xx_hal_conf.h，包含了hal的配置
- Core/Src/main.c，仅用到SystemClock_Config()函数，cubemx**不要**`生成main()函数`
- Core/Src/stm32h7xx_hal_msp.c，包含了外设的管脚配置初始化

详细过程请参考：<https://www.nextpilot.org/develop/>
