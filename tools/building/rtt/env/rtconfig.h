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

/* kservice optimization */

#define RT_KPRINTF_USING_LONGLONG
#define RT_DEBUG

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
#define RT_CONSOLE_DEVICE_NAME "uart4"
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
#define RT_USING_MSH
#define RT_USING_FINSH
#define FINSH_USING_MSH
#define FINSH_THREAD_NAME       "tshell"
#define FINSH_THREAD_PRIORITY   28
#define FINSH_THREAD_STACK_SIZE 4096
#define FINSH_USING_HISTORY
#define FINSH_HISTORY_LINES 5
#define FINSH_USING_SYMTAB
#define FINSH_CMD_SIZE 80
#define MSH_USING_BUILT_IN_COMMANDS
#define FINSH_USING_DESCRIPTION
#define FINSH_ARG_MAX 10
#define RT_USING_DFS
#define DFS_USING_POSIX
#define DFS_USING_WORKDIR
#define DFS_FILESYSTEMS_MAX      4
#define DFS_FILESYSTEM_TYPES_MAX 4
#define DFS_FD_MAX               16
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
#define RT_SERIAL_RB_BUFSZ 64
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
#define RT_MMCSD_STACK_SIZE      2048
#define RT_MMCSD_THREAD_PREORITY 22
#define RT_MMCSD_MAX_PARTITION   16
#define RT_USING_SPI

/* Using USB */

#define RT_USING_USB
#define RT_USING_USB_DEVICE
#define RT_USBD_THREAD_STACK_SZ 4096
#define USB_VENDOR_ID           0x0FFE
#define USB_PRODUCT_ID          0x0001
#define _RT_USB_DEVICE_CDC
#define RT_USB_DEVICE_CDC
#define RT_VCOM_TASK_STK_SIZE 512
#define RT_CDC_RX_BUFSIZE     128
#define RT_VCOM_SERNO         "32021919830108"
#define RT_VCOM_SER_LEN       14
#define RT_VCOM_TX_TIMEOUT    1000

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

#define RT_USING_POSIX_MESSAGE_QUEUE
#define RT_USING_POSIX_MESSAGE_SEMAPHORE

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
#define ULOG_ASYNC_OUTPUT_THREAD_STACK    2048
#define ULOG_ASYNC_OUTPUT_THREAD_PRIORITY 30

/* log format */

#define ULOG_USING_COLOR
#define ULOG_OUTPUT_TIME
#define ULOG_OUTPUT_LEVEL
#define ULOG_OUTPUT_TAG
#define ULOG_BACKEND_USING_CONSOLE
#define ULOG_BACKEND_USING_FILE

/* RT-Thread Utestcases */

/* RT-Thread online packages */

/* IoT - internet of things */

/* Wi-Fi */

/* Marvell WiFi */

/* Wiced WiFi */

/* CYW43012 WiFi */

/* BL808 WiFi */

/* CYW43439 WiFi */

/* IoT Cloud */

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
#define PKG_RT_MEMCPY_CM_PATH "/packages/system/enhanced-kservice/rt_memcpy_cm"
#define PKG_USING_RT_MEMCPY_CM_LATEST_VERSION
#define PKG_RT_MEMCPY_CM_VER "latest"
#define PKG_USING_RT_VSNPRINTF_FULL
#define PKG_RT_VSNPRINTF_FULL_PATH "/packages/system/enhanced-kservice/rt_vsnprintf_full"
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
#define PKG_USING_RT_VSNPRINTF_FULL_LATEST_VERSION
#define PKG_RT_VSNPRINTF_FULL_VER "latest"

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
#define PKG_OPTPARSE_PATH "/packages/misc/optparse"
#define PKG_USING_OPTPARSE_LATEST_VERSION
#define PKG_OPTPARSE_VER "latest"

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

#define BSP_PX4_FMUv5
#define SOC_STM32H753II

/* Chip Memory Setting */

#define FLASH_START_ADDRESS    0x80000000
#define FLASH_APP_VTOR_OFFSET  128
#define FLASH_APP_TOTAL_LENGTH 1920
#define SRAM_START_ADDRESS     0x24000000
#define SRAM_TOTAL_LENGTH      512

