# -*- coding: utf-8 -*-

from SCons.Script import *


def get_file_md5(file):
    import hashlib

    md5 = hashlib.new("md5")
    with open(file, "r") as fp:
        md5.update(fp.read().encode("utf8"))
        fp_md5 = md5.hexdigest()
        return fp_md5


def is_pkg_special_config(config_str):
    """judge if it's CONFIG_PKG_XX_PATH or CONFIG_PKG_XX_VER"""

    if type(config_str) == type("a"):
        if config_str.startswith("PKG_") and (
            config_str.endswith("_PATH") or config_str.endswith("_VER")
        ):
            return True
    return False


# 从rtconfig.h文件中读取配置信息
def get_option_from_header(file="rtconfig.h"):

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
    options = PreProcessor.cpp_namespace

    # TODO: 替换linker中的flash起始地址和大小
    # FLASH_START_ADRESS = 0x08000000
    # FLASH_TOTAL_SIZE = 1024 * 2048
    # if "APP_VECT_TAB_OFFSET" in options:
    #     FLASH_START_ADRESS += options["APP_VECT_TAB_OFFSET"] * 1024
    #     FLASH_TOTAL_SIZE -= options["APP_VECT_TAB_OFFSET"] * 1024
    # with open(LINKER_FILE, "r") as f:
    #     old = f.read()

    return options


def gen_config_from_header(header_file="rtconfig.h", config_file=".config"):

    from SCons.Script import SCons

    PreProcessor = SCons.cpp.PreProcessor()

    try:
        f = open(header_file, "r")
        contents = f.read()
        f.close()
    except:
        print("Open %s file failed." % header_file)

    PreProcessor.process_contents(contents)
    options = PreProcessor.cpp_namespace

    try:
        f = open(config_file, "w")
        for opt, value in options.items():
            if type(value) == type(1):
                f.write("CONFIG_%s=%d\n" % (opt, value))

            if type(value) == type("") and value == "":
                f.write("CONFIG_%s=y\n" % opt)
            elif type(value) == type("str"):
                f.write("CONFIG_%s=%s\n" % (opt, value))

        # print("Generate %s done!" % config_file)
        f.close()
    except:
        print("Generate %s file failed." % config_file)


# 根据.config文件生成rtconfig.h文件
def gen_header_from_config(config_file=".config", header_file="rtconfig.h"):
    import re
    from pathlib import Path

    try:
        config = open(config_file, "r")
    except:
        print("open config: %s failed" % config_file)
        return

    rtconfig = open(header_file, "w")
    rtconfig.write("#ifndef RT_CONFIG_H__\n")
    rtconfig.write("#define RT_CONFIG_H__\n\n")

    empty_line = 1

    for line in config:
        line = line.lstrip(" ").replace("\n", "").replace("\r", "")

        if len(line) == 0:
            continue

        if line[0] == "#":
            if len(line) == 1:
                if empty_line:
                    continue

                rtconfig.write("\n")
                empty_line = 1
                continue

            if line.startswith("# CONFIG_"):
                line = " " + line[9:]
            else:
                line = line[1:]
                rtconfig.write("/*%s */\n" % line)

            empty_line = 0
        else:
            empty_line = 0
            setting = line.split("=")
            if len(setting) >= 2:
                if setting[0].startswith("CONFIG_"):
                    setting[0] = setting[0][7:]

                # remove CONFIG_PKG_XX_PATH or CONFIG_PKG_XX_VER
                if is_pkg_special_config(setting[0]):
                    continue

                if setting[1] == "y":
                    rtconfig.write("#define %s\n" % setting[0])
                else:
                    rtconfig.write(
                        "#define %s %s\n"
                        % (setting[0], re.findall(r"^.*?=(.*)$", line)[0])
                    )

    if Path("rtconfig_project.h").is_file():
        rtconfig.write('#include "rtconfig_project.h"\n')

    rtconfig.write("\n")
    rtconfig.write("#endif\n")
    rtconfig.close()
