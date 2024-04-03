# DATAMAN

## 简介

DATAMAN模块主要是用来管理飞行航线`mission`、安全返航点`safepoint`和地理围栏`geofence`等数据。dataman支持将数据存储在`FILE`或`RAM`中，默认是保存到文件`/mnt/microsd/dataman`。

## 存储结构

dataman存储了下表中7类数据，需要额外说明的是：

1. SAFE_POINTS和FENCE_POINTS，第0条消息是存储mission_stats_entry_s，表示实际存储的安全点和地理围栏的个数，后续才存储安全点和禁飞区数据；

1. WAYPOINTS_OFFBOARD_0和WAYPOINTS_OFFBOARD_1，地面站上传的航线，但是实际飞行只会使用其中一条，如果飞机当前正在使用WAYPOINTS_OFFBOARD_1，此时地面站上传的新航线将保存到WAYPOINTS_OFFBOARD_0，当上传完毕，飞控会自动切换到WAYPOINTS_OFFBOARD_0并执行，原来的WAYPOINTS_OFFBOARD_1将失效，那目前到底使用哪一条航线是根据MISSION_STATE确定的；

1. WAYPOINTS_ONBOARD，机载自动生成航线，也就是飞控生成了一条航线，然后让按照该航线飞行，**总条数是地面站上传的十分之一**；

1. MISSION_STATE，存储航线飞行状态，飞的是WAYPOINTS_OFFBOARD_0还是WAYPOINTS_OFFBOARD_1，以及第几个航点。

|类型(dm_item_t)|第一条(index=0)|第K条(index=k)|总条数N|说明|
|---|---|---|---|---|
|SAFE_POINTS=0|mission_stats_entry_s|mission_safe_point_s|8|地面站上传安全返航点|
|FENCE_POINTS=1|mission_stats_entry_s|mission_fence_point_s|16|地面站上传地理围栏|
|WAYPOINTS_OFFBOARD_0=2|mission_s|mission_item_s|50/**2000**/65535|地面站上传航线0|
|WAYPOINTS_OFFBOARD_1=3|mission_s|mission_item_s|50/**2000**/65535|地面站上传航线1|
|WAYPOINTS_ONBOARD=4|mission_s|mission_item_s|5/**200**/6553|机载自动生成航线|
|MISSION_STATE=5|mission_s|/|1|当前飞行航线的状态|
|COMPAT=6|dataman_compat_s|/|1||

### 存储方式

为了便于读写航点等数据，dataman是固定格式二进制文件，通过计算偏移地址进行随机访问：

1. 按照顺序依次存储SAFE_POINTS、FENCE_POINTS、WAYPOINTS_OFFBOARD_0等7类消息，每种消息`总条数`和`单条字节数`见下表；

1. 每种消息`总条数`和`单条字节数`是已知的，因此dataman文件大小是固定的，初始化时候就直接开辟固定大小的空间；

1. dm_item_t类型第index条消息的偏移地址是可以直接计算出来，因此支持快速随机特定的消息；

1. 虽然SAFE_POINTS和FENCE_POINTS的第0条消息是mission_stats_entry_s，但是还是按照mission_safe_point_s和mission_fence_point_s大小开辟的空间；

1. 为了判断某条消息的有效性，在存储消息时候，会先插入4个字节（`也就是下表中的+4`），意义为：

- 第1个字节，表示写入消息的字节数，读数据的时候会检查该数值是否 > 0
- 第2个字节，表示数据保持水平，比如是否上电就擦除
- 第3-4字节，暂时没用

```c
 /* Write out the data, prefixed with length and persistence level */
 buffer[0] = count; // 写入消息的字节数，比如sizeof(mission_item_s)
 buffer[1] = persistence;
 buffer[2] = 0;
 buffer[3] = 0;
```

|类型(dm_item_t)|初始偏移地址(offset)|单条字节(sizeof)|总条数(N)|说明|
|---|---|---|---|---|
|SAFE_POINTS=0|0|sizeof(mission_safe_point_s)+4|8|地面站上传安全返航点|
|FENCE_POINTS=1||sizeof(mission_fence_point_s)+4|16|地面站上传地理围栏|
|WAYPOINTS_OFFBOARD_0=2||sizeof(mission_item_s)+4|50/**2000**/65535|地面站上传航线0|
|WAYPOINTS_OFFBOARD_1=3||sizeof(mission_item_s)+4|50/**2000**/65535|地面站上传航线1|
|WAYPOINTS_ONBOARD=4||sizeof(mission_item_s)+4|5/**200**/6553|机载自动生成航线|
|MISSION_STATE=5||sizeof(mission_s)+4|1|当前飞行航线的状态|
|COMPAT=6||sizeof(dataman_compat_s)+4|1||

