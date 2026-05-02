# 接口名称：org.freedesktop.systemd1.Slice

## 接口说明

`org.freedesktop.systemd1.Slice` 是 systemd 中**切片单元（`.slice`）的 D-Bus 接口**。切片单元用于构建 systemd 的**层级化 cgroup 树**，将服务、范围等单元组织成资源管理树，以实现对一组进程进行统一的资源配额控制。

切片单元本身不直接管理进程，而是作为**资源管理容器**，其子单元（服务、范围等）共享该切片的资源配额。

系统内置的切片单元包括：

| 切片名称 | 说明 |
|---------|------|
| `-.slice` | 根切片，所有其他切片的父级 |
| `system.slice` | 系统服务的默认切片 |
| `user.slice` | 用户会话的默认切片 |
| `machine.slice` | 虚拟机和容器的切片 |

## 总线名称

`org.freedesktop.systemd1`

## 继承关系

```
org.freedesktop.systemd1.Unit
    └── org.freedesktop.systemd1.Slice
```

## 对象路径

`/org/freedesktop/systemd1/unit/<slice_name>`

其中 `<slice_name>` 是切片单元名称经过 D-Bus 路径转义后的形式（例如 `system.slice` 对应 `/org/freedesktop/systemd1/unit/system_2eslice`；`user-1000.slice` 对应 `/org/freedesktop/systemd1/unit/user_2d1000_2eslice`）。

---

## 属性（Properties）

### 切片特有属性

| 属性名称 | 类型 | 读写 | 说明 |
|---------|------|------|------|
| `ConcurrencyHardMax` | `u` | 只读 | 此切片中可同时处于活跃状态的直接子单元数量硬上限；`0` 表示无限制。超过上限后新的子单元将无法启动 |
| `ConcurrencySoftMax` | `u` | 只读 | 此切片中可同时处于活跃状态的直接子单元数量软上限；`0` 表示无限制。超过软上限后系统可能采取降级措施，但不强制阻止 |
| `NCurrentlyActive` | `u` | 只读 | 当前处于活跃状态（`active`）的直接子单元数量（不含嵌套子切片中的单元） |

### 继承自 Unit 接口的关键属性

| 属性名称 | 类型 | 说明 |
|---------|------|------|
| `Id` | `s` | 切片单元名称（如 `system.slice`） |
| `ActiveState` | `s` | 活跃状态：`active`（已激活）/ `inactive`（未激活） |
| `SubState` | `s` | 子状态：切片单元通常为 `active` 或 `dead` |

### 继承自 CGroup 上下文的关键属性

切片单元的核心功能是资源管理，以下 CGroup 属性对切片单元尤为重要：

| 属性名称 | 类型 | 说明 |
|---------|------|------|
| `CPUWeight` | `t` | CPU 调度权重（1–10000，默认 100） |
| `CPUQuotaPerSecUSec` | `t` | CPU 配额（微秒/秒，即百分比；`UINT64_MAX` 表示无限制） |
| `MemoryMax` | `t` | 内存使用硬上限（字节；`UINT64_MAX` 表示无限制） |
| `MemoryHigh` | `t` | 内存使用软上限（字节；超过后降低内存使用速度） |
| `MemorySwapMax` | `t` | 内存+交换合计硬上限（字节） |
| `TasksMax` | `t` | 切片内最大任务数（进程数）上限 |
| `IOWeight` | `t` | I/O 调度权重（1–10000，默认 100） |
| `Slice` | `s` | 此切片所属的父切片名称 |

---

## 方法（Methods）

切片单元**不定义任何特有方法**。通用控制方法由基础接口 `org.freedesktop.systemd1.Unit` 提供。

---

## 信号（Signals）

切片单元**不定义任何特有信号**。

---

## 切片单元命名规则

切片单元遵循层级命名规则，使用 `-` 作为路径分隔符：

| 切片名称 | cgroup 路径 | 说明 |
|---------|-----------|------|
| `-.slice` | `/` | 根切片 |
| `system.slice` | `/system.slice` | 系统服务切片 |
| `user.slice` | `/user.slice` | 用户会话切片 |
| `user-1000.slice` | `/user.slice/user-1000.slice` | UID 1000 的用户切片 |
| `myapp.slice` | `/system.slice/myapp.slice`（默认）| 自定义应用切片 |

---

## 使用示例

```bash
# 查询 system.slice 的当前活跃子单元数
busctl get-property org.freedesktop.systemd1 \
    /org/freedesktop/systemd1/unit/system_2eslice \
    org.freedesktop.systemd1.Slice NCurrentlyActive

# 查询并发硬上限
busctl get-property org.freedesktop.systemd1 \
    /org/freedesktop/systemd1/unit/system_2eslice \
    org.freedesktop.systemd1.Slice ConcurrencyHardMax

# 查询切片的内存使用上限（来自 CGroup 上下文）
busctl get-property org.freedesktop.systemd1 \
    /org/freedesktop/systemd1/unit/system_2eslice \
    org.freedesktop.systemd1.Unit MemoryMax

# 列出所有切片单元
systemctl list-units --type=slice

# 查看切片的 cgroup 树结构
systemd-cgls /system.slice
```

## 配置示例

```ini
# /etc/systemd/system/myapp.slice
[Unit]
Description=My Application Slice
Before=slices.target

[Slice]
MemoryMax=2G
CPUWeight=200
TasksMax=100
ConcurrencyHardMax=10
```

在服务单元中指定所属切片：

```ini
# /etc/systemd/system/myapp.service
[Unit]
Description=My Application

[Service]
Slice=myapp.slice
ExecStart=/usr/bin/myapp
```

## 参见

- [`org.freedesktop.systemd1.Unit`](org.freedesktop.systemd1.Unit.md) — 所有单元类型的基础接口
- `systemd.slice(5)` — 切片单元配置文件参考手册
- `systemd.resource-control(5)` — cgroup 资源控制配置参考
- `systemd-cgls(1)` — 查看 cgroup 层级树
- `cgroups(7)` — Linux 控制组（cgroup）机制
