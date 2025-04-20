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

sudo apt update -y
sudo apt upgrade -y
sudo apt install -y \
    gcc gcc-arm-none-eabi binutils-arm-none-eabi gdb-multiarch \
    libncurses5 libncurses5-dev libncursesw5-dev \
    qemu qemu-system-arm

python3 -m pip install -r tools/install/requirements_v3.txt

# echo "RTT_EXEC_PATH=/opt/gcc-arm-none-eabi-10-2020-q4-major/bin" >>$GITHUB_ENV
# echo "RTT_CC=gcc" >>$GITHUB_ENV

export RTT_EXEC_PATH=/usr/bin/
export RTT_CC=gcc

scons -C bsps/sitl/qemu default -j$(nproc)
