#ifndef RT_CONFIG_H__
#define RT_CONFIG_H__

/* Automatically generated file; DO NOT EDIT. */
/* RT-Thread Project Configuration */

/* RT-Thread Kernel */

#define RT_NAME_MAX 20
#define RT_USING_SMP
#define RT_CPUS_NR 2
#define RT_ALIGN_SIZE 4
#define RT_THREAD_PRIORITY_32
#define RT_THREAD_PRIORITY_MAX 32
#define RT_TICK_PER_SECOND 1000
#define RT_USING_OVERFLOW_CHECK
#define RT_USING_HOOK
#define RT_HOOK_USING_FUNC_PTR
#define RT_USING_IDLE_HOOK
#define RT_IDLE_HOOK_LIST_SIZE 4
#define IDLE_THREAD_STACK_SIZE 1024
#define SYSTEM_THREAD_STACK_SIZE 1024
#define RT_USING_TIMER_SOFT
#define RT_TIMER_THREAD_PRIO 4
#define RT_TIMER_THREAD_STACK_SIZE 1024

/* kservice optimization */

#define RT_KPRINTF_USING_LONGLONG
#define RT_DEBUG
#define RT_DEBUG_COLOR

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
#define RT_USING_SLAB
#define RT_USING_MEMHEAP
#define RT_MEMHEAP_FAST_MODE
#define RT_USING_SMALL_MEM_AS_HEAP
#define RT_USING_MEMTRACE
#define RT_USING_HEAP

/* Kernel Device Object */

#define RT_USING_DEVICE
#define RT_USING_DEVICE_OPS
#define RT_USING_INTERRUPT_INFO
#define RT_USING_CONSOLE
#define RT_CONSOLEBUF_SIZE 256
#define RT_CONSOLE_DEVICE_NAME "uart0"
#define RT_VER_NUM 0x40101
#define ARCH_ARM
#define RT_USING_CPU_FFS
#define ARCH_ARM_CORTEX_A
#define RT_USING_GIC_V2
#define ARCH_ARM_CORTEX_A9

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
#define FINSH_THREAD_PRIORITY 28
#define FINSH_THREAD_STACK_SIZE 8192
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
#define DFS_FILESYSTEMS_MAX 4
#define DFS_FILESYSTEM_TYPES_MAX 8
#define DFS_FD_MAX 32
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
#define RT_DFS_ELM_MAX_SECTOR_SIZE 4096
#define RT_DFS_ELM_REENTRANT
#define RT_DFS_ELM_MUTEX_TIMEOUT 3000
#define RT_USING_DFS_DEVFS
#define RT_USING_DFS_ROMFS
#define RT_USING_LWP

/* Device Drivers */

#define RT_USING_DEVICE_IPC
#define RT_USING_SYSTEM_WORKQUEUE
#define RT_SYSTEM_WORKQUEUE_STACKSIZE 2048
#define RT_SYSTEM_WORKQUEUE_PRIORITY 23
#define RT_USING_SERIAL
#define RT_USING_SERIAL_V1
#define RT_SERIAL_USING_DMA
#define RT_SERIAL_RB_BUFSZ 128
#define RT_USING_I2C
#define RT_USING_I2C_BITOPS
#define RT_USING_PIN
#define RT_USING_MTD_NOR
#define RT_USING_MTD_NAND
#define RT_MTD_NAND_DEBUG
#define RT_USING_RTC
#define RT_USING_SOFT_RTC
#define RT_USING_SDIO
#define RT_SDIO_STACK_SIZE 512
#define RT_SDIO_THREAD_PRIORITY 15
#define RT_MMCSD_STACK_SIZE 1024
#define RT_MMCSD_THREAD_PREORITY 22
#define RT_MMCSD_MAX_PARTITION 16
#define RT_USING_SPI
#define RT_USING_SPI_MSD
#define RT_USING_SFUD
#define RT_SFUD_USING_SFDP
#define RT_SFUD_USING_FLASH_INFO_TABLE
#define RT_SFUD_SPI_MAX_HZ 50000000
#define RT_USING_WDT

/* Using USB */


/* C/C++ and POSIX layer */

#define RT_LIBC_DEFAULT_TIMEZONE 8

