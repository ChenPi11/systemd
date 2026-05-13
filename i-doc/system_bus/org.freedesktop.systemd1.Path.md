# 接口名称：org.freedesktop.systemd1.Path

## 接口说明

`org.freedesktop.systemd1.Path` 是 systemd 中**路径单元（`.path`）的 D-Bus 接口**。路径单元用于监视文件系统路径的变化，当指定条件满足时（文件出现、文件修改、目录非空等）自动激活关联的服务单元。

路径单元基于 Linux 的 `inotify` 机制实现，是实现**基于文件系统事件的服务激活**的标准方式。

典型用途包括：

- **监控打印队列**：当打印任务出现在队列目录时自动启动打印服务
- **监控配置文件变化**：配置文件修改后自动触发服务重载
- **监控上传目录**：当新文件上传完成后自动触发处理服务

## 总线名称

`org.freedesktop.systemd1`

## 继承关系

```
org.freedesktop.systemd1.Unit
    └── org.freedesktop.systemd1.Path
```

## 对象路径

`/org/freedesktop/systemd1/unit/<path_name>`

其中 `<path_name>` 是路径单元名称经过 D-Bus 路径转义后的形式（例如 `cups.path` 对应 `/org/freedesktop/systemd1/unit/cups_2epath`）。

---

## 属性（Properties）

| 属性名称 | 类型 | 读写 | 说明 |
|---------|------|------|------|
| `Unit` | `s` | 只读 | 路径条件满足时激活的目标单元名称（默认与路径单元同名但扩展名为 `.service`；可通过 `Unit=` 配置覆盖） |
| `Paths` | `a(ss)` | 只读 | 监视路径列表；每项为 `(监视类型, 文件系统路径)` |
| `MakeDirectory` | `b` | 只读 | 为 `true` 时，若被监视的路径不存在，systemd 会自动创建该目录 |
| `DirectoryMode` | `u` | 只读 | `MakeDirectory=yes` 时自动创建目录使用的文件权限（八进制，如 `0755`） |
| `Result` | `s` | 只读 | 路径单元的最终结果：`success` / `resources`（资源不足）/ `trigger-limit-hit`（触发频率超限） |
| `TriggerLimitIntervalUSec` | `t` | 只读 | 触发频率限制的时间窗口（微秒）；在此窗口内触发次数超过 `TriggerLimitBurst` 则停止监控 |
| `TriggerLimitBurst` | `u` | 只读 | 触发频率限制的最大触发次数 |

### Paths 数组中的监视类型说明

`Paths` 属性返回的数组中，每项的监视类型字符串含义如下：

| 监视类型 | 对应配置指令 | 触发条件 |
|---------|------------|---------|
| `PathExists` | `PathExists=` | 指定路径存在时触发；若路径消失后再次出现，则再次触发 |
| `PathExistsGlob` | `PathExistsGlob=` | 匹配 glob 模式的路径存在时触发 |
| `PathChanged` | `PathChanged=` | 指定路径被修改（内容或元数据变化）时触发 |
| `PathModified` | `PathModified=` | 指定路径被写入时触发（比 `PathChanged` 更细粒度，文件被打开写入时即触发） |
| `DirectoryNotEmpty` | `DirectoryNotEmpty=` | 指定目录变为非空时触发 |

---

## 方法（Methods）

路径单元**不定义任何特有方法**。通用控制方法由基础接口 `org.freedesktop.systemd1.Unit` 提供。

---

## 信号（Signals）

路径单元**不定义任何特有信号**。

---

## 使用示例

```bash
# 查询 cups.path 监视的路径列表
busctl get-property org.freedesktop.systemd1 \
    /org/freedesktop/systemd1/unit/cups_2epath \
    org.freedesktop.systemd1.Path Paths

# 查询触发的目标单元
busctl get-property org.freedesktop.systemd1 \
    /org/freedesktop/systemd1/unit/cups_2epath \
    org.freedesktop.systemd1.Path Unit

# 查询路径单元结果
busctl get-property org.freedesktop.systemd1 \
    /org/freedesktop/systemd1/unit/cups_2epath \
    org.freedesktop.systemd1.Path Result

# 查询触发频率限制配置
busctl get-property org.freedesktop.systemd1 \
    /org/freedesktop/systemd1/unit/cups_2epath \
    org.freedesktop.systemd1.Path TriggerLimitBurst

# 列出所有路径单元
systemctl list-units --type=path
```

## 配置示例

```ini
# /etc/systemd/system/process-upload.path
[Unit]
Description=监控上传目录并触发处理

[Path]
DirectoryNotEmpty=/var/spool/upload
MakeDirectory=yes
DirectoryMode=0700
TriggerLimitIntervalSec=10s
TriggerLimitBurst=5

[Install]
WantedBy=multi-user.target
```

对应的服务单元 `process-upload.service`：

```ini
[Unit]
Description=处理上传文件

[Service]
Type=oneshot
ExecStart=/usr/local/bin/process-upload.sh
```

## 参见

- [`org.freedesktop.systemd1.Unit`](org.freedesktop.systemd1.Unit.md) — 所有单元类型的基础接口
- `systemd.path(5)` — 路径单元配置文件参考手册
- `inotify(7)` — Linux 文件系统事件通知机制
