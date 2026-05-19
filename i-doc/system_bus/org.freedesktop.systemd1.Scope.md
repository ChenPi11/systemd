# 接口名称：org.freedesktop.systemd1.Scope

## 接口说明

`org.freedesktop.systemd1.Scope` 是 systemd 中**范围单元（`.scope`）的 D-Bus 接口**。与其他单元类型不同，范围单元**不由 systemd 启动进程**，而是由外部程序（通常是系统服务或用户应用）通过 D-Bus 创建，用于将一组外部启动的进程纳入 systemd 的管理框架。

范围单元的典型用途包括：

- **容器运行时管理**：将容器内的进程注册到 systemd cgroup 层级中（如 `libvirt-machine-*.scope`）
- **用户会话进程管理**：GNOME、KDE 等桌面环境将用户启动的应用注册为 scope 单元进行资源管理
- **系统服务派生进程管理**：将服务动态创建的进程组纳入独立的 cgroup 进行资源控制

> **注意**：范围单元通过 `org.freedesktop.systemd1.Manager` 接口的 `StartTransientUnit()` 方法创建，不能手动编写 `.scope` 单元文件。

## 总线名称

`org.freedesktop.systemd1`

## 继承关系

```
org.freedesktop.systemd1.Unit
    └── org.freedesktop.systemd1.Scope
```

## 对象路径

`/org/freedesktop/systemd1/unit/<scope_name>`

其中 `<scope_name>` 是范围单元名称经过 D-Bus 路径转义后的形式（例如 `session-1.scope` 对应 `/org/freedesktop/systemd1/unit/session_2d1_2escope`）。

---

## 属性（Properties）

| 属性名称 | 类型 | 读写 | 说明 |
|---------|------|------|------|
| `Controller` | `s` | 只读 | 控制此 scope 单元的 D-Bus 连接名称（唯一名称，如 `:1.42`）。控制器负责通过 `Abandon()` 方法或连接断开来释放控制权 |
| `TimeoutStopUSec` | `t` | 只读 | 停止超时时间（微秒）；超时后发送 `SIGKILL` 强制终止 scope 内的所有进程 |
| `Result` | `s` | 只读 | 范围单元的最终结果：`success` / `resources`（资源不足）/ `timeout`（停止超时）/ `kill-context`（被 KillContext 设置强制终止） |
| `RuntimeMaxUSec` | `t` | 只读 | 最大运行时间（微秒）；超过此时间后 scope 将被停止，`0` 表示无限制 |
| `RuntimeRandomizedExtraUSec` | `t` | 只读 | 在 `RuntimeMaxUSec` 基础上随机增加的额外时间（微秒），用于错开多个 scope 的超时 |
| `OOMPolicy` | `s` | 只读 | OOM 终止策略：`stop`（停止 scope）/ `continue`（继续运行）/ `kill`（终止整个 cgroup） |

---

## 方法（Methods）

### Abandon

**说明：** 放弃对此 scope 单元的控制权。调用此方法后，控制器与 scope 的绑定关系解除；scope 内的进程继续运行，但 scope 进入**无控制器状态**，不再向控制器发送 `RequestStop` 信号。

当控制器的 D-Bus 连接断开时，systemd 也会自动触发相应的放弃逻辑（即连接断开等同于调用 `Abandon()`）。

**参数：** 无

**返回：** 无

---

## 信号（Signals）

### RequestStop

**说明：** 当 scope 单元被请求停止时（例如通过 `systemctl stop` 命令），systemd 向控制器发出此信号，通知其进行优雅关闭操作。控制器应负责终止 scope 内的进程，或调用 `Abandon()` 放弃控制权。

**参数：** 无（此信号无参数）

> **注意**：此信号发送给 scope 的控制器 D-Bus 连接，而非广播给所有 D-Bus 客户端。

---

## Scope 单元的创建方式

通过 `Manager` 接口的 `StartTransientUnit()` 创建 scope 单元：

```python
import dbus

bus = dbus.SystemBus()
manager = bus.get_object('org.freedesktop.systemd1',
                          '/org/freedesktop/systemd1')
iface = dbus.Interface(manager, 'org.freedesktop.systemd1.Manager')

# 创建临时 scope 单元，将当前进程（PID）纳入管理
properties = [
    ('PIDs', dbus.Array([dbus.UInt32(12345)], signature='u')),
]
job = iface.StartTransientUnit(
    'myapp.scope',      # scope 单元名称
    'fail',             # 入队模式
    properties,         # 属性列表
    []                  # 辅助单元列表
)
```

---

## 使用示例

```bash
# 查询 scope 的控制器 D-Bus 连接名
busctl get-property org.freedesktop.systemd1 \
    /org/freedesktop/systemd1/unit/session_2d1_2escope \
    org.freedesktop.systemd1.Scope Controller

# 查询停止超时时间
busctl get-property org.freedesktop.systemd1 \
    /org/freedesktop/systemd1/unit/session_2d1_2escope \
    org.freedesktop.systemd1.Scope TimeoutStopUSec

# 查询运行结果
busctl get-property org.freedesktop.systemd1 \
    /org/freedesktop/systemd1/unit/session_2d1_2escope \
    org.freedesktop.systemd1.Scope Result

# 调用 Abandon 放弃控制
busctl call org.freedesktop.systemd1 \
    /org/freedesktop/systemd1/unit/session_2d1_2escope \
    org.freedesktop.systemd1.Scope Abandon

# 列出所有 scope 单元
systemctl list-units --type=scope
```

## 参见

- [`org.freedesktop.systemd1.Unit`](org.freedesktop.systemd1.Unit.md) — 所有单元类型的基础接口
- [`org.freedesktop.systemd1.Manager`](org.freedesktop.systemd1.Manager.md) — 管理器接口（`StartTransientUnit` 方法）
- `systemd.scope(5)` — 范围单元配置参考手册
- `sd_bus_open_system(3)` — 连接系统总线的 API
