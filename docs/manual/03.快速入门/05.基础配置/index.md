# 标准配置

This section covers the common software configuration and calibration needed for most PX4 vehicles.

You must first [load firmware and select your vehicle frame/type](#firmware-vehicle-selection).
Most other steps can be done out of order, except for [tuning](#tuning), which must be done last.

## Preconditions

Before starting you should [Download QGroundControl](https://qgroundcontrol.com/downloads/) and install it on your **desktop** computer.
Then open the QGC application menu ("Q" icon in the top-left corner) and choose **Vehicle Setup** in the _Select Tool_ popup:

![QGC Main Menu Popup: highlighting Vehicle Setup](../../assets/qgc/setup/menu_setup.png)

## Configuration Steps

### Firmware/Vehicle Selection

- [Loading Firmware](./01.upload-firmware.md)
- [Vehicle (Frame) Selection](./02.select-airframe.md)

### Motor/Actuator Setup

- [ESC Calibration](../19.高级调参/esc_calibration.md)
- [Actuator Configuration and Testing](./actuators.md)

### Sensor Calibration

- [Sensor Orientation](./05.flight_controller_orientation.md)
- [Magnetometer (Compass)](./06.compass.md)
- [Gyroscope](./09.gyroscope.md)
- [Accelerometer](./07.accelerometer.md)
- [Level Horizon](./11.level_horizon_calibration.md)
- [Airspeed](./12.airspeed.md) (Fixed-wing/VTOL only)

::: info
Setup for these and other sensors is located in [Sensor Hardware & Setup](../21.周边设备/04.传 感 器/index.md).
:::

### Manual Control Setup

Radio Control:

- [Radio Controller (RC) Setup](./radio.md)
- [Flight Mode Configuration](./flight_mode.md)

Joystick/GamePad:

- [Joystick Setup](./joystick.md)

### Safety Configuration

- [Battery Estimation Tuning](./battery.md) (requires [Power Module](../21.周边设备/12.电源模块/README.md))
- [Safety Configuration (Failsafes)](./99.safety.md)

### Tuning

- [Autotune](./98.autotune.md) (Recommended on vehicles and frames that support it)

## Video Guide

The video below shows most of the calibration process (it uses an older version of _QGroundControl_, but most of the process is unchanged).

@[youtube](https://youtu.be/91VGmdSlbo4)

## Support

If you need help with the configuration you can ask for help on the [QGroundControl Support forum](https://discuss.px4.io//c/qgroundcontrol/qgroundcontrol-usage).

## See Also

- [QGroundControl > Setup](https://docs.qgroundcontrol.com/master/en/qgc-user-guide/setup_view/setup_view.html)
- [Flight Controller Peripherals](../21.周边设备/04.传 感 器/index.md) - Setup specific sensors, optional sensors, actuators, and so on.
- [Advanced Configuration](../19.高级调参/index.md) - Factory/OEM calibration, configuring advanced features, less-common configuration.
- Vehicle-Centric Config/Tuning:

  - [Multicopter Config/Tuning](../09.机型调参/11.多旋翼/index.md)
  - [Helicopter Config/Tuning](../09.机型调参/14.直升机/index.md)
  - [Fixed-wing Config/Tuning](../09.机型调参/12.固定翼/index.md)
  - [VTOL Config/Tuning](../09.机型调参/13.垂直起/index.md)
