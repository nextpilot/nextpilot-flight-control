
/**
 * Airframe selection
 *
 * Defines which mixer implementation to use.
 * Some are generic, while others are specifically fit to a certain vehicle with a fixed set of actuators.
 * 
 * 'Custom' should only be used if noting else can be used.
 * 
 *
 * @group Geometry
 * @value 0 Multirotor
 * @value 1 Fixed-wing
 * @value 2 Standard VTOL
 * @value 3 Tiltrotor VTOL
 * @value 4 Tailsitter VTOL
 * @value 5 Rover (Ackermann)
 * @value 6 Rover (Differential)
 * @value 7 Motors (6DOF)
 * @value 8 Multirotor with Tilt
 * @value 9 Custom
 * @value 10 Helicopter
 */
PARAM_DEFINE_INT32(CA_AIRFRAME, 0);

/**
 * Control allocation method
 *
 * Selects the algorithm and desaturation method.
 * If set to Automtic, the selection is based on the airframe (CA_AIRFRAME).
 * 
 *
 * @group Geometry
 * @value 0 Pseudo-inverse with output clipping
 * @value 1 Pseudo-inverse with sequential desaturation technique
 * @value 2 Automatic
 */
PARAM_DEFINE_INT32(CA_METHOD, 2);

/**
 * Bidirectional/Reversible motors
 *
 * Configure motors to be bidirectional/reversible. Note that the output driver needs to support this as well.
 * 
 *
 * @group Geometry
 * @bit 0 Motor 1
 * @bit 1 Motor 2
 * @bit 2 Motor 3
 * @bit 3 Motor 4
 * @bit 4 Motor 5
 * @bit 5 Motor 6
 * @bit 6 Motor 7
 * @bit 7 Motor 8
 * @bit 8 Motor 9
 * @bit 9 Motor 10
 * @bit 10 Motor 11
 * @bit 11 Motor 12
 * @min 0
 * @max 4095
 */
PARAM_DEFINE_INT32(CA_R_REV, 0);

/**
 * Motor 0 slew rate limit
 *
 * Minimum time allowed for the motor input signal to pass through
 * the full output range. A value x means that the motor signal
 * can only go from 0 to 1 in minimum x seconds (in case of
 * reversible motors, the range is -1 to 1).
 * 
 * Zero means that slew rate limiting is disabled.
 * 
 *
 * @group Geometry
 * @decimal 2
 * @increment 0.01
 * @min 0
 * @max 10
 */
PARAM_DEFINE_FLOAT(CA_R0_SLEW, 0.0);

/**
 * Motor 1 slew rate limit
 *
 * Minimum time allowed for the motor input signal to pass through
 * the full output range. A value x means that the motor signal
 * can only go from 0 to 1 in minimum x seconds (in case of
 * reversible motors, the range is -1 to 1).
 * 
 * Zero means that slew rate limiting is disabled.
 * 
 *
 * @group Geometry
 * @decimal 2
 * @increment 0.01
 * @min 0
 * @max 10
 */
PARAM_DEFINE_FLOAT(CA_R1_SLEW, 0.0);

/**
 * Motor 2 slew rate limit
 *
 * Minimum time allowed for the motor input signal to pass through
 * the full output range. A value x means that the motor signal
 * can only go from 0 to 1 in minimum x seconds (in case of
 * reversible motors, the range is -1 to 1).
 * 
 * Zero means that slew rate limiting is disabled.
 * 
 *
 * @group Geometry
 * @decimal 2
 * @increment 0.01
 * @min 0
 * @max 10
 */
PARAM_DEFINE_FLOAT(CA_R2_SLEW, 0.0);

/**
 * Motor 3 slew rate limit
 *
 * Minimum time allowed for the motor input signal to pass through
 * the full output range. A value x means that the motor signal
 * can only go from 0 to 1 in minimum x seconds (in case of
 * reversible motors, the range is -1 to 1).
 * 
 * Zero means that slew rate limiting is disabled.
 * 
 *
 * @group Geometry
 * @decimal 2
 * @increment 0.01
 * @min 0
 * @max 10
 */
PARAM_DEFINE_FLOAT(CA_R3_SLEW, 0.0);

/**
 * Motor 4 slew rate limit
 *
 * Minimum time allowed for the motor input signal to pass through
 * the full output range. A value x means that the motor signal
 * can only go from 0 to 1 in minimum x seconds (in case of
 * reversible motors, the range is -1 to 1).
 * 
 * Zero means that slew rate limiting is disabled.
 * 
 *
 * @group Geometry
 * @decimal 2
 * @increment 0.01
 * @min 0
 * @max 10
 */
PARAM_DEFINE_FLOAT(CA_R4_SLEW, 0.0);

/**
 * Motor 5 slew rate limit
 *
 * Minimum time allowed for the motor input signal to pass through
 * the full output range. A value x means that the motor signal
 * can only go from 0 to 1 in minimum x seconds (in case of
 * reversible motors, the range is -1 to 1).
 * 
 * Zero means that slew rate limiting is disabled.
 * 
 *
 * @group Geometry
 * @decimal 2
 * @increment 0.01
 * @min 0
 * @max 10
 */
PARAM_DEFINE_FLOAT(CA_R5_SLEW, 0.0);

/**
 * Motor 6 slew rate limit
 *
 * Minimum time allowed for the motor input signal to pass through
 * the full output range. A value x means that the motor signal
 * can only go from 0 to 1 in minimum x seconds (in case of
 * reversible motors, the range is -1 to 1).
 * 
 * Zero means that slew rate limiting is disabled.
 * 
 *
 * @group Geometry
 * @decimal 2
 * @increment 0.01
 * @min 0
 * @max 10
 */
PARAM_DEFINE_FLOAT(CA_R6_SLEW, 0.0);

/**
 * Motor 7 slew rate limit
 *
 * Minimum time allowed for the motor input signal to pass through
 * the full output range. A value x means that the motor signal
 * can only go from 0 to 1 in minimum x seconds (in case of
 * reversible motors, the range is -1 to 1).
 * 
 * Zero means that slew rate limiting is disabled.
 * 
 *
 * @group Geometry
 * @decimal 2
 * @increment 0.01
 * @min 0
 * @max 10
 */
PARAM_DEFINE_FLOAT(CA_R7_SLEW, 0.0);

/**
 * Motor 8 slew rate limit
 *
 * Minimum time allowed for the motor input signal to pass through
 * the full output range. A value x means that the motor signal
 * can only go from 0 to 1 in minimum x seconds (in case of
 * reversible motors, the range is -1 to 1).
 * 
 * Zero means that slew rate limiting is disabled.
 * 
 *
 * @group Geometry
 * @decimal 2
 * @increment 0.01
 * @min 0
 * @max 10
 */
PARAM_DEFINE_FLOAT(CA_R8_SLEW, 0.0);

/**
 * Motor 9 slew rate limit
 *
 * Minimum time allowed for the motor input signal to pass through
 * the full output range. A value x means that the motor signal
 * can only go from 0 to 1 in minimum x seconds (in case of
 * reversible motors, the range is -1 to 1).
 * 
 * Zero means that slew rate limiting is disabled.
 * 
 *
 * @group Geometry
 * @decimal 2
 * @increment 0.01
 * @min 0
 * @max 10
 */
PARAM_DEFINE_FLOAT(CA_R9_SLEW, 0.0);

/**
 * Motor 10 slew rate limit
 *
 * Minimum time allowed for the motor input signal to pass through
 * the full output range. A value x means that the motor signal
 * can only go from 0 to 1 in minimum x seconds (in case of
 * reversible motors, the range is -1 to 1).
 * 
 * Zero means that slew rate limiting is disabled.
 * 
 *
 * @group Geometry
 * @decimal 2
 * @increment 0.01
 * @min 0
 * @max 10
 */
PARAM_DEFINE_FLOAT(CA_R10_SLEW, 0.0);

/**
 * Motor 11 slew rate limit
 *
 * Minimum time allowed for the motor input signal to pass through
 * the full output range. A value x means that the motor signal
 * can only go from 0 to 1 in minimum x seconds (in case of
 * reversible motors, the range is -1 to 1).
 * 
 * Zero means that slew rate limiting is disabled.
 * 
 *
 * @group Geometry
 * @decimal 2
 * @increment 0.01
 * @min 0
 * @max 10
 */
PARAM_DEFINE_FLOAT(CA_R11_SLEW, 0.0);

/**
 * Servo 0 slew rate limit
 *
 * Minimum time allowed for the servo input signal to pass through
 * the full output range. A value x means that the servo signal
 * can only go from -1 to 1 in minimum x seconds.
 * 
 * Zero means that slew rate limiting is disabled.
 * 
 *
 * @group Geometry
 * @decimal 2
 * @increment 0.05
 * @min 0
 * @max 10
 */
PARAM_DEFINE_FLOAT(CA_SV0_SLEW, 0.0);

/**
 * Servo 1 slew rate limit
 *
 * Minimum time allowed for the servo input signal to pass through
 * the full output range. A value x means that the servo signal
 * can only go from -1 to 1 in minimum x seconds.
 * 
 * Zero means that slew rate limiting is disabled.
 * 
 *
 * @group Geometry
 * @decimal 2
 * @increment 0.05
 * @min 0
 * @max 10
 */
PARAM_DEFINE_FLOAT(CA_SV1_SLEW, 0.0);

/**
 * Servo 2 slew rate limit
 *
 * Minimum time allowed for the servo input signal to pass through
 * the full output range. A value x means that the servo signal
 * can only go from -1 to 1 in minimum x seconds.
 * 
 * Zero means that slew rate limiting is disabled.
 * 
 *
 * @group Geometry
 * @decimal 2
 * @increment 0.05
 * @min 0
 * @max 10
 */
PARAM_DEFINE_FLOAT(CA_SV2_SLEW, 0.0);

/**
 * Servo 3 slew rate limit
 *
 * Minimum time allowed for the servo input signal to pass through
 * the full output range. A value x means that the servo signal
 * can only go from -1 to 1 in minimum x seconds.
 * 
 * Zero means that slew rate limiting is disabled.
 * 
 *
 * @group Geometry
 * @decimal 2
 * @increment 0.05
 * @min 0
 * @max 10
 */
PARAM_DEFINE_FLOAT(CA_SV3_SLEW, 0.0);

/**
 * Servo 4 slew rate limit
 *
 * Minimum time allowed for the servo input signal to pass through
 * the full output range. A value x means that the servo signal
 * can only go from -1 to 1 in minimum x seconds.
 * 
 * Zero means that slew rate limiting is disabled.
 * 
 *
 * @group Geometry
 * @decimal 2
 * @increment 0.05
 * @min 0
 * @max 10
 */
PARAM_DEFINE_FLOAT(CA_SV4_SLEW, 0.0);

/**
 * Servo 5 slew rate limit
 *
 * Minimum time allowed for the servo input signal to pass through
 * the full output range. A value x means that the servo signal
 * can only go from -1 to 1 in minimum x seconds.
 * 
 * Zero means that slew rate limiting is disabled.
 * 
 *
 * @group Geometry
 * @decimal 2
 * @increment 0.05
 * @min 0
 * @max 10
 */
PARAM_DEFINE_FLOAT(CA_SV5_SLEW, 0.0);

/**
 * Servo 6 slew rate limit
 *
 * Minimum time allowed for the servo input signal to pass through
 * the full output range. A value x means that the servo signal
 * can only go from -1 to 1 in minimum x seconds.
 * 
 * Zero means that slew rate limiting is disabled.
 * 
 *
 * @group Geometry
 * @decimal 2
 * @increment 0.05
 * @min 0
 * @max 10
 */
PARAM_DEFINE_FLOAT(CA_SV6_SLEW, 0.0);

/**
 * Servo 7 slew rate limit
 *
 * Minimum time allowed for the servo input signal to pass through
 * the full output range. A value x means that the servo signal
 * can only go from -1 to 1 in minimum x seconds.
 * 
 * Zero means that slew rate limiting is disabled.
 * 
 *
 * @group Geometry
 * @decimal 2
 * @increment 0.05
 * @min 0
 * @max 10
 */
PARAM_DEFINE_FLOAT(CA_SV7_SLEW, 0.0);

/**
 * Total number of rotors
 *
 * 
 *
 * @group Geometry
 * @value 0 0
 * @value 1 1
 * @value 2 2
 * @value 3 3
 * @value 4 4
 * @value 5 5
 * @value 6 6
 * @value 7 7
 * @value 8 8
 * @value 9 9
 * @value 10 10
 * @value 11 11
 * @value 12 12
 */
PARAM_DEFINE_INT32(CA_ROTOR_COUNT, 0);

/**
 * Position of rotor 0 along X body axis
 *
 * 
 *
 * @group Geometry
 * @decimal 2
 * @increment 0.1
 * @min -100
 * @max 100
 * @unit m
 */
PARAM_DEFINE_FLOAT(CA_ROTOR0_PX, 0.0);

/**
 * Position of rotor 1 along X body axis
 *
 * 
 *
 * @group Geometry
 * @decimal 2
 * @increment 0.1
 * @min -100
 * @max 100
 * @unit m
 */
PARAM_DEFINE_FLOAT(CA_ROTOR1_PX, 0.0);

/**
 * Position of rotor 2 along X body axis
 *
 * 
 *
 * @group Geometry
 * @decimal 2
 * @increment 0.1
 * @min -100
 * @max 100
 * @unit m
 */
PARAM_DEFINE_FLOAT(CA_ROTOR2_PX, 0.0);

/**
 * Position of rotor 3 along X body axis
 *
 * 
 *
 * @group Geometry
 * @decimal 2
 * @increment 0.1
 * @min -100
 * @max 100
 * @unit m
 */
PARAM_DEFINE_FLOAT(CA_ROTOR3_PX, 0.0);

/**
 * Position of rotor 4 along X body axis
 *
 * 
 *
 * @group Geometry
 * @decimal 2
 * @increment 0.1
 * @min -100
 * @max 100
 * @unit m
 */
PARAM_DEFINE_FLOAT(CA_ROTOR4_PX, 0.0);

/**
 * Position of rotor 5 along X body axis
 *
 * 
 *
 * @group Geometry
 * @decimal 2
 * @increment 0.1
 * @min -100
 * @max 100
 * @unit m
 */
PARAM_DEFINE_FLOAT(CA_ROTOR5_PX, 0.0);

/**
 * Position of rotor 6 along X body axis
 *
 * 
 *
 * @group Geometry
 * @decimal 2
 * @increment 0.1
 * @min -100
 * @max 100
 * @unit m
 */
PARAM_DEFINE_FLOAT(CA_ROTOR6_PX, 0.0);

/**
 * Position of rotor 7 along X body axis
 *
 * 
 *
 * @group Geometry
 * @decimal 2
 * @increment 0.1
 * @min -100
 * @max 100
 * @unit m
 */
PARAM_DEFINE_FLOAT(CA_ROTOR7_PX, 0.0);

/**
 * Position of rotor 8 along X body axis
 *
 * 
 *
 * @group Geometry
 * @decimal 2
 * @increment 0.1
 * @min -100
 * @max 100
 * @unit m
 */
PARAM_DEFINE_FLOAT(CA_ROTOR8_PX, 0.0);

/**
 * Position of rotor 9 along X body axis
 *
 * 
 *
 * @group Geometry
 * @decimal 2
 * @increment 0.1
 * @min -100
 * @max 100
 * @unit m
 */
PARAM_DEFINE_FLOAT(CA_ROTOR9_PX, 0.0);

/**
 * Position of rotor 10 along X body axis
 *
 * 
 *
 * @group Geometry
 * @decimal 2
 * @increment 0.1
 * @min -100
 * @max 100
 * @unit m
 */
PARAM_DEFINE_FLOAT(CA_ROTOR10_PX, 0.0);

/**
 * Position of rotor 11 along X body axis
 *
 * 
 *
 * @group Geometry
 * @decimal 2
 * @increment 0.1
 * @min -100
 * @max 100
 * @unit m
 */
PARAM_DEFINE_FLOAT(CA_ROTOR11_PX, 0.0);

/**
 * Position of rotor 0 along Y body axis
 *
 * 
 *
 * @group Geometry
 * @decimal 2
 * @increment 0.1
 * @min -100
 * @max 100
 * @unit m
 */
PARAM_DEFINE_FLOAT(CA_ROTOR0_PY, 0.0);

/**
 * Position of rotor 1 along Y body axis
 *
 * 
 *
 * @group Geometry
 * @decimal 2
 * @increment 0.1
 * @min -100
 * @max 100
 * @unit m
 */
PARAM_DEFINE_FLOAT(CA_ROTOR1_PY, 0.0);

/**
 * Position of rotor 2 along Y body axis
 *
 * 
 *
 * @group Geometry
 * @decimal 2
 * @increment 0.1
 * @min -100
 * @max 100
 * @unit m
 */
PARAM_DEFINE_FLOAT(CA_ROTOR2_PY, 0.0);

/**
 * Position of rotor 3 along Y body axis
 *
 * 
 *
 * @group Geometry
 * @decimal 2
 * @increment 0.1
 * @min -100
 * @max 100
 * @unit m
 */
PARAM_DEFINE_FLOAT(CA_ROTOR3_PY, 0.0);

/**
 * Position of rotor 4 along Y body axis
 *
 * 
 *
 * @group Geometry
 * @decimal 2
 * @increment 0.1
 * @min -100
 * @max 100
 * @unit m
 */
PARAM_DEFINE_FLOAT(CA_ROTOR4_PY, 0.0);

/**
 * Position of rotor 5 along Y body axis
 *
 * 
 *
 * @group Geometry
 * @decimal 2
 * @increment 0.1
 * @min -100
 * @max 100
 * @unit m
 */
PARAM_DEFINE_FLOAT(CA_ROTOR5_PY, 0.0);

/**
 * Position of rotor 6 along Y body axis
 *
 * 
 *
 * @group Geometry
 * @decimal 2
 * @increment 0.1
 * @min -100
 * @max 100
 * @unit m
 */
PARAM_DEFINE_FLOAT(CA_ROTOR6_PY, 0.0);

/**
 * Position of rotor 7 along Y body axis
 *
 * 
 *
 * @group Geometry
 * @decimal 2
 * @increment 0.1
 * @min -100
 * @max 100
 * @unit m
 */
PARAM_DEFINE_FLOAT(CA_ROTOR7_PY, 0.0);

/**
 * Position of rotor 8 along Y body axis
 *
 * 
 *
 * @group Geometry
 * @decimal 2
 * @increment 0.1
 * @min -100
 * @max 100
 * @unit m
 */
PARAM_DEFINE_FLOAT(CA_ROTOR8_PY, 0.0);

/**
 * Position of rotor 9 along Y body axis
 *
 * 
 *
 * @group Geometry
 * @decimal 2
 * @increment 0.1
 * @min -100
 * @max 100
 * @unit m
 */
PARAM_DEFINE_FLOAT(CA_ROTOR9_PY, 0.0);

/**
 * Position of rotor 10 along Y body axis
 *
 * 
 *
 * @group Geometry
 * @decimal 2
 * @increment 0.1
 * @min -100
 * @max 100
 * @unit m
 */
PARAM_DEFINE_FLOAT(CA_ROTOR10_PY, 0.0);

/**
 * Position of rotor 11 along Y body axis
 *
 * 
 *
 * @group Geometry
 * @decimal 2
 * @increment 0.1
 * @min -100
 * @max 100
 * @unit m
 */
PARAM_DEFINE_FLOAT(CA_ROTOR11_PY, 0.0);

/**
 * Position of rotor 0 along Z body axis
 *
 * 
 *
 * @group Geometry
 * @decimal 2
 * @increment 0.1
 * @min -100
 * @max 100
 * @unit m
 */
PARAM_DEFINE_FLOAT(CA_ROTOR0_PZ, 0.0);

/**
 * Position of rotor 1 along Z body axis
 *
 * 
 *
 * @group Geometry
 * @decimal 2
 * @increment 0.1
 * @min -100
 * @max 100
 * @unit m
 */
PARAM_DEFINE_FLOAT(CA_ROTOR1_PZ, 0.0);

/**
 * Position of rotor 2 along Z body axis
 *
 * 
 *
 * @group Geometry
 * @decimal 2
 * @increment 0.1
 * @min -100
 * @max 100
 * @unit m
 */
PARAM_DEFINE_FLOAT(CA_ROTOR2_PZ, 0.0);

/**
 * Position of rotor 3 along Z body axis
 *
 * 
 *
 * @group Geometry
 * @decimal 2
 * @increment 0.1
 * @min -100
 * @max 100
 * @unit m
 */
PARAM_DEFINE_FLOAT(CA_ROTOR3_PZ, 0.0);

/**
 * Position of rotor 4 along Z body axis
 *
 * 
 *
 * @group Geometry
 * @decimal 2
 * @increment 0.1
 * @min -100
 * @max 100
 * @unit m
 */
PARAM_DEFINE_FLOAT(CA_ROTOR4_PZ, 0.0);

/**
 * Position of rotor 5 along Z body axis
 *
 * 
 *
 * @group Geometry
 * @decimal 2
 * @increment 0.1
 * @min -100
 * @max 100
 * @unit m
 */
PARAM_DEFINE_FLOAT(CA_ROTOR5_PZ, 0.0);

/**
 * Position of rotor 6 along Z body axis
 *
 * 
 *
 * @group Geometry
 * @decimal 2
 * @increment 0.1
 * @min -100
 * @max 100
 * @unit m
 */
PARAM_DEFINE_FLOAT(CA_ROTOR6_PZ, 0.0);

/**
 * Position of rotor 7 along Z body axis
 *
 * 
 *
 * @group Geometry
 * @decimal 2
 * @increment 0.1
 * @min -100
 * @max 100
 * @unit m
 */
PARAM_DEFINE_FLOAT(CA_ROTOR7_PZ, 0.0);

/**
 * Position of rotor 8 along Z body axis
 *
 * 
 *
 * @group Geometry
 * @decimal 2
 * @increment 0.1
 * @min -100
 * @max 100
 * @unit m
 */
PARAM_DEFINE_FLOAT(CA_ROTOR8_PZ, 0.0);

/**
 * Position of rotor 9 along Z body axis
 *
 * 
 *
 * @group Geometry
 * @decimal 2
 * @increment 0.1
 * @min -100
 * @max 100
 * @unit m
 */
PARAM_DEFINE_FLOAT(CA_ROTOR9_PZ, 0.0);

/**
 * Position of rotor 10 along Z body axis
 *
 * 
 *
 * @group Geometry
 * @decimal 2
 * @increment 0.1
 * @min -100
 * @max 100
 * @unit m
 */
PARAM_DEFINE_FLOAT(CA_ROTOR10_PZ, 0.0);

/**
 * Position of rotor 11 along Z body axis
 *
 * 
 *
 * @group Geometry
 * @decimal 2
 * @increment 0.1
 * @min -100
 * @max 100
 * @unit m
 */
PARAM_DEFINE_FLOAT(CA_ROTOR11_PZ, 0.0);

/**
 * Axis of rotor 0 thrust vector, X body axis component
 *
 * Only the direction is considered (the vector is normalized).
 *
 * @group Geometry
 * @decimal 2
 * @increment 0.1
 * @min -100
 * @max 100
 */
PARAM_DEFINE_FLOAT(CA_ROTOR0_AX, 0.0);

/**
 * Axis of rotor 1 thrust vector, X body axis component
 *
 * Only the direction is considered (the vector is normalized).
 *
 * @group Geometry
 * @decimal 2
 * @increment 0.1
 * @min -100
 * @max 100
 */
PARAM_DEFINE_FLOAT(CA_ROTOR1_AX, 0.0);

/**
 * Axis of rotor 2 thrust vector, X body axis component
 *
 * Only the direction is considered (the vector is normalized).
 *
 * @group Geometry
 * @decimal 2
 * @increment 0.1
 * @min -100
 * @max 100
 */
PARAM_DEFINE_FLOAT(CA_ROTOR2_AX, 0.0);

/**
 * Axis of rotor 3 thrust vector, X body axis component
 *
 * Only the direction is considered (the vector is normalized).
 *
 * @group Geometry
 * @decimal 2
 * @increment 0.1
 * @min -100
 * @max 100
 */
PARAM_DEFINE_FLOAT(CA_ROTOR3_AX, 0.0);

/**
 * Axis of rotor 4 thrust vector, X body axis component
 *
 * Only the direction is considered (the vector is normalized).
 *
 * @group Geometry
 * @decimal 2
 * @increment 0.1
 * @min -100
 * @max 100
 */
PARAM_DEFINE_FLOAT(CA_ROTOR4_AX, 0.0);

/**
 * Axis of rotor 5 thrust vector, X body axis component
 *
 * Only the direction is considered (the vector is normalized).
 *
 * @group Geometry
 * @decimal 2
 * @increment 0.1
 * @min -100
 * @max 100
 */
PARAM_DEFINE_FLOAT(CA_ROTOR5_AX, 0.0);

/**
 * Axis of rotor 6 thrust vector, X body axis component
 *
 * Only the direction is considered (the vector is normalized).
 *
 * @group Geometry
 * @decimal 2
 * @increment 0.1
 * @min -100
 * @max 100
 */
PARAM_DEFINE_FLOAT(CA_ROTOR6_AX, 0.0);

/**
 * Axis of rotor 7 thrust vector, X body axis component
 *
 * Only the direction is considered (the vector is normalized).
 *
 * @group Geometry
 * @decimal 2
 * @increment 0.1
 * @min -100
 * @max 100
 */
PARAM_DEFINE_FLOAT(CA_ROTOR7_AX, 0.0);

/**
 * Axis of rotor 8 thrust vector, X body axis component
 *
 * Only the direction is considered (the vector is normalized).
 *
 * @group Geometry
 * @decimal 2
 * @increment 0.1
 * @min -100
 * @max 100
 */
PARAM_DEFINE_FLOAT(CA_ROTOR8_AX, 0.0);

/**
 * Axis of rotor 9 thrust vector, X body axis component
 *
 * Only the direction is considered (the vector is normalized).
 *
 * @group Geometry
 * @decimal 2
 * @increment 0.1
 * @min -100
 * @max 100
 */
PARAM_DEFINE_FLOAT(CA_ROTOR9_AX, 0.0);

/**
 * Axis of rotor 10 thrust vector, X body axis component
 *
 * Only the direction is considered (the vector is normalized).
 *
 * @group Geometry
 * @decimal 2
 * @increment 0.1
 * @min -100
 * @max 100
 */
PARAM_DEFINE_FLOAT(CA_ROTOR10_AX, 0.0);

/**
 * Axis of rotor 11 thrust vector, X body axis component
 *
 * Only the direction is considered (the vector is normalized).
 *
 * @group Geometry
 * @decimal 2
 * @increment 0.1
 * @min -100
 * @max 100
 */
PARAM_DEFINE_FLOAT(CA_ROTOR11_AX, 0.0);

/**
 * Axis of rotor 0 thrust vector, Y body axis component
 *
 * Only the direction is considered (the vector is normalized).
 *
 * @group Geometry
 * @decimal 2
 * @increment 0.1
 * @min -100
 * @max 100
 */
PARAM_DEFINE_FLOAT(CA_ROTOR0_AY, 0.0);

/**
 * Axis of rotor 1 thrust vector, Y body axis component
 *
 * Only the direction is considered (the vector is normalized).
 *
 * @group Geometry
 * @decimal 2
 * @increment 0.1
 * @min -100
 * @max 100
 */
PARAM_DEFINE_FLOAT(CA_ROTOR1_AY, 0.0);

/**
 * Axis of rotor 2 thrust vector, Y body axis component
 *
 * Only the direction is considered (the vector is normalized).
 *
 * @group Geometry
 * @decimal 2
 * @increment 0.1
 * @min -100
 * @max 100
 */
PARAM_DEFINE_FLOAT(CA_ROTOR2_AY, 0.0);

/**
 * Axis of rotor 3 thrust vector, Y body axis component
 *
 * Only the direction is considered (the vector is normalized).
 *
 * @group Geometry
 * @decimal 2
 * @increment 0.1
 * @min -100
 * @max 100
 */
PARAM_DEFINE_FLOAT(CA_ROTOR3_AY, 0.0);

/**
 * Axis of rotor 4 thrust vector, Y body axis component
 *
 * Only the direction is considered (the vector is normalized).
 *
 * @group Geometry
 * @decimal 2
 * @increment 0.1
 * @min -100
 * @max 100
 */
PARAM_DEFINE_FLOAT(CA_ROTOR4_AY, 0.0);

/**
 * Axis of rotor 5 thrust vector, Y body axis component
 *
 * Only the direction is considered (the vector is normalized).
 *
 * @group Geometry
 * @decimal 2
 * @increment 0.1
 * @min -100
 * @max 100
 */
PARAM_DEFINE_FLOAT(CA_ROTOR5_AY, 0.0);

/**
 * Axis of rotor 6 thrust vector, Y body axis component
 *
 * Only the direction is considered (the vector is normalized).
 *
 * @group Geometry
 * @decimal 2
 * @increment 0.1
 * @min -100
 * @max 100
 */
PARAM_DEFINE_FLOAT(CA_ROTOR6_AY, 0.0);

/**
 * Axis of rotor 7 thrust vector, Y body axis component
 *
 * Only the direction is considered (the vector is normalized).
 *
 * @group Geometry
 * @decimal 2
 * @increment 0.1
 * @min -100
 * @max 100
 */
PARAM_DEFINE_FLOAT(CA_ROTOR7_AY, 0.0);

/**
 * Axis of rotor 8 thrust vector, Y body axis component
 *
 * Only the direction is considered (the vector is normalized).
 *
 * @group Geometry
 * @decimal 2
 * @increment 0.1
 * @min -100
 * @max 100
 */
PARAM_DEFINE_FLOAT(CA_ROTOR8_AY, 0.0);

/**
 * Axis of rotor 9 thrust vector, Y body axis component
 *
 * Only the direction is considered (the vector is normalized).
 *
 * @group Geometry
 * @decimal 2
 * @increment 0.1
 * @min -100
 * @max 100
 */
PARAM_DEFINE_FLOAT(CA_ROTOR9_AY, 0.0);

/**
 * Axis of rotor 10 thrust vector, Y body axis component
 *
 * Only the direction is considered (the vector is normalized).
 *
 * @group Geometry
 * @decimal 2
 * @increment 0.1
 * @min -100
 * @max 100
 */
PARAM_DEFINE_FLOAT(CA_ROTOR10_AY, 0.0);

/**
 * Axis of rotor 11 thrust vector, Y body axis component
 *
 * Only the direction is considered (the vector is normalized).
 *
 * @group Geometry
 * @decimal 2
 * @increment 0.1
 * @min -100
 * @max 100
 */
PARAM_DEFINE_FLOAT(CA_ROTOR11_AY, 0.0);

/**
 * Axis of rotor 0 thrust vector, Z body axis component
 *
 * Only the direction is considered (the vector is normalized).
 *
 * @group Geometry
 * @decimal 2
 * @increment 0.1
 * @min -100
 * @max 100
 */
PARAM_DEFINE_FLOAT(CA_ROTOR0_AZ, -1.0);

/**
 * Axis of rotor 1 thrust vector, Z body axis component
 *
 * Only the direction is considered (the vector is normalized).
 *
 * @group Geometry
 * @decimal 2
 * @increment 0.1
 * @min -100
 * @max 100
 */
PARAM_DEFINE_FLOAT(CA_ROTOR1_AZ, -1.0);

/**
 * Axis of rotor 2 thrust vector, Z body axis component
 *
 * Only the direction is considered (the vector is normalized).
 *
 * @group Geometry
 * @decimal 2
 * @increment 0.1
 * @min -100
 * @max 100
 */
PARAM_DEFINE_FLOAT(CA_ROTOR2_AZ, -1.0);

/**
 * Axis of rotor 3 thrust vector, Z body axis component
 *
 * Only the direction is considered (the vector is normalized).
 *
 * @group Geometry
 * @decimal 2
 * @increment 0.1
 * @min -100
 * @max 100
 */
PARAM_DEFINE_FLOAT(CA_ROTOR3_AZ, -1.0);

/**
 * Axis of rotor 4 thrust vector, Z body axis component
 *
 * Only the direction is considered (the vector is normalized).
 *
 * @group Geometry
 * @decimal 2
 * @increment 0.1
 * @min -100
 * @max 100
 */
PARAM_DEFINE_FLOAT(CA_ROTOR4_AZ, -1.0);

/**
 * Axis of rotor 5 thrust vector, Z body axis component
 *
 * Only the direction is considered (the vector is normalized).
 *
 * @group Geometry
 * @decimal 2
 * @increment 0.1
 * @min -100
 * @max 100
 */
PARAM_DEFINE_FLOAT(CA_ROTOR5_AZ, -1.0);

/**
 * Axis of rotor 6 thrust vector, Z body axis component
 *
 * Only the direction is considered (the vector is normalized).
 *
 * @group Geometry
 * @decimal 2
 * @increment 0.1
 * @min -100
 * @max 100
 */
PARAM_DEFINE_FLOAT(CA_ROTOR6_AZ, -1.0);

/**
 * Axis of rotor 7 thrust vector, Z body axis component
 *
 * Only the direction is considered (the vector is normalized).
 *
 * @group Geometry
 * @decimal 2
 * @increment 0.1
 * @min -100
 * @max 100
 */
PARAM_DEFINE_FLOAT(CA_ROTOR7_AZ, -1.0);

/**
 * Axis of rotor 8 thrust vector, Z body axis component
 *
 * Only the direction is considered (the vector is normalized).
 *
 * @group Geometry
 * @decimal 2
 * @increment 0.1
 * @min -100
 * @max 100
 */
PARAM_DEFINE_FLOAT(CA_ROTOR8_AZ, -1.0);

/**
 * Axis of rotor 9 thrust vector, Z body axis component
 *
 * Only the direction is considered (the vector is normalized).
 *
 * @group Geometry
 * @decimal 2
 * @increment 0.1
 * @min -100
 * @max 100
 */
PARAM_DEFINE_FLOAT(CA_ROTOR9_AZ, -1.0);

/**
 * Axis of rotor 10 thrust vector, Z body axis component
 *
 * Only the direction is considered (the vector is normalized).
 *
 * @group Geometry
 * @decimal 2
 * @increment 0.1
 * @min -100
 * @max 100
 */
PARAM_DEFINE_FLOAT(CA_ROTOR10_AZ, -1.0);

/**
 * Axis of rotor 11 thrust vector, Z body axis component
 *
 * Only the direction is considered (the vector is normalized).
 *
 * @group Geometry
 * @decimal 2
 * @increment 0.1
 * @min -100
 * @max 100
 */
PARAM_DEFINE_FLOAT(CA_ROTOR11_AZ, -1.0);

/**
 * Thrust coefficient of rotor 0
 *
 * The thrust coefficient if defined as Thrust = CT * u^2,
 * where u (with value between actuator minimum and maximum)
 * is the output signal sent to the motor controller.
 * 
 *
 * @group Geometry
 * @decimal 1
 * @increment 1
 * @min 0
 * @max 100
 */
PARAM_DEFINE_FLOAT(CA_ROTOR0_CT, 6.5);

/**
 * Thrust coefficient of rotor 1
 *
 * The thrust coefficient if defined as Thrust = CT * u^2,
 * where u (with value between actuator minimum and maximum)
 * is the output signal sent to the motor controller.
 * 
 *
 * @group Geometry
 * @decimal 1
 * @increment 1
 * @min 0
 * @max 100
 */
PARAM_DEFINE_FLOAT(CA_ROTOR1_CT, 6.5);

/**
 * Thrust coefficient of rotor 2
 *
 * The thrust coefficient if defined as Thrust = CT * u^2,
 * where u (with value between actuator minimum and maximum)
 * is the output signal sent to the motor controller.
 * 
 *
 * @group Geometry
 * @decimal 1
 * @increment 1
 * @min 0
 * @max 100
 */
PARAM_DEFINE_FLOAT(CA_ROTOR2_CT, 6.5);

/**
 * Thrust coefficient of rotor 3
 *
 * The thrust coefficient if defined as Thrust = CT * u^2,
 * where u (with value between actuator minimum and maximum)
 * is the output signal sent to the motor controller.
 * 
 *
 * @group Geometry
 * @decimal 1
 * @increment 1
 * @min 0
 * @max 100
 */
PARAM_DEFINE_FLOAT(CA_ROTOR3_CT, 6.5);

