# 飞行控制

该文件夹下主要存放 **飞行控制** 相关功能模块。

## 遥控输入

- manual_control，遥控器动作和选择等
- rc_update，将遥控器原始值rc_input转为manual_control和manual_switches

## 通用模块

- commander，飞行逻辑，包括模式切换、加锁解锁、安全保护等逻辑
- navigator，自主导航，包括任务、围栏、起降等自主任务模块
- land_detector，着陆检测

## 多旋翼

- flight_mode_manager，多旋翼飞行模式管理，并平滑位置指令
- mc_pos_control，多旋翼位置控制
- mc_hover_thrust_estimator，多旋翼悬停油门估计
- mc_att_control，多旋翼姿态控制
- mc_rate_control，多旋翼角速率控制
- mc_autotune_attitude_control，多旋翼内环自动调参

## 固定翼

- fw_pos_control，固定翼位置控制
- fw_att_control，固定翼姿态控制
- fw_rate_control，固定翼角速率控制
- fw_auotune_attitude_control，固定翼内环自动调参

## 垂直起降

- vtol_att_control，垂起姿态控制，支持标准、尾座、倾转等三种垂直无人机
