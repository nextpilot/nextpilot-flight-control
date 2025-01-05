# -*- coding: utf-8 -*-

# 根据scons命令行，配置编译环境和参数

import os
import sys
import subprocess
import platform
import uuid
from SCons.Script import *

# option是否已经初始化
__scons_inited__ = False

#
__options__ = {}


def add_scons_option():
    import multiprocessing

    global __scons_inited__

    if __scons_inited__:
        return

    SetOption("num_jobs", multiprocessing.cpu_count())

    # AddOpiton(
    #     "--upgrade",
    #     dest="upgrade",
    #     action="store_true",
    #     help="upgrade firmwre using nextpilot bootloader",
    # )

    # AddOption(
    #     "--gen-c-header",
    #     dest="gen-c-header",
    #     action="store_true",
    #     default=False,
    #     help="generate rtconfig.h from config/xxx.config file",
    # )

    # AddOption(
    #     "--menuconfig",
    #     dest="menuconfig",
    #     action="store_true",
    #     default=False,
    #     help="start kconfig tool to configurate nextpilot",
    # )

    AddOption(
        "--deploy",
        dest="deploy-path",
        default=None,
        type="string",
        help="deploy bsp to a project",
    )

    AddOption(
        "--download",
        dest="download",
        default=None,
        choices=["stlink", "jlink", "bootloader"],
        help="start download program",
    )

    AddOption(
        "--debugging",
        dest="debugging",
        choices=["stlink", "jlink"],
        help="start debugging program",
    )

    # 编译类型release/debug
    AddOption(
        "--build-type",
        dest="build-type",
        choices=["release", "debug"],
        nargs=1,
        default="release",
        help="build debug or release",
    )

    # 进行覆盖率测试
    AddOption(
        "--coverage",
        dest="coverage",
        action="store_true",
        default=False,
        help="add coverage test",
    )

    # 编译工具gcc/armcc/armclang/iccarm
    AddOption(
        "--cross-tool",
        dest="cross-tool",
        choices=["gcc", "armcc", "armclang", "iccarm"],
        nargs=1,
        default=None,
        help="cross tool name",
    )

    # 编译器的路径
    # AddOption(
    #     "--exec-path",
    #     dest="exec-path",
    #     nargs=1,
    #     default=None,
    #     help="cross tool path",
    # )

    __scons_inited__ = True


def update_command_option():

    if GetOption("cross-tool"):
        os.environ["RTT_CC"] = GetOption("cross-tool")

    # if GetOption("exec-path"):
    #     os.environ["RTT_EXEC_PATH"] = GetOption("exec-path")

    if GetOption("build-type"):
        os.environ["RTT_BUILD_TYPE"] = GetOption("build-type")


def update_toolchain_option():

    # 获取ENV路径
    if os.getenv("ENV_ROOT"):
        RTT_ENV_ROOT = os.path.normpath(os.getenv("ENV_ROOT")).replace("\\", "/")
        os.environ["ENV_ROOT"] = RTT_ENV_ROOT
    else:
        RTT_ENV_ROOT = "UNKOWN"

    if r"platform/env_released/env" in RTT_ENV_ROOT:
        RTT_STD_ROOT = RTT_ENV_ROOT.replace(r"/platform/env_released/env", "")
    else:
        RTT_STD_ROOT = os.getenv("RTT_STD_ROOT", "UNKOWN")

    if os.getenv("QEMU_HOME"):
        os.environ["QEMU_HOME"] = os.path.normpath(os.getenv("QEMU_HOME")).replace("\\", "/")
    else:
        if RTT_ENV_ROOT != "UNKOWN":
            if os.path.exists(os.path.join(RTT_ENV_ROOT, r"tools/qemu/qemu64")):
                os.environ["QEMU_HOME"] = os.path.join(RTT_ENV_ROOT, r"tools/qemu/qemu64").replace("\\", "/")
            else:
                os.environ["QEMU_HOME"] = os.path.join(RTT_ENV_ROOT, r"tools/qemu/qemu32").replace("\\", "/")
        else:
            os.environ["QEMU_HOME"] = "UNKOWN"

    if os.getenv("GCC_EXEC_PATH"):
        os.environ["GCC_EXEC_PATH"] = os.path.normpath(os.getenv("GCC_EXEC_PATH")).replace("\\", "/")
    else:
        if RTT_ENV_ROOT != "UNKOWN":
            os.environ["GCC_EXEC_PATH"] = os.path.join(RTT_ENV_ROOT, r"tools/gnu_gcc/arm_gcc/mingw/bin").replace("\\", "/")
        else:
            os.environ["GCC_EXEC_PATH"] = "UNKOWN"

    if os.getenv("RTT_EXEC_PATH"):
        os.environ["RTT_EXEC_PATH"] = os.path.normpath(os.getenv("RTT_EXEC_PATH").replace("\\", "/"))
    else:
        pass


def update_project_option(target="default"):

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
        BSP_NAME = os.path.relpath(BSP_ROOT, os.path.join(BSP_ROOT, r"../../")).replace("\\", "-").replace("/", "-")

    # 板子厂家
    board_vendor = os.path.basename(os.path.normpath(os.path.join(BSP_ROOT, "../")))
    # 板子版本
    board_version = os.path.basename(BSP_ROOT)
    # 板子名称
    if target:
        os.environ["RTT_BOARD_NAME"] = board_vendor + "_" + board_version + "_" + target
    else:
        os.environ["RTT_BOARD_NAME"] = board_vendor + "_" + board_version + "_default"

    Export("PRJ_ROOT")
    Export("APP_ROOT")
    Export("BSP_ROOT")
    Export("RTT_ROOT")
    Export("SDK_ROOT")

    os.environ["PRJ_ROOT"] = PRJ_ROOT
    os.environ["PRJ_DIR"] = PRJ_ROOT
    os.environ["RTT_ROOT"] = RTT_ROOT.replace("\\", "/")
    os.environ["RTT_DIR"] = RTT_ROOT
    os.environ["SDK_DIR"] = SDK_ROOT
    os.environ["BSP_DIR"] = BSP_ROOT
    os.environ["PKGS_DIR"] = os.path.join(PRJ_ROOT, "rtos")

    # 添加path目录
    sys.path += [
        os.path.join(RTT_ROOT, "tools"),
        os.path.join(PRJ_ROOT, "tools/scripts"),
    ]


