# 该脚本用于安装nextpilot windows toolchain，包括以下工具
# git
# python
# gcc-arm，编译嵌入式代码
# qemu，虚拟飞行仿真



# 判断是国内还是国外环境
Test-Connection www.google.com | Out-Null
if ($?) {
    $china_network = False
}
else {
    $china_network = True
}

if ($args[0] -eq "--china") {
    $china_network = True
}

function Test-Command([string] $cmd) {
    ($null -ne (Get-Command $cmd -ErrorAction SilentlyContinue))
}


function Install-Git() {
    if (!(Test-Command git)) {
        if ($china_network) {
            $git_url = "https://registry.npmmirror.com/-/binary/git-for-windows/v2.39.2.windows.1/Git-2.39.2-64-bit.exe"
        }
        else {
            $git_url = "https://github.com/git-for-windows/git/releases/download/v2.39.2.windows.1/Git-2.39.2-64-bit.exe"
        }

        Write-Host "Git is not installed, will install Git"
        Write-Output "Downdload Git for $git_url"
        wget -O Git-2.39.2-64-bit.exe $git_url
        Write-Output "Install git"
        cmd /c Git-2.39.2-64-bit.exe /quiet PrependPath=1
    }
    else {
        Write-Output "Git environment has installed. Jump this step."
    }
}


function Install-Python() {
    cmd /c python --version | findstr "Python 3" | Out-Null
    if (!$?) {
        Write-Host "Python3 not found, will install python 3.11.2"
        Write-Host "downloading python ..."
        Invoke-WebRequest -O python-3.11.2.exe https://www.python.org/ftp/python/3.11.2/python-3.11.2.exe
        Write-Host "installing python ..."
        cmd /c python-3.11.2.exe /quiet PrependPath=1 Include_test=0
    }
    else {
        Write-Host "Python had installed"
    }

    # 安装pip
    cmd /c python -m ensurepip --upgrade

    # 设置pip
    if ($china_network) {
        $PIP_SOURCE = "http://mirrors.aliyun.com/pypi/simple"
        $PIP_HOST = "mirrors.aliyun.com"
    }
    else {
        $PIP_SOURCE = "https://pypi.org/simple"
        $PIP_HOST = "pypi.org"
    }
    cmd /c python -m pip list -i $PIP_SOURCE --trusted-host $PIP_HOST | Out-Null

    # 安装所需的pip库
    $pkgs = "scons==4.4.0", "pyocd", "requests", "psutil", "argparse", "matplotlib", "numpy", "pymavlink",
    "pyyaml", "jinja2", "mkdocs", "empy", "pyros-genmsg", "packaging", "clang-format", "pre-commit"

    foreach ($pkg in $pkgs) {
        Write-Output "pipp install $pkg"
        cmd /c python -m pip install $pkg -i $PIP_SOURCE --trusted-host $PIP_HOST
    }
}



function Install-GCC() {
    $gcc_arm_ver = "10.3-2021.10"
    $gcc_arm_url = https://armkeil.blob.core.windows.net/developer/Files/downloads/gnu-rm/$gcc_arm_ver/gcc-arm-none-eabi-$gcc_arm_ver-x86_64-linux.tar.bz2


}

function Install-Qemu() {}


function Install-Toolchain() {
    # env可能安装目录
    if ($env:nextpilot_toolchain_root) {
        #
        $tools_root = "$env:nextpilot_toolchain_root"
    }
    else {
        # env默认安装到$env:USERPROFILE\.env
        $tools_root = "$env:USERPROFILE\.env"
    }
    # 判断env目录是否存在
    if (!(Test-Path -Path $tools_root)) {
        if ($china_network) {
            $tools_url = "https://gitee.com/nextpilot/nextpilot-windows-toolchain/repository/archive/master.zip"
        }
        else {
            $tools_url = "https://gitee.com/nextpilot/nextpilot-windows-toolchain/repository/archive/master.zip"
        }
        # 下载toolchain
        Write-Host "Downloading toolchain from $tools_url"
        wget -O nextpilot-windows-toolchain.zip $tools_url
        # 解压toolchain
        Write-Host "Extract toolchain to $env:USERPROFILE\.env"
        Expand-Archive -Path nextpilot-windows-toolchain.zip -DestinationPath "$env:USERPROFILE\.env"
        # 设置环境变量
        [System.Environment]::SetEnvironmentvariable("nextpilot_toolchain_root", "$env:USERPROFILE\.env", "user")
    }
    else {
        Write-Host "Nextpilot Toolchain had installed"
    }
}

if ($args[0] -eq "--step") {
    Install-Git
    Install-Python
    Install-Qemu
    Install-GCC
}
else {
    Install-Toolchain
}