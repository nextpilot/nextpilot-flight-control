# Vehicle Types & Setup

PX4 supports numerous types of vehicles, including different configurations of multicopters, planes, VTOL vehicles, ground vehicles, and so on.

This section explains how to assemble, configure, and tune PX4-based autopilot systems for each type (much of this setup is common to all types).

!!! info
    [Basic Concepts > Drone Types](../01.基本概念/basic_concepts.md#drone-types) provides high level information about the types of vehicles and the use cases for which they are best suited.

## Supported Vehicles

The frame types that have a maintainer and are well tested and supported are:

- [Multicopters](../09.机型调参/frames_multicopter/index.md) (tri-, quad-, hexa-, octa-, and even [omnicopter](../09.机型调参/frames_multicopter/omnicopter.md) vehicles)
- Planes (Fixed-Wing(../09.机型调参/frames_plane/index.md)
- [VTOL](../09.机型调参/frames_vtol/index.md): [Standard VTOL](../09.机型调参/frames_vtol/standardvtol.md), [Tailsitter VTOL](../09.机型调参/frames_vtol/tailsitter.md), [Tiltrotor VTOL](../09.机型调参/frames_vtol/tiltrotor.md)

## Experimental Vehicles

Experimental frames are those vehicle types that:

- Do not have a maintainer.
- Are not regularly tested by the core development team.
- May not be tested in CI.
- May lack features required for production-ready vehicles.
- May not support some common vehicle configurations for the vehicle type.

The following vehicle types are considered experimental:

- [Airships](../09.机型调参/frames_airship/index.md)
- [Autogyros](../09.机型调参/frames_autogyro/index.md)
- [Balloons](../09.机型调参/frames_balloon/index.md)
- [Helicopter](../09.机型调参/frames_helicopter/index.md)
- [Rovers](../09.机型调参/frames_rover/index.md)
- [Submarines](../09.机型调参/frames_sub/index.md)

!!! info
    Maintainer volunteers, contribution of new features, new frame configurations, or other improvements would all be very welcome!

## Other Vehicles

The complete set of supported vehicle types and their configurations can be found in the [Airframes Reference](../09.机型调参/01.airframe_reference.md).
