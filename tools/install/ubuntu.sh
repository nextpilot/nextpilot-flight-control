#! /usr/bin/env bash

echo
echo '******************************************************************'
echo "* Welcome to Nextpilot Develop Envrionment Installer on Ubuntu LTS"
echo '*      _   __             __   ____   _  __        __'
echo '*     / | / /___   _  __ / /_ / __ \ (_)/ /____   / /_'
echo '*    /  |/ // _ \ | |/_// __// /_/ // // // __ \ / __/'
echo '*   / /|  //  __/_>  < / /_ / ____// // // /_/ // /_'
echo '*  /_/ |_/ \___//_/|_| \__//_/    /_//_/ \____/ \__/'
echo '*'
echo '* Copyright All Reserved 2015-2024 NextPilot Development Team'
echo '******************************************************************'

################################################################
# 判断ubuntu版本
################################################################
# 支持的Ubuntu版本列表
supported_versions=("20.04" "22.04")
# 当前Ubuntu版本
ubuntu_release="$(lsb_release -rs)"
if [[ ! " ${supported_versions[@]} " =~ " ${ubuntu_release} " ]]; then
    echo
    echo "Error: only support Ubuntu ${supported_versions[*]}, but current $ubuntu_release"
    exit 1
fi

################################################################
# 输入参数解析
################################################################
using_mirror=0
# need_install=0
using_ci=0
for arg in "$@"; do
    if [[ "$arg" == "--gitee" ]]; then
        using_mirror=1
    fi
    # if [[ "$arg" == "--install" ]]; then
    #     need_install=1
    # fi
    if [[ "$arg" == "--ci" ]]; then
        using_ci=1
    fi
done

if [ $using_mirror ]; then
    # echo "using gitee repo"
    pkg_url=https://gitee.com/RT-Thread-Mirror/packages.git
    env_url=https://gitee.com/RT-Thread-Mirror/env.git
    sdk_url=https://gitee.com/RT-Thread-Mirror/sdk.git
    rtt_url=https://gitee.com/rtthread/rt-thread.git
else
    # echo "using github reop"
    pkg_url=https://github.com/RT-Thread/packages.git
    env_url=https://github.com/RT-Thread/env.git
    sdk_url=https://github.com/RT-Thread/sdk.git
    rtt_url=https://github.com/RT-Thread/rt-thread.git
fi

################################################################
# 准备安装目录
################################################################
# install path
ndt_dir=$HOME/.ndt
# shell file dirctory
shell_dir=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)

mkdir -p $ndt_dir
echo "export NDT_ROOT=$ndt_dir" >$ndt_dir/init.sh

echo
echo "################################################################"
echo "install ubuntu dependencies"
echo "################################################################"

# update ubuntu
sudo apt update -y
sudo apt upgrade -y

if [[ $using_ci != 0 ]]; then
    sudo apt install -y -qq \
        gcc-multilib libncurses5 libncurses5-dev libncursesw5-dev \
        scons
else
    sudo apt install -y \
        git \
        gcc gcc-arm-none-eabi binutils-arm-none-eabi gdb-multiarch \
        libncurses5 libncurses5-dev libncursesw5-dev \
        python3 python3-pip python3-setuptools python3-wheel \
        qemu qemu-system-arm
fi

# config git http.postBuffer
git config --global http.postBuffer 524288000

# add user to dialout group (serial port access)
if [ -n "$USER" ]; then
    sudo usermod -a -G dialout $USER
fi

echo
echo "################################################################"
echo "install python3 dependencies"
echo "################################################################"
if [ $using_mirror == 1 ]; then
    echo "set global.index-url to https://mirrors.aliyun.com/pypi/simple/"
    python3 -m pip config set global.index-url https://mirrors.aliyun.com/pypi/simple/
fi
# upgrade pip
python3 -m pip install --user --upgrade pip
# create venv
python3 -m venv ~/.ndt/.venv
source ~/.ndt/.venv/bin/activate
# install pip packages
if [ -f ${shell_dir}/requirements_v3.txt ]; then
    echo "pip insall from ${shell_dir}/requirements_v3.txt"
    python3 -m pip install -r ${shell_dir}/requirements_v3.txt
else
    python3 -m pip install scons==4.4.0 requests psutil
    python3 -m pip install -U pyocd
fi
echo 'export PATH=`python3 -m site --user-base`/bin:$PATH' >>$ndt_dir/init.sh

