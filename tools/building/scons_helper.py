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
    global __scons_inited__

    if __scons_inited__:
        return

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


def get_project_option():

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
        BSP_NAME = os.path.relpath(BSP_ROOT, os.path.join(PRJ_ROOT, "board")).replace("\\", "-").replace("/", "-")

    # 获取ENV路径
    if os.getenv("ENV_ROOT"):
        RTT_ENV_ROOT = os.path.abspath(os.getenv("ENV_ROOT")).replace("\\", "/")
    else:
        print("ERROR: please run scons in env.exe console")
        exit(-1)

    if r"platform/env_released/env" in RTT_ENV_ROOT:
        RTT_STD_ROOT = RTT_ENV_ROOT.replace(r"/platform/env_released/env", "")
    else:
        RTT_STD_ROOT = os.getenv("RTT_STD_ROOT", "UNKOWN")

    Export("PRJ_ROOT")
    Export("APP_ROOT")
    Export("BSP_ROOT")
    Export("RTT_ROOT")
    Export("SDK_ROOT")

    __options__["PRJ_ROOT"] = PRJ_ROOT
    __options__["APP_ROOT"] = APP_ROOT
    __options__["BSP_ROOT"] = BSP_ROOT
    __options__["RTT_ROOT"] = RTT_ROOT
    __options__["SDK_ROOT"] = SDK_ROOT
    # __options__["PKG_ROOT"] = PKG_ROOT
    __options__["BSP_NAME"] = BSP_NAME
    __options__["RTT_ENV_ROOT"] = RTT_ENV_ROOT
    __options__["RTT_STD_ROOT"] = RTT_STD_ROOT

    # 添加path目录
    sys.path += [
        os.path.join(RTT_ROOT, "tools"),
        os.path.join(PRJ_ROOT, "tools/scripts"),
    ]

    os.environ["RTT_DIR"] = RTT_ROOT
    os.environ["PRJ_DIR"] = PRJ_ROOT
    os.environ["SDK_DIR"] = SDK_ROOT
    os.environ["BSP_DIR"] = BSP_ROOT

    return __options__


def gen_rtconfig_header(target="default"):
    from menuconfig import mk_rtconfig
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
    PreProcessor.process_contents(contents)
    __options__.update(PreProcessor.cpp_namespace)

    # TODO: 替换linker中的flash起始地址和大小
    # FLASH_START_ADRESS = 0x08000000
    # FLASH_TOTAL_SIZE = 1024 * 2048
    # if "APP_VECT_TAB_OFFSET" in __options__:
    #     FLASH_START_ADRESS += __options__["APP_VECT_TAB_OFFSET"] * 1024
    #     FLASH_TOTAL_SIZE -= __options__["APP_VECT_TAB_OFFSET"] * 1024
    # with open(LINKER_FILE, "r") as f:
    #     old = f.read()

    return __options__


def get_command_option():

    if not __scons_inited__:
        BUILD_TYPE = "release"
        CROSS_TOOL = "gcc"
        TARGET_EXT = ".elf"
        PLATFORM = "gcc"
    else:

        # 编译方式
        if GetOption("build-type"):
            BUILD_TYPE = GetOption("build-type")
        else:
            BUILD_TYPE = "release"

        # 根据命令行和环境变量，确定工具链名称
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

        # 根据命令行和环境变量，覆盖工具链路径
        # if GetOption("exec-path"):
        #     EXEC_PATH = GetOption("exec-path")
        # elif
        if os.getenv("RTT_EXEC_PATH"):
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


def reload_rtconfig_module():
    import rtconfig

    if sys.version_info.major >= 3:
        import importlib

        importlib.reload(rtconfig)
    else:
        reload(rtconfig)

    Export("rtconfig")


def deploy_bsp_project():
    import fnmatch
    import shutil
    import rtconfig

    def ignore_patterns(*patterns):
        """Function that can be used as copytree() ignore parameter.

        Patterns is a sequence of glob-style patterns
        that are used to exclude files"""

        def _ignore_patterns(path, names):
            ignored_names = []
            for pattern in patterns:
                ignored_names.extend(fnmatch.filter(names, pattern))
            return set(ignored_names)

        return _ignore_patterns

    def do_copy_file(src, dst):
        # check source file
        if not os.path.exists(src):
            return

        path = os.path.dirname(dst)
        # mkdir if path not exist
        if not os.path.exists(path):
            os.makedirs(path)

        shutil.copy2(src, dst)

    def do_copy_folder(src_dir, dst_dir, ignore=None):
        # check source directory
        if not os.path.exists(src_dir):
            return
        try:
            if os.path.exists(dst_dir):
                shutil.rmtree(dst_dir)
        except:
            print("Deletes folder: %s failed." % dst_dir)
            return
        shutil.copytree(src_dir, dst_dir, ignore=ignore)

    #
    dist_root = "dist/cetcs-fcs"

    # common
    copy_list = [
        "apps",
        "tools",
        ".gitignore",
        "SConscript",
        "SConstruct",
    ]
    # rtos
    copy_list += [
        "rtos/rt-thread/components",
        "rtos/rt-thread/include",
        "rtos/rt-thread/libcpu",
        "rtos/rt-thread/src",
        "rtos/rt-thread/tools",
        "rtos/rt-thread/Kconfig",
        "rtos/rt-thread/README.md",
    ]
    # stm32 lib
    if hasattr(rtconfig, "CHIP_SERIES"):
        copy_list += [
            "rtos/SConscript",
            "rtos/platform/SConscript",
            "rtos/platform/stm32/HAL_Drivers",
            "rtos/platform/stm32/" + rtconfig.CHIP_SERIES + "xx_HAL",
            "rtos/platform/stm32/Kconfig",
            "rtos/platform/stm32/SConscript",
        ]

    for item in copy_list:
        src = os.path.join(__options__["PRJ_ROOT"], item)
        dst = os.path.join(dist_root, item)
        if os.path.isdir(src):
            do_copy_folder(src, dst)
        else:
            do_copy_file(src, dst)

    # target
    do_copy_folder(
        __options__["BSP_ROOT"],
        os.path.join(dist_root, "bsp"),
        ignore_patterns(
            "build",
            "dist",
            "DebugConfig",
            ".config" "*.scvd" "*.pyc",
            "*.old",
            "*.map",
            "*.bin",
            "*.elf",
            "*.axf",
            "*.o",
            ".sconsign.dblite",
            "cconfig.h",
            "rtconfig.h",
        ),
    )
    # rtconfig
    do_copy_file(os.path.join(__options__["BSP_ROOT"], "rtconfig.h"), dist_root)


