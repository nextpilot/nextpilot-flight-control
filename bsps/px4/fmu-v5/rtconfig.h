#ifndef RT_CONFIG_H__
#define RT_CONFIG_H__

/* RT-Thread Kernel */

#define RT_NAME_MAX 16
#define RT_ALIGN_SIZE 4
#define RT_THREAD_PRIORITY_32
#define RT_THREAD_PRIORITY_MAX 32
#define RT_TICK_PER_SECOND 1000
#define RT_USING_OVERFLOW_CHECK
#define RT_USING_HOOK
#define RT_HOOK_USING_FUNC_PTR
#define RT_USING_IDLE_HOOK
#define RT_IDLE_HOOK_LIST_SIZE 4
#define IDLE_THREAD_STACK_SIZE 256
#define RT_USING_TIMER_SOFT
#define RT_TIMER_THREAD_PRIO 4
#define RT_TIMER_THREAD_STACK_SIZE 512

/* kservice optimization */

#define RT_KSERVICE_USING_STDLIB
/* end of kservice optimization */
#define RT_USING_DEBUG
#define RT_DEBUGING_COLOR
#define RT_DEBUGING_CONTEXT

/* Inter-Thread communication */

#define RT_USING_SEMAPHORE
#define RT_USING_MUTEX
#define RT_USING_EVENT
#define RT_USING_MAILBOX
#define RT_USING_MESSAGEQUEUE
/* end of Inter-Thread communication */

/* Memory Management */

#define RT_USING_MEMPOOL
#define RT_USING_SMALL_MEM
#define RT_USING_SMALL_MEM_AS_HEAP
#define RT_USING_HEAP
/* end of Memory Management */

/* Kernel Device Object */

#define RT_USING_DEVICE
#define RT_USING_CONSOLE
#define RT_CONSOLEBUF_SIZE 256
#define RT_CONSOLE_DEVICE_NAME "uart7"
/* end of Kernel Device Object */
#define RT_VER_NUM 0x50002
/* end of RT-Thread Kernel */

/* RT-Thread Libcpu */

#define RT_USING_CACHE
#define RT_USING_HW_ATOMIC
#define RT_USING_CPU_FFS
#define ARCH_ARM
#define ARCH_ARM_CORTEX_M
#define ARCH_ARM_CORTEX_M7
/* end of RT-Thread Libcpu */

/* RT-Thread Components */

#define RT_USING_COMPONENTS_INIT
#define RT_USING_USER_MAIN
#define RT_MAIN_THREAD_STACK_SIZE 2048
#define RT_MAIN_THREAD_PRIORITY 10
#define RT_USING_LEGACY
#define RT_USING_MSH
#define RT_USING_FINSH
#define FINSH_USING_MSH
#define FINSH_THREAD_NAME "tshell"
#define FINSH_THREAD_PRIORITY 20
#define FINSH_THREAD_STACK_SIZE 4096
#define FINSH_USING_HISTORY
#define FINSH_HISTORY_LINES 5
#define FINSH_USING_SYMTAB
#define FINSH_CMD_SIZE 80
#define MSH_USING_BUILT_IN_COMMANDS
#define FINSH_USING_DESCRIPTION
#define FINSH_ARG_MAX 10
#define FINSH_USING_OPTION_COMPLETION

/* DFS: device virtual file system */

#define RT_USING_DFS
#define DFS_USING_POSIX
#define DFS_USING_WORKDIR
#define DFS_FD_MAX 16
#define RT_USING_DFS_V1
#define DFS_FILESYSTEMS_MAX 4
#define DFS_FILESYSTEM_TYPES_MAX 4
#define RT_USING_DFS_ELMFAT

/* elm-chan's FatFs, Generic FAT Filesystem Module */

#define RT_DFS_ELM_CODE_PAGE 437
#define RT_DFS_ELM_WORD_ACCESS
#define RT_DFS_ELM_USE_LFN_3
#define RT_DFS_ELM_USE_LFN 3
#define RT_DFS_ELM_LFN_UNICODE_0
#define RT_DFS_ELM_LFN_UNICODE 0
#define RT_DFS_ELM_MAX_LFN 255
#define RT_DFS_ELM_DRIVES 2
#define RT_DFS_ELM_MAX_SECTOR_SIZE 512
#define RT_DFS_ELM_REENTRANT
#define RT_DFS_ELM_MUTEX_TIMEOUT 3000
/* end of elm-chan's FatFs, Generic FAT Filesystem Module */
#define RT_USING_DFS_DEVFS
/* end of DFS: device virtual file system */

