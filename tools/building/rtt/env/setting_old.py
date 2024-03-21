# -*- coding: utf-8 -*-

import os
import sys

from SCons.Script import *

__options__ = {}


def get_project_option():
    #######################################################################################
    # 获取ENV路径
    #######################################################################################
    if os.getenv("ENV_ROOT"):
        RTT_ENV_ROOT = os.path.abspath(os.getenv("ENV_ROOT")).replace("\\", "/")
    else:
        print("ERROR: please run scons in env.exe console")
        exit(-1)

    if r"platform/env_released/env" in RTT_ENV_ROOT:
        RTT_STD_ROOT = RTT_ENV_ROOT.replace(r"/platform/env_released/env", "")
    else:
        RTT_STD_ROOT = os.getenv("RTT_STD_ROOT", "UNKOWN")

    #######################################################################################
    # 获取各种路径
    #######################################################################################
    BSP_ROOT = Dir("#").abspath
    if os.path.exists(BSP_ROOT + "/rtos"):  # nexpilot根目录
        PRJ_ROOT = BSP_ROOT
        APP_ROOT = os.path.normpath(PRJ_ROOT + "/apps")
        PKG_ROOT = os.path.normpath(PRJ_ROOT + "apps/package")
        RTT_ROOT = os.path.normpath(PRJ_ROOT + "/rtos/rt-thread")
        SDK_ROOT = os.path.normpath(PRJ_ROOT + "/rtos/platform")
        BSP_NAME = os.path.basename(BSP_ROOT)
    elif os.path.exists(BSP_ROOT + "/rt-thread"):  # rt-thread bsp目录
        PRJ_ROOT = BSP_ROOT
        APP_ROOT = os.path.normpath(PRJ_ROOT + "/applictions")
        PKG_ROOT = os.path.normpath(PRJ_ROOT + "packages")
        RTT_ROOT = os.path.normpath(PRJ_ROOT + "/rt-thread")
        SDK_ROOT = os.path.normpath(PRJ_ROOT + "/libraries")
        BSP_NAME = os.path.basename(BSP_ROOT)
    else:
        PRJ_ROOT = os.path.normpath(BSP_ROOT + "/../../..")  # nextpilot bsp目录
        APP_ROOT = os.path.normpath(PRJ_ROOT + "/apps")
        PKG_ROOT = os.path.normpath(PRJ_ROOT + "apps/package")
        RTT_ROOT = os.path.normpath(PRJ_ROOT + "/rtos/rt-thread")
        SDK_ROOT = os.path.normpath(PRJ_ROOT + "/rtos/platform")
        BSP_NAME = os.path.relpath(BSP_ROOT, os.path.join(PRJ_ROOT, "target")).replace("\\", "-").replace("/", "-")

    os.environ["BSP_ROOT"] = BPS_ROOT

    Export("PRJ_ROOT")
    Export("APP_ROOT")
    Export("BSP_ROOT")
    Export("RTT_ROOT")
    Export("SDK_ROOT")

    # 添加path目录
    sys.path += [
        os.path.join(RTT_ROOT, "tools"),
        os.path.join(PRJ_ROOT, "tools/scripts"),
    ]

    __options__["PRJ_ROOT"] = PRJ_ROOT
    __options__["APP_ROOT"] = APP_ROOT
    __options__["BSP_ROOT"] = BSP_ROOT
    __options__["RTT_ROOT"] = RTT_ROOT
    __options__["SDK_ROOT"] = SDK_ROOT
    __options__["PKG_ROOT"] = PKG_ROOT
    __options__["BSP_NAME"] = BSP_NAME
    __options__["RTT_ENV_ROOT"] = RTT_ENV_ROOT
    __options__["RTT_STD_ROOT"] = RTT_STD_ROOT

    return __options__


