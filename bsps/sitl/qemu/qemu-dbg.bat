@echo off
if exist sd.bin goto run
qemu-img create -f raw sd.bin 512M
:run
qemu-system-arm -M vexpress-a9 -kernel build/sitl-qemu-default.elf -display none -serial stdio -sd sd.bin -S -s

rem -net nic -net tap,ifname=tap
