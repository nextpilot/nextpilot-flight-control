# scons调试

​		RT-Thead使用scons做为构建工具，而SCons 是一套由 Python 语言编写的开源构建系统，支持标准python语法，并且能够非常方便的通过python脚本对编译流程进行扩展与定制。

​		本文以qemu仿真工程为例，给出在进行scons命令编译时如何对python脚本进行调试。

​		在nextpilot工程下，我们以调试`tools\building\scons_helper.py`文件为例进行讲解。

## 方式一：附加进程方式

### 工具

​		调试环境基于VSCode搭建，需要安装插件有Python、Python Debugger。

### 原理

​		VSCode提供了多种调试方式，其中包括**Attach using Process ID**，即附加进程调试。例如在launch.json中新增配置时会提示选择的调试方式。

![image-20240611230700561](image/image-20240611230700561.png)

​		当在RT-Thread ENV下运行scons构建命令后，会启动一个python线程，我们只需要找到该线程即可。

### 步骤

#### 新建调试配置

​		首先我们需要在qemu工程中的`.vscode/launch.json`中新增调试配置。打开launch.json文件后，可以直接在右下角点击**Add Configuration**，然后选择**Attach using Process ID**。并修改如下：

```shell
{
    "version": "0.2.0",
    "configurations": [
        {
            "name": "Python Debugger: Attach using Process Id",
            "type": "debugpy",
            "request": "attach",
            "python": "C:\\nextpilot-windows-toolchain\\toolchain\\python\\python-3.11.9-amd64\\python.exe",
            "processId": "${command:pickProcess}"
        },
    ],
    "enable": true
}
```

关于配置项说明如下：

- 这里我们增加了"python"选项，该选项用于设置python解释器，这里我们采用的是nextpilot工具链下的解释器；
- "processId"选项就是待输入的python.exe线程ID，后面会说明如何填写。



#### 更改scons_helper.py

​		注意，由于目前nextpilot在构建时会根据模板自动生成launch.json文件，故需要将自动生成功能注释掉，只需要注释`tools\building\scons_helper.py`文件中如下一行代码即可：

```python
# 生成调试文件
# gen_setting_file()
```

​		另外为了在进入要调试的代码前查找到python.exe的ID，可以在入口处等待一段时间，在`tools\building\scons_helper.py`文件中增加延时：

```python
def build(target=None):
    import rtconfig
    import building
    import time
    print("Before enter debug, sleep for a while, now you can set processId in launch.json")
    time.sleep(20)
    print("Begin enter debug")
```



#### 准备调试

​		打开cmd终端，输入**tasklist | find "python"**，查看当前的python线程，列出来的线程应该是vscode启动的python终端。

​		启动nextpilot工具链下的`start.bat`文件，进入ENV终端，用于输入构建命令。

​		在`tools\building\scons_helper.py`的build函数内打个断点：

![image-20240611234359215](image/image-20240611234359215.png)

#### 启动调试

​		在ENV终端，输入**scons -20**，然后切到cmd终端再次输入**tasklist | find "python"**命令，然后查看新建的python进程的ID，并拷贝。

![image-20240611233232830](image/image-20240611233232830.png)

​		例如上图中新建的ID为12012，将该ID拷贝至`launch.json`文件内的"processId"选项后，如下：

```shell
"processId": "12012"
```

​		这时切到VSCode，然后在左侧选择调试工具栏，并且选择刚才新建的调试配置并启动。

![image-20240611234246564](image/image-20240611234246564.png)

​		成功进入调试后在ENV终端会提示如下：

![image-20240611233116933](image/image-20240611233116933.png)

​		最终，会在停在断点位置：

![scons_debug_1](image/scons_debug_1.png)

#### 注意事项

1. 这种方式需要修改源码，且有多个先后步骤，执行起来不太方便；
2. 一定要在延时函数（scons_helper.py的build()内增加的time.sleep()函数）执行完之前点击调试，不然断点就可能会过；