
# 搭建开发环境

|文件|功能|
|---|---|
|windows.ps1|自动安装windows工具链，需要联网下载|
|ubuntu.sh|自动安装ubuntu工具链，需要联网下载|
|requirements_v2.txt|python2环境下的python包清单|
|requirements_v3.txt|python2环境下的python包清单|

> `requirements.txt`是所需的python第三方库，通过`pip freeze > requirements.txt`导出的，重新安装指令为`pip install -r requirements.txt`

## Windows

由于Windows在线安装，经常会因为各种不知名原因而失败，因此NextPilot开发团队将Windows环境下的工具链进行打包，其中包含：

- gcc-arm-none-eabi-10.3-2021.10
- MinGit-2.42.0.2-64-bit
- python-3.11 （已经添加了所需的python模块）
- qemu
- rt-thread [packages]() 和 [env]()

请下载<https://gitee.com/nextpilot/nextpilot-windows-toolchain/repository/archive/master.zip>，然后解压到`c:\nextpilot-windows-toolchain`。当也可以直接使用git clone命令进行下载：

```bat
rem 下载开发工具链
git clone --recursive https://gitee.com/nextpilot/nextpilot-windows-toolchain.git c:\nextpilot-windows-toolchain
```

双击运行`c:\nextpilot-windows-toolchain\start.bat`，启动CMD.exe终端。为了方便使用，建议将start.bat添加桌面快捷方式。

## Ubuntu

请在终端中直接运行`ubuntu.sh`即可安装ubuntu下的开发环境：

```shell
sudo chmod 777 tools/ubuntu.sh
./tools/ubuntu.sh
```

以上脚本，会将所需的工具和脚本下载到`~/.ndt`目录，开发所需的环境变量配置在`~/.ndt/ndt.sh`里面。

当然NextPilot开发团队也为您准好了`到手用的`工具链，请下载<ttps://gitee.com/nextpilot/nextpilot-ubuntun-toolchain>，您直接clone到`~/.ndt`即可：

```shell
# 下载开发工具链
git clone --recursive https://gitee.com/nextpilot/nextpilot-ubuntun-toolchain.git ~/.ndt
```
