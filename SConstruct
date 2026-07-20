# scons 命令行参数说明
#
# scons a b c -Q --prefix=/home/ INSTANCE=10
#
# 其中：
# a b c       是Targets，编译目标
# -q --prefix 是Options，编译选项
# INSTANCE    是Variables，编译变量


import os
import sys
import pdb
import multiprocessing
import logging


class ColoredFormatter(logging.Formatter):
    """自定义格式化器：ERROR 红色，WARNING 黄色"""

    RED = "\033[31m"
    YELLOW = "\033[33m"
    RESET = "\033[0m"

    def format(self, record):
        text = super().format(record)
        if record.levelno >= logging.ERROR:
            return f"{self.RED}{text}{self.RESET}"
        if record.levelno >= logging.WARNING:
            return f"{self.YELLOW}{text}{self.RESET}"
        return text


logger = logging.getLogger("nextpilot")
logger.setLevel(logging.DEBUG)
_handler = logging.StreamHandler()
_handler.setFormatter(ColoredFormatter("[%(levelname)s] %(message)s"))
logger.addHandler(_handler)


# 显示所有可用目标
def list_build_target(target=None, source=None, env=None):
    print("---------------------------------------------------")
    print("all avaliabe targets as follow, we can start a build by:")
    print("scons -C bsps/<vendor>/<board> <target>")
    print("---------------------------------------------------")
    for root, dirs, files in os.walk("bsps"):
        if "SConstruct" in files:
            print(root)


# 格式化代码
def clang_format_code(target, source, env):
    cmd = "clang-format -style=google -i foo.c"
    os.system(cmd)


####################################################
# 启用python调试
####################################################
# pdb.set_trace()
#
# Command-Line Construction Variables
# logging.debug("ARGUMENTS: %s", ARGUMENTS)
#
# Command-Line Targets
# logging.debug("COMMAND_LINE_TARGETS: %s", COMMAND_LINE_TARGETS)
#
# print("BUILD_TARGET:", BUILD_TARGET)
# logging.debug("DEFAULT_TARGETS: %s", DEFAULT_TARGETS)


####################################################
# 自定义帮助文档
####################################################
# Help(
#     """
# Usage:
#   - scons --list
#   - scons -C bsps/<vendor>/<board> target
#   - scons -C bsps/<vendor>/<board> --target=mdk5/vs2012/vsc
#   - scons -C bsps/<vendor>/<board> --menuconfig
#   - scons -C bsps/<vendor>/<board> --clean
#   - scons -C bsps/<vendor>/<board> --upload
# """,
#     append=True,
# )


####################################################
# Command-Line Options
#
# scons --prefix=/home/xxx
# scons -j 4
# scons -Q
#
# scons提供了一些常用的命令行选项，请参考
# https://www.scons.org/doc/HTML/scons-man.html#f-SetOption
####################################################
#
# SetOption函数设置命令行选项值，但优先级低于从命令行输入的
# SetOption("prefix", "/home/")
#
# GetOption函数获取命令行选项值
# print("--prefix=%s" % GetOption("prefix"))
#
# AddOption函数添加自定义命令行选项
# AddOption(
#     "--prefix",
#     dest="prefix",
#     type="string",
#     nargs=1,
#     action="store",
#     metavar="DIR",
#     help="installation prefix",
# )

# 设置编译默认线程数，如果命令行会覆盖当前值
SetOption("num_jobs", multiprocessing.cpu_count())

# scons --list，列出搜有可以用的board
AddOption(
    "--list",
    dest="list-target",
    action="store_true",
    default=False,
    help="list all avaliabe board",
)

# scons --sitl，启动软件在环仿真
AddOption(
    "--sitl",
    dest="start-sitl",
    action="store_true",
    default=False,
    help="start a software in loop simulation",
)

# scons --format，格式化代码
AddOption(
    "--format",
    dest="clang-format",
    action="store_true",
    default=False,
    help="use clang-format format code",
)