### 偏移地址

单条数据字节数，需要注意的是每条消息都加了`DM_SECTOR_HDR_SIZE=4`个字节，这4字节是用来表征该条消息是否有效。

```c
/*Table of the len of each item type*/
static constexpr size_t g_per_item_size[DM_KEY_NUM_KEYS] = {
        sizeof(struct mission_safe_point_s) + DM_SECTOR_HDR_SIZE,
        sizeof(struct mission_fence_point_s) + DM_SECTOR_HDR_SIZE,
        sizeof(struct mission_item_s) + DM_SECTOR_HDR_SIZE,
        sizeof(struct mission_item_s) + DM_SECTOR_HDR_SIZE,
        sizeof(struct mission_item_s) + DM_SECTOR_HDR_SIZE,
        sizeof(struct mission_s) + DM_SECTOR_HDR_SIZE,
        sizeof(struct dataman_compat_s) + DM_SECTOR_HDR_SIZE
};

```

dm_item_t的初始偏移地址，其实很简单，就是`单条消息字节sizeof × 最大条数N + 上一个类型的offset`

```
 /* Initialize global variables */
 g_key_offsets[0] = 0;

 for (int i = 0; i < ((int)DM_KEY_NUM_KEYS - 1); i++) {
  g_key_offsets[i + 1] = g_key_offsets[i] + (g_per_item_max_index[i] * g_per_item_size[i]);
 }
```

最终计算dm_item_t类型第index条消息的偏移地址，计算公式为 `查表获取dm_item_t类型的初始偏移地址 + 单条消息字节sizeof × index`

```
/* Calculate the offset in file of specific item */
static int calculate_offset(dm_item_t item, unsigned index)
{

 /* Make sure the item type is valid */
 if (item >= DM_KEY_NUM_KEYS) {
  return -1;
 }

 /* Make sure the index for this item type is valid */
 if (index >= g_per_item_max_index[item]) {
  return -1;
 }

 /* Calculate and return the item index based on type and index */
 return g_key_offsets[item] + (index * g_per_item_size[item]);
}
```

## 消息定义

### 安全返航点

安全返航点，是无人机应急情况用于着陆的位置。飞控支持最多7个安全返航点，其中：

- 第0个，mission_stats_entry_s，表示设置的安全返航点个数，以及更新的次数
- 第1-7个，mission_safe_point_s，是安全返航点的纬度、经度、高度和坐标系

```c
struct mission_stats_entry_s {
 uint16_t num_items;   /**< total number of items stored (excluding this one) */
 uint16_t update_counter;   /**< This counter is increased when (some) items change (this can wrap) */
};

struct mission_safe_point_s {
 double lat;
 double lon;
 float alt;
 uint8_t frame;     /**< MAV_FRAME */

 uint8_t _padding0[3];    /**< padding struct size to alignment boundary  */
};
```

### 地理围栏

地理围栏，是否无人机不允许进入的区域，如果进入地理围栏无人机将会出发指定的动作，比如返航、悬停、降落等。地里面围栏有`多边形`和`圆形`，飞控支持最多15个禁飞区：

- 第0个，mission_stats_entry_s，表示设置的禁飞区的个数和更新次数；
- 第1-15个，mission_fence_point_s，是地理围栏的类型（nav_cmd），多边形点个数（vertex_count），或圆形半径（circle_radius），以及纬度、经度、高度和坐标系；
- 当nav_cmd=MAV_CMD_NAV_FENCE_POLYGON，表示多边形围栏，接下来的vertex_count个数据是多边形的顶点；
- 当nav_cmd=MAV_CMD_NAV_FENCE_CIRCLE，表示圆形围栏，circle_radius是圆的半径。

