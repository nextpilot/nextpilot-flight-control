#ifndef RT_CONFIG_H__
#define RT_CONFIG_H__

/* Automatically generated file; DO NOT EDIT. */
/* RT-Thread Configuration */

/* RT-Thread Kernel */

#define RT_NAME_MAX   16
#define RT_ALIGN_SIZE 4
#define RT_THREAD_PRIORITY_32
#define RT_THREAD_PRIORITY_MAX 32
#define RT_TICK_PER_SECOND     1000
#define RT_USING_OVERFLOW_CHECK
#define RT_USING_HOOK
#define RT_HOOK_USING_FUNC_PTR
#define RT_USING_IDLE_HOOK
#define RT_IDLE_HOOK_LIST_SIZE 4
#define IDLE_THREAD_STACK_SIZE 256
#define RT_USING_TIMER_SOFT
#define RT_TIMER_THREAD_PRIO       4
#define RT_TIMER_THREAD_STACK_SIZE 512

/* kservice optimization */

/* Inter-Thread communication */

#define RT_USING_SEMAPHORE
#define RT_USING_MUTEX
#define RT_USING_EVENT
#define RT_USING_MAILBOX
#define RT_USING_MESSAGEQUEUE
#define RT_USING_SIGNALS

/* Memory Management */

#define RT_USING_MEMPOOL
#define RT_USING_SMALL_MEM
#define RT_USING_SMALL_MEM_AS_HEAP
#define RT_USING_HEAP

/* Kernel Device Object */

#define RT_USING_DEVICE
#define RT_USING_DEVICE_OPS
#define RT_USING_CONSOLE
#define RT_CONSOLEBUF_SIZE     256
#define RT_CONSOLE_DEVICE_NAME "uart7"
#define RT_VER_NUM             0x40101
#define ARCH_ARM
#define RT_USING_CPU_FFS
#define ARCH_ARM_CORTEX_M
#define ARCH_ARM_CORTEX_M7

/* RT-Thread Components */

#define RT_USING_COMPONENTS_INIT
#define RT_USING_USER_MAIN
#define RT_MAIN_THREAD_STACK_SIZE 2048
#define RT_MAIN_THREAD_PRIORITY   10
#define RT_USING_LEGACY
#define RT_USING_MSH
#define RT_USING_FINSH
#define FINSH_USING_MSH
#define FINSH_THREAD_NAME       "tshell"
#define FINSH_THREAD_PRIORITY   30
#define FINSH_THREAD_STACK_SIZE 4096
#define FINSH_USING_HISTORY
#define FINSH_HISTORY_LINES 5
#define FINSH_USING_SYMTAB
#define FINSH_CMD_SIZE 80
#define MSH_USING_BUILT_IN_COMMANDS
#define FINSH_USING_DESCRIPTION
#define FINSH_ARG_MAX 8
#define RT_USING_DFS
#define DFS_USING_POSIX
#define DFS_USING_WORKDIR
#define DFS_FILESYSTEMS_MAX      3
#define DFS_FILESYSTEM_TYPES_MAX 3
#define DFS_FD_MAX               10
#define RT_USING_DFS_ELMFAT

/* elm-chan's FatFs, Generic FAT Filesystem Module */

#define RT_DFS_ELM_CODE_PAGE 437
#define RT_DFS_ELM_WORD_ACCESS
#define RT_DFS_ELM_USE_LFN_3
#define RT_DFS_ELM_USE_LFN 3
#define RT_DFS_ELM_LFN_UNICODE_0
#define RT_DFS_ELM_LFN_UNICODE     0
#define RT_DFS_ELM_MAX_LFN         255
#define RT_DFS_ELM_DRIVES          2
#define RT_DFS_ELM_MAX_SECTOR_SIZE 512
#define RT_DFS_ELM_USE_ERASE
#define RT_DFS_ELM_REENTRANT
#define RT_DFS_ELM_MUTEX_TIMEOUT 3000
#define RT_USING_DFS_DEVFS
#define RT_USING_DFS_ROMFS

/* Device Drivers */