/**
 * Thrust coefficient of rotor 4
 *
 * The thrust coefficient if defined as Thrust = CT * u^2,
 * where u (with value between actuator minimum and maximum)
 * is the output signal sent to the motor controller.
 * 
 *
 * @group Geometry
 * @decimal 1
 * @increment 1
 * @min 0
 * @max 100
 */
PARAM_DEFINE_FLOAT(CA_ROTOR4_CT, 6.5);

/**
 * Thrust coefficient of rotor 5
 *
 * The thrust coefficient if defined as Thrust = CT * u^2,
 * where u (with value between actuator minimum and maximum)
 * is the output signal sent to the motor controller.
 * 
 *
 * @group Geometry
 * @decimal 1
 * @increment 1
 * @min 0
 * @max 100
 */
PARAM_DEFINE_FLOAT(CA_ROTOR5_CT, 6.5);

/**
 * Thrust coefficient of rotor 6
 *
 * The thrust coefficient if defined as Thrust = CT * u^2,
 * where u (with value between actuator minimum and maximum)
 * is the output signal sent to the motor controller.
 * 
 *
 * @group Geometry
 * @decimal 1
 * @increment 1
 * @min 0
 * @max 100
 */
PARAM_DEFINE_FLOAT(CA_ROTOR6_CT, 6.5);

/**
 * Thrust coefficient of rotor 7
 *
 * The thrust coefficient if defined as Thrust = CT * u^2,
 * where u (with value between actuator minimum and maximum)
 * is the output signal sent to the motor controller.
 * 
 *
 * @group Geometry
 * @decimal 1
 * @increment 1
 * @min 0
 * @max 100
 */
PARAM_DEFINE_FLOAT(CA_ROTOR7_CT, 6.5);

/**
 * Thrust coefficient of rotor 8
 *
 * The thrust coefficient if defined as Thrust = CT * u^2,
 * where u (with value between actuator minimum and maximum)
 * is the output signal sent to the motor controller.
 * 
 *
 * @group Geometry
 * @decimal 1
 * @increment 1
 * @min 0
 * @max 100
 */
PARAM_DEFINE_FLOAT(CA_ROTOR8_CT, 6.5);

/**
 * Thrust coefficient of rotor 9
 *
 * The thrust coefficient if defined as Thrust = CT * u^2,
 * where u (with value between actuator minimum and maximum)
 * is the output signal sent to the motor controller.
 * 
 *
 * @group Geometry
 * @decimal 1
 * @increment 1
 * @min 0
 * @max 100
 */
PARAM_DEFINE_FLOAT(CA_ROTOR9_CT, 6.5);

/**
 * Thrust coefficient of rotor 10
 *
 * The thrust coefficient if defined as Thrust = CT * u^2,
 * where u (with value between actuator minimum and maximum)
 * is the output signal sent to the motor controller.
 * 
 *
 * @group Geometry
 * @decimal 1
 * @increment 1
 * @min 0
 * @max 100
 */
PARAM_DEFINE_FLOAT(CA_ROTOR10_CT, 6.5);

/**
 * Thrust coefficient of rotor 11
 *
 * The thrust coefficient if defined as Thrust = CT * u^2,
 * where u (with value between actuator minimum and maximum)
 * is the output signal sent to the motor controller.
 * 
 *
 * @group Geometry
 * @decimal 1
 * @increment 1
 * @min 0
 * @max 100
 */
PARAM_DEFINE_FLOAT(CA_ROTOR11_CT, 6.5);

/**
 * Moment coefficient of rotor 0
 *
 * The moment coefficient if defined as Torque = KM * Thrust.
 * 
 * Use a positive value for a rotor with CCW rotation.
 * Use a negative value for a rotor with CW rotation.
 * 
 *
 * @group Geometry
 * @decimal 3
 * @increment 0.01
 * @min -1
 * @max 1
 */
PARAM_DEFINE_FLOAT(CA_ROTOR0_KM, 0.05);

/**
 * Moment coefficient of rotor 1
 *
 * The moment coefficient if defined as Torque = KM * Thrust.
 * 
 * Use a positive value for a rotor with CCW rotation.
 * Use a negative value for a rotor with CW rotation.
 * 
 *
 * @group Geometry
 * @decimal 3
 * @increment 0.01
 * @min -1
 * @max 1
 */
PARAM_DEFINE_FLOAT(CA_ROTOR1_KM, 0.05);

/**
 * Moment coefficient of rotor 2
 *
 * The moment coefficient if defined as Torque = KM * Thrust.
 * 
 * Use a positive value for a rotor with CCW rotation.
 * Use a negative value for a rotor with CW rotation.
 * 
 *
 * @group Geometry
 * @decimal 3
 * @increment 0.01
 * @min -1
 * @max 1
 */
PARAM_DEFINE_FLOAT(CA_ROTOR2_KM, 0.05);

/**
 * Moment coefficient of rotor 3
 *
 * The moment coefficient if defined as Torque = KM * Thrust.
 * 
 * Use a positive value for a rotor with CCW rotation.
 * Use a negative value for a rotor with CW rotation.
 * 
 *
 * @group Geometry
 * @decimal 3
 * @increment 0.01
 * @min -1
 * @max 1
 */
PARAM_DEFINE_FLOAT(CA_ROTOR3_KM, 0.05);

/**
 * Moment coefficient of rotor 4
 *
 * The moment coefficient if defined as Torque = KM * Thrust.
 * 
 * Use a positive value for a rotor with CCW rotation.
 * Use a negative value for a rotor with CW rotation.
 * 
 *
 * @group Geometry
 * @decimal 3
 * @increment 0.01
 * @min -1
 * @max 1
 */
PARAM_DEFINE_FLOAT(CA_ROTOR4_KM, 0.05);

/**
 * Moment coefficient of rotor 5
 *
 * The moment coefficient if defined as Torque = KM * Thrust.
 * 
 * Use a positive value for a rotor with CCW rotation.
 * Use a negative value for a rotor with CW rotation.
 * 
 *
 * @group Geometry
 * @decimal 3
 * @increment 0.01
 * @min -1
 * @max 1
 */
PARAM_DEFINE_FLOAT(CA_ROTOR5_KM, 0.05);

/**
 * Moment coefficient of rotor 6
 *
 * The moment coefficient if defined as Torque = KM * Thrust.
 * 
 * Use a positive value for a rotor with CCW rotation.
 * Use a negative value for a rotor with CW rotation.
 * 
 *
 * @group Geometry
 * @decimal 3
 * @increment 0.01
 * @min -1
 * @max 1
 */
PARAM_DEFINE_FLOAT(CA_ROTOR6_KM, 0.05);

/**
 * Moment coefficient of rotor 7
 *
 * The moment coefficient if defined as Torque = KM * Thrust.
 * 
 * Use a positive value for a rotor with CCW rotation.
 * Use a negative value for a rotor with CW rotation.
 * 
 *
 * @group Geometry
 * @decimal 3
 * @increment 0.01
 * @min -1
 * @max 1
 */
PARAM_DEFINE_FLOAT(CA_ROTOR7_KM, 0.05);

/**
 * Moment coefficient of rotor 8
 *
 * The moment coefficient if defined as Torque = KM * Thrust.
 * 
 * Use a positive value for a rotor with CCW rotation.
 * Use a negative value for a rotor with CW rotation.
 * 
 *
 * @group Geometry
 * @decimal 3
 * @increment 0.01
 * @min -1
 * @max 1
 */
PARAM_DEFINE_FLOAT(CA_ROTOR8_KM, 0.05);

/**
 * Moment coefficient of rotor 9
 *
 * The moment coefficient if defined as Torque = KM * Thrust.
 * 
 * Use a positive value for a rotor with CCW rotation.
 * Use a negative value for a rotor with CW rotation.
 * 
 *
 * @group Geometry
 * @decimal 3
 * @increment 0.01
 * @min -1
 * @max 1
 */
PARAM_DEFINE_FLOAT(CA_ROTOR9_KM, 0.05);

/**
 * Moment coefficient of rotor 10
 *
 * The moment coefficient if defined as Torque = KM * Thrust.
 * 
 * Use a positive value for a rotor with CCW rotation.
 * Use a negative value for a rotor with CW rotation.
 * 
 *
 * @group Geometry
 * @decimal 3
 * @increment 0.01
 * @min -1
 * @max 1
 */
PARAM_DEFINE_FLOAT(CA_ROTOR10_KM, 0.05);

/**
 * Moment coefficient of rotor 11
 *
 * The moment coefficient if defined as Torque = KM * Thrust.
 * 
 * Use a positive value for a rotor with CCW rotation.
 * Use a negative value for a rotor with CW rotation.
 * 
 *
 * @group Geometry
 * @decimal 3
 * @increment 0.01
 * @min -1
 * @max 1
 */
PARAM_DEFINE_FLOAT(CA_ROTOR11_KM, 0.05);

/**
 * Rotor 0 tilt assignment
 *
 * If not set to None, this motor is tilted by the configured tilt servo.
 *
 * @group Geometry
 * @value 0 None
 * @value 1 Tilt 1
 * @value 2 Tilt 2
 * @value 3 Tilt 3
 * @value 4 Tilt 4
 */
PARAM_DEFINE_INT32(CA_ROTOR0_TILT, 0);

/**
 * Rotor 1 tilt assignment
 *
 * If not set to None, this motor is tilted by the configured tilt servo.
 *
 * @group Geometry
 * @value 0 None
 * @value 1 Tilt 1
 * @value 2 Tilt 2
 * @value 3 Tilt 3
 * @value 4 Tilt 4
 */
PARAM_DEFINE_INT32(CA_ROTOR1_TILT, 0);

/**
 * Rotor 2 tilt assignment
 *
 * If not set to None, this motor is tilted by the configured tilt servo.
 *
 * @group Geometry
 * @value 0 None
 * @value 1 Tilt 1
 * @value 2 Tilt 2
 * @value 3 Tilt 3
 * @value 4 Tilt 4
 */
PARAM_DEFINE_INT32(CA_ROTOR2_TILT, 0);

/**
 * Rotor 3 tilt assignment
 *
 * If not set to None, this motor is tilted by the configured tilt servo.
 *
 * @group Geometry
 * @value 0 None
 * @value 1 Tilt 1
 * @value 2 Tilt 2
 * @value 3 Tilt 3
 * @value 4 Tilt 4
 */
PARAM_DEFINE_INT32(CA_ROTOR3_TILT, 0);

/**
 * Rotor 4 tilt assignment
 *
 * If not set to None, this motor is tilted by the configured tilt servo.
 *
 * @group Geometry
 * @value 0 None
 * @value 1 Tilt 1
 * @value 2 Tilt 2
 * @value 3 Tilt 3
 * @value 4 Tilt 4
 */
PARAM_DEFINE_INT32(CA_ROTOR4_TILT, 0);

/**
 * Rotor 5 tilt assignment
 *
 * If not set to None, this motor is tilted by the configured tilt servo.
 *
 * @group Geometry
 * @value 0 None
 * @value 1 Tilt 1
 * @value 2 Tilt 2
 * @value 3 Tilt 3
 * @value 4 Tilt 4
 */
PARAM_DEFINE_INT32(CA_ROTOR5_TILT, 0);

/**
 * Rotor 6 tilt assignment
 *
 * If not set to None, this motor is tilted by the configured tilt servo.
 *
 * @group Geometry
 * @value 0 None
 * @value 1 Tilt 1
 * @value 2 Tilt 2
 * @value 3 Tilt 3
 * @value 4 Tilt 4
 */
PARAM_DEFINE_INT32(CA_ROTOR6_TILT, 0);

/**
 * Rotor 7 tilt assignment
 *
 * If not set to None, this motor is tilted by the configured tilt servo.
 *
 * @group Geometry
 * @value 0 None
 * @value 1 Tilt 1
 * @value 2 Tilt 2
 * @value 3 Tilt 3
 * @value 4 Tilt 4
 */
PARAM_DEFINE_INT32(CA_ROTOR7_TILT, 0);

/**
 * Rotor 8 tilt assignment
 *
 * If not set to None, this motor is tilted by the configured tilt servo.
 *
 * @group Geometry
 * @value 0 None
 * @value 1 Tilt 1
 * @value 2 Tilt 2
 * @value 3 Tilt 3
 * @value 4 Tilt 4
 */
PARAM_DEFINE_INT32(CA_ROTOR8_TILT, 0);

/**
 * Rotor 9 tilt assignment
 *
 * If not set to None, this motor is tilted by the configured tilt servo.
 *
 * @group Geometry
 * @value 0 None
 * @value 1 Tilt 1
 * @value 2 Tilt 2
 * @value 3 Tilt 3
 * @value 4 Tilt 4
 */
PARAM_DEFINE_INT32(CA_ROTOR9_TILT, 0);

/**
 * Rotor 10 tilt assignment
 *
 * If not set to None, this motor is tilted by the configured tilt servo.
 *
 * @group Geometry
 * @value 0 None
 * @value 1 Tilt 1
 * @value 2 Tilt 2
 * @value 3 Tilt 3
 * @value 4 Tilt 4
 */
PARAM_DEFINE_INT32(CA_ROTOR10_TILT, 0);

/**
 * Rotor 11 tilt assignment
 *
 * If not set to None, this motor is tilted by the configured tilt servo.
 *
 * @group Geometry
 * @value 0 None
 * @value 1 Tilt 1
 * @value 2 Tilt 2
 * @value 3 Tilt 3
 * @value 4 Tilt 4
 */
PARAM_DEFINE_INT32(CA_ROTOR11_TILT, 0);

/**
 * Total number of Control Surfaces
 *
 * 
 *
 * @group Geometry
 * @value 0 0
 * @value 1 1
 * @value 2 2
 * @value 3 3
 * @value 4 4
 * @value 5 5
 * @value 6 6
 * @value 7 7
 * @value 8 8
 */
PARAM_DEFINE_INT32(CA_SV_CS_COUNT, 0);

/**
 * Control Surface 0 type
 *
 * 
 *
 * @group Geometry
 * @value 0 (Not set)
 * @value 1 Left Aileron
 * @value 2 Right Aileron
 * @value 3 Elevator
 * @value 4 Rudder
 * @value 5 Left Elevon
 * @value 6 Right Elevon
 * @value 7 Left V-Tail
 * @value 8 Right V-Tail
 * @value 9 Left Flap
 * @value 10 Right Flap
 * @value 11 Airbrake
 * @value 12 Custom
 * @value 13 Left A-tail
 * @value 14 Right A-tail
 * @value 15 Single Channel Aileron
 * @value 16 Steering Wheel
 * @value 17 Left Spoiler
 * @value 18 Right Spoiler
 */
PARAM_DEFINE_INT32(CA_SV_CS0_TYPE, 0);

/**
 * Control Surface 1 type
 *
 * 
 *
 * @group Geometry
 * @value 0 (Not set)
 * @value 1 Left Aileron
 * @value 2 Right Aileron
 * @value 3 Elevator
 * @value 4 Rudder
 * @value 5 Left Elevon
 * @value 6 Right Elevon
 * @value 7 Left V-Tail
 * @value 8 Right V-Tail
 * @value 9 Left Flap
 * @value 10 Right Flap
 * @value 11 Airbrake
 * @value 12 Custom
 * @value 13 Left A-tail
 * @value 14 Right A-tail
 * @value 15 Single Channel Aileron
 * @value 16 Steering Wheel
 * @value 17 Left Spoiler
 * @value 18 Right Spoiler
 */
PARAM_DEFINE_INT32(CA_SV_CS1_TYPE, 0);

/**
 * Control Surface 2 type
 *
 * 
 *
 * @group Geometry
 * @value 0 (Not set)
 * @value 1 Left Aileron
 * @value 2 Right Aileron
 * @value 3 Elevator
 * @value 4 Rudder
 * @value 5 Left Elevon
 * @value 6 Right Elevon
 * @value 7 Left V-Tail
 * @value 8 Right V-Tail
 * @value 9 Left Flap
 * @value 10 Right Flap
 * @value 11 Airbrake
 * @value 12 Custom
 * @value 13 Left A-tail
 * @value 14 Right A-tail
 * @value 15 Single Channel Aileron
 * @value 16 Steering Wheel
 * @value 17 Left Spoiler
 * @value 18 Right Spoiler
 */
PARAM_DEFINE_INT32(CA_SV_CS2_TYPE, 0);

/**
 * Control Surface 3 type
 *
 * 
 *
 * @group Geometry
 * @value 0 (Not set)
 * @value 1 Left Aileron
 * @value 2 Right Aileron
 * @value 3 Elevator
 * @value 4 Rudder
 * @value 5 Left Elevon
 * @value 6 Right Elevon
 * @value 7 Left V-Tail
 * @value 8 Right V-Tail
 * @value 9 Left Flap
 * @value 10 Right Flap
 * @value 11 Airbrake
 * @value 12 Custom
 * @value 13 Left A-tail
 * @value 14 Right A-tail
 * @value 15 Single Channel Aileron
 * @value 16 Steering Wheel
 * @value 17 Left Spoiler
 * @value 18 Right Spoiler
 */
PARAM_DEFINE_INT32(CA_SV_CS3_TYPE, 0);

/**
 * Control Surface 4 type
 *
 * 
 *
 * @group Geometry
 * @value 0 (Not set)
 * @value 1 Left Aileron
 * @value 2 Right Aileron
 * @value 3 Elevator
 * @value 4 Rudder
 * @value 5 Left Elevon
 * @value 6 Right Elevon
 * @value 7 Left V-Tail
 * @value 8 Right V-Tail
 * @value 9 Left Flap
 * @value 10 Right Flap
 * @value 11 Airbrake
 * @value 12 Custom
 * @value 13 Left A-tail
 * @value 14 Right A-tail
 * @value 15 Single Channel Aileron
 * @value 16 Steering Wheel
 * @value 17 Left Spoiler
 * @value 18 Right Spoiler
 */
PARAM_DEFINE_INT32(CA_SV_CS4_TYPE, 0);

/**
 * Control Surface 5 type
 *
 * 
 *
 * @group Geometry
 * @value 0 (Not set)
 * @value 1 Left Aileron
 * @value 2 Right Aileron
 * @value 3 Elevator
 * @value 4 Rudder
 * @value 5 Left Elevon
 * @value 6 Right Elevon
 * @value 7 Left V-Tail
 * @value 8 Right V-Tail
 * @value 9 Left Flap
 * @value 10 Right Flap
 * @value 11 Airbrake
 * @value 12 Custom
 * @value 13 Left A-tail
 * @value 14 Right A-tail
 * @value 15 Single Channel Aileron
 * @value 16 Steering Wheel
 * @value 17 Left Spoiler
 * @value 18 Right Spoiler
 */
PARAM_DEFINE_INT32(CA_SV_CS5_TYPE, 0);

/**
 * Control Surface 6 type
 *
 * 
 *
 * @group Geometry
 * @value 0 (Not set)
 * @value 1 Left Aileron
 * @value 2 Right Aileron
 * @value 3 Elevator
 * @value 4 Rudder
 * @value 5 Left Elevon
 * @value 6 Right Elevon
 * @value 7 Left V-Tail
 * @value 8 Right V-Tail
 * @value 9 Left Flap
 * @value 10 Right Flap
 * @value 11 Airbrake
 * @value 12 Custom
 * @value 13 Left A-tail
 * @value 14 Right A-tail
 * @value 15 Single Channel Aileron
 * @value 16 Steering Wheel
 * @value 17 Left Spoiler
 * @value 18 Right Spoiler
 */
PARAM_DEFINE_INT32(CA_SV_CS6_TYPE, 0);

/**
 * Control Surface 7 type
 *
 * 
 *
 * @group Geometry
 * @value 0 (Not set)
 * @value 1 Left Aileron
 * @value 2 Right Aileron
 * @value 3 Elevator
 * @value 4 Rudder
 * @value 5 Left Elevon
 * @value 6 Right Elevon
 * @value 7 Left V-Tail
 * @value 8 Right V-Tail
 * @value 9 Left Flap
 * @value 10 Right Flap
 * @value 11 Airbrake
 * @value 12 Custom
 * @value 13 Left A-tail
 * @value 14 Right A-tail
 * @value 15 Single Channel Aileron
 * @value 16 Steering Wheel
 * @value 17 Left Spoiler
 * @value 18 Right Spoiler
 */
PARAM_DEFINE_INT32(CA_SV_CS7_TYPE, 0);

/**
 * Control Surface 0 roll torque scaling
 *
 * 
 *
 * @group Geometry
 * @decimal 2
 */
PARAM_DEFINE_FLOAT(CA_SV_CS0_TRQ_R, 0.0);

/**
 * Control Surface 1 roll torque scaling
 *
 * 
 *
 * @group Geometry
 * @decimal 2
 */
PARAM_DEFINE_FLOAT(CA_SV_CS1_TRQ_R, 0.0);

/**
 * Control Surface 2 roll torque scaling
 *
 * 
 *
 * @group Geometry
 * @decimal 2
 */
PARAM_DEFINE_FLOAT(CA_SV_CS2_TRQ_R, 0.0);

/**
 * Control Surface 3 roll torque scaling
 *
 * 
 *
 * @group Geometry
 * @decimal 2
 */
PARAM_DEFINE_FLOAT(CA_SV_CS3_TRQ_R, 0.0);

/**
 * Control Surface 4 roll torque scaling
 *
 * 
 *
 * @group Geometry
 * @decimal 2
 */
PARAM_DEFINE_FLOAT(CA_SV_CS4_TRQ_R, 0.0);

/**
 * Control Surface 5 roll torque scaling
 *
 * 
 *
 * @group Geometry
 * @decimal 2
 */
PARAM_DEFINE_FLOAT(CA_SV_CS5_TRQ_R, 0.0);

/**
 * Control Surface 6 roll torque scaling
 *
 * 
 *
 * @group Geometry
 * @decimal 2
 */
PARAM_DEFINE_FLOAT(CA_SV_CS6_TRQ_R, 0.0);

/**
 * Control Surface 7 roll torque scaling
 *
 * 
 *
 * @group Geometry
 * @decimal 2
 */
PARAM_DEFINE_FLOAT(CA_SV_CS7_TRQ_R, 0.0);

/**
 * Control Surface 0 pitch torque scaling
 *
 * 
 *
 * @group Geometry
 * @decimal 2
 */
PARAM_DEFINE_FLOAT(CA_SV_CS0_TRQ_P, 0.0);

/**
 * Control Surface 1 pitch torque scaling
 *
 * 
 *
 * @group Geometry
 * @decimal 2
 */
PARAM_DEFINE_FLOAT(CA_SV_CS1_TRQ_P, 0.0);

/**
 * Control Surface 2 pitch torque scaling
 *
 * 
 *
 * @group Geometry
 * @decimal 2
 */
PARAM_DEFINE_FLOAT(CA_SV_CS2_TRQ_P, 0.0);

/**
 * Control Surface 3 pitch torque scaling
 *
 * 
 *
 * @group Geometry
 * @decimal 2
 */
PARAM_DEFINE_FLOAT(CA_SV_CS3_TRQ_P, 0.0);

/**
 * Control Surface 4 pitch torque scaling
 *
 * 
 *
 * @group Geometry
 * @decimal 2
 */
PARAM_DEFINE_FLOAT(CA_SV_CS4_TRQ_P, 0.0);

/**
 * Control Surface 5 pitch torque scaling
 *
 * 
 *
 * @group Geometry
 * @decimal 2
 */
PARAM_DEFINE_FLOAT(CA_SV_CS5_TRQ_P, 0.0);

/**
 * Control Surface 6 pitch torque scaling
 *
 * 
 *
 * @group Geometry
 * @decimal 2
 */
PARAM_DEFINE_FLOAT(CA_SV_CS6_TRQ_P, 0.0);

/**
 * Control Surface 7 pitch torque scaling
 *
 * 
 *
 * @group Geometry
 * @decimal 2
 */
PARAM_DEFINE_FLOAT(CA_SV_CS7_TRQ_P, 0.0);

/**
 * Control Surface 0 yaw torque scaling
 *
 * 
 *
 * @group Geometry
 * @decimal 2
 */
PARAM_DEFINE_FLOAT(CA_SV_CS0_TRQ_Y, 0.0);

/**
 * Control Surface 1 yaw torque scaling
 *
 * 
 *
 * @group Geometry
 * @decimal 2
 */
PARAM_DEFINE_FLOAT(CA_SV_CS1_TRQ_Y, 0.0);

/**
 * Control Surface 2 yaw torque scaling
 *
 * 
 *
 * @group Geometry
 * @decimal 2
 */
PARAM_DEFINE_FLOAT(CA_SV_CS2_TRQ_Y, 0.0);

/**
 * Control Surface 3 yaw torque scaling
 *
 * 
 *
 * @group Geometry
 * @decimal 2
 */
PARAM_DEFINE_FLOAT(CA_SV_CS3_TRQ_Y, 0.0);

/**
 * Control Surface 4 yaw torque scaling
 *
 * 
 *
 * @group Geometry
 * @decimal 2
 */
PARAM_DEFINE_FLOAT(CA_SV_CS4_TRQ_Y, 0.0);

/**
 * Control Surface 5 yaw torque scaling
 *
 * 
 *
 * @group Geometry
 * @decimal 2
 */
PARAM_DEFINE_FLOAT(CA_SV_CS5_TRQ_Y, 0.0);

/**
 * Control Surface 6 yaw torque scaling
 *
 * 
 *
 * @group Geometry
 * @decimal 2
 */
PARAM_DEFINE_FLOAT(CA_SV_CS6_TRQ_Y, 0.0);

/**
 * Control Surface 7 yaw torque scaling
 *
 * 
 *
 * @group Geometry
 * @decimal 2
 */
PARAM_DEFINE_FLOAT(CA_SV_CS7_TRQ_Y, 0.0);

/**
 * Control Surface 0 trim
 *
 * Can be used to add an offset to the servo control.
 *
 * @group Geometry
 * @decimal 2
 * @min -1.0
 * @max 1.0
 */
PARAM_DEFINE_FLOAT(CA_SV_CS0_TRIM, 0.0);

/**
 * Control Surface 1 trim
 *
 * Can be used to add an offset to the servo control.
 *
 * @group Geometry
 * @decimal 2
 * @min -1.0
 * @max 1.0
 */
PARAM_DEFINE_FLOAT(CA_SV_CS1_TRIM, 0.0);

/**
 * Control Surface 2 trim
 *
 * Can be used to add an offset to the servo control.
 *
 * @group Geometry
 * @decimal 2
 * @min -1.0
 * @max 1.0
 */
PARAM_DEFINE_FLOAT(CA_SV_CS2_TRIM, 0.0);

/**
 * Control Surface 3 trim
 *
 * Can be used to add an offset to the servo control.
 *
 * @group Geometry
 * @decimal 2
 * @min -1.0
 * @max 1.0
 */
PARAM_DEFINE_FLOAT(CA_SV_CS3_TRIM, 0.0);

/**
 * Control Surface 4 trim
 *
 * Can be used to add an offset to the servo control.
 *
 * @group Geometry
 * @decimal 2
 * @min -1.0
 * @max 1.0
 */
PARAM_DEFINE_FLOAT(CA_SV_CS4_TRIM, 0.0);

/**
 * Control Surface 5 trim
 *
 * Can be used to add an offset to the servo control.
 *
 * @group Geometry
 * @decimal 2
 * @min -1.0
 * @max 1.0
 */
PARAM_DEFINE_FLOAT(CA_SV_CS5_TRIM, 0.0);

/**
 * Control Surface 6 trim
 *
 * Can be used to add an offset to the servo control.
 *
 * @group Geometry
 * @decimal 2
 * @min -1.0
 * @max 1.0
 */
PARAM_DEFINE_FLOAT(CA_SV_CS6_TRIM, 0.0);

/**
 * Control Surface 7 trim
 *
 * Can be used to add an offset to the servo control.
 *
 * @group Geometry
 * @decimal 2
 * @min -1.0
 * @max 1.0
 */
PARAM_DEFINE_FLOAT(CA_SV_CS7_TRIM, 0.0);

/**
 * Control Surface 0 configuration as flap
 *
 * 
 *
 * @group Geometry
 * @decimal 2
 * @min -1.0
 * @max 1.0
 */
PARAM_DEFINE_FLOAT(CA_SV_CS0_FLAP, 0);

/**
 * Control Surface 1 configuration as flap
 *
 * 
 *
 * @group Geometry
 * @decimal 2
 * @min -1.0
 * @max 1.0
 */
PARAM_DEFINE_FLOAT(CA_SV_CS1_FLAP, 0);

/**
 * Control Surface 2 configuration as flap
 *
 * 
 *
 * @group Geometry
 * @decimal 2
 * @min -1.0
 * @max 1.0
 */
PARAM_DEFINE_FLOAT(CA_SV_CS2_FLAP, 0);

/**
 * Control Surface 3 configuration as flap
 *
 * 
 *
 * @group Geometry
 * @decimal 2
 * @min -1.0
 * @max 1.0
 */
PARAM_DEFINE_FLOAT(CA_SV_CS3_FLAP, 0);

/**
 * Control Surface 4 configuration as flap
 *
 * 
 *
 * @group Geometry
 * @decimal 2
 * @min -1.0
 * @max 1.0
 */
PARAM_DEFINE_FLOAT(CA_SV_CS4_FLAP, 0);

/**
 * Control Surface 5 configuration as flap
 *
 * 
 *
 * @group Geometry
 * @decimal 2
 * @min -1.0
 * @max 1.0
 */
PARAM_DEFINE_FLOAT(CA_SV_CS5_FLAP, 0);

/**
 * Control Surface 6 configuration as flap
 *
 * 
 *
 * @group Geometry
 * @decimal 2
 * @min -1.0
 * @max 1.0
 */
PARAM_DEFINE_FLOAT(CA_SV_CS6_FLAP, 0);

/**
 * Control Surface 7 configuration as flap
 *
 * 
 *
 * @group Geometry
 * @decimal 2
 * @min -1.0
 * @max 1.0
 */
PARAM_DEFINE_FLOAT(CA_SV_CS7_FLAP, 0);

/**
 * Control Surface 0 configuration as spoiler
 *
 * 
 *
 * @group Geometry
 * @decimal 2
 * @min -1.0
 * @max 1.0
 */
PARAM_DEFINE_FLOAT(CA_SV_CS0_SPOIL, 0);

/**
 * Control Surface 1 configuration as spoiler
 *
 * 
 *
 * @group Geometry
 * @decimal 2
 * @min -1.0
 * @max 1.0
 */
PARAM_DEFINE_FLOAT(CA_SV_CS1_SPOIL, 0);

/**
 * Control Surface 2 configuration as spoiler
 *
 * 
 *
 * @group Geometry
 * @decimal 2
 * @min -1.0
 * @max 1.0
 */
PARAM_DEFINE_FLOAT(CA_SV_CS2_SPOIL, 0);

/**
 * Control Surface 3 configuration as spoiler
 *
 * 
 *
 * @group Geometry
 * @decimal 2
 * @min -1.0
 * @max 1.0
 */
PARAM_DEFINE_FLOAT(CA_SV_CS3_SPOIL, 0);

/**
 * Control Surface 4 configuration as spoiler
 *
 * 
 *
 * @group Geometry
 * @decimal 2
 * @min -1.0
 * @max 1.0
 */
PARAM_DEFINE_FLOAT(CA_SV_CS4_SPOIL, 0);

/**
 * Control Surface 5 configuration as spoiler
 *
 * 
 *
 * @group Geometry
 * @decimal 2
 * @min -1.0
 * @max 1.0
 */
PARAM_DEFINE_FLOAT(CA_SV_CS5_SPOIL, 0);

/**
 * Control Surface 6 configuration as spoiler
 *
 * 
 *
 * @group Geometry
 * @decimal 2
 * @min -1.0
 * @max 1.0
 */
PARAM_DEFINE_FLOAT(CA_SV_CS6_SPOIL, 0);

/**
 * Control Surface 7 configuration as spoiler
 *
 * 
 *
 * @group Geometry
 * @decimal 2
 * @min -1.0
 * @max 1.0
 */
PARAM_DEFINE_FLOAT(CA_SV_CS7_SPOIL, 0);

/**
 * Total number of Tilt Servos
 *
 * 
 *
 * @group Geometry
 * @value 0 0
 * @value 1 1
 * @value 2 2
 * @value 3 3
 * @value 4 4
 */
PARAM_DEFINE_INT32(CA_SV_TL_COUNT, 0);

/**
 * Tilt 0 is used for control
 *
 * Define if this servo is used for additional control.
 *
 * @group Geometry
 * @value 0 None
 * @value 1 Yaw
 * @value 2 Pitch
 * @value 3 Yaw and Pitch
 */
PARAM_DEFINE_INT32(CA_SV_TL0_CT, 1);

/**
 * Tilt 1 is used for control
 *
 * Define if this servo is used for additional control.
 *
 * @group Geometry
 * @value 0 None
 * @value 1 Yaw
 * @value 2 Pitch
 * @value 3 Yaw and Pitch
 */
PARAM_DEFINE_INT32(CA_SV_TL1_CT, 1);

/**
 * Tilt 2 is used for control
 *
 * Define if this servo is used for additional control.
 *
 * @group Geometry
 * @value 0 None
 * @value 1 Yaw
 * @value 2 Pitch
 * @value 3 Yaw and Pitch
 */
PARAM_DEFINE_INT32(CA_SV_TL2_CT, 1);

/**
 * Tilt 3 is used for control
 *
 * Define if this servo is used for additional control.
 *
 * @group Geometry
 * @value 0 None
 * @value 1 Yaw
 * @value 2 Pitch
 * @value 3 Yaw and Pitch
 */
PARAM_DEFINE_INT32(CA_SV_TL3_CT, 1);

/**
 * Tilt Servo 0 Tilt Angle at Minimum
 *
 * Defines the tilt angle when the servo is at the minimum.
 * An angle of zero means upwards.
 * 
 *
 * @group Geometry
 * @decimal 0
 * @min -90.0
 * @max 90.0
 * @unit deg
 */
PARAM_DEFINE_FLOAT(CA_SV_TL0_MINA, 0.0);

/**
 * Tilt Servo 1 Tilt Angle at Minimum
 *
 * Defines the tilt angle when the servo is at the minimum.
 * An angle of zero means upwards.
 * 
 *
 * @group Geometry
 * @decimal 0
 * @min -90.0
 * @max 90.0
 * @unit deg
 */
PARAM_DEFINE_FLOAT(CA_SV_TL1_MINA, 0.0);

/**
 * Tilt Servo 2 Tilt Angle at Minimum
 *
 * Defines the tilt angle when the servo is at the minimum.
 * An angle of zero means upwards.
 * 
 *
 * @group Geometry
 * @decimal 0
 * @min -90.0
 * @max 90.0
 * @unit deg
 */
PARAM_DEFINE_FLOAT(CA_SV_TL2_MINA, 0.0);

/**
 * Tilt Servo 3 Tilt Angle at Minimum
 *
 * Defines the tilt angle when the servo is at the minimum.
 * An angle of zero means upwards.
 * 
 *
 * @group Geometry
 * @decimal 0
 * @min -90.0
 * @max 90.0
 * @unit deg
 */
PARAM_DEFINE_FLOAT(CA_SV_TL3_MINA, 0.0);

/**
 * Tilt Servo 0 Tilt Angle at Maximum
 *
 * Defines the tilt angle when the servo is at the maximum.
 * An angle of zero means upwards.
 * 
 *
 * @group Geometry
 * @decimal 0
 * @min -90.0
 * @max 90.0
 * @unit deg
 */
PARAM_DEFINE_FLOAT(CA_SV_TL0_MAXA, 90.0);

/**
 * Tilt Servo 1 Tilt Angle at Maximum
 *
 * Defines the tilt angle when the servo is at the maximum.
 * An angle of zero means upwards.
 * 
 *
 * @group Geometry
 * @decimal 0
 * @min -90.0
 * @max 90.0
 * @unit deg
 */
PARAM_DEFINE_FLOAT(CA_SV_TL1_MAXA, 90.0);

/**
 * Tilt Servo 2 Tilt Angle at Maximum
 *
 * Defines the tilt angle when the servo is at the maximum.
 * An angle of zero means upwards.
 * 
 *
 * @group Geometry
 * @decimal 0
 * @min -90.0
 * @max 90.0
 * @unit deg
 */
PARAM_DEFINE_FLOAT(CA_SV_TL2_MAXA, 90.0);

/**
 * Tilt Servo 3 Tilt Angle at Maximum
 *
 * Defines the tilt angle when the servo is at the maximum.
 * An angle of zero means upwards.
 * 
 *
 * @group Geometry
 * @decimal 0
 * @min -90.0
 * @max 90.0
 * @unit deg
 */
