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

# 在ci环境下已经安装了以下工具：
# - python 3.11
# - git
#
# 还需要安装以下工具：
# - gcc-arm
# - qemu

#################################################################
# install system tools
#################################################################
sudo apt update -y
sudo apt upgrade -y
sudo apt install -y \
    gcc gdb-multiarch \
    gcc-arm-none-eabi binutils-arm-none-eabi \
    libncurses5 libncurses5-dev libncursesw5-dev \
    qemu qemu-system-arm

#################################################################
# install pip package
#################################################################
python3 -m pip install -r tools/install/requirements_v3.txt

#################################################################
# display toolchain information
#################################################################
echo
echo "python: $(python3 -V), $(which python3)"
echo "----------------------------------------------------------------"
echo "pip: $(pip3 -V)"
echo "----------------------------------------------------------------"
echo "scons: $(which scons)"
echo "$(scons --version)"
echo "----------------------------------------------------------------"
echo "gcc: $(which arm-none-eabi-gcc)"
echo "$(arm-none-eabi-gcc --version)"

#################################################################
# buld sitl/qemu board
#################################################################
# echo "RTT_EXEC_PATH=/opt/gcc-arm-none-eabi-10-2020-q4-major/bin" >>$GITHUB_ENV
# echo "RTT_CC=gcc" >>$GITHUB_ENV
export RTT_EXEC_PATH=/usr/bin/
export RTT_CC=gcc
scons -C bsps/sitl/qemu default -j$(nproc)