#define RT_USING_DEVICE_IPC
#define RT_USING_SERIAL
#define RT_USING_SERIAL_V1
#define RT_SERIAL_USING_DMA
#define RT_SERIAL_RB_BUFSZ 256
#define RT_USING_CAN
#define RT_USING_HWTIMER
#define RT_USING_I2C
#define RT_USING_I2C_BITOPS
#define RT_USING_PIN
#define RT_USING_ADC
#define RT_USING_PWM
#define RT_USING_RTC
#define RT_USING_SDIO
#define RT_SDIO_STACK_SIZE       512
#define RT_SDIO_THREAD_PRIORITY  15
#define RT_MMCSD_STACK_SIZE      1024
#define RT_MMCSD_THREAD_PREORITY 22
#define RT_MMCSD_MAX_PARTITION   16
#define RT_USING_SPI
#define RT_USING_INPUT_CAPTURE
#define RT_INPUT_CAPTURE_RB_SIZE 100

/* Using USB */

#define RT_USING_USB
#define RT_USING_USB_DEVICE
#define RT_USBD_THREAD_STACK_SZ 4096
#define USB_VENDOR_ID           0x483
#define USB_PRODUCT_ID          0x5740
#define RT_USB_DEVICE_COMPOSITE
#define RT_USB_DEVICE_CDC
#define RT_USB_DEVICE_NONE
#define RT_VCOM_TASK_STK_SIZE 512
#define RT_CDC_RX_BUFSIZE     128
#define RT_VCOM_TX_USE_DMA
#define RT_VCOM_SERNO      "32021919830108"
#define RT_VCOM_SER_LEN    14
#define RT_VCOM_TX_TIMEOUT 1000

/* C/C++ and POSIX layer */

#define RT_LIBC_DEFAULT_TIMEZONE 8

/* POSIX (Portable Operating System Interface) layer */

#define RT_USING_POSIX_FS
#define RT_USING_POSIX_DEVIO
#define RT_USING_POSIX_STDIO
#define RT_USING_POSIX_DELAY
#define RT_USING_POSIX_CLOCK
#define RT_USING_PTHREADS
#define PTHREAD_NUM_MAX 8

/* Interprocess Communication (IPC) */

/* Socket is in the 'Network' category */

#define RT_USING_CPLUSPLUS

/* Network */

/* Utilities */

#define RT_USING_ULOG
#define ULOG_OUTPUT_LVL_D
#define ULOG_OUTPUT_LVL 7
#define ULOG_ASSERT_ENABLE
#define ULOG_LINE_BUF_SIZE 128
#define ULOG_USING_ASYNC_OUTPUT
#define ULOG_ASYNC_OUTPUT_BUF_SIZE 4096
#define ULOG_ASYNC_OUTPUT_BY_THREAD
#define ULOG_ASYNC_OUTPUT_THREAD_STACK    4096
#define ULOG_ASYNC_OUTPUT_THREAD_PRIORITY 30

/* log format */

#define ULOG_OUTPUT_FLOAT
#define ULOG_USING_COLOR
#define ULOG_OUTPUT_TIME
#define ULOG_TIME_USING_TIMESTAMP
#define ULOG_OUTPUT_LEVEL
#define ULOG_OUTPUT_TAG
#define ULOG_BACKEND_USING_CONSOLE
#define ULOG_BACKEND_USING_FILE
#define ULOG_USING_FILTER

/* RT-Thread Utestcases */

/* RT-Thread online packages */

/* IoT - internet of things */

/* Wi-Fi */

/* Marvell WiFi */

/* Wiced WiFi */

/* IoT Cloud */

#define PKG_USING_MAVLINK
#define PKG_USING_MAVLINK_LATEST_VERSION
#define PKG_USING_MAVLINK_DIALECT_standard

/* security packages */

/* language packages */

/* JSON: JavaScript Object Notation, a lightweight data-interchange format */

/* XML: Extensible Markup Language */

/* multimedia packages */

/* LVGL: powerful and easy-to-use embedded GUI library */

/* u8g2: a monochrome graphic library */

/* tools packages */

/* system packages */

/* enhanced kernel services */

#define PKG_USING_RT_MEMCPY_CM
#define PKG_USING_RT_MEMCPY_CM_LATEST_VERSION
#define PKG_USING_RT_VSNPRINTF_FULL
#define PKG_USING_RT_VSNPRINTF_FULL_LATEST_VERSION