PARAM_DEFINE_FLOAT(CA_SV_TL3_MAXA, 90.0);

/**
 * Tilt Servo 0 Tilt Direction
 *
 * Defines the direction the servo tilts towards when moving towards the maximum tilt angle.
 * For example if the minimum tilt angle is -90, the maximum 90, and the direction 'Towards Front',
 * the motor axis aligns with the XZ-plane, points towards -X at the minimum and +X at the maximum tilt.
 * 
 *
 * @group Geometry
 * @value 0 Towards Front
 * @value 90 Towards Right
 * @min 0
 * @max 359
 */
PARAM_DEFINE_INT32(CA_SV_TL0_TD, 0);

/**
 * Tilt Servo 1 Tilt Direction
 *
 * Defines the direction the servo tilts towards when moving towards the maximum tilt angle.
 * For example if the minimum tilt angle is -90, the maximum 90, and the direction 'Towards Front',
 * the motor axis aligns with the XZ-plane, points towards -X at the minimum and +X at the maximum tilt.
 * 
 *
 * @group Geometry
 * @value 0 Towards Front
 * @value 90 Towards Right
 * @min 0
 * @max 359
 */
PARAM_DEFINE_INT32(CA_SV_TL1_TD, 0);

/**
 * Tilt Servo 2 Tilt Direction
 *
 * Defines the direction the servo tilts towards when moving towards the maximum tilt angle.
 * For example if the minimum tilt angle is -90, the maximum 90, and the direction 'Towards Front',
 * the motor axis aligns with the XZ-plane, points towards -X at the minimum and +X at the maximum tilt.
 * 
 *
 * @group Geometry
 * @value 0 Towards Front
 * @value 90 Towards Right
 * @min 0
 * @max 359
 */
PARAM_DEFINE_INT32(CA_SV_TL2_TD, 0);

/**
 * Tilt Servo 3 Tilt Direction
 *
 * Defines the direction the servo tilts towards when moving towards the maximum tilt angle.
 * For example if the minimum tilt angle is -90, the maximum 90, and the direction 'Towards Front',
 * the motor axis aligns with the XZ-plane, points towards -X at the minimum and +X at the maximum tilt.
 * 
 *
 * @group Geometry
 * @value 0 Towards Front
 * @value 90 Towards Right
 * @min 0
 * @max 359
 */
PARAM_DEFINE_INT32(CA_SV_TL3_TD, 0);

/**
 * Number of swash plates servos
 *
 * 
 *
 * @group Geometry
 * @value 3 3
 * @value 4 4
 */
PARAM_DEFINE_INT32(CA_SP0_COUNT, 3);

/**
 * Angle for swash plate servo 0
 *
 * The angle is measured clockwise (as seen from top), with 0 pointing forwards (X axis).
 * 
 *
 * @group Geometry
 * @decimal 0
 * @increment 10
 * @min 0
 * @max 360
 * @unit deg
 */
PARAM_DEFINE_FLOAT(CA_SP0_ANG0, 0);

/**
 * Angle for swash plate servo 1
 *
 * The angle is measured clockwise (as seen from top), with 0 pointing forwards (X axis).
 * 
 *
 * @group Geometry
 * @decimal 0
 * @increment 10
 * @min 0
 * @max 360
 * @unit deg
 */
PARAM_DEFINE_FLOAT(CA_SP0_ANG1, 140);

/**
 * Angle for swash plate servo 2
 *
 * The angle is measured clockwise (as seen from top), with 0 pointing forwards (X axis).
 * 
 *
 * @group Geometry
 * @decimal 0
 * @increment 10
 * @min 0
 * @max 360
 * @unit deg
 */
PARAM_DEFINE_FLOAT(CA_SP0_ANG2, 220);

/**
 * Angle for swash plate servo 3
 *
 * The angle is measured clockwise (as seen from top), with 0 pointing forwards (X axis).
 * 
 *
 * @group Geometry
 * @decimal 0
 * @increment 10
 * @min 0
 * @max 360
 * @unit deg
 */
PARAM_DEFINE_FLOAT(CA_SP0_ANG3, 0);

/**
 * Arm length for swash plate servo 0
 *
 * This is relative to the other arm lengths.
 * 
 *
 * @group Geometry
 * @decimal 3
 * @increment 0.1
 * @min 0
 * @max 10
 */
PARAM_DEFINE_FLOAT(CA_SP0_ARM_L0, 1.0);

/**
 * Arm length for swash plate servo 1
 *
 * This is relative to the other arm lengths.
 * 
 *
 * @group Geometry
 * @decimal 3
 * @increment 0.1
 * @min 0
 * @max 10
 */
PARAM_DEFINE_FLOAT(CA_SP0_ARM_L1, 1.0);

/**
 * Arm length for swash plate servo 2
 *
 * This is relative to the other arm lengths.
 * 
 *
 * @group Geometry
 * @decimal 3
 * @increment 0.1
 * @min 0
 * @max 10
 */
PARAM_DEFINE_FLOAT(CA_SP0_ARM_L2, 1.0);

/**
 * Arm length for swash plate servo 3
 *
 * This is relative to the other arm lengths.
 * 
 *
 * @group Geometry
 * @decimal 3
 * @increment 0.1
 * @min 0
 * @max 10
 */
PARAM_DEFINE_FLOAT(CA_SP0_ARM_L3, 1.0);

/**
 * Throttle curve at position 0
 *
 * Defines the output throttle at the interval position 0.
 * 
 *
 * @group Geometry
 * @decimal 3
 * @increment 0.1
 * @min 0
 * @max 1
 */
PARAM_DEFINE_FLOAT(CA_HELI_THR_C0, 1);

/**
 * Throttle curve at position 1
 *
 * Defines the output throttle at the interval position 1.
 * 
 *
 * @group Geometry
 * @decimal 3
 * @increment 0.1
 * @min 0
 * @max 1
 */
PARAM_DEFINE_FLOAT(CA_HELI_THR_C1, 1);

/**
 * Throttle curve at position 2
 *
 * Defines the output throttle at the interval position 2.
 * 
 *
 * @group Geometry
 * @decimal 3
 * @increment 0.1
 * @min 0
 * @max 1
 */
PARAM_DEFINE_FLOAT(CA_HELI_THR_C2, 1);

/**
 * Throttle curve at position 3
 *
 * Defines the output throttle at the interval position 3.
 * 
 *
 * @group Geometry
 * @decimal 3
 * @increment 0.1
 * @min 0
 * @max 1
 */
PARAM_DEFINE_FLOAT(CA_HELI_THR_C3, 1);

/**
 * Throttle curve at position 4
 *
 * Defines the output throttle at the interval position 4.
 * 
 *
 * @group Geometry
 * @decimal 3
 * @increment 0.1
 * @min 0
 * @max 1
 */
PARAM_DEFINE_FLOAT(CA_HELI_THR_C4, 1);

/**
 * Collective pitch curve at position 0
 *
 * Defines the collective pitch at the interval position 0 for a given thrust setpoint.
 * 
 * Use negative values if the swash plate needs to move down to provide upwards thrust.
 * 
 *
 * @group Geometry
 * @decimal 3
 * @increment 0.1
 * @min -1
 * @max 1
 */
PARAM_DEFINE_FLOAT(CA_HELI_PITCH_C0, -0.05);

/**
 * Collective pitch curve at position 1
 *
 * Defines the collective pitch at the interval position 1 for a given thrust setpoint.
 * 
 * Use negative values if the swash plate needs to move down to provide upwards thrust.
 * 
 *
 * @group Geometry
 * @decimal 3
 * @increment 0.1
 * @min -1
 * @max 1
 */
PARAM_DEFINE_FLOAT(CA_HELI_PITCH_C1, 0.0725);

/**
 * Collective pitch curve at position 2
 *
 * Defines the collective pitch at the interval position 2 for a given thrust setpoint.
 * 
 * Use negative values if the swash plate needs to move down to provide upwards thrust.
 * 
 *
 * @group Geometry
 * @decimal 3
 * @increment 0.1
 * @min -1
 * @max 1
 */
PARAM_DEFINE_FLOAT(CA_HELI_PITCH_C2, 0.2);

/**
 * Collective pitch curve at position 3
 *
 * Defines the collective pitch at the interval position 3 for a given thrust setpoint.
 * 
 * Use negative values if the swash plate needs to move down to provide upwards thrust.
 * 
 *
 * @group Geometry
 * @decimal 3
 * @increment 0.1
 * @min -1
 * @max 1
 */
PARAM_DEFINE_FLOAT(CA_HELI_PITCH_C3, 0.325);

/**
 * Collective pitch curve at position 4
 *
 * Defines the collective pitch at the interval position 4 for a given thrust setpoint.
 * 
 * Use negative values if the swash plate needs to move down to provide upwards thrust.
 * 
 *
 * @group Geometry
 * @decimal 3
 * @increment 0.1
 * @min -1
 * @max 1
 */
PARAM_DEFINE_FLOAT(CA_HELI_PITCH_C4, 0.45);

/**
 * Scale for yaw compensation based on collective pitch
 *
 * This allows to add a proportional factor of the collective pitch command to the yaw command.
 * A negative value is needed when positive thrust of the tail rotor rotates the vehicle opposite to the main rotor turn direction.
 * 
 * tail_output += CA_HELI_YAW_CP_S * abs(collective_pitch - CA_HELI_YAW_CP_O)
 * 
 *
 * @group Geometry
 * @decimal 3
 * @increment 0.1
 * @min -2
 * @max 2
 */
PARAM_DEFINE_FLOAT(CA_HELI_YAW_CP_S, 0.0);

/**
 * Offset for yaw compensation based on collective pitch
 *
 * This allows to specify which collective pitch command results in the least amount of rotor drag.
 * This is used to increase the accuracy of the yaw drag torque compensation based on collective pitch
 * by aligning the lowest rotor drag with zero compensation.
 * For symmetric profile blades this is the command that results in exactly 0° collective blade angle.
 * For lift profile blades this is typically a command resulting in slightly negative collective blade angle.
 * 
 * tail_output += CA_HELI_YAW_CP_S * abs(collective_pitch - CA_HELI_YAW_CP_O)
 * 
 *
 * @group Geometry
 * @decimal 3
 * @increment 0.1
 * @min -2
 * @max 2
 */
PARAM_DEFINE_FLOAT(CA_HELI_YAW_CP_O, 0.0);

/**
 * Scale for yaw compensation based on throttle
 *
 * This allows to add a proportional factor of the throttle command to the yaw command.
 * A negative value is needed when positive thrust of the tail rotor rotates the vehicle opposite to the main rotor turn direction.
 * 
 * tail_output += CA_HELI_YAW_TH_S * throttle
 * 
 *
 * @group Geometry
 * @decimal 3
 * @increment 0.1
 * @min -2
 * @max 2
 */
PARAM_DEFINE_FLOAT(CA_HELI_YAW_TH_S, 0.0);

/**
 * Main rotor turns counter-clockwise
 *
 * Default configuration is for a clockwise turning main rotor and
 * positive thrust of the tail rotor is expected to rotate the vehicle clockwise.
 * Set this parameter to true if the tail rotor provides thrust in counter-clockwise direction
 * which is mostly the case when the main rotor turns counter-clockwise.
 * 
 *
 * @group Geometry
 * @boolean
 */
PARAM_DEFINE_INT32(CA_HELI_YAW_CCW, 0);

/**
 * Motor failure handling mode
 *
 * This is used to specify how to handle motor failures
 * reported by failure detector.
 * 
 *
 * @group Geometry
 * @value 0 Ignore
 * @value 1 Remove first failed motor from effectiveness
 */
PARAM_DEFINE_INT32(CA_FAILURE_MODE, 0);

/**
 * SIM_GZ ESC 1 Output Function
 *
 * Select what should be output on SIM_GZ ESC 1.
 * 
 * The default failsafe value is set according to the selected function:
 * - 'Min' for ConstantMin
 * - 'Max' for ConstantMax
 * - 'Max' for Parachute
 * - ('Max'+'Min')/2 for Servos
 * - 'Disarmed' for the rest
 * 
 *
 * @group Actuator Outputs
 * @value 0 Disabled
 * @value 1 Constant Min
 * @value 2 Constant Max
 * @value 101 Motor 1
 * @value 102 Motor 2
 * @value 103 Motor 3
 * @value 104 Motor 4
 * @value 105 Motor 5
 * @value 106 Motor 6
 * @value 107 Motor 7
 * @value 108 Motor 8
 * @value 109 Motor 9
 * @value 110 Motor 10
 * @value 111 Motor 11
 * @value 112 Motor 12
 * @value 201 Servo 1
 * @value 202 Servo 2
 * @value 203 Servo 3
 * @value 204 Servo 4
 * @value 205 Servo 5
 * @value 206 Servo 6
 * @value 207 Servo 7
 * @value 208 Servo 8
 * @value 301 Offboard Actuator Set 1
 * @value 302 Offboard Actuator Set 2
 * @value 303 Offboard Actuator Set 3
 * @value 304 Offboard Actuator Set 4
 * @value 305 Offboard Actuator Set 5
 * @value 306 Offboard Actuator Set 6
 * @value 400 Landing Gear
 * @value 401 Parachute
 * @value 402 RC Roll
 * @value 403 RC Pitch
 * @value 404 RC Throttle
 * @value 405 RC Yaw
 * @value 406 RC Flaps
 * @value 407 RC AUX 1
 * @value 408 RC AUX 2
 * @value 409 RC AUX 3
 * @value 410 RC AUX 4
 * @value 411 RC AUX 5
 * @value 412 RC AUX 6
 * @value 420 Gimbal Roll
 * @value 421 Gimbal Pitch
 * @value 422 Gimbal Yaw
 * @value 430 Gripper
 * @value 440 Landing Gear Wheel
 */
PARAM_DEFINE_INT32(SIM_GZ_EC_FUNC1, 0);

/**
 * SIM_GZ ESC 2 Output Function
 *
 * Select what should be output on SIM_GZ ESC 2.
 * 
 * The default failsafe value is set according to the selected function:
 * - 'Min' for ConstantMin
 * - 'Max' for ConstantMax
 * - 'Max' for Parachute
 * - ('Max'+'Min')/2 for Servos
 * - 'Disarmed' for the rest
 * 
 *
 * @group Actuator Outputs
 * @value 0 Disabled
 * @value 1 Constant Min
 * @value 2 Constant Max
 * @value 101 Motor 1
 * @value 102 Motor 2
 * @value 103 Motor 3
 * @value 104 Motor 4
 * @value 105 Motor 5
 * @value 106 Motor 6
 * @value 107 Motor 7
 * @value 108 Motor 8
 * @value 109 Motor 9
 * @value 110 Motor 10
 * @value 111 Motor 11
 * @value 112 Motor 12
 * @value 201 Servo 1
 * @value 202 Servo 2
 * @value 203 Servo 3
 * @value 204 Servo 4
 * @value 205 Servo 5
 * @value 206 Servo 6
 * @value 207 Servo 7
 * @value 208 Servo 8
 * @value 301 Offboard Actuator Set 1
 * @value 302 Offboard Actuator Set 2
 * @value 303 Offboard Actuator Set 3
 * @value 304 Offboard Actuator Set 4
 * @value 305 Offboard Actuator Set 5
 * @value 306 Offboard Actuator Set 6
 * @value 400 Landing Gear
 * @value 401 Parachute
 * @value 402 RC Roll
 * @value 403 RC Pitch
 * @value 404 RC Throttle
 * @value 405 RC Yaw
 * @value 406 RC Flaps
 * @value 407 RC AUX 1
 * @value 408 RC AUX 2
 * @value 409 RC AUX 3
 * @value 410 RC AUX 4
 * @value 411 RC AUX 5
 * @value 412 RC AUX 6
 * @value 420 Gimbal Roll
 * @value 421 Gimbal Pitch
 * @value 422 Gimbal Yaw
 * @value 430 Gripper
 * @value 440 Landing Gear Wheel
 */
PARAM_DEFINE_INT32(SIM_GZ_EC_FUNC2, 0);

/**
 * SIM_GZ ESC 3 Output Function
 *
 * Select what should be output on SIM_GZ ESC 3.
 * 
 * The default failsafe value is set according to the selected function:
 * - 'Min' for ConstantMin
 * - 'Max' for ConstantMax
 * - 'Max' for Parachute
 * - ('Max'+'Min')/2 for Servos
 * - 'Disarmed' for the rest
 * 
 *
 * @group Actuator Outputs
 * @value 0 Disabled
 * @value 1 Constant Min
 * @value 2 Constant Max
 * @value 101 Motor 1
 * @value 102 Motor 2
 * @value 103 Motor 3
 * @value 104 Motor 4
 * @value 105 Motor 5
 * @value 106 Motor 6
 * @value 107 Motor 7
 * @value 108 Motor 8
 * @value 109 Motor 9
 * @value 110 Motor 10
 * @value 111 Motor 11
 * @value 112 Motor 12
 * @value 201 Servo 1
 * @value 202 Servo 2
 * @value 203 Servo 3
 * @value 204 Servo 4
 * @value 205 Servo 5
 * @value 206 Servo 6
 * @value 207 Servo 7
 * @value 208 Servo 8
 * @value 301 Offboard Actuator Set 1
 * @value 302 Offboard Actuator Set 2
 * @value 303 Offboard Actuator Set 3
 * @value 304 Offboard Actuator Set 4
 * @value 305 Offboard Actuator Set 5
 * @value 306 Offboard Actuator Set 6
 * @value 400 Landing Gear
 * @value 401 Parachute
 * @value 402 RC Roll
 * @value 403 RC Pitch
 * @value 404 RC Throttle
 * @value 405 RC Yaw
 * @value 406 RC Flaps
 * @value 407 RC AUX 1
 * @value 408 RC AUX 2
 * @value 409 RC AUX 3
 * @value 410 RC AUX 4
 * @value 411 RC AUX 5
 * @value 412 RC AUX 6
 * @value 420 Gimbal Roll
 * @value 421 Gimbal Pitch
 * @value 422 Gimbal Yaw
 * @value 430 Gripper
 * @value 440 Landing Gear Wheel
 */
PARAM_DEFINE_INT32(SIM_GZ_EC_FUNC3, 0);

/**
 * SIM_GZ ESC 4 Output Function
 *
 * Select what should be output on SIM_GZ ESC 4.
 * 
 * The default failsafe value is set according to the selected function:
 * - 'Min' for ConstantMin
 * - 'Max' for ConstantMax
 * - 'Max' for Parachute
 * - ('Max'+'Min')/2 for Servos
 * - 'Disarmed' for the rest
 * 
 *
 * @group Actuator Outputs
 * @value 0 Disabled
 * @value 1 Constant Min
 * @value 2 Constant Max
 * @value 101 Motor 1
 * @value 102 Motor 2
 * @value 103 Motor 3
 * @value 104 Motor 4
 * @value 105 Motor 5
 * @value 106 Motor 6
 * @value 107 Motor 7
 * @value 108 Motor 8
 * @value 109 Motor 9
 * @value 110 Motor 10
 * @value 111 Motor 11
 * @value 112 Motor 12
 * @value 201 Servo 1
 * @value 202 Servo 2
 * @value 203 Servo 3
 * @value 204 Servo 4
 * @value 205 Servo 5
 * @value 206 Servo 6
 * @value 207 Servo 7
 * @value 208 Servo 8
 * @value 301 Offboard Actuator Set 1
 * @value 302 Offboard Actuator Set 2
 * @value 303 Offboard Actuator Set 3
 * @value 304 Offboard Actuator Set 4
 * @value 305 Offboard Actuator Set 5
 * @value 306 Offboard Actuator Set 6
 * @value 400 Landing Gear
 * @value 401 Parachute
 * @value 402 RC Roll
 * @value 403 RC Pitch
 * @value 404 RC Throttle
 * @value 405 RC Yaw
 * @value 406 RC Flaps
 * @value 407 RC AUX 1
 * @value 408 RC AUX 2
 * @value 409 RC AUX 3
 * @value 410 RC AUX 4
 * @value 411 RC AUX 5
 * @value 412 RC AUX 6
 * @value 420 Gimbal Roll
 * @value 421 Gimbal Pitch
 * @value 422 Gimbal Yaw
 * @value 430 Gripper
 * @value 440 Landing Gear Wheel
 */
PARAM_DEFINE_INT32(SIM_GZ_EC_FUNC4, 0);

/**
 * SIM_GZ ESC 5 Output Function
 *
 * Select what should be output on SIM_GZ ESC 5.
 * 
 * The default failsafe value is set according to the selected function:
 * - 'Min' for ConstantMin
 * - 'Max' for ConstantMax
 * - 'Max' for Parachute
 * - ('Max'+'Min')/2 for Servos
 * - 'Disarmed' for the rest
 * 
 *
 * @group Actuator Outputs
 * @value 0 Disabled
 * @value 1 Constant Min
 * @value 2 Constant Max
 * @value 101 Motor 1
 * @value 102 Motor 2
 * @value 103 Motor 3
 * @value 104 Motor 4
 * @value 105 Motor 5
 * @value 106 Motor 6
 * @value 107 Motor 7
 * @value 108 Motor 8
 * @value 109 Motor 9
 * @value 110 Motor 10
 * @value 111 Motor 11
 * @value 112 Motor 12
 * @value 201 Servo 1
 * @value 202 Servo 2
 * @value 203 Servo 3
 * @value 204 Servo 4
 * @value 205 Servo 5
 * @value 206 Servo 6
 * @value 207 Servo 7
 * @value 208 Servo 8
 * @value 301 Offboard Actuator Set 1
 * @value 302 Offboard Actuator Set 2
 * @value 303 Offboard Actuator Set 3
 * @value 304 Offboard Actuator Set 4
 * @value 305 Offboard Actuator Set 5
 * @value 306 Offboard Actuator Set 6
 * @value 400 Landing Gear
 * @value 401 Parachute
 * @value 402 RC Roll
 * @value 403 RC Pitch
 * @value 404 RC Throttle
 * @value 405 RC Yaw
 * @value 406 RC Flaps
 * @value 407 RC AUX 1
 * @value 408 RC AUX 2
 * @value 409 RC AUX 3
 * @value 410 RC AUX 4
 * @value 411 RC AUX 5
 * @value 412 RC AUX 6
 * @value 420 Gimbal Roll
 * @value 421 Gimbal Pitch
 * @value 422 Gimbal Yaw
 * @value 430 Gripper
 * @value 440 Landing Gear Wheel
 */
PARAM_DEFINE_INT32(SIM_GZ_EC_FUNC5, 0);

/**
 * SIM_GZ ESC 6 Output Function
 *
 * Select what should be output on SIM_GZ ESC 6.
 * 
 * The default failsafe value is set according to the selected function:
 * - 'Min' for ConstantMin
 * - 'Max' for ConstantMax
 * - 'Max' for Parachute
 * - ('Max'+'Min')/2 for Servos
 * - 'Disarmed' for the rest
 * 
 *
 * @group Actuator Outputs
 * @value 0 Disabled
 * @value 1 Constant Min
 * @value 2 Constant Max
 * @value 101 Motor 1
 * @value 102 Motor 2
 * @value 103 Motor 3
 * @value 104 Motor 4
 * @value 105 Motor 5
 * @value 106 Motor 6
 * @value 107 Motor 7
 * @value 108 Motor 8
 * @value 109 Motor 9
 * @value 110 Motor 10
 * @value 111 Motor 11
 * @value 112 Motor 12
 * @value 201 Servo 1
 * @value 202 Servo 2
 * @value 203 Servo 3
 * @value 204 Servo 4
 * @value 205 Servo 5
 * @value 206 Servo 6
 * @value 207 Servo 7
 * @value 208 Servo 8
 * @value 301 Offboard Actuator Set 1
 * @value 302 Offboard Actuator Set 2
 * @value 303 Offboard Actuator Set 3
 * @value 304 Offboard Actuator Set 4
 * @value 305 Offboard Actuator Set 5
 * @value 306 Offboard Actuator Set 6
 * @value 400 Landing Gear
 * @value 401 Parachute
 * @value 402 RC Roll
 * @value 403 RC Pitch
 * @value 404 RC Throttle
 * @value 405 RC Yaw
 * @value 406 RC Flaps
 * @value 407 RC AUX 1
 * @value 408 RC AUX 2
 * @value 409 RC AUX 3
 * @value 410 RC AUX 4
 * @value 411 RC AUX 5
 * @value 412 RC AUX 6
 * @value 420 Gimbal Roll
 * @value 421 Gimbal Pitch
 * @value 422 Gimbal Yaw
 * @value 430 Gripper
 * @value 440 Landing Gear Wheel
 */
PARAM_DEFINE_INT32(SIM_GZ_EC_FUNC6, 0);

/**
 * SIM_GZ ESC 7 Output Function
 *
 * Select what should be output on SIM_GZ ESC 7.
 * 
 * The default failsafe value is set according to the selected function:
 * - 'Min' for ConstantMin
 * - 'Max' for ConstantMax
 * - 'Max' for Parachute
 * - ('Max'+'Min')/2 for Servos
 * - 'Disarmed' for the rest
 * 
 *
 * @group Actuator Outputs
 * @value 0 Disabled
 * @value 1 Constant Min
 * @value 2 Constant Max
 * @value 101 Motor 1
 * @value 102 Motor 2
 * @value 103 Motor 3
 * @value 104 Motor 4
 * @value 105 Motor 5
 * @value 106 Motor 6
 * @value 107 Motor 7
 * @value 108 Motor 8
 * @value 109 Motor 9
 * @value 110 Motor 10
 * @value 111 Motor 11
 * @value 112 Motor 12
 * @value 201 Servo 1
 * @value 202 Servo 2
 * @value 203 Servo 3
 * @value 204 Servo 4
 * @value 205 Servo 5
 * @value 206 Servo 6
 * @value 207 Servo 7
 * @value 208 Servo 8
 * @value 301 Offboard Actuator Set 1
 * @value 302 Offboard Actuator Set 2
 * @value 303 Offboard Actuator Set 3
 * @value 304 Offboard Actuator Set 4
 * @value 305 Offboard Actuator Set 5
 * @value 306 Offboard Actuator Set 6
 * @value 400 Landing Gear
 * @value 401 Parachute
 * @value 402 RC Roll
 * @value 403 RC Pitch
 * @value 404 RC Throttle
 * @value 405 RC Yaw
 * @value 406 RC Flaps
 * @value 407 RC AUX 1
 * @value 408 RC AUX 2
 * @value 409 RC AUX 3
 * @value 410 RC AUX 4
 * @value 411 RC AUX 5
 * @value 412 RC AUX 6
 * @value 420 Gimbal Roll
 * @value 421 Gimbal Pitch
 * @value 422 Gimbal Yaw
 * @value 430 Gripper
 * @value 440 Landing Gear Wheel
 */
PARAM_DEFINE_INT32(SIM_GZ_EC_FUNC7, 0);

/**
 * SIM_GZ ESC 8 Output Function
 *
 * Select what should be output on SIM_GZ ESC 8.
 * 
 * The default failsafe value is set according to the selected function:
 * - 'Min' for ConstantMin
 * - 'Max' for ConstantMax
 * - 'Max' for Parachute
 * - ('Max'+'Min')/2 for Servos
 * - 'Disarmed' for the rest
 * 
 *
 * @group Actuator Outputs
 * @value 0 Disabled
 * @value 1 Constant Min
 * @value 2 Constant Max
 * @value 101 Motor 1
 * @value 102 Motor 2
 * @value 103 Motor 3
 * @value 104 Motor 4
 * @value 105 Motor 5
 * @value 106 Motor 6
 * @value 107 Motor 7
 * @value 108 Motor 8
 * @value 109 Motor 9
 * @value 110 Motor 10
 * @value 111 Motor 11
 * @value 112 Motor 12
 * @value 201 Servo 1
 * @value 202 Servo 2
 * @value 203 Servo 3
 * @value 204 Servo 4
 * @value 205 Servo 5
 * @value 206 Servo 6
 * @value 207 Servo 7
 * @value 208 Servo 8
 * @value 301 Offboard Actuator Set 1
 * @value 302 Offboard Actuator Set 2
 * @value 303 Offboard Actuator Set 3
 * @value 304 Offboard Actuator Set 4
 * @value 305 Offboard Actuator Set 5
 * @value 306 Offboard Actuator Set 6
 * @value 400 Landing Gear
 * @value 401 Parachute
 * @value 402 RC Roll
 * @value 403 RC Pitch
 * @value 404 RC Throttle
 * @value 405 RC Yaw
 * @value 406 RC Flaps
 * @value 407 RC AUX 1
 * @value 408 RC AUX 2
 * @value 409 RC AUX 3
 * @value 410 RC AUX 4
 * @value 411 RC AUX 5
 * @value 412 RC AUX 6
 * @value 420 Gimbal Roll
 * @value 421 Gimbal Pitch
 * @value 422 Gimbal Yaw
 * @value 430 Gripper
 * @value 440 Landing Gear Wheel
 */
PARAM_DEFINE_INT32(SIM_GZ_EC_FUNC8, 0);

/**
 * SIM_GZ ESC 1 Disarmed Value
 *
 * This is the output value that is set when not armed.
 * 
 * Note that non-motor outputs might already be active in prearm state if COM_PREARM_MODE is set.
 * 
 *
 * @group Actuator Outputs
 * @min 0
 * @max 1000
 */
PARAM_DEFINE_INT32(SIM_GZ_EC_DIS1, 0);

/**
 * SIM_GZ ESC 2 Disarmed Value
 *
 * This is the output value that is set when not armed.
 * 
 * Note that non-motor outputs might already be active in prearm state if COM_PREARM_MODE is set.
 * 
 *
 * @group Actuator Outputs
 * @min 0
 * @max 1000
 */
PARAM_DEFINE_INT32(SIM_GZ_EC_DIS2, 0);

/**
 * SIM_GZ ESC 3 Disarmed Value
 *
 * This is the output value that is set when not armed.
 * 
 * Note that non-motor outputs might already be active in prearm state if COM_PREARM_MODE is set.
 * 
 *
 * @group Actuator Outputs
 * @min 0
 * @max 1000
 */
PARAM_DEFINE_INT32(SIM_GZ_EC_DIS3, 0);

/**
 * SIM_GZ ESC 4 Disarmed Value
 *
 * This is the output value that is set when not armed.
 * 
 * Note that non-motor outputs might already be active in prearm state if COM_PREARM_MODE is set.
 * 
 *
 * @group Actuator Outputs
 * @min 0
 * @max 1000
 */
PARAM_DEFINE_INT32(SIM_GZ_EC_DIS4, 0);

/**
 * SIM_GZ ESC 5 Disarmed Value
 *
 * This is the output value that is set when not armed.
 * 
 * Note that non-motor outputs might already be active in prearm state if COM_PREARM_MODE is set.
 * 
 *
 * @group Actuator Outputs
 * @min 0
 * @max 1000
 */
PARAM_DEFINE_INT32(SIM_GZ_EC_DIS5, 0);

/**
 * SIM_GZ ESC 6 Disarmed Value
 *
 * This is the output value that is set when not armed.
 * 
 * Note that non-motor outputs might already be active in prearm state if COM_PREARM_MODE is set.
 * 
 *
 * @group Actuator Outputs
 * @min 0
 * @max 1000
 */
PARAM_DEFINE_INT32(SIM_GZ_EC_DIS6, 0);

/**
 * SIM_GZ ESC 7 Disarmed Value
 *
 * This is the output value that is set when not armed.
 * 
 * Note that non-motor outputs might already be active in prearm state if COM_PREARM_MODE is set.
 * 
 *
 * @group Actuator Outputs
 * @min 0
 * @max 1000
 */
PARAM_DEFINE_INT32(SIM_GZ_EC_DIS7, 0);

/**
 * SIM_GZ ESC 8 Disarmed Value
 *
 * This is the output value that is set when not armed.
 * 
 * Note that non-motor outputs might already be active in prearm state if COM_PREARM_MODE is set.
 * 
 *
 * @group Actuator Outputs
 * @min 0
 * @max 1000
 */
PARAM_DEFINE_INT32(SIM_GZ_EC_DIS8, 0);

/**
 * SIM_GZ ESC 1 Minimum Value
 *
 * Minimum output value (when not disarmed).
 * 
 *
 * @group Actuator Outputs
 * @min 0
 * @max 1000
 */
PARAM_DEFINE_INT32(SIM_GZ_EC_MIN1, 0);

/**
 * SIM_GZ ESC 2 Minimum Value
 *
 * Minimum output value (when not disarmed).
 * 
 *
 * @group Actuator Outputs
 * @min 0
 * @max 1000
 */
PARAM_DEFINE_INT32(SIM_GZ_EC_MIN2, 0);

/**
 * SIM_GZ ESC 3 Minimum Value
 *
 * Minimum output value (when not disarmed).
 * 
 *
 * @group Actuator Outputs
 * @min 0
 * @max 1000
 */
PARAM_DEFINE_INT32(SIM_GZ_EC_MIN3, 0);

/**
 * SIM_GZ ESC 4 Minimum Value
 *
 * Minimum output value (when not disarmed).
 * 
 *
 * @group Actuator Outputs
 * @min 0
 * @max 1000
 */
PARAM_DEFINE_INT32(SIM_GZ_EC_MIN4, 0);

/**
 * SIM_GZ ESC 5 Minimum Value
 *
 * Minimum output value (when not disarmed).
 * 
 *
 * @group Actuator Outputs
 * @min 0
 * @max 1000
 */
PARAM_DEFINE_INT32(SIM_GZ_EC_MIN5, 0);

/**
 * SIM_GZ ESC 6 Minimum Value
 *
 * Minimum output value (when not disarmed).
 * 
 *
 * @group Actuator Outputs
 * @min 0
 * @max 1000
 */
PARAM_DEFINE_INT32(SIM_GZ_EC_MIN6, 0);

/**
 * SIM_GZ ESC 7 Minimum Value
 *
 * Minimum output value (when not disarmed).
 * 
 *
 * @group Actuator Outputs
 * @min 0
 * @max 1000
 */
PARAM_DEFINE_INT32(SIM_GZ_EC_MIN7, 0);

/**
 * SIM_GZ ESC 8 Minimum Value
 *
 * Minimum output value (when not disarmed).
 * 
 *
 * @group Actuator Outputs
 * @min 0
 * @max 1000
 */
PARAM_DEFINE_INT32(SIM_GZ_EC_MIN8, 0);

/**
 * SIM_GZ ESC 1 Maximum Value
 *
 * Maxmimum output value (when not disarmed).
 * 
 *
 * @group Actuator Outputs
 * @min 0
 * @max 1000
 */
PARAM_DEFINE_INT32(SIM_GZ_EC_MAX1, 1000);

/**
 * SIM_GZ ESC 2 Maximum Value
 *
 * Maxmimum output value (when not disarmed).
 * 
 *
 * @group Actuator Outputs
 * @min 0
 * @max 1000
 */
PARAM_DEFINE_INT32(SIM_GZ_EC_MAX2, 1000);

/**
 * SIM_GZ ESC 3 Maximum Value
 *
 * Maxmimum output value (when not disarmed).
 * 
 *
 * @group Actuator Outputs
 * @min 0
 * @max 1000
 */
PARAM_DEFINE_INT32(SIM_GZ_EC_MAX3, 1000);

/**
 * SIM_GZ ESC 4 Maximum Value
 *
 * Maxmimum output value (when not disarmed).
 * 
 *
 * @group Actuator Outputs
 * @min 0
 * @max 1000
 */
PARAM_DEFINE_INT32(SIM_GZ_EC_MAX4, 1000);

/**
 * SIM_GZ ESC 5 Maximum Value
 *
 * Maxmimum output value (when not disarmed).
 * 
 *
 * @group Actuator Outputs
 * @min 0
 * @max 1000
 */
PARAM_DEFINE_INT32(SIM_GZ_EC_MAX5, 1000);

/**
 * SIM_GZ ESC 6 Maximum Value
 *
 * Maxmimum output value (when not disarmed).
 * 
 *
 * @group Actuator Outputs
 * @min 0
 * @max 1000
 */
PARAM_DEFINE_INT32(SIM_GZ_EC_MAX6, 1000);

/**
 * SIM_GZ ESC 7 Maximum Value
 *
 * Maxmimum output value (when not disarmed).
 * 
 *
 * @group Actuator Outputs
 * @min 0
 * @max 1000
 */
