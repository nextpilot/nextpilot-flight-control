# CUAV V5+ 开源飞控硬件（FMUv5）

![](docs/cuav_v5_plus.png)

[CUAV V5+](https://docs.px4.io/main/en/flight_controller/cuav_v5_plus.html) 是由 [雷迅CUAV](https://www.cuav.net/) 参照 [PX4 FMUv5](https://docs.google.com/spreadsheets/d/1-n0__BYDedQrc_2NHqBenG1DNepAgnHpSGglke-QQwY/edit#gid=912976165) 标准设计的开源飞控硬件，所有外部接口采用了 [Pixhawk Connector Standard](https://pixhawk.org/pixhawk-connector-standard/)。

## 硬件设计

### 快速概要

- 主处理器：STM32F765，32 Bit Arm® Cortex®-M7, 216MHz, 2MB memory, 512KB RAM
- 辅处理器：STM32F100，32 Bit Arm® Cortex®-M3, 24MHz, 8KB SRAM
- 板载传感器：
  - IMU： ICM-20689、BMI055
  - MAG：IST8310
  - BARO：MS5611
- 对外接口：
  - 8-14 PWM outputs (6 from IO, 8 from FMU)
  - 3 dedicated PWM/Capture inputs on FMU
  - Dedicated R/C input for CPPM
  - Dedicated R/C input for Spektrum / DSM and S.Bus with analog / PWM RSSI input
  - analog / PWM RSSI input
  - S.Bus servo output
  - 5 general purpose serial ports
  - 4 I2C ports
  - 4 SPI buses
  - 2 CANBuses with serial ESC
  - Analog inputs for voltage / current of 2 batteries

### 串口映射

|串口|名称|描述|
|-|-|-|
|UART1|uart1|GPS|
|UART2|uart2|TELEM1|
|UART3|uart3|TELEM2|
|UART4|uart4|TELEM4|
|UART5|/|无效，管脚复用了|
|UART6|uart6|S.BUS|
|UART7|uart7|调试串口|
|UART8|uart8|连接IO板|

外设接口定义如下：[请点击这里](./docs/cuav_v5_plus_pinouts.png)

## 如何使用

```shell
# 生成keil工程
scons --target=mdk5

# 使用gcc编译
scons -j10  # 10个线程编译
```

## 参考链接

- [CUAV V5+ 开源硬件原理图](https://github.com/cuav/hardware/tree/master/V5_Autopilot/V5%2B)
- [CUAV V5+ 用户使用说明书](http://manual.cuav.net/V5-Plus.pdf)
- [FMUv5 参考设计管脚定义](https://docs.google.com/spreadsheets/d/1-n0__BYDedQrc_2NHqBenG1DNepAgnHpSGglke-QQwY/edit#gid=912976165)
