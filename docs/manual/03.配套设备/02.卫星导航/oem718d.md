# Novatel OEM718D

## 简介

OEM718d 是由Novatel推出的全系统多频单机测向卫星接收板卡，支持双频双天线信号输入，接收机数据稳定传输，卓越的RTK差分定位、测向功能。

**产品特点**

采用555跟踪通道设计，支持所有GNSS系统，包括GPS、GLONASS、BEIDOU、Galileo。小尺寸外形设计、全新硬件平台、高抗干扰算法，支持双天线输入，为用户提供厘米级差分定位和高精度航向测量，低功耗、重量轻、接口灵活等特点尤其适合各领域集成应用。

- 拥有555个通道，支持全系统、多频测向和定位解算
- 支持串口、USB口、CAN口以及基于Web的网络接口
- 先进的可视化抗干扰功能
- 支持RTK、GLIDE 和STEADYLINE® 功能选项
- 易于集成，体积小，20g 高抗振性能
- 支持 SPAN GNSS+INS 组合导航技术

**应用领域**

无人机定向、船载定向、车载定向、GNSS+INS 组合导航、机械控制、精准控制、工业控制等。

## 指标参数

![](./oem718d-specifications.png)

## 消息定义

每条二进制消息由帧头+数据+校验组成，其中header固定28个字节，CRC为4个字节。

|变量|长度|说明|
|---|---|---|
|header|28|header长度28个字节，同步字是`0xAA 0x44 0x12`|
|data|x|数据|
|crc|4|CRC校验|

## 常用指令

|指令|举例|作用|
|---|---|---|
|freset [target]|freset standard|重置|
|saveconfig||保存当前配置|
|unlogall [port] [held]|unlog com1|禁用port的所有输出，如果不指定port，则禁用所有port的所有输出|
|unlog [port] message|unlog com1 bestposa|禁用指定port的message消息输出|
|movingbasestation enable/disabe||设置为移动基准站|
|interfacemode [port] rxtype txtype [responses]|interfacemode com2 novatel novatel on|设置端口发送和接收模式
|serialconfig [port] baud [parity] [databits] [stopbits][handshaking] [break]||设置串口的波特率等|
|serialprotocol port protocol||设置串口协议是0=RS232还是1=RS422|
|log comconfig||输出串口配置|
|log version||输出版本信息|
|log [port] message trigger||配置输出哪些消息|

### 配置串口

```shell
# 设置波特率等
serialconfig com1 115200
# 设置为RS422
serialprotocol com1 rs422
# 设置串口输入输出类型
interfacemode com1 novatel/rtcm  novatel/rtcm on
# 查询串口配置
log comconfig once
```

### 消息输出

```
# 禁用所有输出
unlogall

# 以10hz输出ASCII形式的bestpos
log bestposa ontime 0.1

# 保存配置
saveconfig
```

## RTK模式

> 一般建议：一个串口用来输入RTCM校正数据，一个串口输出POS定位数据

### 天空端

RTK模式，天空端一般需要输出以下信息：

- BESTPOS，飞机位置`LOG BESTPOSB ONTINE 0.05`
- BESTVEL，飞机速度`LOG BESTVELB ONTINE 0.05`
- PSRDOP，DOP定位精度`LOG PSRDOPB，DOP定位精度 ONTINE 0.1`
- DUALANTENNAHEADING，双天线夹角`LOG DUALANTENNAHEADINGB ONTIME 0.1`
- TIME，时间戳`LOG TIMEB ONTIME 0.1`

#### 单串口模式

只用串口2，输入RTCM校正数据（不支持配置指令）并输出POS定位数据

```shell
unlogall com2

serialconfig com2 115200 n 8 1 n on
interfacemode com2 rtcmv3 novatel on

log com2 bestposb ontine 0.05
log com2 bestvelb ontine 0.05
log com2 psrdopb ontine 0.1
log com2 dualantennaheadingb ontime 0.1
log com2 timeb 0.1

saveconfig
```

#### 双串口模式

串口1输入RTCM校正数据（不支持配置指令），串口2输出定位数据

```shell
unlogall com1
unlogall com2

serialconfig com1 115200 n 8 1 n on
interfacemode com1 rtcmv3 novatel on

serialconfig com2 115200 n 8 1 n on
interfacemode com2 none novatel on

log com2 bestposb ontine 0.05
log com2 bestvelb ontine 0.05
log com2 psrdopb ontine 0.1
log com2 dualantennaheadingb ontime 0.1
log com2 timeb 1

saveconfig
```

### 地面端

RTK模式，地面端一般需要输出以下消息：

- RTCMv3，必须，发给天空端用于进行RTK校正，`GENERATERTKCORRECTIONS rtcmv3 com2`等效于下面几条指令

```shell
# GPS L1/L2 basic observables, extended
RTCM1004 ontime 1
# GLONASS L1/L2 basic observables, extended
RTCM1012 ontime 1
# Stationary RTK Base Station ARP with Antenna Height
RTCM1006 ontime 10
# Extended Antenna Reference Station Description and Serial Number
RTCM1008 ontime 10
# Receiver and antenna descriptors
RTCM1033 ontime 10
```

- BESTPOS，非必须，地面站位置，用于地面站显示或发给飞机`LOG BESTPOSB ONTIME 1.0`
- BESTVEL，非必须，地面站速度，用于地面站显示或发给飞机`LOG BESTVELB ONTIME 1.0`
- DUALANTENNAHEADING，非必须，双天线角度，用于地面站显示或发给飞机`LOG DUALANTENNAHEADING ONTIME 1.0`
- TIME，时间戳，非必须，用于地面站显示或时间同步`LOG TIMEB ONTINE 1.0`