PARAM_DEFINE_INT32(SIM_GZ_EC_MAX7, 1000);

/**
 * SIM_GZ ESC 8 Maximum Value
 *
 * Maxmimum output value (when not disarmed).
 * 
 *
 * @group Actuator Outputs
 * @min 0
 * @max 1000
 */
PARAM_DEFINE_INT32(SIM_GZ_EC_MAX8, 1000);

/**
 * SIM_GZ ESC 1 Failsafe Value
 *
 * This is the output value that is set when in failsafe mode.
 * 
 * When set to -1 (default), the value depends on the function (see SIM_GZ_EC_FUNC1).
 * 
 *
 * @group Actuator Outputs
 * @min -1
 * @max 1000
 */
PARAM_DEFINE_INT32(SIM_GZ_EC_FAIL1, -1);

/**
 * SIM_GZ ESC 2 Failsafe Value
 *
 * This is the output value that is set when in failsafe mode.
 * 
 * When set to -1 (default), the value depends on the function (see SIM_GZ_EC_FUNC2).
 * 
 *
 * @group Actuator Outputs
 * @min -1
 * @max 1000
 */
PARAM_DEFINE_INT32(SIM_GZ_EC_FAIL2, -1);

/**
 * SIM_GZ ESC 3 Failsafe Value
 *
 * This is the output value that is set when in failsafe mode.
 * 
 * When set to -1 (default), the value depends on the function (see SIM_GZ_EC_FUNC3).
 * 
 *
 * @group Actuator Outputs
 * @min -1
 * @max 1000
 */
PARAM_DEFINE_INT32(SIM_GZ_EC_FAIL3, -1);

/**
 * SIM_GZ ESC 4 Failsafe Value
 *
 * This is the output value that is set when in failsafe mode.
 * 
 * When set to -1 (default), the value depends on the function (see SIM_GZ_EC_FUNC4).
 * 
 *
 * @group Actuator Outputs
 * @min -1
 * @max 1000
 */
PARAM_DEFINE_INT32(SIM_GZ_EC_FAIL4, -1);

/**
 * SIM_GZ ESC 5 Failsafe Value
 *
 * This is the output value that is set when in failsafe mode.
 * 
 * When set to -1 (default), the value depends on the function (see SIM_GZ_EC_FUNC5).
 * 
 *
 * @group Actuator Outputs
 * @min -1
 * @max 1000
 */
PARAM_DEFINE_INT32(SIM_GZ_EC_FAIL5, -1);

/**
 * SIM_GZ ESC 6 Failsafe Value
 *
 * This is the output value that is set when in failsafe mode.
 * 
 * When set to -1 (default), the value depends on the function (see SIM_GZ_EC_FUNC6).
 * 
 *
 * @group Actuator Outputs
 * @min -1
 * @max 1000
 */
PARAM_DEFINE_INT32(SIM_GZ_EC_FAIL6, -1);

/**
 * SIM_GZ ESC 7 Failsafe Value
 *
 * This is the output value that is set when in failsafe mode.
 * 
 * When set to -1 (default), the value depends on the function (see SIM_GZ_EC_FUNC7).
 * 
 *
 * @group Actuator Outputs
 * @min -1
 * @max 1000
 */
PARAM_DEFINE_INT32(SIM_GZ_EC_FAIL7, -1);

/**
 * SIM_GZ ESC 8 Failsafe Value
 *
 * This is the output value that is set when in failsafe mode.
 * 
 * When set to -1 (default), the value depends on the function (see SIM_GZ_EC_FUNC8).
 * 
 *
 * @group Actuator Outputs
 * @min -1
 * @max 1000
 */
PARAM_DEFINE_INT32(SIM_GZ_EC_FAIL8, -1);

/**
 * SIM_GZ Servo 1 Output Function
 *
 * Select what should be output on SIM_GZ Servo 1.
 * 
 * The default failsafe value is set according to the selected function:
 * - 'Min' for ConstantMin
 * - 'Max' for ConstantMax
 * - 'Max' for Parachute
 * - ('Max'+'Min')/2 for Servos
 * - 'Disarmed' for the rest
 * 
 *
 * @group Actuator Outputs
 * @value 0 Disabled
 * @value 1 Constant Min
 * @value 2 Constant Max
 * @value 101 Motor 1
 * @value 102 Motor 2
 * @value 103 Motor 3
 * @value 104 Motor 4
 * @value 105 Motor 5
 * @value 106 Motor 6
 * @value 107 Motor 7
 * @value 108 Motor 8
 * @value 109 Motor 9
 * @value 110 Motor 10
 * @value 111 Motor 11
 * @value 112 Motor 12
 * @value 201 Servo 1
 * @value 202 Servo 2
 * @value 203 Servo 3
 * @value 204 Servo 4
 * @value 205 Servo 5
 * @value 206 Servo 6
 * @value 207 Servo 7
 * @value 208 Servo 8
 * @value 301 Offboard Actuator Set 1
 * @value 302 Offboard Actuator Set 2
 * @value 303 Offboard Actuator Set 3
 * @value 304 Offboard Actuator Set 4
 * @value 305 Offboard Actuator Set 5
 * @value 306 Offboard Actuator Set 6
 * @value 400 Landing Gear
 * @value 401 Parachute
 * @value 402 RC Roll
 * @value 403 RC Pitch
 * @value 404 RC Throttle
 * @value 405 RC Yaw
 * @value 406 RC Flaps
 * @value 407 RC AUX 1
 * @value 408 RC AUX 2
 * @value 409 RC AUX 3
 * @value 410 RC AUX 4
 * @value 411 RC AUX 5
 * @value 412 RC AUX 6
 * @value 420 Gimbal Roll
 * @value 421 Gimbal Pitch
 * @value 422 Gimbal Yaw
 * @value 430 Gripper
 * @value 440 Landing Gear Wheel
 */
PARAM_DEFINE_INT32(SIM_GZ_SV_FUNC1, 0);

/**
 * SIM_GZ Servo 2 Output Function
 *
 * Select what should be output on SIM_GZ Servo 2.
 * 
 * The default failsafe value is set according to the selected function:
 * - 'Min' for ConstantMin
 * - 'Max' for ConstantMax
 * - 'Max' for Parachute
 * - ('Max'+'Min')/2 for Servos
 * - 'Disarmed' for the rest
 * 
 *
 * @group Actuator Outputs
 * @value 0 Disabled
 * @value 1 Constant Min
 * @value 2 Constant Max
 * @value 101 Motor 1
 * @value 102 Motor 2
 * @value 103 Motor 3
 * @value 104 Motor 4
 * @value 105 Motor 5
 * @value 106 Motor 6
 * @value 107 Motor 7
 * @value 108 Motor 8
 * @value 109 Motor 9
 * @value 110 Motor 10
 * @value 111 Motor 11
 * @value 112 Motor 12
 * @value 201 Servo 1
 * @value 202 Servo 2
 * @value 203 Servo 3
 * @value 204 Servo 4
 * @value 205 Servo 5
 * @value 206 Servo 6
 * @value 207 Servo 7
 * @value 208 Servo 8
 * @value 301 Offboard Actuator Set 1
 * @value 302 Offboard Actuator Set 2
 * @value 303 Offboard Actuator Set 3
 * @value 304 Offboard Actuator Set 4
 * @value 305 Offboard Actuator Set 5
 * @value 306 Offboard Actuator Set 6
 * @value 400 Landing Gear
 * @value 401 Parachute
 * @value 402 RC Roll
 * @value 403 RC Pitch
 * @value 404 RC Throttle
 * @value 405 RC Yaw
 * @value 406 RC Flaps
 * @value 407 RC AUX 1
 * @value 408 RC AUX 2
 * @value 409 RC AUX 3
 * @value 410 RC AUX 4
 * @value 411 RC AUX 5
 * @value 412 RC AUX 6
 * @value 420 Gimbal Roll
 * @value 421 Gimbal Pitch
 * @value 422 Gimbal Yaw
 * @value 430 Gripper
 * @value 440 Landing Gear Wheel
 */
PARAM_DEFINE_INT32(SIM_GZ_SV_FUNC2, 0);

/**
 * SIM_GZ Servo 3 Output Function
 *
 * Select what should be output on SIM_GZ Servo 3.
 * 
 * The default failsafe value is set according to the selected function:
 * - 'Min' for ConstantMin
 * - 'Max' for ConstantMax
 * - 'Max' for Parachute
 * - ('Max'+'Min')/2 for Servos
 * - 'Disarmed' for the rest
 * 
 *
 * @group Actuator Outputs
 * @value 0 Disabled
 * @value 1 Constant Min
 * @value 2 Constant Max
 * @value 101 Motor 1
 * @value 102 Motor 2
 * @value 103 Motor 3
 * @value 104 Motor 4
 * @value 105 Motor 5
 * @value 106 Motor 6
 * @value 107 Motor 7
 * @value 108 Motor 8
 * @value 109 Motor 9
 * @value 110 Motor 10
 * @value 111 Motor 11
 * @value 112 Motor 12
 * @value 201 Servo 1
 * @value 202 Servo 2
 * @value 203 Servo 3
 * @value 204 Servo 4
 * @value 205 Servo 5
 * @value 206 Servo 6
 * @value 207 Servo 7
 * @value 208 Servo 8
 * @value 301 Offboard Actuator Set 1
 * @value 302 Offboard Actuator Set 2
 * @value 303 Offboard Actuator Set 3
 * @value 304 Offboard Actuator Set 4
 * @value 305 Offboard Actuator Set 5
 * @value 306 Offboard Actuator Set 6
 * @value 400 Landing Gear
 * @value 401 Parachute
 * @value 402 RC Roll
 * @value 403 RC Pitch
 * @value 404 RC Throttle
 * @value 405 RC Yaw
 * @value 406 RC Flaps
 * @value 407 RC AUX 1
 * @value 408 RC AUX 2
 * @value 409 RC AUX 3
 * @value 410 RC AUX 4
 * @value 411 RC AUX 5
 * @value 412 RC AUX 6
 * @value 420 Gimbal Roll
 * @value 421 Gimbal Pitch
 * @value 422 Gimbal Yaw
 * @value 430 Gripper
 * @value 440 Landing Gear Wheel
 */
PARAM_DEFINE_INT32(SIM_GZ_SV_FUNC3, 0);

/**
 * SIM_GZ Servo 4 Output Function
 *
 * Select what should be output on SIM_GZ Servo 4.
 * 
 * The default failsafe value is set according to the selected function:
 * - 'Min' for ConstantMin
 * - 'Max' for ConstantMax
 * - 'Max' for Parachute
 * - ('Max'+'Min')/2 for Servos
 * - 'Disarmed' for the rest
 * 
 *
 * @group Actuator Outputs
 * @value 0 Disabled
 * @value 1 Constant Min
 * @value 2 Constant Max
 * @value 101 Motor 1
 * @value 102 Motor 2
 * @value 103 Motor 3
 * @value 104 Motor 4
 * @value 105 Motor 5
 * @value 106 Motor 6
 * @value 107 Motor 7
 * @value 108 Motor 8
 * @value 109 Motor 9
 * @value 110 Motor 10
 * @value 111 Motor 11
 * @value 112 Motor 12
 * @value 201 Servo 1
 * @value 202 Servo 2
 * @value 203 Servo 3
 * @value 204 Servo 4
 * @value 205 Servo 5
 * @value 206 Servo 6
 * @value 207 Servo 7
 * @value 208 Servo 8
 * @value 301 Offboard Actuator Set 1
 * @value 302 Offboard Actuator Set 2
 * @value 303 Offboard Actuator Set 3
 * @value 304 Offboard Actuator Set 4
 * @value 305 Offboard Actuator Set 5
 * @value 306 Offboard Actuator Set 6
 * @value 400 Landing Gear
 * @value 401 Parachute
 * @value 402 RC Roll
 * @value 403 RC Pitch
 * @value 404 RC Throttle
 * @value 405 RC Yaw
 * @value 406 RC Flaps
 * @value 407 RC AUX 1
 * @value 408 RC AUX 2
 * @value 409 RC AUX 3
 * @value 410 RC AUX 4
 * @value 411 RC AUX 5
 * @value 412 RC AUX 6
 * @value 420 Gimbal Roll
 * @value 421 Gimbal Pitch
 * @value 422 Gimbal Yaw
 * @value 430 Gripper
 * @value 440 Landing Gear Wheel
 */
PARAM_DEFINE_INT32(SIM_GZ_SV_FUNC4, 0);

/**
 * SIM_GZ Servo 5 Output Function
 *
 * Select what should be output on SIM_GZ Servo 5.
 * 
 * The default failsafe value is set according to the selected function:
 * - 'Min' for ConstantMin
 * - 'Max' for ConstantMax
 * - 'Max' for Parachute
 * - ('Max'+'Min')/2 for Servos
 * - 'Disarmed' for the rest
 * 
 *
 * @group Actuator Outputs
 * @value 0 Disabled
 * @value 1 Constant Min
 * @value 2 Constant Max
 * @value 101 Motor 1
 * @value 102 Motor 2
 * @value 103 Motor 3
 * @value 104 Motor 4
 * @value 105 Motor 5
 * @value 106 Motor 6
 * @value 107 Motor 7
 * @value 108 Motor 8
 * @value 109 Motor 9
 * @value 110 Motor 10
 * @value 111 Motor 11
 * @value 112 Motor 12
 * @value 201 Servo 1
 * @value 202 Servo 2
 * @value 203 Servo 3
 * @value 204 Servo 4
 * @value 205 Servo 5
 * @value 206 Servo 6
 * @value 207 Servo 7
 * @value 208 Servo 8
 * @value 301 Offboard Actuator Set 1
 * @value 302 Offboard Actuator Set 2
 * @value 303 Offboard Actuator Set 3
 * @value 304 Offboard Actuator Set 4
 * @value 305 Offboard Actuator Set 5
 * @value 306 Offboard Actuator Set 6
 * @value 400 Landing Gear
 * @value 401 Parachute
 * @value 402 RC Roll
 * @value 403 RC Pitch
 * @value 404 RC Throttle
 * @value 405 RC Yaw
 * @value 406 RC Flaps
 * @value 407 RC AUX 1
 * @value 408 RC AUX 2
 * @value 409 RC AUX 3
 * @value 410 RC AUX 4
 * @value 411 RC AUX 5
 * @value 412 RC AUX 6
 * @value 420 Gimbal Roll
 * @value 421 Gimbal Pitch
 * @value 422 Gimbal Yaw
 * @value 430 Gripper
 * @value 440 Landing Gear Wheel
 */
PARAM_DEFINE_INT32(SIM_GZ_SV_FUNC5, 0);

/**
 * SIM_GZ Servo 6 Output Function
 *
 * Select what should be output on SIM_GZ Servo 6.
 * 
 * The default failsafe value is set according to the selected function:
 * - 'Min' for ConstantMin
 * - 'Max' for ConstantMax
 * - 'Max' for Parachute
 * - ('Max'+'Min')/2 for Servos
 * - 'Disarmed' for the rest
 * 
 *
 * @group Actuator Outputs
 * @value 0 Disabled
 * @value 1 Constant Min
 * @value 2 Constant Max
 * @value 101 Motor 1
 * @value 102 Motor 2
 * @value 103 Motor 3
 * @value 104 Motor 4
 * @value 105 Motor 5
 * @value 106 Motor 6
 * @value 107 Motor 7
 * @value 108 Motor 8
 * @value 109 Motor 9
 * @value 110 Motor 10
 * @value 111 Motor 11
 * @value 112 Motor 12
 * @value 201 Servo 1
 * @value 202 Servo 2
 * @value 203 Servo 3
 * @value 204 Servo 4
 * @value 205 Servo 5
 * @value 206 Servo 6
 * @value 207 Servo 7
 * @value 208 Servo 8
 * @value 301 Offboard Actuator Set 1
 * @value 302 Offboard Actuator Set 2
 * @value 303 Offboard Actuator Set 3
 * @value 304 Offboard Actuator Set 4
 * @value 305 Offboard Actuator Set 5
 * @value 306 Offboard Actuator Set 6
 * @value 400 Landing Gear
 * @value 401 Parachute
 * @value 402 RC Roll
 * @value 403 RC Pitch
 * @value 404 RC Throttle
 * @value 405 RC Yaw
 * @value 406 RC Flaps
 * @value 407 RC AUX 1
 * @value 408 RC AUX 2
 * @value 409 RC AUX 3
 * @value 410 RC AUX 4
 * @value 411 RC AUX 5
 * @value 412 RC AUX 6
 * @value 420 Gimbal Roll
 * @value 421 Gimbal Pitch
 * @value 422 Gimbal Yaw
 * @value 430 Gripper
 * @value 440 Landing Gear Wheel
 */
PARAM_DEFINE_INT32(SIM_GZ_SV_FUNC6, 0);

/**
 * SIM_GZ Servo 7 Output Function
 *
 * Select what should be output on SIM_GZ Servo 7.
 * 
 * The default failsafe value is set according to the selected function:
 * - 'Min' for ConstantMin
 * - 'Max' for ConstantMax
 * - 'Max' for Parachute
 * - ('Max'+'Min')/2 for Servos
 * - 'Disarmed' for the rest
 * 
 *
 * @group Actuator Outputs
 * @value 0 Disabled
 * @value 1 Constant Min
 * @value 2 Constant Max
 * @value 101 Motor 1
 * @value 102 Motor 2
 * @value 103 Motor 3
 * @value 104 Motor 4
 * @value 105 Motor 5
 * @value 106 Motor 6
 * @value 107 Motor 7
 * @value 108 Motor 8
 * @value 109 Motor 9
 * @value 110 Motor 10
 * @value 111 Motor 11
 * @value 112 Motor 12
 * @value 201 Servo 1
 * @value 202 Servo 2
 * @value 203 Servo 3
 * @value 204 Servo 4
 * @value 205 Servo 5
 * @value 206 Servo 6
 * @value 207 Servo 7
 * @value 208 Servo 8
 * @value 301 Offboard Actuator Set 1
 * @value 302 Offboard Actuator Set 2
 * @value 303 Offboard Actuator Set 3
 * @value 304 Offboard Actuator Set 4
 * @value 305 Offboard Actuator Set 5
 * @value 306 Offboard Actuator Set 6
 * @value 400 Landing Gear
 * @value 401 Parachute
 * @value 402 RC Roll
 * @value 403 RC Pitch
 * @value 404 RC Throttle
 * @value 405 RC Yaw
 * @value 406 RC Flaps
 * @value 407 RC AUX 1
 * @value 408 RC AUX 2
 * @value 409 RC AUX 3
 * @value 410 RC AUX 4
 * @value 411 RC AUX 5
 * @value 412 RC AUX 6
 * @value 420 Gimbal Roll
 * @value 421 Gimbal Pitch
 * @value 422 Gimbal Yaw
 * @value 430 Gripper
 * @value 440 Landing Gear Wheel
 */
PARAM_DEFINE_INT32(SIM_GZ_SV_FUNC7, 0);

/**
 * SIM_GZ Servo 8 Output Function
 *
 * Select what should be output on SIM_GZ Servo 8.
 * 
 * The default failsafe value is set according to the selected function:
 * - 'Min' for ConstantMin
 * - 'Max' for ConstantMax
 * - 'Max' for Parachute
 * - ('Max'+'Min')/2 for Servos
 * - 'Disarmed' for the rest
 * 
 *
 * @group Actuator Outputs
 * @value 0 Disabled
 * @value 1 Constant Min
 * @value 2 Constant Max
 * @value 101 Motor 1
 * @value 102 Motor 2
 * @value 103 Motor 3
 * @value 104 Motor 4
 * @value 105 Motor 5
 * @value 106 Motor 6
 * @value 107 Motor 7
 * @value 108 Motor 8
 * @value 109 Motor 9
 * @value 110 Motor 10
 * @value 111 Motor 11
 * @value 112 Motor 12
 * @value 201 Servo 1
 * @value 202 Servo 2
 * @value 203 Servo 3
 * @value 204 Servo 4
 * @value 205 Servo 5
 * @value 206 Servo 6
 * @value 207 Servo 7
 * @value 208 Servo 8
 * @value 301 Offboard Actuator Set 1
 * @value 302 Offboard Actuator Set 2
 * @value 303 Offboard Actuator Set 3
 * @value 304 Offboard Actuator Set 4
 * @value 305 Offboard Actuator Set 5
 * @value 306 Offboard Actuator Set 6
 * @value 400 Landing Gear
 * @value 401 Parachute
 * @value 402 RC Roll
 * @value 403 RC Pitch
 * @value 404 RC Throttle
 * @value 405 RC Yaw
 * @value 406 RC Flaps
 * @value 407 RC AUX 1
 * @value 408 RC AUX 2
 * @value 409 RC AUX 3
 * @value 410 RC AUX 4
 * @value 411 RC AUX 5
 * @value 412 RC AUX 6
 * @value 420 Gimbal Roll
 * @value 421 Gimbal Pitch
 * @value 422 Gimbal Yaw
 * @value 430 Gripper
 * @value 440 Landing Gear Wheel
 */
PARAM_DEFINE_INT32(SIM_GZ_SV_FUNC8, 0);

/**
 * SIM_GZ Servo 1 Disarmed Value
 *
 * This is the output value that is set when not armed.
 * 
 * Note that non-motor outputs might already be active in prearm state if COM_PREARM_MODE is set.
 * 
 *
 * @group Actuator Outputs
 * @min 0
 * @max 1000
 */
PARAM_DEFINE_INT32(SIM_GZ_SV_DIS1, 500);

/**
 * SIM_GZ Servo 2 Disarmed Value
 *
 * This is the output value that is set when not armed.
 * 
 * Note that non-motor outputs might already be active in prearm state if COM_PREARM_MODE is set.
 * 
 *
 * @group Actuator Outputs
 * @min 0
 * @max 1000
 */
PARAM_DEFINE_INT32(SIM_GZ_SV_DIS2, 500);

/**
 * SIM_GZ Servo 3 Disarmed Value
 *
 * This is the output value that is set when not armed.
 * 
 * Note that non-motor outputs might already be active in prearm state if COM_PREARM_MODE is set.
 * 
 *
 * @group Actuator Outputs
 * @min 0
 * @max 1000
 */
PARAM_DEFINE_INT32(SIM_GZ_SV_DIS3, 500);

/**
 * SIM_GZ Servo 4 Disarmed Value
 *
 * This is the output value that is set when not armed.
 * 
 * Note that non-motor outputs might already be active in prearm state if COM_PREARM_MODE is set.
 * 
 *
 * @group Actuator Outputs
 * @min 0
 * @max 1000
 */
PARAM_DEFINE_INT32(SIM_GZ_SV_DIS4, 500);

/**
 * SIM_GZ Servo 5 Disarmed Value
 *
 * This is the output value that is set when not armed.
 * 
 * Note that non-motor outputs might already be active in prearm state if COM_PREARM_MODE is set.
 * 
 *
 * @group Actuator Outputs
 * @min 0
 * @max 1000
 */
PARAM_DEFINE_INT32(SIM_GZ_SV_DIS5, 500);

/**
 * SIM_GZ Servo 6 Disarmed Value
 *
 * This is the output value that is set when not armed.
 * 
 * Note that non-motor outputs might already be active in prearm state if COM_PREARM_MODE is set.
 * 
 *
 * @group Actuator Outputs
 * @min 0
 * @max 1000
 */
PARAM_DEFINE_INT32(SIM_GZ_SV_DIS6, 500);

/**
 * SIM_GZ Servo 7 Disarmed Value
 *
 * This is the output value that is set when not armed.
 * 
 * Note that non-motor outputs might already be active in prearm state if COM_PREARM_MODE is set.
 * 
 *
 * @group Actuator Outputs
 * @min 0
 * @max 1000
 */
PARAM_DEFINE_INT32(SIM_GZ_SV_DIS7, 500);

/**
 * SIM_GZ Servo 8 Disarmed Value
 *
 * This is the output value that is set when not armed.
 * 
 * Note that non-motor outputs might already be active in prearm state if COM_PREARM_MODE is set.
 * 
 *
 * @group Actuator Outputs
 * @min 0
 * @max 1000
 */
PARAM_DEFINE_INT32(SIM_GZ_SV_DIS8, 500);

/**
 * SIM_GZ Servo 1 Minimum Value
 *
 * Minimum output value (when not disarmed).
 * 
 *
 * @group Actuator Outputs
 * @min 0
 * @max 1000
 */
PARAM_DEFINE_INT32(SIM_GZ_SV_MIN1, 0);

/**
 * SIM_GZ Servo 2 Minimum Value
 *
 * Minimum output value (when not disarmed).
 * 
 *
 * @group Actuator Outputs
 * @min 0
 * @max 1000
 */
PARAM_DEFINE_INT32(SIM_GZ_SV_MIN2, 0);

/**
 * SIM_GZ Servo 3 Minimum Value
 *
 * Minimum output value (when not disarmed).
 * 
 *
 * @group Actuator Outputs
 * @min 0
 * @max 1000
 */
PARAM_DEFINE_INT32(SIM_GZ_SV_MIN3, 0);

/**
 * SIM_GZ Servo 4 Minimum Value
 *
 * Minimum output value (when not disarmed).
 * 
 *
 * @group Actuator Outputs
 * @min 0
 * @max 1000
 */
PARAM_DEFINE_INT32(SIM_GZ_SV_MIN4, 0);

/**
 * SIM_GZ Servo 5 Minimum Value
 *
 * Minimum output value (when not disarmed).
 * 
 *
 * @group Actuator Outputs
 * @min 0
 * @max 1000
 */
PARAM_DEFINE_INT32(SIM_GZ_SV_MIN5, 0);

/**
 * SIM_GZ Servo 6 Minimum Value
 *
 * Minimum output value (when not disarmed).
 * 
 *
 * @group Actuator Outputs
 * @min 0
 * @max 1000
 */
PARAM_DEFINE_INT32(SIM_GZ_SV_MIN6, 0);

/**
 * SIM_GZ Servo 7 Minimum Value
 *
 * Minimum output value (when not disarmed).
 * 
 *
 * @group Actuator Outputs
 * @min 0
 * @max 1000
 */
PARAM_DEFINE_INT32(SIM_GZ_SV_MIN7, 0);

/**
 * SIM_GZ Servo 8 Minimum Value
 *
 * Minimum output value (when not disarmed).
 * 
 *
 * @group Actuator Outputs
 * @min 0
 * @max 1000
 */
PARAM_DEFINE_INT32(SIM_GZ_SV_MIN8, 0);

/**
 * SIM_GZ Servo 1 Maximum Value
 *
 * Maxmimum output value (when not disarmed).
 * 
 *
 * @group Actuator Outputs
 * @min 0
 * @max 1000
 */
PARAM_DEFINE_INT32(SIM_GZ_SV_MAX1, 1000);

/**
 * SIM_GZ Servo 2 Maximum Value
 *
 * Maxmimum output value (when not disarmed).
 * 
 *
 * @group Actuator Outputs
 * @min 0
 * @max 1000
 */
PARAM_DEFINE_INT32(SIM_GZ_SV_MAX2, 1000);

/**
 * SIM_GZ Servo 3 Maximum Value
 *
 * Maxmimum output value (when not disarmed).
 * 
 *
 * @group Actuator Outputs
 * @min 0
 * @max 1000
 */
PARAM_DEFINE_INT32(SIM_GZ_SV_MAX3, 1000);

/**
 * SIM_GZ Servo 4 Maximum Value
 *
 * Maxmimum output value (when not disarmed).
 * 
 *
 * @group Actuator Outputs
 * @min 0
 * @max 1000
 */
PARAM_DEFINE_INT32(SIM_GZ_SV_MAX4, 1000);

/**
 * SIM_GZ Servo 5 Maximum Value
 *
 * Maxmimum output value (when not disarmed).
 * 
 *
 * @group Actuator Outputs
 * @min 0
 * @max 1000
 */
PARAM_DEFINE_INT32(SIM_GZ_SV_MAX5, 1000);

/**
 * SIM_GZ Servo 6 Maximum Value
 *
 * Maxmimum output value (when not disarmed).
 * 
 *
 * @group Actuator Outputs
 * @min 0
 * @max 1000
 */
PARAM_DEFINE_INT32(SIM_GZ_SV_MAX6, 1000);

/**
 * SIM_GZ Servo 7 Maximum Value
 *
 * Maxmimum output value (when not disarmed).
 * 
 *
 * @group Actuator Outputs
 * @min 0
 * @max 1000
 */
PARAM_DEFINE_INT32(SIM_GZ_SV_MAX7, 1000);

/**
 * SIM_GZ Servo 8 Maximum Value
 *
 * Maxmimum output value (when not disarmed).
 * 
 *
 * @group Actuator Outputs
 * @min 0
 * @max 1000
 */
PARAM_DEFINE_INT32(SIM_GZ_SV_MAX8, 1000);

/**
 * SIM_GZ Servo 1 Failsafe Value
 *
 * This is the output value that is set when in failsafe mode.
 * 
 * When set to -1 (default), the value depends on the function (see SIM_GZ_SV_FUNC1).
 * 
 *
 * @group Actuator Outputs
 * @min -1
 * @max 1000
 */
PARAM_DEFINE_INT32(SIM_GZ_SV_FAIL1, -1);

/**
 * SIM_GZ Servo 2 Failsafe Value
 *
 * This is the output value that is set when in failsafe mode.
 * 
 * When set to -1 (default), the value depends on the function (see SIM_GZ_SV_FUNC2).
 * 
 *
 * @group Actuator Outputs
 * @min -1
 * @max 1000
 */
PARAM_DEFINE_INT32(SIM_GZ_SV_FAIL2, -1);

/**
 * SIM_GZ Servo 3 Failsafe Value
 *
 * This is the output value that is set when in failsafe mode.
 * 
 * When set to -1 (default), the value depends on the function (see SIM_GZ_SV_FUNC3).
 * 
 *
 * @group Actuator Outputs
 * @min -1
 * @max 1000
 */
PARAM_DEFINE_INT32(SIM_GZ_SV_FAIL3, -1);

/**
 * SIM_GZ Servo 4 Failsafe Value
 *
 * This is the output value that is set when in failsafe mode.
 * 
 * When set to -1 (default), the value depends on the function (see SIM_GZ_SV_FUNC4).
 * 
 *
 * @group Actuator Outputs
 * @min -1
 * @max 1000
 */
PARAM_DEFINE_INT32(SIM_GZ_SV_FAIL4, -1);

/**
 * SIM_GZ Servo 5 Failsafe Value
 *
 * This is the output value that is set when in failsafe mode.
 * 
 * When set to -1 (default), the value depends on the function (see SIM_GZ_SV_FUNC5).
 * 
 *
 * @group Actuator Outputs
 * @min -1
 * @max 1000
 */
PARAM_DEFINE_INT32(SIM_GZ_SV_FAIL5, -1);

/**
 * SIM_GZ Servo 6 Failsafe Value
 *
 * This is the output value that is set when in failsafe mode.
 * 
 * When set to -1 (default), the value depends on the function (see SIM_GZ_SV_FUNC6).
 * 
 *
 * @group Actuator Outputs
 * @min -1
 * @max 1000
 */
PARAM_DEFINE_INT32(SIM_GZ_SV_FAIL6, -1);

/**
 * SIM_GZ Servo 7 Failsafe Value
 *
 * This is the output value that is set when in failsafe mode.
 * 
 * When set to -1 (default), the value depends on the function (see SIM_GZ_SV_FUNC7).
 * 
 *
 * @group Actuator Outputs
 * @min -1
 * @max 1000
 */
PARAM_DEFINE_INT32(SIM_GZ_SV_FAIL7, -1);

/**
 * SIM_GZ Servo 8 Failsafe Value
 *
 * This is the output value that is set when in failsafe mode.
 * 
 * When set to -1 (default), the value depends on the function (see SIM_GZ_SV_FUNC8).
 * 
 *
 * @group Actuator Outputs
 * @min -1
 * @max 1000
 */
PARAM_DEFINE_INT32(SIM_GZ_SV_FAIL8, -1);

/**
 * Reverse Output Range for SIM_GZ
 *
 * Allows to reverse the output range for each channel.
 * Note: this is only useful for servos.
 * 
 *
 * @group Actuator Outputs
 * @bit 0 SIM_GZ ESC 1
 * @bit 1 SIM_GZ ESC 2
 * @bit 2 SIM_GZ ESC 3
 * @bit 3 SIM_GZ ESC 4
 * @bit 4 SIM_GZ ESC 5
 * @bit 5 SIM_GZ ESC 6
 * @bit 6 SIM_GZ ESC 7
 * @bit 7 SIM_GZ ESC 8
 * @min 0
 * @max 255
 */
PARAM_DEFINE_INT32(SIM_GZ_EC_REV, 0);

/**
 * Reverse Output Range for SIM_GZ
 *
 * Allows to reverse the output range for each channel.
 * Note: this is only useful for servos.
 * 
 *
 * @group Actuator Outputs
 * @bit 0 SIM_GZ Servo 1
 * @bit 1 SIM_GZ Servo 2
 * @bit 2 SIM_GZ Servo 3
 * @bit 3 SIM_GZ Servo 4
 * @bit 4 SIM_GZ Servo 5
 * @bit 5 SIM_GZ Servo 6
 * @bit 6 SIM_GZ Servo 7
 * @bit 7 SIM_GZ Servo 8
 * @min 0
 * @max 255
 */
PARAM_DEFINE_INT32(SIM_GZ_SV_REV, 0);

/**
 * Battery 1 voltage divider (V divider)
 *
 * This is the divider from battery 1 voltage to ADC voltage.
 * If using e.g. Mauch power modules the value from the datasheet
 * can be applied straight here. A value of -1 means to use
 * the board default.
 * 
 *
 * @group Battery Calibration
 * @decimal 8
 * @reboot_required True
 */
PARAM_DEFINE_FLOAT(BAT1_V_DIV, -1.0);

/**
 * Battery 2 voltage divider (V divider)
 *
 * This is the divider from battery 2 voltage to ADC voltage.
 * If using e.g. Mauch power modules the value from the datasheet
 * can be applied straight here. A value of -1 means to use
 * the board default.
 * 
 *
 * @group Battery Calibration
 * @decimal 8
 * @reboot_required True
 */
PARAM_DEFINE_FLOAT(BAT2_V_DIV, -1.0);

/**
 * Battery 1 current per volt (A/V)
 *
 * The voltage seen by the ADC multiplied by this factor
 * will determine the battery current. A value of -1 means to use
 * the board default.
 * 
 *
 * @group Battery Calibration
 * @decimal 8
 * @reboot_required True
 */
PARAM_DEFINE_FLOAT(BAT1_A_PER_V, -1.0);

/**
 * Battery 2 current per volt (A/V)
 *
 * The voltage seen by the ADC multiplied by this factor
 * will determine the battery current. A value of -1 means to use
 * the board default.
 * 
 *
 * @group Battery Calibration
 * @decimal 8
 * @reboot_required True
 */
PARAM_DEFINE_FLOAT(BAT2_A_PER_V, -1.0);

/**
 * Battery 1 Voltage ADC Channel
 *
 * This parameter specifies the ADC channel used to monitor voltage of main power battery.
 * A value of -1 means to use the board default.
 * 
 *
 * @group Battery Calibration
 * @reboot_required True
 */
PARAM_DEFINE_INT32(BAT1_V_CHANNEL, -1);

/**
 * Battery 2 Voltage ADC Channel
 *
 * This parameter specifies the ADC channel used to monitor voltage of main power battery.
 * A value of -1 means to use the board default.
 * 
 *
 * @group Battery Calibration
 * @reboot_required True
 */
PARAM_DEFINE_INT32(BAT2_V_CHANNEL, -1);

/**
 * Battery 1 Current ADC Channel
 *
 * This parameter specifies the ADC channel used to monitor current of main power battery.
 * A value of -1 means to use the board default.
 * 
 *
 * @group Battery Calibration
 * @reboot_required True
 */
PARAM_DEFINE_INT32(BAT1_I_CHANNEL, -1);

/**
 * Battery 2 Current ADC Channel
 *
 * This parameter specifies the ADC channel used to monitor current of main power battery.
 * A value of -1 means to use the board default.
 * 
 *
 * @group Battery Calibration
 * @reboot_required True
 */
PARAM_DEFINE_INT32(BAT2_I_CHANNEL, -1);