/* POSIX (Portable Operating System Interface) layer */

#define RT_USING_POSIX_FS
#define RT_USING_POSIX_DEVIO
#define RT_USING_POSIX_STDIO
#define RT_USING_POSIX_POLL
#define RT_USING_POSIX_SELECT
#define RT_USING_POSIX_SOCKET
#define RT_USING_POSIX_TERMIOS
#define RT_USING_POSIX_AIO
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

#define RT_USING_SAL
#define SAL_INTERNET_CHECK

/* Docking with protocol stacks */

#define SAL_USING_LWIP
#define SAL_USING_POSIX
#define RT_USING_NETDEV
#define NETDEV_USING_IFCONFIG
#define NETDEV_USING_PING
#define NETDEV_USING_NETSTAT
#define NETDEV_USING_AUTO_DEFAULT
#define NETDEV_IPV4 1
#define NETDEV_IPV6 0
#define RT_USING_LWIP
#define RT_USING_LWIP203
#define RT_USING_LWIP_VER_NUM 0x20003
#define RT_LWIP_MEM_ALIGNMENT 4
#define RT_LWIP_IGMP
#define RT_LWIP_ICMP
#define RT_LWIP_DNS
#define RT_LWIP_DHCP
#define IP_SOF_BROADCAST 1
#define IP_SOF_BROADCAST_RECV 1

/* Static IPv4 Address */

#define RT_LWIP_IPADDR "192.168.1.50"
#define RT_LWIP_GWADDR "192.168.1.1"
#define RT_LWIP_MSKADDR "255.255.255.0"
#define RT_LWIP_UDP
#define RT_LWIP_TCP
#define RT_LWIP_RAW
#define RT_MEMP_NUM_NETCONN 8
#define RT_LWIP_PBUF_NUM 16
#define RT_LWIP_RAW_PCB_NUM 4
#define RT_LWIP_UDP_PCB_NUM 4
#define RT_LWIP_TCP_PCB_NUM 4
#define RT_LWIP_TCP_SEG_NUM 40
#define RT_LWIP_TCP_SND_BUF 8196
#define RT_LWIP_TCP_WND 8196
#define RT_LWIP_TCPTHREAD_PRIORITY 10
#define RT_LWIP_TCPTHREAD_MBOX_SIZE 8
#define RT_LWIP_TCPTHREAD_STACKSIZE 2048
#define RT_LWIP_ETHTHREAD_PRIORITY 12
#define RT_LWIP_ETHTHREAD_STACKSIZE 1024
#define RT_LWIP_ETHTHREAD_MBOX_SIZE 8
#define RT_LWIP_REASSEMBLY_FRAG
#define LWIP_NETIF_STATUS_CALLBACK 1
#define LWIP_NETIF_LINK_CALLBACK 1
#define SO_REUSE 1
#define LWIP_SO_RCVTIMEO 1
#define LWIP_SO_SNDTIMEO 1
#define LWIP_SO_RCVBUF 1
#define LWIP_SO_LINGER 0
#define LWIP_NETIF_LOOPBACK 0
#define RT_LWIP_USING_PING

/* Utilities */

#define RT_USING_ULOG
#define ULOG_OUTPUT_LVL_D
#define ULOG_OUTPUT_LVL 7
#define ULOG_ASSERT_ENABLE
#define ULOG_LINE_BUF_SIZE 128
#define ULOG_USING_ASYNC_OUTPUT
#define ULOG_ASYNC_OUTPUT_BUF_SIZE 8192
#define ULOG_ASYNC_OUTPUT_BY_THREAD
#define ULOG_ASYNC_OUTPUT_THREAD_STACK 2048
#define ULOG_ASYNC_OUTPUT_THREAD_PRIORITY 30

/* log format */

#define ULOG_OUTPUT_FLOAT
#define ULOG_USING_COLOR
#define ULOG_OUTPUT_TIME
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

#define LIB_USING_MAVLINK
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

#define BSP_USING_QEMU

/* Nextpilot Hardware Config */

#define SOC_VEXPRESS_A9

/* Onboard Peripheral Drivers */

#define RT_USING_UART0
#define RT_USING_UART1
#define BSP_DRV_EMAC

/* Nextpilot Firmware Config */