```c
struct mission_stats_entry_s {
 uint16_t num_items;   /**< total number of items stored (excluding this one) */
 uint16_t update_counter;   /**< This counter is increased when (some) items change (this can wrap) */
};


struct mission_fence_point_s {
 double lat;
 double lon;
 float alt;

 union {
  uint16_t vertex_count;   /**< number of vertices in this polygon */
  float circle_radius;   /**< geofence circle radius in meters (only used for NAV_CMD_NAV_FENCE_CIRCLE*) */
 };

 uint16_t nav_cmd;    /**< navigation command (one of MAV_CMD_NAV_FENCE_*) */
 uint8_t frame;     /**< MAV_FRAME */

 uint8_t _padding0[5];    /**< padding struct size to alignment boundary  */
};

```

### 飞行航线

航线有地面站上传和飞控自动生成等两类，其中地面站上传默认最大支持2000个点，自动生成航点数是地面站上传的十分之一。

为了保障飞行航线和上传航线不相互干扰，飞控将地面站上传航线分成WAYPOINTS_OFFBOARD_0和WAYPOINTS_OFFBOARD_1来存储，如果飞机当前正在使用WAYPOINTS_OFFBOARD_1，则地面站上传的新航线将保存到WAYPOINTS_OFFBOARD_0，上传完毕之后，飞控会自动切换到WAYPOINTS_OFFBOARD_0并执行，原来的WAYPOINTS_OFFBOARD_1将失效，那目前到底使用哪一条航线是根据MISSION_STATE确定的。

```c
struct mission_item_s {
 double lat;     /**< latitude in degrees    */
 double lon;     /**< longitude in degrees    */
 union {
  struct {
   union {
    float time_inside;  /**< time that the MAV should stay inside the radius before advancing in seconds */
    float circle_radius;  /**< geofence circle radius in meters (only used for NAV_CMD_NAV_FENCE_CIRCLE*) */
   };
   float acceptance_radius;  /**< default radius in which the mission is accepted as reached in meters */
   float loiter_radius;   /**< loiter radius in meters, 0 for a VTOL to hover, negative for counter-clockwise */
   float yaw;    /**< in radians NED -PI..+PI, NAN means don't change yaw  */
   float ___lat_float;   /**< padding */
   float ___lon_float;   /**< padding */
   float altitude;    /**< altitude in meters (AMSL)   */
  };
  float params[7];    /**< array to store mission command values for MAV_FRAME_MISSION ***/
 };

 uint16_t nav_cmd;    /**< navigation command     */

 int16_t do_jump_mission_index;   /**< index where the do jump will go to                 */
 uint16_t do_jump_repeat_count;   /**< how many times do jump needs to be done            */

 union {
  uint16_t do_jump_current_count;   /**< count how many times the jump has been done */
  uint16_t vertex_count;    /**< Polygon vertex count (geofence) */
  uint16_t land_precision;   /**< Defines if landing should be precise: 0 = normal landing, 1 = opportunistic precision landing, 2 = required precision landing (with search) */
 };

 struct {
  uint16_t frame : 4,    /**< mission frame */
    origin : 3,    /**< how the mission item was generated */
    loiter_exit_xtrack : 1,  /**< exit xtrack location: 0 for center of loiter wp, 1 for exit location */
    force_heading : 1,   /**< heading needs to be reached */
    altitude_is_relative : 1,  /**< true if altitude is relative from start point */
    autocontinue : 1,   /**< true if next waypoint should follow after this one */
    vtol_back_transition : 1,  /**< part of the vtol back transition sequence */
    _padding0 : 4;    /**< padding remaining unused bits  */
 };

 uint8_t _padding1[2];    /**< padding struct size to alignment boundary  */
};
```

### 航线状态

```c
struct mission_s {
#endif
 uint64_t timestamp;
 int32_t current_seq;
 uint16_t count;
 uint8_t dataman_id;
 uint8_t _padding0[1]; // required for logger
};

```

### 文件有效性校验

```c
struct dataman_compat_s {
 uint64_t key;
};
```

## 接口函数

### dm_write

```c
ssize_t dm_write(dm_item_t item,
                unsigned index,
                dm_persitence_t persistence,
                const void *buf,
                size_t count)
```

### dm_read

```c
ssize_t dm_read(dm_item_t item,
                unsigned index,
                void *buf,
                size_t count)
```

### dm_clear

```c
int dm_clear(dm_item_t item)
```

### dm_lock

```c
int dm_lock(dm_item_t item)
```

### dm_unlock

```c
void dm_unlock(dm_item_t item)
```

### dm_trylock

```c
int dm_trylock(dm_item_t item)
```

### dm_restart

```c
int dm_restart(dm_reset_reason reason)
```