/**
 * RC input protocol
 *
 * Select your RC input protocol or auto to scan.
 * 
 *
 * @group RC Input
 * @value -1 Auto
 * @value 0 None
 * @value 1 PPM
 * @value 2 SBUS
 * @value 3 DSM
 * @value 4 ST24
 * @value 5 SUMD
 * @value 6 CRSF
 * @value 7 GHST
 * @category System
 * @min -1
 * @max 7
 */
PARAM_DEFINE_INT32(RC_INPUT_PROTO, -1);

/**
 * Accelerometer 0 calibration device ID
 *
 * Device ID of the accelerometer this calibration applies to.
 *
 * @group Sensor Calibration
 * @decimal 3
 * @category System
 */
PARAM_DEFINE_INT32(CAL_ACC0_ID, 0);

/**
 * Accelerometer 1 calibration device ID
 *
 * Device ID of the accelerometer this calibration applies to.
 *
 * @group Sensor Calibration
 * @decimal 3
 * @category System
 */
PARAM_DEFINE_INT32(CAL_ACC1_ID, 0);

/**
 * Accelerometer 2 calibration device ID
 *
 * Device ID of the accelerometer this calibration applies to.
 *
 * @group Sensor Calibration
 * @decimal 3
 * @category System
 */
PARAM_DEFINE_INT32(CAL_ACC2_ID, 0);

/**
 * Accelerometer 3 calibration device ID
 *
 * Device ID of the accelerometer this calibration applies to.
 *
 * @group Sensor Calibration
 * @decimal 3
 * @category System
 */
PARAM_DEFINE_INT32(CAL_ACC3_ID, 0);

/**
 * Accelerometer 0 priority
 *
 * 
 *
 * @group Sensor Calibration
 * @value -1 Uninitialized
 * @value 0 Disabled
 * @value 1 Min
 * @value 25 Low
 * @value 50 Medium (Default)
 * @value 75 High
 * @value 100 Max
 * @decimal 3
 * @category System
 */
PARAM_DEFINE_INT32(CAL_ACC0_PRIO, -1);

/**
 * Accelerometer 1 priority
 *
 * 
 *
 * @group Sensor Calibration
 * @value -1 Uninitialized
 * @value 0 Disabled
 * @value 1 Min
 * @value 25 Low
 * @value 50 Medium (Default)
 * @value 75 High
 * @value 100 Max
 * @decimal 3
 * @category System
 */
PARAM_DEFINE_INT32(CAL_ACC1_PRIO, -1);

/**
 * Accelerometer 2 priority
 *
 * 
 *
 * @group Sensor Calibration
 * @value -1 Uninitialized
 * @value 0 Disabled
 * @value 1 Min
 * @value 25 Low
 * @value 50 Medium (Default)
 * @value 75 High
 * @value 100 Max
 * @decimal 3
 * @category System
 */
PARAM_DEFINE_INT32(CAL_ACC2_PRIO, -1);

/**
 * Accelerometer 3 priority
 *
 * 
 *
 * @group Sensor Calibration
 * @value -1 Uninitialized
 * @value 0 Disabled
 * @value 1 Min
 * @value 25 Low
 * @value 50 Medium (Default)
 * @value 75 High
 * @value 100 Max
 * @decimal 3
 * @category System
 */
PARAM_DEFINE_INT32(CAL_ACC3_PRIO, -1);

/**
 * Accelerometer 0 rotation relative to airframe
 *
 * An internal sensor will force a value of -1, so a GCS should only attempt to configure the rotation if the value is greater than or equal to zero.
 * 
 *
 * @group Sensor Calibration
 * @value -1 Internal
 * @value 0 No rotation
 * @value 1 Yaw 45°
 * @value 2 Yaw 90°
 * @value 3 Yaw 135°
 * @value 4 Yaw 180°
 * @value 5 Yaw 225°
 * @value 6 Yaw 270°
 * @value 7 Yaw 315°
 * @value 8 Roll 180°
 * @value 9 Roll 180°, Yaw 45°
 * @value 10 Roll 180°, Yaw 90°
 * @value 11 Roll 180°, Yaw 135°
 * @value 12 Pitch 180°
 * @value 13 Roll 180°, Yaw 225°
 * @value 14 Roll 180°, Yaw 270°
 * @value 15 Roll 180°, Yaw 315°
 * @value 16 Roll 90°
 * @value 17 Roll 90°, Yaw 45°
 * @value 18 Roll 90°, Yaw 90°
 * @value 19 Roll 90°, Yaw 135°
 * @value 20 Roll 270°
 * @value 21 Roll 270°, Yaw 45°
 * @value 22 Roll 270°, Yaw 90°
 * @value 23 Roll 270°, Yaw 135°
 * @value 24 Pitch 90°
 * @value 25 Pitch 270°
 * @value 26 Pitch 180°, Yaw 90°
 * @value 27 Pitch 180°, Yaw 270°
 * @value 28 Roll 90°, Pitch 90°
 * @value 29 Roll 180°, Pitch 90°
 * @value 30 Roll 270°, Pitch 90°
 * @value 31 Roll 90°, Pitch 180°
 * @value 32 Roll 270°, Pitch 180°
 * @value 33 Roll 90°, Pitch 270°
 * @value 34 Roll 180°, Pitch 270°
 * @value 35 Roll 270°, Pitch 270°
 * @value 36 Roll 90°, Pitch 180°, Yaw 90°
 * @value 37 Roll 90°, Yaw 270°
 * @value 38 Roll 90°, Pitch 68°, Yaw 293°
 * @value 39 Pitch 315°
 * @value 40 Roll 90°, Pitch 315°
 * @category System
 * @min -1
 * @max 40
 */
PARAM_DEFINE_INT32(CAL_ACC0_ROT, -1);

/**
 * Accelerometer 1 rotation relative to airframe
 *
 * An internal sensor will force a value of -1, so a GCS should only attempt to configure the rotation if the value is greater than or equal to zero.
 * 
 *
 * @group Sensor Calibration
 * @value -1 Internal
 * @value 0 No rotation
 * @value 1 Yaw 45°
 * @value 2 Yaw 90°
 * @value 3 Yaw 135°
 * @value 4 Yaw 180°
 * @value 5 Yaw 225°
 * @value 6 Yaw 270°
 * @value 7 Yaw 315°
 * @value 8 Roll 180°
 * @value 9 Roll 180°, Yaw 45°
 * @value 10 Roll 180°, Yaw 90°
 * @value 11 Roll 180°, Yaw 135°
 * @value 12 Pitch 180°
 * @value 13 Roll 180°, Yaw 225°
 * @value 14 Roll 180°, Yaw 270°
 * @value 15 Roll 180°, Yaw 315°
 * @value 16 Roll 90°
 * @value 17 Roll 90°, Yaw 45°
 * @value 18 Roll 90°, Yaw 90°
 * @value 19 Roll 90°, Yaw 135°
 * @value 20 Roll 270°
 * @value 21 Roll 270°, Yaw 45°
 * @value 22 Roll 270°, Yaw 90°
 * @value 23 Roll 270°, Yaw 135°
 * @value 24 Pitch 90°
 * @value 25 Pitch 270°
 * @value 26 Pitch 180°, Yaw 90°
 * @value 27 Pitch 180°, Yaw 270°
 * @value 28 Roll 90°, Pitch 90°
 * @value 29 Roll 180°, Pitch 90°
 * @value 30 Roll 270°, Pitch 90°
 * @value 31 Roll 90°, Pitch 180°
 * @value 32 Roll 270°, Pitch 180°
 * @value 33 Roll 90°, Pitch 270°
 * @value 34 Roll 180°, Pitch 270°
 * @value 35 Roll 270°, Pitch 270°
 * @value 36 Roll 90°, Pitch 180°, Yaw 90°
 * @value 37 Roll 90°, Yaw 270°
 * @value 38 Roll 90°, Pitch 68°, Yaw 293°
 * @value 39 Pitch 315°
 * @value 40 Roll 90°, Pitch 315°
 * @category System
 * @min -1
 * @max 40
 */
PARAM_DEFINE_INT32(CAL_ACC1_ROT, -1);

/**
 * Accelerometer 2 rotation relative to airframe
 *
 * An internal sensor will force a value of -1, so a GCS should only attempt to configure the rotation if the value is greater than or equal to zero.
 * 
 *
 * @group Sensor Calibration
 * @value -1 Internal
 * @value 0 No rotation
 * @value 1 Yaw 45°
 * @value 2 Yaw 90°
 * @value 3 Yaw 135°
 * @value 4 Yaw 180°
 * @value 5 Yaw 225°
 * @value 6 Yaw 270°
 * @value 7 Yaw 315°
 * @value 8 Roll 180°
 * @value 9 Roll 180°, Yaw 45°
 * @value 10 Roll 180°, Yaw 90°
 * @value 11 Roll 180°, Yaw 135°
 * @value 12 Pitch 180°
 * @value 13 Roll 180°, Yaw 225°
 * @value 14 Roll 180°, Yaw 270°
 * @value 15 Roll 180°, Yaw 315°
 * @value 16 Roll 90°
 * @value 17 Roll 90°, Yaw 45°
 * @value 18 Roll 90°, Yaw 90°
 * @value 19 Roll 90°, Yaw 135°
 * @value 20 Roll 270°
 * @value 21 Roll 270°, Yaw 45°
 * @value 22 Roll 270°, Yaw 90°
 * @value 23 Roll 270°, Yaw 135°
 * @value 24 Pitch 90°
 * @value 25 Pitch 270°
 * @value 26 Pitch 180°, Yaw 90°
 * @value 27 Pitch 180°, Yaw 270°
 * @value 28 Roll 90°, Pitch 90°
 * @value 29 Roll 180°, Pitch 90°
 * @value 30 Roll 270°, Pitch 90°
 * @value 31 Roll 90°, Pitch 180°
 * @value 32 Roll 270°, Pitch 180°
 * @value 33 Roll 90°, Pitch 270°
 * @value 34 Roll 180°, Pitch 270°
 * @value 35 Roll 270°, Pitch 270°
 * @value 36 Roll 90°, Pitch 180°, Yaw 90°
 * @value 37 Roll 90°, Yaw 270°
 * @value 38 Roll 90°, Pitch 68°, Yaw 293°
 * @value 39 Pitch 315°
 * @value 40 Roll 90°, Pitch 315°
 * @category System
 * @min -1
 * @max 40
 */
PARAM_DEFINE_INT32(CAL_ACC2_ROT, -1);

/**
 * Accelerometer 3 rotation relative to airframe
 *
 * An internal sensor will force a value of -1, so a GCS should only attempt to configure the rotation if the value is greater than or equal to zero.
 * 
 *
 * @group Sensor Calibration
 * @value -1 Internal
 * @value 0 No rotation
 * @value 1 Yaw 45°
 * @value 2 Yaw 90°
 * @value 3 Yaw 135°
 * @value 4 Yaw 180°
 * @value 5 Yaw 225°
 * @value 6 Yaw 270°
 * @value 7 Yaw 315°
 * @value 8 Roll 180°
 * @value 9 Roll 180°, Yaw 45°
 * @value 10 Roll 180°, Yaw 90°
 * @value 11 Roll 180°, Yaw 135°
 * @value 12 Pitch 180°
 * @value 13 Roll 180°, Yaw 225°
 * @value 14 Roll 180°, Yaw 270°
 * @value 15 Roll 180°, Yaw 315°
 * @value 16 Roll 90°
 * @value 17 Roll 90°, Yaw 45°
 * @value 18 Roll 90°, Yaw 90°
 * @value 19 Roll 90°, Yaw 135°
 * @value 20 Roll 270°
 * @value 21 Roll 270°, Yaw 45°
 * @value 22 Roll 270°, Yaw 90°
 * @value 23 Roll 270°, Yaw 135°
 * @value 24 Pitch 90°
 * @value 25 Pitch 270°
 * @value 26 Pitch 180°, Yaw 90°
 * @value 27 Pitch 180°, Yaw 270°
 * @value 28 Roll 90°, Pitch 90°
 * @value 29 Roll 180°, Pitch 90°
 * @value 30 Roll 270°, Pitch 90°
 * @value 31 Roll 90°, Pitch 180°
 * @value 32 Roll 270°, Pitch 180°
 * @value 33 Roll 90°, Pitch 270°
 * @value 34 Roll 180°, Pitch 270°
 * @value 35 Roll 270°, Pitch 270°
 * @value 36 Roll 90°, Pitch 180°, Yaw 90°
 * @value 37 Roll 90°, Yaw 270°
 * @value 38 Roll 90°, Pitch 68°, Yaw 293°
 * @value 39 Pitch 315°
 * @value 40 Roll 90°, Pitch 315°
 * @category System
 * @min -1
 * @max 40
 */
PARAM_DEFINE_INT32(CAL_ACC3_ROT, -1);

/**
 * Accelerometer 0 X-axis offset
 *
 * 
 *
 * @group Sensor Calibration
 * @decimal 3
 * @category System
 * @volatile True
 * @unit m/s^2
 */
PARAM_DEFINE_FLOAT(CAL_ACC0_XOFF, 0.0);

/**
 * Accelerometer 1 X-axis offset
 *
 * 
 *
 * @group Sensor Calibration
 * @decimal 3
 * @category System
 * @volatile True
 * @unit m/s^2
 */
PARAM_DEFINE_FLOAT(CAL_ACC1_XOFF, 0.0);

/**
 * Accelerometer 2 X-axis offset
 *
 * 
 *
 * @group Sensor Calibration
 * @decimal 3
 * @category System
 * @volatile True
 * @unit m/s^2
 */
PARAM_DEFINE_FLOAT(CAL_ACC2_XOFF, 0.0);

/**
 * Accelerometer 3 X-axis offset
 *
 * 
 *
 * @group Sensor Calibration
 * @decimal 3
 * @category System
 * @volatile True
 * @unit m/s^2
 */
PARAM_DEFINE_FLOAT(CAL_ACC3_XOFF, 0.0);

/**
 * Accelerometer 0 Y-axis offset
 *
 * 
 *
 * @group Sensor Calibration
 * @decimal 3
 * @category System
 * @volatile True
 * @unit m/s^2
 */
PARAM_DEFINE_FLOAT(CAL_ACC0_YOFF, 0.0);

/**
 * Accelerometer 1 Y-axis offset
 *
 * 
 *
 * @group Sensor Calibration
 * @decimal 3
 * @category System
 * @volatile True
 * @unit m/s^2
 */
PARAM_DEFINE_FLOAT(CAL_ACC1_YOFF, 0.0);

/**
 * Accelerometer 2 Y-axis offset
 *
 * 
 *
 * @group Sensor Calibration
 * @decimal 3
 * @category System
 * @volatile True
 * @unit m/s^2
 */
PARAM_DEFINE_FLOAT(CAL_ACC2_YOFF, 0.0);

/**
 * Accelerometer 3 Y-axis offset
 *
 * 
 *
 * @group Sensor Calibration
 * @decimal 3
 * @category System
 * @volatile True
 * @unit m/s^2
 */
PARAM_DEFINE_FLOAT(CAL_ACC3_YOFF, 0.0);

/**
 * Accelerometer 0 Z-axis offset
 *
 * 
 *
 * @group Sensor Calibration
 * @decimal 3
 * @category System
 * @volatile True
 * @unit m/s^2
 */
PARAM_DEFINE_FLOAT(CAL_ACC0_ZOFF, 0.0);

/**
 * Accelerometer 1 Z-axis offset
 *
 * 
 *
 * @group Sensor Calibration
 * @decimal 3
 * @category System
 * @volatile True
 * @unit m/s^2
 */
PARAM_DEFINE_FLOAT(CAL_ACC1_ZOFF, 0.0);

/**
 * Accelerometer 2 Z-axis offset
 *
 * 
 *
 * @group Sensor Calibration
 * @decimal 3
 * @category System
 * @volatile True
 * @unit m/s^2
 */
PARAM_DEFINE_FLOAT(CAL_ACC2_ZOFF, 0.0);

/**
 * Accelerometer 3 Z-axis offset
 *
 * 
 *
 * @group Sensor Calibration
 * @decimal 3
 * @category System
 * @volatile True
 * @unit m/s^2
 */
PARAM_DEFINE_FLOAT(CAL_ACC3_ZOFF, 0.0);

/**
 * Accelerometer 0 X-axis scaling factor
 *
 * 
 *
 * @group Sensor Calibration
 * @decimal 3
 * @category System
 * @volatile True
 * @min 0.1
 * @max 3.0
 */
PARAM_DEFINE_FLOAT(CAL_ACC0_XSCALE, 1.0);

/**
 * Accelerometer 1 X-axis scaling factor
 *
 * 
 *
 * @group Sensor Calibration
 * @decimal 3
 * @category System
 * @volatile True
 * @min 0.1
 * @max 3.0
 */
PARAM_DEFINE_FLOAT(CAL_ACC1_XSCALE, 1.0);

/**
 * Accelerometer 2 X-axis scaling factor
 *
 * 
 *
 * @group Sensor Calibration
 * @decimal 3
 * @category System
 * @volatile True
 * @min 0.1
 * @max 3.0
 */
PARAM_DEFINE_FLOAT(CAL_ACC2_XSCALE, 1.0);

/**
 * Accelerometer 3 X-axis scaling factor
 *
 * 
 *
 * @group Sensor Calibration
 * @decimal 3
 * @category System
 * @volatile True
 * @min 0.1
 * @max 3.0
 */
PARAM_DEFINE_FLOAT(CAL_ACC3_XSCALE, 1.0);

/**
 * Accelerometer 0 Y-axis scaling factor
 *
 * 
 *
 * @group Sensor Calibration
 * @decimal 3
 * @category System
 * @volatile True
 * @min 0.1
 * @max 3.0
 */
PARAM_DEFINE_FLOAT(CAL_ACC0_YSCALE, 1.0);

/**
 * Accelerometer 1 Y-axis scaling factor
 *
 * 
 *
 * @group Sensor Calibration
 * @decimal 3
 * @category System
 * @volatile True
 * @min 0.1
 * @max 3.0
 */
PARAM_DEFINE_FLOAT(CAL_ACC1_YSCALE, 1.0);

/**
 * Accelerometer 2 Y-axis scaling factor
 *
 * 
 *
 * @group Sensor Calibration
 * @decimal 3
 * @category System
 * @volatile True
 * @min 0.1
 * @max 3.0
 */
PARAM_DEFINE_FLOAT(CAL_ACC2_YSCALE, 1.0);

/**
 * Accelerometer 3 Y-axis scaling factor
 *
 * 
 *
 * @group Sensor Calibration
 * @decimal 3
 * @category System
 * @volatile True
 * @min 0.1
 * @max 3.0
 */
PARAM_DEFINE_FLOAT(CAL_ACC3_YSCALE, 1.0);

/**
 * Accelerometer 0 Z-axis scaling factor
 *
 * 
 *
 * @group Sensor Calibration
 * @decimal 3
 * @category System
 * @volatile True
 * @min 0.1
 * @max 3.0
 */
PARAM_DEFINE_FLOAT(CAL_ACC0_ZSCALE, 1.0);

/**
 * Accelerometer 1 Z-axis scaling factor
 *
 * 
 *
 * @group Sensor Calibration
 * @decimal 3
 * @category System
 * @volatile True
 * @min 0.1
 * @max 3.0
 */
PARAM_DEFINE_FLOAT(CAL_ACC1_ZSCALE, 1.0);

/**
 * Accelerometer 2 Z-axis scaling factor
 *
 * 
 *
 * @group Sensor Calibration
 * @decimal 3
 * @category System
 * @volatile True
 * @min 0.1
 * @max 3.0
 */
PARAM_DEFINE_FLOAT(CAL_ACC2_ZSCALE, 1.0);

/**
 * Accelerometer 3 Z-axis scaling factor
 *
 * 
 *
 * @group Sensor Calibration
 * @decimal 3
 * @category System
 * @volatile True
 * @min 0.1
 * @max 3.0
 */
PARAM_DEFINE_FLOAT(CAL_ACC3_ZSCALE, 1.0);

/**
 * Barometer 0 calibration device ID
 *
 * Device ID of the barometer this calibration applies to.
 *
 * @group Sensor Calibration
 * @category System
 */
PARAM_DEFINE_INT32(CAL_BARO0_ID, 0);

/**
 * Barometer 1 calibration device ID
 *
 * Device ID of the barometer this calibration applies to.
 *
 * @group Sensor Calibration
 * @category System
 */
PARAM_DEFINE_INT32(CAL_BARO1_ID, 0);

/**
 * Barometer 2 calibration device ID
 *
 * Device ID of the barometer this calibration applies to.
 *
 * @group Sensor Calibration
 * @category System
 */
PARAM_DEFINE_INT32(CAL_BARO2_ID, 0);

/**
 * Barometer 3 calibration device ID
 *
 * Device ID of the barometer this calibration applies to.
 *
 * @group Sensor Calibration
 * @category System
 */
PARAM_DEFINE_INT32(CAL_BARO3_ID, 0);

/**
 * Barometer 0 priority
 *
 * 
 *
 * @group Sensor Calibration
 * @value -1 Uninitialized
 * @value 0 Disabled
 * @value 1 Min
 * @value 25 Low
 * @value 50 Medium (Default)
 * @value 75 High
 * @value 100 Max
 * @category System
 */
PARAM_DEFINE_INT32(CAL_BARO0_PRIO, -1);

/**
 * Barometer 1 priority
 *
 * 
 *
 * @group Sensor Calibration
 * @value -1 Uninitialized
 * @value 0 Disabled
 * @value 1 Min
 * @value 25 Low
 * @value 50 Medium (Default)
 * @value 75 High
 * @value 100 Max
 * @category System
 */
PARAM_DEFINE_INT32(CAL_BARO1_PRIO, -1);

/**
 * Barometer 2 priority
 *
 * 
 *
 * @group Sensor Calibration
 * @value -1 Uninitialized
 * @value 0 Disabled
 * @value 1 Min
 * @value 25 Low
 * @value 50 Medium (Default)
 * @value 75 High
 * @value 100 Max
 * @category System
 */
PARAM_DEFINE_INT32(CAL_BARO2_PRIO, -1);

/**
 * Barometer 3 priority
 *
 * 
 *
 * @group Sensor Calibration
 * @value -1 Uninitialized
 * @value 0 Disabled
 * @value 1 Min
 * @value 25 Low
 * @value 50 Medium (Default)
 * @value 75 High
 * @value 100 Max
 * @category System
 */
PARAM_DEFINE_INT32(CAL_BARO3_PRIO, -1);

/**
 * Barometer 0 offset
 *
 * 
 *
 * @group Sensor Calibration
 * @decimal 3
 * @category System
 * @volatile True
 */
PARAM_DEFINE_FLOAT(CAL_BARO0_OFF, 0.0);

/**
 * Barometer 1 offset
 *
 * 
 *
 * @group Sensor Calibration
 * @decimal 3
 * @category System
 * @volatile True
 */
PARAM_DEFINE_FLOAT(CAL_BARO1_OFF, 0.0);

/**
 * Barometer 2 offset
 *
 * 
 *
 * @group Sensor Calibration
 * @decimal 3
 * @category System
 * @volatile True
 */
PARAM_DEFINE_FLOAT(CAL_BARO2_OFF, 0.0);

/**
 * Barometer 3 offset
 *
 * 
 *
 * @group Sensor Calibration
 * @decimal 3
 * @category System
 * @volatile True
 */
PARAM_DEFINE_FLOAT(CAL_BARO3_OFF, 0.0);

/**
 * Gyroscope 0 calibration device ID
 *
 * Device ID of the gyroscope this calibration applies to.
 *
 * @group Sensor Calibration
 * @category System
 */
PARAM_DEFINE_INT32(CAL_GYRO0_ID, 0);

/**
 * Gyroscope 1 calibration device ID
 *
 * Device ID of the gyroscope this calibration applies to.
 *
 * @group Sensor Calibration
 * @category System
 */
PARAM_DEFINE_INT32(CAL_GYRO1_ID, 0);

/**
 * Gyroscope 2 calibration device ID
 *
 * Device ID of the gyroscope this calibration applies to.
 *
 * @group Sensor Calibration
 * @category System
 */
PARAM_DEFINE_INT32(CAL_GYRO2_ID, 0);

/**
 * Gyroscope 3 calibration device ID
 *
 * Device ID of the gyroscope this calibration applies to.
 *
 * @group Sensor Calibration
 * @category System
 */
PARAM_DEFINE_INT32(CAL_GYRO3_ID, 0);

/**
 * Gyroscope 0 priority
 *
 * 
 *
 * @group Sensor Calibration
 * @value -1 Uninitialized
 * @value 0 Disabled
 * @value 1 Min
 * @value 25 Low
 * @value 50 Medium (Default)
 * @value 75 High
 * @value 100 Max
 * @category System
 */
PARAM_DEFINE_INT32(CAL_GYRO0_PRIO, -1);

/**
 * Gyroscope 1 priority
 *
 * 
 *
 * @group Sensor Calibration
 * @value -1 Uninitialized
 * @value 0 Disabled
 * @value 1 Min
 * @value 25 Low
 * @value 50 Medium (Default)
 * @value 75 High
 * @value 100 Max
 * @category System
 */
PARAM_DEFINE_INT32(CAL_GYRO1_PRIO, -1);

/**
 * Gyroscope 2 priority
 *
 * 
 *
 * @group Sensor Calibration
 * @value -1 Uninitialized
 * @value 0 Disabled
 * @value 1 Min
 * @value 25 Low
 * @value 50 Medium (Default)
 * @value 75 High
 * @value 100 Max
 * @category System
 */
PARAM_DEFINE_INT32(CAL_GYRO2_PRIO, -1);

/**
 * Gyroscope 3 priority
 *
 * 
 *
 * @group Sensor Calibration
 * @value -1 Uninitialized
 * @value 0 Disabled
 * @value 1 Min
 * @value 25 Low
 * @value 50 Medium (Default)
 * @value 75 High
 * @value 100 Max
 * @category System
 */
PARAM_DEFINE_INT32(CAL_GYRO3_PRIO, -1);

/**
 * Gyroscope 0 rotation relative to airframe
 *
 * An internal sensor will force a value of -1, so a GCS should only attempt to configure the rotation if the value is greater than or equal to zero.
 * 
 *
 * @group Sensor Calibration
 * @value -1 Internal
 * @value 0 No rotation
 * @value 1 Yaw 45°
 * @value 2 Yaw 90°
 * @value 3 Yaw 135°
 * @value 4 Yaw 180°
 * @value 5 Yaw 225°
 * @value 6 Yaw 270°
 * @value 7 Yaw 315°
 * @value 8 Roll 180°
 * @value 9 Roll 180°, Yaw 45°
 * @value 10 Roll 180°, Yaw 90°
 * @value 11 Roll 180°, Yaw 135°
 * @value 12 Pitch 180°
 * @value 13 Roll 180°, Yaw 225°
 * @value 14 Roll 180°, Yaw 270°
 * @value 15 Roll 180°, Yaw 315°
 * @value 16 Roll 90°
 * @value 17 Roll 90°, Yaw 45°
 * @value 18 Roll 90°, Yaw 90°
 * @value 19 Roll 90°, Yaw 135°
 * @value 20 Roll 270°
 * @value 21 Roll 270°, Yaw 45°
 * @value 22 Roll 270°, Yaw 90°
 * @value 23 Roll 270°, Yaw 135°
 * @value 24 Pitch 90°
 * @value 25 Pitch 270°
 * @value 26 Pitch 180°, Yaw 90°
 * @value 27 Pitch 180°, Yaw 270°
 * @value 28 Roll 90°, Pitch 90°
 * @value 29 Roll 180°, Pitch 90°
 * @value 30 Roll 270°, Pitch 90°
 * @value 31 Roll 90°, Pitch 180°
 * @value 32 Roll 270°, Pitch 180°
 * @value 33 Roll 90°, Pitch 270°
 * @value 34 Roll 180°, Pitch 270°
 * @value 35 Roll 270°, Pitch 270°
 * @value 36 Roll 90°, Pitch 180°, Yaw 90°
 * @value 37 Roll 90°, Yaw 270°
 * @value 38 Roll 90°, Pitch 68°, Yaw 293°
 * @value 39 Pitch 315°
 * @value 40 Roll 90°, Pitch 315°
 * @category System
 * @min -1
 * @max 40
 */
PARAM_DEFINE_INT32(CAL_GYRO0_ROT, -1);

/**
 * Gyroscope 1 rotation relative to airframe
 *
 * An internal sensor will force a value of -1, so a GCS should only attempt to configure the rotation if the value is greater than or equal to zero.
 * 
 *
 * @group Sensor Calibration
 * @value -1 Internal
 * @value 0 No rotation
 * @value 1 Yaw 45°
 * @value 2 Yaw 90°
 * @value 3 Yaw 135°
 * @value 4 Yaw 180°
 * @value 5 Yaw 225°
 * @value 6 Yaw 270°
 * @value 7 Yaw 315°
 * @value 8 Roll 180°
 * @value 9 Roll 180°, Yaw 45°
 * @value 10 Roll 180°, Yaw 90°
 * @value 11 Roll 180°, Yaw 135°
 * @value 12 Pitch 180°
 * @value 13 Roll 180°, Yaw 225°
 * @value 14 Roll 180°, Yaw 270°
 * @value 15 Roll 180°, Yaw 315°
 * @value 16 Roll 90°
 * @value 17 Roll 90°, Yaw 45°
 * @value 18 Roll 90°, Yaw 90°
 * @value 19 Roll 90°, Yaw 135°
 * @value 20 Roll 270°
 * @value 21 Roll 270°, Yaw 45°
 * @value 22 Roll 270°, Yaw 90°
 * @value 23 Roll 270°, Yaw 135°
 * @value 24 Pitch 90°
 * @value 25 Pitch 270°
 * @value 26 Pitch 180°, Yaw 90°
 * @value 27 Pitch 180°, Yaw 270°
 * @value 28 Roll 90°, Pitch 90°
 * @value 29 Roll 180°, Pitch 90°
 * @value 30 Roll 270°, Pitch 90°
 * @value 31 Roll 90°, Pitch 180°
 * @value 32 Roll 270°, Pitch 180°
 * @value 33 Roll 90°, Pitch 270°
 * @value 34 Roll 180°, Pitch 270°
 * @value 35 Roll 270°, Pitch 270°
 * @value 36 Roll 90°, Pitch 180°, Yaw 90°
 * @value 37 Roll 90°, Yaw 270°
 * @value 38 Roll 90°, Pitch 68°, Yaw 293°
 * @value 39 Pitch 315°
 * @value 40 Roll 90°, Pitch 315°
 * @category System
 * @min -1
 * @max 40
 */
PARAM_DEFINE_INT32(CAL_GYRO1_ROT, -1);

/**
 * Gyroscope 2 rotation relative to airframe
 *
 * An internal sensor will force a value of -1, so a GCS should only attempt to configure the rotation if the value is greater than or equal to zero.
 * 
 *
 * @group Sensor Calibration
 * @value -1 Internal
 * @value 0 No rotation
 * @value 1 Yaw 45°
 * @value 2 Yaw 90°
 * @value 3 Yaw 135°
 * @value 4 Yaw 180°
 * @value 5 Yaw 225°
 * @value 6 Yaw 270°
 * @value 7 Yaw 315°
 * @value 8 Roll 180°
 * @value 9 Roll 180°, Yaw 45°
 * @value 10 Roll 180°, Yaw 90°
 * @value 11 Roll 180°, Yaw 135°
 * @value 12 Pitch 180°
 * @value 13 Roll 180°, Yaw 225°
 * @value 14 Roll 180°, Yaw 270°
 * @value 15 Roll 180°, Yaw 315°
 * @value 16 Roll 90°
 * @value 17 Roll 90°, Yaw 45°
 * @value 18 Roll 90°, Yaw 90°
 * @value 19 Roll 90°, Yaw 135°
 * @value 20 Roll 270°
 * @value 21 Roll 270°, Yaw 45°
 * @value 22 Roll 270°, Yaw 90°
 * @value 23 Roll 270°, Yaw 135°
 * @value 24 Pitch 90°
 * @value 25 Pitch 270°
 * @value 26 Pitch 180°, Yaw 90°
 * @value 27 Pitch 180°, Yaw 270°
 * @value 28 Roll 90°, Pitch 90°
 * @value 29 Roll 180°, Pitch 90°
 * @value 30 Roll 270°, Pitch 90°
 * @value 31 Roll 90°, Pitch 180°
 * @value 32 Roll 270°, Pitch 180°
 * @value 33 Roll 90°, Pitch 270°
 * @value 34 Roll 180°, Pitch 270°
 * @value 35 Roll 270°, Pitch 270°
 * @value 36 Roll 90°, Pitch 180°, Yaw 90°
 * @value 37 Roll 90°, Yaw 270°
 * @value 38 Roll 90°, Pitch 68°, Yaw 293°
 * @value 39 Pitch 315°
 * @value 40 Roll 90°, Pitch 315°
 * @category System
 * @min -1
 * @max 40
 */
PARAM_DEFINE_INT32(CAL_GYRO2_ROT, -1);

/**
 * Gyroscope 3 rotation relative to airframe
 *
 * An internal sensor will force a value of -1, so a GCS should only attempt to configure the rotation if the value is greater than or equal to zero.
 * 
 *
 * @group Sensor Calibration
 * @value -1 Internal
 * @value 0 No rotation
 * @value 1 Yaw 45°
 * @value 2 Yaw 90°
 * @value 3 Yaw 135°
 * @value 4 Yaw 180°
 * @value 5 Yaw 225°
 * @value 6 Yaw 270°
 * @value 7 Yaw 315°
 * @value 8 Roll 180°
 * @value 9 Roll 180°, Yaw 45°
 * @value 10 Roll 180°, Yaw 90°
 * @value 11 Roll 180°, Yaw 135°
 * @value 12 Pitch 180°
 * @value 13 Roll 180°, Yaw 225°
 * @value 14 Roll 180°, Yaw 270°
 * @value 15 Roll 180°, Yaw 315°
 * @value 16 Roll 90°
 * @value 17 Roll 90°, Yaw 45°
 * @value 18 Roll 90°, Yaw 90°
 * @value 19 Roll 90°, Yaw 135°
 * @value 20 Roll 270°
 * @value 21 Roll 270°, Yaw 45°
 * @value 22 Roll 270°, Yaw 90°
 * @value 23 Roll 270°, Yaw 135°
 * @value 24 Pitch 90°
 * @value 25 Pitch 270°
 * @value 26 Pitch 180°, Yaw 90°
 * @value 27 Pitch 180°, Yaw 270°
 * @value 28 Roll 90°, Pitch 90°
 * @value 29 Roll 180°, Pitch 90°
 * @value 30 Roll 270°, Pitch 90°
 * @value 31 Roll 90°, Pitch 180°
 * @value 32 Roll 270°, Pitch 180°
 * @value 33 Roll 90°, Pitch 270°
 * @value 34 Roll 180°, Pitch 270°
 * @value 35 Roll 270°, Pitch 270°
 * @value 36 Roll 90°, Pitch 180°, Yaw 90°
 * @value 37 Roll 90°, Yaw 270°
 * @value 38 Roll 90°, Pitch 68°, Yaw 293°
 * @value 39 Pitch 315°
 * @value 40 Roll 90°, Pitch 315°
 * @category System
 * @min -1
 * @max 40
 */
PARAM_DEFINE_INT32(CAL_GYRO3_ROT, -1);

/**
 * Gyroscope 0 X-axis offset
 *
 * 
 *
 * @group Sensor Calibration
 * @decimal 3
 * @category System
 * @volatile True
 * @unit rad/s
 */
PARAM_DEFINE_FLOAT(CAL_GYRO0_XOFF, 0.0);

/**
 * Gyroscope 1 X-axis offset
 *
 * 
 *
 * @group Sensor Calibration
 * @decimal 3
 * @category System
 * @volatile True
 * @unit rad/s
 */
PARAM_DEFINE_FLOAT(CAL_GYRO1_XOFF, 0.0);

/**
 * Gyroscope 2 X-axis offset
 *
 * 
 *
 * @group Sensor Calibration
 * @decimal 3
 * @category System
 * @volatile True
 * @unit rad/s
 */
PARAM_DEFINE_FLOAT(CAL_GYRO2_XOFF, 0.0);

/**
 * Gyroscope 3 X-axis offset
 *
 * 
 *
 * @group Sensor Calibration
 * @decimal 3
 * @category System
 * @volatile True
 * @unit rad/s
 */
PARAM_DEFINE_FLOAT(CAL_GYRO3_XOFF, 0.0);

