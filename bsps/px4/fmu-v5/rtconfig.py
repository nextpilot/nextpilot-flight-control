# -*- coding: utf-8 -*-

import os
import sys

# 芯片相关
ARCH = "arm"
# CPU架构
CPU = "cortex-m7"

# 板子的名称
BOARD_NAME = os.getenv("RTT_BOARD_NAME", "px4-fmuv5-default")
# 芯片名称
CHIP_NAME = "STM32F765IIk"
# SVD文件名
SVD_FILE = ""
# 启动文件名，必须
STARTUP_FILE = "startup_stm32f765xx.s"

RTT_ROOT = os.getenv("RTT_ROOT", "../../../rtos/rt-thread")

CROSS_TOOL = os.getenv("RTT_CC", "gcc")

LINK_SCRIPT = "board/linker_scripts/link.lds"

# cross_tool provides the cross compiler
# EXEC_PATH is the compiler execute path, for example, CodeSourcery, Keil MDK, IAR
if CROSS_TOOL == "gcc":
    PLATFORM = "gcc"
    EXEC_PATH = r"C:\Users\XXYYZZ"
elif CROSS_TOOL == "keil":
    PLATFORM = "armcc"
    EXEC_PATH = r"C:/Keil_v5"
elif CROSS_TOOL == "iar":
    PLATFORM = "iccarm"
    EXEC_PATH = r"C:/Program Files (x86)/IAR Systems/Embedded Workbench 8.3"

if os.getenv("RTT_EXEC_PATH"):
    EXEC_PATH = os.getenv("RTT_EXEC_PATH")

BUILD = os.getenv("RTT_BUILD_TYPE", "release")

if PLATFORM == "gcc":
    # toolchains
    PREFIX = "arm-none-eabi-"
    CC = PREFIX + "gcc"
    AS = PREFIX + "gcc"
    AR = PREFIX + "ar"
    CXX = PREFIX + "g++"
    LINK = PREFIX + "gcc"
    TARGET_EXT = "elf"
    SIZE = PREFIX + "size"
    OBJDUMP = PREFIX + "objdump"
    OBJCPY = PREFIX + "objcopy"

    DEVICE = " -mcpu=cortex-m7 -mthumb -mfpu=fpv5-d16 -mfloat-abi=hard -ffunction-sections -fdata-sections"
    CFLAGS = (
        DEVICE
        + " -Dgcc -Wall -Wno-missing-braces -Wno-address-of-packed-member -Wno-unused-variable -Wno-unused-function -Wno-unused-const-variable"
    )
    AFLAGS = " -c" + DEVICE + " -x assembler-with-cpp -Wa,-mimplicit-it=thumb "
    LFLAGS = DEVICE + f" -Wl,--gc-sections,-Map=build/{BOARD_NAME}.map,-cref,-u,Reset_Handler -T {LINK_SCRIPT}"

    CPATH = ""
    LPATH = ""

    if BUILD == "debug":
        CFLAGS += " -O0 -gdwarf-2 -g"
        AFLAGS += " -gdwarf-2"
    else:
        CFLAGS += " -O2"

    CXXFLAGS = CFLAGS

    POST_ACTION = OBJCPY + f" -O binary $TARGET build/{BOARD_NAME}.bin \n"
    POST_ACTION += OBJCPY + f" -O ihex $TARGET build/{BOARD_NAME}.hex \n"
    POST_ACTION += SIZE + " $TARGET \n" + SIZE + f" build/{BOARD_NAME}.hex \n"

    CFLAGS += " -Werror-implicit-function-declaration"

elif PLATFORM == "armcc":
    # toolchains
    CC = "armcc"
    CXX = "armcc"
    AS = "armasm"
    AR = "armar"
    LINK = "armlink"
    TARGET_EXT = "axf"

    DEVICE = " --cpu Cortex-M7.fp.sp"
    CFLAGS = "-c " + DEVICE + " --apcs=interwork --c99"
    AFLAGS = DEVICE + " --apcs=interwork "
    LFLAGS = (
        DEVICE
        + f' --scatter "{LINK_SCRIPT}" --info sizes --info totals --info unused --info veneers --list build/{BOARD_NAME}.map --strict'
    )
    CFLAGS += " -I" + EXEC_PATH + "/ARM/ARMCC/include"
    LFLAGS += " --libpath=" + EXEC_PATH + "/ARM/ARMCC/lib"

    CFLAGS += " -D__MICROLIB "
    AFLAGS += ' --pd "__MICROLIB SETA 1" '
    LFLAGS += " --library_type=microlib "
    EXEC_PATH += "/ARM/ARMCC/bin/"

    if BUILD == "debug":
        CFLAGS += " -g -O0"
        AFLAGS += " -g"
    else:
        CFLAGS += " -O2"

    CXXFLAGS = CFLAGS

    POST_ACTION = f"fromelf --bin $TARGET --output build/{BOARD_NAME}.bin \nfromelf -z $TARGET"

