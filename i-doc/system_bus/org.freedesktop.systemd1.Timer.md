# 接口名称：org.freedesktop.systemd1.Timer

## 接口说明

`org.freedesktop.systemd1.Timer` 是 systemd 中**定时器单元（`.timer`）的 D-Bus 接口**。定时器单元用于在指定时间或按照指定间隔自动激活关联的服务单元，是 systemd 对 cron 和 at 命令的替代方案。

定时器单元支持两种触发机制：

- **单调时钟定时器**：基于系统启动后的相对时间（如 `OnBootSec=`、`OnActiveSec=`、`OnUnitActiveSec=`）
- **日历定时器**：基于实时时钟的绝对时间（如 `OnCalendar=Mon *-*-* 04:00:00`）

通过此接口，客户端可以：

- **查询定时器配置**：触发规则、精度设置、节能配置等
- **查询下次触发时间**：实时时钟和单调时钟两种表示
- **查询触发历史**：上次触发时间

## 总线名称

`org.freedesktop.systemd1`

## 继承关系

```
org.freedesktop.systemd1.Unit
    └── org.freedesktop.systemd1.Timer
```

## 对象路径

`/org/freedesktop/systemd1/unit/<timer_name>`

其中 `<timer_name>` 是定时器单元名称经过 D-Bus 路径转义后的形式（例如 `logrotate.timer` 对应 `/org/freedesktop/systemd1/unit/logrotate_2etimer`）。

---

## 属性（Properties）

### 触发目标

| 属性名称 | 类型 | 读写 | 说明 |
|---------|------|------|------|
| `Unit` | `s` | 只读 | 定时器触发时激活的目标单元名称（默认与定时器同名但扩展名为 `.service`；可通过 `Unit=` 配置覆盖） |

### 定时器规则

| 属性名称 | 类型 | 读写 | 说明 |
|---------|------|------|------|
| `TimersMonotonic` | `a(stt)` | 只读 | 单调时钟定时器列表；每项为 `(设置名称, 配置值微秒, 下次触发时间微秒)`。设置名称包括：`OnActiveSec`（定时器激活后）、`OnBootSec`（系统启动后）、`OnStartupSec`（systemd 启动后）、`OnUnitActiveSec`（关联服务上次激活后）、`OnUnitInactiveSec`（关联服务上次停止后） |
| `TimersCalendar` | `a(sst)` | 只读 | 日历定时器列表；每项为 `(设置名称, 日历表达式字符串, 下次触发实时时间戳微秒)`。设置名称为 `OnCalendar`；日历表达式如 `Mon *-*-* 04:00:00`、`daily`、`weekly` |
| `OnClockChange` | `b` | 只读 | 为 `true` 时，系统实时时钟发生跳变时也触发定时器 |
| `OnTimezoneChange` | `b` | 只读 | 为 `true` 时，系统时区发生变化时也触发定时器 |

### 触发时间戳

| 属性名称 | 类型 | 读写 | 说明 |
|---------|------|------|------|
| `NextElapseUSecRealtime` | `t` | 只读 | 下次触发时间的实时时钟表示（微秒，`CLOCK_REALTIME`）；`UINT64_MAX` 表示无计划触发 |
| `NextElapseUSecMonotonic` | `t` | 只读 | 下次触发时间的单调时钟表示（微秒，`CLOCK_MONOTONIC`）；`UINT64_MAX` 表示无计划触发 |
| `LastTriggerUSec` | `t` | 只读 | 上次触发时间的实时时钟表示（微秒）；`0` 表示从未触发 |
| `LastTriggerUSecMonotonic` | `t` | 只读 | 上次触发时间的单调时钟表示（微秒）；`0` 表示从未触发 |

### 精度与节能

| 属性名称 | 类型 | 读写 | 说明 |
|---------|------|------|------|
| `AccuracyUSec` | `t` | 只读 | 定时器精度（微秒）。systemd 可以将定时器的触发时间推迟最多此值，以便与其他定时器对齐，减少系统唤醒次数（节能）。默认 `1min` |
| `RandomizedDelayUSec` | `t` | 只读 | 随机延迟范围（微秒）。每次触发时在 `[0, RandomizedDelayUSec)` 范围内随机选取一个延迟值加到触发时间上，用于避免多台机器同时触发 |
| `RandomizedOffsetUSec` | `t` | 只读 | 固定随机偏移量（微秒），当 `FixedRandomDelay=yes` 时使用 |
| `FixedRandomDelay` | `b` | 只读 | 为 `true` 时，随机延迟值在定时器的整个生命周期内保持固定（由机器 ID 和定时器名称决定），而非每次触发时重新随机生成 |
| `WakeSystem` | `b` | 只读 | 为 `true` 时，定时器到期时允许唤醒处于休眠状态的系统（使用 `CLOCK_REALTIME_ALARM` 或 `CLOCK_BOOTTIME_ALARM`） |

### 持久化与状态保留

| 属性名称 | 类型 | 读写 | 说明 |
|---------|------|------|------|
| `Persistent` | `b` | 只读 | 为 `true` 时，若定时器在停止期间错过了触发时间，则在下次启动时立即触发（对 `OnCalendar=` 有效） |
| `RemainAfterElapse` | `b` | 只读 | 为 `true` 时（默认），定时器触发后保持加载状态，方便查询 `LastTriggerUSec`；为 `false` 时触发后卸载定时器 |
| `DeferReactivation` | `b` | 只读 | 为 `true` 时，延迟定时器的重新激活，避免在关联服务退出后立即重新触发 |

### 运行结果

| 属性名称 | 类型 | 读写 | 说明 |
|---------|------|------|------|
| `Result` | `s` | 只读 | 定时器单元的最终结果：`success` / `resources`（资源不足） |

---

## 方法（Methods）

定时器单元**不定义任何特有方法**。通用控制方法由基础接口 `org.freedesktop.systemd1.Unit` 提供。

---

## 信号（Signals）

定时器单元**不定义任何特有信号**。

---

## 使用示例

```bash
# 查询 logrotate.timer 的下次触发时间（实时时钟）
busctl get-property org.freedesktop.systemd1 \
    /org/freedesktop/systemd1/unit/logrotate_2etimer \
    org.freedesktop.systemd1.Timer NextElapseUSecRealtime

# 查询日历定时器规则
busctl get-property org.freedesktop.systemd1 \
    /org/freedesktop/systemd1/unit/logrotate_2etimer \
    org.freedesktop.systemd1.Timer TimersCalendar

# 查询上次触发时间
busctl get-property org.freedesktop.systemd1 \
    /org/freedesktop/systemd1/unit/logrotate_2etimer \
    org.freedesktop.systemd1.Timer LastTriggerUSec

# 查询是否启用持久化（错过后立即触发）
busctl get-property org.freedesktop.systemd1 \
    /org/freedesktop/systemd1/unit/logrotate_2etimer \
    org.freedesktop.systemd1.Timer Persistent

# 列出所有定时器及下次触发时间
systemctl list-timers --all
```

## 配置示例

```ini
# /etc/systemd/system/backup.timer
[Unit]
Description=每日备份定时器

[Timer]
OnCalendar=daily
AccuracySec=1h
RandomizedDelaySec=30min
Persistent=true
WakeSystem=false

[Install]
WantedBy=timers.target
```

## 参见

- [`org.freedesktop.systemd1.Unit`](org.freedesktop.systemd1.Unit.md) — 所有单元类型的基础接口
- `systemd.timer(5)` — 定时器单元配置文件参考手册
- `systemd.time(7)` — systemd 时间表达式语法
- `systemctl list-timers(1)` — 查看所有定时器及触发时间
