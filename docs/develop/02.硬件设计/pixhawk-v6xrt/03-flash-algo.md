# 下载算法

I.MXRT1176 没有内置非易失性存储器，需要外挂一块存储器用于加载启动，最常用的是通过 FlexSPI 外挂串行 NOR Flash，挂了 NOR Flash 我们既可以离线启动，也可以在线调试，而在线调试就必然离不开下载算法（Programming Algorithm）。

MDK下Flash下载算法是开源的，有较详细的文档，文档在[arm-software的github主页](https://arm-software.github.io/CMSIS_5/Pack/html/flashAlgorithm.html)，根据这些文档，我们基本可以了解下载算法设计细节。另外 KEIL 安装目录下 `C:\Keil_v5\ARM\Flash\_Template`，提供了 下载算法的参考模板。J-Link 下载算法和 KEIL 下载算法采用相同的接口，__一般可以通用__。

下载算法需要 __根据 MCU 和 Flash 的连接以及型号__ 而定，对于了解其原理的人来说，制作下载算法并不难，但是对于不了解的人来说却又不容易。从i.MXRT原厂技术支持角度，重复的工作又不太想一次次去做，基于此，痞子衡发起了一个开源项目，命名为 [RT-UFL](https://github.com/JayHeng/RT-UFL)，就是设计一个超级下载算法。

本项目基于 痞子衡 [RT-UFL](https://github.com/JayHeng/RT-UFL)，制作 适用于 Pixhawk V6X-RT 的下载算法。

## 硬件地址

根据 [Pixhawk V6X-RT](https://github.com/pixhawk/Pixhawk-Standards/blob/master/DS-020%20Pixhawk%20Autopilot%20v6X-RT%20Standard.pdf) 开发标准，I.MXRT1176 处理器通过 `FlexSPI1` 连接 MX25UM51345GXDI00。

查询 [I.MX RT1170 Processor Reference Manual](./IMXRT1170RM.pdf) 中 `3.2 System memory map (CM7)` 可知：

- RT1176 FlexSPI1 Start Address：`0x30000000`

查询 [MX25UM51345G Datasheet](./MX25UM51345G,%201.8V,%20512Mb,%20v1.3.pdf) 中 `5. MEMORY ORGANIZATION` 可知，MX25UM51345G 总共 64MB，分成 1024 个block（64MB/1024=64KB），每个 block 由 16 个 sector（64KB/16=4KB）组成，另外有 256 bytes page buffer，因此：

- MX25UM51345G Memory size: 512Mb (64MB, 0x04000000)
- MX25UM51345G Block size: 64KB
- MX25UM51345G Sector size: 4KB (uniform, 0x1000)
- MX25UM51345G Page size: 256bytes (0x100)

## 修改算法

下载 <https://github.com/JayHeng/RT-UFL> 到本地

```
git clone https://github.com/JayHeng/RT-UFL
```

打开`build/mdk/FlashPrg.c`文件，修改 31 行左右`flexspi_nor_config_t flashConfig`结构体，增加 `sectorSize`字段并初始化为 `0x1000` (4KB，uniform)

=== "修改后"

    ```c
    flexspi_nor_config_t flashConfig = {
        .pageSize = FLASH_DRV_PAGE_SIZE,
        .sectorSize = FLASH_DRV_SECTOR_SIZE,
    };
    ```

=== "修改前"

    ```c
    flexspi_nor_config_t flashConfig = {.pageSize = FLASH_DRV_PAGE_SIZE};
    ```

打开`build/mdk/FlashDev.c`文件，修改 128 行左右 `struct FlashDevice const FlashDevice`结构体

主要修改内容为：

- RT1176 FlexSPI1 Start Address：`0x30000000`
- MX25UM51345G Size：`0x04000000`（64MB，512Mb）
- MX25UM51345G Page size：`0x100` (256Bytes)
- MX25UM51345G Sector size: `0x1000` (4KB，uniform)

=== "修改后"

    ```c
    struct FlashDevice const FlashDevice = {
    FLASH_DRV_VERS,             // Driver Version, do not modify!
    "MIMXRT1176_MX25UM51345G",  // Device Name
    EXTSPI,                     // Device Type
    0x30000000,                 // Device Start Address (RT1176 FlexSPI AMBA)
    0x04000000,                 // Device Size in Bytes (64 MB)
    FLASH_DRV_PAGE_SIZE,        // Programming Page Size (256 B)
    0,                          // Reserved, must be 0
    0xFF,                       // Initial Content of Erased Memory
    3000,                       // Program Page Timeout 3000 mSec
    15000,                      // Erase Sector Timeout 15000 mSec

    // Specify Size and Address of Sectors
    FLASH_DRV_SECTOR_SIZE, 0x00000000,    // Sector Size 4 kB
    SECTOR_END
    };
    ```

=== "修改前"

    ```c
    struct FlashDevice const FlashDevice = {
    FLASH_DRV_VERS,             // Driver Version, do not modify!
    "MIMXRT_FLEXSPI",           // Device Name
    EXTSPI,                     // Device Type
    0x00000000,                 // Device Start Address
    0x40000000,                 // Device Size in Bytes (1GB)
    FLASH_DRV_PAGE_SIZE,        // Programming Page Size
    0,                          // Reserved, must be 0
    0xFF,                       // Initial Content of Erased Memory
    3000,                       // Program Page Timeout 100 mSec
    15000,                      // Erase Sector Timeout 15000 mSec

    // Specify Size and Address of Sectors
    FLASH_DRV_SECTOR_SIZE, 0x00000000,    // Sector Size  4kB (256 Sectors)
    SECTOR_END
    };
    ```

然后用 MDK 打开`build/mdk/MIMXRT_FLEXSPI_UV5.uvprojx`工程，重新编译生成 [MIMXRT1176_MX25UM51345G.FLM](./MIMXRT1176_MX25UM51345G.FLM) 文件。

## 如何使用

1. 将生成的 `MIMXRT1176_MX25UM51345G.FLM` 文件复制到 KEIL 安装目录下的 `ARM\Flash\` 目录中
2. 在 KEIL 工程中打开 `Options for Target` → `Debug` → `Settings` → `Flash Download`，点击 `Add` 按钮
3. 在列表中找到 `MIMXRT1176_MX25UM51345G` 并添加
4. 确认 Flash 起始地址为 `0x30000000`，大小 `0x04000000`
5. 点击 `Download` 即可将程序烧写到外部 NOR Flash 中

## 参考资料

1. [RT-UFL - 一个适用全平台 i.MXRT 的超级下载算法设计](https://github.com/JayHeng/RT-UFL)
2. [CMSIS Flash Algorithm 文档](https://arm-software.github.io/CMSIS_5/Pack/html/flashAlgorithm.html)
3. [I.MX RT1170 Processor Reference Manual](./IMXRT1170RM.pdf), `Chapter 3 System Memory Map`
4. [MX25UM51345G Datasheet](./MX25UM51345G,%201.8V,%20512Mb,%20v1.3.pdf)
