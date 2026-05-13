# 接口名称：org.freedesktop.systemd1.Path（用户总线版本）

此接口与系统总线上的同名接口相同，但在用户会话上下文中操作。

## 概述

`org.freedesktop.systemd1.Path` 是 systemd 路径单元（`.path`）的 D-Bus 接口。路径单元监控文件系统路径的变化（通过 `inotify`），并在特定事件发生时触发对应的服务单元。

## 总线连接信息

| 项目 | 值 |
|------|-----|
| 总线类型 | 用户总线（Session Bus） |
| 总线名称 | `org.freedesktop.systemd1` |
| 对象路径示例 | `/org/freedesktop/systemd1/unit/watch_2epath` |

## 用户会话的关键差异

- **busctl 命令**：使用 `busctl --user`
- **单元文件路径**：用户路径单元文件位于 `~/.config/systemd/user/`，示例：`~/.config/systemd/user/watch-inbox.path`
- **监控路径**：通常监控用户主目录下的路径（如 `~/Inbox/`、`$XDG_RUNTIME_DIR/`）
- **触发服务**：路径变化时触发的服务也是用户服务，运行在用户会话上下文中
- **无需 polkit**：会话用户可直接管理自己的路径单元
- **支持 `%h` 说明符**：用户单元文件中可使用 `%h`（用户主目录）、`%t`（`$XDG_RUNTIME_DIR`）等说明符

## 接口属性（继承自 Unit + Path 专有属性）

路径单元除继承 `org.freedesktop.systemd1.Unit` 全部属性外，还暴露以下 Path 专有属性：

| 属性名称 | 类型 | 说明 |
|---------|------|------|
| `Unit` | `s` | 该路径单元激活的目标单元名称（默认为同名的 `.service`） |
| `Paths` | `a(ss)` | 监控路径列表，每项为 `(监控类型, 路径)` 格式；监控类型包括：`PathExists`（路径存在时触发）、`PathExistsGlob`（glob匹配存在时触发）、`PathChanged`（路径内容变化时触发）、`PathModified`（路径修改时触发）、`DirectoryNotEmpty`（目录非空时触发） |
| `MakeDirectory` | `b` | 若监控目录不存在，是否自动创建 |
| `DirectoryMode` | `u` | 自动创建监控目录的权限（八进制） |
| `TriggerLimitIntervalUSec` | `t` | 触发频率限制时间窗口（微秒） |
| `TriggerLimitBurst` | `u` | 时间窗口内允许的最大触发次数 |
| `Result` | `s` | 最终结果：`success`、`resources`、`start-limit-hit`、`trigger-limit-hit` 等 |

## 使用示例

```bash
# 查询路径监控列表
busctl --user get-property org.freedesktop.systemd1 \
    /org/freedesktop/systemd1/unit/watch_2dinbox_2epath \
    org.freedesktop.systemd1.Path Paths

# 查询触发的目标单元
busctl --user get-property org.freedesktop.systemd1 \
    /org/freedesktop/systemd1/unit/watch_2dinbox_2epath \
    org.freedesktop.systemd1.Path Unit
```

## 用户路径单元文件示例

```ini
# ~/.config/systemd/user/watch-inbox.path
[Unit]
Description=Watch Inbox Directory for New Files

[Path]
PathChanged=%h/Inbox
# %h 展开为用户主目录

[Install]
WantedBy=default.target
```

## 参见

- [`org.freedesktop.systemd1.Unit`](org.freedesktop.systemd1.Unit.md) — 基础接口（用户总线版本）
- `systemd.path(5)` — 路径单元配置文件参考手册
- `inotify(7)` — Linux 文件系统监控接口
- 系统总线版本：`../system_bus/org.freedesktop.systemd1.Path.md`