# scons --tidy，格式化代码
AddOption(
    "--tidy",
    dest="clang-tidy",
    action="store_true",
    default=False,
    help="use clang-tidy check code",
)

#
# for vendor in os.listdir("target"):
# 	for product in os.listdir("target/" + vendor):
# 		print("target/"+ vendor +"/"+ product)
# 		Command("target#"+ vendor +"#"+ product, [], scons_target)
#
# Default("sitl")
# Default("sitl/qemu-vexpress-a9")

####################################################
# Command-Line Construction Variables
#
# scons NAME=foo
# scons RELEASE=1 CC="cl.exe"
#
# 通过命令行输入的变量都存在了ARGUMENTS字典里面
# https://www.scons.org/doc/HTML/scons-man.html#v-ARGUMENTS
#
# 通过命令行输入的变量同时也存在了ARGLIST数组里面
# https://www.scons.org/doc/HTML/scons-man.html#v-ARGLIST
####################################################
#
# 获取命令行变量值
# debug = ARGUMENTS.get('debug', 0)
# debug = ARGUMENTS["debug"]
#
# 判断命令行是否设置某个值
# if "debug" in ARGUMENTS:
#     print(ARGUMENTS debug = %s" % ARGUMENTS["debug"])
#
# 循环穷举所有的输入变量
# for arg in ARGUMENTS:
#     print("ARGUMENTS %s = %s" % (arg, ARGUMENTS[arg]))
#
# 循环穷举列出ARGLIST中变量
# for key, value in ARGLIST:
#     print("ARGLIST %s = %s" % (key, value))
#
# 根据ARGUMENTS创建变量列表
# vars = Variables(None, ARGUMENTS)
# 增加一个变量
# vars.Add('RELEASE', help='Set to 1 to build for release', default=0)
# Help(vars.GenerateHelpText(env))


####################################################
# Command-Line Targets
#
# scons install all
#
# 通过命令行输入的target都保存在COMMAND_LINE_TARGETS数组里面
####################################################
#
# 将help目标转为显示帮助
if "help" in COMMAND_LINE_TARGETS:
    SetOption("help", True)

# 将clean目标转为清空编译
if "clean" in COMMAND_LINE_TARGETS:
    SetOption("clean", True)


####################################################
# Command(target, source, action, [key=val, ...])
# 对source执行action生成target
####################################################

# 可以使用Command函数来实现一些类似make中phony伪目标
# 由于是伪目标target是不存在的，因此伪目标每次必定执行

# 生成uorb消息
# Command("update_uorb", None, "")
#
# 生成param定义
# Command("update_param", None, "")
#
# 生成mavlink消息
# Command("update_mavlink", None, "")
#
# 软件在环仿真
# Command("sitl", None, "")
#
# Command("list", None, list_all_target)
#
# scons format
# Command("format", None, clang_format_code)
#
# scons clang-tidy
# Command("tidy", None, clang_tidy_code)


####################################################
# Alias(alias, [targets, [action]])
# 生成目标targets时执行action动作
####################################################


# 列出所有支持的板子
if GetOption("list-target"):
    list_build_target()
    exit(0)

# 启动sitl虚拟飞行仿真
if GetOption("start-sitl"):
    os.system("pushd bsps/sitl/qemu && scons && qemu.bat && popd")
    exit(0)


# 必须通过-C指定BSP目录，无法在根目录下直接编译
if not GetOption("help") and not GetOption("clean"):
    if not GetOption("directory"):
        logger.error("""Please change to bsp-dir or use 'scons -C bsp-dir' to specific start path.
        for example: scons -C bsps/px4/fmu-v6xrt""")
        list_build_target()
        exit(1)
    elif not os.path.exists(GetOption("directory")):
        logger.error("'%s' not a valid bsp path ", GetOption("directory"))
        list_build_target()
        exit(1)
