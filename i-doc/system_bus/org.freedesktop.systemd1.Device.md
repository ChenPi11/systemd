# 接口名称：org.freedesktop.systemd1.Device

## 接口说明

`org.freedesktop.systemd1.Device` 是 systemd 中**设备单元（`.device`）的 D-Bus 接口**。设备单元由 systemd 根据内核通过 udev 报告的设备事件自动创建，用于将内核设备暴露为 systemd 依赖图中的节点，从而实现**基于设备的服务激活**。

设备单元的典型用途包括：

- **触发依赖启动**：当特定硬件设备出现时（如 USB 存储器插入），自动启动相关服务
- **建立启动顺序依赖**：服务可以声明 `After=dev-sda.device`，确保在磁盘设备就绪后再启动
- **反映 udev 设备生命周期**：设备插入时单元变为 `active`，设备拔出时变为 `inactive`

> **注意**：设备单元由 systemd 根据 udev 规则自动生成，通常**不需要**手动创建 `.device` 文件。可通过 udev 规则中的 `SYSTEMD_WANTS=` 属性向设备单元添加依赖。

## 总线名称

`org.freedesktop.systemd1`

## 继承关系

```
org.freedesktop.systemd1.Unit
    └── org.freedesktop.systemd1.Device
```

## 对象路径

`/org/freedesktop/systemd1/unit/<device_name>`

其中 `<device_name>` 是设备单元名称经过 D-Bus 路径转义后的形式。设备单元名称通常由内核设备路径转换而来（例如 `/dev/sda` 对应单元名 `dev-sda.device`，路径为 `/org/freedesktop/systemd1/unit/dev_2dsda_2edevice`）。

---

## 属性（Properties）

### 设备特有属性

| 属性名称 | 类型 | 读写 | 说明 |
|---------|------|------|------|
| `SysFSPath` | `s` | 只读 | 设备在 sysfs 虚拟文件系统中的路径，例如 `/sys/bus/pci/devices/0000:00:1f.2` 或 `/sys/devices/pci0000:00/0000:00:1f.2`。可通过此路径读取设备属性（驱动信息、电源状态等） |

### 继承自 Unit 接口的关键属性

| 属性名称 | 类型 | 说明 |
|---------|------|------|
| `Id` | `s` | 设备单元名称（如 `dev-sda.device`） |
| `ActiveState` | `s` | 活跃状态：设备存在时为 `active`，设备不存在时为 `inactive` |
| `SubState` | `s` | 子状态：`plugged`（设备已连接）/ `dead`（设备未连接） |
| `LoadState` | `s` | 加载状态：对于设备单元通常为 `loaded`（设备存在时）或 `not-found`（设备不存在时） |
| `Description` | `s` | 设备描述，通常来自 udev 的 `SYSTEMD_ALIAS` 或设备自身信息 |
| `Wants` | `as` | 此设备单元激活时需要一并启动的单元（通常由 udev 规则 `SYSTEMD_WANTS=` 注入） |
| `BindsTo` | `as` | 强绑定依赖单元列表；设备消失时，这些单元也会被停止 |

---

## 方法（Methods）

设备单元**不定义任何特有方法**。通用查询方法由基础接口 `org.freedesktop.systemd1.Unit` 提供。

> **注意**：设备单元通常不能通过 D-Bus 手动启动或停止，其生命周期由内核/udev 事件控制。

---

## 信号（Signals）

设备单元**不定义任何特有信号**。

---

## 设备单元命名规则

内核设备路径到单元名称的转换规则：

| 内核设备 | 对应单元名称 |
|---------|-------------|
| `/dev/sda` | `dev-sda.device` |
| `/dev/sda1` | `dev-sda1.device` |
| `/dev/disk/by-uuid/...` | `dev-disk-by\x2duuid-....device` |
| `/dev/ttyUSB0` | `dev-ttyUSB0.device` |
| `/dev/mapper/vg-lv` | `dev-mapper-vg\x2dlv.device` |

可通过以下命令查看设备单元名称：

```bash
systemctl list-units --type=device
```

---

## 使用示例

```bash
# 查询 /dev/sda 设备单元的 sysfs 路径
busctl get-property org.freedesktop.systemd1 \
    /org/freedesktop/systemd1/unit/dev_2dsda_2edevice \
    org.freedesktop.systemd1.Device SysFSPath

# 查询设备单元的活跃状态
busctl get-property org.freedesktop.systemd1 \
    /org/freedesktop/systemd1/unit/dev_2dsda_2edevice \
    org.freedesktop.systemd1.Unit ActiveState

# 查询设备单元的子状态
busctl get-property org.freedesktop.systemd1 \
    /org/freedesktop/systemd1/unit/dev_2dsda_2edevice \
    org.freedesktop.systemd1.Unit SubState

# 列出所有设备单元
systemctl list-units --type=device --all
```

## udev 集成

在 udev 规则中可以通过以下属性影响设备单元的行为：

| udev 属性 | 说明 |
|-----------|------|
| `SYSTEMD_WANTS=unit.service` | 设备出现时自动启动指定单元 |
| `SYSTEMD_ALIAS=/dev/mydevice` | 为设备单元添加符号名称别名 |
| `SYSTEMD_READY=0` | 标记设备尚未就绪（即使内核已报告设备存在） |
| `SYSTEMD_UNIT=my.device` | 为此设备使用自定义的单元文件 |

## 参见

- [`org.freedesktop.systemd1.Unit`](org.freedesktop.systemd1.Unit.md) — 所有单元类型的基础接口
- `systemd.device(5)` — 设备单元配置文件参考手册
- `udev(7)` — udev 设备管理器
- `systemd-udevd.service(8)` — udev 守护进程
