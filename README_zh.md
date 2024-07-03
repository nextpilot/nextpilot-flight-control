# NextPilot 飞行控制系统

<p align="center">
<img src="docs/logo.png" width="60%" >
</p>

[English](README.md) | [中文](README_zh.md) | [Español](README_es.md) | [Deutsch](README_de.md)

NextPilot飞控系统是一款功能丰富、高度扩展、稳定可靠的国产先进自动驾驶仪。目标是便捷的应用于教育、研究和工业等领域，让工程师专注于自己的擅长领域（比如嵌入式、控制算法、应用业务）进行高效的开发。

1. 出色的嵌入式性能。基于 [RT-Thread V5.02](https://github.com/RT-Thread/rt-thread/tree/v5.0.2) 国产实时操作系统，它资源占用极低、超低功耗设计、支持高性能应用，拥有活跃的社区、丰富的组件（如FINSH、DFS、ULOG等）、繁荣发展的软件包生态。

2. 丰富的自动驾驶功能。核心算法移植于著名开源飞控软件 [PX4 V1.14.2](https://github.com/PX4/PX4-Autopilot/tree/v1.14.2)，它由来自全世界工业界和学术界的开发者维护，支持多旋翼、复合翼、固定翼、潜水艇等。

3. 高度的可扩展性。拥有良好的软件架构（[uORB](pkgs/uORB/uORB.h)、[PARAM](pkgs/param/param.h)、[AIRFRAME](apps/airframe/README.md)）、代码松耦合（[自动初始化机制](https://www.rt-thread.org/document/site/#/rt-线程版本/rt线程标准/编程手册/basic/basic？id=rt线程-%e8%87%aa%e5%8a%a8%e5%88%9d%e5%a7%8b%e5%8c%96%e6%9c%ba%e5%88%b6)），充分模块化（每个文件夹对应一个独立模块），易于定制和扩展等特性。

4. 高效的开发效率。支持 MATLAB/Simulink [基于模型的设计](https://www.mathworks.com/help/simulink/gs/model-based-design.html)，在统一的 Simulink 环境下完成设计、仿真、测试和一键代码生成，快速且经济高效地开发控制系统、信号处理系统和通信系统。
5. 支持多种飞行仿真，如模型在环（Model in Loop）、软件在环（[Software in Loop](bsps/sitl/qemu/README.md)）、硬件在环（Hardware in Loop）和硬件内飞行仿真（[Simulation in Hardware](apps/simulation/sih/sih.cpp)），以及多机编队仿真。

6. 支持多种飞控硬件，包括广泛使用的开源硬件 [Pixhawk FMUv5](bsps/px4/fmu-v5/README.md) 和 [NextPilot团队开发的硬件](bsps/ndt//README.md)。

7. 支持主流开发工具，包括 Gcc+VScode（[Windows](https://github.com/nextpilot/nextpilot-windows-toolchain)/[Ubuntu](https://github.com/nextpilot/nextpilot-ubuntu-toolchain)）、Keil 和 IAR等，通过 scons 工具生成多种 IDE 项目文件。

# 系统框架

![](./docs/develop/03.系统框架/architecture.png)

## 机型支持

- 多旋翼：quad, six, oct, hex
- 固定翼：plane
- 垂  起：standard, tailsitter, tiltrotor

## 功能支持

### 飞行控制

- 飞行模式：手动, 特技, 姿态, 定高, 定点; 起飞, 着陆, 任务, 悬停, 返航, 跟随; 外部控制等
- 安全保护：地理围栏, 低电量保护, GNSS失锁, 遥控失联, 链路失联, 发动机故障等

### 组合导航

输出姿态和位置，并支持风速估计、地形估计、多传感器表决、振动频率估计等，具备融合gps、imu、mag、baro等传感器数据。

### 飞行仿真

- [软件在环]((bsps/sitl/qemu/README.md))：运行于qemu环境（支持windows和ubuntu），飞控集成了飞行动力学仿真，传感器采用仿真数据，地面站通过UDP连接飞控

- 硬件在环：运行于stm32硬件上，飞控集成了飞行动力学仿真，传感器采用仿真数据，地面站通过UART连接飞控

## 硬件支持

Pixhawk开源硬件:

- [CUAV Pixhawk V5+ (FMUv5)](https://docs.px4.io/main/en/flight_controller/cuav_v5_plus.html), [bsp code](bsps/px4/fmu-v5)
- [CUAV Pixhawk V6X (FMUv6x)](https://docs.px4.io/main/en/flight_controller/cuav_pixhawk_v6x.html), 即将支持
- [Holybro Pixhawk V5X (FMUv5X)](https://docs.px4.io/main/en/flight_controller/pixhawk5x.html), 即将支持
- [Holybro Pixhawk V6X (FMUv6X)](https://docs.px4.io/main/en/flight_controller/pixhawk6x.html), 即将支持

NextPilot设计硬件:

- fcs-v1, 飞行控制计算机
- ins-v1, 组合导航计算机
- ads-v1, 大气数据计算机

支持的硬件设计点:

- 处理器：STM32F765IIK 和 STM32H753IIK
- 传感器：gnss/imu/baro/mag
- 作动器：pwm/can/rs485
- 遥控器：s.bus
- 告  警：led/tunes

# 文档资料

## 用户手册

[快速入门](https://docs.nextpilot.cn/manual) | [飞行模式](https://docs.nextpilot.cn/manual/05.%E5%9F%BA%E6%9C%AC%E6%A6%82%E5%BF%B5/01.%E9%A3%9E%E8%A1%8C%E6%A8%A1%E5%BC%8F/index.html) |[高级调参]() | [飞行仿真]()  |  [日志分析]()

<!-- # 社区支持

NextPilot 非常感谢所有社区小伙伴的支持，在使用 NextPilot 的过程中若您有任何的想法，建议或疑问都可通过以下方式联系到 NextPilot，我们也实时在这些频道更新 NextPilot 的最新讯息。同时，任何问题都可以在 [论坛](h) 中提出，社区成员将回答这些问题。 -->

## 开发指南

NextPilot 主要支持以下 IDE:

- KEIL MDK
- ARM GCC + VSCode,  [Windows](https://github.com/nextpilot/nextpilot-windows-toolchain) | [Ubuntu](https://github.com/nextpilot/nextpilot-ubuntu-toolchain)

您可以通过 [开发指南](https://docs.nextpilot.cn/develop) 了解如何进行NextPilot开发，比如：[搭建开发环境](https://docs.nextpilot.cn/develop/01.%E5%BF%AB%E9%80%9F%E5%85%A5%E9%97%A8/01.setup-develop-environment.html)， [如何添加代码](https://docs.nextpilot.cn/develop/01.%E5%BF%AB%E9%80%9F%E5%85%A5%E9%97%A8/99.add-new-code.html)， [系统底层框架]()等。

# 许可协议

NextPilot 系统完全开源，遵循[BSD 3-Clause License](./LICENSE) 开源许可协议，可以免费在商业产品中使用，并且不需要公开私有代码，没有潜在商业风险。

# 贡献代码

如果您对 NextPilot 感兴趣，希望参与 NextPilot 的开发并成为代码贡献者，请参阅[代码贡献指南](.github/CONTRIBUTING.md)。

感谢以下小伙伴对本仓库的贡献！

<a href="https://github.com/nextpilot/nextpilot-flight-control/graphs/contributors">
  <img src="https://contrib.rocks/image?repo=nextpilot/nextpilot-flight-control" />
</a>
