import os
import sys
import argparse


script_path = os.path.dirname(os.path.realpath(__file__))


def parse_airframe_toml(airframe_toml_dirs = [os.getcwd()], airframe_toml_files=set()):
    import tomli

    # 搜索所有_params.toml文件
    for param_toml_dir in airframe_toml_dirs:
        for root, dirs, files in os.walk(param_toml_dir):
            for file in files:
                if file.endswith(".toml"):
                    airframe_toml_files.add(os.path.join(root, file))

    airframes =[]
    for file in airframe_toml_files:
        with open(file, "rb") as f:
            airframe_toml_dict = tomli.load(f)
            print(airframe_toml_dict)


def main():
    parse_airframe_toml([os.path.join(script_path, "../")])


if __name__ == "__main__":
    main()