/**
 * Gyroscope 0 Y-axis offset
 *
 * 
 *
 * @group Sensor Calibration
 * @decimal 3
 * @category System
 * @volatile True
 * @unit rad/s
 */
PARAM_DEFINE_FLOAT(CAL_GYRO0_YOFF, 0.0);

/**
 * Gyroscope 1 Y-axis offset
 *
 * 
 *
 * @group Sensor Calibration
 * @decimal 3
 * @category System
 * @volatile True
 * @unit rad/s
 */
PARAM_DEFINE_FLOAT(CAL_GYRO1_YOFF, 0.0);

/**
 * Gyroscope 2 Y-axis offset
 *
 * 
 *
 * @group Sensor Calibration
 * @decimal 3
 * @category System
 * @volatile True
 * @unit rad/s
 */
PARAM_DEFINE_FLOAT(CAL_GYRO2_YOFF, 0.0);

/**
 * Gyroscope 3 Y-axis offset
 *
 * 
 *
 * @group Sensor Calibration
 * @decimal 3
 * @category System
 * @volatile True
 * @unit rad/s
 */
PARAM_DEFINE_FLOAT(CAL_GYRO3_YOFF, 0.0);

/**
 * Gyroscope 0 Z-axis offset
 *
 * 
 *
 * @group Sensor Calibration
 * @decimal 3
 * @category System
 * @volatile True
 * @unit rad/s
 */
PARAM_DEFINE_FLOAT(CAL_GYRO0_ZOFF, 0.0);

/**
 * Gyroscope 1 Z-axis offset
 *
 * 
 *
 * @group Sensor Calibration
 * @decimal 3
 * @category System
 * @volatile True
 * @unit rad/s
 */
PARAM_DEFINE_FLOAT(CAL_GYRO1_ZOFF, 0.0);

/**
 * Gyroscope 2 Z-axis offset
 *
 * 
 *
 * @group Sensor Calibration
 * @decimal 3
 * @category System
 * @volatile True
 * @unit rad/s
 */
PARAM_DEFINE_FLOAT(CAL_GYRO2_ZOFF, 0.0);

/**
 * Gyroscope 3 Z-axis offset
 *
 * 
 *
 * @group Sensor Calibration
 * @decimal 3
 * @category System
 * @volatile True
 * @unit rad/s
 */
PARAM_DEFINE_FLOAT(CAL_GYRO3_ZOFF, 0.0);

/**
 * Magnetometer 0 calibration device ID
 *
 * Device ID of the magnetometer this calibration applies to.
 *
 * @group Sensor Calibration
 * @category System
 */
PARAM_DEFINE_INT32(CAL_MAG0_ID, 0);

/**
 * Magnetometer 1 calibration device ID
 *
 * Device ID of the magnetometer this calibration applies to.
 *
 * @group Sensor Calibration
 * @category System
 */
PARAM_DEFINE_INT32(CAL_MAG1_ID, 0);

/**
 * Magnetometer 2 calibration device ID
 *
 * Device ID of the magnetometer this calibration applies to.
 *
 * @group Sensor Calibration
 * @category System
 */
PARAM_DEFINE_INT32(CAL_MAG2_ID, 0);

/**
 * Magnetometer 3 calibration device ID
 *
 * Device ID of the magnetometer this calibration applies to.
 *
 * @group Sensor Calibration
 * @category System
 */
PARAM_DEFINE_INT32(CAL_MAG3_ID, 0);

/**
 * Magnetometer 0 priority
 *
 * 
 *
 * @group Sensor Calibration
 * @value -1 Uninitialized
 * @value 0 Disabled
 * @value 1 Min
 * @value 25 Low
 * @value 50 Medium (Default)
 * @value 75 High
 * @value 100 Max
 * @category System
 */
PARAM_DEFINE_INT32(CAL_MAG0_PRIO, -1);

/**
 * Magnetometer 1 priority
 *
 * 
 *
 * @group Sensor Calibration
 * @value -1 Uninitialized
 * @value 0 Disabled
 * @value 1 Min
 * @value 25 Low
 * @value 50 Medium (Default)
 * @value 75 High
 * @value 100 Max
 * @category System
 */
PARAM_DEFINE_INT32(CAL_MAG1_PRIO, -1);

/**
 * Magnetometer 2 priority
 *
 * 
 *
 * @group Sensor Calibration
 * @value -1 Uninitialized
 * @value 0 Disabled
 * @value 1 Min
 * @value 25 Low
 * @value 50 Medium (Default)
 * @value 75 High
 * @value 100 Max
 * @category System
 */
PARAM_DEFINE_INT32(CAL_MAG2_PRIO, -1);

/**
 * Magnetometer 3 priority
 *
 * 
 *
 * @group Sensor Calibration
 * @value -1 Uninitialized
 * @value 0 Disabled
 * @value 1 Min
 * @value 25 Low
 * @value 50 Medium (Default)
 * @value 75 High
 * @value 100 Max
 * @category System
 */
PARAM_DEFINE_INT32(CAL_MAG3_PRIO, -1);

/**
 * Magnetometer 0 rotation relative to airframe
 *
 * An internal sensor will force a value of -1, so a GCS should only attempt to configure the rotation if the value is greater than or equal to zero.
 * 
 *
 * @group Sensor Calibration
 * @value -1 Internal
 * @value 0 No rotation
 * @value 1 Yaw 45°
 * @value 2 Yaw 90°
 * @value 3 Yaw 135°
 * @value 4 Yaw 180°
 * @value 5 Yaw 225°
 * @value 6 Yaw 270°
 * @value 7 Yaw 315°
 * @value 8 Roll 180°
 * @value 9 Roll 180°, Yaw 45°
 * @value 10 Roll 180°, Yaw 90°
 * @value 11 Roll 180°, Yaw 135°
 * @value 12 Pitch 180°
 * @value 13 Roll 180°, Yaw 225°
 * @value 14 Roll 180°, Yaw 270°
 * @value 15 Roll 180°, Yaw 315°
 * @value 16 Roll 90°
 * @value 17 Roll 90°, Yaw 45°
 * @value 18 Roll 90°, Yaw 90°
 * @value 19 Roll 90°, Yaw 135°
 * @value 20 Roll 270°
 * @value 21 Roll 270°, Yaw 45°
 * @value 22 Roll 270°, Yaw 90°
 * @value 23 Roll 270°, Yaw 135°
 * @value 24 Pitch 90°
 * @value 25 Pitch 270°
 * @value 26 Pitch 180°, Yaw 90°
 * @value 27 Pitch 180°, Yaw 270°
 * @value 28 Roll 90°, Pitch 90°
 * @value 29 Roll 180°, Pitch 90°
 * @value 30 Roll 270°, Pitch 90°
 * @value 31 Roll 90°, Pitch 180°
 * @value 32 Roll 270°, Pitch 180°
 * @value 33 Roll 90°, Pitch 270°
 * @value 34 Roll 180°, Pitch 270°
 * @value 35 Roll 270°, Pitch 270°
 * @value 36 Roll 90°, Pitch 180°, Yaw 90°
 * @value 37 Roll 90°, Yaw 270°
 * @value 38 Roll 90°, Pitch 68°, Yaw 293°
 * @value 39 Pitch 315°
 * @value 40 Roll 90°, Pitch 315°
 * @category System
 * @min -1
 * @max 40
 */
PARAM_DEFINE_INT32(CAL_MAG0_ROT, -1);

/**
 * Magnetometer 1 rotation relative to airframe
 *
 * An internal sensor will force a value of -1, so a GCS should only attempt to configure the rotation if the value is greater than or equal to zero.
 * 
 *
 * @group Sensor Calibration
 * @value -1 Internal
 * @value 0 No rotation
 * @value 1 Yaw 45°
 * @value 2 Yaw 90°
 * @value 3 Yaw 135°
 * @value 4 Yaw 180°
 * @value 5 Yaw 225°
 * @value 6 Yaw 270°
 * @value 7 Yaw 315°
 * @value 8 Roll 180°
 * @value 9 Roll 180°, Yaw 45°
 * @value 10 Roll 180°, Yaw 90°
 * @value 11 Roll 180°, Yaw 135°
 * @value 12 Pitch 180°
 * @value 13 Roll 180°, Yaw 225°
 * @value 14 Roll 180°, Yaw 270°
 * @value 15 Roll 180°, Yaw 315°
 * @value 16 Roll 90°
 * @value 17 Roll 90°, Yaw 45°
 * @value 18 Roll 90°, Yaw 90°
 * @value 19 Roll 90°, Yaw 135°
 * @value 20 Roll 270°
 * @value 21 Roll 270°, Yaw 45°
 * @value 22 Roll 270°, Yaw 90°
 * @value 23 Roll 270°, Yaw 135°
 * @value 24 Pitch 90°
 * @value 25 Pitch 270°
 * @value 26 Pitch 180°, Yaw 90°
 * @value 27 Pitch 180°, Yaw 270°
 * @value 28 Roll 90°, Pitch 90°
 * @value 29 Roll 180°, Pitch 90°
 * @value 30 Roll 270°, Pitch 90°
 * @value 31 Roll 90°, Pitch 180°
 * @value 32 Roll 270°, Pitch 180°
 * @value 33 Roll 90°, Pitch 270°
 * @value 34 Roll 180°, Pitch 270°
 * @value 35 Roll 270°, Pitch 270°
 * @value 36 Roll 90°, Pitch 180°, Yaw 90°
 * @value 37 Roll 90°, Yaw 270°
 * @value 38 Roll 90°, Pitch 68°, Yaw 293°
 * @value 39 Pitch 315°
 * @value 40 Roll 90°, Pitch 315°
 * @category System
 * @min -1
 * @max 40
 */
PARAM_DEFINE_INT32(CAL_MAG1_ROT, -1);

/**
 * Magnetometer 2 rotation relative to airframe
 *
 * An internal sensor will force a value of -1, so a GCS should only attempt to configure the rotation if the value is greater than or equal to zero.
 * 
 *
 * @group Sensor Calibration
 * @value -1 Internal
 * @value 0 No rotation
 * @value 1 Yaw 45°
 * @value 2 Yaw 90°
 * @value 3 Yaw 135°
 * @value 4 Yaw 180°
 * @value 5 Yaw 225°
 * @value 6 Yaw 270°
 * @value 7 Yaw 315°
 * @value 8 Roll 180°
 * @value 9 Roll 180°, Yaw 45°
 * @value 10 Roll 180°, Yaw 90°
 * @value 11 Roll 180°, Yaw 135°
 * @value 12 Pitch 180°
 * @value 13 Roll 180°, Yaw 225°
 * @value 14 Roll 180°, Yaw 270°
 * @value 15 Roll 180°, Yaw 315°
 * @value 16 Roll 90°
 * @value 17 Roll 90°, Yaw 45°
 * @value 18 Roll 90°, Yaw 90°
 * @value 19 Roll 90°, Yaw 135°
 * @value 20 Roll 270°
 * @value 21 Roll 270°, Yaw 45°
 * @value 22 Roll 270°, Yaw 90°
 * @value 23 Roll 270°, Yaw 135°
 * @value 24 Pitch 90°
 * @value 25 Pitch 270°
 * @value 26 Pitch 180°, Yaw 90°
 * @value 27 Pitch 180°, Yaw 270°
 * @value 28 Roll 90°, Pitch 90°
 * @value 29 Roll 180°, Pitch 90°
 * @value 30 Roll 270°, Pitch 90°
 * @value 31 Roll 90°, Pitch 180°
 * @value 32 Roll 270°, Pitch 180°
 * @value 33 Roll 90°, Pitch 270°
 * @value 34 Roll 180°, Pitch 270°
 * @value 35 Roll 270°, Pitch 270°
 * @value 36 Roll 90°, Pitch 180°, Yaw 90°
 * @value 37 Roll 90°, Yaw 270°
 * @value 38 Roll 90°, Pitch 68°, Yaw 293°
 * @value 39 Pitch 315°
 * @value 40 Roll 90°, Pitch 315°
 * @category System
 * @min -1
 * @max 40
 */
PARAM_DEFINE_INT32(CAL_MAG2_ROT, -1);

/**
 * Magnetometer 3 rotation relative to airframe
 *
 * An internal sensor will force a value of -1, so a GCS should only attempt to configure the rotation if the value is greater than or equal to zero.
 * 
 *
 * @group Sensor Calibration
 * @value -1 Internal
 * @value 0 No rotation
 * @value 1 Yaw 45°
 * @value 2 Yaw 90°
 * @value 3 Yaw 135°
 * @value 4 Yaw 180°
 * @value 5 Yaw 225°
 * @value 6 Yaw 270°
 * @value 7 Yaw 315°
 * @value 8 Roll 180°
 * @value 9 Roll 180°, Yaw 45°
 * @value 10 Roll 180°, Yaw 90°
 * @value 11 Roll 180°, Yaw 135°
 * @value 12 Pitch 180°
 * @value 13 Roll 180°, Yaw 225°
 * @value 14 Roll 180°, Yaw 270°
 * @value 15 Roll 180°, Yaw 315°
 * @value 16 Roll 90°
 * @value 17 Roll 90°, Yaw 45°
 * @value 18 Roll 90°, Yaw 90°
 * @value 19 Roll 90°, Yaw 135°
 * @value 20 Roll 270°
 * @value 21 Roll 270°, Yaw 45°
 * @value 22 Roll 270°, Yaw 90°
 * @value 23 Roll 270°, Yaw 135°
 * @value 24 Pitch 90°
 * @value 25 Pitch 270°
 * @value 26 Pitch 180°, Yaw 90°
 * @value 27 Pitch 180°, Yaw 270°
 * @value 28 Roll 90°, Pitch 90°
 * @value 29 Roll 180°, Pitch 90°
 * @value 30 Roll 270°, Pitch 90°
 * @value 31 Roll 90°, Pitch 180°
 * @value 32 Roll 270°, Pitch 180°
 * @value 33 Roll 90°, Pitch 270°
 * @value 34 Roll 180°, Pitch 270°
 * @value 35 Roll 270°, Pitch 270°
 * @value 36 Roll 90°, Pitch 180°, Yaw 90°
 * @value 37 Roll 90°, Yaw 270°
 * @value 38 Roll 90°, Pitch 68°, Yaw 293°
 * @value 39 Pitch 315°
 * @value 40 Roll 90°, Pitch 315°
 * @category System
 * @min -1
 * @max 40
 */
PARAM_DEFINE_INT32(CAL_MAG3_ROT, -1);

/**
 * Magnetometer 0 X-axis offset
 *
 * 
 *
 * @group Sensor Calibration
 * @decimal 3
 * @category System
 * @volatile True
 * @unit gauss
 */
PARAM_DEFINE_FLOAT(CAL_MAG0_XOFF, 0.0);

/**
 * Magnetometer 1 X-axis offset
 *
 * 
 *
 * @group Sensor Calibration
 * @decimal 3
 * @category System
 * @volatile True
 * @unit gauss
 */
PARAM_DEFINE_FLOAT(CAL_MAG1_XOFF, 0.0);

/**
 * Magnetometer 2 X-axis offset
 *
 * 
 *
 * @group Sensor Calibration
 * @decimal 3
 * @category System
 * @volatile True
 * @unit gauss
 */
PARAM_DEFINE_FLOAT(CAL_MAG2_XOFF, 0.0);

/**
 * Magnetometer 3 X-axis offset
 *
 * 
 *
 * @group Sensor Calibration
 * @decimal 3
 * @category System
 * @volatile True
 * @unit gauss
 */
PARAM_DEFINE_FLOAT(CAL_MAG3_XOFF, 0.0);

/**
 * Magnetometer 0 Y-axis offset
 *
 * 
 *
 * @group Sensor Calibration
 * @decimal 3
 * @category System
 * @volatile True
 * @unit gauss
 */
PARAM_DEFINE_FLOAT(CAL_MAG0_YOFF, 0.0);

/**
 * Magnetometer 1 Y-axis offset
 *
 * 
 *
 * @group Sensor Calibration
 * @decimal 3
 * @category System
 * @volatile True
 * @unit gauss
 */
PARAM_DEFINE_FLOAT(CAL_MAG1_YOFF, 0.0);

/**
 * Magnetometer 2 Y-axis offset
 *
 * 
 *
 * @group Sensor Calibration
 * @decimal 3
 * @category System
 * @volatile True
 * @unit gauss
 */
PARAM_DEFINE_FLOAT(CAL_MAG2_YOFF, 0.0);

/**
 * Magnetometer 3 Y-axis offset
 *
 * 
 *
 * @group Sensor Calibration
 * @decimal 3
 * @category System
 * @volatile True
 * @unit gauss
 */
PARAM_DEFINE_FLOAT(CAL_MAG3_YOFF, 0.0);

/**
 * Magnetometer 0 Z-axis offset
 *
 * 
 *
 * @group Sensor Calibration
 * @decimal 3
 * @category System
 * @volatile True
 * @unit gauss
 */
PARAM_DEFINE_FLOAT(CAL_MAG0_ZOFF, 0.0);

/**
 * Magnetometer 1 Z-axis offset
 *
 * 
 *
 * @group Sensor Calibration
 * @decimal 3
 * @category System
 * @volatile True
 * @unit gauss
 */
PARAM_DEFINE_FLOAT(CAL_MAG1_ZOFF, 0.0);

/**
 * Magnetometer 2 Z-axis offset
 *
 * 
 *
 * @group Sensor Calibration
 * @decimal 3
 * @category System
 * @volatile True
 * @unit gauss
 */
PARAM_DEFINE_FLOAT(CAL_MAG2_ZOFF, 0.0);

/**
 * Magnetometer 3 Z-axis offset
 *
 * 
 *
 * @group Sensor Calibration
 * @decimal 3
 * @category System
 * @volatile True
 * @unit gauss
 */
PARAM_DEFINE_FLOAT(CAL_MAG3_ZOFF, 0.0);

/**
 * Magnetometer 0 X-axis scaling factor
 *
 * 
 *
 * @group Sensor Calibration
 * @decimal 3
 * @category System
 * @volatile True
 * @min 0.1
 * @max 3.0
 */
PARAM_DEFINE_FLOAT(CAL_MAG0_XSCALE, 1.0);

/**
 * Magnetometer 1 X-axis scaling factor
 *
 * 
 *
 * @group Sensor Calibration
 * @decimal 3
 * @category System
 * @volatile True
 * @min 0.1
 * @max 3.0
 */
PARAM_DEFINE_FLOAT(CAL_MAG1_XSCALE, 1.0);

/**
 * Magnetometer 2 X-axis scaling factor
 *
 * 
 *
 * @group Sensor Calibration
 * @decimal 3
 * @category System
 * @volatile True
 * @min 0.1
 * @max 3.0
 */
PARAM_DEFINE_FLOAT(CAL_MAG2_XSCALE, 1.0);

/**
 * Magnetometer 3 X-axis scaling factor
 *
 * 
 *
 * @group Sensor Calibration
 * @decimal 3
 * @category System
 * @volatile True
 * @min 0.1
 * @max 3.0
 */
PARAM_DEFINE_FLOAT(CAL_MAG3_XSCALE, 1.0);

/**
 * Magnetometer 0 Y-axis scaling factor
 *
 * 
 *
 * @group Sensor Calibration
 * @decimal 3
 * @category System
 * @volatile True
 * @min 0.1
 * @max 3.0
 */
PARAM_DEFINE_FLOAT(CAL_MAG0_YSCALE, 1.0);

/**
 * Magnetometer 1 Y-axis scaling factor
 *
 * 
 *
 * @group Sensor Calibration
 * @decimal 3
 * @category System
 * @volatile True
 * @min 0.1
 * @max 3.0
 */
PARAM_DEFINE_FLOAT(CAL_MAG1_YSCALE, 1.0);

/**
 * Magnetometer 2 Y-axis scaling factor
 *
 * 
 *
 * @group Sensor Calibration
 * @decimal 3
 * @category System
 * @volatile True
 * @min 0.1
 * @max 3.0
 */
PARAM_DEFINE_FLOAT(CAL_MAG2_YSCALE, 1.0);

/**
 * Magnetometer 3 Y-axis scaling factor
 *
 * 
 *
 * @group Sensor Calibration
 * @decimal 3
 * @category System
 * @volatile True
 * @min 0.1
 * @max 3.0
 */
PARAM_DEFINE_FLOAT(CAL_MAG3_YSCALE, 1.0);

/**
 * Magnetometer 0 Z-axis scaling factor
 *
 * 
 *
 * @group Sensor Calibration
 * @decimal 3
 * @category System
 * @volatile True
 * @min 0.1
 * @max 3.0
 */
PARAM_DEFINE_FLOAT(CAL_MAG0_ZSCALE, 1.0);

/**
 * Magnetometer 1 Z-axis scaling factor
 *
 * 
 *
 * @group Sensor Calibration
 * @decimal 3
 * @category System
 * @volatile True
 * @min 0.1
 * @max 3.0
 */
PARAM_DEFINE_FLOAT(CAL_MAG1_ZSCALE, 1.0);

/**
 * Magnetometer 2 Z-axis scaling factor
 *
 * 
 *
 * @group Sensor Calibration
 * @decimal 3
 * @category System
 * @volatile True
 * @min 0.1
 * @max 3.0
 */
PARAM_DEFINE_FLOAT(CAL_MAG2_ZSCALE, 1.0);

/**
 * Magnetometer 3 Z-axis scaling factor
 *
 * 
 *
 * @group Sensor Calibration
 * @decimal 3
 * @category System
 * @volatile True
 * @min 0.1
 * @max 3.0
 */
PARAM_DEFINE_FLOAT(CAL_MAG3_ZSCALE, 1.0);

/**
 * Magnetometer 0 X-axis off diagonal scale factor
 *
 * 
 *
 * @group Sensor Calibration
 * @decimal 3
 * @category System
 * @volatile True
 */
PARAM_DEFINE_FLOAT(CAL_MAG0_XODIAG, 0.0);

/**
 * Magnetometer 1 X-axis off diagonal scale factor
 *
 * 
 *
 * @group Sensor Calibration
 * @decimal 3
 * @category System
 * @volatile True
 */
PARAM_DEFINE_FLOAT(CAL_MAG1_XODIAG, 0.0);

/**
 * Magnetometer 2 X-axis off diagonal scale factor
 *
 * 
 *
 * @group Sensor Calibration
 * @decimal 3
 * @category System
 * @volatile True
 */
PARAM_DEFINE_FLOAT(CAL_MAG2_XODIAG, 0.0);

/**
 * Magnetometer 3 X-axis off diagonal scale factor
 *
 * 
 *
 * @group Sensor Calibration
 * @decimal 3
 * @category System
 * @volatile True
 */
PARAM_DEFINE_FLOAT(CAL_MAG3_XODIAG, 0.0);

/**
 * Magnetometer 0 Y-axis off diagonal scale factor
 *
 * 
 *
 * @group Sensor Calibration
 * @decimal 3
 * @category System
 * @volatile True
 */
PARAM_DEFINE_FLOAT(CAL_MAG0_YODIAG, 0.0);

/**
 * Magnetometer 1 Y-axis off diagonal scale factor
 *
 * 
 *
 * @group Sensor Calibration
 * @decimal 3
 * @category System
 * @volatile True
 */
PARAM_DEFINE_FLOAT(CAL_MAG1_YODIAG, 0.0);

/**
 * Magnetometer 2 Y-axis off diagonal scale factor
 *
 * 
 *
 * @group Sensor Calibration
 * @decimal 3
 * @category System
 * @volatile True
 */
PARAM_DEFINE_FLOAT(CAL_MAG2_YODIAG, 0.0);

/**
 * Magnetometer 3 Y-axis off diagonal scale factor
 *
 * 
 *
 * @group Sensor Calibration
 * @decimal 3
 * @category System
 * @volatile True
 */
PARAM_DEFINE_FLOAT(CAL_MAG3_YODIAG, 0.0);

/**
 * Magnetometer 0 Z-axis off diagonal scale factor
 *
 * 
 *
 * @group Sensor Calibration
 * @category System
 * @volatile True
 */
PARAM_DEFINE_FLOAT(CAL_MAG0_ZODIAG, 0.0);

/**
 * Magnetometer 1 Z-axis off diagonal scale factor
 *
 * 
 *
 * @group Sensor Calibration
 * @category System
 * @volatile True
 */
PARAM_DEFINE_FLOAT(CAL_MAG1_ZODIAG, 0.0);

/**
 * Magnetometer 2 Z-axis off diagonal scale factor
 *
 * 
 *
 * @group Sensor Calibration
 * @category System
 * @volatile True
 */
PARAM_DEFINE_FLOAT(CAL_MAG2_ZODIAG, 0.0);

/**
 * Magnetometer 3 Z-axis off diagonal scale factor
 *
 * 
 *
 * @group Sensor Calibration
 * @category System
 * @volatile True
 */
PARAM_DEFINE_FLOAT(CAL_MAG3_ZODIAG, 0.0);

/**
 * Magnetometer 0 X Axis throttle compensation
 *
 * Coefficient describing linear relationship between
 * X component of magnetometer in body frame axis
 * and either current or throttle depending on value of CAL_MAG_COMP_TYP.
 * Unit for throttle-based compensation is [G] and
 * for current-based compensation [G/kA]
 * 
 *
 * @group Sensor Calibration
 * @decimal 3
 * @category System
 * @volatile True
 */
PARAM_DEFINE_FLOAT(CAL_MAG0_XCOMP, 0.0);

/**
 * Magnetometer 1 X Axis throttle compensation
 *
 * Coefficient describing linear relationship between
 * X component of magnetometer in body frame axis
 * and either current or throttle depending on value of CAL_MAG_COMP_TYP.
 * Unit for throttle-based compensation is [G] and
 * for current-based compensation [G/kA]
 * 
 *
 * @group Sensor Calibration
 * @decimal 3
 * @category System
 * @volatile True
 */
PARAM_DEFINE_FLOAT(CAL_MAG1_XCOMP, 0.0);

/**
 * Magnetometer 2 X Axis throttle compensation
 *
 * Coefficient describing linear relationship between
 * X component of magnetometer in body frame axis
 * and either current or throttle depending on value of CAL_MAG_COMP_TYP.
 * Unit for throttle-based compensation is [G] and
 * for current-based compensation [G/kA]
 * 
 *
 * @group Sensor Calibration
 * @decimal 3
 * @category System
 * @volatile True
 */
PARAM_DEFINE_FLOAT(CAL_MAG2_XCOMP, 0.0);

/**
 * Magnetometer 3 X Axis throttle compensation
 *
 * Coefficient describing linear relationship between
 * X component of magnetometer in body frame axis
 * and either current or throttle depending on value of CAL_MAG_COMP_TYP.
 * Unit for throttle-based compensation is [G] and
 * for current-based compensation [G/kA]
 * 
 *
 * @group Sensor Calibration
 * @decimal 3
 * @category System
 * @volatile True
 */
PARAM_DEFINE_FLOAT(CAL_MAG3_XCOMP, 0.0);

/**
 * Magnetometer 0 Y Axis throttle compensation
 *
 * Coefficient describing linear relationship between
 * Y component of magnetometer in body frame axis
 * and either current or throttle depending on value of CAL_MAG_COMP_TYP.
 * Unit for throttle-based compensation is [G] and
 * for current-based compensation [G/kA]
 * 
 *
 * @group Sensor Calibration
 * @decimal 3
 * @category System
 * @volatile True
 */
PARAM_DEFINE_FLOAT(CAL_MAG0_YCOMP, 0.0);

/**
 * Magnetometer 1 Y Axis throttle compensation
 *
 * Coefficient describing linear relationship between
 * Y component of magnetometer in body frame axis
 * and either current or throttle depending on value of CAL_MAG_COMP_TYP.
 * Unit for throttle-based compensation is [G] and
 * for current-based compensation [G/kA]
 * 
 *
 * @group Sensor Calibration
 * @decimal 3
 * @category System
 * @volatile True
 */
PARAM_DEFINE_FLOAT(CAL_MAG1_YCOMP, 0.0);

/**
 * Magnetometer 2 Y Axis throttle compensation
 *
 * Coefficient describing linear relationship between
 * Y component of magnetometer in body frame axis
 * and either current or throttle depending on value of CAL_MAG_COMP_TYP.
 * Unit for throttle-based compensation is [G] and
 * for current-based compensation [G/kA]
 * 
 *
 * @group Sensor Calibration
 * @decimal 3
 * @category System
 * @volatile True
 */
PARAM_DEFINE_FLOAT(CAL_MAG2_YCOMP, 0.0);

/**
 * Magnetometer 3 Y Axis throttle compensation
 *
 * Coefficient describing linear relationship between
 * Y component of magnetometer in body frame axis
 * and either current or throttle depending on value of CAL_MAG_COMP_TYP.
 * Unit for throttle-based compensation is [G] and
 * for current-based compensation [G/kA]
 * 
 *
 * @group Sensor Calibration
 * @decimal 3
 * @category System
 * @volatile True
 */
PARAM_DEFINE_FLOAT(CAL_MAG3_YCOMP, 0.0);

/**
 * Magnetometer 0 Z Axis throttle compensation
 *
 * Coefficient describing linear relationship between
 * Z component of magnetometer in body frame axis
 * and either current or throttle depending on value of CAL_MAG_COMP_TYP.
 * Unit for throttle-based compensation is [G] and
 * for current-based compensation [G/kA]
 * 
 *
 * @group Sensor Calibration
 * @decimal 3
 * @category System
 * @volatile True
 */
PARAM_DEFINE_FLOAT(CAL_MAG0_ZCOMP, 0.0);

/**
 * Magnetometer 1 Z Axis throttle compensation
 *
 * Coefficient describing linear relationship between
 * Z component of magnetometer in body frame axis
 * and either current or throttle depending on value of CAL_MAG_COMP_TYP.
 * Unit for throttle-based compensation is [G] and
 * for current-based compensation [G/kA]
 * 
 *
 * @group Sensor Calibration
 * @decimal 3
 * @category System
 * @volatile True
 */
PARAM_DEFINE_FLOAT(CAL_MAG1_ZCOMP, 0.0);

/**
 * Magnetometer 2 Z Axis throttle compensation
 *
 * Coefficient describing linear relationship between
 * Z component of magnetometer in body frame axis
 * and either current or throttle depending on value of CAL_MAG_COMP_TYP.
 * Unit for throttle-based compensation is [G] and
 * for current-based compensation [G/kA]
 * 
 *
 * @group Sensor Calibration
 * @decimal 3
 * @category System
 * @volatile True
 */
PARAM_DEFINE_FLOAT(CAL_MAG2_ZCOMP, 0.0);

/**
 * Magnetometer 3 Z Axis throttle compensation
 *
 * Coefficient describing linear relationship between
 * Z component of magnetometer in body frame axis
 * and either current or throttle depending on value of CAL_MAG_COMP_TYP.
 * Unit for throttle-based compensation is [G] and
 * for current-based compensation [G/kA]
 * 
 *
 * @group Sensor Calibration
 * @decimal 3
 * @category System
 * @volatile True
 */
PARAM_DEFINE_FLOAT(CAL_MAG3_ZCOMP, 0.0);

/**
 * Enable Gripper actuation in Payload Deliverer
 *
 * 
 *
 * @group Payload Deliverer
 * @boolean
 * @reboot_required True
 */
PARAM_DEFINE_INT32(PD_GRIPPER_EN, 0);

/**
 * Type of Gripper (Servo, etc.)
 *
 * 
 *
 * @group Payload Deliverer
 * @value -1 Undefined
 * @value 0 Servo
 * @min -1
 * @max 0
 */
PARAM_DEFINE_INT32(PD_GRIPPER_TYPE, 0);

/**
 * Timeout for successful gripper actuation acknowledgement
 *
 * Maximum time Gripper will wait while the successful griper actuation isn't recognised.
 * If the gripper has no feedback sensor, it will simply wait for
 * this time before considering gripper actuation successful and publish a
 * 'VehicleCommandAck' signaling successful gripper action
 * 
 *
 * @group Payload Deliverer
 * @min 0
 * @unit s
 */
PARAM_DEFINE_FLOAT(PD_GRIPPER_TO, 3);

/**
 * UAVCAN ESC 1 Output Function
 *
 * Select what should be output on UAVCAN ESC 1.
 * 
 * The default failsafe value is set according to the selected function:
 * - 'Min' for ConstantMin
 * - 'Max' for ConstantMax
 * - 'Max' for Parachute
 * - ('Max'+'Min')/2 for Servos
 * - 'Disarmed' for the rest
 * 
 *
 * @group Actuator Outputs
 * @value 0 Disabled
 * @value 1 Constant Min
 * @value 2 Constant Max
 * @value 101 Motor 1
 * @value 102 Motor 2
 * @value 103 Motor 3
 * @value 104 Motor 4
 * @value 105 Motor 5
 * @value 106 Motor 6
 * @value 107 Motor 7
 * @value 108 Motor 8
 * @value 109 Motor 9
 * @value 110 Motor 10
 * @value 111 Motor 11
 * @value 112 Motor 12
 * @value 201 Servo 1
 * @value 202 Servo 2
 * @value 203 Servo 3
 * @value 204 Servo 4
 * @value 205 Servo 5
 * @value 206 Servo 6
 * @value 207 Servo 7
 * @value 208 Servo 8
 * @value 301 Offboard Actuator Set 1
 * @value 302 Offboard Actuator Set 2
 * @value 303 Offboard Actuator Set 3
 * @value 304 Offboard Actuator Set 4
 * @value 305 Offboard Actuator Set 5
 * @value 306 Offboard Actuator Set 6
 * @value 400 Landing Gear
 * @value 401 Parachute
 * @value 402 RC Roll
 * @value 403 RC Pitch
 * @value 404 RC Throttle
 * @value 405 RC Yaw
 * @value 406 RC Flaps
 * @value 407 RC AUX 1
 * @value 408 RC AUX 2
 * @value 409 RC AUX 3
 * @value 410 RC AUX 4
 * @value 411 RC AUX 5
 * @value 412 RC AUX 6
 * @value 420 Gimbal Roll
 * @value 421 Gimbal Pitch
 * @value 422 Gimbal Yaw
 * @value 430 Gripper
 * @value 440 Landing Gear Wheel
 */
PARAM_DEFINE_INT32(UAVCAN_EC_FUNC1, 0);

/**
 * UAVCAN ESC 2 Output Function
 *
 * Select what should be output on UAVCAN ESC 2.
 * 
 * The default failsafe value is set according to the selected function:
 * - 'Min' for ConstantMin
 * - 'Max' for ConstantMax
 * - 'Max' for Parachute
 * - ('Max'+'Min')/2 for Servos
 * - 'Disarmed' for the rest
 * 
 *
 * @group Actuator Outputs
 * @value 0 Disabled
 * @value 1 Constant Min
 * @value 2 Constant Max
 * @value 101 Motor 1
 * @value 102 Motor 2
 * @value 103 Motor 3
 * @value 104 Motor 4
 * @value 105 Motor 5
 * @value 106 Motor 6
 * @value 107 Motor 7
 * @value 108 Motor 8
 * @value 109 Motor 9
 * @value 110 Motor 10
 * @value 111 Motor 11
 * @value 112 Motor 12
 * @value 201 Servo 1
 * @value 202 Servo 2
 * @value 203 Servo 3
 * @value 204 Servo 4
 * @value 205 Servo 5
 * @value 206 Servo 6
 * @value 207 Servo 7
 * @value 208 Servo 8
 * @value 301 Offboard Actuator Set 1
 * @value 302 Offboard Actuator Set 2
 * @value 303 Offboard Actuator Set 3
 * @value 304 Offboard Actuator Set 4
 * @value 305 Offboard Actuator Set 5
 * @value 306 Offboard Actuator Set 6
 * @value 400 Landing Gear
 * @value 401 Parachute
 * @value 402 RC Roll
 * @value 403 RC Pitch
 * @value 404 RC Throttle
 * @value 405 RC Yaw
 * @value 406 RC Flaps
 * @value 407 RC AUX 1
 * @value 408 RC AUX 2
 * @value 409 RC AUX 3
 * @value 410 RC AUX 4
 * @value 411 RC AUX 5
 * @value 412 RC AUX 6
 * @value 420 Gimbal Roll
 * @value 421 Gimbal Pitch
 * @value 422 Gimbal Yaw
 * @value 430 Gripper
 * @value 440 Landing Gear Wheel
 */
PARAM_DEFINE_INT32(UAVCAN_EC_FUNC2, 0);

