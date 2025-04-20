import os
import sys
import subprocess

cwd = os.getcwd()

generated_events_from_source_file = os.path.join(cwd, "events_from_source.json")
generated_events_common_enums_file = os.path.join(cwd, "events_common_with_enums.json")
generated_events_combined_all_file = os.path.join(cwd, "events_combined_all.json")

all_src_files = {
    os.path.join(cwd, "../../apps"),
}


# Generating event json file from source
cmd = [
    sys.executable,
    os.path.join(cwd, "tools/process_events.py"),
    "--json",
    generated_events_from_source_file,
    "--src-path",
] + list(all_src_files)

subprocess.run(cmd)


# combine common.json with our enums for the code generation
cmd = [
    sys.executable,
    os.path.join(cwd, "libevents/scripts/combine.py"),
    os.path.join(cwd, "enums.json"),
    os.path.join(cwd, "libevents/events/common.json"),
    "--output",
    generated_events_common_enums_file,
]

subprocess.run(cmd)


# Generating combined event json file
cmd = [
    sys.executable,
    os.path.join(cwd, "libevents/scripts/combine.py"),
    generated_events_from_source_file,
    generated_events_common_enums_file,
    "--output",
    generated_events_combined_all_file,
]

subprocess.run(cmd)


# 检验合并的events_all.json文件
cmd = [
    sys.executable,
    os.path.join(cwd, "libevents/scripts/validate.py"),
    generated_events_combined_all_file,
]

# 创建压缩文件
# cmd = [
#     sys.executable,
#     os.path.join(cwd, "../../tools/compress.py"),
#     generated_events_combined_all_file,
# ]

# subprocess.run(cmd)

# Generating event header file
cmd = [
    sys.executable,
    os.path.join(cwd, "libevents/scripts/generate.py"),
    "--template",
    os.path.join(cwd, "libevents/libs/cpp/templates/events_generated.h.jinja"),
    "--output",
    cwd,
    generated_events_common_enums_file,
]
print("22222222222222222222")

subprocess.run(cmd)


exit(0)
