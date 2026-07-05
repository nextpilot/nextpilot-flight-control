# Pixhawk V6X-RT

根据 [Pixhawk V6X-RT 开发标准](https://github.com/pixhawk/Pixhawk-Standards/blob/master/DS-020%20Pixhawk%20Autopilot%20v6X-RT%20Standard.pdf) ，Pixhawk V6X-RT 采用 NXP 的 [MIMXRT1176DVMAA](https://www.nxp.com/part/MIMXRT1176DVMAA) 高性能跨界处理器， 并外挂 MXIC（旺宏电子）[MX25UM51345GXDI00](https://www.macronix.com/Lists/Datasheet/Attachments/8724/MX25UM51345G,%201.8V,%20512Mb,%20v1.3.pdf) Octal Flash。目前市场只有发布了两款基于 Pixhawk V6X-RT 的 飞控：

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
  - Memory size: 512Mb (64MB, 0x04000000)
  - Block size: 64KB
  - Sector size: 4KB (uniform, 0x1000)
  - Page size: 256bytes (0x100)

- FLEXSPI2：FM25V02A-GTR，FRAM 256Kb (32KB)，存储参数

- USDHC1：MicroSD，存储日志

### 传 感 器

- Accel/Gyro: ICM-20649 or BMI088
- Accel/Gyro: ICM-42688-P
- Accel/Gyro: ICM-42670-P
- Mag: BMM150
- Barometer: 2x BMP388

## 编译固件

```shell
# 编译bootloader
make px4_fmu-v6xrt_bootloader

# 编译固件
make px4_fmu-v6xrt_default
```

- 烧写bootloader：<https://docs.px4.io/main/en/advanced_config/bootloader_update_v6xrt>
- 升级固件：使用QGroundControl

## 串口映射

|串口|设备|用途|
|---|---|---|
| UART | Device | Port |
| UART1 | /dev/ttyS0 | Debug |
| UART3 | /dev/ttyS1 | GPS |
| UART4 | /dev/ttyS2 | TELEM1 |
| UART5 | /dev/ttyS3 | GPS2 |
| UART6 | /dev/ttyS4 | PX4IO |
| UART10 | /dev/ttyS6 | TELEM3 |
| UART11 | /dev/ttyS7 | External |

## 参考资料

- NXP MR-VMU-RT1176帮助文档：<https://nxp.gitbook.io/vmu-rt1176>

- PX4 MR-VMU-RT1176帮助文档：<https://docs.px4.io/main/en/flight_controller/nxp_mr_vmu_rt1176>

- Pixhawk FMUv6X-RT 设计标准：<https://github.com/pixhawk/Pixhawk-Standards/blob/master/DS-020%20Pixhawk%20Autopilot%20v6X-RT%20Standard.pdf>
