# -*- coding: utf-8 -*-


def gen_rtconfig_header(target="default"):
    from menuconfig import mk_rtconfig
    import shutil

    if not target:
        return

    config_file = "config/%s.config" % target

    # 判断配置文件是否存在
    if not os.path.isfile(config_file):
        if target == "default":  # 如果default目标，但是配置文件不存在，则使用当前rtconfig.h
            print("Use current rtconfig.h")
            return
        else:  # 非default目标，且配置文件不存在，则报错
            print("ERROR: %s not found!!!" % config_file)
            exit(-1)
    else:
        # 重新生成rtconfig.h文件
        # os.system("menuconfig --silent --config %s > nul" % file)
        shutil.copyfile(config_file, ".config")
        mk_rtconfig(config_file)
        print("Info: regenerate rtconfig.h from %s" % config_file)


def init():
    pass


def build():
    if COMMAND_LINE_TARGETS:
        for target in COMMAND_LINE_TARGETS:
            build(target)
    else:
        build("default")


def build_internal(target="default"):
    import sys
    import rtconfig
    import builiding

    # 重新生成rtconfig.h文件
    gen_rtconfig_header(target)

    # 重新加载rtconfig.py文件
    if sys.version_info.major >= 3:
        import importlib

        importlib.reload(rtconfig)
    else:
        reload(rtconfig)


    # 生成配置文件

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
    objs += SConscript(os.path.join(rtconfig.SDK_ROOT, "SConscript"))

    # include application
    # building.AddDepend()
    objs += SConscript(os.path.join(rtconfig.PRJ_ROOT, "SConscript"))

    # make a building
    building.DoBuilding(rtconfig.TARGET_FILE, objs)

    #
    Alias(target, rtconfig.TARGET_FILE)