/* Device Drivers */

#define RT_USING_DEVICE_IPC
#define RT_UNAMED_PIPE_NUMBER 64
#define RT_USING_SERIAL
#define RT_USING_SERIAL_V1
#define RT_SERIAL_USING_DMA
#define RT_SERIAL_RB_BUFSZ 256
#define RT_USING_PIN
#define RT_USING_ADC
#define RT_USING_PWM
#define RT_USING_RTC
#define RT_USING_SDIO
#define RT_SDIO_STACK_SIZE 512
#define RT_SDIO_THREAD_PRIORITY 15
#define RT_MMCSD_STACK_SIZE 1024
#define RT_MMCSD_THREAD_PREORITY 22
#define RT_MMCSD_MAX_PARTITION 16
#define RT_USING_SPI
#define RT_USING_KTIME

/* Using USB */

/* end of Using USB */
/* end of Device Drivers */

/* C/C++ and POSIX layer */

/* ISO-ANSI C layer */

/* Timezone and Daylight Saving Time */

#define RT_LIBC_USING_LIGHT_TZ_DST
#define RT_LIBC_TZ_DEFAULT_HOUR 8
#define RT_LIBC_TZ_DEFAULT_MIN 0
#define RT_LIBC_TZ_DEFAULT_SEC 0
/* end of Timezone and Daylight Saving Time */
/* end of ISO-ANSI C layer */

/* POSIX (Portable Operating System Interface) layer */

#define RT_USING_POSIX_DELAY
#define RT_USING_POSIX_CLOCK
#define RT_USING_PTHREADS
#define PTHREAD_NUM_MAX 8

/* Interprocess Communication (IPC) */


/* Socket is in the 'Network' category */

/* end of Interprocess Communication (IPC) */
/* end of POSIX (Portable Operating System Interface) layer */
#define RT_USING_CPLUSPLUS
/* end of C/C++ and POSIX layer */

/* Network */

/* end of Network */

/* Utilities */

#define RT_USING_ULOG
#define ULOG_OUTPUT_LVL_D
#define ULOG_OUTPUT_LVL 7
#define ULOG_ASSERT_ENABLE
#define ULOG_LINE_BUF_SIZE 128
#define ULOG_USING_ASYNC_OUTPUT
#define ULOG_ASYNC_OUTPUT_BUF_SIZE 2048
#define ULOG_ASYNC_OUTPUT_BY_THREAD
#define ULOG_ASYNC_OUTPUT_THREAD_STACK 2048
#define ULOG_ASYNC_OUTPUT_THREAD_PRIORITY 30

/* log format */

#define ULOG_USING_COLOR
#define ULOG_OUTPUT_TIME
#define ULOG_OUTPUT_LEVEL
#define ULOG_OUTPUT_TAG
/* end of log format */
#define ULOG_BACKEND_USING_CONSOLE
#define ULOG_BACKEND_USING_FILE
/* end of Utilities */
/* end of RT-Thread Components */

/* RT-Thread Utestcases */

/* end of RT-Thread Utestcases */

/* RT-Thread online packages */

/* IoT - internet of things */


/* Wi-Fi */

/* Marvell WiFi */

/* end of Marvell WiFi */

/* Wiced WiFi */

/* end of Wiced WiFi */

/* CYW43012 WiFi */

/* end of CYW43012 WiFi */

/* BL808 WiFi */

/* end of BL808 WiFi */

/* CYW43439 WiFi */

/* end of CYW43439 WiFi */
/* end of Wi-Fi */

/* IoT Cloud */

/* end of IoT Cloud */
/* end of IoT - internet of things */

/* security packages */

/* end of security packages */

/* language packages */

/* JSON: JavaScript Object Notation, a lightweight data-interchange format */

/* end of JSON: JavaScript Object Notation, a lightweight data-interchange format */

/* XML: Extensible Markup Language */

/* end of XML: Extensible Markup Language */
/* end of language packages */

/* multimedia packages */

/* LVGL: powerful and easy-to-use embedded GUI library */

/* end of LVGL: powerful and easy-to-use embedded GUI library */

/* u8g2: a monochrome graphic library */

/* end of u8g2: a monochrome graphic library */
/* end of multimedia packages */