/* acceleration: Assembly language or algorithmic acceleration packages */

/* CMSIS: ARM Cortex-M Microcontroller Software Interface Standard */

/* Micrium: Micrium software products porting for RT-Thread */

/* peripheral libraries and drivers */

/* sensors drivers */

/* touch drivers */

/* Kendryte SDK */

/* AI packages */

/* Signal Processing and Control Algorithm Packages */

/* miscellaneous packages */

/* project laboratory */

/* samples: kernel and components samples */

/* entertainment: terminal games and other interesting software packages */

#define PKG_USING_OPTPARSE
#define PKG_USING_OPTPARSE_LATEST_VERSION

/* Arduino libraries */

/* Projects and Demos */

/* Sensors */

/* Display */

/* Timing */

/* Data Processing */

/* Data Storage */

/* Communication */

/* Device Control */

/* Other */

/* Signal IO */

/* Uncategorized */

/* Nextpilot Hardware Config */

#define BSP_CETCS_FCS
#define SOC_STM32F765II

/* Chip Memory Setting */

#define FLASH_START_ADDRESS    0x08000000
#define FLASH_APP_VTOR_OFFSET  32
#define FLASH_APP_TOTAL_LENGTH 2016
#define SRAM_START_ADDRESS     0x20000000
#define SRAM_TOTAL_LENGTH      512

/* On-chip Peripheral Drivers */

#define BSP_USING_GPIO
#define BSP_USING_ADC
#define BSP_USING_ADC1
#define BSP_USING_PWM
#define BSP_USING_PWM1
#define BSP_USING_PWM1_CH1
#define BSP_USING_PWM1_CH2
#define BSP_USING_PWM1_CH3
#define BSP_USING_PWM1_CH4
#define BSP_USING_PWM2
#define BSP_USING_PWM3
#define BSP_USING_PWM4
#define BSP_USING_PWM4_CH2
#define BSP_USING_PWM4_CH3
#define BSP_USING_PWM5
#define BSP_USING_PWM8
#define BSP_USING_PWM9
#define BSP_USING_PWM9_CH1
#define BSP_USING_PWM12
#define BSP_USING_PWM12_CH1
#define BSP_USING_PWM12_CH2
#define BSP_USING_PWM14
#define BSP_USING_UART
#define BSP_USING_UART1
#define BSP_UART1_RX_USING_DMA
#define BSP_USING_UART2
#define BSP_UART2_RX_USING_DMA
#define BSP_USING_UART3
#define BSP_UART3_RX_USING_DMA
#define BSP_UART3_TX_USING_DMA
#define BSP_USING_UART4
#define BSP_UART4_RX_USING_DMA
#define BSP_UART4_TX_USING_DMA
#define BSP_USING_UART5
#define BSP_UART5_RX_USING_DMA
#define BSP_UART5_TX_USING_DMA
#define BSP_USING_UART6
#define BSP_UART6_RX_USING_DMA
#define BSP_UART6_TX_USING_DMA
#define BSP_USING_UART7
#define BSP_USING_UART8
#define BSP_UART8_RX_USING_DMA
#define BSP_USING_CAN
#define BSP_USING_CAN1
#define BSP_USING_CAN2
#define BSP_USING_I2C
#define BSP_USING_I2C1
#define BSP_I2C1_SCL_PIN 24
#define BSP_I2C1_SDA_PIN 25
#define BSP_USING_I2C2
#define BSP_I2C2_SCL_PIN 81
#define BSP_I2C2_SDA_PIN 80
#define BSP_USING_I2C3
#define BSP_I2C3_SCL_PIN 119
#define BSP_I2C3_SDA_PIN 120
#define BSP_USING_I2C4
#define BSP_I2C4_SCL_PIN 94
#define BSP_I2C4_SDA_PIN 95
#define BSP_USING_SPI
#define BSP_USING_SPI1
#define BSP_USING_SPI2
#define BSP_USING_SPI4
#define BSP_USING_SPI5
#define BSP_USING_SPI6
#define BSP_USING_USBD
#define BSP_USING_ONCHIP_RTC
#define BSP_RTC_USING_LSI
#define BSP_USING_SDIO
#define BSP_SDIO_RX_USING_DMA
#define BSP_SDIO_TX_USING_DMA
#define SOC_FAMILY_STM32
#define SOC_SERIES_STM32F7