/* On-chip Peripheral Drivers */

#define BSP_USING_GPIO
#define BSP_USING_ADC
#define BSP_USING_ADC1
#define BSP_USING_ADC3
#define BSP_USING_PWM
#define BSP_USING_PWM1
#define BSP_USING_PWM1_CH1
#define BSP_USING_PWM1_CH2
#define BSP_USING_PWM1_CH3
#define BSP_USING_PWM1_CH4
#define BSP_USING_PWM3
#define BSP_USING_PWM3_CH1
#define BSP_USING_PWM3_CH2
#define BSP_USING_PWM3_CH3
#define BSP_USING_PWM3_CH4
#define BSP_USING_PWM4
#define BSP_USING_PWM4_CH1
#define BSP_USING_PWM4_CH2
#define BSP_USING_PWM4_CH3
#define BSP_USING_PWM4_CH4
#define BSP_USING_PWM5
#define BSP_USING_PWM5_CH4
#define BSP_USING_PWM8
#define BSP_USING_PWM8_CH1
#define BSP_USING_PWM8_CH2
#define BSP_USING_PWM8_CH3
#define BSP_USING_PWM8_CH4
#define BSP_USING_UART
#define BSP_USING_UART1
#define BSP_USING_UART2
#define BSP_USING_UART3
#define BSP_USING_UART4
#define BSP_USING_UART5
#define BSP_USING_UART6
#define BSP_USING_UART7
#define BSP_UART7_TX_USING_DMA
#define BSP_UART7_RX_USING_DMA
#define BSP_USING_UART8
#define BSP_UART8_RX_USING_DMA
#define BSP_USING_SPI
#define BSP_USING_SPI4
#define BSP_USING_I2C
#define BSP_USING_USBD
#define BSP_USING_ONCHIP_RTC
#define BSP_RTC_USING_LSE
#define BSP_USING_ON_CHIP_FLASH
#define BSP_USING_SDIO
#define BSP_USING_SDIO2
#define SOC_FAMILY_STM32
#define SOC_SERIES_STM32H7

/* Onboard Peripheral Drivers */

#define MICROSD_MOUNT_POINT "/mnt/microsd"

/* Board extended module Drivers */

/* Nextpilot Firmware Config */

/* select application type */

