import os
import sys
import argparse

try:
    from jinja2 import Environment, FileSystemLoader
except ImportError as e:
    print("Failed to import jinja2: " + str(e))
    print("")
    print("You may need to install it using:")
    print("    pip3 install --user jinja2")
    print("")
    sys.exit(-1)

script_path = os.path.dirname(os.path.realpath(__file__))


def param_parse_toml(param_toml_dirs=[os.getcwd()], param_toml_files=set()):
    import toml

    # 合法的类型
    param_valid_type = {"enum", "bool", "int8", "uint8", "uint32", "int32", "float"}
    # 必须包含的字段
    param_must_fields = {"name", "type", "default"}
    # 合法的字段
    param_valid_fields = {
        "board",
        "category",
        "short_desc",
        "long_desc",
        "name",
        "type",
        "default",
        "values",
        "reboot_required",
        "disarm_required",
        "develop_required",
        "min",
        "max",
        "unit",
        "decimal",
        "increment",
    }

    # 搜索所有_params.toml文件
    for param_toml_dir in param_toml_dirs:
        for root, dirs, files in os.walk(param_toml_dir):
            for file in files:
                if file.endswith("_params.toml"):
                    param_toml_files.add(os.path.join(root, file))

    # 从_params.toml中读取param
    params = []
    for file in param_toml_files:
        param_toml_dict = toml.load(file)
        for param_section in param_toml_dict["parameters"]:

            # 必须包含name,type和default字段
            for key in param_must_fields:
                if key not in param_section:
                    raise Exception("without '{:}' in {:}".format(key, file))

            # 参数类型合法性校验
            param_type = param_section["type"]
            if param_type not in param_valid_type:
                raise Exception("unkonwn param type '{:}' in {:}".format(param_type, file))

            # 取值说明
            # if "values" in param_section:
            #     param_values = [
            #         x.strip().split(":", 1) for x in param_section["values"].split("\n")
            #     ]
            # else:
            #     param_values = None
            # param_section["values"] = param_values

            # 实例个数
            if "instance" in param_section:
                instance = param_section["instance"]
                del param_section["instance"]
            else:
                instance = 1

            # 默认值
            default_values = param_section.get("default", None)
            if default_values:
                if type(default_values) == list:
                    if len(default_values) < instance:
                        default_values += [0] * (instance - len(default_values))
                else:
                    default_values = [default_values] * instance
            else:
                default_values = [0] * instance

            # 计算flags
            flags = 0
            if param_section.get("system_required", False):
                flag |= 1 << 0
            if param_section.get("reboot_required", False):
                flag |= 1 << 1
            if param_section.get("disarm_required", False):
                flag |= 1 << 2
            param_section["flags"] = flags

            for idx in range(instance):
                param = param_section.copy()
                # 重写default
                param["default"] = default_values[idx]
                # 将字段${i}替换为实际的值
                for key in param_section:
                    if type(param_section[key]) == str and "${i}" in param_section[key]:
                        param[key] = param_section[key].replace("${i}", str(idx))
                    else:
                        pass
                # 名称转为大写
                param["name"].upper()
                param["type"].lower()
                params.append(param)
    # 根据name进行排序
    params.sort(key=lambda x: x["name"])
    # TODO:判断是否有重复的name

    return params


def param_generate_hpp(params, dest=os.getcwd()):

    if not os.path.isdir(dest):
        os.path.mkdir(dest)

    env = Environment(loader=FileSystemLoader(script_path))

    template_files = ["parameters.hpp.jinja"]

    for template_file in template_files:
        template = env.get_template(template_file)
        with open(os.path.join(dest, template_file.replace(".hpp.jinja", "_autogen.hpp")), "w") as f:
            f.write(template.render(params=params))


def param_generate_json(params):
    pass


def main():

    params = param_parse_toml([script_path])
    param_generate_hpp(params, script_path)


if __name__ == "__main__":
    main()
