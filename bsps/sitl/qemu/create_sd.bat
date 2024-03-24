@echo off

set path=C:\env-windows-v1.3.5\tools\fatdisk;%ENV_ROOT%\tools\fatdisk;%path%

rem create fatdisk.xml file
echo ^<?xml version="1.0" encoding="UTF-8"?^> > fatdisk.xml
echo ^<fatdisk^> >> fatdisk.xml
echo    ^<disk_size^>131072^</disk_size^> >> fatdisk.xml
echo    ^<sector_size^>512^</sector_size^> >> fatdisk.xml
echo    ^<root_dir^>sd^</root_dir^> >> fatdisk.xml
echo    ^<output^>sd.bin^</output^> >> fatdisk.xml
echo    ^<strip^>0^</strip^> >> fatdisk.xml
echo ^</fatdisk^> >> fatdisk.xml

if exist sd (
    fatdisk.exe
)
