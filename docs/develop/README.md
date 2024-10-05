# 开发指南

!!! note
    本节适用于软件开发人员和（新）硬件集成商。如果您只是编译一个现有机架或使用 NextPilot 进行飞行，则不需要它。

本节介绍 如何搭建开发环境、底层框架是什么样的、如何支持新的机型和设备、如何修改飞行控制算法、如何添加新模式、如何集成新硬件以及飞控与外部通信是怎么实现的等。

## 如何开始

- 如何快速入门，比如：[搭建开发环境](./01.快速入门/01.setup-develop-environment.md)、[编译项目代码](01.快速入门/02.build-code.md)、[断点单步调试](./01.快速入门/03.ide-debug.md)等

- 系统框架介绍，比如：[系统启动流程](./03.系统框架/03.system-startup.md)、[FINSH终端](./03.系统框架/05.finsh-console.md)、[ULOG日志](./03.系统框架/07.micro-log.md)等

- 通用模块介绍，比如：[uorb订阅发布机制](./04.通用组件/01.uorb.md)、[param参数管理](./04.通用组件/03.param.md)、[mavlink通信管理](./04.通用组件/05.mavlink.md)等

- 设备驱动介绍，比如：传感器、遥控器、作动器等

- 飞行控制介绍，比如：[机架管理](./06.飞行控制/54.airframe.md)、[飞行模式](./06.飞行控制/59.flight-mode.md)

- 组合导航介绍

- 飞行仿真介绍

- 如何调试分析

## 获取帮助

核心开发团队和社区成员活跃于以下渠道，您可以从以下方式获得帮助。

问题反馈：<https://github.com/nextpilot/nextpilot-flight-control/issues>

代码提交：<https://github.com/nextpilot/nextpilot-flight-control/pulls>

论坛讨论：<https://github.com/nextpilot/nextpilot-flight-control/discussions>

## 反馈问题

如果您在使用 NextPilot 时遇到任何问题，请先在 [Github Discussions](https://github.com/nextpilot/nextpilot-flight-control/discussions) 上发布帖子（因为它们可能是由飞控配置引起的）。

根据开发团队指导，如果确定是代码问题则需要在 [Github Issue](https://github.com/nextpilot/nextpilot-flight-control/issues) 上反馈问题。如果可能，请提供[飞行日志](https://logs.px4.io)和其他信息。

## 开源许可

在 [BSD 3-clause](https://opensource.org/licenses/BSD-3-Clause) 许可证的条款，您可以免费使用和修改 [nextpilot-flight-control](https://github.com/nextpilot/nextpilot-flight-control) 代码，没有任何商业风险。

本文档采用 [CC BY 4.0](https://creativecommons.org/licenses/by/4.0/) 协议。

## 致谢

开发文档中，借用了 [PX4 用户指南](https://docs.px4.io) 和 [RT-Thread 在线文档](https://gitee.com/rtthread/docs-online) 等部分资料，在此表示感谢。
