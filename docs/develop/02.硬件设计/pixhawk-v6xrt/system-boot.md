# 系统启动

I.MXRT1170 系列单片机，内部固化了最基础的“开机程序”，也就是 [BootROM](#boot-rom)。你一上电，BootROM 永远第一个醒来，但这时候它两眼一抹黑：该从哪里找你的主程序？是旁边的 NOR Flash，还是 SD 卡，或者是通过串口送进来？找到了程序，又该怎么把它搬到合适的地方跑起来？这一连串的问题，都需要一套明确的“行动指南”。

这套指南，就藏在芯片的启动模式（[BOOT_MODE](#boot-mode)）、OPT熔丝（[eFuse](#efuse)）、引脚配置（[BOOT_CFG](#boot-cfg)）和你精心准备的程序镜像（[Bootable Image](#bootable-image)）里。整个流程环环相扣，任何一个环节配置错了，你的板子就可能“变砖”，或者卡在启动的门口死活进不去。所以，理解它，不仅是学习知识，更是实战调试的必备技能。

不少工程师，程序编译得好好的，一下载进去就不运行，最后折腾半天发现是启动头没配置对。所以，咱们今天的目标就是让你彻底搞懂这套流程，以后遇到启动问题，能自己顺藤摸瓜找到原因。咱们先不急着看代码，先从宏观上把握几个核心角色：负责发号施令的 BootROM、提供硬件配置信息的 BOOT_CFG 引脚和熔丝 eFuse、以及承载你心血的那个最终的可启动镜像（Bootable Image）文件。

本文主要参考 [i.MX RT1170 Processor Reference Manual](./IMXRT1170RM.pdf) 中 `Chapter 10 System Boot` 章节，来讲解 `RT1170` 的系统启动流程。

## Boot ROM

BootROM 其实是芯片在出厂前固化在 ROM 里的一段启动程序（__不可改变和擦除__），它的目的就相当于一个引导程序。i.MX RT1170系列 BootROM 物理内存映射地址范围：`0x0000_0000 ~ 0x0003_FFFF`（共 `0x40000` 字节，256 KB），注意 BootROM 执行完毕后会将`0x0000_0000 ~ 0x0003_FFFF`重新映射为 `ITCM`  使用。

!!! note
    BootROM 是工厂一次性掩膜固化的片内 ROM，物理只读，没有擦写电路，任何工具（J-Link、MfgTool、MCUBootUtility、SDP 协议）都无法修改、升级、替换 BootROM 固件，__也就是出厂版本终身固定__。不过 NXP 给 i.MX RT 系列预留了eFuse ROM 补丁入口，仅用于修复 ROM 少量已知硬件 Bug，属于运行时内存替换，无法整体更新 BootROM。

根据参考手册`10.2 Overview`章节，可知 BootROM 可以帮助你完成：

- 支持从多种启动存储设备启动，比如Nor Flash、 NAND Flash、eMMC等
    - Serial NOR Flash via FlexSPI
    - Serial NAND Flash via FlexSPI
    - SLC RAWNAND Flash via SEMC
    - SD/MMC via uSDHC
    - SPI NOR/EEPROM via LPSPI
- 内置串行下载器，用来更新程序（不需要使用Jlink等调试器），支持 USB-HID、UART 两种下载通道
- 设备配置数据（DCD，BootROM 外设 / 内存初始化指令段）
- 基于数字签名的高可信安全启动（HAB，NXP 原厂安全启动框架）
- 外部内存配置数据（XMCD，DDR / 外部 RAM 时序初始化配置）
- 通过 FlexSPI 接口实现串行 NOR Flash 加密原地执行（XIP），由以下硬件引擎提供支持：
    - 内嵌式硬件加密引擎（IEE）
    - 实时 AES 硬件解密引擎（OTFAD，片上硬件流解密，读取 Flash 时自动解密）

芯片复位触发后，启动流程正式开始，硬件复位逻辑强制 ARM 内核从片内 BootROM 开始执行代码：

1. 首先 BootROM 根据熔丝位 `BT_CORE_SEL` 的配置决定从 M7 还是 M4 核启动：若 `BT_CORE_SEL = 1` （默认是 0），BootROM 将由 M4 内核执行，而非默认 M7 内核。因为 M4 运行 ROM 代码的执行效率更低，因此从 M4 内核启动的速度会更慢，若开启 HAB 高可信启动功能，耗时差异会尤为明显。

2. 接着 BootROM 代码会读取内部寄存器 BOOT_MODE[1:0] 的电平状态，同时结合各类 eFuse 熔丝配置、BOOT_CFG 引脚电平（开发阶段BOOT_CFG引脚电平可以覆盖eFuse设置），综合判定芯片的完整启动流程分支。

### Boot MODE

BOOT_MODE 是告诉 BootROM 本次启要执行什么“模式的行动”。RT1170 共提供 4 种启动模式：熔丝启动、串行下载器启动、片内内置启动、预留。各模式配置详见下表：

| BOOT_MODE[1:0] | 启动方式 | 说明 |
| --- | --- | --- |
| 00 | Boot From Fuses，从熔丝启动，一般用于量产阶段 | BootROM 完全忽略 BOOT_CFG 引脚配置，直接根据 eFuse 的配置决定如何启动：<br />1. BT_FUSE_SEL=0，自动进入串行下载，这个状态没有实际意义和使用价值，就算下载了程序，每次重启还是停留在串行下载，而不会运行Flash上的程序<br />2. BT_FUSE_SEL=1 ，根据 eFuse 进行启动，用于量产阶段 |
| 01 | Serial Downloader，串行下载 | 这是一个“救援”和“烧录”模式。BootROM 初始化 USB 或 UART1 接口，等待主机（你的电脑）通过特定协议发送数据并写入Flash |
| 10 | Internal Boot，从内部启动，一般用于开发阶段 | 根据 BT_FUSE_SEL 的值决定选择哪个配置来启动：<br />1. BT_FUSE_SEL=0，按照 BOOT_CFG 引脚启动，用于开发阶段<br />2. BT_FUSE_SEL=1，按照 eFuse 进行启动，类似量产阶段 |
| 11 | Reserved | 保留模式 |

启动模式 BOOT_MODE[1:0] 由片内 SRC_SBMR2 寄存器 BMOD[1:0] 位的数值决定。当上电复位信号 POR_B 上升沿阶段，芯片会采集 BOOT_MODE0（复用引脚GPIO_LPSR_02）与 BOOT_MODE1（复用引脚GPIO_LPSR_03）两个引脚的电平，以此完成 SRC_SBMR2.BMOD[1:0] 寄存器的初始化。引脚电平采集完成后，后续引脚状态变化将不再改变片内 SRC_SBMR2.BMOD[1:0] 寄存器的值。

| BOOT_MODE | 引脚 | 寄存器 | 说明 |
| --- | --- | --- | --- |
| BT_FUSE_SEL | / | SRC->SBMR2.BT_FUSE_SEL，bit20 | eFuse一次性烧入 |
| BOOT_MODE0 | GPIO_LPSR_02 | SRC->SBMR2.BMOD[0]，bit24 | 从引脚读取 |
| BOOT_MODE1 | GPIO_LPSR_03 | SRC->SBMR2.BMOD[1]，bit25 | 从引脚读取 |

> SRC_SBMR2 是 32 位只读寄存器，其中Bit25~24 => BMOD[1:0]，Bit20 => BT_FUSE_SEL

可以使用以下代码，读取启动模式 BOOT_MODE：

```c
// 读取启动模式
uint32_t sbmr2 = SRC->SBMR2;
uint8_t boot_mode = (sbmr2 >> 24U) & 0x3U; // 提取BMOD[1:0]

switch(boot_mode)
{
    case 0: // Boot From Fuses
        break;
    case 1: // Serial Downloader
        break;
    case 2: // Internal Boot
        break;
    case 3: // Reserved
        break;
}
```

## BOOT CFG

BOOT_CFG 是一组专用的GPIO引脚（例如RT1170上的GPIO_AD_00到GPIO_AD_07等）。在上电复位时，BootROM 会立刻采样这些引脚的电平状态，并将其解读为一组配置字。这组配置里面至少包含了以下关键信息：

- 启动设备类型：是从串行NOR Flash（FlexSPI）启动，还是从NAND Flash（SEMC），或者是从SD卡启动？
- 接口实例和配置：如果是FlexSPI，用的是哪个实例（FlexSPI1还是FlexSPI2），Flash是几线制的（1-bit, 2-bit, 4-bit, 8-bit）
- 其他设备参数：比如NAND Flash的页大小、ECC配置等。

另外芯片内部还有一块叫做 eFuse（一次性可编程熔丝）的存储区，其中有一部分 eFuses 位，其定义和 BOOT_CFG 引脚完全一样。在特定 BOOT_MODE 启动模式下，BOOT_CFG 能够 覆盖 eFuses 的配置：

- `BOOT_MODE=0b10 且 BT_FUSE_SEL=0`时，BOOT_CFG 引脚的电平覆盖 eFuses 的配置，BootROM 根据 BOOT_CFG 引脚启动，用电阻配置 BOOT_CFG 引脚，灵活方便，一般用户开发阶段。
- `BOOT_MODE=0b10 且 BT_FUSE_SEL=1`时，BootROM 不再看 BOOT_CFG 引脚了，直接读 eFuses 熔丝里的配置吧！ 把确定好的配置烧进 eFuses，这样就能防止生产焊接或用户误触改变启动方式，提高了可靠性，一般用于量产阶段。

## eFuse

eFUSE是 I.MXRT1170 内嵌的一块OTP(One time Programmable) memory，初始状态下所有bit均为0，仅能被烧写1次。eFUSE 的本质就是一块OTP（One Time Programmable）memory，也就是只可以烧写一次的储存区域，__初始状态下所有 bit 均为 0，仅能被烧写 1 次__。

RT1170 系列单片机的 eFUSE 有 8Kb，分为 32 个 Bank，每个 Bank 有 8 个 word(1个 word 为 4 字节)，即 32 字节。下图 0~0x8F 为 eFUSE 的 bank word 索引地址。

## Bootable Image

Bootable image通俗的来说就是APP镜像，里面集成了除了机器执行码（也就是我们经常说的bin文件）之外，还有其他各种信息。以STM32举例来说，通常我们只要将Bin文件下载到其内部的FLASH中，芯片上电后即可读取程序并运行，但是对于RT系列来说则是不不可以，必须将可执行文件加上其他信息（如FDCB, IVT,BD, DCD,CSF等，其中IVT和BD包含了image的目标地址和总长度）打包成规定格式的image文件下载进外部存储器中，RT才能顺利运行。（关于Bootable image的具体介绍大家可参考文章：痞子衡嵌入式：恩智浦i.MX RT1xxx系列MCU启动那些事（6）- Bootable image格式与加载(elftosb/.bd)…）
