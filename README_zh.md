# NextPilot 飞行控制系统

<p align="center">
<img src="docs/logo.png" width="60%" >
</p>

[English](README.md) | [中文](README_zh.md) | [Español](README_es.md) | [Deutsch](README_de.md)

NextPilot是一款的高性能高可靠的自动驾驶仪，支持多旋翼、复合翼、固定翼等。软件框架(param/workq)、通信中间层（uorb/mavlink）、控制算法等参考或移植于[PX4 V1.14.2](https://github.com/PX4/PX4-Autopilot/tree/v1.14.2)项目，并基于国产[RT-Thread V5.0.2](https://github.com/RT-Thread/rt-thread/tree/v5.0.2)实时操作系统进行打造。

1. 采用国产[RT-Thread](https://rt-thread.org)实时嵌入式操作系统，拥有高可靠和自主可控等特性；
1. 支持[MATLAB/Simulink](https://www.mathworks.com)工具链，应用MBD进行飞控算法设计、仿真、验证；
1. 支持模型在环(MIL)、软件在环(SIL)、硬件在环(HIL)、仿真在环(SIH)等各种飞行验证。

# 系统框架

![](./docs/develop/03.系统框架/architecture.png)

## 机型支持

- 多旋翼: quad, six, oct, hex
- 固定翼: plane
- 垂  起: standard, tailsitter, tiltrotor

## 功能支持

### 飞行控制

- 飞行模式: manul, arco, att, alt, pos; takeoff, land, mission, loiter, rtl, follow; offboard
- 安全保护: geofence, low power, gnss loss, rc loss, datalink loss, engine failure, etc

### 组合导航

输出姿态和位置，并支持风速估计、地形估计、多传感器表决、振动频率估计等，具备融合gps、imu、mag、baro等传感器数据。

### 飞行仿真

- 硬件在环：运行于stm32硬件上，飞控集成了飞行动力学仿真，传感器采用仿真数据，地面站通过UART连接飞控

- 软件在环：运行于qemu环境（支持windows和ubuntu），飞控集成了飞行动力学仿真，传感器采用仿真数据，地面站通过UDP连接飞控

## 硬件支持

目前 NextPilot 支持 [PX4 FMUv5](https://docs.px4.io/main/en/flight_controller/cuav_v5_plus.html) 系列开源硬件，以及 NextPilot 自主开发的飞控硬件。

NextPilot 主要当前支持以下硬件：

- 处理器: STM32F765IIK 和 STM32H753IIK
- 传感器：gnss/imu/baro/mag
- 作动器：pwm/can/rs485
- 遥控器：s.bus
- 告  警: led/tunes

# 文档资料

## 用户手册

[快速入门](https://docs.nextpilot.cn/manual) | [飞行模式](https://docs.nextpilot.cn/manual/05.%E5%9F%BA%E6%9C%AC%E6%A6%82%E5%BF%B5/01.%E9%A3%9E%E8%A1%8C%E6%A8%A1%E5%BC%8F/index.html) |[高级调参]() | [飞行仿真]()  |  [日志分析]()

<!-- # 社区支持

NextPilot 非常感谢所有社区小伙伴的支持，在使用 NextPilot 的过程中若您有任何的想法，建议或疑问都可通过以下方式联系到 NextPilot，我们也实时在这些频道更新 NextPilot 的最新讯息。同时，任何问题都可以在 [论坛](h) 中提出，社区成员将回答这些问题。 -->

## 开发指南

[开发环境](https://docs.nextpilot.cn/develop/01.%E5%BF%AB%E9%80%9F%E5%85%A5%E9%97%A8/01.setup-develop-environment.html) | [添加代码](https://docs.nextpilot.cn/develop/01.%E5%BF%AB%E9%80%9F%E5%85%A5%E9%97%A8/99.add-new-code.html) | [系统框架]()

NextPilot 主要支持以下 IDE:

- MDK KEIL
- ARM GCC + VSCode

# 许可协议

NextPilot 系统完全开源，遵循[BSD 3-Clause License](./LICENSE) 开源许可协议，可以免费在商业产品中使用，并且不需要公开私有代码，没有潜在商业风险。

# 贡献代码

如果您对 NextPilot 感兴趣，希望参与 NextPilot 的开发并成为代码贡献者，请参阅[代码贡献指南](.github/CONTRIBUTING.md)。

感谢以下小伙伴对本仓库的贡献！

<a href="https://github.com/nextpilot/nextpilot-flight-control/graphs/contributors">
  <img src="https://contrib.rocks/image?repo=nextpilot/nextpilot-flight-control" />
</a>
