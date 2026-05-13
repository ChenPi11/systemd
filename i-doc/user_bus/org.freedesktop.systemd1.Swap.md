# 接口名称：org.freedesktop.systemd1.Swap（用户总线版本）

此接口与系统总线上的同名接口相同，但在用户会话上下文中操作。

## 概述

`org.freedesktop.systemd1.Swap` 是 systemd 交换分区/交换文件单元（`.swap`）的 D-Bus 接口。交换单元封装了内核的 swap 设备或文件，由 systemd 从 `/proc/swaps` 自动生成，也可手动创建单元文件。

## 总线连接信息

| 项目 | 值 |
|------|-----|
| 总线类型 | 用户总线（Session Bus） |
| 总线名称 | `org.freedesktop.systemd1` |
| 对象路径示例 | `/org/freedesktop/systemd1/unit/dev_2dsda2_2eswap` |

## 用户会话的关键差异

- **busctl 命令**：使用 `busctl --user`
- **实际适用性**：交换单元通常由系统级 systemd 管理；用户会话中的 `systemd --user` 也会感知这些单元（若系统总线上有对应单元），但**主动管理**（启用/停用 swap）通常需要特权
- **查询用途**：在用户会话上下文中，通常只读取交换单元状态，而非控制它
- **无需 polkit**：会话用户可查询用户实例中可见的交换单元信息

## 接口属性（继承自 Unit + Swap 专有属性）

交换单元除继承 `org.freedesktop.systemd1.Unit` 全部属性外，还暴露以下 Swap 专有属性：

| 属性名称 | 类型 | 说明 |
|---------|------|------|
| `What` | `s` | 交换设备或文件路径 |
| `Priority` | `i` | swap 优先级（`-1` 表示使用内核默认优先级） |
| `Options` | `s` | 附加的 swap 选项 |
| `TimeoutUSec` | `t` | 激活/停用超时时间（微秒） |
| `ControlPID` | `u` | 当前控制进程的 PID（如执行 `swapon`/`swapoff` 的进程） |
| `Result` | `s` | 最终结果：`success`、`resources`、`timeout`、`exit-code`、`signal`、`core-dump` |

## 使用示例

```bash
# 查询交换设备路径
busctl --user get-property org.freedesktop.systemd1 \
    /org/freedesktop/systemd1/unit/dev_2dsda2_2eswap \
    org.freedesktop.systemd1.Swap What

# 查询 swap 优先级
busctl --user get-property org.freedesktop.systemd1 \
    /org/freedesktop/systemd1/unit/dev_2dsda2_2eswap \
    org.freedesktop.systemd1.Swap Priority
```

## 参见

- [`org.freedesktop.systemd1.Unit`](org.freedesktop.systemd1.Unit.md) — 基础接口（用户总线版本）
- `systemd.swap(5)` — 交换单元配置文件参考手册
- 系统总线版本：`../system_bus/org.freedesktop.systemd1.Swap.md`