echo
echo "################################################################"
echo "install arm-gcc toolchain"
echo "################################################################"
# arm-gcc下载地址为https://developer.arm.com/downloads/-/gnu-rm
gcc_arm_ver="10.3-2021.10"
gcc_arm_url=https://armkeil.blob.core.windows.net/developer/Files/downloads/gnu-rm/$gcc_arm_ver/gcc-arm-none-eabi-$gcc_arm_ver-x86_64-linux.tar.bz2

if [ -f $ndt_dir/toolchain/gcc-arm/gcc-arm-none-eabi-$gcc_arm_ver/bin/arm-none-eabi-gcc ]; then
    echo "found arm-none-eabi-gcc-${gcc_arm_ver} in $ndt_dir/toolchain/gcc-arm/gcc-arm-none-eabi-$gcc_arm_ver"
else
    # 下载
    if [ ! -f $ndt_dir/download/gcc-arm-none-eabi-$gcc_arm_ver-x86_64-linux.tar.bz2 ]; then
        echo "downloading arm-none-eabi-gcc-${gcc_arm_ver} from $gcc_arm_url"
        mkdir -p $ndt_dir/download
        wget -O $ndt_dir/download/gcc-arm-none-eabi-$gcc_arm_ver-x86_64-linux.tar.bz2 $gcc_arm_url
    fi
    # 解压
    echo "extracting to $ndt_dir/toolchain/gcc-arm/gcc-arm-none-eabi-$gcc_arm_ver"
    mkdir -p $ndt_dir/toolchain/gcc-arm
    tar -xjf $ndt_dir/download/gcc-arm-none-eabi-$gcc_arm_ver-x86_64-linux.tar.bz2 -C $ndt_dir/toolchain/gcc-arm
    gcc_arm_dir=$ndt_dir/toolchain/gcc-arm/gcc-arm-none-eabi-$gcc_arm_ver/bin
fi
#
echo "export PATH=$ndt_dir/toolchain/gcc-arm/gcc-arm-none-eabi-$gcc_arm_ver/bin:\$PATH" >>$ndt_dir/init.sh
echo "export RTT_EXEC_PATH=$ndt_dir/toolchain/gcc-arm/gcc-arm-none-eabi-$gcc_arm_ver/bin" >>$ndt_dir/init.sh
echo "export RTT_CC=gcc" >>$ndt_dir/init.sh

echo
echo "################################################################"
echo "install rt-thread tools"
echo "################################################################"
echo "export ENV_ROOT=$ndt_dir/rtthread" >>$ndt_dir/init.sh
# 下载rtt-packages到$HOME/.ndt/rtthread/packages
pkg_dir=$ndt_dir/rtthread/packages/packages
if [ -d $pkg_dir/.git ]; then
    echo "pull rtt-packages from $pkg_url"
    pushd $pkg_dir >/dev/null 2>&1
    git pull
    popd >/dev/null 2>&1
else
    echo "clone rtt-packages frome $pkg_url"
    rm -rf $pkg_dir
    git clone $pkg_url $pkg_dir
fi
echo 'source "$PKGS_DIR/Kconfig"' >$ndt_dir/rtthread/packages/Kconfig
echo "export PKGS_DIR=$ndt_dir/rtthread/packages" >>$ndt_dir/init.sh

# 下载rtt-env到$HOME/.ndt/rtthread/tools/scripts/
env_dir=$ndt_dir/rtthread/tools/scritps
if [ -d $env_dir/.git ]; then
    echo "pull rtt-env from $env_url"
    pushd $env_dir >/dev/null 2>&1
    git pull
    popd >/dev/null 2>&1
else
    echo "clone rtt-env from $env_url"
    rm -rf $env_dir
    git clone $env_url $env_dir
fi
# echo "export PATH=$env_dir:\$PATH" >> $ndt_dir/init.sh

echo
echo "################################################################"
echo "show toolchain info"
echo "################################################################"
# load init.sh
source ~/.ndt/init.sh
echo "python: $(python3 -V), $(which python3)"
echo "----------------------------------------------------------------"
echo "pip: $(pip3 -V)"
echo "----------------------------------------------------------------"
echo "scons: $(which scons)"
echo "$(scons --version)"
echo "----------------------------------------------------------------"
echo "gcc: $(which arm-none-eabi-gcc)"
echo "$(arm-none-eabi-gcc --version)"