/* select application type */

#define APP_USING_AUTOPILOT
#define SYS_USING_AIRFRAME
#define TEL_USING_MAVLINK
#define MAVLINK_COMM_NUM_BUFFERS 2
#define MAVLINK_USING_UART
#define MAVLINK_USING_UDP
#define MAVLINK_USING_NORM_MODE
#define MAVLINK_HANDLE_COMMAND_LONG
#define MAVLINK_HANDLE_OFFBAORD_CONTROL
#define MAVLINK_HANDLE_FOLLOW_TARGET
#define MAVLINK_HANDLE_MANUAL_CONTROL
#define MAVLINK_HANDLE_LANDING_TARGET
#define MAVLINK_HANDLE_TRANSPONDER
#define MAVLINK_HANDLE_EXTERNAL_VISION
#define MAVLINK_HANDLE_OBSTACLE
#define MAVLINK_HADNLE_TRAJECTORY
#define MAVLINK_HANDLE_GENERATOR
#define MAVLINK_HANDLE_DEBUG
#define MAVLINK_HANDLE_GIMBAL_CONTROL
#define MAVLINK_HANDLE_PARAM
#define MAVLINK_HANDLE_MISSION
#define MAVLINK_HANDLE_SHELL
#define MAVLINK_HANDLE_LOG_DOWNLOAD
#define MALVINK_LOG_ROOT_DIR "/mnt/microsd/log"
#define MAVLINK_HANDLE_TIMESYNC yp
#define SYS_USING_UORB
#define UORB_USING_CALLBACK
#define SYS_USING_PARAM
#define PARAM_USING_SAVE_FILE
#define PARAM_DEFAULT_FILE_PATH "/mnt/microsd/etc/param.bin"
#define SYS_USING_DATAMAN
#define DATAMAN_DEFAULT_FILE_PATH "/mnt/microsd/etc/dataman.bin"
#define SYS_USING_LOGGER
#define LOGGER_FULL_LOG_ROOT_PATH "/mnt/microsd/log"
#define LOGGER_LESS_LOG_ROOT_PATH "/mnt/micorsd/mission_log"
#define SYS_USING_VERSION
#define BOARD_TYPE_NAME "SITL-QEMU-DEFAULT"
#define BOARD_CHIP_NAME "Cortex-A9"
#define SYS_USING_ROMFS
#define LIB_USING_CONTAINER
#define LIB_USING_QUEUE
#define LIB_USING_HRTIMER
#define LIB_USING_MODULE
#define LIB_USING_LIBCRC
#define LIB_USING_MATHLIB
#define LIB_USING_MATRIX
#define LIB_USING_ROTATION
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
#define LIB_USING_AIRSPEED_CALC
#define LIB_USING_AIRSPEED_VALIDATOR
#define LIB_USING_SENSOR_CALIBRATION
#define LIB_USING_ECL
#define ECL_USING_EKF
#define ECL_USING_WIND_ESTIMATOR
#define ECL_USING_GEO
#define ECL_USING_MAG_LOOKUP
#define LIB_USING_ACCEL
#define LIB_USING_GYRO
#define LIB_USING_MAG
#define LIB_USING_BARO
#define LIB_USING_AIRSPD
#define LIB_USING_BATTERY
#define PKG_USING_PERF
#define PKG_VSNPRINTF_SUPPORT_DECIMAL_SPECIFIERS
#define PKG_VSNPRINTF_SUPPORT_EXPONENTIAL_SPECIFIERS
#define PKG_VSNPRINTF_SUPPORT_WRITEBACK_SPECIFIER
#define PKG_VSNPRINTF_SUPPORT_LONG_LONG
#define PKG_VSNPRINTF_CHECK_FOR_NUL_IN_FORMAT_SPECIFIER
#define PKG_VSNPRINTF_INTEGER_BUFFER_SIZE 32
#define PKG_VSNPRINTF_DECIMAL_BUFFER_SIZE 32
#define PKG_VSNPRINTF_DEFAULT_FLOAT_PRECISION 6
#define PKG_VSNPRINTF_MAX_INTEGRAL_DIGITS_FOR_DECIMAL 9
#define PKG_VSNPRINTF_LOG10_TAYLOR_TERMS 4

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
