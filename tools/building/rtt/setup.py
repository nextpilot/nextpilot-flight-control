from setuptools import setup, find_packages

setup(
    name="rtt env",  # 应用名
    version="1.0.0",  # 版本号
    packages=find_packages("src"),  # 包含所有src中的包
    package_dir={"": "src"},  # 告诉distutils包都在src下
    # 打包时需要打包的数据文件，如图片，配置文件等
    package_data={
        # 任何包中含有.txt文件，都包含它
        "": ["*.xml"],
    },
    # 项目依赖的 Python 库，使用 pip 安装本项目时会自动检查和安装依赖
    install_requires=["numpy"],
    # 指定项目依赖的 Python 版本。
    python_requires=">=3",
    # 是否使用静态文件，为true时静态文件生效，否则不起作用
    include_package_data=True,
    # 此项需要，否则卸载时报windows error
    zip_safe=False,
    setup_cfg=True,
    # 其他信息暂时没加，后面会加附录进行说明
    entry_ponits={"console_scripts": ("rtt = rtt:main")},
)
