:: 请先下载env并解压到不含中文的目录，然后打开env.exe，并在env中运行该脚本
:: https://download_redirect.rt-thread.org/download/env_release/env_released_1.3.5.7z

@echo off

:: 切换目录
pushd %~dp0

:: 判断是否在env.exe环境
if "%ENV_ROOT%" == "" (
    echo.
    echo ERROR: please run in ENV.exe console
    pause
    exit
) else (
    echo.
    echo ENV_ROOT = %ENV_ROOT%
)

:: 执行env初始化
@REM call %ENV_ROOT%\tools\ConEmu\ConEmu\CmdInit.cmd

:: 安装pip工具
if exist %ENV_ROOT%\tools\Python27\Scripts\pip.exe (
    echo.
    echo pip had installed
) else (
    echo.
    echo install pip tools
    python %ENV_ROOT%\tools\get-pip.py
    pip config set global.index-url https://mirrors.aliyun.com/pypi/simple
)

:: 安装python依赖项
echo.
echo pip install -r requirements_v2.txt
pip install -r requirements_v2.txt

:: 退出当前目录
popd