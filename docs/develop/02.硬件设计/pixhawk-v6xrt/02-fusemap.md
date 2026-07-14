# eFUSE 定义

> **数据来源**：[i.MX RT1170 Processor Reference Manual](./IMXRT1170RM.pdf), Rev. 5, 01/2026
>
> - `Chapter 26 Fusemap` — 完整的 Fusemap 定义
> - `Chapter 27 On-Chip OTP Controller (OCOTP_CTRL)` — 烧写操作流程
> - 安全相关 fuse（HAB、SRK、CSF 等）位于 `Secure Reference Manual`，需联系 NXP FAE 获取

---

## 1. eFUSE 概览

I.MXRT1176 内嵌 **8 Kbit** OTP（One-Time Programmable）存储器，初始状态所有 bit 均为 `0`，仅能被烧写为 `1`（不可逆）。RT1170/1176 共用同一颗硅片，eFUSE 定义完全一致。

### 1.1 内存组织方式

| 项目 | 规格 |
| --- | --- |
| 总容量 | 8 Kbit = 1024 bytes = 256 words |
| Bank 数量 | 32 个 Bank |
| 每 Bank 字数 | 8 word |
| 每 Word 字节 | 4 字节（32-bit） |

> 注：1 word = 4 bytes = 32 bits

### 1.2 两类保护机制

| 保护类型 | 说明 | 烧写规则 |
| --- | --- | --- |
| **ECC 模式** | 32-bit 整体 ECC 校验，可纠正 1 bit 错误，检测 2 bit 错误 | **整个 word 只能烧写一次**（不能追加烧写不同 bit） |
| **RED模式** | bit[15:0] 自动复制到 bit[31:16]（冗余备份），读时 OR | **同一 word 可多次烧写不同 bit** |

### 1.3 地址编码说明

参考手册中有两套地址体系，指向同一块物理 eFUSE：

| 地址体系 | 示例 | 来源 | 用途 |
| --- | --- | --- | --- |
| **Fuse Address**（本文 §2 使用） | `0x840[2]`、`0x960[4]` | `Table 26-8. Fusemap Descriptions` | 精确到 bit，用于查表定位 |
| **Fuse Row Index**（图 Figure 27-2 使用） | `0x14`、`0x15`、`0x16` | OCOTP_CTRL Shadow Register | 影子寄存器行索引，软件读取用 |

> Fuse Address 编码：`0x800 + bank×0x100 + word×0x8 + bit_offset`

---

## 2. Fusemap 总表

### 2.1 Bank 0

系统配置 / ECC / 调试 / 锁定（0x800 ~ 0x8FF）

| Fuse Address | Fuse Name | 位数 | 保护 | 功能 | 设定值 |
| --- | --- | --- | --- | --- | --- |
| 0x800 ~ 0x83C | Reserved | — | — | 保留 | — |
| 0x840[2] | MECC_ENABLE | 1 | RED | 使能内部内存 ECC | 0=禁用, 1=使能 |
| 0x840[3] | XECC_ENABLE | 1 | RED | 使能外部内存 ECC | 0=禁用, 1=使能 |
| 0x840[15] | FLEXRAMECC_ENABLE | 1 | RED | 使能 FlexRAM ECC | 0=禁用, 1=使能 |
| 0x840[1:0], [14:4], [31:16] | Reserved | — | — | 保留 | — |
| 0x850 ~ 0x86C | Reserved | — | — | 保留 | — |
| 0x870[0] | M7_DEBUG_DISABLE | 1 | RED | M7 内核调试开关 | 0=使能调试, 1=禁用调试 |
| 0x870[1] | M4_DEBUG_DISABLE | 1 | RED | M4 内核调试开关 | 0=使能调试, 1=禁用调试 |
| 0x870[4] | FBB_DISABLE | 1 | RED | 禁用 FBB（Forward Body Bias） | 0=超频需FBB, 1=不需FBB |
| 0x870[3:2], [31:5] | Reserved | — | — | 保留 | — |
| 0x880 ~ 0x88C | Reserved | — | — | 保留 | — |
| 0x890[10] | GP1_RLOCK | 1 | RED | GP1 读锁定 | 0=可读, 1=锁定(不可读) |
| 0x890[11] | GP2_RLOCK | 1 | RED | GP2 读锁定 | 0=可读, 1=锁定(不可读) |
| 0x890[12] | GP3_RLOCK | 1 | RED | GP3 读锁定 | 0=可读, 1=锁定(不可读) |
| 0x890[13] | GP4_RLOCK | 1 | RED | GP4 读锁定 | 0=可读, 1=锁定(不可读) |
| 0x890[14] | GP5_RLOCK | 1 | RED | GP5 读锁定 | 0=可读, 1=锁定(不可读) |
| 0x890[9:0], [31:15] | Reserved | — | — | 保留 | — |
| 0x8A0 ~ 0x8FC | Reserved | — | — | 保留 | — |

