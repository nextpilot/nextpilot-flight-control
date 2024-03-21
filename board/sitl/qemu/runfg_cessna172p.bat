@echo off

cd C:\Program Files\FlightGear-2020.3

SET FG_ROOT=C:\Program Files\FlightGear-2020.3\data

START .\\bin\fgfs.exe --fdm=null --native-fdm=socket,in,25,192.168.137.1,5502, udp --native-ctrls=socket,out,30,localhost,5505, udp --enable-terrasync --prop:/sim/rendering/shaders/quality-level=0 --aircraft=c172p --fog-fastest --disable-clouds --start-date-lat=2022:12:01:12:00:00 --disable-sound --in-air --airport=KSFO --runway=10L --altitude=7224 --heading=113 --offset-distance=4.72 --offset-azimuth=0  
