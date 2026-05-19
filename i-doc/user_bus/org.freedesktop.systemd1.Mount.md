# 接口名称：org.freedesktop.systemd1.Mount（用户总线版本）

此接口与系统总线上的同名接口相同，但在用户会话上下文中操作。

## 概述

`org.freedesktop.systemd1.Mount` 是 systemd 挂载单元（`.mount`）的 D-Bus 接口。挂载单元封装了文件系统挂载点，由 systemd 从 `/proc/self/mountinfo` 自动生成，也可手动创建单元文件。

## 总线连接信息

| 项目 | 值 |
|------|-----|
| 总线类型 | 用户总线（Session Bus） |
| 总线名称 | `org.freedesktop.systemd1` |
| 对象路径示例 | `/org/freedesktop/systemd1/unit/home_2emount` |

## 用户会话的关键差异

- **busctl 命令**：使用 `busctl --user`
- **实际适用性**：挂载单元在用户会话中主要用于**用户命名空间内的文件系统操作**，或监控特定挂载点状态并触发用户服务
- **挂载路径**：用户挂载点通常位于 `$XDG_RUNTIME_DIR`（`/run/user/<UID>/`）下或用户主目录中
- **无需 polkit**：会话用户可查询并（在权限允许的情况下）管理用户会话中的挂载单元
- **用户命名空间挂载**：若系统支持用户命名空间，用户可通过 `systemd-mount --user` 在用户命名空间内挂载文件系统

## 接口属性（继承自 Unit + Mount 专有属性）

挂载单元除继承 `org.freedesktop.systemd1.Unit` 全部属性外，还暴露以下 Mount 专有属性：

| 属性名称 | 类型 | 说明 |
|---------|------|------|
| `Where` | `s` | 挂载点路径 |
| `What` | `s` | 挂载的设备或源 |
| `Options` | `s` | 挂载选项 |
| `Type` | `s` | 文件系统类型（如 `ext4`、`tmpfs`） |
| `TimeoutUSec` | `t` | 挂载超时时间（微秒） |
| `ControlPID` | `u` | 当前控制进程的 PID |
| `DirectoryMode` | `u` | 自动创建挂载点目录的权限（八进制） |
| `SloppyOptions` | `b` | 是否忽略不支持的挂载选项 |
| `LazyUnmount` | `b` | 是否使用懒卸载（`MNT_DETACH`） |
| `ForceUnmount` | `b` | 是否强制卸载（即使繁忙） |
| `ReadWriteOnly` | `b` | 是否仅接受读写挂载（拒绝只读） |
| `Result` | `s` | 最终结果：`success`、`resources`、`timeout`、`exit-code`、`signal`、`core-dump` |

## 使用示例

```bash
# 查询挂载点路径
busctl --user get-property org.freedesktop.systemd1 \
    /org/freedesktop/systemd1/unit/run_2duser_2d1000_2emount \
    org.freedesktop.systemd1.Mount Where

# 查询挂载类型
busctl --user get-property org.freedesktop.systemd1 \
    /org/freedesktop/systemd1/unit/run_2duser_2d1000_2emount \
    org.freedesktop.systemd1.Mount Type
```

## 参见

- [`org.freedesktop.systemd1.Unit`](org.freedesktop.systemd1.Unit.md) — 基础接口（用户总线版本）
- `systemd.mount(5)` — 挂载单元配置文件参考手册
- 系统总线版本：`../system_bus/org.freedesktop.systemd1.Mount.md`
