## libevents

This project contains the events interface, which includes the following:
- a protocol to send events based on an ID (and possibly arguments) to inform other components (in a reliable way)
- using MAVLink as a transport layer.
  Each MAVLink component can send and/or receive events to/from other components.
  The primary use-case is for an autopilot to inform a user (through a GCS) about certain events (such as a failsafe action).
- metadata for each event, for example a description, stored as JSON.
  Metadata is typically distributed via the COMPONENT_INFORMATION MAVLink API.
- a set of commonly used predefined events that can be used to build simple protocols
- a parser (MAVLink-independent) to combine event ID's with the JSON metadata for display or analysis purposes.
  This also includes log processing.

### Design
Initial design doc: https://docs.google.com/document/d/1VLdnvjG6r6Cw-_FSo9lcG-Nlk7eGA7DaEHhe-3bP4-Q

### Event definitions
Event definitions are separated by namespaces.
Each namespace has a name and an 8 bit component ID (typically matching the MAVLink component ID. For multi-instance components, it should be the first one, e.g. GIMBAL1).
Within a namespace, each event has a unique name and sub ID (24 bits). The event ID is the combined component ID with sub ID.
So events can be uniquely identified for the whole system by ID or `<namespace>::<name>`.
This is generally not required, as metadata is queried independently per MAVLink component. It is only relevant when combining all events from a system without further component identification, e.g. for logging.

Each event can have a set of arguments, using either basic types or enums.

#### Message and description
Events have a single-line (short) message and possibly a more extensive description.

Supported format and parsing:
- characters can be escaped with \\

  These have to be escaped: '\\\\', '\\<', '\\{'.
- tags:
  - Profiles: `<profile name="[!]NAME">CONTENT</profile>`

    `CONTENT` will only be shown if the name matches the configured profile.
	This can be used for example to hide developer information from end-users.
  - URLs: `<a [href="URL"]>CONTENT</a>`.
    If href is not set, use `CONTENT` as `URL`
  - Parameters: `<param>PARAM_NAME</param>`
  - no nested tags of the same type
- arguments: following python syntax, with 1-based indexing (instead of 0)
  - general form: `{ARG_IDX[:.NUM_DECIMAL_DIGITS][UNIT]}`

    UNIT:
      - m: horizontal distance in meters
      - m_v: vertical distance in meters
      - m^2: area in m^2
      - m/s: speed in m/s
      - C: temperature in degrees celcius
      - These are used to be able to switch the display unit (e.g. metric vs
        imperial)

#### Log levels
Events have 2 log levels:
- Internal: for logging purposes
- External: log level used by a GCS

Typically the same log level is used for both cases, but they can differ.
For example a low battery event could be shown as warning to the user, but is either an info or disabled in the log.

An event with log level 'Protocol' should not be shown.

#### Event groups
Every event belongs to a group. That is essentially just a tag, and can be used to treat a set of events different from others.
A UI should not display unkown groups.

Special groups:
- *calibration*: used for the sensor/RC calibration protocol
- *health* and *arming_check*: used to report health and arming check results.
  Apart from the 2 summary messages (via `type='summary'`), each event needs to have at least 2 arguments:
  - `<navigation_group>`: affected flight modes (bitmask)
  - `uint8_t health_component_index`: affected health component index (to the `<health_component>` bitset), can be 0 if none applies.

  `<navigation_group>` and `<health_component>` are custom enums (bitsets) based on any unsigned integer.
  The mapping from the navigation groups to custom_modes is defined via `navigation_mode_groups` in the json.
  The first entry (1. bit) in `<health_component>` must always be 'none'.

  The order of sending is:
  - `arming_check_summary`
  - set of N health and arming check events
  - `health_summary`

  The set of checks can be split into multiple chunks that the receiver then combines.

  See libs/cpp/parse/health_and_arming_checks.h for an implementation.

  Events with type=`append_health_and_arming_messages` can be used to display a message with all the current arming and health warnings and errors appended to the message.
  These events need to specify the mode as the first argument.

#### common namespace
Since events in the common namespace are used by multiple components, they cannot be changed arbitrarily.
Rules:
- changing the description or message is ok, as long as the semantic is the same
- arguments cannot be changed, it is only possible to add new arguments
- for any other change (name, ID, ...), a new event needs to be defined, and the old one deprecated.
- enums cannot be changed, except for adding new values

### Serialization
- Event arguments are serialized in the same order as defined.
  No extra padding, so accesses might be unaligned.
- Everything assumes little endian



