# Pixhawk V6X-RT

根据 [Pixhawk V6X-RT 开发标准](https://github.com/pixhawk/Pixhawk-Standards/blob/master/DS-020%20Pixhawk%20Autopilot%20v6X-RT%20Standard.pdf) ，Pixhawk V6X-RT 采用 NXP 的 [MIMXRT1176CVM8B](https://www.nxp.com/part/MIMXRT1176CVM8B) 高性能跨界处理器， 并外挂 MXIC（旺宏电子）[MX25UM51345GXDI00](https://www.macronix.com/Lists/Datasheet/Attachments/8724/MX25UM51345G,%201.8V,%20512Mb,%20v1.3.pdf) Octal Flash。目前市场只有发布了两款基于 Pixhawk V6X-RT 的 飞控：

- [NXP MR-VMU-RT1176 Flight Controller](https://docs.px4.io/main/en/flight_controller/nxp_mr_vmu_rt1176)
- [Holybro Pixhawk 6X-RT](https://docs.px4.io/main/en/flight_controller/pixhawk6x-rt)

## 硬件资源

### 主控芯片

 i.MX RT1176 跨界双核 MCU 包含最高 1GHz 的 Arm Cortex-M7 核心和最高 400MHz 的 Arm Cortex-M4 核心，配备 2MB SRAM 和 64MB 的外接 XIP 闪存。

- 32 Bit Arm® Cortex®-M7, 1GHz
- 32 Bit Arm® Cortex®-M4, 400MHz secondary core
- 64MB external flash memory (FlexSpi1 0x30000000)
- 2MB RAM

### 安全元件

- NXP EdgeLock SE051 硬件安全元件
- IEC62443-4-2 认证符合相关要求
- 46 kB 用户内存，带有个性化选项，最高可达 104 kB
- 开创性的CC EAL6+认证物联网部署解决方案
- AES和3DES的加密与解密

### 存储设备

- FLEXSPI1：MX25UM51345GXDI00（MXIC，旺宏电子），Octal Flash，存储代码
  - Memory size: 64MB (0x04000000)
  - Block size: 64KB
  - Sector size: 4KB (uniform, 0x1000)
  - Page size: 256bytes (0x100)
- FLEXSPI2：FM25V02A-GTR，FRAM 32KB (256kb)，存储参数
- USDHC1：MicroSD，存储日志

### 传 感 器

- Accel/Gyro: ICM-20649 or BMI088
- Accel/Gyro: ICM-42688-P
- Accel/Gyro: ICM-42670-P
- Mag: BMM150
- Barometer: 2x BMP388

## 总线划分

### 串口

总线 | 设备 | 用途 | 说明
--- | --- | --- | ---
UART1 | /dev/ttyS0 | Debug | 外接调试串口 |
UART2 | x | x | 与I2C3冲突，未使用 |
UART3 | /dev/ttyS1 | GPS1 |
UART4 | /dev/ttyS2 | TELEM1 |
UART5 | /dev/ttyS3 | GPS2 |
UART6 | /dev/ttyS4 | RC |
UART7 | x | x | 与TRACE冲突，V6XRT标准未使用，但NEXTPILOT硬件放弃了TRACE把UART7引出
UART8 | /dev/ttyS5 | TELEM2 |
UART9 | x | x | 与FLEXSPI1冲突，未使用
UART10 | /dev/ttyS6 | TELEM3 |
UART11 | /dev/ttyS7 | External |
UART12 | x | x | 与SPI6冲突，V6XRT标准未使用，但NEXTPILOT硬件放弃了SPI6把UART12引出

### SPI

总线 | 设备 | 用途
--- | --- | ---
FlexSPI1 | MX25UM51345G | NorFlash， 存储代码/code
FlesSPI2 | FM25V02A | Fram， 保存参数/param
SPI1 | ICM-42688P | 1176核心板，用于IMU传感器，比如ICM
SPI2 | ICM-42688P | 独立IMU板，用于IMU传感器，比如ICM
SPI3 | BMI088 | 独立IMU板，用于IMU传感器，比如bmi088
SPI4 |  | 1176核心板，用于IMU传感器，比如bmi088
SPI5 | x | 没使用
SPI6 | | 外部传感器

> BMI088 作为高频 IMU，建议独占 SPI 总线；多设备共享会引入 MISO 电平竞争、时序抖动、上电初始化不稳定，高性能飞控 / 机器人不建议复用总线。

## I2C

总线 | 设备 | 用途
--- | --- | ---
I2C1 | | 引出到gps1，一般是LED，或者SMBUS
I2C2 | BMP388 | 1176核心板，用于气压计
I2C3 | BMP388、SEM051 | 独立IMU板，用于气压计，以及板载的安全芯片
I2C4 | BM315，Memeory | 独立IMU板，磁罗盘，还有校准用的Memory
I2C5 | x | 没用到
I2C6 |  | 引出去到gps2，一般是LED，或者SMBUS，另外还有板载校准用的Memory以及安全芯片

## 编译固件

```shell
scons
```

## 参考资料

- NXP MR-VMU-RT1176帮助文档：<https://nxp.gitbook.io/vmu-rt1176>
- PX4 MR-VMU-RT1176帮助文档：<https://docs.px4.io/main/en/flight_controller/nxp_mr_vmu_rt1176>
- Pixhawk FMUv6X-RT 设计标准：<https://github.com/pixhawk/Pixhawk-Standards/blob/master/DS-020%20Pixhawk%20Autopilot%20v6X-RT%20Standard.pdf>
