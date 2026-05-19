# 接口名称：org.freedesktop.systemd1.Target（用户总线版本）

此接口与系统总线上的同名接口相同，但在用户会话上下文中操作。

## 概述

`org.freedesktop.systemd1.Target` 是 systemd 目标单元（`.target`）的 D-Bus 接口。目标单元本身不执行任何命令，仅用于将其他单元聚合在一起作为同步点。该接口除继承自 `org.freedesktop.systemd1.Unit` 的全部属性和方法外，**不定义任何额外的属性、方法或信号**。

## 总线连接信息

| 项目 | 值 |
|------|-----|
| 总线类型 | 用户总线（Session Bus） |
| 总线名称 | `org.freedesktop.systemd1` |
| 对象路径示例 | `/org/freedesktop/systemd1/unit/default_2etarget` |

## 用户会话的关键差异

- **busctl 命令**：使用 `busctl --user`（而非默认的系统总线）
- **单元文件路径**：用户目标单元文件位于 `~/.config/systemd/user/`（如 `default.target`、`graphical-session.target`）
- **系统路径**：`/usr/lib/systemd/user/` 下有许多预定义的用户目标单元
- **无需 polkit**：会话用户可直接操作自己的用户目标单元
- **常用用户目标**：`default.target`、`basic.target`、`sockets.target`、`graphical-session.target`、`graphical-session-pre.target`

## 使用示例

```bash
# 查询 default.target 的属性
busctl --user introspect org.freedesktop.systemd1 \
    /org/freedesktop/systemd1/unit/default_2etarget \
    org.freedesktop.systemd1.Unit

# 切换用户会话目标（相当于 systemctl --user isolate graphical-session.target）
busctl --user call org.freedesktop.systemd1 \
    /org/freedesktop/systemd1/unit/graphical_2dsession_2etarget \
    org.freedesktop.systemd1.Unit \
    Start s isolate
```

## 参见

- [`org.freedesktop.systemd1.Unit`](org.freedesktop.systemd1.Unit.md) — 基础接口（用户总线版本）
- `systemd.target(5)` — 目标单元配置文件参考手册
- 系统总线版本：`../system_bus/org.freedesktop.systemd1.Target.md`
