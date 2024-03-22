# RomFS只读文件系统

RomFS 是一种只读文件系统，是在嵌入式设备上常用的一种文件系统，具备体积小，可靠性高，读取速度快等优点，常用来作为系统初始文件系统。在rt-thread中，如果需要挂载多个文件系统，则一般需要使用ROMFS先创建挂点（比如/mnt/nandflash，/mnt/microsd等）。

## 生成 romfs.c 文件

由于 RomFS 是只读文件系统，所以需要放入到 RomFS 的文件都需要在系统运行前加入。我们可以将要存入 RomFS 中的文件数据放在 romfs.c 文件中，NextPilot提供了制作 romfs.c 的 Python 脚本文件 mkromfs.py，根据用户需要加入到 RomFS 的文件和目录生成对应的数据结构。

打开 mkromfs.py 脚本文件所在路径 rtos\rt-thread\tools 并在该目录下新建文件夹 romfs，放入我们需要在 RomFS 中放置的文件或文件夹。在该目录下打开 Env 工具，并运行命令：

```shell
python mkromfs.py romfs romfs.c
```

可以看到目录下成功生成 romfs.c 文件，将生成的 romfs.c 文件拷贝在与 mnt.c 文件相同的路径内。

## 挂载 ROMFS

```c++
# include <rtthread.h>

# ifdef RT_USING_DFS
# include <dfs_fs.h>
# include "dfs_romfs.h"

int mnt_init(void)
{
    if (dfs_mount(RT_NULL, "/", "rom", 0, &(romfs_root)) == 0)
    {
        LOG_I("ROM file system initializated!");
    }
    else
    {
        LOG_I("ROM file system initializate failed!");
    }

    return 0;
}
INIT_ENV_EXPORT(mnt_init);
# endif
```

## 目录结构

```
/                                根目录，RomFS挂载点
├─dev                            DevFS挂载点
├─etc                            板载配置文件，用户不可访问，只读
└─fs                             附加存储设备
    ├─eeprom                     eeprom挂载点（LittleFS），用户不可访问，一般存储控制参数
    │      param                 控制参数存储文件
    │
    ├─norflash                   norflash挂载点（LittleFS），用户不可访问，一般存储航线、出厂固件等
    │      dataman               航线、禁飞区等存储文件
    │      factory.bin           出厂固件
    │
    └─microsd                     SD卡挂载点（FatFS），用户可以访问，一般存储日志等数据
        ├─etc                    用户配置文件
        │      firmware.bin      待升级固件，升级完会自动删除
        │      param.txt         用户配置控制参数
        │      geofense.txt      用户配置禁飞区
        └─log                    飞行日志，按日期分文件夹存储
```
