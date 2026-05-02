# 接口名称：org.freedesktop.systemd1.Mount

## 接口说明

`org.freedesktop.systemd1.Mount` 是 systemd 中**挂载单元（`.mount`）的 D-Bus 接口**。挂载单元用于管理文件系统挂载点的生命周期，可以是手动创建的单元文件，也可以由 systemd 根据 `/etc/fstab` 的内容自动生成。

通过此接口，客户端可以：

- **查询挂载配置**：挂载点、设备来源、文件系统类型和挂载选项
- **查询挂载状态**：控制进程 PID、操作结果等
- **监控挂载事件**：通过 `ActiveState` 属性变化感知挂载/卸载事件

> **注意**：此接口同时继承 `org.freedesktop.systemd1.Unit` 接口的全部属性和方法，以及执行上下文（ExecuteContext）和 CGroup 上下文的全部属性。

## 总线名称

`org.freedesktop.systemd1`

## 继承关系

```
org.freedesktop.systemd1.Unit
    └── org.freedesktop.systemd1.Mount
```

## 对象路径

`/org/freedesktop/systemd1/unit/<mount_name>`

其中 `<mount_name>` 是挂载单元名称经过 D-Bus 路径转义后的形式。挂载单元名称由挂载点路径转换而来（例如 `/home` 对应单元 `home.mount`，路径为 `/org/freedesktop/systemd1/unit/home_2emount`；`/mnt/data` 对应 `mnt-data.mount`）。

---

## 属性（Properties）

### 挂载配置

| 属性名称 | 类型 | 读写 | 说明 |
|---------|------|------|------|
| `Where` | `s` | 只读 | 文件系统挂载点的绝对路径（如 `/home`、`/mnt/data`） |
| `What` | `s` | 只读 | 要挂载的设备、网络路径或特殊文件系统标识（如 `/dev/sda1`、`//server/share`、`tmpfs`） |
| `Options` | `s` | 只读 | 挂载选项字符串，与 `mount(8)` 的 `-o` 参数格式相同（如 `defaults,noatime,ro`） |
| `Type` | `s` | 只读 | 文件系统类型（如 `ext4`、`xfs`、`tmpfs`、`nfs`、`cifs`）；为空时由内核自动探测 |
| `SloppyOptions` | `b` | 只读 | 为 `true` 时，忽略 `Options` 中对当前内核不支持的选项（相当于 `mount -s`） |
| `LazyUnmount` | `b` | 只读 | 为 `true` 时，使用懒惰卸载（`umount -l`）：即使挂载点仍有文件打开，也立即从目录树中分离，待所有引用关闭后再真正卸载 |
| `ForceUnmount` | `b` | 只读 | 为 `true` 时，使用强制卸载（`umount -f`）：即使文件系统不可达（如 NFS 服务器宕机）也强制卸载 |
| `ReadWriteOnly` | `b` | 只读 | 为 `true` 时，若文件系统以只读方式挂载，则拒绝挂载（要求必须以读写方式挂载） |
| `DirectoryMode` | `u` | 只读 | 自动创建挂载点目录时使用的文件权限（八进制，如 `0755`） |
| `TimeoutUSec` | `t` | 只读 | 挂载/卸载操作的超时时间（微秒）；超时后视为失败 |

### 运行状态

| 属性名称 | 类型 | 读写 | 说明 |
|---------|------|------|------|
| `ControlPID` | `u` | 只读 | 当前控制进程（执行 `mount`/`umount` 的进程）的 PID；无则为 `0` |
| `UID` | `u` | 只读 | 控制进程的有效用户 ID |
| `GID` | `u` | 只读 | 控制进程的有效组 ID |
| `Result` | `s` | 只读 | 挂载操作的最终结果：`success` / `resources`（资源不足）/ `timeout`（操作超时）/ `exit-code`（mount 命令异常退出）/ `signal`（被信号终止）/ `core-dump`（产生核心转储） |
| `ReloadResult` | `s` | 只读 | 最近一次重载（重新挂载 `remount`）操作的结果 |
| `CleanResult` | `s` | 只读 | 最近一次清理操作的结果 |

---

## 方法（Methods）

挂载单元**不定义任何特有方法**。通用控制方法（启动对应挂载、停止对应卸载等）由基础接口 `org.freedesktop.systemd1.Unit` 提供。

---

## 信号（Signals）

挂载单元**不定义任何特有信号**。

---

## 挂载单元命名规则

挂载点路径到单元名称的转换规则：

| 挂载点路径 | 对应单元名称 |
|-----------|-------------|
| `/` | `-.mount` |
| `/home` | `home.mount` |
| `/mnt/data` | `mnt-data.mount` |
| `/var/lib/docker` | `var-lib-docker.mount` |
| `/media/usb drive` | `media-usb\x20drive.mount`（空格转义） |

---

## 使用示例

```bash
# 查询 /home 挂载点的设备来源
busctl get-property org.freedesktop.systemd1 \
    /org/freedesktop/systemd1/unit/home_2emount \
    org.freedesktop.systemd1.Mount What

# 查询 /home 的文件系统类型
busctl get-property org.freedesktop.systemd1 \
    /org/freedesktop/systemd1/unit/home_2emount \
    org.freedesktop.systemd1.Mount Type

# 查询挂载选项
busctl get-property org.freedesktop.systemd1 \
    /org/freedesktop/systemd1/unit/home_2emount \
    org.freedesktop.systemd1.Mount Options

# 查询挂载操作结果
busctl get-property org.freedesktop.systemd1 \
    /org/freedesktop/systemd1/unit/home_2emount \
    org.freedesktop.systemd1.Mount Result

# 通过 Unit 接口触发挂载
busctl call org.freedesktop.systemd1 \
    /org/freedesktop/systemd1/unit/home_2emount \
    org.freedesktop.systemd1.Unit Start s "replace"

# 通过 Unit 接口触发卸载
busctl call org.freedesktop.systemd1 \
    /org/freedesktop/systemd1/unit/home_2emount \
    org.freedesktop.systemd1.Unit Stop s "replace"
```

## 与 /etc/fstab 的关系

systemd 在启动时会自动将 `/etc/fstab` 中的每个挂载条目转换为对应的 `.mount` 单元：

- `nofail` 选项 → 单元添加 `WantedBy=local-fs.target`（可选挂载）
- `auto` 选项（默认）→ 单元添加 `WantedBy=local-fs.target`
- `noauto` 选项 → 单元不自动启动
- `_netdev` 选项 → 单元依赖 `network-online.target`

## 参见

- [`org.freedesktop.systemd1.Unit`](org.freedesktop.systemd1.Unit.md) — 所有单元类型的基础接口
- [`org.freedesktop.systemd1.Automount`](org.freedesktop.systemd1.Automount.md) — 自动挂载单元接口
- `systemd.mount(5)` — 挂载单元配置文件参考手册
- `mount(8)` — Linux 挂载命令
- `fstab(5)` — 文件系统静态配置文件格式