/**
 * UAVCAN ESC 3 Output Function
 *
 * Select what should be output on UAVCAN ESC 3.
 * 
 * The default failsafe value is set according to the selected function:
 * - 'Min' for ConstantMin
 * - 'Max' for ConstantMax
 * - 'Max' for Parachute
 * - ('Max'+'Min')/2 for Servos
 * - 'Disarmed' for the rest
 * 
 *
 * @group Actuator Outputs
 * @value 0 Disabled
 * @value 1 Constant Min
 * @value 2 Constant Max
 * @value 101 Motor 1
 * @value 102 Motor 2
 * @value 103 Motor 3
 * @value 104 Motor 4
 * @value 105 Motor 5
 * @value 106 Motor 6
 * @value 107 Motor 7
 * @value 108 Motor 8
 * @value 109 Motor 9
 * @value 110 Motor 10
 * @value 111 Motor 11
 * @value 112 Motor 12
 * @value 201 Servo 1
 * @value 202 Servo 2
 * @value 203 Servo 3
 * @value 204 Servo 4
 * @value 205 Servo 5
 * @value 206 Servo 6
 * @value 207 Servo 7
 * @value 208 Servo 8
 * @value 301 Offboard Actuator Set 1
 * @value 302 Offboard Actuator Set 2
 * @value 303 Offboard Actuator Set 3
 * @value 304 Offboard Actuator Set 4
 * @value 305 Offboard Actuator Set 5
 * @value 306 Offboard Actuator Set 6
 * @value 400 Landing Gear
 * @value 401 Parachute
 * @value 402 RC Roll
 * @value 403 RC Pitch
 * @value 404 RC Throttle
 * @value 405 RC Yaw
 * @value 406 RC Flaps
 * @value 407 RC AUX 1
 * @value 408 RC AUX 2
 * @value 409 RC AUX 3
 * @value 410 RC AUX 4
 * @value 411 RC AUX 5
 * @value 412 RC AUX 6
 * @value 420 Gimbal Roll
 * @value 421 Gimbal Pitch
 * @value 422 Gimbal Yaw
 * @value 430 Gripper
 * @value 440 Landing Gear Wheel
 */
PARAM_DEFINE_INT32(UAVCAN_EC_FUNC3, 0);

/**
 * UAVCAN ESC 4 Output Function
 *
 * Select what should be output on UAVCAN ESC 4.
 * 
 * The default failsafe value is set according to the selected function:
 * - 'Min' for ConstantMin
 * - 'Max' for ConstantMax
 * - 'Max' for Parachute
 * - ('Max'+'Min')/2 for Servos
 * - 'Disarmed' for the rest
 * 
 *
 * @group Actuator Outputs
 * @value 0 Disabled
 * @value 1 Constant Min
 * @value 2 Constant Max
 * @value 101 Motor 1
 * @value 102 Motor 2
 * @value 103 Motor 3
 * @value 104 Motor 4
 * @value 105 Motor 5
 * @value 106 Motor 6
 * @value 107 Motor 7
 * @value 108 Motor 8
 * @value 109 Motor 9
 * @value 110 Motor 10
 * @value 111 Motor 11
 * @value 112 Motor 12
 * @value 201 Servo 1
 * @value 202 Servo 2
 * @value 203 Servo 3
 * @value 204 Servo 4
 * @value 205 Servo 5
 * @value 206 Servo 6
 * @value 207 Servo 7
 * @value 208 Servo 8
 * @value 301 Offboard Actuator Set 1
 * @value 302 Offboard Actuator Set 2
 * @value 303 Offboard Actuator Set 3
 * @value 304 Offboard Actuator Set 4
 * @value 305 Offboard Actuator Set 5
 * @value 306 Offboard Actuator Set 6
 * @value 400 Landing Gear
 * @value 401 Parachute
 * @value 402 RC Roll
 * @value 403 RC Pitch
 * @value 404 RC Throttle
 * @value 405 RC Yaw
 * @value 406 RC Flaps
 * @value 407 RC AUX 1
 * @value 408 RC AUX 2
 * @value 409 RC AUX 3
 * @value 410 RC AUX 4
 * @value 411 RC AUX 5
 * @value 412 RC AUX 6
 * @value 420 Gimbal Roll
 * @value 421 Gimbal Pitch
 * @value 422 Gimbal Yaw
 * @value 430 Gripper
 * @value 440 Landing Gear Wheel
 */
PARAM_DEFINE_INT32(UAVCAN_EC_FUNC4, 0);

/**
 * UAVCAN ESC 5 Output Function
 *
 * Select what should be output on UAVCAN ESC 5.
 * 
 * The default failsafe value is set according to the selected function:
 * - 'Min' for ConstantMin
 * - 'Max' for ConstantMax
 * - 'Max' for Parachute
 * - ('Max'+'Min')/2 for Servos
 * - 'Disarmed' for the rest
 * 
 *
 * @group Actuator Outputs
 * @value 0 Disabled
 * @value 1 Constant Min
 * @value 2 Constant Max
 * @value 101 Motor 1
 * @value 102 Motor 2
 * @value 103 Motor 3
 * @value 104 Motor 4
 * @value 105 Motor 5
 * @value 106 Motor 6
 * @value 107 Motor 7
 * @value 108 Motor 8
 * @value 109 Motor 9
 * @value 110 Motor 10
 * @value 111 Motor 11
 * @value 112 Motor 12
 * @value 201 Servo 1
 * @value 202 Servo 2
 * @value 203 Servo 3
 * @value 204 Servo 4
 * @value 205 Servo 5
 * @value 206 Servo 6
 * @value 207 Servo 7
 * @value 208 Servo 8
 * @value 301 Offboard Actuator Set 1
 * @value 302 Offboard Actuator Set 2
 * @value 303 Offboard Actuator Set 3
 * @value 304 Offboard Actuator Set 4
 * @value 305 Offboard Actuator Set 5
 * @value 306 Offboard Actuator Set 6
 * @value 400 Landing Gear
 * @value 401 Parachute
 * @value 402 RC Roll
 * @value 403 RC Pitch
 * @value 404 RC Throttle
 * @value 405 RC Yaw
 * @value 406 RC Flaps
 * @value 407 RC AUX 1
 * @value 408 RC AUX 2
 * @value 409 RC AUX 3
 * @value 410 RC AUX 4
 * @value 411 RC AUX 5
 * @value 412 RC AUX 6
 * @value 420 Gimbal Roll
 * @value 421 Gimbal Pitch
 * @value 422 Gimbal Yaw
 * @value 430 Gripper
 * @value 440 Landing Gear Wheel
 */
PARAM_DEFINE_INT32(UAVCAN_EC_FUNC5, 0);

/**
 * UAVCAN ESC 6 Output Function
 *
 * Select what should be output on UAVCAN ESC 6.
 * 
 * The default failsafe value is set according to the selected function:
 * - 'Min' for ConstantMin
 * - 'Max' for ConstantMax
 * - 'Max' for Parachute
 * - ('Max'+'Min')/2 for Servos
 * - 'Disarmed' for the rest
 * 
 *
 * @group Actuator Outputs
 * @value 0 Disabled
 * @value 1 Constant Min
 * @value 2 Constant Max
 * @value 101 Motor 1
 * @value 102 Motor 2
 * @value 103 Motor 3
 * @value 104 Motor 4
 * @value 105 Motor 5
 * @value 106 Motor 6
 * @value 107 Motor 7
 * @value 108 Motor 8
 * @value 109 Motor 9
 * @value 110 Motor 10
 * @value 111 Motor 11
 * @value 112 Motor 12
 * @value 201 Servo 1
 * @value 202 Servo 2
 * @value 203 Servo 3
 * @value 204 Servo 4
 * @value 205 Servo 5
 * @value 206 Servo 6
 * @value 207 Servo 7
 * @value 208 Servo 8
 * @value 301 Offboard Actuator Set 1
 * @value 302 Offboard Actuator Set 2
 * @value 303 Offboard Actuator Set 3
 * @value 304 Offboard Actuator Set 4
 * @value 305 Offboard Actuator Set 5
 * @value 306 Offboard Actuator Set 6
 * @value 400 Landing Gear
 * @value 401 Parachute
 * @value 402 RC Roll
 * @value 403 RC Pitch
 * @value 404 RC Throttle
 * @value 405 RC Yaw
 * @value 406 RC Flaps
 * @value 407 RC AUX 1
 * @value 408 RC AUX 2
 * @value 409 RC AUX 3
 * @value 410 RC AUX 4
 * @value 411 RC AUX 5
 * @value 412 RC AUX 6
 * @value 420 Gimbal Roll
 * @value 421 Gimbal Pitch
 * @value 422 Gimbal Yaw
 * @value 430 Gripper
 * @value 440 Landing Gear Wheel
 */
PARAM_DEFINE_INT32(UAVCAN_EC_FUNC6, 0);

/**
 * UAVCAN ESC 7 Output Function
 *
 * Select what should be output on UAVCAN ESC 7.
 * 
 * The default failsafe value is set according to the selected function:
 * - 'Min' for ConstantMin
 * - 'Max' for ConstantMax
 * - 'Max' for Parachute
 * - ('Max'+'Min')/2 for Servos
 * - 'Disarmed' for the rest
 * 
 *
 * @group Actuator Outputs
 * @value 0 Disabled
 * @value 1 Constant Min
 * @value 2 Constant Max
 * @value 101 Motor 1
 * @value 102 Motor 2
 * @value 103 Motor 3
 * @value 104 Motor 4
 * @value 105 Motor 5
 * @value 106 Motor 6
 * @value 107 Motor 7
 * @value 108 Motor 8
 * @value 109 Motor 9
 * @value 110 Motor 10
 * @value 111 Motor 11
 * @value 112 Motor 12
 * @value 201 Servo 1
 * @value 202 Servo 2
 * @value 203 Servo 3
 * @value 204 Servo 4
 * @value 205 Servo 5
 * @value 206 Servo 6
 * @value 207 Servo 7
 * @value 208 Servo 8
 * @value 301 Offboard Actuator Set 1
 * @value 302 Offboard Actuator Set 2
 * @value 303 Offboard Actuator Set 3
 * @value 304 Offboard Actuator Set 4
 * @value 305 Offboard Actuator Set 5
 * @value 306 Offboard Actuator Set 6
 * @value 400 Landing Gear
 * @value 401 Parachute
 * @value 402 RC Roll
 * @value 403 RC Pitch
 * @value 404 RC Throttle
 * @value 405 RC Yaw
 * @value 406 RC Flaps
 * @value 407 RC AUX 1
 * @value 408 RC AUX 2
 * @value 409 RC AUX 3
 * @value 410 RC AUX 4
 * @value 411 RC AUX 5
 * @value 412 RC AUX 6
 * @value 420 Gimbal Roll
 * @value 421 Gimbal Pitch
 * @value 422 Gimbal Yaw
 * @value 430 Gripper
 * @value 440 Landing Gear Wheel
 */
PARAM_DEFINE_INT32(UAVCAN_EC_FUNC7, 0);

/**
 * UAVCAN ESC 8 Output Function
 *
 * Select what should be output on UAVCAN ESC 8.
 * 
 * The default failsafe value is set according to the selected function:
 * - 'Min' for ConstantMin
 * - 'Max' for ConstantMax
 * - 'Max' for Parachute
 * - ('Max'+'Min')/2 for Servos
 * - 'Disarmed' for the rest
 * 
 *
 * @group Actuator Outputs
 * @value 0 Disabled
 * @value 1 Constant Min
 * @value 2 Constant Max
 * @value 101 Motor 1
 * @value 102 Motor 2
 * @value 103 Motor 3
 * @value 104 Motor 4
 * @value 105 Motor 5
 * @value 106 Motor 6
 * @value 107 Motor 7
 * @value 108 Motor 8
 * @value 109 Motor 9
 * @value 110 Motor 10
 * @value 111 Motor 11
 * @value 112 Motor 12
 * @value 201 Servo 1
 * @value 202 Servo 2
 * @value 203 Servo 3
 * @value 204 Servo 4
 * @value 205 Servo 5
 * @value 206 Servo 6
 * @value 207 Servo 7
 * @value 208 Servo 8
 * @value 301 Offboard Actuator Set 1
 * @value 302 Offboard Actuator Set 2
 * @value 303 Offboard Actuator Set 3
 * @value 304 Offboard Actuator Set 4
 * @value 305 Offboard Actuator Set 5
 * @value 306 Offboard Actuator Set 6
 * @value 400 Landing Gear
 * @value 401 Parachute
 * @value 402 RC Roll
 * @value 403 RC Pitch
 * @value 404 RC Throttle
 * @value 405 RC Yaw
 * @value 406 RC Flaps
 * @value 407 RC AUX 1
 * @value 408 RC AUX 2
 * @value 409 RC AUX 3
 * @value 410 RC AUX 4
 * @value 411 RC AUX 5
 * @value 412 RC AUX 6
 * @value 420 Gimbal Roll
 * @value 421 Gimbal Pitch
 * @value 422 Gimbal Yaw
 * @value 430 Gripper
 * @value 440 Landing Gear Wheel
 */
PARAM_DEFINE_INT32(UAVCAN_EC_FUNC8, 0);

/**
 * UAVCAN ESC 1 Minimum Value
 *
 * Minimum output value (when not disarmed).
 * 
 *
 * @group Actuator Outputs
 * @min 0
 * @max 8191
 */
PARAM_DEFINE_INT32(UAVCAN_EC_MIN1, 1);

/**
 * UAVCAN ESC 2 Minimum Value
 *
 * Minimum output value (when not disarmed).
 * 
 *
 * @group Actuator Outputs
 * @min 0
 * @max 8191
 */
PARAM_DEFINE_INT32(UAVCAN_EC_MIN2, 1);

/**
 * UAVCAN ESC 3 Minimum Value
 *
 * Minimum output value (when not disarmed).
 * 
 *
 * @group Actuator Outputs
 * @min 0
 * @max 8191
 */
PARAM_DEFINE_INT32(UAVCAN_EC_MIN3, 1);

/**
 * UAVCAN ESC 4 Minimum Value
 *
 * Minimum output value (when not disarmed).
 * 
 *
 * @group Actuator Outputs
 * @min 0
 * @max 8191
 */
PARAM_DEFINE_INT32(UAVCAN_EC_MIN4, 1);

/**
 * UAVCAN ESC 5 Minimum Value
 *
 * Minimum output value (when not disarmed).
 * 
 *
 * @group Actuator Outputs
 * @min 0
 * @max 8191
 */
PARAM_DEFINE_INT32(UAVCAN_EC_MIN5, 1);

/**
 * UAVCAN ESC 6 Minimum Value
 *
 * Minimum output value (when not disarmed).
 * 
 *
 * @group Actuator Outputs
 * @min 0
 * @max 8191
 */
PARAM_DEFINE_INT32(UAVCAN_EC_MIN6, 1);

/**
 * UAVCAN ESC 7 Minimum Value
 *
 * Minimum output value (when not disarmed).
 * 
 *
 * @group Actuator Outputs
 * @min 0
 * @max 8191
 */
PARAM_DEFINE_INT32(UAVCAN_EC_MIN7, 1);

/**
 * UAVCAN ESC 8 Minimum Value
 *
 * Minimum output value (when not disarmed).
 * 
 *
 * @group Actuator Outputs
 * @min 0
 * @max 8191
 */
PARAM_DEFINE_INT32(UAVCAN_EC_MIN8, 1);

/**
 * UAVCAN ESC 1 Maximum Value
 *
 * Maxmimum output value (when not disarmed).
 * 
 *
 * @group Actuator Outputs
 * @min 0
 * @max 8191
 */
PARAM_DEFINE_INT32(UAVCAN_EC_MAX1, 8191);

/**
 * UAVCAN ESC 2 Maximum Value
 *
 * Maxmimum output value (when not disarmed).
 * 
 *
 * @group Actuator Outputs
 * @min 0
 * @max 8191
 */
PARAM_DEFINE_INT32(UAVCAN_EC_MAX2, 8191);

/**
 * UAVCAN ESC 3 Maximum Value
 *
 * Maxmimum output value (when not disarmed).
 * 
 *
 * @group Actuator Outputs
 * @min 0
 * @max 8191
 */
PARAM_DEFINE_INT32(UAVCAN_EC_MAX3, 8191);

/**
 * UAVCAN ESC 4 Maximum Value
 *
 * Maxmimum output value (when not disarmed).
 * 
 *
 * @group Actuator Outputs
 * @min 0
 * @max 8191
 */
PARAM_DEFINE_INT32(UAVCAN_EC_MAX4, 8191);

/**
 * UAVCAN ESC 5 Maximum Value
 *
 * Maxmimum output value (when not disarmed).
 * 
 *
 * @group Actuator Outputs
 * @min 0
 * @max 8191
 */
PARAM_DEFINE_INT32(UAVCAN_EC_MAX5, 8191);

/**
 * UAVCAN ESC 6 Maximum Value
 *
 * Maxmimum output value (when not disarmed).
 * 
 *
 * @group Actuator Outputs
 * @min 0
 * @max 8191
 */
PARAM_DEFINE_INT32(UAVCAN_EC_MAX6, 8191);

/**
 * UAVCAN ESC 7 Maximum Value
 *
 * Maxmimum output value (when not disarmed).
 * 
 *
 * @group Actuator Outputs
 * @min 0
 * @max 8191
 */
PARAM_DEFINE_INT32(UAVCAN_EC_MAX7, 8191);

/**
 * UAVCAN ESC 8 Maximum Value
 *
 * Maxmimum output value (when not disarmed).
 * 
 *
 * @group Actuator Outputs
 * @min 0
 * @max 8191
 */
PARAM_DEFINE_INT32(UAVCAN_EC_MAX8, 8191);

/**
 * UAVCAN ESC 1 Failsafe Value
 *
 * This is the output value that is set when in failsafe mode.
 * 
 * When set to -1 (default), the value depends on the function (see UAVCAN_EC_FUNC1).
 * 
 *
 * @group Actuator Outputs
 * @min -1
 * @max 8191
 */
PARAM_DEFINE_INT32(UAVCAN_EC_FAIL1, -1);

/**
 * UAVCAN ESC 2 Failsafe Value
 *
 * This is the output value that is set when in failsafe mode.
 * 
 * When set to -1 (default), the value depends on the function (see UAVCAN_EC_FUNC2).
 * 
 *
 * @group Actuator Outputs
 * @min -1
 * @max 8191
 */
PARAM_DEFINE_INT32(UAVCAN_EC_FAIL2, -1);

/**
 * UAVCAN ESC 3 Failsafe Value
 *
 * This is the output value that is set when in failsafe mode.
 * 
 * When set to -1 (default), the value depends on the function (see UAVCAN_EC_FUNC3).
 * 
 *
 * @group Actuator Outputs
 * @min -1
 * @max 8191
 */
PARAM_DEFINE_INT32(UAVCAN_EC_FAIL3, -1);

/**
 * UAVCAN ESC 4 Failsafe Value
 *
 * This is the output value that is set when in failsafe mode.
 * 
 * When set to -1 (default), the value depends on the function (see UAVCAN_EC_FUNC4).
 * 
 *
 * @group Actuator Outputs
 * @min -1
 * @max 8191
 */
PARAM_DEFINE_INT32(UAVCAN_EC_FAIL4, -1);

/**
 * UAVCAN ESC 5 Failsafe Value
 *
 * This is the output value that is set when in failsafe mode.
 * 
 * When set to -1 (default), the value depends on the function (see UAVCAN_EC_FUNC5).
 * 
 *
 * @group Actuator Outputs
 * @min -1
 * @max 8191
 */
PARAM_DEFINE_INT32(UAVCAN_EC_FAIL5, -1);

/**
 * UAVCAN ESC 6 Failsafe Value
 *
 * This is the output value that is set when in failsafe mode.
 * 
 * When set to -1 (default), the value depends on the function (see UAVCAN_EC_FUNC6).
 * 
 *
 * @group Actuator Outputs
 * @min -1
 * @max 8191
 */
PARAM_DEFINE_INT32(UAVCAN_EC_FAIL6, -1);

/**
 * UAVCAN ESC 7 Failsafe Value
 *
 * This is the output value that is set when in failsafe mode.
 * 
 * When set to -1 (default), the value depends on the function (see UAVCAN_EC_FUNC7).
 * 
 *
 * @group Actuator Outputs
 * @min -1
 * @max 8191
 */
PARAM_DEFINE_INT32(UAVCAN_EC_FAIL7, -1);

/**
 * UAVCAN ESC 8 Failsafe Value
 *
 * This is the output value that is set when in failsafe mode.
 * 
 * When set to -1 (default), the value depends on the function (see UAVCAN_EC_FUNC8).
 * 
 *
 * @group Actuator Outputs
 * @min -1
 * @max 8191
 */
PARAM_DEFINE_INT32(UAVCAN_EC_FAIL8, -1);

/**
 * UAVCAN Servo 1 Output Function
 *
 * Select what should be output on UAVCAN Servo 1.
 * 
 * The default failsafe value is set according to the selected function:
 * - 'Min' for ConstantMin
 * - 'Max' for ConstantMax
 * - 'Max' for Parachute
 * - ('Max'+'Min')/2 for Servos
 * - 'Disarmed' for the rest
 * 
 *
 * @group Actuator Outputs
 * @value 0 Disabled
 * @value 1 Constant Min
 * @value 2 Constant Max
 * @value 101 Motor 1
 * @value 102 Motor 2
 * @value 103 Motor 3
 * @value 104 Motor 4
 * @value 105 Motor 5
 * @value 106 Motor 6
 * @value 107 Motor 7
 * @value 108 Motor 8
 * @value 109 Motor 9
 * @value 110 Motor 10
 * @value 111 Motor 11
 * @value 112 Motor 12
 * @value 201 Servo 1
 * @value 202 Servo 2
 * @value 203 Servo 3
 * @value 204 Servo 4
 * @value 205 Servo 5
 * @value 206 Servo 6
 * @value 207 Servo 7
 * @value 208 Servo 8
 * @value 301 Offboard Actuator Set 1
 * @value 302 Offboard Actuator Set 2
 * @value 303 Offboard Actuator Set 3
 * @value 304 Offboard Actuator Set 4
 * @value 305 Offboard Actuator Set 5
 * @value 306 Offboard Actuator Set 6
 * @value 400 Landing Gear
 * @value 401 Parachute
 * @value 402 RC Roll
 * @value 403 RC Pitch
 * @value 404 RC Throttle
 * @value 405 RC Yaw
 * @value 406 RC Flaps
 * @value 407 RC AUX 1
 * @value 408 RC AUX 2
 * @value 409 RC AUX 3
 * @value 410 RC AUX 4
 * @value 411 RC AUX 5
 * @value 412 RC AUX 6
 * @value 420 Gimbal Roll
 * @value 421 Gimbal Pitch
 * @value 422 Gimbal Yaw
 * @value 430 Gripper
 * @value 440 Landing Gear Wheel
 */
PARAM_DEFINE_INT32(UAVCAN_SV_FUNC1, 0);

/**
 * UAVCAN Servo 2 Output Function
 *
 * Select what should be output on UAVCAN Servo 2.
 * 
 * The default failsafe value is set according to the selected function:
 * - 'Min' for ConstantMin
 * - 'Max' for ConstantMax
 * - 'Max' for Parachute
 * - ('Max'+'Min')/2 for Servos
 * - 'Disarmed' for the rest
 * 
 *
 * @group Actuator Outputs
 * @value 0 Disabled
 * @value 1 Constant Min
 * @value 2 Constant Max
 * @value 101 Motor 1
 * @value 102 Motor 2
 * @value 103 Motor 3
 * @value 104 Motor 4
 * @value 105 Motor 5
 * @value 106 Motor 6
 * @value 107 Motor 7
 * @value 108 Motor 8
 * @value 109 Motor 9
 * @value 110 Motor 10
 * @value 111 Motor 11
 * @value 112 Motor 12
 * @value 201 Servo 1
 * @value 202 Servo 2
 * @value 203 Servo 3
 * @value 204 Servo 4
 * @value 205 Servo 5
 * @value 206 Servo 6
 * @value 207 Servo 7
 * @value 208 Servo 8
 * @value 301 Offboard Actuator Set 1
 * @value 302 Offboard Actuator Set 2
 * @value 303 Offboard Actuator Set 3
 * @value 304 Offboard Actuator Set 4
 * @value 305 Offboard Actuator Set 5
 * @value 306 Offboard Actuator Set 6
 * @value 400 Landing Gear
 * @value 401 Parachute
 * @value 402 RC Roll
 * @value 403 RC Pitch
 * @value 404 RC Throttle
 * @value 405 RC Yaw
 * @value 406 RC Flaps
 * @value 407 RC AUX 1
 * @value 408 RC AUX 2
 * @value 409 RC AUX 3
 * @value 410 RC AUX 4
 * @value 411 RC AUX 5
 * @value 412 RC AUX 6
 * @value 420 Gimbal Roll
 * @value 421 Gimbal Pitch
 * @value 422 Gimbal Yaw
 * @value 430 Gripper
 * @value 440 Landing Gear Wheel
 */
PARAM_DEFINE_INT32(UAVCAN_SV_FUNC2, 0);

/**
 * UAVCAN Servo 3 Output Function
 *
 * Select what should be output on UAVCAN Servo 3.
 * 
 * The default failsafe value is set according to the selected function:
 * - 'Min' for ConstantMin
 * - 'Max' for ConstantMax
 * - 'Max' for Parachute
 * - ('Max'+'Min')/2 for Servos
 * - 'Disarmed' for the rest
 * 
 *
 * @group Actuator Outputs
 * @value 0 Disabled
 * @value 1 Constant Min
 * @value 2 Constant Max
 * @value 101 Motor 1
 * @value 102 Motor 2
 * @value 103 Motor 3
 * @value 104 Motor 4
 * @value 105 Motor 5
 * @value 106 Motor 6
 * @value 107 Motor 7
 * @value 108 Motor 8
 * @value 109 Motor 9
 * @value 110 Motor 10
 * @value 111 Motor 11
 * @value 112 Motor 12
 * @value 201 Servo 1
 * @value 202 Servo 2
 * @value 203 Servo 3
 * @value 204 Servo 4
 * @value 205 Servo 5
 * @value 206 Servo 6
 * @value 207 Servo 7
 * @value 208 Servo 8
 * @value 301 Offboard Actuator Set 1
 * @value 302 Offboard Actuator Set 2
 * @value 303 Offboard Actuator Set 3
 * @value 304 Offboard Actuator Set 4
 * @value 305 Offboard Actuator Set 5
 * @value 306 Offboard Actuator Set 6
 * @value 400 Landing Gear
 * @value 401 Parachute
 * @value 402 RC Roll
 * @value 403 RC Pitch
 * @value 404 RC Throttle
 * @value 405 RC Yaw
 * @value 406 RC Flaps
 * @value 407 RC AUX 1
 * @value 408 RC AUX 2
 * @value 409 RC AUX 3
 * @value 410 RC AUX 4
 * @value 411 RC AUX 5
 * @value 412 RC AUX 6
 * @value 420 Gimbal Roll
 * @value 421 Gimbal Pitch
 * @value 422 Gimbal Yaw
 * @value 430 Gripper
 * @value 440 Landing Gear Wheel
 */
PARAM_DEFINE_INT32(UAVCAN_SV_FUNC3, 0);

/**
 * UAVCAN Servo 4 Output Function
 *
 * Select what should be output on UAVCAN Servo 4.
 * 
 * The default failsafe value is set according to the selected function:
 * - 'Min' for ConstantMin
 * - 'Max' for ConstantMax
 * - 'Max' for Parachute
 * - ('Max'+'Min')/2 for Servos
 * - 'Disarmed' for the rest
 * 
 *
 * @group Actuator Outputs
 * @value 0 Disabled
 * @value 1 Constant Min
 * @value 2 Constant Max
 * @value 101 Motor 1
 * @value 102 Motor 2
 * @value 103 Motor 3
 * @value 104 Motor 4
 * @value 105 Motor 5
 * @value 106 Motor 6
 * @value 107 Motor 7
 * @value 108 Motor 8
 * @value 109 Motor 9
 * @value 110 Motor 10
 * @value 111 Motor 11
 * @value 112 Motor 12
 * @value 201 Servo 1
 * @value 202 Servo 2
 * @value 203 Servo 3
 * @value 204 Servo 4
 * @value 205 Servo 5
 * @value 206 Servo 6
 * @value 207 Servo 7
 * @value 208 Servo 8
 * @value 301 Offboard Actuator Set 1
 * @value 302 Offboard Actuator Set 2
 * @value 303 Offboard Actuator Set 3
 * @value 304 Offboard Actuator Set 4
 * @value 305 Offboard Actuator Set 5
 * @value 306 Offboard Actuator Set 6
 * @value 400 Landing Gear
 * @value 401 Parachute
 * @value 402 RC Roll
 * @value 403 RC Pitch
 * @value 404 RC Throttle
 * @value 405 RC Yaw
 * @value 406 RC Flaps
 * @value 407 RC AUX 1
 * @value 408 RC AUX 2
 * @value 409 RC AUX 3
 * @value 410 RC AUX 4
 * @value 411 RC AUX 5
 * @value 412 RC AUX 6
 * @value 420 Gimbal Roll
 * @value 421 Gimbal Pitch
 * @value 422 Gimbal Yaw
 * @value 430 Gripper
 * @value 440 Landing Gear Wheel
 */
PARAM_DEFINE_INT32(UAVCAN_SV_FUNC4, 0);

/**
 * UAVCAN Servo 5 Output Function
 *
 * Select what should be output on UAVCAN Servo 5.
 * 
 * The default failsafe value is set according to the selected function:
 * - 'Min' for ConstantMin
 * - 'Max' for ConstantMax
 * - 'Max' for Parachute
 * - ('Max'+'Min')/2 for Servos
 * - 'Disarmed' for the rest
 * 
 *
 * @group Actuator Outputs
 * @value 0 Disabled
 * @value 1 Constant Min
 * @value 2 Constant Max
 * @value 101 Motor 1
 * @value 102 Motor 2
 * @value 103 Motor 3
 * @value 104 Motor 4
 * @value 105 Motor 5
 * @value 106 Motor 6
 * @value 107 Motor 7
 * @value 108 Motor 8
 * @value 109 Motor 9
 * @value 110 Motor 10
 * @value 111 Motor 11
 * @value 112 Motor 12
 * @value 201 Servo 1
 * @value 202 Servo 2
 * @value 203 Servo 3
 * @value 204 Servo 4
 * @value 205 Servo 5
 * @value 206 Servo 6
 * @value 207 Servo 7
 * @value 208 Servo 8
 * @value 301 Offboard Actuator Set 1
 * @value 302 Offboard Actuator Set 2
 * @value 303 Offboard Actuator Set 3
 * @value 304 Offboard Actuator Set 4
 * @value 305 Offboard Actuator Set 5
 * @value 306 Offboard Actuator Set 6
 * @value 400 Landing Gear
 * @value 401 Parachute
 * @value 402 RC Roll
 * @value 403 RC Pitch
 * @value 404 RC Throttle
 * @value 405 RC Yaw
 * @value 406 RC Flaps
 * @value 407 RC AUX 1
 * @value 408 RC AUX 2
 * @value 409 RC AUX 3
 * @value 410 RC AUX 4
 * @value 411 RC AUX 5
 * @value 412 RC AUX 6
 * @value 420 Gimbal Roll
 * @value 421 Gimbal Pitch
 * @value 422 Gimbal Yaw
 * @value 430 Gripper
 * @value 440 Landing Gear Wheel
 */
PARAM_DEFINE_INT32(UAVCAN_SV_FUNC5, 0);

/**
 * UAVCAN Servo 6 Output Function
 *
 * Select what should be output on UAVCAN Servo 6.
 * 
 * The default failsafe value is set according to the selected function:
 * - 'Min' for ConstantMin
 * - 'Max' for ConstantMax
 * - 'Max' for Parachute
 * - ('Max'+'Min')/2 for Servos
 * - 'Disarmed' for the rest
 * 
 *
 * @group Actuator Outputs
 * @value 0 Disabled
 * @value 1 Constant Min
 * @value 2 Constant Max
 * @value 101 Motor 1
 * @value 102 Motor 2
 * @value 103 Motor 3
 * @value 104 Motor 4
 * @value 105 Motor 5
 * @value 106 Motor 6
 * @value 107 Motor 7
 * @value 108 Motor 8
 * @value 109 Motor 9
 * @value 110 Motor 10
 * @value 111 Motor 11
 * @value 112 Motor 12
 * @value 201 Servo 1
 * @value 202 Servo 2
 * @value 203 Servo 3
 * @value 204 Servo 4
 * @value 205 Servo 5
 * @value 206 Servo 6
 * @value 207 Servo 7
 * @value 208 Servo 8
 * @value 301 Offboard Actuator Set 1
 * @value 302 Offboard Actuator Set 2
 * @value 303 Offboard Actuator Set 3
 * @value 304 Offboard Actuator Set 4
 * @value 305 Offboard Actuator Set 5
 * @value 306 Offboard Actuator Set 6
 * @value 400 Landing Gear
 * @value 401 Parachute
 * @value 402 RC Roll
 * @value 403 RC Pitch
 * @value 404 RC Throttle
 * @value 405 RC Yaw
 * @value 406 RC Flaps
 * @value 407 RC AUX 1
 * @value 408 RC AUX 2
 * @value 409 RC AUX 3
 * @value 410 RC AUX 4
 * @value 411 RC AUX 5
 * @value 412 RC AUX 6
 * @value 420 Gimbal Roll
 * @value 421 Gimbal Pitch
 * @value 422 Gimbal Yaw
 * @value 430 Gripper
 * @value 440 Landing Gear Wheel
 */
PARAM_DEFINE_INT32(UAVCAN_SV_FUNC6, 0);

/**
 * UAVCAN Servo 7 Output Function
 *
 * Select what should be output on UAVCAN Servo 7.
 * 
 * The default failsafe value is set according to the selected function:
 * - 'Min' for ConstantMin
 * - 'Max' for ConstantMax
 * - 'Max' for Parachute
 * - ('Max'+'Min')/2 for Servos
 * - 'Disarmed' for the rest
 * 
 *
 * @group Actuator Outputs
 * @value 0 Disabled
 * @value 1 Constant Min
 * @value 2 Constant Max
 * @value 101 Motor 1
 * @value 102 Motor 2
 * @value 103 Motor 3
 * @value 104 Motor 4
 * @value 105 Motor 5
 * @value 106 Motor 6
 * @value 107 Motor 7
 * @value 108 Motor 8
 * @value 109 Motor 9
 * @value 110 Motor 10
 * @value 111 Motor 11
 * @value 112 Motor 12
 * @value 201 Servo 1
 * @value 202 Servo 2
 * @value 203 Servo 3
 * @value 204 Servo 4
 * @value 205 Servo 5
 * @value 206 Servo 6
 * @value 207 Servo 7
 * @value 208 Servo 8
 * @value 301 Offboard Actuator Set 1
 * @value 302 Offboard Actuator Set 2
 * @value 303 Offboard Actuator Set 3
 * @value 304 Offboard Actuator Set 4
 * @value 305 Offboard Actuator Set 5
 * @value 306 Offboard Actuator Set 6
 * @value 400 Landing Gear
 * @value 401 Parachute
 * @value 402 RC Roll
 * @value 403 RC Pitch
 * @value 404 RC Throttle
 * @value 405 RC Yaw
 * @value 406 RC Flaps
 * @value 407 RC AUX 1
 * @value 408 RC AUX 2
 * @value 409 RC AUX 3
 * @value 410 RC AUX 4
 * @value 411 RC AUX 5
 * @value 412 RC AUX 6
 * @value 420 Gimbal Roll
 * @value 421 Gimbal Pitch
 * @value 422 Gimbal Yaw
 * @value 430 Gripper
 * @value 440 Landing Gear Wheel
 */
PARAM_DEFINE_INT32(UAVCAN_SV_FUNC7, 0);

