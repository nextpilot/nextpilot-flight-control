#
#  执行 scons 时调试 scons 运行的python脚本
#
###################################################
# 方法一：使用python远程附加方式
###################################################
# 首先添加一个launch.json配置
# {
#     "name": "NextPilot Scons-Debug-Attach",
#     "type": "debugpy",
#     "request": "attach",
#     "connect": {
#         "host": "localhost",       // 远程调试debugpy服务器
#         "port": 5678               // 远程调试debugpy端口号
#     },
#     "pathMappings": [
#         {
#             "localRoot": "${workspaceFolder}",
#             "remoteRoot": "."
#         }
#     ]
# },
#
# 接着，在所需调试的python文件打上断点，比如scons_helper.py
#
# 然后，在命令行输入以下命令，注意 scons-debug.py 路径哈
# 其中 default -j20 是传递给 scons 的参数
# python -m debugpy --listen 5678 --wait-for-client scons-debug.py default -j20
#
# 选择 NextPilot Scons-Debug-Attach 配置，即可启动调试
#
###################################################
# 方法二：使用python调试文件方式
###################################################
# 首先添加一个launch.json配置
# {
#     "name": "NextPilot Scons-Debug",
#     "type": "debugpy",
#     "request": "launch",
#     "program": "tools/building/scons-debug.py",   // 要调试的文件
#     "args": "${command:pickArgs}"                 // 从UI获取传递给scons的参数
# },
#
# 接着，在所需调试的python文件打上断点，比如scons_helper.py
#
# 选择 NextPilot Scons-Debug 配置，此时会提示输入参数，也就是希望传递给scons的参数
#

import SCons.Script

SCons.Script.main()
