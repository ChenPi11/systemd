# 接口名称：org.freedesktop.systemd1.Slice（用户总线版本）

此接口与系统总线上的同名接口相同，但在用户会话上下文中操作。

## 概述

`org.freedesktop.systemd1.Slice` 是 systemd 切片单元（`.slice`）的 D-Bus 接口。Slice 单元用于将服务和 scope 单元组织成层次化的资源管理组（基于 cgroup）。

在用户会话中，`systemd --user` 维护自己的 slice 层次结构，用于对用户服务进行资源分组和限制。

## 总线连接信息

| 项目 | 值 |
|------|-----|
| 总线类型 | 用户总线（Session Bus） |
| 总线名称 | `org.freedesktop.systemd1` |
| 对象路径示例 | `/org/freedesktop/systemd1/unit/app_2eslice` |

## 用户会话的关键差异

- **busctl 命令**：使用 `busctl --user`
- **单元文件路径**：用户 slice 单元文件位于 `~/.config/systemd/user/`
- **用户会话默认 slice 层次结构**：
  - `-.slice` — 用户会话根 slice（对应 cgroup：`user@<UID>.service/`）
  - `app.slice` — 用户应用服务的默认分组（cgroup：`user@<UID>.service/app.slice/`）
  - `background.slice` — 后台任务（低优先级）
  - `session.slice` — 会话范围的服务
- **cgroup 位置**：用户 slice 的 cgroup 位于 `/sys/fs/cgroup/user.slice/user-<UID>.slice/user@<UID>.service/` 下
- **无需 polkit**：会话用户可查询并管理自己会话中的 slice 单元
- **资源限制**：对 slice 设置的 CPU、内存等限制会作用于其下所有子单元

## 接口属性（继承自 Unit + Slice 专有属性）

Slice 单元除继承 `org.freedesktop.systemd1.Unit` 全部属性（包括全部 CGroup 上下文属性）外，`org.freedesktop.systemd1.Slice` 接口本身**不定义任何额外的属性、方法或信号**。

关键继承属性（来自 CGroup 上下文）：

| 属性名称 | 类型 | 说明 |
|---------|------|------|
| `CPUWeight` | `t` | CPU 调度权重（1–10000） |
| `CPUQuotaPerSecUSec` | `t` | CPU 配额（微秒/秒） |
| `MemoryMax` | `t` | 内存使用硬上限（字节） |
| `MemoryHigh` | `t` | 内存使用软上限（字节，超出后调节但不立即停止） |
| `TasksMax` | `t` | 最大任务数（进程数）上限 |
| `IOWeight` | `t` | I/O 调度权重（1–10000） |
| `Slice` | `s` | 父 slice 单元名称 |
| `ControlGroup` | `s` | cgroup 路径 |

## 使用示例

```bash
# 查询 app.slice 的 cgroup 路径
busctl --user get-property org.freedesktop.systemd1 \
    /org/freedesktop/systemd1/unit/app_2eslice \
    org.freedesktop.systemd1.Unit ControlGroup

# 查询 app.slice 的内存限制
busctl --user get-property org.freedesktop.systemd1 \
    /org/freedesktop/systemd1/unit/app_2eslice \
    org.freedesktop.systemd1.Unit MemoryMax

# 通过 D-Bus 设置 app.slice 的内存限制（持久化）
busctl --user call org.freedesktop.systemd1 \
    /org/freedesktop/systemd1/unit/app_2eslice \
    org.freedesktop.systemd1.Unit \
    SetProperties 'ba(sv)' true 1 MemoryMax t 1073741824
```

## 参见

- [`org.freedesktop.systemd1.Unit`](org.freedesktop.systemd1.Unit.md) — 基础接口（用户总线版本）
- `systemd.slice(5)` — Slice 单元配置文件参考手册
- `systemd.resource-control(5)` — 资源控制参考手册
- 系统总线版本：`../system_bus/org.freedesktop.systemd1.Slice.md`
