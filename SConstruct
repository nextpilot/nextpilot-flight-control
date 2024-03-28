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

# pdb.set_trace()

# Help(
#     """
# 	Usage:
# 	- scons -C bsps/vandor/borad target
#     - scons -C bsps/vandor/borad --target=mdk5/vs2012/vsc
#     - scons -C bsps/vandor/board --menuconfig

# 	""",
#     append=True,
# )


# 调试使用
print("ARGUMENTS:", ARGUMENTS)
print("COMMAND_LINE_TARGETS:", COMMAND_LINE_TARGETS)
print("BUILD_TARGET:", BUILD_TARGET)
print("DEFAULT_TARGETS:", DEFAULT_TARGETS)

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

AddOption(
    "--list-board",
    dest="list-board",
    action="store_true",
    default=False,
    help="list all avaliabe board",
)

AddOption(
    "--start-sitl",
    "--sitl",
    "--simulation",
    dest="start-sitl",
    action="store_true",
    default=False,
    help="start a software in loop simulation",
)

# SetOption函数设置命令行选项值，但优先级低于从命令行输入的
# SetOption("prefix", "/home/")

# GetOption函数获取命令行选项值
# print("--prefix=%s" % GetOption("prefix"))


####################################################
# Command-Line Construction Variables
#
# scons NAME=foo
# scons RELEASE=1 CC="cl.exe"
####################################################
# 通过命令行输入的变量都存在了ARGUMENTS字典里面
# https://www.scons.org/doc/HTML/scons-man.html#v-ARGUMENTS
# print("ARGUMENTS = ")
# print(ARGUMENTS)

# 获取命令行变量值
# debug = ARGUMENTS.get('debug', 0)
# debug = ARGUMENTS["debug"]

# 判断命令行是否设置某个值
# if "debug" in ARGUMENTS:
#     print(ARGUMENTS debug = %s" % ARGUMENTS["debug"])

# 循环穷举所有的输入白能量
# for arg in ARGUMENTS:
#     print("ARGUMENTS %s = %s" % (arg, ARGUMENTS[arg]))

# 通过命令行输入的变量同时也存在了ARGLIST数组里面
# https://www.scons.org/doc/HTML/scons-man.html#v-ARGLIST
# print("ARGLIST = ")
# print(ARGLIST)

# 循环穷举列出ARGLIST中变量
# for key, value in ARGLIST:
#     print("ARGLIST %s = %s" % (key, value))

# 根据ARGUMENTS创建变量列表
# vars = Variables(None, ARGUMENTS)
# 增加一个变量
# vars.Add('RELEASE', help='Set to 1 to build for release', default=0)
# Help(vars.GenerateHelpText(env))


####################################################
# Command-Line Targets
#
# scons install all
####################################################
# 通过命令行输入的target都保存在COMMAND_LINE_TARGETS数组里面
# print("COMMAND_LINE_TARGETS =")
# print(COMMAND_LINE_TARGETS)

# 将help目标转为显示帮助
# if "help" in COMMAND_LINE_TARGETS:
#     SetOption("help", True)
SetOption("help", True)
# 将clean目标转为清空编译
# if "clean" in COMMAND_LINE_TARGETS:
#     SetOption("clean", True)


####################################################
# Command(target, source, action, [key=val, ...])
# 对source执行action生成target
####################################################

# 可以使用Command函数来实现一些类似make中phony伪目标
# 由于是伪目标target是不存在的，因此伪目标每次必定执行

# 生成uorb消息
# Command("update_uorb", None, "")

# 生成param定义
# Command("update_param", None, "")

# 生成mavlink消息
# Command("update_mavlink", None, "")

# 软件在环仿真
# Command("sitl", None, "")


# 显示所有可用目标
# def list_target(target=None, source=None, env=None):
#     for root, dirs, files in os.walk("bsps"):
#         if "SConstruct" in files:
#             print(root)
#     return 0


# Command("list_target", None, list_target)


# 格式化代码
# def clang_format_code(target, source, env):
#     cmd = "clang-format -style=google -i foo.c"
#     os.system(cmd)


# scons format
# Command("format", None, clang_format_code)

# scons clang-tidy
# Command("clang-tidy", None, clang_format_code)


####################################################
# Alias(alias, [targets, [action]])
# 生成目标targets时执行action动作
####################################################


##################################################################
##################################################################


# Default("sitl")

# for vendor in os.listdir("target"):
# 	for product in os.listdir("target/" + vendor):
# 		print("target/"+ vendor +"/"+ product)
# 		Command("target#"+ vendor +"#"+ product, [], scons_target)

# Default("sitl/qemu-vexpress-a9")

# 设置编译默认线程数，如果命令行会覆盖当前值
num_jobs = multiprocessing.cpu_count()
SetOption("num_jobs", num_jobs)

# 必须通过-C指定BSP目录，无法在根目录下直接编译
# bsp_root = GetOption("directory")

# if BUILD_TARGETS:
#     print(BUILD_TARGETS)
#     if not bsp_root:
#         # SetOption("no_exec", 1)
#         print(
#             """
#     can NOT run scons in project root path! please change to bsp-dir then run 'scons'.
#     or use 'scons -C bsp-dir' to specific start path.
#         """
#         )
#     elif os.path.exist(bsp_root):
#         # SetOption("no_exec", 1)
#         print("bsp-dir {} not exist".format(bsp_root))


# 列出所有支持的板子
if GetOption("list-board"):
    print("-------------------------------------")
    print("all avaliabe board:")
    print("-------------------------------------")
    for root, dirs, files in os.walk("bsps"):
        if "SConstruct" in files:
            print(root)
    print("")
    exit(0)

# 启动sitl虚拟飞行仿真
if GetOption("start-sitl"):
    os.system("pushd bsps\sitl\qemu && scons && qemu.bat && popd")
    exit(0)
