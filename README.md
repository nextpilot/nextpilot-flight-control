# NextPilot 飞行控制系统

[English](README.md) | [中文](README_zh.md) | [Español](README_es.md) | [Deutsch](README_de.md)

NextPilot是一款的高性能高可靠的自动驾驶仪，支持多旋翼、复合翼、固定翼等。软件框架(param/workq)、通信中间层（uorb/mavlink）、控制算法等参考或移植于[px4 v1.14.2](https://github.com/px4/px4-autopilot.git)项目，并基于国产[RT-Thread v5.0.2](https://github.com/RT-Thread/rt-thread/tree/v5.0.2)实时操作系统进行打造。

## 产品特色

1. 采用国产[RT-Thread](https://rt-thread.org)实时嵌入式操作系统，拥有高可靠和自主可控等特性；
1. 支持[MATLAB/Simulink](https://www.mathworks.com)工具链，应用MBD进行飞控算法设计、仿真、验证；
1. 支持模型在环(MIL)、软件在环(SIL)、硬件在环(HIL)、仿真在环(SIH)等各种飞行验证。

## 系统框架

![](./docs/Y.开发指南/03.系统框架/architecture.png)

## 支持机型

## 硬件支持

目前 NextPilot 支持 [PX4 FMUv5]() 系列开源硬件，以及 NextPilot 自主开发的飞控硬件。

# 快速入门

## 使用手册

[用户指南]() | [飞行模式]() | [飞行仿真]()

## 开发指南

[开发环境]() | [硬件适配]()

# 代码贡献