def gen_setting_file():
    import multiprocessing
    import jinja2
    import rtconfig

    # 模板中所需的配置
    __options__["CPU_COUNT"] = str(multiprocessing.cpu_count())

    if os.path.exists(__options__["RTT_ENV_ROOT"] + r"/tools/qemu/qemu64"):
        __options__["QEMU_ROOT"] = __options__["RTT_ENV_ROOT"] + r"/tools/qemu/qemu64"
    else:
        __options__["QEMU_ROOT"] = __options__["RTT_ENV_ROOT"] + r"/tools/qemu/qemu32"

    # 模板文件路径
    current_path = os.path.dirname(os.path.abspath(__file__))
    if "qemu" in __options__["BSP_ROOT"]:
        template_path = os.path.join(current_path, "template/qemu")
    else:
        template_path = os.path.join(current_path, "template/stm32")

    out_path_dict = {
        "launch.json": os.path.join(__options__["BSP_ROOT"], ".vscode"),
        "settings.json": os.path.join(__options__["BSP_ROOT"], ".vscode"),
        ".rtthread_jlink_download": __options__["BSP_ROOT"],
        ".rtthread_stlink_download": __options__["BSP_ROOT"],
        "link.icf": os.path.join(__options__["BSP_ROOT"], "build/linker_scripts"),
        "link.lds": os.path.join(__options__["BSP_ROOT"], "build/linker_scripts"),
        "link.sct": os.path.join(__options__["BSP_ROOT"], "build/linker_scripts"),
    }

    # 创建jinja2环境
    env = jinja2.Environment(loader=jinja2.FileSystemLoader(template_path, "utf-8"))

    for template_file in os.listdir(template_path):
        # 确定文件保存路径
        out_name = template_file.replace(".jinja", "")
        out_path = out_path_dict[out_name]
        if not os.path.exists(out_path):
            os.makedirs(out_path)
        out_file = os.path.join(out_path, out_name)

        # 读取模板文件
        template = env.get_template(template_file)
        # 生成渲染后的文件
        result = template.render(__options__)
        # 保存文件
        with open(out_file, "wb") as f_out:
            f_out.write(result.encode("utf-8"))


def start_stlink_debug():
    pass


def start_stlink_upload():
    pass


def start_jlink_upload():
    import rtconfig

    stm32_programmer_cli = "C:/RT-ThreadStudio/repo/Extract/Debugger_Support_Packages/STMicroelectronics/ST-LINK_Debugger/1.6.0/tools/bin/STM32_Programmer_CLI"
    st_link_cmd = " -c port=SWD  mode=NORMAL -d {0} 0x08000000 -s".format(rtconfig.BIN_FILE)
    os.system(stm32_programmer_cli + st_link_cmd)


def add_post_action():
    pass


def get_chip_info():
    import rtconfig

    arch = "arm"
    if "CONFIG_SOC_SERIES_STM32F7" in __options__:
        cpu = "cortex-m7"

    return (arch, cpu)


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

    # 添加工程目录
    get_build_option()


def auto_build():
    if COMMAND_LINE_TARGETS:
        for target in COMMAND_LINE_TARGETS:
            build(target)
    else:
        build("default")


def build(target="default"):
    import rtconfig
    import building

    # 根据target生成rtconfig.h文件
    gen_rtconfig_header(target)

    # 需要重新加载rtconfig.py文件
    reload_rtconfig_module()

    print("=======================================================================")
    print("ENV_ROOT: " + __options__["RTT_ENV_ROOT"])
    print("STD_ROOT: " + __options__["RTT_STD_ROOT"])
    print("CROSS_TOOL: " + __options__["CROSS_TOOL"])
    print("EXEC_PATH: " + __options__["EXEC_PATH"])
    print("LINKER_FILE: " + __options__["LINKER_FILE"])
    print("TARGET_FILE: " + __options__["TARGET_FILE"])
    print("=======================================================================")

    # 生成调试文件
    gen_setting_file()

    # 获取编译环境
    env = rtconfig.get_build_env()

    # prepare building environment, include bsp and rtos
    objs = building.PrepareBuilding(env, __options__["RTT_ROOT"], has_libcpu=False)

    # windows平台防止命令行太长报错
    if platform.system() == "Windows":
        env["SPAWN"] = ourspawn
    else:
        pass

    # include libraries
    if os.path.exists(os.path.join(__options__["SDK_ROOT"], "SConscript")):
        objs += SConscript(os.path.join(__options__["SDK_ROOT"], "SConscript"))

    # include application
    # building.AddDepend()
    if os.path.exists(os.path.join(__options__["PRJ_ROOT"], "SConscript")):
        objs += SConscript(os.path.join(__options__["PRJ_ROOT"], "SConscript"))

    # make a building
    building.DoBuilding(__options__["TARGET_FILE"], objs)

    #
    Alias(target, __options__["TARGET_FILE"])

    # deploy_bsp_project()
