/*
 * Copyright 2018-2020 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "flexspi_nor_config.h"

/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#   define FSL_COMPONENT_ID "platform.drivers.xip_board"
#endif

/*******************************************************************************
 * Code
 ******************************************************************************/
#if defined(XIP_BOOT_HEADER_ENABLE) && (XIP_BOOT_HEADER_ENABLE == 1)
#   if defined(__CC_ARM) || defined(__ARMCC_VERSION) || defined(__GNUC__)
__attribute__((section(".boot_hdr.conf"), used))
#   elif defined(__ICCARM__)
#      pragma location = ".boot_hdr.conf"
#   endif

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
#endif /* XIP_BOOT_HEADER_ENABLE */
