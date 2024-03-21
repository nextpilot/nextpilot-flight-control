# -*- coding: utf-8 -*-

from SCons.Script import *

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

os.env["BSP_ROOT"] = BPS_ROOT

Export("PRJ_ROOT")
Export("APP_ROOT")
Export("BSP_ROOT")
Export("RTT_ROOT")
Export("SDK_ROOT")


#######################################################################################
# 获取编译工具
#######################################################################################
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


#######################################################################################
# 读取rtconfig.h
#######################################################################################


print("=======================================================================")
print("ENV_ROOT: " + RTT_ENV_ROOT)
print("STD_ROOT: " + RTT_STD_ROOT)
print("CROSS_TOOL: " + CROSS_TOOL)
print("EXEC_PATH: " + EXEC_PATH)
print("LINKER_FILE: " + LINKER_FILE)
print("TARGET_FILE: " + TARGET_FILE)
print("=======================================================================")