#######################################################################################
# 获取编译工具
#######################################################################################
def get_command_option():
    # 编译方式
    if GetOption("build-type"):
        BUILD_TYPE = GetOption("build-type")
    else:
        BUILD_TYPE = "release"

    # 工具链名称
    if GetOption("cross-tool"):
        CROSS_TOOL = GetOption("cross-tool")
    elif os.getenv("RTT_CC"):
        CROSS_TOOL = os.getenv("RTT_CC")
    else:
        CROSS_TOOL = "gcc"

    # 工具链路径、LINKER和TARGET文件
    if CROSS_TOOL == "gcc":
        EXEC_PATH = os.getenv("GCC_EXEC_PATH")
        TARGET_EXT = ".elf"
        PLATFORM = "gcc"
    elif CROSS_TOOL == "keil":
        EXEC_PATH = os.getenv("MDK_EXEC_PATH")
        TARGET_EXT = ".axf"
        PLATFORM = "armclang"  # "armcc"
    elif CROSS_TOOL == "keil5":
        EXEC_PATH = os.getenv("MDK_EXEC_PATH")
        TARGET_EXT = ".axf"
        PLATFORM = "armclang"
    elif CROSS_TOOL == "iar":
        EXEC_PATH = os.getenv("IAR_EXEC_PATH")
        TARGET_EXT = ".out"
        PLATFORM = "iar"
    else:
        print("ERROR: cross tool '%s' not support" % CROSS_TOOL)
        exit(-1)

    # 覆盖工具链路径
    if GetOption("exec-path"):
        EXEC_PATH = GetOption("exec-path")
    elif os.getenv("RTT_EXEC_PATH"):
        EXEC_PATH = os.getenv("RTT_EXEC_PATH")
    else:
        EXEC_PATH = "UNKNOWN"
    EXEC_PATH = os.path.normpath(EXEC_PATH)

    __options__["CROSS_TOOL"] = CROSS_TOOL
    __options__["EXEC_PATH"] = EXEC_PATH
    __options__["PLATFORM"] = PLATFORM
    __options__["BUILD_TYPE"] = BUILD_TYPE
    __options__["TARGET_EXT"] = TARGET_EXT

    return __options__


#######################################################################################
# 读取rtconfig.h
#######################################################################################
def get_rtconfig_option(file="rtconfig.h"):
    # 从rtconfig.h文件中读取配置信息
    def start_handling_includes(self, t=None):
        d = self.dispatch_table
        p = self.stack[-1] if self.stack else self.default_table
        for k in ("import", "include", "include_next", "define"):
            d[k] = p[k]

    def stop_handling_includes(self, t=None):
        d = self.dispatch_table
        d["import"] = self.do_nothing
        d["include"] = self.do_nothing
        d["include_next"] = self.do_nothing
        d["define"] = self.do_nothing

    PatchedPreProcessor = SCons.cpp.PreProcessor
    PatchedPreProcessor.start_handling_includes = start_handling_includes
    PatchedPreProcessor.stop_handling_includes = stop_handling_includes
    f = open(file, "r")
    contents = f.read()
    f.close()
    PreProcessor = PatchedPreProcessor()
    PreProcessor.process_contents(contents)
    __options__.update(PreProcessor.cpp_namespace)

    # TODO: 替换linker中的flash起始地址和大小
    # FLASH_START_ADRESS = 0x08000000
    # FLASH_TOTAL_SIZE = 1024 * 2048
    # if "APP_VECT_TAB_OFFSET" in options:
    #     FLASH_START_ADRESS += options["APP_VECT_TAB_OFFSET"] * 1024
    #     FLASH_TOTAL_SIZE -= options["APP_VECT_TAB_OFFSET"] * 1024
    # with open(LINKER_FILE, "r") as f:
    #     old = f.read()

    return __options__


def get_build_option(target=None):
    # 路径相关的选项
    get_project_option()

    # 从rtconfig.h文件读取配置
    get_rtconfig_option()

    # 从命令行获取配置信息
    get_command_option()

    # 板子名称
    if "BOARD_TYPE_NAME" in __options__:
        __options__["BOARD_NAME"] = __options__["BOARD_TYPE_NAME"].strip('"').lower()
    else:
        if target:
            __options__["BOARD_NAME"] = __options__["BSP_NAME"] + "-" + target
        else:
            __options__["BOARD_NAME"] = __options__["BSP_NAME"] + "-default"

    # 根据target名字确定生成文件名称
    __options__["TARGET_FILE"] = "build/" + __options__["BOARD_NAME"] + __options__["TARGET_EXT"]
    __options__["BIN_FILE"] = "build/" + __options__["BOARD_NAME"] + ".bin"
    __options__["HEX_FILE"] = "build/" + __options__["BOARD_NAME"] + ".hex"
    __options__["MAP_FILE"] = "build/" + __options__["BOARD_NAME"] + ".map"
    # linker文件会自动根据模板生成
    __options__["LINKER_FILE"] = "board/linker_scripts/link.lds"

    return (
        __options__,
        __options__["CROSS_TOOL"],
        __options__["EXEC_PATH"],
        __options__["PLATFORM"],
    )
