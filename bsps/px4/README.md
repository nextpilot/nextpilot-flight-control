# Pixhawk 开源飞控硬件

## 支持硬件

> 注意：从Pixhawk FMU V5版本开始，PX4官方只是提供[硬件设计标准](https://github.com/pixhawk/Pixhawk-Standards)，不再开放硬件原理图了。也就是规定了MCU管脚定义，以及Payload接口、IMU接口等。

NextPilot 支持以下 Pixhawk 开源硬件：

### CUAV v5+ (FMUv5)

[硬件介绍](https://docs.px4.io/main/en/flight_controller/cuav_v5_plus.html) | [管脚定义](https://docs.google.com/spreadsheets/d/1-n0__BYDedQrc_2NHqBenG1DNepAgnHpSGglke-QQwY/edit#gid=912976165) | [BSP代码](./fmu-v5/README.md)

- 首次采用 STM32F765 处理器（216 MHz, 512 KB RAM, 2 MB flash），拥有更高的性能。

### Holybro Pixhawk v5x (FMUv5x)

[硬件介绍](https://docs.px4.io/main/en/flight_controller/pixhawk5x.html) | [管脚定义](https://docs.google.com/spreadsheets/d/1Su7u8PHp-Y1AlLGVuH_I8ewkEEXt_bHHYBHglRuVH7E/edit?gid=730959725#gid=730959725) | [BSP代码](./fmu-v5x/README.md)

- 处理器 STM32F765, 216 MHz, 512 KB RAM, 2 MB flash
- FMUv5的行业版本，模块化设计，隔离低噪声多冗余传感器，提供LAN接口

### CUAV Pixhawk v6x (FMUv6x)

[硬件介绍](https://docs.px4.io/main/en/flight_controller/cuav_pixhawk_v6x.html) | [管脚定义](https://docs.google.com/spreadsheets/d/192yS1ewajvsgp90g_rVM3QiBnM678sPVkYVWaQhRWBQ/edit?gid=730959725#gid=730959725) | [BSP代码](./fmu-v6x/README.md)

- 处理器升级为 STM32H743，相当于V6的行业版本

## 参考资料

- PX4开源硬件原理图：<https://github.com/pixhawk/Hardware>
- PX4开源参考硬件设计：<https://github.com/pixhawk/Pixhawk-Standards>
