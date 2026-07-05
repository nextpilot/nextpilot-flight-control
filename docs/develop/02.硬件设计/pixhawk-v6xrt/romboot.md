# 系统启动

## Boot ROM

The boot process begins at any Reset where the hardware reset logic forces the Arm core (boot core is determined by BT_CORE_SEL fuse setting) to begin execution starting from the on-chip boot ROM.

If the BT_CORE_SEL fuse is blown, ROM will be executing from the M4 core instead of the M7 core. Booting from the M4 core can be slower due to the slower ROM execution time with the M4, especially if using HAB.

The boot ROM code uses the state of the internal register BOOT_MODE[1:0] as well as the state of various eFuses and/or GPIO settings to determine the boot flow behavior of the device.

The main features of the ROM include:

- Support for booting from various boot devices
- Serial downloader support (USB-HID and UART)
- Device Configuration Data (DCD)
- Digital signature based High-Assurance Boot (HAB)
- eXternal Memory Configuration Data (XMCD)
- Encrypted execute-in-place (XIP) on Serial NOR via FlexSPI interface powered by:
  - Inline Encryption Engine (IEE)
  - On-the-Fly AES Decryption (OTFAD)

The boot ROM supports boot device as below:

- Serial NOR Flash via FlexSPI
- Serial NAND Flash via FlexSPI
- SLC RAWNAND Flash via SEMC
- SD/MMC via uSDHC
- SPI NOR/EEPROM via LPSPI

### Boot MODE

|BOOT_MODE[1:0]| Boot Type|
|---|---|
| 00 | Boot From Fuses，根据eFuse设置进行系统启动，忽略GPIO配置，BT_FUSE_SEL=0表示没有烧写固件，会自动进入串行下载 |
| 01 | Serial Downloader，通过UART1或者USB下载程序 |
| 10 | Internal Boot，根据BT_FUSE_SEL=1按照eFuse系统启动，BT_FUSE_SEL=0按照GPIO配置系统启动 |
| 11 | Reserved |