/* Onboard Peripheral Drivers */

/* Board extended module Drivers */

/* Nextpilot Firmware Config */

#define APP_USING_AUTOPILOT

/* nextpilot peripheral config */

/* radio control */

#define DRV_USING_RC_INPUT
#define RC_USING_SBUS
#define RC_SERIAL_PORT "uart6"
#define RC_SERIAL_INVERT_INPUT

/* battery monitor */

#define DRV_USING_BATTERY_STATUS
#define BOARD_NUMBER_BRICKS          1
#define ADC_BATTERY_DEVICE_NAME      "adc1"
#define ADC_BATTERY1_VOLTAGE_CHANNEL 0
#define ADC_BATTERY1_CURRENT_CHANNEL 1
#define DRV_USING_SYSTEM_POWER
#define ADC_POWER_DEVICE_NAME   "adc1"
#define ADC_SCALED_V5_CHANNEL   10
#define ADC_SCALED_3V3_CHANNEL1 11
#define ADC_SCALED_3V3_CHANNEL2 0
#define ADC_SCALED_3V3_CHANNEL3 0
#define ADC_SCALED_3V3_CHANNEL4 0

/* fast get pin number: (x - A) * 16 + n */

#define GPIO_OTGFS_VBUS         9
#define GPIO_nVDD_USB_VALID     99
#define GPIO_nVDD_5V_HIPOWER_OC 93
#define GPIO_nVDD_5V_PERIPH_OC  79

/* navigation sensor */

/* ahrs : attitude and heading reference system */

/* gnss : global navigation satellite system */

/* imu : inertial measurement unit */

#define DRV_USING_BMI055
#define BMI055_GYRO_DEVICE_NAME  "bmi055_gyro"
#define BMI055_ACCEL_DEVICE_NAME "bmi055_accel"
#define BMI055_ROTATION          2

/* mag : magnetometer or compass */

#define DRV_USING_IST8310
#define IST8310_DEVICE_NAME "i2c3"
#define IST8310_I2C_ADDRESS 0x0e
#define IST8310_ROTATION    10

/* baro : barometric altimeter */

#define DRV_USING_MS5611
#define MS5611_DEVICE_NAME "ms5611"
#define MS5611_DEVICE_TYPE 5611

/* ads : air data system, airspeed */

/* actuator output */

#define DRV_USING_PWM_OUT
#define BOARD_HAS_PWM 8

/* engine */

/* payload */

/* traffic */

/* utilities */

#define DRV_USING_SAFETY_BUTTON

/* fast get pin number: (x - A) * 16 + n */

#define GPIO_SAFETY_SWITCH_IN  74
#define GPIO_SAFETY_SWITCH_LED 76
#define DRV_USING_TONE_ALARM

/* nextpilot airframe config */

#define SYS_USING_AIRFRAME

/* nextpilot controller config */

#define FCS_USING_RC_UPDATE
#define FCS_USING_COMMANDER
#define FCS_USING_NAVIGATOR
#define FCS_USING_LAND_DETECTOR
#define FCS_USING_FW_POS_CONTROL
#define FCS_USING_FW_ATT_CONTROL
#define FCS_USING_FLIGHT_MANAGER
#define FCS_USING_MC_HOVER_THRUST_ESTIMATOR
#define FCS_USING_MC_POS_CONTROL
#define FCS_USING_MC_ATT_CONTROL
#define FCS_USING_MC_RATE_CONTROL
#define FCS_USING_VTOL_ATT_CONTROL

/* nextpilot estimator config */

#define INS_USING_EKF2_TYPE
#define INS_USING_AIRSPEED_SELECTOR
#define INS_USING_SENSORS
#define SENSORS_USING_AIRSPEED
#define SENSORS_USING_BARO
#define SENSORS_USING_GYRO
#define SENSORS_USING_ACCEL
#define SENSORS_USING_GPS_POSITION
#define SENSORS_USING_MAG
#define SENSORS_USING_OPTICAL_FLOW
#define INS_USING_EKF2

/* nextpilot storage config */

