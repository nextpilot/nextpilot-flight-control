# 组合导航

该文件夹用于存放 **组合导航** 相关功能模块，包括传感器表决、滤波、补偿、校准、振动频率计算、零偏估计，以及EKF融合算法等

## 传感器预处理模块

- gyro_calibration，陀螺仪校准
- gyro_fft，使用快速傅里叶变换，根据gyro数据在线计算无人机的振动频率，用于滤波
- airspeed_selector，空速计表决和有效性处理
- mag_bias_estimator，磁罗盘零偏估计
- temperature_compensation，温度补偿
- sensor

## 便捷组合导航算法

- local_position_estimator，简单ekf估计位置
- attitude_estimator_q，采用互补滤波估计姿态

## ekf2高级组合导航

- ekf2
