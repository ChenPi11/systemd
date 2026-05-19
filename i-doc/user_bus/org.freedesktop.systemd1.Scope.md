# 接口名称：org.freedesktop.systemd1.Scope（用户总线版本）

此接口与系统总线上的同名接口相同，但在用户会话上下文中操作。

## 概述

`org.freedesktop.systemd1.Scope` 是 systemd 作用域单元（`.scope`）的 D-Bus 接口。Scope 单元用于管理**由外部程序创建的进程组**（而非由 systemd 直接启动）。Scope 单元只能通过 D-Bus API 动态创建（`StartTransientUnit`），不能通过单元文件创建。

在用户总线上，用户进程（如桌面应用、终端模拟器等）可以通过 `StartTransientUnit` 将自己注册到用户会话的 scope 中，从而受益于 systemd 的资源管理（cgroup）功能。

## 总线连接信息

| 项目 | 值 |
|------|-----|
| 总线类型 | 用户总线（Session Bus） |
| 总线名称 | `org.freedesktop.systemd1` |
| 对象路径示例 | `/org/freedesktop/systemd1/unit/app_2dmyapp_2d12345_2escope` |

## 用户会话的关键差异

- **busctl 命令**：使用 `busctl --user`
- **创建方式**：通过用户总线调用 `Manager.StartTransientUnit`，将外部进程的 PID 注册到 scope 中
- **常见使用场景**：`systemd-run --user --scope myapp`，或桌面环境自动将图形应用分配到 scope 单元中
- **cgroup 位置**：用户 scope 的 cgroup 位于 `/sys/fs/cgroup/user.slice/user-<UID>.slice/user@<UID>.service/app.slice/` 下
- **无需 polkit**：会话用户可通过用户总线创建和管理自己的 scope 单元
- **资源限制**：可通过 scope 属性对进程组设置 CPU、内存等资源限制（即使对方不是 systemd 服务）

## 接口属性（继承自 Unit + Scope 专有属性）

Scope 单元除继承 `org.freedesktop.systemd1.Unit` 全部属性外，还暴露以下 Scope 专有属性：

| 属性名称 | 类型 | 说明 |
|---------|------|------|
| `Controller` | `s` | 控制此 scope 的 D-Bus 名称（通常是创建该 scope 的客户端总线名） |
| `TimeoutStopUSec` | `t` | 停止超时时间（微秒）；超时后强制终止 scope 内所有进程 |
| `Result` | `s` | 最终结果：`success`、`resources`、`timeout`、`oom-kill` 等 |
| `OOMPolicy` | `s` | OOM 终止策略：`stop`（停止 scope）/ `continue`（继续运行）/ `kill`（终止整个 cgroup） |
| `RuntimeMaxUSec` | `t` | 最大运行时间（微秒），`0` 表示无限制 |
| `RuntimeRandomizedExtraUSec` | `t` | 随机附加运行时间（微秒） |

## 使用示例

```bash
# 通过用户总线创建一个临时 scope 单元
# （将当前 shell 的子进程注册到 myscope.scope 中）
systemd-run --user --scope --unit=myscope.scope bash -c 'sleep 3600'

# 查询 scope 的控制器（D-Bus 名称）
busctl --user get-property org.freedesktop.systemd1 \
    /org/freedesktop/systemd1/unit/myscope_2escope \
    org.freedesktop.systemd1.Scope Controller

# 通过 D-Bus 创建一个用户 scope 单元并设置内存限制
busctl --user call org.freedesktop.systemd1 \
    /org/freedesktop/systemd1 \
    org.freedesktop.systemd1.Manager \
    StartTransientUnit 'ssa(sv)a(sa(sv))' \
    myscope.scope replace \
    2 \
    PIDs au 1 $$ \
    MemoryMax t 104857600 \
    0
```

## 参见

- [`org.freedesktop.systemd1.Unit`](org.freedesktop.systemd1.Unit.md) — 基础接口（用户总线版本）
- [`org.freedesktop.systemd1.Manager`](org.freedesktop.systemd1.Manager.md) — `StartTransientUnit` 方法（用户总线版本）
- `systemd.scope(5)` — Scope 单元参考手册
- `systemd-run(1)` — 以临时单元形式运行命令
- 系统总线版本：`../system_bus/org.freedesktop.systemd1.Scope.md`