/* tools packages */

/* end of tools packages */

/* system packages */

/* enhanced kernel services */

/* end of enhanced kernel services */

/* acceleration: Assembly language or algorithmic acceleration packages */

/* end of acceleration: Assembly language or algorithmic acceleration packages */

/* CMSIS: ARM Cortex-M Microcontroller Software Interface Standard */

/* end of CMSIS: ARM Cortex-M Microcontroller Software Interface Standard */

/* Micrium: Micrium software products porting for RT-Thread */

/* end of Micrium: Micrium software products porting for RT-Thread */
/* end of system packages */

/* peripheral libraries and drivers */

/* HAL & SDK Drivers */

/* STM32 HAL & SDK Drivers */

/* end of STM32 HAL & SDK Drivers */

/* Kendryte SDK */

/* end of Kendryte SDK */
/* end of HAL & SDK Drivers */

/* sensors drivers */

/* end of sensors drivers */

/* touch drivers */

/* end of touch drivers */
/* end of peripheral libraries and drivers */

/* AI packages */

/* end of AI packages */

/* Signal Processing and Control Algorithm Packages */

/* end of Signal Processing and Control Algorithm Packages */

/* miscellaneous packages */

/* project laboratory */

/* end of project laboratory */

/* samples: kernel and components samples */

/* end of samples: kernel and components samples */

/* entertainment: terminal games and other interesting software packages */

/* end of entertainment: terminal games and other interesting software packages */
/* end of miscellaneous packages */

/* Arduino libraries */


/* Projects and Demos */

/* end of Projects and Demos */

/* Sensors */

/* end of Sensors */

/* Display */

/* end of Display */

/* Timing */

/* end of Timing */

/* Data Processing */

/* end of Data Processing */

/* Data Storage */

/* Communication */

/* end of Communication */

/* Device Control */

/* end of Device Control */

/* Other */

/* end of Other */

/* Signal IO */

/* end of Signal IO */

/* Uncategorized */

/* end of Arduino libraries */
/* end of RT-Thread online packages */

/* RT-Thread HAL Drivers */

#define SOC_FAMILY_STM32
#define SOC_SERIES_STM32F7
/* end of RT-Thread HAL Drivers */

/* Nextpilot Hardware Config */

#define BSP_PX4_FMUv5
#define SOC_STM32F765II

/* Chip Memory Setting */

#define FLASH_START_ADDRESS 0x08000000
#define FLASH_APP_VTOR_OFFSET 0
#define FLASH_APP_TOTAL_LENGTH 2048
#define SRAM_START_ADDRESS 0x20000000
#define SRAM_TOTAL_LENGTH 512
/* end of Chip Memory Setting */

/* On-chip Peripheral Drivers */

#define BSP_USING_GPIO
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
/* end of On-chip Peripheral Drivers */

/* Onboard Peripheral Drivers */

/* Board extended module Drivers */

/* end of Nextpilot Hardware Config */

/* Nextpilot Firmware Config */

#define BOARD_CONSTRAINED_MEMORY
#define BOARD_CONSTRAINED_FLASH

/* Nextpilot Airframe Config */

#define SYS_USING_AIRFRAME
#define SYS_USING_1100_QUAD_X
#define SYS_USING_1101_PLANE_AERT
#define SYS_USING_1102_TAILSITTER_DUO
#define SYS_USING_1103_SIH_STANDARD_VTOL
/* end of Nextpilot Airframe Config */

/* Nextpilot Bootloader Config */

/* Nextpilot Controller Config */

#define FCS_USING_COMMANDER
#define FCS_USING_CONTROL_ALLOCATOR
#define FCS_USING_FLIGHT_MODE_MANAGER
#define FCS_USING_FW_ATT_CONTROL
#define FCS_USING_FW_AUTOTUNE_ATTITUDE_CONTROL
#define FCS_UISNG_FW_POS_CONTROL
#define FCS_USING_FW_RATE_CONTROL
#define FCS_USING_LAND_DETECTOR
#define FCS_USING_LANDING_TARGET_ESTIMATOR
#define FCS_USING_MANUAL_CONTROL
#define FCS_USING_MC_ATT_CONTROL
#define FCS_MC_AUTOTUNE_ATTITUDE_CONTROL
#define FCS_USING_MC_HOVER_THRUST_ESTIMATOR
#define FCS_USING_MC_POS_CONTROL
#define FCS_USING_MC_RATE_CONTROL
#define FCS_USING_NAVIGATOR
#define FCS_USING_RC_UPDATE
#define FCS_USING_VTOL_ATT_CONTROL
/* end of Nextpilot Controller Config */