def gen_rtconfig_header(target=None):
    from menukconfig import mk_rtconfig
    import shutil

    if not target:
        return

    file = "config/%s.config" % target

    # 判断配置文件是否存在
    if not os.path.isfile(file):
        if target == "default":  # 如果default目标，但是配置文件不存在，则使用当前rtconfig.h
            return
        else:  # 非default目标，且配置文件不存在，则报错
            print("ERROR: %s not found!!!" % file)
            exit(-1)
    else:
        # 重新生成rtconfig.h文件
        # os.system("menuconfig --silent --config %s > nul" % file)
        shutil.copyfile(file, ".config")
        mk_rtconfig(file)
        print("Info: regenerate rtconfig.h from %s" % file)


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
    option = PreProcessor.process_contents(contents)

    return option


def update_rtconfig_option():
    pass


def update_build_option(target=None):

    # 路径相关的选项
    update_project_option(target)

    # 从更新工具配置信息
    update_toolchain_option()

    # 从rtconfig.h文件读取配置
    update_rtconfig_option()


def reload_rtconfig_module():
    import rtconfig

    if sys.version_info.major >= 3:
        import importlib

        importlib.reload(rtconfig)
    else:
        reload(rtconfig)

    Export("rtconfig")


def gen_vscode_setting():
    import vscode_helper as vscode

    BSP_ROOT = Dir("#").abspath

    if "qemu" in BSP_ROOT:
        os.makedirs(os.path.join(BSP_ROOT, ".vscode"), exist_ok=True)
        launch_file = os.path.join(BSP_ROOT, ".vscode", "launch.json")
        vscode.add_qemu_debug_launch(launch_file)
        vscode.add_scons_debug_lanuch(launch_file)
        setting_file = os.path.join(BSP_ROOT, ".vscode", "settings.json")
        vscode.add_rtt_studio_setting(setting_file)
    else:
        pass


# set spawn
def ourspawn(sh, escape, cmd, args, e):
    filename = str(uuid.uuid4())
    newargs = " ".join(args[1:])
    cmdline = cmd + " " + newargs
    if len(cmdline) > 16 * 1024:
        f = open(filename, "w")
        f.write(" ".join(args[1:]).replace("\\", "/"))
        f.close()
        # exec
        cmdline = cmd + " @" + filename
    proc = subprocess.Popen(
        cmdline,
        stdin=subprocess.PIPE,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        shell=False,
        env=e,
    )
    data, err = proc.communicate()
    rv = proc.wait()

    def res_output(_output, _s):
        if len(_s):
            if isinstance(_s, str):
                _output(_s)
            elif isinstance(_s, bytes):
                _output(str(_s, "UTF-8"))
            else:
                _output(str(_s))

    res_output(sys.stderr.write, err)
    res_output(sys.stdout.write, data)
    if os.path.isfile(filename):
        os.remove(filename)
    return rv


def init():
    # 添加自定义option
    add_scons_option()

    # 获取命令选项
    update_command_option()

    # 获取编译选型
    update_build_option()


def auto_build():
    if COMMAND_LINE_TARGETS:
        for target in COMMAND_LINE_TARGETS:
            build(target)
    else:
        build()


def build(target=None):
    import rtconfig
    import building

    # 根据target生成rtconfig.h文件
    gen_rtconfig_header(target)

    # 更新编译选项
    update_build_option()

    # 重新加载rtconfig.py文件
    reload_rtconfig_module()

    print("=======================================================================")
    # print("ENV_ROOT: " + rtconfig.ENV_ROOT)
    # print("STD_ROOT: " + __options__["RTT_STD_ROOT"])
    print("BOARD_NAME:  " + rtconfig.BOARD_NAME)
    print("CROSS_TOOL:  " + rtconfig.CROSS_TOOL)
    print("EXEC_PATH:   " + rtconfig.EXEC_PATH)
    print("LINK_SCRIPT: " + rtconfig.LINK_SCRIPT)
    print("TARGET_FILE: " + rtconfig.TARGET_FILE)
    print("=======================================================================")

    # 生成调试文件
    gen_vscode_setting()

    # 获取编译环境
    env = rtconfig.get_build_env()

    # prepare building environment, include bsp and rtos
    objs = building.PrepareBuilding(env, rtconfig.RTT_ROOT, has_libcpu=False)

    # windows平台防止命令行太长报错
    if platform.system() == "Windows":
        env["SPAWN"] = ourspawn
    else:
        pass

    # include libraries
    # if os.path.exists(os.path.join(__options__["SDK_ROOT"], "SConscript")):
    #    objs += SConscript(os.path.join(__options__["SDK_ROOT"], "SConscript"))

    # include application
    if os.path.exists(os.path.join(os.environ["PRJ_ROOT"], "SConscript")):
        objs += SConscript(os.path.join(os.environ["PRJ_ROOT"], "SConscript"))

    # make a building
    building.DoBuilding(rtconfig.TARGET_FILE, objs)

    #
    if target:
        Alias(target, rtconfig.TARGET_FILE)
    else:
        pass