### 2.2 Bank 1

UID / 硅片信息 / Boot 配置（0x900 ~ 0x9FF）

| Fuse Address | Fuse Name | 位数 | 保护 | 功能 | 设定值 |
| --- | --- | --- | --- | --- | --- |
| 0x900[31:0] + 0x910[10:0] | JTAG_CHALL[42:0] / UNIQUE_ID[42:0] | 43 | RED | JTAG Challenge / 唯一 ID（低 43 bit） | NXP 出厂烧写 |
| 0x910[15:11] | JTAG_CHALL[47:43] / UNIQUE_ID[47:43] | 5 | RED | JTAG Challenge / 唯一 ID（bit 47:43） | NXP 出厂烧写 |
| 0x910[23:16] | JTAG_CHALL[55:48] / UNIQUE_ID[55:48] | 8 | RED | JTAG Challenge / 唯一 ID（bit 55:48） | NXP 出厂烧写 |
| 0x910[31:24] | JTAG_CHALL[63:56] / UNIQUE_ID[63:56] | 8 | RED | JTAG Challenge / 唯一 ID（bit 63:56） | NXP 出厂烧写 |
| 0x920[27:24] | SI_REV[3:0] | 4 | ECC | 硅片版本号 | NXP 出厂烧写 |
| 0x920[31:28], [23:0] | Reserved | — | — | 保留 | — |
| 0x930[31:0] | Reserved | 32 | — | 保留 | — |
| 0x940[7:0] | **BOOT_CFG1[7:0]** | 8 | ECC | 启动配置 1 | 详见 [§3.1](#31-boot-cfg) |
| 0x940[15:8] | **BOOT_CFG2[7:0]** | 8 | ECC | 启动配置 2 | 详见 [§3.1](#31-boot-cfg) |
| 0x940[31:16] | Reserved | 16 | — | 保留 | — |
| 0x950[7:0] | **BOOT_CFG3[7:0]** | 8 | RED | 启动配置 3 | 参考 Boot Fusemap（按启动设备不同） |
| 0x950[15:8] | **BOOT_CFG4[7:0]** | 8 | RED | 启动配置 4 | 参考 Boot Fusemap（按启动设备不同） |
| 0x950[31:16] | Reserved | 16 | — | 保留 | — |
| 0x960[4] | **BT_FUSE_SEL** | 1 | RED | 决定 Boot 配置来源 | 0=取 GPIO 配置, 1=取 eFUSE 配置<br />详见 [§3.2](#32-bt-fuse) |
| 0x960[12] | **BT_CORE_SEL** | 1 | RED | 启动内核选择 | 0=从 M7 启动, 1=从 M4 启动<br />详见 [§3.2](#32-bt-fuse) |
| 0x960[3:0], [5], [11:6], [31:13] | Reserved | — | — | 保留 | — |
| 0x970[15:0] | **BOOT_PARAM1[15:0]** | 16 | RED | Boot 参数 1 | 详见 [§3.3](#33-boot-param) |
| 0x970[31:16] | Reserved | 16 | — | 保留 | — |
| 0x980[15:0] | **BOOT_PARAM2[15:0]** | 16 | RED | Boot 参数 2 | 详见 [§3.3](#33-boot-param) |
| 0x980[31:16] | Reserved | 16 | — | 保留 | — |
| 0x990[15:0] | **BOOT_PARAM3[15:0]** | 16 | RED | Boot 参数 3 | 详见 [§3.3](#33-boot-param) |
| 0x990[31:16] | Reserved | 16 | — | 保留 | — |
| 0x9A0[15:0] | **BOOT_PARAM4[15:0]** | 16 | RED | Boot 参数 4 | 详见 [§3.3](#33-boot-param) |
| 0x9A0[31:16] | Reserved | 16 | — | 保留 | — |
| 0x9B0[15:0] | **BOOT_PARAM5[15:0]** | 16 | RED | Boot 参数 5 | 详见 [§3.3](#33-boot-param) |
| 0x9B0[31:16] | Reserved | 16 | — | 保留 | — |

### 2.3 Bank 2

温度传感器 / MAC 地址 / USB ID（0xA00 ~ 0xAFF）

| Fuse Address | Fuse Name | 位数 | 保护 | 功能 | 设定值 |
| --- | --- | --- | --- | --- | --- |
| 0xA00[31:0] ~ 0xA10[31:0] | Reserved | — | — | 保留 | — |
| 0xA20[27:16] | TMPSNS_TEMP_VAL[11:0] | 12 | RED | 温度传感器 25°C 校准值 | NXP 出厂烧写 |
| 0xA20[31:28], [15:0] | Reserved | — | — | 保留 | — |
| 0xA30[31:0] ~ 0xA70[31:0] | Reserved | — | — | 保留 | — |
| 0xA80[31:0] + 0xA90[15:0] | **MAC1_ADDR[47:0]** | 48 | ECC | 以太网 1 MAC 地址 | 用户自定义 |
| 0xA90[31:16] | Reserved | 16 | — | 保留 | — |
| 0xAA0[31:0] + 0xAB0[15:0] | **MAC2_ADDR[47:0]** | 48 | ECC | 以太网 2 MAC 地址 | 用户自定义 |
| 0xAB0[31:16] | Reserved | 16 | — | 保留 | — |
| 0xAC0[31:0] + 0xAD0[15:0] | **MAC3_ADDR[47:0]** | 48 | ECC | 以太网 3 MAC 地址 | 用户自定义 |
| 0xAD0[31:16] | Reserved | 16 | — | 保留 | — |
| 0xAE0[15:0] | **USB_VID[15:0]** | 16 | ECC | USB Vendor ID | 用户自定义 |
| 0xAE0[31:16] | **USB_PID[15:0]** | 16 | ECC | USB Product ID | 用户自定义 |
| 0xAF0[31:0] | Reserved | 32 | — | 保留 | — |

### 2.4 Bank 3

保留区（0xB00 ~ 0xBFF）

| Fuse Address | Fuse Name | 位数 | 功能 |
| --- | --- | --- | --- |
| 0xB00[31:0] ~ 0xBF0[31:0] | Reserved | — | 保留 |

### 2.5 Bank 4~7

Boot Config MISC（0xC00 ~ 0xCFF）

| Fuse Address | Fuse Name | 位数 | 保护 | 功能 | 设定值 |
| --- | --- | --- | --- | --- | --- |
| 0xC00[31:0] ~ 0xC6C[31:0] | Reserved | — | — | 保留 | — |
| 0xC70[7:0] | Reserved | 8 | — | 保留 | — |
| 0xC70[15:8] | BOOT_CONFIG_MISC1[15:8] | 8 | ECC | PAD_SETTINGS（覆盖 IO PAD 驱动强度 DSE/PDRV） | 参考 Boot Fusemap |
| 0xC70[21:16] | **Default_FlexRAM_Part[5:0]** | 6 | ECC | 默认 FlexRAM 分区配置 | 详见 [§3.4](#34-flexram-part) |
| 0xC70[31:22] | BOOT_CONFIG_MISC1[31:22] | 10 | ECC | 其他 misc 启动配置 | 参考 Boot Fusemap |
| 0xC80[7:0] | **BOOT_CONFIG_MISC2[7:0]** | 8 | ECC | FlexSPI NOR Flash 配置字（低字节） | 详见 [§3.5](#35-boot-config-misc2) |
| 0xC80[15:8] | BOOT_CONFIG_MISC2[15:8] | 8 | ECC | Flash 镜像大小 + Dummy Cycle | 详见 [§3.5](#35-boot-config-misc2) |
| 0xC80[23:16] | BOOT_CONFIG_MISC2[23:16] | 8 | ECC | 第二镜像偏移 | 详见 [§3.5](#35-boot-config-misc2) |
| 0xC80[31:24] | BOOT_CONFIG_MISC2[31:24] | 8 | ECC | Reserved | — |
| 0xC90[31:0] | **BOOT_CONFIG_MISC3[31:0]** | 32 | ECC | XMC 完整性校验 CRC / 其他 misc 配置 | 参考 Boot Fusemap |
| 0xCA0[31:0] | **BOOT_CONFIG_MISC4[31:0]** | 32 | ECC | 其他 misc 启动配置 | 参考 Boot Fusemap |
| 0xCB0[31:0] ~ 0xCF0[31:0] | Reserved | — | — | 保留 | — |

### 2.6 Bank 8~12

保留区（0x1000 ~ 0x12FF）

| Fuse Address | Fuse Name | 位数 | 功能 |
| --- | --- | --- | --- |
| 0x1000[31:0] ~ 0x12F0[31:0] | Reserved | — | 保留 |

### 2.7 Bank 13~17

通用 eFUSE 寄存器 GP1 ~ GP5（0x1300 ~ 0x17FF）

| Fuse Address | Fuse Name | 位数 | 保护 | 功能 | 读锁 |
| --- | --- | --- | --- | --- | --- |
| 0x1300[31:0] ~ 0x13F0[31:0] | **GP1[511:0]** | 512 (16 words) | ECC | 通用用户 eFUSE 寄存器 #1 | GP1_RLOCK |
| 0x1400[31:0] ~ 0x14F0[31:0] | **GP2[511:0]** | 512 (16 words) | ECC | 通用用户 eFUSE 寄存器 #2 | GP2_RLOCK |
| 0x1500[31:0] ~ 0x15F0[31:0] | **GP3[511:0]** | 512 (16 words) | ECC | 通用用户 eFUSE 寄存器 #3 | GP3_RLOCK |
| 0x1600[31:0] ~ 0x16F0[31:0] | **GP4[511:0]** | 512 (16 words) | ECC | 通用用户 eFUSE 寄存器 #4 | GP4_RLOCK |
| 0x1700[31:0] ~ 0x17F0[31:0] | **GP5[511:0]** | 512 (16 words) | ECC | 通用用户 eFUSE 寄存器 #5 | GP5_RLOCK |

> GP1~GP5 是用户可自由使用的通用 eFUSE 区域，可用于存储序列号、校准参数、配置标记等自定义数据。每个 GP 区域由对应的 `GPx_RLOCK` 位控制读保护。

### 2.8 Bank 18

Boot Config MISC5（0x1800 ~ 0x18FF）

| Fuse Address | Fuse Name | 位数 | 保护 | 功能 |
| --- | --- | --- | --- | --- |
| 0x1800[31:0] ~ 0x18F0[31:0] | **BOOT_CONFIG_MISC5[511:0]** | 512 (16 words) | ECC | 扩展 Boot 配置（参考 Boot Fusemap） |

### 2.9 Bank 19~31

NXP 保留区（0x1900 ~ 0x1FFF）

| Fuse Address | Fuse Name | 位数 | 功能 |
| --- | --- | --- | --- |
| 0x1900[31:0] ~ 0x1FF0[31:0] | Reserved | 全部保留 | NXP 内部保留，用户不得烧写 |

> 参考手册仅定义至 Bank 18（0x1800），Bank 19~31 合计 13 bank × 8 word = 104 word 均标记为 Reserved，无任何公开的 fuse 定义。根据 RM 中"Customers must not attempt to burn these, because the IC behavior may be unpredictable"的明确警告，这些地址不应被用户烧写。

---

## 3. 关键寄存器详解

### 3.1 BOOT_CFG1 / 2（0x940） {#31-boot-cfg}

这是最核心的启动配置寄存器，决定了启动设备类型、Flash 接口参数。

#### BOOT_CFG1（0x940[7:0]）

| Bit | 名称 | 功能 | 设定值 |
| --- | --- | --- | --- |
| 0 | FLASH_AUTO_PROBE_EN | xSPI Flash 自动探测使能 | 0=禁用, 1=使能 |
| 1 | ENCRYPTED_XIP_EN | 加密 XIP 使能 | 0=禁用, 1=使能 |
| 3:2 | FLASH_PROBE_TYPE | xSPI Flash 自动探测类型 | 0=QuadSPI NOR, 1=MXIC Octal, 2=Micron Octal, 3=Adesto Octal |
| 7:4 | BOOT_DEVICE | **启动设备选择** | 0000=FlexSPI(Serial NOR), 01xx=SD, 10xx=MMC/eMMC, 001x=SEMC(NAND), 11xx=FlexSPI(Serial NAND) |

#### BOOT_CFG2（0x940[15:8]）

| Bit | 名称 | 功能 | 设定值 |
| --- | --- | --- | --- |
| 2:0 | xSPI_FLASH_TYPE | Flash 类型 | 000=默认0x03读命令, 001=保留, 010=HyperFlash 1.8V, 011=HyperFlash 3.0V, 100=MXIC Octal Read, 101=Micron Octal Read |
| 3 | FLEXSPI_INSTANCE | FlexSPI 实例选择 | 0=FlexSPI1, 1=FlexSPI2 |
| 7:4 | Reserved | 保留 | — |

### 3.2 BT_FUSE_SEL / BT_CORE_SEL（0x960） {#32-bt-fuse}

这两个 bit 位于 `0x960`，是决定启动流程走向的**全局开关**，优先级高于 BOOT_CFG 引脚和 BOOT_CFG1/2 的具体设备配置。

| Fuse Address | Bit | 名称 | 功能 | 设定值 |
| --- | --- | --- | --- | --- |
| 0x960[4] | 4 | **BT_FUSE_SEL** | Boot 配置来源选择 | 0=从 GPIO 引脚取配置, 1=从 eFUSE 取配置 |
| 0x960[12] | 12 | **BT_CORE_SEL** | 启动内核选择 | 0=从 M7 内核启动, 1=从 M4 内核启动 |

#### BT_FUSE_SEL（0x960[4]）

`BT_FUSE_SEL` 的值在不同 `BOOT_MODE` 下有不同含义：

| BOOT_MODE[1:0] | BT_FUSE_SEL | 行为 |
| --- | --- | --- |
| 00（Boot From Fuses） | 0 | BootROM 自动进入 Serial Downloader 模式（等同于 BOOT_MODE=01），**不会运行 Flash 上的程序** |
| 00（Boot From Fuses） | 1 | BootROM 根据 eFUSE 中其他 Boot 配置信息选择启动设备并 Boot |
| 10（Internal Boot） | 0 | BootROM 根据 BOOT_CFG 引脚电平决定启动设备，**BOOT_CFG 引脚配置覆盖 eFUSE**（开发阶段常用） |
| 10（Internal Boot） | 1 | BootROM 完全忽略 BOOT_CFG 引脚，**仅**根据 eFUSE 配置选择启动设备（量产阶段推荐） |
| 01（Serial Downloader） | 任意 | BootROM 始终进入串行下载模式，不受 BT_FUSE_SEL 影响 |
| 11（Reserved） | 任意 | 保留 |

> **量产关键**：产品定型后应将 `BT_FUSE_SEL=1` 烧入 eFUSE，防止 BOOT_CFG 引脚被外部干扰改变启动行为。同时建议将 `BOOT_MODE` 引脚固定为 `10`（Internal Boot）。

#### BT_CORE_SEL（0x960[12]）

| BT_CORE_SEL | 启动内核 | 说明 |
| --- | --- | --- |
| 0（默认） | **Cortex-M7** | 默认启动核，性能更高，适合运行 RTOS / 飞控固件 |
| 1 | **Cortex-M4** | 从 M4 启动，ROM 代码执行效率更低，启动速度更慢（开启 HAB 安全启动时耗时差异尤为明显） |

> 仅多核型号（如 RT1176）有此 fuse。上电后 BootROM 先根据 `BT_CORE_SEL` 决定在哪个核上执行 ROM 代码，执行完毕后将 `0x0000_0000 ~ 0x0003_FFFF` 重映射为 ITCM 供应用程序使用。

### 3.3 BOOT_PARAM1 ~ 5（0x970 ~ 0x9B0） {#33-boot-param}

这些参数的含义随启动设备（FlexSPI NOR / SD / MMC / NAND）不同而变化。以下是通用字段和 FlexSPI NOR 模式下的定义。

#### BOOT_PARAM1（0x970[15:0]）

| Bit | 名称 | 功能 | 设定值 |
| --- | --- | --- | --- |
| 0~7 | (按启动设备) | 设备相关参数 | 参考各设备的 Boot Fusemap |
| 14 | XMC_CHK_EN | XMC 完整性校验使能 | 0=禁用, 1=使能 |
| 15 | (按启动设备) | 设备相关参数 | 参考各设备的 Boot Fusemap |

#### BOOT_PARAM2（0x980[15:0]）

| Bit | 名称 | 功能 | 设定值 |
| --- | --- | --- | --- |
| 0~15 | (按启动设备) | 设备相关参数 | 参考各设备的 Boot Fusemap |

#### BOOT_PARAM3（0x990[15:0]）

| Bit | 名称 | 功能 | 设定值 |
| --- | --- | --- | --- |
| 0 | BOOT_FAIL_IND_EN | Boot 失败指示使能 | 0=禁用, 1=使能 |
| 5:1 | BOOT_FAIL_IND_PIN_SEL[4:0] | Boot 失败指示引脚选择 | 00000=GPIO1.IO00, ..., 11111=GPIO1.IO31 |
| 6 | Override Pad Settings | 覆盖 PAD 驱动配置（使用 PAD_SETTINGS 值） | 0=禁用, 1=使能 |
| 14:8 | SDMMC_DLL_DLY[6:0] / DELAY_CELL_NUM | SD/eMMC DLL 延时 / FlexSPI DLL 覆盖 | 0=禁用DLL覆盖, 1~127=DLL覆盖目标值 |
| 15 | (按启动设备) | — | — |

#### BOOT_PARAM4（0x9A0[15:0]）

| Bit | 名称 | 功能 | 设定值 |
| --- | --- | --- | --- |
| 0 | SDP_DISABLE | 禁用串行下载（Serial Downloader） | 0=使能SDP, 1=禁用SDP |
| 1 | UART_SDP_DISABLE | 禁用 UART 串行下载 | 0=使能UART SDP, 1=禁用UART SDP |
| 2 | OSC_REF_FREQ | 晶振参考频率 | 0=24MHz, 1=19.2MHz |
| 3 | BOOT_FREQ (M7) | M7 启动频率 | 0=400MHz, 1=700MHz（M4: 0=200MHz, 1=240MHz） |
| 5:4 | LPB_BOOT | Loop Back Boot 分频 | 00=Div1, 01=Div2, 10=Div4, 11=Div8 |
| 6 | D_CACHE_DISABLE | 禁用 D-Cache | 0=使能, 1=禁用 |
| 7 | I_CACHE_DISABLE | 禁用 I-Cache | 0=使能, 1=禁用 |
| 8 | FLASH_CONNECTION_SEL | FlexSPI Flash 连接方式 | 0=PORTA, 1=Parallel Mode, 2=PORTB |
| 9 | FLEXSPI_PIN_GROUP_SEL | FlexSPI 引脚组选择 | 0=Primary, 1=Secondary |
| 10 | FLEXSPI_DQS_PIN_SEL | FlexSPI DQS 引脚选择 | 0=Primary DQS, 1=Secondary DQS |
| 11 | FORCE_INTERNAL_BOOT | 强制内部启动模式（忽略 BOOT_MODE 引脚） | 0=由BOOT_MODE引脚决定, 1=强制Internal Boot |
| 12 | WDOG_ENABLE | WDOG1 使能 | 0=禁用, 1=使能 |
| 13~15 | Reserved | 保留 | — |

#### BOOT_PARAM5（0x9B0[15:0]）

| Bit | 名称 | 功能 | 设定值 |
| --- | --- | --- | --- |
| 0 | WDOG_B_PIN_EN | WDOG_B 引脚使能 | 0=禁用, 1=使能 |
| 2:1 | WDOG_B_PIN_SEL | WDOG_B 引脚选择 | 0=GPIO_DISP_B2_00, 1=GPIO_DISP_B2_15, 2=GPIO_AD_04, 3=GPIO_EMC_B2_18 |
| 5:3 | WDOG_TIMEOUT_SEL | WDOG 超时时间 | 000=64s, 001=32s, 010=16s, 011=8s |
| 6~15 | Reserved | 保留 | — |

### 3.4 Default_FlexRAM_Part（0xC70[21:16]） {#34-flexram-part}

FlexRAM 默认分区配置，控制 ITCM / DTCM / OCRAM 的大小分配。**常用配置：**

| 编码 | OCRAM | ITCM | DTCM | 适用场景 |
| --- | --- | --- | --- | --- |
| 0b000000 | 256KB | 256KB | 0KB | 默认值，均衡分配 |
| 0b001001 | 320KB | 256KB | 64KB | M7 大 ITCM |
| 0b010001 | 384KB | 192KB | 64KB | 大 OCRAM |
| 0b100010 | 384KB | 0KB | 256KB | 大 DTCM（M4/M7 数据密集型） |

> 完整 64 种分区组合见参考手册 `FlexRAM Partition Table`。

### 3.5 BOOT_CONFIG_MISC2（0xC80） {#35-boot-config-misc2}

当 BOOT_DEVICE = `0000`（FlexSPI Serial NOR）时，0xC80 提供完整的 Flash 参数配置。

| Fuse Address | Bit(s) | 名称 | 功能 | 设定值 |
| --- | --- | --- | --- | --- |
| 0xC80[0] | 0 | RESET_PIN_EN | 使能 Flash 复位引脚 | 0=禁用, 1=使能 |
| 0xC80[1] | 1 | JEDEC_HW_RESET_EN | 使能 JEDEC 硬件复位 | 0=禁用, 1=使能 |
| 0xC80[2] | 2 | RESET_PIN_SEL | 复位引脚选择 | 0=GPIO4[3], 1=GPIO2[8] |
| 0xC80[4:3] | 4:3 | HOLD_TIME | Flash 复位保持时间 | 0=500µs, 1=1ms, 2=3ms, 3=10ms |
| 0xC80[7:5] | 7:5 | xSPI_FLASH_FREQ | Flash 工作频率 | 0=100MHz, 1=120MHz, 2=133MHz, 3=166MHz, 5=80MHz, 6=60MHz |
| 0xC80[11:8] | 11:8 | xSPI_FLASH_IMG_SIZE | 主镜像大小 | 0=由SEC_IMG_OFFSET指定, 1~12=1~12MB, 13=256KB, 14=512KB, 15=768KB |
| 0xC80[15:12] | 15:12 | xSPI_FLASH_DUMMY_CYCLE | Flash 读命令 Dummy Cycle 数 | 0=自动探测, 其他=用户指定值 |
| 0xC80[23:16] | 23:16 | xSPI_FLASH_SEC_IMG_OFFSET | 第二镜像偏移量 | fuse值 × 256KB |
| 0xC80[31:24] | 31:24 | Reserved | 保留 | — |

---

## 4. 影子寄存器内存映射

OCOTP 外设在上电复位后自动将 eFUSE 内容加载到影子寄存器，软件通过读取影子寄存器获取 fuse 值（无需直接读 fusebox）。

以下是 RM Figure 27-2（OTP Memory Footprint）与 `Table 26-8. Fusemap Descriptions` 的 Row Index ↔ Fuse Address 完整对照：

| Row Index | 名称 | Fuse Address | 所属 Bank | 功能 | 保护模式 |
| --- | --- | --- | --- | --- | --- |
| 0x00 ~ 0x02 | RESERVED | 0x800 ~ 0x830 | Bank 0 | 保留 | — |
| 0x03 | ECC_ENABLE | 0x840 | Bank 0 | MECC_ENABLE / XECC_ENABLE / FLEXRAMECC_ENABLE | RED |
| 0x04 ~ 0x06 | RESERVED | 0x850 ~ 0x860 | Bank 0 | 保留 | — |
| 0x07 | DEBUG_DISABLE | 0x870 | Bank 0 | M7/M4_DEBUG_DISABLE / FBB_DISABLE | RED |
| 0x08 | RLOCK | 0x890 | Bank 0 | GP1~GP5_RLOCK | RED |
| 0x09 ~ 0x0F | RESERVED | 0x8A0 ~ 0x8F0 | Bank 0 | 保留 | — |
| 0x10 ~ 0x11 | UID | 0x900 ~ 0x910 | Bank 1 | JTAG_CHALL[63:0] / UNIQUE_ID[63:0] | RED |
| 0x12 | SI_REV / TESTER | 0x920 | Bank 1 | SI_REV[3:0] | ECC |
| 0x13 | RESERVED | 0x930 | Bank 1 | 保留 | — |
| 0x14 | BOOT_CFG | 0x940 | Bank 1 | BOOT_CFG1[7:0] / BOOT_CFG2[7:0] | ECC |
| 0x15 | BOOT_CFG | 0x950 | Bank 1 | BOOT_CFG3[7:0] / BOOT_CFG4[7:0] | RED |
| 0x16 | BOOT_OPT | 0x960 | Bank 1 | BT_FUSE_SEL / BT_CORE_SEL | RED |
| 0x17 | BOOT_PARAM | 0x970 | Bank 1 | BOOT_PARAM1[15:0] | RED |
| 0x18 | BOOT_PARAM | 0x980 | Bank 1 | BOOT_PARAM2[15:0] | RED |
| 0x19 | BOOT_PARAM | 0x990 | Bank 1 | BOOT_PARAM3[15:0] | RED |
| 0x1A | BOOT_PARAM | 0x9A0 | Bank 1 | BOOT_PARAM4[15:0] | RED |
| 0x1B | BOOT_PARAM | 0x9B0 | Bank 1 | BOOT_PARAM5[15:0] | RED |
| 0x1C ~ 0x27 | RESERVED | 0xA00 ~ 0xA70 | Bank 2 | 保留（含 TMPSNS_TEMP_VAL @ 0xA20） | — |
| 0x28 ~ 0x29 | MAC1 | 0xA80 ~ 0xA90 | Bank 2 | MAC1_ADDR[47:0] | ECC |
| 0x2A ~ 0x2B | MAC2 | 0xAA0 ~ 0xAB0 | Bank 2 | MAC2_ADDR[47:0] | ECC |
| 0x2C ~ 0x2D | MAC3 | 0xAC0 ~ 0xAD0 | Bank 2 | MAC3_ADDR[47:0] | ECC |
| 0x2E | USB ID | 0xAE0 | Bank 2 | USB_VID[15:0] / USB_PID[15:0] | ECC |
| 0x2F | RESERVED | 0xAF0 ~ 0xBF0 | Bank 2~3 | 保留 | — |
| 0x30 ~ 0x46 | RESERVED | 0xC00 ~ 0xC60 | Bank 4~6 | 保留 | — |
| 0x47 | BOOT_CFG_MISC | 0xC70 | Bank 7 | PAD_SETTINGS / Default_FlexRAM_Part[5:0] | ECC |
| 0x48 | BOOT_CFG_MISC | 0xC80 | Bank 7 | FlexSPI NOR Flash 参数配置 | ECC |
| 0x49 | BOOT_CFG_MISC | 0xC90 | Bank 7 | BOOT_CONFIG_MISC3（XMC CRC 等） | ECC |
| 0x4A | BOOT_CFG_MISC | 0xCA0 | Bank 7 | BOOT_CONFIG_MISC4 | ECC |
| 0x4B ~ 0x4F | RESERVED | 0xCB0 ~ 0xCF0 | Bank 7~8 | 保留 | — |
| 0x50 ~ 0x57 | (GP1) | 0x1300 ~ 0x13F0 | Bank 13 | GP1[511:0] | ECC |
| 0x58 ~ 0x5F | (GP2) | 0x1400 ~ 0x14F0 | Bank 14 | GP2[511:0] | ECC |
| 0x60 ~ 0x67 | (GP3) | 0x1500 ~ 0x15F0 | Bank 15 | GP3[511:0] | ECC |
| 0x68 ~ 0x6F | (GP4) | 0x1600 ~ 0x16F0 | Bank 16 | GP4[511:0] | ECC |
| 0x70 ~ 0x77 | (GP5) | 0x1700 ~ 0x17F0 | Bank 17 | GP5[511:0] | ECC |
| 0x78 ~ 0x7F | (BOOT_CFG_MISC5) | 0x1800 ~ 0x18F0 | Bank 18 | BOOT_CONFIG_MISC5[511:0] | ECC |
| 0x80 ~ 0x8F | RESERVED | 0x1900 ~ 0x1FF0 | Bank 19~31 | NXP 保留，禁止烧写 | — |
| 0xB0 ~ 0x110 | (补充 fuse，无影子寄存器) | 需通过 `READ_FUSE` 直接读 | — | NXP 内部使用 | — |

> **地址换算规律**：Fuse Address 的 `[10:8]` = Bank 号，`[7:5]` = Word 号，`[4:0]` = bit 偏移。更直接的换算：`Row_Index = (Fuse_Address - 0x800) / 8`（当 Fuse Address 对齐到 word 起始时）。
>
> **补充 fuse**：Row Index `0xB0~0x110` 区域存放额外 fuse（不映射到影子寄存器，需通过 `READ_FUSE` 命令直接读取）。

---

## 5. 烧写注意事项

1. **ECC 保护的 word 只能烧写一次**：整个 32-bit word 必须在一次操作中烧写完，因为 ECC 校验码随数据一起计算。分两次烧写同一 word 会导致 ECC 错误。

2. **Redundancy 保护的 word 可分批烧写**：可以先烧写部分 bit，后续再追加烧写其他 bit（不能将已烧为 `1` 的 bit 改回 `0`）。

3. **烧写前必须解锁**：向 `CTRL[WR_UNLOCK]` 写入解锁码 `0x3E77`。

4. **烧写后等待**：烧写完成后需等待 `CTRL[BUSY]` 清零，再等待 **2µs** 才能进行下一次 OTP 操作。

5. **LOCK 位不可逆**：一旦设置了 GPx_RLOCK 或安全锁定，对应区域将永久不可读取/烧写。

6. **安全 fuse**：HAB、SRK、CSF 等安全启动相关 fuse 在 `Secure Reference Manual` 中定义，需联系 NXP FAE 签署 NDA 后获取。

---

## 6. 代码读取 eFUSE

通过 OCOTP 影子寄存器读取 fuse 值（以 BOOT_MODE 相关 fuse 为例）：

```c
// OCOTP 基地址
#define OCOTP_BASE 0x40CA8000

// 影子寄存器偏移（word 地址 -> 寄存器偏移）
// BOOT_CFG1/2 位于 Row 0x14，对应 OCOTP 寄存器偏移 0x50
#define OCOTP_BOOT_CFG12_OFFSET 0x50
// BOOT_OPT 位于 Row 0x16，对应 OCOTP 寄存器偏移 0x58
#define OCOTP_BOOT_OPT_OFFSET   0x58

void read_boot_fuses(void)
{
    uint32_t boot_cfg12 = *(volatile uint32_t *)(OCOTP_BASE + OCOTP_BOOT_CFG12_OFFSET);
    uint32_t boot_opt   = *(volatile uint32_t *)(OCOTP_BASE + OCOTP_BOOT_OPT_OFFSET);

    uint8_t boot_cfg1   = (boot_cfg12 >> 0)  & 0xFF;   // BOOT_CFG1[7:0]
    uint8_t boot_cfg2   = (boot_cfg12 >> 8)  & 0xFF;   // BOOT_CFG2[7:0]
    uint8_t boot_device = (boot_cfg1 >> 4)   & 0x0F;   // BOOT_DEVICE[3:0]
    uint8_t bt_fuse_sel = (boot_opt >> 4)    & 0x01;   // BT_FUSE_SEL
    uint8_t bt_core_sel = (boot_opt >> 12)   & 0x01;   // BT_CORE_SEL

    printf("BOOT_CFG1    = 0x%02X\n", boot_cfg1);
    printf("  BOOT_DEVICE  = %d\n", boot_device);
    printf("BOOT_CFG2    = 0x%02X\n", boot_cfg2);
    printf("BT_FUSE_SEL  = %d\n", bt_fuse_sel);
    printf("BT_CORE_SEL  = %d\n", bt_core_sel);
}
```

> **注意**：完整的 OCOTP 影子寄存器地址映射请参考 `Chapter 27 OCOTP_CTRL` 寄存器描述。

---

*本文档基于 i.MX RT1170 Processor Reference Manual, Rev. 5, 01/2026 整理。*