/**
 * UAVCAN Servo 8 Output Function
 *
 * Select what should be output on UAVCAN Servo 8.
 * 
 * The default failsafe value is set according to the selected function:
 * - 'Min' for ConstantMin
 * - 'Max' for ConstantMax
 * - 'Max' for Parachute
 * - ('Max'+'Min')/2 for Servos
 * - 'Disarmed' for the rest
 * 
 *
 * @group Actuator Outputs
 * @value 0 Disabled
 * @value 1 Constant Min
 * @value 2 Constant Max
 * @value 101 Motor 1
 * @value 102 Motor 2
 * @value 103 Motor 3
 * @value 104 Motor 4
 * @value 105 Motor 5
 * @value 106 Motor 6
 * @value 107 Motor 7
 * @value 108 Motor 8
 * @value 109 Motor 9
 * @value 110 Motor 10
 * @value 111 Motor 11
 * @value 112 Motor 12
 * @value 201 Servo 1
 * @value 202 Servo 2
 * @value 203 Servo 3
 * @value 204 Servo 4
 * @value 205 Servo 5
 * @value 206 Servo 6
 * @value 207 Servo 7
 * @value 208 Servo 8
 * @value 301 Offboard Actuator Set 1
 * @value 302 Offboard Actuator Set 2
 * @value 303 Offboard Actuator Set 3
 * @value 304 Offboard Actuator Set 4
 * @value 305 Offboard Actuator Set 5
 * @value 306 Offboard Actuator Set 6
 * @value 400 Landing Gear
 * @value 401 Parachute
 * @value 402 RC Roll
 * @value 403 RC Pitch
 * @value 404 RC Throttle
 * @value 405 RC Yaw
 * @value 406 RC Flaps
 * @value 407 RC AUX 1
 * @value 408 RC AUX 2
 * @value 409 RC AUX 3
 * @value 410 RC AUX 4
 * @value 411 RC AUX 5
 * @value 412 RC AUX 6
 * @value 420 Gimbal Roll
 * @value 421 Gimbal Pitch
 * @value 422 Gimbal Yaw
 * @value 430 Gripper
 * @value 440 Landing Gear Wheel
 */
PARAM_DEFINE_INT32(UAVCAN_SV_FUNC8, 0);

/**
 * UAVCAN Servo 1 Disarmed Value
 *
 * This is the output value that is set when not armed.
 * 
 * Note that non-motor outputs might already be active in prearm state if COM_PREARM_MODE is set.
 * 
 *
 * @group Actuator Outputs
 * @min 0
 * @max 1000
 */
PARAM_DEFINE_INT32(UAVCAN_SV_DIS1, 500);

/**
 * UAVCAN Servo 2 Disarmed Value
 *
 * This is the output value that is set when not armed.
 * 
 * Note that non-motor outputs might already be active in prearm state if COM_PREARM_MODE is set.
 * 
 *
 * @group Actuator Outputs
 * @min 0
 * @max 1000
 */
PARAM_DEFINE_INT32(UAVCAN_SV_DIS2, 500);

/**
 * UAVCAN Servo 3 Disarmed Value
 *
 * This is the output value that is set when not armed.
 * 
 * Note that non-motor outputs might already be active in prearm state if COM_PREARM_MODE is set.
 * 
 *
 * @group Actuator Outputs
 * @min 0
 * @max 1000
 */
PARAM_DEFINE_INT32(UAVCAN_SV_DIS3, 500);

/**
 * UAVCAN Servo 4 Disarmed Value
 *
 * This is the output value that is set when not armed.
 * 
 * Note that non-motor outputs might already be active in prearm state if COM_PREARM_MODE is set.
 * 
 *
 * @group Actuator Outputs
 * @min 0
 * @max 1000
 */
PARAM_DEFINE_INT32(UAVCAN_SV_DIS4, 500);

/**
 * UAVCAN Servo 5 Disarmed Value
 *
 * This is the output value that is set when not armed.
 * 
 * Note that non-motor outputs might already be active in prearm state if COM_PREARM_MODE is set.
 * 
 *
 * @group Actuator Outputs
 * @min 0
 * @max 1000
 */
PARAM_DEFINE_INT32(UAVCAN_SV_DIS5, 500);

/**
 * UAVCAN Servo 6 Disarmed Value
 *
 * This is the output value that is set when not armed.
 * 
 * Note that non-motor outputs might already be active in prearm state if COM_PREARM_MODE is set.
 * 
 *
 * @group Actuator Outputs
 * @min 0
 * @max 1000
 */
PARAM_DEFINE_INT32(UAVCAN_SV_DIS6, 500);

/**
 * UAVCAN Servo 7 Disarmed Value
 *
 * This is the output value that is set when not armed.
 * 
 * Note that non-motor outputs might already be active in prearm state if COM_PREARM_MODE is set.
 * 
 *
 * @group Actuator Outputs
 * @min 0
 * @max 1000
 */
PARAM_DEFINE_INT32(UAVCAN_SV_DIS7, 500);

/**
 * UAVCAN Servo 8 Disarmed Value
 *
 * This is the output value that is set when not armed.
 * 
 * Note that non-motor outputs might already be active in prearm state if COM_PREARM_MODE is set.
 * 
 *
 * @group Actuator Outputs
 * @min 0
 * @max 1000
 */
PARAM_DEFINE_INT32(UAVCAN_SV_DIS8, 500);

/**
 * UAVCAN Servo 1 Minimum Value
 *
 * Minimum output value (when not disarmed).
 * 
 *
 * @group Actuator Outputs
 * @min 0
 * @max 1000
 */
PARAM_DEFINE_INT32(UAVCAN_SV_MIN1, 0);

/**
 * UAVCAN Servo 2 Minimum Value
 *
 * Minimum output value (when not disarmed).
 * 
 *
 * @group Actuator Outputs
 * @min 0
 * @max 1000
 */
PARAM_DEFINE_INT32(UAVCAN_SV_MIN2, 0);

/**
 * UAVCAN Servo 3 Minimum Value
 *
 * Minimum output value (when not disarmed).
 * 
 *
 * @group Actuator Outputs
 * @min 0
 * @max 1000
 */
PARAM_DEFINE_INT32(UAVCAN_SV_MIN3, 0);

/**
 * UAVCAN Servo 4 Minimum Value
 *
 * Minimum output value (when not disarmed).
 * 
 *
 * @group Actuator Outputs
 * @min 0
 * @max 1000
 */
PARAM_DEFINE_INT32(UAVCAN_SV_MIN4, 0);

/**
 * UAVCAN Servo 5 Minimum Value
 *
 * Minimum output value (when not disarmed).
 * 
 *
 * @group Actuator Outputs
 * @min 0
 * @max 1000
 */
PARAM_DEFINE_INT32(UAVCAN_SV_MIN5, 0);

/**
 * UAVCAN Servo 6 Minimum Value
 *
 * Minimum output value (when not disarmed).
 * 
 *
 * @group Actuator Outputs
 * @min 0
 * @max 1000
 */
PARAM_DEFINE_INT32(UAVCAN_SV_MIN6, 0);

/**
 * UAVCAN Servo 7 Minimum Value
 *
 * Minimum output value (when not disarmed).
 * 
 *
 * @group Actuator Outputs
 * @min 0
 * @max 1000
 */
PARAM_DEFINE_INT32(UAVCAN_SV_MIN7, 0);

/**
 * UAVCAN Servo 8 Minimum Value
 *
 * Minimum output value (when not disarmed).
 * 
 *
 * @group Actuator Outputs
 * @min 0
 * @max 1000
 */
PARAM_DEFINE_INT32(UAVCAN_SV_MIN8, 0);

/**
 * UAVCAN Servo 1 Maximum Value
 *
 * Maxmimum output value (when not disarmed).
 * 
 *
 * @group Actuator Outputs
 * @min 0
 * @max 1000
 */
PARAM_DEFINE_INT32(UAVCAN_SV_MAX1, 1000);

/**
 * UAVCAN Servo 2 Maximum Value
 *
 * Maxmimum output value (when not disarmed).
 * 
 *
 * @group Actuator Outputs
 * @min 0
 * @max 1000
 */
PARAM_DEFINE_INT32(UAVCAN_SV_MAX2, 1000);

/**
 * UAVCAN Servo 3 Maximum Value
 *
 * Maxmimum output value (when not disarmed).
 * 
 *
 * @group Actuator Outputs
 * @min 0
 * @max 1000
 */
PARAM_DEFINE_INT32(UAVCAN_SV_MAX3, 1000);

/**
 * UAVCAN Servo 4 Maximum Value
 *
 * Maxmimum output value (when not disarmed).
 * 
 *
 * @group Actuator Outputs
 * @min 0
 * @max 1000
 */
PARAM_DEFINE_INT32(UAVCAN_SV_MAX4, 1000);

/**
 * UAVCAN Servo 5 Maximum Value
 *
 * Maxmimum output value (when not disarmed).
 * 
 *
 * @group Actuator Outputs
 * @min 0
 * @max 1000
 */
PARAM_DEFINE_INT32(UAVCAN_SV_MAX5, 1000);

/**
 * UAVCAN Servo 6 Maximum Value
 *
 * Maxmimum output value (when not disarmed).
 * 
 *
 * @group Actuator Outputs
 * @min 0
 * @max 1000
 */
PARAM_DEFINE_INT32(UAVCAN_SV_MAX6, 1000);

/**
 * UAVCAN Servo 7 Maximum Value
 *
 * Maxmimum output value (when not disarmed).
 * 
 *
 * @group Actuator Outputs
 * @min 0
 * @max 1000
 */
PARAM_DEFINE_INT32(UAVCAN_SV_MAX7, 1000);

/**
 * UAVCAN Servo 8 Maximum Value
 *
 * Maxmimum output value (when not disarmed).
 * 
 *
 * @group Actuator Outputs
 * @min 0
 * @max 1000
 */
PARAM_DEFINE_INT32(UAVCAN_SV_MAX8, 1000);

/**
 * UAVCAN Servo 1 Failsafe Value
 *
 * This is the output value that is set when in failsafe mode.
 * 
 * When set to -1 (default), the value depends on the function (see UAVCAN_SV_FUNC1).
 * 
 *
 * @group Actuator Outputs
 * @min -1
 * @max 1000
 */
PARAM_DEFINE_INT32(UAVCAN_SV_FAIL1, -1);

/**
 * UAVCAN Servo 2 Failsafe Value
 *
 * This is the output value that is set when in failsafe mode.
 * 
 * When set to -1 (default), the value depends on the function (see UAVCAN_SV_FUNC2).
 * 
 *
 * @group Actuator Outputs
 * @min -1
 * @max 1000
 */
PARAM_DEFINE_INT32(UAVCAN_SV_FAIL2, -1);

/**
 * UAVCAN Servo 3 Failsafe Value
 *
 * This is the output value that is set when in failsafe mode.
 * 
 * When set to -1 (default), the value depends on the function (see UAVCAN_SV_FUNC3).
 * 
 *
 * @group Actuator Outputs
 * @min -1
 * @max 1000
 */
PARAM_DEFINE_INT32(UAVCAN_SV_FAIL3, -1);

/**
 * UAVCAN Servo 4 Failsafe Value
 *
 * This is the output value that is set when in failsafe mode.
 * 
 * When set to -1 (default), the value depends on the function (see UAVCAN_SV_FUNC4).
 * 
 *
 * @group Actuator Outputs
 * @min -1
 * @max 1000
 */
PARAM_DEFINE_INT32(UAVCAN_SV_FAIL4, -1);

/**
 * UAVCAN Servo 5 Failsafe Value
 *
 * This is the output value that is set when in failsafe mode.
 * 
 * When set to -1 (default), the value depends on the function (see UAVCAN_SV_FUNC5).
 * 
 *
 * @group Actuator Outputs
 * @min -1
 * @max 1000
 */
PARAM_DEFINE_INT32(UAVCAN_SV_FAIL5, -1);

/**
 * UAVCAN Servo 6 Failsafe Value
 *
 * This is the output value that is set when in failsafe mode.
 * 
 * When set to -1 (default), the value depends on the function (see UAVCAN_SV_FUNC6).
 * 
 *
 * @group Actuator Outputs
 * @min -1
 * @max 1000
 */
PARAM_DEFINE_INT32(UAVCAN_SV_FAIL6, -1);

/**
 * UAVCAN Servo 7 Failsafe Value
 *
 * This is the output value that is set when in failsafe mode.
 * 
 * When set to -1 (default), the value depends on the function (see UAVCAN_SV_FUNC7).
 * 
 *
 * @group Actuator Outputs
 * @min -1
 * @max 1000
 */
PARAM_DEFINE_INT32(UAVCAN_SV_FAIL7, -1);

/**
 * UAVCAN Servo 8 Failsafe Value
 *
 * This is the output value that is set when in failsafe mode.
 * 
 * When set to -1 (default), the value depends on the function (see UAVCAN_SV_FUNC8).
 * 
 *
 * @group Actuator Outputs
 * @min -1
 * @max 1000
 */
PARAM_DEFINE_INT32(UAVCAN_SV_FAIL8, -1);

/**
 * Reverse Output Range for UAVCAN
 *
 * Allows to reverse the output range for each channel.
 * Note: this is only useful for servos.
 * 
 *
 * @group Actuator Outputs
 * @bit 0 UAVCAN ESC 1
 * @bit 1 UAVCAN ESC 2
 * @bit 2 UAVCAN ESC 3
 * @bit 3 UAVCAN ESC 4
 * @bit 4 UAVCAN ESC 5
 * @bit 5 UAVCAN ESC 6
 * @bit 6 UAVCAN ESC 7
 * @bit 7 UAVCAN ESC 8
 * @min 0
 * @max 255
 */
PARAM_DEFINE_INT32(UAVCAN_EC_REV, 0);

/**
 * Reverse Output Range for UAVCAN
 *
 * Allows to reverse the output range for each channel.
 * Note: this is only useful for servos.
 * 
 *
 * @group Actuator Outputs
 * @bit 0 UAVCAN Servo 1
 * @bit 1 UAVCAN Servo 2
 * @bit 2 UAVCAN Servo 3
 * @bit 3 UAVCAN Servo 4
 * @bit 4 UAVCAN Servo 5
 * @bit 5 UAVCAN Servo 6
 * @bit 6 UAVCAN Servo 7
 * @bit 7 UAVCAN Servo 8
 * @min 0
 * @max 255
 */
PARAM_DEFINE_INT32(UAVCAN_SV_REV, 0);

/**
 * uXRCE-DDS domain ID
 *
 * uXRCE-DDS domain ID
 *
 * @group UXRCE-DDS Client
 * @category System
 * @reboot_required True
 */
PARAM_DEFINE_INT32(UXRCE_DDS_DOM_ID, 0);

/**
 * uXRCE-DDS Session key
 *
 * uXRCE-DDS key, must be different from zero.
 * In a single agent - multi client configuration, each client
 * must have a unique session key.
 * 
 *
 * @group UXRCE-DDS Client
 * @category System
 * @reboot_required True
 */
PARAM_DEFINE_INT32(UXRCE_DDS_KEY, 1);

/**
 * uXRCE-DDS UDP Port
 *
 * If ethernet enabled and selected as configuration for uXRCE-DDS,
 * selected udp port will be set and used.
 * 
 *
 * @group UXRCE-DDS Client
 * @min 0
 * @max 65535
 * @reboot_required True
 */
PARAM_DEFINE_INT32(UXRCE_DDS_PRT, 8888);

/**
 * uXRCE-DDS Agent IP address
 *
 * If ethernet enabled and selected as configuration for uXRCE-DDS,
 * selected Agent IP address will be set and used.
 * Decimal dot notation is not supported. IP address must be provided
 * in int32 format. For example, 192.168.1.2 is mapped to -1062731518;
 * 127.0.0.1 is mapped to 2130706433.
 * 
 *
 * @group UXRCE-DDS Client
 * @reboot_required True
 */
PARAM_DEFINE_INT32(UXRCE_DDS_AG_IP, 2130706433);

/**
 * Empty cell voltage (5C load)
 *
 * Defines the voltage where a single cell of battery 1 is considered empty.
 * The voltage should be chosen before the steep dropoff to 2.8V. A typical
 * lithium battery can only be discharged down to 10% before it drops off
 * to a voltage level damaging the cells.
 * 
 *
 * @group Battery Calibration
 * @decimal 2
 * @increment 0.01
 * @unit V
 * @reboot_required True
 */
PARAM_DEFINE_FLOAT(BAT1_V_EMPTY, 3.6);

/**
 * Empty cell voltage (5C load)
 *
 * Defines the voltage where a single cell of battery 1 is considered empty.
 * The voltage should be chosen before the steep dropoff to 2.8V. A typical
 * lithium battery can only be discharged down to 10% before it drops off
 * to a voltage level damaging the cells.
 * 
 *
 * @group Battery Calibration
 * @decimal 2
 * @increment 0.01
 * @unit V
 * @reboot_required True
 */
PARAM_DEFINE_FLOAT(BAT2_V_EMPTY, 3.6);

/**
 * Full cell voltage (5C load)
 *
 * Defines the voltage where a single cell of battery 1 is considered full
 * under a mild load. This will never be the nominal voltage of 4.2V
 * 
 *
 * @group Battery Calibration
 * @decimal 2
 * @increment 0.01
 * @unit V
 * @reboot_required True
 */
PARAM_DEFINE_FLOAT(BAT1_V_CHARGED, 4.05);

/**
 * Full cell voltage (5C load)
 *
 * Defines the voltage where a single cell of battery 1 is considered full
 * under a mild load. This will never be the nominal voltage of 4.2V
 * 
 *
 * @group Battery Calibration
 * @decimal 2
 * @increment 0.01
 * @unit V
 * @reboot_required True
 */
PARAM_DEFINE_FLOAT(BAT2_V_CHARGED, 4.05);

/**
 * Voltage drop per cell on full throttle
 *
 * This implicitly defines the internal resistance
 * to maximum current ratio for battery 1 and assumes linearity.
 * A good value to use is the difference between the
 * 5C and 20-25C load. Not used if BAT1_R_INTERNAL is
 * set.
 * 
 *
 * @group Battery Calibration
 * @decimal 2
 * @increment 0.01
 * @min 0.07
 * @max 0.5
 * @unit V
 * @reboot_required True
 */
PARAM_DEFINE_FLOAT(BAT1_V_LOAD_DROP, 0.1);

/**
 * Voltage drop per cell on full throttle
 *
 * This implicitly defines the internal resistance
 * to maximum current ratio for battery 1 and assumes linearity.
 * A good value to use is the difference between the
 * 5C and 20-25C load. Not used if BAT2_R_INTERNAL is
 * set.
 * 
 *
 * @group Battery Calibration
 * @decimal 2
 * @increment 0.01
 * @min 0.07
 * @max 0.5
 * @unit V
 * @reboot_required True
 */
PARAM_DEFINE_FLOAT(BAT2_V_LOAD_DROP, 0.1);

/**
 * Explicitly defines the per cell internal resistance for battery 1
 *
 * If non-negative, then this will be used in place of
 * BAT1_V_LOAD_DROP for all calculations.
 * 
 *
 * @group Battery Calibration
 * @decimal 4
 * @increment 0.0005
 * @min -1.0
 * @max 0.2
 * @unit Ohm
 * @reboot_required True
 */
PARAM_DEFINE_FLOAT(BAT1_R_INTERNAL, 0.005);

/**
 * Explicitly defines the per cell internal resistance for battery 2
 *
 * If non-negative, then this will be used in place of
 * BAT2_V_LOAD_DROP for all calculations.
 * 
 *
 * @group Battery Calibration
 * @decimal 4
 * @increment 0.0005
 * @min -1.0
 * @max 0.2
 * @unit Ohm
 * @reboot_required True
 */
PARAM_DEFINE_FLOAT(BAT2_R_INTERNAL, 0.005);

/**
 * Number of cells for battery 1.
 *
 * Defines the number of cells the attached battery consists of.
 * 
 *
 * @group Battery Calibration
 * @value 1 1S Battery
 * @value 2 2S Battery
 * @value 3 3S Battery
 * @value 4 4S Battery
 * @value 5 5S Battery
 * @value 6 6S Battery
 * @value 7 7S Battery
 * @value 8 8S Battery
 * @value 9 9S Battery
 * @value 10 10S Battery
 * @value 11 11S Battery
 * @value 12 12S Battery
 * @value 13 13S Battery
 * @value 14 14S Battery
 * @value 15 15S Battery
 * @value 16 16S Battery
 * @reboot_required True
 */
PARAM_DEFINE_INT32(BAT1_N_CELLS, 0);

/**
 * Number of cells for battery 2.
 *
 * Defines the number of cells the attached battery consists of.
 * 
 *
 * @group Battery Calibration
 * @value 1 1S Battery
 * @value 2 2S Battery
 * @value 3 3S Battery
 * @value 4 4S Battery
 * @value 5 5S Battery
 * @value 6 6S Battery
 * @value 7 7S Battery
 * @value 8 8S Battery
 * @value 9 9S Battery
 * @value 10 10S Battery
 * @value 11 11S Battery
 * @value 12 12S Battery
 * @value 13 13S Battery
 * @value 14 14S Battery
 * @value 15 15S Battery
 * @value 16 16S Battery
 * @reboot_required True
 */
PARAM_DEFINE_INT32(BAT2_N_CELLS, 0);

/**
 * Battery 1 capacity.
 *
 * Defines the capacity of battery 1 in mAh.
 * 
 *
 * @group Battery Calibration
 * @decimal 0
 * @increment 50
 * @min -1.0
 * @max 100000
 * @unit mAh
 * @reboot_required True
 */
PARAM_DEFINE_FLOAT(BAT1_CAPACITY, -1.0);

/**
 * Battery 2 capacity.
 *
 * Defines the capacity of battery 2 in mAh.
 * 
 *
 * @group Battery Calibration
 * @decimal 0
 * @increment 50
 * @min -1.0
 * @max 100000
 * @unit mAh
 * @reboot_required True
 */
PARAM_DEFINE_FLOAT(BAT2_CAPACITY, -1.0);

/**
 * Battery 1 monitoring source.
 *
 * This parameter controls the source of battery data. The value 'Power Module'
 * means that measurements are expected to come from a power module. If the value is set to
 * 'External' then the system expects to receive mavlink battery status messages.
 * If the value is set to 'ESCs', the battery information are taken from the esc_status message.
 * This requires the ESC to provide both voltage as well as current.
 * 
 *
 * @group Battery Calibration
 * @value -1 Disabled
 * @value 0 Power Module
 * @value 1 External
 * @value 2 ESCs
 * @reboot_required True
 */
PARAM_DEFINE_INT32(BAT1_SOURCE, 0);

/**
 * Battery 2 monitoring source.
 *
 * This parameter controls the source of battery data. The value 'Power Module'
 * means that measurements are expected to come from a power module. If the value is set to
 * 'External' then the system expects to receive mavlink battery status messages.
 * If the value is set to 'ESCs', the battery information are taken from the esc_status message.
 * This requires the ESC to provide both voltage as well as current.
 * 
 *
 * @group Battery Calibration
 * @value -1 Disabled
 * @value 0 Power Module
 * @value 1 External
 * @value 2 ESCs
 * @reboot_required True
 */
PARAM_DEFINE_INT32(BAT2_SOURCE, -1);

/**
 * MAVLink Config for instance 0
 *
 * The MAVLink Config defines device(uart/udp/tcp) used by MAVLink instance
 * 
 *
 * @group MAVLink
 * @value 0 Disabled
 * @value 1 Uart1
 * @value 2 Uart2
 * @value 3 Uart3
 * @value 4 Uart4
 * @value 5 Uart5
 * @value 6 Uart6
 * @value 7 Uart7
 * @value 8 Uart8
 * @value 20 UBS1
 * @value 21 USB2
 * @value 30 UDP
 * @value 31 TCP
 * @reboot_required True
 */
PARAM_DEFINE_INT32(MAV_0_CONFIG, 0);

/**
 * MAVLink Config for instance 1
 *
 * The MAVLink Config defines device(uart/udp/tcp) used by MAVLink instance
 * 
 *
 * @group MAVLink
 * @value 0 Disabled
 * @value 1 Uart1
 * @value 2 Uart2
 * @value 3 Uart3
 * @value 4 Uart4
 * @value 5 Uart5
 * @value 6 Uart6
 * @value 7 Uart7
 * @value 8 Uart8
 * @value 20 UBS1
 * @value 21 USB2
 * @value 30 UDP
 * @value 31 TCP
 * @reboot_required True
 */
PARAM_DEFINE_INT32(MAV_1_CONFIG, 0);

/**
 * MAVLink Config for instance 2
 *
 * The MAVLink Config defines device(uart/udp/tcp) used by MAVLink instance
 * 
 *
 * @group MAVLink
 * @value 0 Disabled
 * @value 1 Uart1
 * @value 2 Uart2
 * @value 3 Uart3
 * @value 4 Uart4
 * @value 5 Uart5
 * @value 6 Uart6
 * @value 7 Uart7
 * @value 8 Uart8
 * @value 20 UBS1
 * @value 21 USB2
 * @value 30 UDP
 * @value 31 TCP
 * @reboot_required True
 */
PARAM_DEFINE_INT32(MAV_2_CONFIG, 0);

/**
 * MAVLink Mode for instance 0
 *
 * The MAVLink Mode defines the set of streamed messages (for example the
 * vehicle's attitude) and their sending rates.
 * 
 *
 * @group MAVLink
 * @value 0 Normal
 * @value 1 Custom
 * @value 2 Onboard
 * @value 3 OSD
 * @value 4 Magic
 * @value 5 Config
 * @value 7 Minimal
 * @value 8 External Vision
 * @value 10 Gimbal
 * @value 11 Onboard Low Bandwidth
 * @value 12 uAvionix
 * @reboot_required True
 */
PARAM_DEFINE_INT32(MAV_0_MODE, 0);

/**
 * MAVLink Mode for instance 1
 *
 * The MAVLink Mode defines the set of streamed messages (for example the
 * vehicle's attitude) and their sending rates.
 * 
 *
 * @group MAVLink
 * @value 0 Normal
 * @value 1 Custom
 * @value 2 Onboard
 * @value 3 OSD
 * @value 4 Magic
 * @value 5 Config
 * @value 7 Minimal
 * @value 8 External Vision
 * @value 10 Gimbal
 * @value 11 Onboard Low Bandwidth
 * @value 12 uAvionix
 * @reboot_required True
 */
PARAM_DEFINE_INT32(MAV_1_MODE, 2);

/**
 * MAVLink Mode for instance 2
 *
 * The MAVLink Mode defines the set of streamed messages (for example the
 * vehicle's attitude) and their sending rates.
 * 
 *
 * @group MAVLink
 * @value 0 Normal
 * @value 1 Custom
 * @value 2 Onboard
 * @value 3 OSD
 * @value 4 Magic
 * @value 5 Config
 * @value 7 Minimal
 * @value 8 External Vision
 * @value 10 Gimbal
 * @value 11 Onboard Low Bandwidth
 * @value 12 uAvionix
 * @reboot_required True
 */
PARAM_DEFINE_INT32(MAV_2_MODE, 0);

/**
 * Maximum MAVLink sending rate for instance 0
 *
 * Configure the maximum sending rate for the MAVLink streams in Bytes/sec.
 * If the configured streams exceed the maximum rate, the sending rate of
 * each stream is automatically decreased.
 * 
 * If this is set to 0 a value of half of the theoretical maximum bandwidth is used.
 * This corresponds to baudrate/20 Bytes/s (baudrate/10 = maximum data rate on
 * 8N1-configured links).
 * 
 *
 * @group MAVLink
 * @min 0
 * @unit B/s
 * @reboot_required True
 */
PARAM_DEFINE_INT32(MAV_0_RATE, 1200);

/**
 * Maximum MAVLink sending rate for instance 1
 *
 * Configure the maximum sending rate for the MAVLink streams in Bytes/sec.
 * If the configured streams exceed the maximum rate, the sending rate of
 * each stream is automatically decreased.
 * 
 * If this is set to 0 a value of half of the theoretical maximum bandwidth is used.
 * This corresponds to baudrate/20 Bytes/s (baudrate/10 = maximum data rate on
 * 8N1-configured links).
 * 
 *
 * @group MAVLink
 * @min 0
 * @unit B/s
 * @reboot_required True
 */
PARAM_DEFINE_INT32(MAV_1_RATE, 0);

/**
 * Maximum MAVLink sending rate for instance 2
 *
 * Configure the maximum sending rate for the MAVLink streams in Bytes/sec.
 * If the configured streams exceed the maximum rate, the sending rate of
 * each stream is automatically decreased.
 * 
 * If this is set to 0 a value of half of the theoretical maximum bandwidth is used.
 * This corresponds to baudrate/20 Bytes/s (baudrate/10 = maximum data rate on
 * 8N1-configured links).
 * 
 *
 * @group MAVLink
 * @min 0
 * @unit B/s
 * @reboot_required True
 */
PARAM_DEFINE_INT32(MAV_2_RATE, 0);

/**
 * Enable MAVLink Message forwarding for instance 0
 *
 * If enabled, forward incoming MAVLink messages to other MAVLink ports if the
 * message is either broadcast or the target is not the autopilot.
 * 
 * This allows for example a GCS to talk to a camera that is connected to the
 * autopilot via MAVLink (on a different link than the GCS).
 * 
 *
 * @group MAVLink
 * @boolean
 * @reboot_required True
 */
PARAM_DEFINE_INT32(MAV_0_FORWARD, 1);

/**
 * Enable MAVLink Message forwarding for instance 1
 *
 * If enabled, forward incoming MAVLink messages to other MAVLink ports if the
 * message is either broadcast or the target is not the autopilot.
 * 
 * This allows for example a GCS to talk to a camera that is connected to the
 * autopilot via MAVLink (on a different link than the GCS).
 * 
 *
 * @group MAVLink
 * @boolean
 * @reboot_required True
 */
PARAM_DEFINE_INT32(MAV_1_FORWARD, 0);

/**
 * Enable MAVLink Message forwarding for instance 2
 *
 * If enabled, forward incoming MAVLink messages to other MAVLink ports if the
 * message is either broadcast or the target is not the autopilot.
 * 
 * This allows for example a GCS to talk to a camera that is connected to the
 * autopilot via MAVLink (on a different link than the GCS).
 * 
 *
 * @group MAVLink
 * @boolean
 * @reboot_required True
 */
PARAM_DEFINE_INT32(MAV_2_FORWARD, 0);

/**
 * Enable software throttling of mavlink on instance 0
 *
 * If enabled, MAVLink messages will be throttled according to
 * `txbuf` field reported by radio_status.
 * 
 * Requires a radio to send the mavlink message RADIO_STATUS.
 * 
 *
 * @group MAVLink
 * @boolean
 * @reboot_required True
 */
PARAM_DEFINE_INT32(MAV_0_RADIO_CTL, 1);

/**
 * Enable software throttling of mavlink on instance 1
 *
 * If enabled, MAVLink messages will be throttled according to
 * `txbuf` field reported by radio_status.
 * 
 * Requires a radio to send the mavlink message RADIO_STATUS.
 * 
 *
 * @group MAVLink
 * @boolean
 * @reboot_required True
 */
PARAM_DEFINE_INT32(MAV_1_RADIO_CTL, 1);

/**
 * Enable software throttling of mavlink on instance 2
 *
 * If enabled, MAVLink messages will be throttled according to
 * `txbuf` field reported by radio_status.
 * 
 * Requires a radio to send the mavlink message RADIO_STATUS.
 * 
 *
 * @group MAVLink
 * @boolean
 * @reboot_required True
 */
PARAM_DEFINE_INT32(MAV_2_RADIO_CTL, 1);

/**
 * MAVLink Network Port for instance 0
 *
 * If ethernet enabled and selected as configuration for MAVLink instance 0,
 * selected udp port will be set and used in MAVLink instance 0.
 * 
 *
 * @group MAVLink
 * @reboot_required True
 */
PARAM_DEFINE_INT32(MAV_0_UDP_PRT, 14556);

/**
 * MAVLink Network Port for instance 1
 *
 * If ethernet enabled and selected as configuration for MAVLink instance 1,
 * selected udp port will be set and used in MAVLink instance 1.
 * 
 *
 * @group MAVLink
 * @reboot_required True
 */
PARAM_DEFINE_INT32(MAV_1_UDP_PRT, 0);

/**
 * MAVLink Network Port for instance 2
 *
 * If ethernet enabled and selected as configuration for MAVLink instance 2,
 * selected udp port will be set and used in MAVLink instance 2.
 * 
 *
 * @group MAVLink
 * @reboot_required True
 */
PARAM_DEFINE_INT32(MAV_2_UDP_PRT, 0);

/**
 * MAVLink Remote IP for instance 0
 *
 * If ethernet enabled and selected as configuration for MAVLink instance 0,
 * selected remote IP will be set and used in MAVLink instance 0.
 * 
 *
 * @group MAVLink
 * @reboot_required True
 */
PARAM_DEFINE_INT32(MAV_0_REMOTE_IP, 0);

/**
 * MAVLink Remote IP for instance 1
 *
 * If ethernet enabled and selected as configuration for MAVLink instance 1,
 * selected remote IP will be set and used in MAVLink instance 1.
 * 
 *
 * @group MAVLink
 * @reboot_required True
 */
PARAM_DEFINE_INT32(MAV_1_REMOTE_IP, 0);

/**
 * MAVLink Remote IP for instance 2
 *
 * If ethernet enabled and selected as configuration for MAVLink instance 2,
 * selected remote IP will be set and used in MAVLink instance 2.
 * 
 *
 * @group MAVLink
 * @reboot_required True
 */
PARAM_DEFINE_INT32(MAV_2_REMOTE_IP, 0);

/**
 * MAVLink Remote Port for instance 0
 *
 * If ethernet enabled and selected as configuration for MAVLink instance 0,
 * selected remote port will be set and used in MAVLink instance 0.
 * 
 *
 * @group MAVLink
 * @reboot_required True
 */
PARAM_DEFINE_INT32(MAV_0_REMOTE_PRT, 14550);

/**
 * MAVLink Remote Port for instance 1
 *
 * If ethernet enabled and selected as configuration for MAVLink instance 1,
 * selected remote port will be set and used in MAVLink instance 1.
 * 
 *
 * @group MAVLink
 * @reboot_required True
 */
PARAM_DEFINE_INT32(MAV_1_REMOTE_PRT, 0);

/**
 * MAVLink Remote Port for instance 2
 *
 * If ethernet enabled and selected as configuration for MAVLink instance 2,
 * selected remote port will be set and used in MAVLink instance 2.
 * 
 *
 * @group MAVLink
 * @reboot_required True
 */
PARAM_DEFINE_INT32(MAV_2_REMOTE_PRT, 0);

/**
 * Broadcast heartbeats on local network for MAVLink instance 0
 *
 * This allows a ground control station to automatically find the drone
 * on the local network.
 * 
 *
 * @group MAVLink
 * @value 0 Never broadcast
 * @value 1 Always broadcast
 * @value 2 Only multicast
 */
PARAM_DEFINE_INT32(MAV_0_BROADCAST, 1);

/**
 * Broadcast heartbeats on local network for MAVLink instance 1
 *
 * This allows a ground control station to automatically find the drone
 * on the local network.
 * 
 *
 * @group MAVLink
 * @value 0 Never broadcast
 * @value 1 Always broadcast
 * @value 2 Only multicast
 */
PARAM_DEFINE_INT32(MAV_1_BROADCAST, 0);

/**
 * Broadcast heartbeats on local network for MAVLink instance 2
 *
 * This allows a ground control station to automatically find the drone
 * on the local network.
 * 
 *
 * @group MAVLink
 * @value 0 Never broadcast
 * @value 1 Always broadcast
 * @value 2 Only multicast
 */
PARAM_DEFINE_INT32(MAV_2_BROADCAST, 0);

/**
 * Enable serial flow control for instance 0
 *
 * This is used to force flow control on or off for the the mavlink
 * instance. By default it is auto detected. Use when auto detection fails.
 * 
 *
 * @group MAVLink
 * @value 0 Force off
 * @value 1 Force on
 * @value 2 Auto-detected
 * @reboot_required True
 */
PARAM_DEFINE_INT32(MAV_0_FLOW_CTRL, 2);

/**
 * Enable serial flow control for instance 1
 *
 * This is used to force flow control on or off for the the mavlink
 * instance. By default it is auto detected. Use when auto detection fails.
 * 
 *
 * @group MAVLink
 * @value 0 Force off
 * @value 1 Force on
 * @value 2 Auto-detected
 * @reboot_required True
 */
PARAM_DEFINE_INT32(MAV_1_FLOW_CTRL, 2);

/**
 * Enable serial flow control for instance 2
 *
 * This is used to force flow control on or off for the the mavlink
 * instance. By default it is auto detected. Use when auto detection fails.
 * 
 *
 * @group MAVLink
 * @value 0 Force off
 * @value 1 Force on
 * @value 2 Auto-detected
 * @reboot_required True
 */
PARAM_DEFINE_INT32(MAV_2_FLOW_CTRL, 2);
