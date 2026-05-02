# 接口名称：org.freedesktop.systemd1.Timer（用户总线版本）

此接口与系统总线上的同名接口相同，但在用户会话上下文中操作。

## 概述

`org.freedesktop.systemd1.Timer` 是 systemd 定时器单元（`.timer`）的 D-Bus 接口。定时器单元基于实时时钟或系统启动时间触发对应的服务单元，是 cron 的现代替代方案。

## 总线连接信息

| 项目 | 值 |
|------|-----|
| 总线类型 | 用户总线（Session Bus） |
| 总线名称 | `org.freedesktop.systemd1` |
| 对象路径示例 | `/org/freedesktop/systemd1/unit/backup_2etimer` |

## 用户会话的关键差异

- **busctl 命令**：使用 `busctl --user`
- **单元文件路径**：用户定时器单元文件位于 `~/.config/systemd/user/`，示例：`~/.config/systemd/user/backup.timer`
- **配套服务**：定时器触发的服务也是用户服务，运行在用户会话上下文中
- **无需 polkit**：会话用户可直接管理自己的定时器
- **持久化定时器**：`Persistent=yes` 的用户定时器会在 `~/.local/share/systemd/timers/` 目录下记录最后触发时间戳（而不是 `/var/lib/systemd/timers/`）
- **定时器激活条件**：用户定时器在用户会话期间运行；若需要在未登录时运行，需启用 lingering（`loginctl enable-linger <user>`）

## 接口属性（继承自 Unit + Timer 专有属性）

定时器单元除继承 `org.freedesktop.systemd1.Unit` 全部属性外，还暴露以下 Timer 专有属性：

| 属性名称 | 类型 | 说明 |
|---------|------|------|
| `Unit` | `s` | 该定时器激活的目标单元名称（默认为同名的 `.service`） |
| `TimersMonotonic` | `a(sttt)` | 单调时钟定时器列表，每项为 `(基准点, 偏移量微秒, 下次触发时间, 最后触发时间)` |
| `TimersCalendar` | `a(sst)` | 日历时间定时器列表，每项为 `(定时器规则名, 日历表达式, 下次触发时间)` |
| `OnClockChange` | `b` | 系统时钟变更时是否触发 |
| `OnTimezoneChange` | `b` | 时区变更时是否触发 |
| `NextElapseUSecRealtime` | `t` | 下次基于实时时钟的触发时间（微秒，UNIX 时间戳） |
| `NextElapseUSecMonotonic` | `t` | 下次基于单调时钟的触发时间（微秒） |
| `LastTriggerUSec` | `t` | 最近一次触发的实时时间戳（微秒） |
| `LastTriggerUSecMonotonic` | `t` | 最近一次触发的单调时钟时间戳（微秒） |
| `Result` | `s` | 最终结果：`success`、`resources`、`start-limit-hit` 等 |
| `AccuracyUSec` | `t` | 触发精度（微秒）；触发可延迟至多此时长以合并 CPU 唤醒 |
| `RandomizedDelayUSec` | `t` | 随机延迟的最大值（微秒） |
| `FixedRandomDelay` | `b` | 随机延迟是否固定（相同单元在不同系统上延迟相同） |
| `Persistent` | `b` | 是否启用持久化（记录最后触发时间，补触 missed 触发） |
| `WakeSystem` | `b` | 是否在触发时唤醒系统（对用户会话通常无效，需要系统级权限） |
| `RemainAfterElapse` | `b` | 触发后是否保持加载状态（`false` 表示触发后自动卸载） |

## 使用示例

```bash
# 查询用户定时器下次触发时间
busctl --user get-property org.freedesktop.systemd1 \
    /org/freedesktop/systemd1/unit/backup_2etimer \
    org.freedesktop.systemd1.Timer NextElapseUSecRealtime

# 查询用户定时器最近触发时间
busctl --user get-property org.freedesktop.systemd1 \
    /org/freedesktop/systemd1/unit/backup_2etimer \
    org.freedesktop.systemd1.Timer LastTriggerUSec

# 查询日历规则
busctl --user get-property org.freedesktop.systemd1 \
    /org/freedesktop/systemd1/unit/backup_2etimer \
    org.freedesktop.systemd1.Timer TimersCalendar
```

## 用户定时器单元文件示例

```ini
# ~/.config/systemd/user/backup.timer
[Unit]
Description=Daily Backup Timer

[Timer]
OnCalendar=daily
Persistent=true
# 持久化时间戳保存在 ~/.local/share/systemd/timers/

[Install]
WantedBy=timers.target
```

## 参见

- [`org.freedesktop.systemd1.Unit`](org.freedesktop.systemd1.Unit.md) — 基础接口（用户总线版本）
- `systemd.timer(5)` — 定时器单元配置文件参考手册
- `systemd-analyze calendar` — 分析日历时间表达式
- 系统总线版本：`../system_bus/org.freedesktop.systemd1.Timer.md`
