# 接口名称：org.freedesktop.systemd1.Device（用户总线版本）

此接口与系统总线上的同名接口相同，但在用户会话上下文中操作。

## 概述

`org.freedesktop.systemd1.Device` 是 systemd 设备单元（`.device`）的 D-Bus 接口。设备单元对应内核暴露的 udev 设备，由 systemd 根据 udev 规则自动创建，不能手动创建或控制。该接口除继承自 `org.freedesktop.systemd1.Unit` 的全部属性和方法外，**不定义任何额外的属性、方法或信号**。

## 总线连接信息

| 项目 | 值 |
|------|-----|
| 总线类型 | 用户总线（Session Bus） |
| 总线名称 | `org.freedesktop.systemd1` |
| 对象路径示例 | `/org/freedesktop/systemd1/unit/dev_2dsda_2edevice` |

## 用户会话的关键差异

- **busctl 命令**：使用 `busctl --user`（而非默认的系统总线）
- **实际适用性**：设备单元主要由系统级 systemd 管理；在用户会话（`systemd --user`）中，设备单元的使用场景较少，通常用于在特定设备出现时激活某个用户服务
- **无需 polkit**：会话用户可查询自己用户实例中的设备单元属性
- **udev 规则**：与系统总线相同，设备单元由 udev 规则中的 `TAG+="systemd-user"` 或类似规则触发，但激活目标是用户会话中的单元

## 使用示例

```bash
# 查询用户会话中某设备单元的状态
busctl --user get-property org.freedesktop.systemd1 \
    /org/freedesktop/systemd1/unit/dev_2dsda_2edevice \
    org.freedesktop.systemd1.Unit ActiveState

# 列出用户会话中所有单元（包括设备单元）
busctl --user call org.freedesktop.systemd1 \
    /org/freedesktop/systemd1 \
    org.freedesktop.systemd1.Manager \
    ListUnits
```

## 参见

- [`org.freedesktop.systemd1.Unit`](org.freedesktop.systemd1.Unit.md) — 基础接口（用户总线版本）
- `systemd.device(5)` — 设备单元配置文件参考手册
- `udev(7)` — Linux 设备管理器
- 系统总线版本：`../system_bus/org.freedesktop.systemd1.Device.md`
