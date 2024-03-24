# 模拟器仿真

在模拟器仿真中，可选用多种不同的导航算法进行模拟飞行仿真，其指令分别如下：

1. 启用ekf2_fake线程仿真：

   `scons ins_default -j8`

2. 启用互补滤波线程仿真：

   `scons ins_cf -j8`

3. 启用ekf2仿真：

   `scons ins_ekf2 -j8`
   
**注**：验证除导航的数据外的问题时，建议使用ekf2_fake作为导航首选，方便排查问题。
# 备注

1. 运行互补滤波算法需要sensors_vote.cpp
3. 运行真实的EKF2需要运行sensors_vote.cpp
4. 运行EKF2_FAKE需要运行sensors_vote.cpp
5. 运行声光电的惯导算法不需要运行sensors_vote.cpp

|       导航算法       |  互补滤波    |   ekf2   |  ekf2_fake    |  声光电   |
| :------------------: | :--: | :--: | :--: | :--: |
| 是否依赖sensors_vote线程 | ★ | ★ | ★ | 未维护 |

