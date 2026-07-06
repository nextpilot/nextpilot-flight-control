# 系统启动

[i.MX RT1170 Processor Reference Manual](./IMXRT1170RM.pdf)的`Chapter 10 System Boot`介绍了RT1170系统启动流程。

对于I.MXRT1XXX系列单片机来说，上电永远是从 BootROM 开始启动，BootROM 根据BOOT_MODE、eFuse、BOOT_CFG GPIO等决定的不同行为模式。

## Boot ROM

BootROM 其实是芯片在出厂前固化在ROM里的一段启动程序（不可改变和擦除），它的目的就相当于一个引导程序。

!!! note
    BootROM 是工厂一次性掩膜固化的片内 ROM，物理只读，没有擦写电路，任何工具（J-Link、MfgTool、MCUBootUtility、SDP 协议）都无法修改、升级、替换 BootROM 固件，__也就是出厂版本终身固定__。不过 NXP 给 i.MX RT 系列预留了eFuse ROM 补丁入口，仅用于修复 ROM 少量已知硬件 Bug，属于运行时内存替换，无法整体更新 BootROM。

i.MX RT1170系列 BootROM 物理内存映射地址范围：`0x0000_0000 ~ 0x0003_FFFF`（共 `0x40000` 字节，256 KB），注意 BootROM 执行完毕后会将`0x0000_0000 ~ 0x0003_FFFF`重新映射为 `ITCM`  使用。

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

2. 接着 BootROM 代码会读取内部寄存器 BOOT_MODE[1:0] 的电平状态，同时结合各类 eFuse 熔丝配置、GPIO 引脚电平，综合判定芯片的完整启动流程分支。

### Boot MODE

| BOOT_MODE[1:0] | Boot Type |
| --- | --- |
| 00 | Boot From Fuses，根据eFuse设置进行系统启动，忽略GPIO配置，BT_FUSE_SEL=0表示没有烧写固件，会自动进入串行下载 |
| 01 | Serial Downloader，通过UART1或者USB下载程序 |
| 10 | Internal Boot，根据BT_FUSE_SEL=1按照eFuse系统启动，BT_FUSE_SEL=0按照GPIO配置系统启动 |
| 11 | Reserved |

## eFuse

FUSE的本质就是一块OTP（One Time Programmable）memory，也就是只可以烧写一次的储存区域。启动需要的配置信息就可以存储在里面。（关于FUSE具体介绍大家可参考文章：痞子衡嵌入式：恩智浦i.MX RTxxx系列MCU启动那些事（4）- OTP及其烧写方法）

## Bootable image

Bootable image通俗的来说就是APP镜像，里面集成了除了机器执行码（也就是我们经常说的bin文件）之外，还有其他各种信息。以STM32举例来说，通常我们只要将Bin文件下载到其内部的FLASH中，芯片上电后即可读取程序并运行，但是对于RT系列来说则是不不可以，必须将可执行文件加上其他信息（如FDCB, IVT,BD, DCD,CSF等，其中IVT和BD包含了image的目标地址和总长度）打包成规定格式的image文件下载进外部存储器中，RT才能顺利运行。（关于Bootable image的具体介绍大家可参考文章：痞子衡嵌入式：恩智浦i.MX RT1xxx系列MCU启动那些事（6）- Bootable image格式与加载(elftosb/.bd)…）
