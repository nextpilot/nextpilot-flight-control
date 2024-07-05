# -*- coding: utf-8 -*-

import os
import sys
from scons_helper import *

# rtconfig.py保存了编译所需的变量，其中
# 一部分变量是固定的，比如硬件相关
# 一部分变量会随着命令行或者编译目标不同而变化

# 芯片相关
ARCH = "arm"
# CPU架构
CPU = "cortex-m7"

OPTIONS, CROSS_TOOL, EXEC_PATH, PLATFORM = get_build_option()

# 以下变量添加到了options字典里面，方便自动化模板生成时候使用
OPTIONS["CHIP_NAME"] = "STM32F765IIk"
# SVD文件名
OPTIONS["SVD_FILE"] = ""
# 启动文件名，必须
OPTIONS["STARTUP_FILE"] = "startup_stm32f765xx.s"


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
    LFLAGS = DEVICE + " -Wl,--gc-sections,-Map={MAP_FILE},-cref,-u,Reset_Handler -T {LINKER_FILE}".format(**OPTIONS)

    CPATH = ""
    LPATH = ""

    if OPTIONS["BUILD_TYPE"] == "debug":
        CFLAGS += " -O0 -gdwarf-2 -g"
        AFLAGS += " -gdwarf-2"
    else:
        CFLAGS += " -O2"

    CXXFLAGS = CFLAGS

    POST_ACTION = OBJCPY + " -O binary $TARGET {BIN_FILE} \n".format(**OPTIONS)
    POST_ACTION += OBJCPY + " -O ihex $TARGET {HEX_FILE} \n".format(**OPTIONS)
    POST_ACTION += SIZE + " $TARGET \n" + SIZE + " {HEX_FILE} \n".format(**OPTIONS)

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
        + ' --scatter "{LINKER_FILE}" --info sizes --info totals --info unused --info veneers --list {MAP_FILE} --strict'.format(
            **OPTIONS
        )
    )
    CFLAGS += " -I" + EXEC_PATH + "/ARM/ARMCC/include"
    LFLAGS += " --libpath=" + EXEC_PATH + "/ARM/ARMCC/lib"

    CFLAGS += " -D__MICROLIB "
    AFLAGS += ' --pd "__MICROLIB SETA 1" '
    LFLAGS += " --library_type=microlib "
    EXEC_PATH += "/ARM/ARMCC/bin/"

    if OPTIONS["BUILD_TYPE"] == "debug":
        CFLAGS += " -g -O0"
        AFLAGS += " -g"
    else:
        CFLAGS += " -O2"

    CXXFLAGS = CFLAGS

    POST_ACTION = "fromelf --bin $TARGET --output {BIN_FILE} \nfromelf -z $TARGET".format(**OPTIONS)

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
    LFLAGS += " --list build/{MAP_FILE} ".format(**OPTIONS)
    LFLAGS += r' --strict --scatter "{LINKER_FILE}" '.format(**OPTIONS)
    CFLAGS += " -I" + EXEC_PATH + "/ARM/ARMCLANG/include"
    LFLAGS += " --libpath=" + EXEC_PATH + "/ARM/ARMCLANG/lib"

    EXEC_PATH += "/ARM/ARMCLANG/bin/"

    if OPTIONS["BUILD_TYPE"] == "debug":
        CFLAGS += " -g -O1"  # armclang recommend
        AFLAGS += " -g"
    else:
        CFLAGS += " -O2"

    CXXFLAGS = CFLAGS
    CFLAGS += " -std=c99"

    POST_ACTION = "fromelf --bin $TARGET --output {BIN_FILE} \nfromelf -z $TARGET".format(**OPTIONS)

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

    if OPTIONS["BUILD_TYPE"] == "debug":
        CFLAGS += " --debug"
        CFLAGS += " -On"
    else:
        CFLAGS += " -Oh"

    LFLAGS = ' --config "{LINKER_FILE}"'.format(**OPTIONS)
    LFLAGS += " --entry __iar_program_start"

    CXXFLAGS = CFLAGS

    EXEC_PATH = EXEC_PATH + "/arm/bin/"
    POST_ACTION = "ielftool --bin $TARGET {BIN_FILE}".format(**OPTIONS)


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
        env.Replace(LINKCOM=env["LINKCOM"] + " --map {MAP_FILE}".format(**OPTIONS))
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
