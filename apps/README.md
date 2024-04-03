# 飞控应用程序

## 当前目录结构

|序号|目录|作用|说明|
|---|---|---|---|
|1|[airframe](./airframe/README.md)|机架设置，设置每一个机架独立的参数，切换机架时自动设置加载||
|2|bootloader|bl程序，目前支持 [px4](./bootloader/protocol/pixhawk/README.md) 和 [rtt](./bootloader/protocol/rtthread/rtt_ota.h) 两种协议||
|3|controller|飞行控制，包括逻辑、制导、位置/姿态控制等||
|4|estimator|组合导航，包含传感器的补偿、滤波、表决，以及EKF姿态和位置估计||
|5|example|提供了一些简单例子||
|7|libraries|代码中所用到的lib||
|8|peripheral|飞控外设驱动，包括传感器、PWM、发动机、吊舱等||
|9|simulation|虚拟飞行仿真，包括六自由度、传感器模拟、故障注入等||
|10|telemetry|链路通信相关，包括mavlink、uavcan等||
|11|universal|飞控框架相关，包括param、uorb、romfs、dataman、logger、relpay等||

## 如何新增模块
