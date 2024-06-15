import os
import json

if "ENV_ROOT" in os.environ:
    env_root = os.environ["ENV_ROOT"]
else:
    env_root = "unknow"

env_root = os.path.normpath(env_root).replace("\\", "/")

if "GCC_ARM_HOME" in os.environ:
    gcc_path = os.environ["RTT_EXEC_PATH"]
elif "GCC_EXEC_PATH" in os.environ:
    gcc_path = os.environ["GCC_EXEC_PATH"]
elif "RTT_EXEC_PATH" in os.environ:
    gcc_path = os.environ["RTT_EXEC_PATH"]
else:
    gcc_path = r"C:/nextpilot-windows-toolchain/toolchain/gcc/gcc-arm-none-eabi-10.3-2021.10/bin"

gcc_path = os.path.normpath(gcc_path).replace("\\", "/")


if "QEMU_HOME" in os.environ:
    qemu_home = os.environ["QEMU_HOME"]
elif env_root != "unknow":
    if os.path.exists(os.path.join(env_root, r"tools/qemu/qemu64")):
        qemu_home = os.path.join(env_root, r"tools/qemu/qemu64")
    else:
        qemu_home = os.path.join(env_root, r"tools/qemu/qemu32")
else:
    qemu_home = "C:/nextpilot-windows-toolchain/toolchain/qemu/qemu-w64-v8.2.0"

qemu_home = os.path.normpath(qemu_home).replace("\\", "/")


def add_scons_debug_lanuch(launch_json_file="launch.json"):
    if os.path.isfile(launch_json_file):
        with open(launch_json_file, "r", encoding="utf-8") as f:
            launch = json.load(f)
    else:
        launch = {"version": "0.2.0", "configurations": []}

    for config in launch["configurations"]:
        if config["name"] == "NextPilot Scons-Debug":
            launch["configurations"].remove(config)

    launch["configurations"].append(
        {
            "name": "NextPilot Scons-Debug",
            "type": "debugpy",
            "request": "launch",
            "program": os.path.join(os.path.dirname(__file__), "scons-debug.py").replace("\\", "/"),
            "args": "${command:pickArgs}",
        }
    )

    with open(launch_json_file, "w", encoding="utf-8") as f:
        json.dump(launch, f, indent=4)


def add_qemu_debug_launch(launch_json_file="launch.json"):
    if os.path.isfile(launch_json_file):
        with open(launch_json_file, "r", encoding="utf-8") as f:
            launch = json.load(f)
    else:
        launch = {"version": "0.2.0", "configurations": []}

    for config in launch["configurations"]:
        if config["name"] == "NextPilot Qemu-Debug":
            launch["configurations"].remove(config)

    launch["configurations"].append(
        {
            "name": "NextPilot Qemu-Debug",
            "type": "cppdbg",
            "request": "launch",
            "program": "${workspaceRoot}/build/sitl-qemu-default.elf",
            # "args": [],
            # "stopAtEntry": False,
            "cwd": "${workspaceRoot}",
            # "environment": [],
            # "externalConsole": False,
            "MIMode": "gdb",
            "miDebuggerPath": os.path.join(gcc_path, "arm-none-eabi-gdb.exe").replace("\\", "/"),
            "miDebuggerServerAddress": "localhost:1234",
            "servertype": "qemu",
            "setupCommands": [
                {"text": "cd ${workspaceRoot}"},
                {"text": "file build/sitl-qemu-default.elf"},
                # {"text": "break main"},
            ],
        }
    )

    with open(launch_json_file, "w", encoding="utf-8") as f:
        json.dump(launch, f, indent=4)


def add_rtt_studio_setting(setting_json_file="settings.json"):
    import multiprocessing

    if os.path.isfile(setting_json_file):
        with open(setting_json_file, "r", encoding="utf-8") as f:
            setting = json.load(f)
    else:
        setting = {}

    setting["RTT_Studio.Build.Parallel_Jobs"] = str(multiprocessing.cpu_count())
    setting["RTT_Studio.Env_Location"] = env_root
    setting["RTT_Studio.RTT_ROOT"] = "../../../rtos/rt-thread"
    setting["RTT_Studio.Toolchain_Location"] = gcc_path
    setting["RTT_Studio.SystemInfo.Push.Flag"] = False
    setting["RTT_Studio.Debuger.Gdb_Path"] = gcc_path + "/arm-none-eabi-gdb.exe"
    setting["RTT_Studio.Debuger.Generate_Configuration_Automatically"] = False
    setting["RTT_Studio.Debuger.QEMU.Location"] = qemu_home
    setting["RTT_Studio.Debuger.Adapter"] = "QEMU"
    setting["RTT_Studio.Debuger.Bin_File"] = "build/sitl-qemu-default.elf"
    setting["RTT_Studio.Debuger.Executable_File"] = "build/sitl-qemu-default.elf"
    setting["RTT_Studio.Debuger.QEMU.Commands"] = (
        "qemu-system-arm -M vexpress-a9 -kernel build/sitl-qemu-default.elf -display none -serial stdio -sd sd.bin"
    )

    with open(setting_json_file, "w", encoding="utf-8") as f:
        json.dump(setting, f, indent=4)


if __name__ == "__main__":
    add_scons_debug_lanuch()
    add_qemu_debug_launch()
    add_rtt_studio_setting()
