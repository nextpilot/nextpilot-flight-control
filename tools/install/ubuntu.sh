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
echo '* Copyright All Reserved 2015-2023 NextPilot Development Team'
echo '******************************************************************'


################################################################
# 判断网络环境
################################################################
# 通过ping google.com自动判断网络环境
gitee=0
for arg in "$@"
do
    if [[ $arg == "--gitee" ]]; then
        gitee=1
    fi
done

if [ $gitee ]; then
    # echo "using gitee repo"
    pkg_url=https://gitee.com/RT-Thread-Mirror/packages.git
    env_url=https://gitee.com/RT-Thread-Mirror/env.git
    rtt_url=https://gitee.com/rtthread/rt-thread.git
else
    # echo "using github reop"
    pkg_url=https://github.com/RT-Thread/packages.git
    env_url=https://github.com/RT-Thread/env.git
    rtt_url=https://github.com/RT-Thread/rt-thread.git
fi


################################################################
# 判断是否ci环境使用
################################################################
ci=0
for arg in "$@"
do
    if [[ $arg == "--ci" ]]; then
        ci=1
    fi
done


################################################################
# 准备安装目录
################################################################
# env install path
rtt_dir=$HOME/.env
# shell file dirctory
shell_dir=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )

mkdir -p $rtt_dir
echo "export ENV_ROOT=$rtt_dir" > $rtt_dir/env.sh


echo
echo "################################################################"
echo  install ubuntu dependencies
echo "################################################################"
echo "Current Ubuntu Version `lsb_release -rs`"
# check ubuntu version
ubuntu_release="$(lsb_release -rs)"
if [[ "${ubuntu_release}" == "22.04" ]]; then
    tmp=1
elif [[ "${UBUNTU_RELEASE}" == "20.04" ]]; then
    tmp=1
else
    echo "only support Ubuntu 20.04/22.04"
    exit 1
fi

# update ubuntu
sudo apt update -y
sudo apt upgrade -y

if [[ $ci != 0 ]]; then
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
echo install python3 dependencies
echo "################################################################"
if [ $gitee == 1 ]; then
    echo "set global.index-url to https://mirrors.aliyun.com/pypi/simple/"
    python3 -m pip config set global.index-url https://mirrors.aliyun.com/pypi/simple/
fi
# upgrade pip
python3 -m pip install --user --upgrade pip
if [ -f ${shell_dir}/requirements_v3.txt ]; then
    echo "pip insall from ${shell_dir}/requirements_v3.txt"
    python3 -m pip install --user -r ${shell_dir}/requirements_v3.txt
else
    python3 -m pip install --user scons==4.4.0 requests psutil
    python3 -m pip install --user -U pyocd
fi
echo 'export PATH=`python3 -m site --user-base`/bin:$PATH' >> $rtt_dir/env.sh


echo
echo "################################################################"
echo install arm-gcc toolchain
echo "################################################################"
# arm-gcc下载地址为https://developer.arm.com/downloads/-/gnu-rm
gcc_arm_ver="10.3-2021.10"
gcc_arm_url=https://armkeil.blob.core.windows.net/developer/Files/downloads/gnu-rm/$gcc_arm_ver/gcc-arm-none-eabi-$gcc_arm_ver-x86_64-linux.tar.bz2

if [ -f $rtt_dir/toolchain/gcc-arm/gcc-arm-none-eabi-$gcc_arm_ver/bin/arm-none-eabi-gcc ]; then
    echo "found arm-none-eabi-gcc-${gcc_arm_ver} in $rtt_dir/toolchain/gcc-arm/gcc-arm-none-eabi-$gcc_arm_ver"
else
    # 下载
    if [ ! -f $rtt_dir/download/gcc-arm-none-eabi-$gcc_arm_ver-x86_64-linux.tar.bz2 ]; then
        echo "downloading arm-none-eabi-gcc-${gcc_arm_ver} from $gcc_arm_url"
        mkdir -p $rtt_dir/download
        wget -O $rtt_dir/download/gcc-arm-none-eabi-$gcc_arm_ver-x86_64-linux.tar.bz2 $gcc_arm_url
    fi
    # 解压
    echo "extracting to $rtt_dir/toolchain/gcc-arm/gcc-arm-none-eabi-$gcc_arm_ver"
    mkdir -p $rtt_dir/toolchain/gcc-arm
    tar -xjf $rtt_dir/download/gcc-arm-none-eabi-$gcc_arm_ver-x86_64-linux.tar.bz2 -C $rtt_dir/toolchain/gcc-arm
    gcc_arm_dir=$rtt_dir/toolchain/gcc-arm/gcc-arm-none-eabi-$gcc_arm_ver/bin
fi
#
echo "export PATH=$rtt_dir/toolchain/gcc-arm/gcc-arm-none-eabi-$gcc_arm_ver/bin:\$PATH" >> $rtt_dir/env.sh
echo "export RTT_EXEC_PATH=$rtt_dir/toolchain/gcc-arm/gcc-arm-none-eabi-$gcc_arm_ver/bin" >> $rtt_dir/env.sh
echo "export RTT_CC=gcc" >> $rtt_dir/env.sh


echo
echo "################################################################"
echo install rt-thread env
echo "################################################################"

# 下载rtt-packages到$HOME/.env/rtthread/pkg/packages
pkg_dir=$rtt_dir/rtthread/pkg/packages
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
echo 'source "$PKGS_DIR/Kconfig"' > $rtt_dir/rtthread/pkg/Kconfig
echo "export PKGS_DIR=$rtt_dir/rtthread/pkg" >> $rtt_dir/env.sh

# 下载rtt-env到$HOME/.env/scripts/
env_dir=$rtt_dir/rtthread/env
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
# echo "export PATH=$env_dir:\$PATH" >> $rtt_dir/env.sh


echo
echo "################################################################"
echo show toolchain info
echo "################################################################"
# load env.sh
source ~/.env/env.sh
echo "python: `python3 -V`, `which python3`"
echo "----------------------------------------------------------------"
echo "pip: `pip3 -V`"
echo "----------------------------------------------------------------"
echo "scons: `which scons`"
echo "`scons --version`"
echo "----------------------------------------------------------------"
echo "gcc: `which arm-none-eabi-gcc`"
echo "`arm-none-eabi-gcc --version`"
