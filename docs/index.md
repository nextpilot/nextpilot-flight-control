
# Home

[代码仓库](https://github.com/nextpilot/nextpilot-flight-control) | [开发指南](./develop/README.md) | [用户手册](./manual/README.md)

NextPilot飞控系统是一款功能丰富、高度扩展、稳定可靠的国产先进自动驾驶仪。目标是便捷的应用于教育、研究和工业等领域，让工程师专注于自己的擅长领域（比如嵌入式、控制算法、应用业务）进行高效的开发。

1. 出色的嵌入式性能。基于 [RT-Thread V5.02](https://github.com/RT-Thread/rt-thread/tree/v5.0.2) 国产实时操作系统，它资源占用极低、超低功耗设计、支持高性能应用，拥有活跃的社区、丰富的组件（如FINSH、DFS、ULOG等）、繁荣发展的软件包生态。

2. 丰富的自动驾驶功能。核心算法移植于著名开源飞控软件 [PX4 V1.14.2](https://github.com/PX4/PX4-Autopilot/tree/v1.14.2)，它由来自全世界工业界和学术界的开发者维护，支持多旋翼、复合翼、固定翼、潜水艇等。

3. 高度的可扩展性。拥有良好的软件架构（[uORB](https://github.com/nextpilot/nextpilot-flight-control/pkgs/uORB/uORB.h)、[PARAM](https://github.com/nextpilot/nextpilot-flight-control/pkgs/param/param.h)、[AIRFRAME](https://github.com/nextpilot/nextpilot-flight-control/apps/airframe/README.md)）、代码松耦合（[自动初始化机制](https://www.rt-thread.org/document/site/#/rt-线程版本/rt线程标准/编程手册/basic/basic？id=rt线程-%e8%87%aa%e5%8a%a8%e5%88%9d%e5%a7%8b%e5%8c%96%e6%9c%ba%e5%88%b6)），充分模块化（每个文件夹对应一个独立模块），易于定制和扩展等特性。

4. 高效的开发效率。支持 MATLAB/Simulink [基于模型的设计](https://www.mathworks.com/help/simulink/gs/model-based-design.html)，在统一的 Simulink 环境下完成设计、仿真、测试和一键代码生成，快速且经济高效地开发控制系统、信号处理系统和通信系统。

5. 支持多种飞行仿真，如模型在环（Model in Loop）、软件在环（[Software in Loop](https://github.com/nextpilot/nextpilot-flight-control/bsps/sitl/qemu/README.md)）、硬件在环（Hardware in Loop）和硬件内飞行仿真（[Simulation in Hardware](https://github.com/nextpilot/nextpilot-flight-control/apps/simulation/sih/sih.cpp)），以及多机编队仿真。

6. 支持多种飞控硬件，包括广泛使用的开源硬件 [Pixhawk FMUv5](https://github.com/nextpilot/nextpilot-flight-control/bsps/px4/fmu-v5/README.md) 和 [NextPilot团队开发的硬件](https://github.com/nextpilot/nextpilot-flight-control/bsps/ndt//README.md)。

7. 支持主流开发工具，包括 Gcc+VScode（[Windows](https://github.com/nextpilot/nextpilot-windows-toolchain)/[Ubuntu](https://github.com/nextpilot/nextpilot-ubuntu-toolchain)）、Keil 和 IAR等，通过 scons 工具生成多种 IDE 项目文件。