#define PKG_USING_PARAM
#define SYS_USING_ROMFS
#define SYS_USING_DATAMAN
#define DATAMAN_DEFAULT_FILE_PATH "/mnt/microsd/etc/dataman.bin"
#define SYS_USING_LOGGER

/* nextpilot telemetry config */

#define TEL_USING_MAVLINK
#define MAVLINK_USING_RX_DMA
#define MAVLINK_USING_TX_DMA
#define MAVLINK_USING_PARAM
#define MAVLINK_USING_MISSION
#define MAVLINK_USING_SHELL
#define MAVLINK_USING_LOG_DOWNLOAD
#define MAVLINK_USING_TIMESYNC

/* nextpilot simulation config */

/* nextpilot package config */

/* common packages */

#define SYS_USING_AIRFRAME
#define PKG_USING_CONTAINER
#define PKG_USING_QUEUE
#define PKG_USING_HRTIMER
#define PKG_USING_UORB
#define PKG_USING_PARAM
#define PARAM_USING_FORMAT_JSON
#define PARAM_USING_SAVE_FILE
#define PARAM_FILE_PATH_NAME "/mnt/microsd/etc/param.bin"
#define LIB_USING_MODULE
#define PKG_USING_LIBCRC

/* math packages */

#define PKG_USING_MATHLIB
#define PKG_USING_MATRIX
#define LIB_USING_ROTATION

/* controller packages */

#define LIB_USING_AVOIDANCE
#define LIB_USING_BEZIER
#define LIB_USING_CIRCUIT_BREAKER
#define LIB_USING_COLLISION
#define LIB_USING_CONTROL_BLOCK
#define LIB_USING_HYSTERESIS
#define LIB_USING_LANDING_SLOPE
#define LIB_USING_MIXER
#define LIB_USING_MOTION_PLANNING
#define LIB_USING_SLEW_RATE
#define LIB_USING_WEATHER_VANE

/* estimator packages */

#define LIB_USING_AIRSPEED_CALC
#define LIB_USING_AIRSPEED_VALIDATOR
#define LIB_USING_SENSOR_CALIBRATION
#define LIB_USING_ECL
#define ECL_USING_EKF
#define ECL_USING_WIND_ESTIMATOR
#define ECL_USING_GEO
#define ECL_USING_MAG_LOOKUP

/* peripheral packages */

#define LIB_USING_RC
#define LIB_USING_BATTERY

/* navigation sensors */

#define LIB_USING_ACCEL
#define LIB_USING_GYRO
#define LIB_USING_MAG
#define LIB_USING_BARO
#define LIB_USING_AIRSPD

/* telemetry packages */

/* utilities pacakge */

#define PKG_USING_BOARD_VERSION
#define BOARD_TYPE_NAME "CETCS-FCS-DEFAULT"
#define BOARD_CHIP_NAME "STM32F765IIk"
#define BOARD_HAS_HW_VERSIONING
#define ADC_HW_VER_DEVICE_NAME   "adc1"
#define ADC_HW_VER_SENSE_CHANNEL 12
#define GPIO_HW_VER_DRIVE        126
#define ADC_HW_REV_SENSE_CHANNEL 13
#define GPIO_HW_REV_DRIVE        126
#define PKG_USING_PERF
#define PKG_VSNPRINTF_SUPPORT_DECIMAL_SPECIFIERS
#define PKG_VSNPRINTF_SUPPORT_EXPONENTIAL_SPECIFIERS
#define PKG_VSNPRINTF_SUPPORT_WRITEBACK_SPECIFIER
#define PKG_VSNPRINTF_SUPPORT_LONG_LONG
#define PKG_VSNPRINTF_CHECK_FOR_NUL_IN_FORMAT_SPECIFIER
#define PKG_VSNPRINTF_INTEGER_BUFFER_SIZE             32
#define PKG_VSNPRINTF_DECIMAL_BUFFER_SIZE             32
#define PKG_VSNPRINTF_DEFAULT_FLOAT_PRECISION         6
#define PKG_VSNPRINTF_MAX_INTEGRAL_DIGITS_FOR_DECIMAL 9
#define PKG_VSNPRINTF_LOG10_TAYLOR_TERMS              4

#endif
