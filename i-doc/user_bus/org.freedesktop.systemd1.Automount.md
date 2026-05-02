# 接口名称：org.freedesktop.systemd1.Automount（用户总线版本）

此接口与系统总线上的同名接口相同，但在用户会话上下文中操作。

## 概述

`org.freedesktop.systemd1.Automount` 是 systemd 自动挂载单元（`.automount`）的 D-Bus 接口。自动挂载单元实现内核自动挂载（autofs）功能——当访问特定路径时，systemd 自动触发并挂载对应的挂载单元。

## 总线连接信息

| 项目 | 值 |
|------|-----|
| 总线类型 | 用户总线（Session Bus） |
| 总线名称 | `org.freedesktop.systemd1` |
| 对象路径示例 | `/org/freedesktop/systemd1/unit/mnt_2ddata_2eautomount` |

## 用户会话的关键差异

- **busctl 命令**：使用 `busctl --user`
- **实际适用性**：自动挂载在用户会话中主要用于**用户家目录下的按需挂载**，例如 NFS 家目录的子目录、网络共享等
- **挂载点路径**：用户自动挂载单元的挂载点通常位于用户主目录下（如 `~/mnt/nas`）
- **无需 polkit**：会话用户可查询并管理自己用户会话中的自动挂载单元
- **关联的挂载单元**：每个 `.automount` 单元通常与同名的 `.mount` 单元配合使用

## 接口属性（继承自 Unit + Automount 专有属性）

自动挂载单元除继承 `org.freedesktop.systemd1.Unit` 全部属性外，还暴露以下 Automount 专有属性：

| 属性名称 | 类型 | 说明 |
|---------|------|------|
| `Where` | `s` | 自动挂载的路径（访问此路径时触发挂载） |
| `ExtraOptions` | `s` | 额外的 autofs 挂载选项 |
| `DirectoryMode` | `u` | 自动创建挂载点目录的权限（八进制） |
| `TimeoutIdleUSec` | `t` | 空闲超时时间（微秒）；超过此时间无访问后自动卸载，`0` 表示禁用自动卸载 |
| `Result` | `s` | 最终结果：`success`、`resources`、`start-limit-hit` 等 |

## 使用示例

```bash
# 查询自动挂载路径
busctl --user get-property org.freedesktop.systemd1 \
    /org/freedesktop/systemd1/unit/home_2enas_2eautomount \
    org.freedesktop.systemd1.Automount Where

# 查询空闲超时配置
busctl --user get-property org.freedesktop.systemd1 \
    /org/freedesktop/systemd1/unit/home_2enas_2eautomount \
    org.freedesktop.systemd1.Automount TimeoutIdleUSec
```

## 参见

- [`org.freedesktop.systemd1.Unit`](org.freedesktop.systemd1.Unit.md) — 基础接口（用户总线版本）
- [`org.freedesktop.systemd1.Mount`](org.freedesktop.systemd1.Mount.md) — 与 automount 配对使用的挂载单元接口（用户总线版本）
- `systemd.automount(5)` — 自动挂载单元配置文件参考手册
- 系统总线版本：`../system_bus/org.freedesktop.systemd1.Automount.md`