elif PLATFORM == "armclang":
    # toolchains
    CC = "armclang"
    CXX = "armclang"
    AS = "armasm"
    AR = "armar"
    LINK = "armlink"
    TARGET_EXT = "axf"

    DEVICE = " --cpu Cortex-M7.fp.sp "
    CFLAGS = " --target=arm-arm-none-eabi -mcpu=cortex-m7 "
    CFLAGS += " -mcpu=cortex-m7 -mfpu=fpv5-d16 "
    CFLAGS += " -mfloat-abi=hard -c -fno-rtti -funsigned-char -fshort-enums -fshort-wchar "
    CFLAGS += " -gdwarf-3 -ffunction-sections "
    AFLAGS = DEVICE + " --apcs=interwork "
    LFLAGS = DEVICE + " --info sizes --info totals --info unused --info veneers "
    LFLAGS += f" --list build/{BOARD_NAME}.map "
    LFLAGS += f' --strict --scatter "{LINK_SCRIPT}" '
    CFLAGS += " -I" + EXEC_PATH + "/ARM/ARMCLANG/include"
    LFLAGS += " --libpath=" + EXEC_PATH + "/ARM/ARMCLANG/lib"

    EXEC_PATH += "/ARM/ARMCLANG/bin/"

    if BUILD == "debug":
        CFLAGS += " -g -O1"  # armclang recommend
        AFLAGS += " -g"
    else:
        CFLAGS += " -O2"

    CXXFLAGS = CFLAGS
    CFLAGS += " -std=c99"

    POST_ACTION = f"fromelf --bin $TARGET --output build/{BOARD_NAME}.bin \nfromelf -z $TARGET"

elif PLATFORM == "iar":
    # toolchains
    CC = "iccarm"
    CXX = "iccarm"
    AS = "iasmarm"
    AR = "iarchive"
    LINK = "ilinkarm"
    TARGET_EXT = "out"

    DEVICE = "-Dewarm"

    CFLAGS = DEVICE
    CFLAGS += " --diag_suppress Pa050"
    CFLAGS += " --no_cse"
    CFLAGS += " --no_unroll"
    CFLAGS += " --no_inline"
    CFLAGS += " --no_code_motion"
    CFLAGS += " --no_tbaa"
    CFLAGS += " --no_clustering"
    CFLAGS += " --no_scheduling"
    CFLAGS += " --endian=little"
    CFLAGS += " --cpu=Cortex-M7"
    CFLAGS += " -e"
    CFLAGS += " --fpu=VFPv5_sp"
    CFLAGS += ' --dlib_config "' + EXEC_PATH + '/arm/INC/c/DLib_Config_Normal.h"'
    CFLAGS += " --silent"

    AFLAGS = DEVICE
    AFLAGS += " -s+"
    AFLAGS += " -w+"
    AFLAGS += " -r"
    AFLAGS += " --cpu Cortex-M7"
    AFLAGS += " --fpu VFPv5_sp"
    AFLAGS += " -S"

    if BUILD == "debug":
        CFLAGS += " --debug"
        CFLAGS += " -On"
    else:
        CFLAGS += " -Oh"

    LFLAGS = f' --config "{LINK_SCRIPT}"'
    LFLAGS += " --entry __iar_program_start"

    CXXFLAGS = CFLAGS

    EXEC_PATH = EXEC_PATH + "/arm/bin/"
    POST_ACTION = f"ielftool --bin $TARGET build/{BOARD_NAME}.bin"


TARGET_FILE = f"build/{BOARD_NAME}.{TARGET_EXT}"

# make fw
# cmd = ['python', os.path.join(APP_ROOT, r'tools\scons\fw_mkpx4.py'),
#        # '--prototype', ''
#        '--board_id', '50',
#        '--board_revision', '0',
#        '--git_identity', APP_ROOT,
#        #    '--parameter_xml',
#        #    '--airframe_xml',
#        '--image', BIN_FILE,
#        '> ' + BASE_NAME + '.px4',
#        '\n'
#        ]
# print(' '.join(cmd))
# POST_ACTION += ' '.join(cmd)


def get_build_env():
    from building import GetDepend, DefaultEnvironment, Environment

    # 设置编译环境
    DefaultEnvironment(tools=[])
    env = Environment(
        tools=["mingw"],
        AS=AS,
        ASFLAGS=AFLAGS,
        CC=CC,
        CFLAGS=CFLAGS,
        AR=AR,
        ARFLAGS="-rc",
        CXX=CXX,
        CXXFLAGS=CXXFLAGS,
        LINK=LINK,
        LINKFLAGS=LFLAGS,
    )

    env.PrependENVPath("PATH", EXEC_PATH)

    if PLATFORM == "iar":
        env.Replace(CCCOM=["$CC $CFLAGS $CPPFLAGS $_CPPDEFFLAGS $_CPPINCFLAGS -o $TARGET $SOURCES"])
        env.Replace(ARFLAGS=[""])
        env.Replace(LINKCOM=env["LINKCOM"] + " --map build/{BOARD_NAME}.map")
    return env


def dist_handle(BSP_ROOT, dist_dir):
    import sys
    import os
    import shutil

    # copy apps
    src_dir = os.path.normpath(os.path.join(BSP_ROOT, "../../../apps"))
    dst_dir = os.path.normpath(os.path.join(dist_dir, "src"))
    if os.path.exists(dst_dir):
        shutil.rmtree(dst_dir)
    shutil.copytree(src_dir, dst_dir)

    # copy sdk
    src_dir = os.path.normpath(os.path.join(BSP_ROOT, "../../../rtos/platform/stm32"))
    dst_dir = os.path.normpath(os.path.join(dist_dir, "libraries"))
    if os.path.exists(dst_dir):
        shutil.rmtree(dst_dir)
    shutil.copytree(src_dir, dst_dir)

    # cwd_path = os.getcwd()
    # sys.path.append(os.path.join(os.path.dirname(BSP_ROOT), 'tools'))
    # from sdk_dist import dist_do_building
    # dist_do_building(BSP_ROOT, dist_dir)
