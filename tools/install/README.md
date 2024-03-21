
# 安装开发环境

|文件|功能|
|---|---|
|windows.bat|安装所需的python包，必须在env终端中运行|
|ubuntu.sh|自动安装ubuntu工具链|
|requirements_v2.txt|python2环境下的python包清单|
|requirements_v3.txt|python2环境下的python包清单|

> `requirements.txt`是所需的python第三方库，通过`pip freeze > requirements.txt`导出的，重新安装指令为`pip install -r requirements.txt`

## Windows

Nextpilot开发团队已经提供了完整的开发工具链，其中包含：

- gcc-arm-none-eabi-10.3-2021.10
- MinGit-2.42.0.2-64-bit
- python-3.10.11-embed-amd64 （已经添加了所需的python模块）
- rt-thread [packages]() 和 [env]()

请[点击这里](https://gitee.com/nextpilot/nextpilot-windows-toolchain/repository/archive/master.zip)下载，然后解压到`c:\nextpilot-windows-toolchain`。当也可以直接使用git clone命令进行下载：

```bat
rem 下载开发工具链
git clone --recursive https://gitee.com/nextpilot/nextpilot-windows-toolchain.git c:\nextpilot-windows-toolchain
```

双击运行`c:\nextpilot-windows-toolchain\start.bat`，启动CMD.exe终端。为了方便使用，建议将start.bat添加桌面快捷方式。在CMD终端中输入以下命令，下载nextpilot飞控代码：

```bat
rem 下载nextpilot代码
git clone --recursive https://gitee.com/nextpilot/nextpilot-flight-control.git c:\nextpilot-windows-toolchain\workspace\nextpilot-flight-control
```

切换到target目录下运行`scons`命令即可进行编译等操作：

```bat
cd c:\nextpilot-windows-toolchain\workspace\nextpilot-flight-control

rem 切换到target目录
cd target\target\sitl\qemu-vexpress-a9

rem 编译固件
scons

rem 启动仿真
qemu.bat
```

## Ubuntu

请在终端中直接运行`ubuntu.sh`即可安装ubuntu下的开发环境：

```shell
sudo chmod +777 ubuntu.sh
./ubuntu.sh
```

以上脚本，会将所需的工具和脚本下载到`~/.env`目录，开发所需的环境变量配置在`~/.env/env.sh`里面。
