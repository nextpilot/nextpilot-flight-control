# 飞行仿真

该文件夹存放了 **飞行仿真** 相关模块，包括飞行动力学（六自由度、空气动力、旋翼动力、尾推动力等）、传感器、作动器等模拟。

飞行仿真支持 **SIH模式** （半实物仿真，六自由度和飞控算法运行在STM32硬件上）和 **SITL模式** （纯软件仿真，六自由度和飞控算法运行在在QEMU模拟器中）

# 传感器模拟

- battery_simulator
- sensor_airspeed_sim
- sensor_baro_sim
- sensor_gps_sim
- sensor_mag_sim

# 作动器模拟

- pwm_out_sim

# 飞行动力学

- simulator_sih，飞行动力学仿真，根据

# 外部模拟器

- simulator_mavlink
- gz_bridge
