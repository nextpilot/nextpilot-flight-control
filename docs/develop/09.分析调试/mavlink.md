# MAVLink调试

wireshark 是功能非常强大的网络数据抓包工具，可以通过配置mavlink插件，更好的抓取、过滤mavlink消息。并且可以显示mavlink消息各字段的详细内容。

## 程序安装

### Windows

Windows下安装非常简单，双击安装文件即可。安装完成后，双击打开桌面快捷方式，可以启动wireshark。

### linux

```shell
$ sudo apt-add-repository ppa:wireshark-dev/stable
$ sudo apt-get install wireshark

## 添加到用户组
$ sudo adduser $USER wireshark
```

ubuntu下通过输入命令启动wireshark

```shell
wireshark
```

## 插件生成与配置

### 生成插件

下载mavlink仓库，进入mavlink-master根目录，输入如下命令：

```shell
python3 -m pymavlink.tools.mavgen --lang=WLua --wire-protocol=2.0 --output=mavlink_2_common message_definitions/v1.0/common.xml
```

会在当前输入命令所在目录下生成**mavlink_2_common.lua**.该文件就是mavlink开发对应的lua语言文件。

在`nextpilot-flight-control/tools/mavlink`文件夹下可以找到该文件。

### 配置插件

为了监控mavlink消息，必须指定监控端口，在**mavlink_2_common.lua**文件默认添加了相关配置信息，如下是默认的UDP监控端口

```lua
-- bind protocol dissector to USER0 linktype

wtap_encap = DissectorTable.get("wtap_encap")
wtap_encap:add(wtap.USER0, mavlink_proto)

-- bind protocol dissector to port 14550 and 14580

local udp_dissector_table = DissectorTable.get("udp.port")
udp_dissector_table:add(14550, mavlink_proto)
udp_dissector_table:add(14580, mavlink_proto)
```

仿真环境下，GCS连接端口为：

gazebo：18570  <---->  GCS：14550

## 使用方法

### 导入插件

这一步就是将上一步生成的**mavlink_2_common.lua**文件作为插件导入wirshark。

- 查看插件路径

  打开软件`wireshark`，在Help->Folders，查看Global Lua Plugins的路径。

  Ubuntu系统下路径：

  ```shell
  /usr/lib/x86_64-linux-gnu/wireshark/plugins/
  ```

  Windows系统下路径：

  ```shell
  C:/Program Files/Wireshark/plugins
  ```

- 导入插件

  并将生成的插件**mavlink_2_common.lua**文件拷贝至该路径下即可。  ubuntu下可以通过cp命令拷贝：

  ```shell
  sudo cp mavlink_2_common.lua /usr/lib/x86_64-linux-gnu/wireshark/plugins/
  ```

- 重启wireshark

  导入插件后，重启wireshark软件。

### 列表过滤

可以通过过滤框中输入`mavlink_proto`进行mavlink消息的过滤。

![img](./image/live_output_filtered.jpg)

#### 常用过滤条件

- 只查看地面站发送的消息

地面站的comp_id=0xbe=190

```shell
mavlink_proto.compid == 190
```

- 通过msgid过滤消息

```shell
mavlink_proto.msgid==0
```

#### 其他

过滤条件可以进行与或结合，组成更复杂的条件

```shell
mavlink_proto.msgid==0 && mavlink_proto.compid == 1 &&
(ip.addr == 10.0.115.155 && ip.dst == 10.0.115.141)
```

### 查看mavlink消息具体内容

可以双击抓取的mavlink消息，查看具体信息。

![img](./image/mavlink_message_details.jpg)
