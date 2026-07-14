# Pixhawk 开源飞控硬件

Pixhawk 开源飞控从2012年开始，硬件迭代了近[10个硬件版本](https://docs.px4.io/main/en/flight_controller/pixhawk_series#fmu-versions)，下表列出了他们的主要对比：

Version | Year | Lead Product | CPU | Other
--- | --- | --- | --- | ---
FMUv1 & IOv1 | 2012 | None | 168 MHz M4 | Original Flight Management Unit and Separate I/O board.
FMUv2 | 2013 | Pixhawk 1 | STM32427VI, 168 MHz M4 | Single board with STM32427VI processor.
FMUv3 | 2015 | Pixhawk 2 | 168 MHz M4 | Redundant sensors. Identical to FMUv2, but usable flash doubled to 2MB.
FMUv4 | 2015 | Pixracer | 168 MHz M4 | Increased RAM. Faster CPU. More serial ports. No IO processor.
FMUv4X | 2017 | Pixhawk 3 Pro | 168 MHz M4 | Slightly increased RAM. More serial ports. IO processor. Redundant sensors.
FMUv5 | 2018 | Pixhawk 4 | STM32F765, 200 MHz M7, 512KB RAM, 2MB flash | 采用新的 STM32F765 处理，更快更多RAM，从此开始不提供硬件原理图
FMUv5X | 2019 | Pixhawk 5X | STM32F765, 200 MHz M7, 512KB RAM, 2MB flash | 新增温度校准，传感器冗余，以及网口
FMUv6x | / | Pixhawk 6X | STM32H753, 480 MHz M7, 1MB RAM, 2MB flash | 更换更强大的 STM32H753 处理器，__是目前市场上最主流的版本__
FMUv6x-RT | / | Pixhawk 6X-RT | MIMXRT1176, 800MHz M7, 2MB RAM, 64MB flash | 采用 NXP 跨界处理器 MIMXRT1176

> 注意：从Pixhawk FMU V1 ~ V4 官方提供了硬件原理图，可以[参考这里](https://github.com/pixhawk/Hardware)；从V5 版本开始，PX4官方只提供 [硬件设计标准](https://github.com/pixhawk/Pixhawk-Standards)，不再开放硬件原理图了，也就是规定了MCU 管脚定义，以及Payload接口、IMU接口等。

## 支持硬件

NextPilot 支持以下 `Pixhawk v4 (FMUv5)` 之后的 开源硬件，具体如下：

### Pixhawk v4 (FMUv5)

[管脚定义](https://docs.google.com/spreadsheets/d/1-n0__BYDedQrc_2NHqBenG1DNepAgnHpSGglke-QQwY/edit) | [BSP代码](./fmu-v5/README.md)

- 首次采用 STM32F765 处理器（216 MHz, 512 KB RAM, 2 MB flash），拥有更高的性能。

基于 Pixhawk v4 (FMUv5) 主流飞行控制器有：

- [Holybro Pixhawk 4 (FMUv5)](https://docs.px4.io/v1.17/en/flight_controller/pixhawk4)，没有提供硬件原理图
- [CUAV V5+ Autopilot (FMUv5)](https://docs.px4.io/v1.17/en/flight_controller/cuav_v5_plus)，硬件原理图 [点击这里](https://github.com/cuav/hardware/tree/master/V5_Autopilot/V5%2B)

### Pixhawk v5x (FMUv5x)

[设计标准](https://github.com/pixhawk/Pixhawk-Standards/blob/master/DS-011%20Pixhawk%20Autopilot%20v5X%20Standard.pdf) | [管脚定义](https://docs.google.com/spreadsheets/d/1Su7u8PHp-Y1AlLGVuH_I8ewkEEXt_bHHYBHglRuVH7E/edit?gid=730959725) | [BSP代码](./fmu-v5x/README.md)

- 处理器 STM32F765， 216 MHz， 512 KB RAM， 2 MB flash
- FMUv5的行业版本，模块化设计，隔离低噪声多冗余传感器，提供LAN接口

基于 Pixhawk v5x (FMUv5x) 主流飞行控制器有：

- [Holybro Pixhawk 5X
](https://docs.px4.io/main/en/flight_controller/pixhawk5x.html)

### Pixhawk v6x (FMUv6x)

[设计标准](https://github.com/pixhawk/Pixhawk-Standards/blob/master/DS-012%20Pixhawk%20Autopilot%20v6X%20Standard.pdf) | [管脚定义](https://docs.google.com/spreadsheets/d/192yS1ewajvsgp90g_rVM3QiBnM678sPVkYVWaQhRWBQ/edit?gid=730959725) | [BSP代码](./fmu-v6x/README.md)

- 处理器升级为 STM32H743，480MHz, 1MB RAM, 2MB flash

基于 Pixhawk v6x (FMUv6x) 主流飞行控制器有：

- [CUAV Pixhawk V6X
](https://docs.px4.io/main/en/flight_controller/cuav_pixhawk_v6x.html)

- [Holybro Pixhawk 6X
](https://docs.px4.io/main/en/flight_controller/pixhawk6x)

### Pixhawk V6x-RT (FMUv6xrt)

[设计标准](https://github.com/pixhawk/Pixhawk-Standards/blob/master/DS-020%20Pixhawk%20Autopilot%20v6X-RT%20Standard.pdf) | 管角定义 | BSP

- 处理器 NXP MIMXRT1176，800 Mhz, 2MB SRAM
- 外挂 MXIC MX25UM51345G Octal Flash， 64MB

基于 Pixhawk V6x-RT (FMUv6xrt) 主流飞行控制器有：

- [NXP MR-VMU-RT1176 Flight Controller
](https://docs.px4.io/main/en/flight_controller/nxp_mr_vmu_rt1176.html)

- [Holybro Pixhawk 6X-RT
](https://docs.px4.io/main/en/flight_controller/pixhawk6x-rt.html)

## 参考资料

- PX4开源硬件原理图：<https://github.com/pixhawk/Hardware>
- PX4开源参考硬件设计：<https://github.com/pixhawk/Pixhawk-Standards>
