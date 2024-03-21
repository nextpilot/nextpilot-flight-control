# /******************************************************************
#  *      _   __             __   ____   _  __        __
#  *     / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_
#  *    /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/
#  *   / /|  //  __/_>  < / /_ / ____// // // /_/ // /_
#  *  /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/
#  *
#  * Copyright All Reserved © 2015-2023 NextPilot Development Team
#  ******************************************************************/


# 用于管理

import typer
import os
import pathlib
import subprocess
import shutil
from typing import List

from helper import *

import env.pkgs.main


rtt = typer.Typer(
    help="""
RT-Thread Command Line Tools\n
(1) menuconfig, configuration rt-thread according Kconfig\n
(2) genconfig, generate c .config file from c header file\n
(3) genheader, generate c header file from .config file\n
(4) package, manage rt-thread packages
"""
)

rtt.add_typer(env.pkgs.main.rtt, name="pkg")

__env_setting__ = {
    "env_root": None,
    "gcc_root": None,
    "bsp_root": None,
    "rtt_root": None,
}


@rtt.command()
def init():
    """
    initialize the rt-thread command line tools
    """

    if not os.path.isdir(__env_setting__["env_root"]):
        os.makedirs(__env_setting__["env_root"])

    # 安装所需的pip包

    print("init")


@rtt.command()
def setting():
    """
    configuration rt-thread setting
    """
    if not pathlib.Path(__env_setting__["env_root"]).exists():
        print("please run 'env init' for install env toolchain")
        typer.Exit()

    os.chdir(__env_setting__["env_root"])
    subprocess.run("python -m menuconfig Kconfig", shell=True)


@rtt.command()
def menuconfig(
    kconfig: str = typer.Argument(default="Kconfig", help="Top-level Kconfig file"),
    pkg_dir: str = typer.Option(default="pkg", help="rt-thread package directory"),
    rtt_dir: str = typer.Option(default="rt-thread", help="rt-thread directory"),
):
    """
    run menuconfig to config rt-thread
    """
    import operator

    # 判断Kconfig文件是否存在
    if not os.path.isfile(kconfig):
        typer.echo("Kconfig file not found")
        exit()

    bsp_root = os.path.dirname(os.path.abspath(kconfig))
    os.chdir(bsp_root)

    # 判断.config文件是否存在，不存在则根据rtconfig.h自动生成
    config_file = os.path.join(bsp_root, ".config")
    header_file = os.path.join(bsp_root, "rtconfig.h")
    if os.path.isfile(config_file):
        pass
    elif os.path.isfile(header_file):
        gen_config_from_header(header_file, config_file)
    else:
        print("Can't find %s and %s" % (config_file, header_file))
        typer.Exit()
        exit()

    # 执行menuconfig
    env = os.environ.copy()
    env["PKGS_DIR"] = __env_setting__["pkg_root"]
    env["PRJ_DIR"] = "../../../"
    env["RTT_DIR"] = "../../../rtos/rt-thread"
    env["BSP_DIR"] = bsp_root
    env["SDK_DIR"] = "../../../rtos/platform/stm32"
    subprocess.run("python -m menuconfig", shell=True, env=env)

    # 生成rtconfig.h文件
    if os.path.isfile(".config.old"):
        if not operator.eq(get_file_md5(".config"), get_file_md5(".config.old")):
            gen_header_from_config(config_file, header_file)


@rtt.command()
def genheader(
    config_file: str = typer.Option(
        default=".config", help="configuration file used to generate c header"
    ),
    header_file: str = typer.Option(
        default="rtconfig.h", help="the generated c header file"
    ),
):
    """
    Generates a header file with #defines from the configuration, matching the
    format of include/generated/autoconf.h in the Linux kernel.
    """

    if os.path.isfile(config_file):
        gen_header_from_config(config_file, header_file)
        print("%s generated from %s" % (header_file, config_file))
    else:
        print("%s generated fail" % header_file)


@rtt.command()
def genconfig(
    header_file: str = typer.Option(
        default="rtconfig.h", help="the generated c header file"
    ),
    config_file: str = typer.Option(
        default=".config", help="configuration file used to generate c header"
    ),
):
    """
    Generates a configuration file from the header file with #defines matching the
    format of include/generated/autoconf.h in the Linux kernel.
    """

    if os.path.isfile(header_file):
        gen_config_from_header(header_file, config_file)
        print("%s generated from %s" % (config_file, header_file))
    else:
        print("%s generated fail" % config_file)


@rtt.command()
def package():
    pass


@rtt.command()
def build():
    import sys

    print(sys.argv)
    # subprocess.run("python -m scons")


@rtt.callback(invoke_without_command=True)
def main(
    env_root: str = typer.Option(default=".env", envvar="ENV_ROOT"),
    gcc_root: str = typer.Option(default="", envvar="GCC_ROOT"),
    bsp_root: str = typer.Option(default=os.getcwd()),
    rtt_root: str = typer.Option(default=None, envvar="RTT_ROOT"),
):
    __env_setting__["env_root"] = env_root
    __env_setting__["gcc_root"] = gcc_root
    __env_setting__["pkg_root"] = r"C:\env-windows-v1.3.5\packages"
    __env_setting__["bsp_root"] = bsp_root

    if rtt_root is None:
        rtt_root = os.path.abspath(os.path.join(bsp_root, "../../../rtos/rt-thread"))

    __env_setting__["rtt_root"] = rtt_root

    # print(__env_setting__)


if __name__ == "__main__":
    rtt()