#### 单串口模式

串口2，同时输出RTCM和POS数据

```shell
unlogall com2

posave on 0.01

serialconfig com2 115200 n 8 1 n on
intferacemode com2 novatel novatel on
# generatertkcorrections rtcmv3 com2
log com2 rtcm1075 ontime 1
log com2 rtcm1085 ontime 1
log com2 rtcm1125 ontime 1
log com2 rtcm1006 ontime 10
log com2 rtcm1033 ontime 10

log com2 bestposb ontime 1.0
log com2 bestvelb ontime 1.0
log com2 dualantennaheadingb ontime 1.0
log com2 timeb ontine 1.0

saveconfig
```

#### 双串口模式

串口输出POS定位数据，串口2输出RTCM补偿数据

```shell
unlogall com1
unlogall com2

posave on 0.01

serialconfig com1 115200 n 8 1 n on
intfacemode com1 novatel novatel on
log com1 bestposb ontime 1.0
log com1 bestvelb ontime 1.0
log com1 dualantennaheadingb ontime 1.0
log com1 timeb ontine 1.0

serialconfig com2 115200 n 8 1 n on
intferacemode com2 novatel rtcmv3 on
# generatertkcorrections rtcmv3 com2
log com2 rtcm1075 ontime 1
log com2 rtcm1085 ontime 1
log com2 rtcm1125 ontime 1
log com2 rtcm1006 ontime 10
log com2 rtcm1033 ontime 10


saveconfig
```

### 调试记录

```
LOG RXSTATUSB ONCHANGED
LOG RAWEPHEMB ONNEW
LOG GLORAWEPHEMB ONNEW
LOG QZSSRAWEPHEMB ONNEW
LOG BDSRAWNAVSUBFRAMEB ONNEW
LOG GALFNAVRAWEPHEMERISB ONNEW
LOG GALINAVRAWEPHEMERISB ONNEW
LOG RANGEB ONTIME 1.0
LOG BESTPOSB ONTIME 1.0
LOG RXCONFIGB ONCE
LOG VERSIONB ONCE
LOG TRACKSTATB ONTIME 1.0
LOG RTKPOSB ONTIME 1.0
LOG MATCHEDPOSB ONNEW
LOG MATCHEDSATSB ONNEW
LOG RTKSATSB ONTIME 1.0
LOG PSRPOSB ONTIME 1.0
LOG RAWALMB ONNEW
LOG IONUTCB ONNEW
LOG GLORAWALMB ONNEW
LOG GLOCLOCKB ONNEW
LOG PASSTHROUGHB ONNEW
LOG CLOCKMODELB ONTIME 1.0
LOG REFSTATIONB ONNEW
LOG RTKVELB ONTIME 1.0
```

## ALGIN模式

### 天空端

```
unlogall

serialconfig com2 115200
interfacemode com2 novatelx novatel on

serialconfig com1 115200
interfacemode com2 novatel novatel on
LOG MASTERPOSA ONCHANGED
LOG ROVERPOSA ONCHANGED
LOG ALIGNDOPA ONNEW
log com2 headingextb onchanged

saveconfig
```

### 地面端

配置ALGIN地面端并发送ALGIN校准数据
`GENERATEALIGNCORRECTIONS port [baud] [obsreqrate] [refextreqrate] [interfacemode]`

该指令等效于

```
unlogall [port]
fix none
movingbasestation enable
interfacemode [port] novatel rtca
serialconfig [port] [baud] N 8 1 N ON
log [port] rtcaobs3 ontime [rate = 1/ obsreqrate]
log [port] rtcarefext ontime [rate = 1/ refextreqrate]
```

完整指令

```
unlogall

movingbasestation enable

serialconfig com2 115200
interfacemode com2 novatel novatelx on
log com2 novatelxobs ontime 1
log com2 novatelxref ontime 1

serialconfig com1 115200 n 8 1 n on
intfacemode com1 novatel novatel on
log com1 bestposb ontime 1.0
log com1 bestvelb ontime 1.0
log com1 dualantennaheading ontime 1.0
log com1 timeb ontine 1.0

saveconfig
```

### 调试记录

```
LOG RXSTATUSB ONCHANGED
LOG RAWEPHEMB ONNEW
LOG GLORAWEPHEMB ONNEW
LOG GALINAVRAWEPHEMERISB ONNEW
LOG BDSEPHEMERISB ONNEW
LOG BESTPOSB ONTIME 1
LOG RANGEB ONTIME 1
LOG RXCONFIGA ONCE
LOG VERSIONA ONCE
LOG LOGLISTA ONCE
LOG PORTSTATSA ONTIME 10
LOG ALIGNBSLNENUB ONNEW
LOG ALIGNBSLNXYZB ONNEW
LOG ALIGNDOPB ONNEW
LOG HEADING2B ONNEW
LOG MASTERPOSB ONNEW
LOG ROVERPOSB ONNEW (This log can only be output if you have Y model ALIGN)
LOG HEADINGSATSA ONNEW (This log can only be output on the ALIGN rover)
LOG DUALANTENNAHEADINGB ONTIME 1.0
LOG TRACKSTATB_1 ONTIME 1 (Dual antenna receivers only)
LOG RANGEB_1 ONTIME 1 (dual antenna receivers only)
```

## 参考链接

诺瓦泰OEM7 PDF帮助文档：[https://docs.novatel.com/OEM7/Content/Appendix/PDFDocuments.htm](https://docs.novatel.com/OEM7/Content/Appendix/PDFDocuments.htm)

ALIGN技术相关支持材料：[https://novatel.com/support/firmware-options-pc-software/align](https://novatel.com/support/firmware-options-pc-software/align)
