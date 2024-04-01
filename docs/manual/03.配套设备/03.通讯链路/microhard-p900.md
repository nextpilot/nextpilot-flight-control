# MicroHard P900

## 简介

![](./microhard-p900-top.png)

[Microhard Pico系列电台](http://microhardcorp.com/P900.php)集成了P900射频模块。这是一款尺寸相对较小和低成本的无线电台，支持包括点对点、点对多点和网状模式在内的模式。具有可配置的功率输出，也可以配置为forward error correction。

## 参数指标
![](microhard-p900-specifications.png)

|参数|指标|说明|
|---|---|---|
|频率范围|902-928 MHz|
|发射功率|100mW ~ 1W(20-30dBm)|
|传输距离|40英里（60公里）|
|串口速率|300bps ~ 230kbps|
|无线速率|57.6kbps ~ 276 kbps|

## 电台配置

在`点对点`模下，一般地面端配置为`PP master`，天空端配置为`PP remote`，同时要求`Network Address、Wireless Link Rate、Serial Buad Rate`等保持一致。

在`Mesh或点对多`模式，配置请参考[Pico-Series-P900.Operating-Manual.v1.8.7](./Pico-Series-P900.Operating-Manual.v1.8.7.pdf)。

### 工具配置

Microhard系列电台可以使用PicoConfig.exe进行配置，点击这里下载[PicoConfig-1.7.zip](./PicoConfig-1.7.zip)。

![](./pico_configurator.png)

### 手动配置

|指令|作用|
|---|---|
|+++|从DATA进入COMMAND模式，此时可以配置P900|
|ATA|从COMMAND进入DATA模式，正常数据传输|
|AT&WA|保存配置|
|AT&V|显示配置|
|AT&F1|Mesh Primary Coordinator|
|AT&F2|Mesh Remote|
|AT&F3|Mesh Secondary Coordinator|
|AT&F7|PMP Master|
|AT&F8|PMP Slave|
|AT&F9|PMP Repeater|
|AT&F10|点对点MASTER默认配置|
|AT&F11|点对点SLAVE默认配置|
|AT&F12|点对点Repeater中继默认配置|
|AT&F13|PMP Master 57K|
|AT&F14|PMP Slave 57K|


## 常见问题

1. Microhard P900最远传输距离是多少？
> 高增益天线，无遮挡可达60公里。

1. Microhard P900最小功率和最大功率是多少？
> 最小100mW，最大1W

1. Microhard P900空中最小速率和最大速率是多少？
> 最小57.6Kbps，最大276Kbps

1. Microhard P900串口最小速率和最大速率是多少？
> 最小300bps，最大230400bps

1. Microhard P900工作温度范围是多少？
> -40摄氏度到85摄氏度

1. Microhard P900重量是多少？
> 模块重5克，成品重120克

1. Microhard P900体积是多少？
> 模块：26.5mm x 33mm x 3.5mm，成品：46mm x 66mm x 25mm

1. Microhard P900频率范围是多少？
> 902-928MHz

1. Microhard P900如何设置2线RS485？
> 添加ATS142=1，AT&K1这2条指令。硬件连接是：DB9接口的PIN2&PIN7短接作为DATA-；PIN3&PIN8短接作为DATA+，**手册中描述的有问题，不能正常工作**

1. Microhard P900如何设置4线RS485？
> 添加ATS142=2，AT&K1这2条指令。硬件连接是：DB9接口的PIN2：TX-；PIN3：RX+；PIN7：RX-；PIN8：TX+

1. Microhard P900主板或者盒装品工作电压范围是多少？
> 直流9-30V

1. Microhard P900点对多通信的时，如何知道数据是从哪一个电台发给主的？
> 主设置ATS153=1, 启用包头，含从模块的地址。`如接收的数据包00 01 00 F0 49 00 84 82 31
00 01 //数据包长度
00 F0 49 00 84 82 //从模块的MAC地址
31 //数据`

1. Microhard P900支持Modbus吗？
> 支持modbus RTU。 设置ATS217=1

1. Microhard P900在自己设计的主板上经常出现断开再连接是什么原因？
> 通常是电流不足导致，发射电流需求是1000mA to 1400mA


## 参考链接

1. Microhard P900官网：[http://microhardcorp.com/P900.php](http://microhardcorp.com/P900.php)

1. 国内代理商技术支持：[http://www.isnmp.com/microhard-p900-resources/](http://www.isnmp.com/microhard-p900-resources/)