/* Nextpilot Estimator Config */

/* end of Nextpilot Estimator Config */

/* Nextpilot Libraries Config */

/* Controller Libraries Config */

#define PKG_USING_ADSB
#define PKG_USING_AVOIDANCE
#define PKG_USING_BEZIER
#define PKG_USING_CIRCUIT_BREAKER
#define PKG_USING_COLLISION_PREVENTION
#define PKG_USING_CONTROLLIB
#define PKG_USING_ROTATION
#define PKG_USING_GEO
#define PKG_USING_HYSTERESIS
#define PKG_USING_ECL
#define PKG_USING_MIXER_MODULE
#define PKG_USING_MOTION_PLANNING
#define PKG_USING_NPFG
#define PKG_USING_PID
#define PKG_USING_PID_DESIGN
#define PKG_USING_RATE_CONTROL
#define PKG_USING_SLEW_RATE
#define PKG_USING_SYSTEM_IDENTIFICATION
#define PKG_USING_TECS
#define PKG_USING_WEATHER_VANE
/* end of Controller Libraries Config */

/* Estimator Libraries Config */

#define PKG_USING_SENSOR_CALIBRATION
#define PKG_USING_WIND_ESTIMATOR
#define PKG_USING_WORLD_MAGNETIC_MODEL
/* end of Estimator Libraries Config */

/* Peripheral Libraries Config */

#define PKG_USING_TUNES
#define PKG_USING_ACCELEROMETER
#define PKG_USING_AIRSPEED
#define PKG_USING_GYROSCOPE
#define PKG_USING_MAGNETOMETER
#define PKG_USING_RANGEFINDER
/* end of Peripheral Libraries Config */
#define PKG_USING_BUTTON_PUBLISHER
#define PKG_USING_TIMESYNC
/* end of Nextpilot Libraries Config */

/* Nextpilot Peripheral Config */

/* Nextpilot Simulation Config */

/* end of Nextpilot Simulation Config */

/* Nextpilot Storage Config */

#define SYS_USING_DATAMAN
#define DATAMAN_DEFAULT_FILE_PATH "/dataman.bin"
#define SYS_USING_LOGGER
#define LOGGER_FULL_LOG_ROOT_PATH "/log"
#define LOGGER_LESS_LOG_ROOT_PATH "/mission_log"
/* end of Nextpilot Storage Config */

/* Nextpilot Telemetry Config */

#define TEL_USING_MAVLINK
#define MAVLINK_DIALECT "common"
#define MAVLINK_USING_PARAM
#define MAVLINK_USING_MISSION
/* end of Nextpilot Telemetry Config */
/* end of Nextpilot Firmware Config */

/* Nextpilot Packages Config */

#define PKG_USING_CONSOLE_BUFFER
#define PKG_USING_CONTAINER
#define PKG_USING_CPULOAD
#define PKG_USING_EVENTS
#define PKG_USING_GETOPT
#define PKG_USING_HRTIMER
#define PKG_USING_HRTIMER_V3
#define HRT_USING_OSTICK
#define HRT_USING_OSTIMER
#define PKG_USING_LIBCRC
#define PKG_USING_MATHLIB
#define PKG_USING_MATRIX
#define PKG_USING_PARAM
#define PARAM_USING_GLOBAL_AUTOGEN
#define PARAM_USING_STORAGE_FILE
#define PARAM_DEFAULT_FILE_PATH "/param.bin"
#define PARAM_USING_DEVICE_FILE
#define PKG_USING_PERF
#define PKG_USING_QUEUE
#define PKG_USING_RING_BUFFER
#define PKG_USING_SHUTDOWN
#define PKG_USING_UORB

/* ULog backend */

/* end of ULog backend */
#define PKG_USING_VAR_LEN_RINGBUFFER
#define PKG_USING_VCONSOLE_V2
#define PKG_USING_BOARD_VERSION
#define BOARD_TYPE_NAME "unkown"
#define BOARD_CHIP_NAME "unkown"
#define PKG_USING_WORKQUEUE
/* end of Nextpilot Packages Config */

#endif
