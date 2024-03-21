@echo off

if exist sd.bin goto run
qemu-img create -f raw sd.bin 512M
:run
qemu-system-arm -M vexpress-a9 -kernel build/sitl-qemu-default.bin -display none -serial stdio -sd sd.bin
@REM -net user -net nic
@REM -net nic -net tap,ifname=tap
