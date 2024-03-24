@echo off

set path="C:\Program Files\7-Zip";%path%

if exist sd.bin (
    7z.exe x "sd.bin" -o"./sd"
)
