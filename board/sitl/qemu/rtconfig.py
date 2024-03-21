# -*- coding: utf-8 -*-

import os
import uuid
from scons_helper import *


def get_mac_address():
    mac = uuid.UUID(int=uuid.getnode()).hex[-12:]
    return "#define AUTOMAC".join([str(int(e / 2) + 1) + "  0x" + mac[e : e + 2] + "\n" for e in range(5, 11, 2)])


header = """
#ifndef __MAC_AUTO_GENERATE_H__
#define __MAC_AUTO_GENERATE_H__

/* Automatically generated file; DO NOT EDIT. */
/* mac configure file for RT-Thread qemu */

#define AUTOMAC0  0x52
#define AUTOMAC1  0x54
#define AUTOMAC2  0x00
#define AUTOMAC"""

end = """
#endif
"""

automac_h_fn = os.path.join(os.path.dirname(__file__), "drivers", "automac.h")
with open(automac_h_fn, "w") as f:
    f.write(header + get_mac_address() + end)


# toolchains options
ARCH = "arm"
CPU = "cortex-a"

OPTIONS, CROSS_TOOL, EXEC_PATH, PLATFORM = get_build_option()

# 强制覆盖了
OPTIONS["LINKER_FILE"] = "link.lds"


if PLATFORM == "gcc":
    # toolchains
    PREFIX = "arm-none-eabi-"
    CC = PREFIX + "gcc"
    CXX = PREFIX + "g++"
    AS = PREFIX + "gcc"
    AR = PREFIX + "ar"
    LINK = PREFIX + "gcc"
    TARGET_EXT = "elf"
    SIZE = PREFIX + "size"
    OBJDUMP = PREFIX + "objdump"
    OBJCPY = PREFIX + "objcopy"
    STRIP = PREFIX + "strip"

    DEVICE = " -march=armv7-a -marm -msoft-float"
    CFLAGS = (
        DEVICE
        + " -Wall -Wno-address-of-packed-member -Wno-missing-braces -Wno-unused-variable -Wno-unused-function -Wno-unused-const-variable"
    )
    AFLAGS = " -c" + DEVICE + " -x assembler-with-cpp -D__ASSEMBLY__ -I."
    LINK_SCRIPT = "link.lds"
    LFLAGS = DEVICE + " -nostartfiles -Wl,--gc-sections,-Map={MAP_FILE},-cref,-u,system_vectors -T {LINKER_FILE}".format(
        **OPTIONS
    )

    CPATH = ""
    LPATH = ""

    # generate debug info in all cases
    AFLAGS += " -gdwarf-2"
    CFLAGS += " -g -gdwarf-2"

    if OPTIONS["BUILD_TYPE"] == "debug":
        CFLAGS += " -O0"
    else:
        CFLAGS += " -O0"

    CXXFLAGS = CFLAGS + " -Woverloaded-virtual -fno-exceptions -fno-rtti"

    CFLAGS += CFLAGS + " -Werror-implicit-function-declaration"

    M_CFLAGS = CFLAGS + " -mlong-calls -fPIC "
    M_CXXFLAGS = CXXFLAGS + " -mlong-calls -fPIC"
    M_LFLAGS = (
        DEVICE + CXXFLAGS + " -Wl,--gc-sections,-z,max-page-size=0x4" + " -shared -fPIC -nostartfiles -nostdlib -static-libgcc"
    )
    M_POST_ACTION = STRIP + " -R .hash $TARGET\n" + SIZE + " $TARGET \n"

    POST_ACTION = OBJCPY + " -O binary $TARGET {BIN_FILE}\n".format(**OPTIONS) + SIZE + " $TARGET \n"
else:
    pass


def get_build_env():
    DefaultEnvironment(tools=[])
    env = Environment(
        tools=["mingw"],
        AS=AS,
        ASFLAGS=AFLAGS,
        CC=CC,
        CFLAGS=CFLAGS,
        CXX=CXX,
        CXXFLAGS=CXXFLAGS,
        AR=AR,
        ARFLAGS="-rc",
        LINK=LINK,
        LINKFLAGS=LFLAGS,
    )
    env.PrependENVPath("PATH", EXEC_PATH)
    env["ASCOM"] = env["ASPPCOM"]
    return env
