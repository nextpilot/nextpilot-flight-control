import os
import uuid


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
CROSS_TOOL = "gcc"
PLATFORM = "gcc"
EXEC_PATH = os.getenv("RTT_EXEC_PATH") or r"/usr/bin"
BUILD = "debug"
RTT_ROOT = os.getenv("RTT_ROOT", "../../../rtos/rt-thread")
BOARD_NAME = "sitl-qemu-default"
LINK_SCRIPT = "link.lds"

if PLATFORM == "gcc":
    PREFIX = os.getenv("RTT_CC_PREFIX") or "arm-none-eabi-"
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
    CFPFLAGS = " -msoft-float"
    AFPFLAGS = " -mfloat-abi=softfp -mfpu=neon"
    DEVICE = " -march=armv7-a -mtune=cortex-a7 -ftree-vectorize -ffast-math -funwind-tables -fno-strict-aliasing"

    CXXFLAGS = DEVICE + CFPFLAGS + " -Wall -fdiagnostics-color=always -D__STDC_FORMAT_MACROS"
    CFLAGS = DEVICE + CFPFLAGS + " -Wall -Wno-cpp -std=gnu99 -D_POSIX_SOURCE -fdiagnostics-color=always"
    AFLAGS = DEVICE + " -c" + AFPFLAGS + " -x assembler-with-cpp"
    LFLAGS = (
        DEVICE
        + f" -Wl,--gc-sections,-Map=build/{BOARD_NAME}.map,-cref,-u,system_vectors -T "
        + LINK_SCRIPT
        + " -lsupc++ -lgcc -static"
    )
    CPATH = ""
    LPATH = ""

    if BUILD == "debug":
        CFLAGS += " -O0 -gdwarf-2"
        CXXFLAGS += " -O0 -gdwarf-2"
        AFLAGS += " -gdwarf-2"
    else:
        CFLAGS += " -Os"
        CXXFLAGS += " -Os"
    CXXFLAGS += " -Woverloaded-virtual -fno-rtti"

    M_CFLAGS = CFLAGS + " -mlong-calls -fPIC "
    M_CXXFLAGS = CXXFLAGS + " -mlong-calls -fPIC"
    M_LFLAGS = (
        DEVICE + CXXFLAGS + " -Wl,--gc-sections,-z,max-page-size=0x4" + " -shared -fPIC -nostartfiles -nostdlib -static-libgcc"
    )
    M_POST_ACTION = STRIP + " -R .hash $TARGET\n" + SIZE + " $TARGET \n"

    DUMP_ACTION = OBJDUMP + " -D -S $TARGET > rtt.asm\n"
    POST_ACTION = OBJCPY + f" -O binary $TARGET build/{BOARD_NAME}.bin\n" + SIZE + " $TARGET \n"
else:
    print("ERROR: only support gcc toolchain!!!")
    exit(-1)

TARGET_FILE = f"build/{BOARD_NAME}." + TARGET_EXT


def get_build_env():
    from building import GetDepend, DefaultEnvironment, Environment

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

    if GetDepend("RT_USING_SMART"):
        # use smart link.lds
        env["LINKFLAGS"] = env["LINKFLAGS"].replace("link.lds", "link_smart.lds")

    return env
