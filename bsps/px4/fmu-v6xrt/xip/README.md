# flexspi_nor_config

`flexspi_nor_config_t` 是 NXP 统一描述 SPI NOR Flash（QSPI/OPI Octal）硬件时序、指令、容量、工作模式的结构体。所有 FlexSPI 读写、擦除、XIP 启动、ROM Boot、FlashLoader、MCUboot 都依赖它，核心分为两大场景：

- BootROM 启动阶段：FCB 块本质就是固化后的 flexspi_nor_config，ROM 靠它初始化 FlexSPI 完成 XIP 启动；
- 软件运行时（APP/MCUboot）：驱动读写 Flash 用。

pixhawk v6xrt 板载的 NorFlash 是 旺宏的 MX25UM51345G （1.8V Octal SPI，OPI DDR），MX25UM51345G flash 的 flexspi_nor_config 配置可以参考以下内容：

- [mcux-sdk/evkmimxrt595/flash_config/flash_config.c](https://github.com/NXPmicro/mcux-sdk/blob/main/boards/evkmimxrt595/flash_config/flash_config.c)
- [mcux-sdk/evkmimxrt685/flash_config/flash_config.c](https://github.com/nxp-mcuxpresso/legacy-mcux-sdk/blob/main/boards/evkmimxrt685/flash_config/flash_config.c)
- [PX4-Autopilot/boards/px4/fmu-v6xrt/src/imxrt_flexspi_nor_flash.c](https://github.com/PX4/PX4-Autopilot/blob/main/boards/px4/fmu-v6xrt/src/imxrt_flexspi_nor_flash.c)

## nxp/evkmimxrt595

[evkmimxrt595/flash_config/flash_config.c](https://github.com/NXPmicro/mcux-sdk/blob/main/boards/evkmimxrt595/flash_config/flash_config.c)，该文件内置完整 const flexspi_nor_config_t mx25um51345g_config，是 NXP 原厂出厂标准配置，包含 FCB 所需全部时序、LUT 指令、OPI-DDR 参数。

```c
const flexspi_nor_config_t flash_config = {
    .memConfig =
        {
            .tag                 = FLEXSPI_CFG_BLK_TAG,
            .version             = FLEXSPI_CFG_BLK_VERSION,
            .readSampleClkSrc    = kFlexSPIReadSampleClk_ExternalInputFromDqsPad,
            .csHoldTime          = 3,
            .csSetupTime         = 3,
            .deviceModeCfgEnable = 1,
            .deviceModeType      = kDeviceConfigCmdType_Spi2Xpi,
            .waitTimeCfgCommands = 1,
            .deviceModeSeq =
                {
                    .seqNum   = 1,
                    .seqId    = 6, /* See Lookup table for more details */
                    .reserved = 0,
                },
            .deviceModeArg = 2, /* Enable OPI DDR mode */
            .controllerMiscOption =
                (1u << kFlexSpiMiscOffset_SafeConfigFreqEnable) | (1u << kFlexSpiMiscOffset_DdrModeEnable),
            .deviceType      = kFlexSpiDeviceType_SerialNOR,
            .sflashPadType   = kSerialFlash_8Pads,
            .serialClkFreq   = kFlexSpiSerialClk_60MHz,
            .sflashA1Size    = 64ul * 1024u * 1024u,
            .busyOffset      = 0u,
            .busyBitPolarity = 0u,
            .lookupTable =
                {
                    /* Read */
                    [0] = FLEXSPI_LUT_SEQ(CMD_DDR, FLEXSPI_8PAD, 0xEE, CMD_DDR, FLEXSPI_8PAD, 0x11),
                    [1] = FLEXSPI_LUT_SEQ(RADDR_DDR, FLEXSPI_8PAD, 0x20, DUMMY_DDR, FLEXSPI_8PAD, 0x04),
                    [2] = FLEXSPI_LUT_SEQ(READ_DDR, FLEXSPI_8PAD, 0x04, STOP_EXE, FLEXSPI_1PAD, 0x00),

                    /* Read status SPI */
                    [4 * 1 + 0] = FLEXSPI_LUT_SEQ(CMD_SDR, FLEXSPI_1PAD, 0x05, READ_SDR, FLEXSPI_1PAD, 0x04),

                    /* Read Status OPI */
                    [4 * 2 + 0] = FLEXSPI_LUT_SEQ(CMD_DDR, FLEXSPI_8PAD, 0x05, CMD_DDR, FLEXSPI_8PAD, 0xFA),
                    [4 * 2 + 1] = FLEXSPI_LUT_SEQ(RADDR_DDR, FLEXSPI_8PAD, 0x20, DUMMY_DDR, FLEXSPI_8PAD, 0x04),
                    [4 * 2 + 2] = FLEXSPI_LUT_SEQ(READ_DDR, FLEXSPI_8PAD, 0x04, STOP_EXE, FLEXSPI_1PAD, 0x00),

                    /* Write Enable */
                    [4 * 3 + 0] = FLEXSPI_LUT_SEQ(CMD_SDR, FLEXSPI_1PAD, 0x06, STOP_EXE, FLEXSPI_1PAD, 0x00),

                    /* Write Enable - OPI */
                    [4 * 4 + 0] = FLEXSPI_LUT_SEQ(CMD_DDR, FLEXSPI_8PAD, 0x06, CMD_DDR, FLEXSPI_8PAD, 0xF9),

                    /* Erase Sector */
                    [4 * 5 + 0] = FLEXSPI_LUT_SEQ(CMD_DDR, FLEXSPI_8PAD, 0x21, CMD_DDR, FLEXSPI_8PAD, 0xDE),
                    [4 * 5 + 1] = FLEXSPI_LUT_SEQ(RADDR_DDR, FLEXSPI_8PAD, 0x20, STOP_EXE, FLEXSPI_1PAD, 0x00),

                    /* Enable OPI DDR mode */
                    [4 * 6 + 0] = FLEXSPI_LUT_SEQ(CMD_SDR, FLEXSPI_1PAD, 0x72, CMD_SDR, FLEXSPI_1PAD, 0x00),
                    [4 * 6 + 1] = FLEXSPI_LUT_SEQ(CMD_SDR, FLEXSPI_1PAD, 0x00, CMD_SDR, FLEXSPI_1PAD, 0x00),
                    [4 * 6 + 2] = FLEXSPI_LUT_SEQ(CMD_SDR, FLEXSPI_1PAD, 0x00, WRITE_SDR, FLEXSPI_1PAD, 0x01),

                    /* Erase Block */
                    [4 * 8 + 0] = FLEXSPI_LUT_SEQ(CMD_DDR, FLEXSPI_8PAD, 0xDC, CMD_DDR, FLEXSPI_8PAD, 0x23),
                    [4 * 8 + 1] = FLEXSPI_LUT_SEQ(RADDR_DDR, FLEXSPI_8PAD, 0x20, STOP_EXE, FLEXSPI_1PAD, 0x00),

                    /* Page program */
                    [4 * 9 + 0] = FLEXSPI_LUT_SEQ(CMD_DDR, FLEXSPI_8PAD, 0x12, CMD_DDR, FLEXSPI_8PAD, 0xED),
                    [4 * 9 + 1] = FLEXSPI_LUT_SEQ(RADDR_DDR, FLEXSPI_8PAD, 0x20, WRITE_DDR, FLEXSPI_8PAD, 0x04),

                    /* Erase Chip */
                    [4 * 11 + 0] = FLEXSPI_LUT_SEQ(CMD_DDR, FLEXSPI_8PAD, 0x60, CMD_DDR, FLEXSPI_8PAD, 0x9F),
                },
        },
    .pageSize           = 256u,
    .sectorSize         = 4u * 1024u,
    .ipcmdSerialClkFreq = 1u,
    .serialNorType      = 2u,
    .blockSize          = 64u * 1024u,
    .flashStateCtx      = 0x07008200u,
};
```

## px4/fmu-v6xrt

[PX4-Autopilot/boards/px4/fmu-v6xrt/src/imxrt_flexspi_nor_flash.c](https://github.com/PX4/PX4-Autopilot/blob/main/boards/px4/fmu-v6xrt/src/imxrt_flexspi_nor_flash.c)，定义了 `struct flexspi_nor_config_s g_flash_fast_config` 结构体如下：

```c
const struct flexspi_nor_config_s g_flash_fast_config = {
 .memConfig =
 {
  .tag                 = FLEXSPI_CFG_BLK_TAG,
  .version             = FLEXSPI_CFG_BLK_VERSION,
  .readSampleClkSrc    = kFlexSPIReadSampleClk_ExternalInputFromDqsPad,
  .csHoldTime          = 1,
  .csSetupTime         = 1,
  .deviceModeCfgEnable = 1,
  .deviceModeType      = kDeviceConfigCmdType_Spi2Xpi,
  .waitTimeCfgCommands = 1,
  .deviceModeSeq =
  {
   .seqNum   = 1,
   .seqId    = 6, /* See Lookup table for more details */
   .reserved = 0,
  },
  .deviceModeArg = 2, /* Enable OPI DDR mode */
  .controllerMiscOption =
  (1u << kFlexSpiMiscOffset_SafeConfigFreqEnable) | (1u << kFlexSpiMiscOffset_DdrModeEnable),
  .deviceType    = kFlexSpiDeviceType_SerialNOR,
  .sflashPadType = kSerialFlash_8Pads,
  .serialClkFreq = kFlexSpiSerialClk_200MHz,
  .sflashA1Size  = 64ul * 1024u * 1024u,
  .dataValidTime =
  {
   [0] = {.time_100ps = 0},
  },
  .busyOffset      = 0u,
  .busyBitPolarity = 0u,
  .lookupTable =
  {
   /* Read */// EEH+11H+32bit addr+20dummy cycles+ 4Bytes read data
   /* Macronix manual says 20 dummy cycles @ 200Mhz, FlexSPI peripheral Operand value needs to be 2N in DDR mode hence 0x28 */
   [0 + 0] = FLEXSPI_LUT_SEQ(CMD_DDR, FLEXSPI_8PAD, 0xEE, CMD_DDR, FLEXSPI_8PAD, 0x11),    //0x871187ee,
   [0 + 1] = FLEXSPI_LUT_SEQ(RADDR_DDR, FLEXSPI_8PAD, 0x20, DUMMY_DDR, FLEXSPI_8PAD, 0x28),//0xb3288b20,
   [0 + 2] = FLEXSPI_LUT_SEQ(READ_DDR, FLEXSPI_8PAD, 0x04, STOP_EXE, FLEXSPI_1PAD, 0x00), //0xa704,

   /* Read status */
   [4 * 2 + 0] = FLEXSPI_LUT_SEQ(CMD_DDR, FLEXSPI_8PAD, 0x05, CMD_DDR, FLEXSPI_8PAD, 0xfa),
   [4 * 2 + 1] = FLEXSPI_LUT_SEQ(RADDR_DDR, FLEXSPI_8PAD, 0x20, DUMMY_DDR, FLEXSPI_8PAD, 0x04),
   [4 * 2 + 2] = FLEXSPI_LUT_SEQ(READ_DDR, FLEXSPI_8PAD, 0x04, STOP_EXE, FLEXSPI_1PAD, 0x00),

   /* Write enable SPI *///06h
   [4 * 3 + 0] = FLEXSPI_LUT_SEQ(CMD_SDR, FLEXSPI_1PAD, 0x06, STOP_EXE, FLEXSPI_1PAD, 0x00),//0x00000406,

   /* Write enable OPI SPI *///06h
   [4 * 4 + 0] = FLEXSPI_LUT_SEQ(CMD_DDR, FLEXSPI_8PAD, 0x06, CMD_DDR, FLEXSPI_8PAD, 0xF9),

   /* Erase sector */
   [4 * 5 + 0] = FLEXSPI_LUT_SEQ(CMD_DDR, FLEXSPI_8PAD, 0x21, CMD_DDR, FLEXSPI_8PAD, 0xDE),
   [4 * 5 + 1] = FLEXSPI_LUT_SEQ(RADDR_DDR, FLEXSPI_8PAD, 0x20, STOP_EXE, FLEXSPI_1PAD, 0x00),

   /*Write Configuration Register 2 =01, Enable OPI DDR mode*/ //72H +32bit address + CR20x00000000 = 0x01
   [4 * 6 + 0] = FLEXSPI_LUT_SEQ(CMD_SDR, FLEXSPI_1PAD, 0x72, CMD_SDR, FLEXSPI_1PAD, 0x00),//0x04000472,
   [4 * 6 + 1] = FLEXSPI_LUT_SEQ(CMD_SDR, FLEXSPI_1PAD, 0x00, CMD_SDR, FLEXSPI_1PAD, 0x00),//0x04000400,
   [4 * 6 + 2] = FLEXSPI_LUT_SEQ(CMD_SDR, FLEXSPI_1PAD, 0x00, WRITE_SDR, FLEXSPI_1PAD, 0x01),//0x20010400,

   /*Page program*/
   [4 * 9 + 0] = FLEXSPI_LUT_SEQ(CMD_DDR, FLEXSPI_8PAD, 0x12, CMD_DDR, FLEXSPI_8PAD, 0xED),//0x87ed8712,
   [4 * 9 + 1] = FLEXSPI_LUT_SEQ(RADDR_DDR, FLEXSPI_8PAD, 0x20, WRITE_DDR, FLEXSPI_8PAD, 0x04),//0xa3048b20,
  },
 },
 .pageSize           = 256u,
 .sectorSize         = 4u * 1024u,
 .blockSize          = 64u * 1024u,
 .isUniformBlockSize = false,
 .ipcmdSerialClkFreq = 1,
 .serialNorType = 2,
 .reserve2[0] = 0x7008200,
};
```

## 两个配置的对比

对比 [mcux-sdk/evkmimxrt595/flash_config/flash_config.c](https://github.com/NXPmicro/mcux-sdk/blob/main/boards/evkmimxrt595/flash_config/flash_config.c) 和 [PX4-Autopilot/boards/px4/fmu-v6xrt/src/imxrt_flexspi_nor_flash.c](https://github.com/PX4/PX4-Autopilot/blob/main/boards/px4/fmu-v6xrt/src/imxrt_flexspi_nor_flash.c) 形成本项目所使用的 `flexspi_nor_config_t`。

### memConfig 结构体内的字段

| 字段名 | evkmimxrt595 | PX4 V6XRT | 分析与建议 |
| :--- | :--- | :--- | :--- |
| `tag` | `FLEXSPI_CFG_BLK_TAG` | `FLEXSPI_CFG_BLK_TAG` | 一致。均为标准的 FlexSPI 配置块标签。 |
| `version` | `FLEXSPI_CFG_BLK_VERSION` | `FLEXSPI_CFG_BLK_VERSION` | 一致。配置块版本号相同。 |
| `readSampleClkSrc` | `ExternalInputFromDqsPad` | `ExternalInputFromDqsPad` | 一致。都使用外部 DQS 引脚作为数据采样时钟源，这是实现高频稳定读取的关键。 |
| `csHoldTime` | `3` | `1` | 不同。NXP SDK 的片选保持时间更长，时序更宽松保守。PX4 的设置更紧凑，以追求更高性能。 |
| `csSetupTime` | `3` | `1` | 不同。与 `csHoldTime` 类似，NXP SDK 的片选建立时间更长，更保守。 |
| `deviceModeCfgEnable` | `1` | `1` | 一致。都启用了设备模式配置，意味着启动时会发送命令来初始化 Flash。 |
| `deviceModeType` | `kDeviceConfigCmdType_Spi2Xpi` | `kDeviceConfigCmdType_Spi2Xpi` | 一致。都配置为从标准 SPI 模式切换到 XPI (即 OPI) 模式。 |
| `waitTimeCfgCommands` | `1` | `1` | 一致。发送配置命令后的等待时间相同。 |
| `deviceModeSeq` | `seqNum=1, seqId=6` | `seqNum=1, seqId=6` | 一致。都指向 LUT 表中索引为 6 的序列来执行模式切换。 |
| `deviceModeArg` | `2` | `2` | 一致。参数为 2，通常表示启用 OPI DDR 模式。 |
| `controllerMiscOption` | `SafeConfigFreqEnable` \| `DdrModeEnable` | `SafeConfigFreqEnable` \| `DdrModeEnable` | 一致。都启用了安全配置频率和 DDR 模式。 |
| `deviceType` | `kFlexSpiDeviceType_SerialNOR` | `kFlexSpiDeviceType_SerialNOR` | 一致。设备类型均为串行 NOR Flash。 |
| `sflashPadType` | `kSerialFlash_8Pads` | `kSerialFlash_8Pads` | 一致。都正确配置为 8 线 (Octal) 模式，这是使用 MX25UM51345G 的关键。 |
| `serialClkFreq` | `kFlexSpiSerialClk_60MHz` | `kFlexSpiSerialClk_200MHz` | 核心区别。NXP SDK 采用非常保守的 60MHz 以确保兼容性。PX4 则配置了高达 200MHz 的频率以追求极致性能。 |
| `sflashA1Size` | `64ul * 1024u * 1024u` | `64ul * 1024u * 1024u` | 一致。都正确识别 Flash 大小为 64MB (512Mb)。 |
| `dataValidTime` | (未显式设置) | `[0] = {.time_100ps = 0}` | 不同。PX4 显式设置了数据有效时间，NXP SDK 使用默认值。在此场景下影响不大。 |
| `busyOffset` | `0u` | `0u` | 一致。状态寄存器中忙标志位的偏移量相同。 |
| `busyBitPolarity` | `0u` | `0u` | 一致。忙标志位的极性相同。 |
| `lookupTable` | 见下文分析 | 见下文分析 | 结构相同，参数不同。这是配置的核心，下面将详细对比。 |

### LUT (查找表) 详细对比

LUT 定义了 FlexSPI 控制器与 Flash 通信的具体指令序列。两者结构高度相似，但关键参数不同。

- NXP：存在[4*8]块擦除序列；
- NXP：存在[4*11]整片擦除 0x60 指令；

| 操作 | evkmimxrt595 | PX4 V6XRT | 分析与建议 |
| :--- | :--- | :--- | :--- |
| 读取 (Read) | `CMD_DDR(0xEE)`, `RADDR_DDR`, `DUMMY_DDR(0x04)`, `READ_DDR` | `CMD_DDR(0xEE)`, `RADDR_DDR`, `DUMMY_DDR(0x28)`, `READ_DDR` | Dummy Cycles 差异巨大。NXP SDK 仅用 4 个周期，适用于 60MHz。PX4 使用 0x28 (即 40 个周期)，这是为了在 200MHz 高频下满足 Flash 的时序要求。 |
| 写使能 (Write En) | `CMD_SDR(0x06)` (SPI) 和 `CMD_DDR(0x06)` (OPI) | `CMD_SDR(0x06)` (SPI) 和 `CMD_DDR(0x06)` (OPI) | 一致。都配置了 SPI 和 OPI 两种模式下的写使能命令。 |
| 扇区擦除 (Erase Sector) | `CMD_DDR(0x21)`, `RADDR_DDR` | `CMD_DDR(0x21)`, `RADDR_DDR` | 一致。使用相同的 OPI DDR 模式擦除指令。 |
| 页编程 (Page Prog) | `CMD_DDR(0x12)`, `RADDR_DDR`, `WRITE_DDR` | `CMD_DDR(0x12)`, `RADDR_DDR`, `WRITE_DDR` | 一致。使用相同的 OPI DDR 模式写入指令。 |
| 模式切换 (Enter OPI) | `CMD_SDR(0x72)`, ..., `WRITE_SDR(0x01)` | `CMD_SDR(0x72)`, ..., `WRITE_SDR(0x01)` | 一致。都通过向配置寄存器写入 `0x01` 来启用 OPI 模式。 |

### flexspi_nor_config_t 结构体顶层字段

| 字段名 | evkmimxrt595 | PX4 V6XRT | 分析与建议 |
| :--- | :--- | :--- | :--- |
| `pageSize` | `256u` | `256u` | 一致。页面大小为 256 字节。 |
| `sectorSize` | `4u * 1024u` | `4u * 1024u` | 一致。扇区大小为 4KB。 |
| `blockSize` | `64u * 1024u` | `64u * 1024u` | 一致。块大小为 64KB。 |
| `ipcmdSerialClkFreq` | `1u` | `1u` | 一致。IP 命令的串行时钟频率设置相同。 |
| `isUniformBlockSize` | (未显式设置) | `false` | 不同。PX4 显式声明块大小不均匀。对于您的 Flash，这通常不是关键问题。 |
| `serialNorType` | `2u` | `2u` | 一致。Serial NOR 类型相同。 |
| `flashStateCtx` | `0x07008200u` | (无此字段) | 不同。这是 NXP SDK 特有的字段，PX4 使用了不同的结构体定义。 |
| `reserve2[0]` | (无此字段) | `0x7008200` | 不同。这是 PX4 结构体中的保留字段，其值与 NXP SDK 的 `flashStateCtx` 惊人地相似，可能是用于兼容或特定用途。 |

## 最终推荐配置

```c
const flexspi_nor_config_t g_MX25UM51345G_config = {
    .memConfig =
        {
            .tag                 = FLEXSPI_CFG_BLK_TAG,
            .version             = FLEXSPI_CFG_BLK_VERSION,
            .readSampleClkSrc    = kFlexSPIReadSampleClk_ExternalInputFromDqsPad,
            .csHoldTime          = 1,
            .csSetupTime         = 1,
            .deviceModeCfgEnable = 1,
            .deviceModeType      = kDeviceConfigCmdType_Spi2Xpi,
            .waitTimeCfgCommands = 1,
            .deviceModeSeq =
                {
                    .seqNum   = 1,
                    .seqId    = 6, /* LUT6写入CR2永久开启OPI DDR */
                    .reserved = 0,
                },
            .deviceModeArg = 2,
            .controllerMiscOption =
                (1u << kFlexSpiMiscOffset_SafeConfigFreqEnable) | (1u << kFlexSpiMiscOffset_DdrModeEnable),
            .deviceType    = kFlexSpiDeviceType_SerialNOR,
            .sflashPadType = kSerialFlash_8Pads,
            .serialClkFreq = kFlexSpiSerialClk_200MHz,
            .sflashA1Size  = 64ul * 1024u * 1024u,
            .dataValidTime =
                {
                    [0] = {.time_100ps = 0},
                },
            .busyOffset      = 0u,
            .busyBitPolarity = 0u,
            .lookupTable =
                {
                    /* XIP OPI-DDR读，20个dummy适配200MHz */
                    /* Macronix manual says 20 dummy cycles @ 200Mhz, FlexSPI peripheral Operand value needs to be 2N in DDR mode hence 0x28 */
                    [0 + 0] = FLEXSPI_LUT_SEQ(CMD_DDR, FLEXSPI_8PAD, 0xEE, CMD_DDR, FLEXSPI_8PAD, 0x11),     //0x871187ee,
                    [0 + 1] = FLEXSPI_LUT_SEQ(RADDR_DDR, FLEXSPI_8PAD, 0x20, DUMMY_DDR, FLEXSPI_8PAD, 0x28), //0xb3288b20,
                    [0 + 2] = FLEXSPI_LUT_SEQ(READ_DDR, FLEXSPI_8PAD, 0x04, STOP_EXE, FLEXSPI_1PAD, 0x00),   //0xa704,

                    /* 补齐 [4*1] SDR单线读状态（低速 fallback） */
                    [4 * 1 + 0] = FLEXSPI_LUT_SEQ(CMD_SDR, FLEXSPI_1PAD, 0x05, READ_SDR, FLEXSPI_1PAD, 0x04),

                    /* OPI-DDR读状态寄存器 */
                    [4 * 2 + 0] = FLEXSPI_LUT_SEQ(CMD_DDR, FLEXSPI_8PAD, 0x05, CMD_DDR, FLEXSPI_8PAD, 0xfa),
                    [4 * 2 + 1] = FLEXSPI_LUT_SEQ(RADDR_DDR, FLEXSPI_8PAD, 0x20, DUMMY_DDR, FLEXSPI_8PAD, 0x04),
                    [4 * 2 + 2] = FLEXSPI_LUT_SEQ(READ_DDR, FLEXSPI_8PAD, 0x04, STOP_EXE, FLEXSPI_1PAD, 0x00),

                    /* SDR 写使能 WREN */
                    [4 * 3 + 0] = FLEXSPI_LUT_SEQ(CMD_SDR, FLEXSPI_1PAD, 0x06, STOP_EXE, FLEXSPI_1PAD, 0x00), //0x00000406,

                    /* OPI-DDR 写使能 */
                    [4 * 4 + 0] = FLEXSPI_LUT_SEQ(CMD_DDR, FLEXSPI_8PAD, 0x06, CMD_DDR, FLEXSPI_8PAD, 0xF9),

                    /* 4KB扇区擦除 */
                    [4 * 5 + 0] = FLEXSPI_LUT_SEQ(CMD_DDR, FLEXSPI_8PAD, 0x21, CMD_DDR, FLEXSPI_8PAD, 0xDE),
                    [4 * 5 + 1] = FLEXSPI_LUT_SEQ(RADDR_DDR, FLEXSPI_8PAD, 0x20, STOP_EXE, FLEXSPI_1PAD, 0x00),

                    /* LUT6：写CR2寄存器永久开启OPI DDR */
                    [4 * 6 + 0] = FLEXSPI_LUT_SEQ(CMD_SDR, FLEXSPI_1PAD, 0x72, CMD_SDR, FLEXSPI_1PAD, 0x00),   //0x04000472,
                    [4 * 6 + 1] = FLEXSPI_LUT_SEQ(CMD_SDR, FLEXSPI_1PAD, 0x00, CMD_SDR, FLEXSPI_1PAD, 0x00),   //0x04000400,
                    [4 * 6 + 2] = FLEXSPI_LUT_SEQ(CMD_SDR, FLEXSPI_1PAD, 0x00, WRITE_SDR, FLEXSPI_1PAD, 0x01), //0x20010400,

                    /* 补齐 [4*8] 64KB块擦除 */
                    [4 * 8 + 0] = FLEXSPI_LUT_SEQ(CMD_DDR, FLEXSPI_8PAD, 0xDC, CMD_DDR, FLEXSPI_8PAD, 0x23),
                    [4 * 8 + 1] = FLEXSPI_LUT_SEQ(RADDR_DDR, FLEXSPI_8PAD, 0x20, STOP_EXE, FLEXSPI_1PAD, 0x00),

                    /* 256字节页编程 */
                    [4 * 9 + 0] = FLEXSPI_LUT_SEQ(CMD_DDR, FLEXSPI_8PAD, 0x12, CMD_DDR, FLEXSPI_8PAD, 0xED),     //0x87ed8712,
                    [4 * 9 + 1] = FLEXSPI_LUT_SEQ(RADDR_DDR, FLEXSPI_8PAD, 0x20, WRITE_DDR, FLEXSPI_8PAD, 0x04), //0xa3048b20,

                    /* 补齐 [4*11] 全片擦除 */
                    [4 * 11 + 0] = FLEXSPI_LUT_SEQ(CMD_DDR, FLEXSPI_8PAD, 0x60, CMD_DDR, FLEXSPI_8PAD, 0x9F),
                },
        },
    .pageSize           = 256u,
    .sectorSize         = 4u * 1024u,
    .blockSize          = 64u * 1024u,
    .isUniformBlockSize = false,
    .ipcmdSerialClkFreq = 1,
    .serialNorType      = 2,
    .reserve2[0]        = 0x7008200,
};
```
