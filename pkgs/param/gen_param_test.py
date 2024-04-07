import os
import sys
import subprocess

prj_root = os.path.abspath(os.path.join(os.getcwd(), "../../"))
src_path = set()

for root, dirs, files in os.walk(prj_root):
    for file in files:
        if file.endswith("_params.c") and "rtos" not in root:
            src_path.add(root)


cmd = [
    sys.executable,
    "tools/process_params.py",
    "--xml",
    "parameters.xml",
    "--json",
    "paramters.json",
    "--compress",
    "--inject-xml",
    os.path.join(os.getcwd(), "tools/parameters_injected.xml"),
    "--board",
    "nextpilot",
    # --verbose
    "--src-path",
] + list(src_path)
subprocess.call(cmd)

cmd = [sys.executable, "tools/generate_params.py", "--xml", "parameters.xml", "--dest", os.getcwd()]
subprocess.call(cmd)