#define APP_USING_AUTOPILOT
#define SYS_USING_AIRFRAME
#define TEL_USING_MAVLINK
#define MAVLINK_COMM_NUM_BUFFERS 4
#define MAVLINK_USING_UART
#define MAVLINK_USING_NORM_MODE
#define MAVLINK_HANDLE_COMMAND_LONG
#define MAVLINK_HANDLE_OFFBAORD_CONTROL
#define MAVLINK_HANDLE_MANUAL_CONTROL
#define MAVLINK_HANDLE_LANDING_TARGET
#define MAVLINK_HANDLE_DEBUG
#define MAVLINK_HANDLE_PARAM
#define MAVLINK_HANDLE_MISSION
#define MAVLINK_HANDLE_SHELL
#define MAVLINK_HANDLE_LOG_DOWNLOAD
#define MALVINK_LOG_ROOT_DIR "/mnt/microsd"
#define MAVLINK_HANDLE_TIMESYNC
#define PKG_USING_UORB
#define UORB_USING_CALLBACK
#define PKG_USING_PARAM
#define PARAM_USING_SAVE_FRAM
#define PARAM_FRAM_DEVICE_NAME "fm25v02"
#define PARAM_USING_FORMAT_JSON
#define SYS_USING_DATAMAN
#define DATAMAN_DEFAULT_FILE_PATH "/mnt/microsd/etc/dataman.bin"
#define SYS_USING_LOGGER
#define LOGGER_FULL_LOG_ROOT_PATH "/mnt/microsd/log"
#define LOGGER_LESS_LOG_ROOT_PATH "/mnt/micorsd/mission_log"
#define PKG_USING_BOARD_VERSION
#define BOARD_TYPE_NAME "PX4-FMUv5-DEFAULT"
#define BOARD_CHIP_NAME "STM32H753IIk"
#define SYS_USING_ROMFS
#define DRV_USING_RC_INPUT_OLD
#define RC_SERIAL_PORT "uart8"
#define DRV_USING_BATTERY_STATUS
#define BOARD_NUMBER_BRICKS          1
#define ADC_BATTERY_DEVICE_NAME      "adc3"
#define ADC_BATTERY1_VOLTAGE_CHANNEL 4
#define ADC_BATTERY1_CURRENT_CHANNEL 1
#define SGD_INS_UART_NAME            "uart7"
#define SGD_INS_UART_BAUDRATE        460800
#define SGD_INS_UART_SWAP_RXTX
#define DRV_USING_SGD_INS_V2
#define DRV_USING_PWM_OUT_OLD
#define BOARD_HAS_PWM 8
#define DRV_USING_CYS90H
#define CYS90H_UART_NAME         "uart5"
#define CYS90H_UART_BAUDRATE     "115200"
#define CYS90H_THROTTLE_PWM_NAME "pwm8"
#define CYS90H_THROTTLE_PWM_CHAN 3
#define CYS90H_ENING_START_PIN   129
#define CYS90H_CHARGE_START_PIN  131
#define CYS90H_PUMP_START_PIN    4
#define DRV_USING_TONE_ALARM
#define BUZZER_TIMER_NAME "pwm5"
#define BUZZER_TIMER_CHAN 4
#define PKG_USING_CONTAINER
#define PKG_USING_QUEUE
#define PKG_USING_HRTIMER
#define HRT_USING_SYSTICK
#define LIB_USING_MODULE
#define PKG_USING_LIBCRC
#define PKG_USING_MATHLIB
#define PKG_USING_MATRIX
#define LIB_USING_ROTATION
#define LIB_USING_AVOIDANCE
#define LIB_USING_BEZIER
#define PKG_USING_CIRCUIT_BREAKER
#define LIB_USING_COLLISION
#define LIB_USING_CONTROL_BLOCK
#define LIB_USING_HYSTERESIS
#define LIB_USING_LANDING_SLOPE
#define LIB_USING_MIXER
#define LIB_USING_MOTION_PLANNING
#define LIB_USING_SLEW_RATE
#define LIB_USING_WEATHER_VANE
#define LIB_USING_AIRSPEED_CALC
#define LIB_USING_SENSOR_CALIBRATION
#define LIB_USING_ECL
#define ECL_USING_GEO
#define ECL_USING_MAG_LOOKUP
#define LIB_USING_ACCEL
#define LIB_USING_GYRO
#define LIB_USING_MAG
#define LIB_USING_BARO
#define LIB_USING_AIRSPD
#define LIB_USING_BATTERY
#define LIB_USING_MAVLINK
#define PKG_USING_PERF

/* nextpilot airframe config */

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

#define INS_USING_FAKE_TYPE
#define INS_USING_SENSORS_FAKE
#define INS_USING_EKF2_FAKE

/* nextpilot telemetry config */

/* select mavlink mode */

/* nextpilot simulation config */

#define SIM_USING_SIH
#define SIM_USING_BATTERY
#define SIM_USING_AIRSPEED
#define SIM_USING_BARO
#define SIM_USING_GPS
#define SIM_USING_MAG
#define SIM_USING_PWM_OUT

/* nextpilot universal config */

/* select device to save param */

/* select format for save param */

/* nextpilot peripheral config */

/* radio control */

/* battery monitor */

/* navigator sensor */

/* ahrs : attitude and heading reference system */

/* gnss : global navigation satellite system */

/* imu : inertial measurement unit */

/* mag : magnetometer or compass */

/* baro : barometric altimeter */

/* ads : air data system, airspeed */

/* actuator output */

/* engine output */

/* payload */

/* traffic */

/* utilities */

/* nextpilot libraries config */

/* common packages */

/* math packages */

/* controller packages */

/* estimator packages */

/* peripheral packages */

/* navigator sensor: imu, mag, baro, gps etc */

/* telemetry packages */

/* utilities pacakge */

#endif
