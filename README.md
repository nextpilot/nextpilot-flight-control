# NextPilot Flight Control System

<p align="center">
<img src="docs/logo.png" width="60%" >
</p>

[English](README.md) | [中文](README_zh.md) | [Español](README_es.md) | [Deutsch](README_de.md)

NextPilot是一款的高性能高可靠的自动驾驶仪，支持多旋翼、复合翼、固定翼等。软件框架(param/workq)、通信中间层（uorb/mavlink）、控制算法等参考或移植于[PX4 V1.14.2](https://github.com/px4/px4-autopilot.git)项目，并基于国产[RT-Thread V5.0.2](https://github.com/RT-Thread/rt-thread/tree/v5.0.2)实时操作系统进行打造。

1. 采用国产[RT-Thread](https://rt-thread.org)实时嵌入式操作系统，拥有高可靠和自主可控等特性；
1. 支持[MATLAB/Simulink](https://www.mathworks.com)工具链，应用MBD进行飞控算法设计、仿真、验证；
1. 支持模型在环(MIL)、软件在环(SIL)、硬件在环(HIL)、仿真在环(SIH)等各种飞行验证。

## System Architecture

![](./docs/develop/03.系统框架/architecture.png)

## Supported Vehicle

- Multirotor: quad, six, oct, hex
- Fixed Wing: plane
- Hybrid VTOL: standard, tailsitter, tiltrotor

NextPilot use [airframe]() set Vehicle parameters and start modules.

## Supported Function

### Peripheral

- sensors: gnss/gps/rtk, IMU(gyro, accel), baro, mag, airspeed
- actuator: pwm, can
- battery: smart, analog
- radio control: s.bus
- notification: led, tunes, safety_button
- payload:

### Controller

- Flight Mode: manul, arco, att, alt, pos; takeoff, land, mission, loiter, rtl, follow; offboard 
- Safety: geofence, low power, gnss loss, rc loss, datalink loss, engine failure, etc

### Estimator

- Gyro fft:
- Sensor Vote:
- Wind estimator:

### Telemetry

- mavlink
- uavcan

### Simulation

- SITL: qemu + SIH
- HITL: SIH

## Supported Hardware

目前 NextPilot 支持 [PX4 FMUv5](https://docs.px4.io/main/en/flight_controller/cuav_v5_plus.html) 系列开源硬件，以及 NextPilot 自主开发的飞控硬件。

# Document

## User Manual
[Getting Started](https://docs.nextpilot.cn/manual) | [Flight Mode](https://docs.nextpilot.cn/manual/05.%E5%9F%BA%E6%9C%AC%E6%A6%82%E5%BF%B5/01.%E9%A3%9E%E8%A1%8C%E6%A8%A1%E5%BC%8F/index.html) | [Simulation]()

## Develeop Guide

[Getting Started](https://docs.nextpilot.cn/develop/01.%E5%BF%AB%E9%80%9F%E5%85%A5%E9%97%A8/01.setup-develop-environment.html) | [System Startup](https://docs.nextpilot.cn/develop/03.%E7%B3%BB%E7%BB%9F%E6%A1%86%E6%9E%B6/03.system-startup.html) | [Build Tools](https://docs.nextpilot.cn/develop/01.%E5%BF%AB%E9%80%9F%E5%85%A5%E9%97%A8/%E6%9E%84%E5%BB%BA%E9%85%8D%E7%BD%AE%E7%B3%BB%E7%BB%9F/Kconfig.html)

The main IDE/compilers supported by NextPilot are:

- MDK KEIL
- ARM GCC + VSCode

# License

NextPilt follows the [BSD 3-Clause License](./LICENSE). It's completely open-source, can be used in commercial applications for free, does not require the disclosure of code, and has no potential commercial risk. 

# Contribution

If you are interested in NextPilot and want to join in the development of NextPilot and become a code contributor, please refer to the [Code Contribution Guide](.github/CONTRIBUTING.md).

Thanks for the following contributors!

<a href="https://github.com/nextpilot/nextpilot-flight-control/graphs/contributors">
  <img src="https://contrib.rocks/image?repo=nextpilot/nextpilot-flight-control" />
</a>
