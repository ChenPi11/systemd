# 接口名称：org.freedesktop.systemd1.Manager（用户总线版本）

> **用户总线版本说明：** 本接口通过用户会话总线（user session bus）暴露，由 `systemd --user` 实例提供。与系统总线版本相比，此版本管理的是当前用户的会话服务，而非系统级服务。大多数操作无需 polkit 授权，会话用户对自己的服务拥有完整的管理权限。

## 接口说明

`org.freedesktop.systemd1.Manager` 是 systemd 的核心 D-Bus 管理接口，在用户总线上由 `systemd --user` 实例暴露。通过此接口，客户端可以查询和控制用户会话的 systemd 实例，包括：

- **用户服务管理**：启动、停止、重载、重启用户会话服务
- **作业（Job）管理**：查询和取消正在运行的作业
- **单元文件管理**：启用、禁用、屏蔽、预设用户单元文件
- **环境变量管理**：设置、取消设置用户会话环境变量
- **守护进程控制**：重载用户会话配置、重新执行用户实例

> **注意：** 以下方法在用户总线上**不可用**（仅适用于系统管理器实例）：
> - `Reboot`、`SoftReboot`、`PowerOff`、`Halt`、`KExec`：电源管理操作
> - `SwitchRoot`：仅用于 initrd 环境的根文件系统切换
> - `LookupDynamicUserByName`、`LookupDynamicUserByUID`、`GetDynamicUsers`：动态用户仅用于系统服务

> **与系统总线的主要区别：**
> - 系统总线管理器控制系统服务（PID 1 管理的服务）；用户总线管理器控制用户会话服务（`systemd --user` 管理的服务）
> - 系统总线操作通常需要 polkit 或 `CAP_SYS_BOOT` 授权；用户总线操作通常无需特殊权限（会话用户即拥有访问权）
> - 用户总线的 D-Bus 地址为 `$DBUS_SESSION_BUS_ADDRESS`，或套接字路径 `/run/user/<UID>/bus`

## 用户单元文件搜索路径

`systemd --user` 按以下优先级顺序搜索单元文件：

| 优先级 | 路径 | 说明 |
|--------|------|------|
| 最高 | `$XDG_CONFIG_HOME/systemd/user/`（默认 `~/.config/systemd/user/`） | 用户自定义单元（优先覆盖） |
| 高 | `/run/systemd/user/` | 运行时生成的单元 |
| 中 | `$XDG_DATA_HOME/systemd/user/`（默认 `~/.local/share/systemd/user/`） | 用户本地安装的单元 |
| 低 | `/usr/lib/systemd/user/` | 系统提供给所有用户的单元 |
| 低 | `/usr/local/lib/systemd/user/` | 本地系统管理员安装的用户单元 |
| 最低 | `/lib/systemd/user/` | 系统默认用户单元 |

## 总线连接信息

| 属性 | 值 |
|------|-----|
| 总线名称 | `org.freedesktop.systemd1` |
| 对象路径 | `/org/freedesktop/systemd1` |
| 接口名称 | `org.freedesktop.systemd1.Manager` |
| D-Bus 地址 | `$DBUS_SESSION_BUS_ADDRESS` 或 `/run/user/<UID>/bus` |

## 典型用法

```bash
# 使用 systemctl（推荐方式）
systemctl --user start myapp.service
systemctl --user status myapp.service
systemctl --user enable myapp.service

# 使用 busctl 直接调用 D-Bus 方法
busctl --user call org.freedesktop.systemd1 \
    /org/freedesktop/systemd1 \
    org.freedesktop.systemd1.Manager \
    StartUnit ss myapp.service replace

# 查询用户会话系统状态
busctl --user get-property org.freedesktop.systemd1 \
    /org/freedesktop/systemd1 \
    org.freedesktop.systemd1.Manager \
    SystemState
```

---

## 方法（Methods）

### GetUnit

**说明：** 按名称获取已加载的用户会话单元的 D-Bus 对象路径。若单元未加载，则返回错误。传入空字符串时，返回调用方所属单元的路径。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| name | s | 输入 | 用户单元名称（如 `myapp.service`）。传入空字符串时返回调用方所属单元 |
| unit | o | 输出 | 单元对象路径（如 `/org/freedesktop/systemd1/unit/myapp_2eservice`） |

**权限：** 无需特殊权限（会话用户可直接调用）

**返回：** 单元的 D-Bus 对象路径

---

### GetUnitByPID

**说明：** 按进程 PID 获取该进程所属单元的 D-Bus 对象路径。传入 0 时，返回调用方进程所属的单元。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| pid | u | 输入 | 进程 PID（uint32）。传入 0 时使用调用方 PID |
| unit | o | 输出 | 单元对象路径 |

**权限：** 无需特殊权限

**返回：** 该 PID 所属单元的 D-Bus 对象路径

---

### GetUnitByInvocationID

**说明：** 按调用 ID（invocation ID）查找用户会话单元。每次单元启动时都会分配一个唯一的 128 位 UUID 作为调用 ID。传入全零 ID 时返回调用方所属单元。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| invocation_id | ay | 输入 | 128 位调用 ID（字节数组，16 字节）。全零时返回调用方所属单元 |
| unit | o | 输出 | 单元对象路径（以调用 ID 为基础） |

**权限：** 无需特殊权限

**返回：** 该调用 ID 对应单元的 D-Bus 对象路径

---

### GetUnitByControlGroup

**说明：** 按 cgroup 路径查找所属单元。路径必须是绝对路径且规范化。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| cgroup | s | 输入 | cgroup 绝对路径（如 `/user.slice/user-1000.slice/user@1000.service/myapp.service`） |
| unit | o | 输出 | 单元对象路径 |

**权限：** 无需特殊权限

**返回：** 该 cgroup 路径对应单元的 D-Bus 对象路径

---

### GetUnitByPIDFD

**说明：** 通过 PIDFD（进程文件描述符）查找单元。PIDFD 是 Linux 5.3+ 提供的安全进程引用机制，能避免 PID 重用竞争。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| pidfd | h | 输入 | PIDFD 文件描述符（Unix fd） |
| unit | o | 输出 | 单元对象路径 |
| unit_id | s | 输出 | 单元 ID（名称字符串） |
| invocation_id | ay | 输出 | 当前调用 ID（16 字节字节数组） |

**权限：** 无需特殊权限

**返回：** 单元对象路径、单元 ID 字符串、调用 ID 字节数组

---

### LoadUnit

**说明：** 按名称加载用户会话单元（如果尚未加载则加载），并返回其对象路径。与 `GetUnit` 不同，此方法会触发单元加载。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| name | s | 输入 | 单元名称（如 `myapp.service`） |
| unit | o | 输出 | 单元对象路径 |

**权限：** 无需特殊权限

**返回：** 单元的 D-Bus 对象路径

---

### StartUnit

**说明：** 启动指定的用户会话单元。如果单元尚未加载，会先加载它。等同于 `systemctl --user start`。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| name | s | 输入 | 要启动的单元名称 |
| mode | s | 输入 | 作业模式（见附录） |
| job | o | 输出 | 排队的作业对象路径 |

**权限：** 会话用户可直接调用（无需 polkit）

**返回：** 排队作业的对象路径

**示例：**
```bash
busctl --user call org.freedesktop.systemd1 \
    /org/freedesktop/systemd1 \
    org.freedesktop.systemd1.Manager \
    StartUnit ss myapp.service replace
```

---

### StartUnitWithFlags

**说明：** 与 `StartUnit` 相同，但额外支持通过 `flags` 参数传递启动标志（uint64 位域）。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| name | s | 输入 | 要启动的单元名称 |
| mode | s | 输入 | 作业模式 |
| flags | t | 输入 | 额外标志（uint64，位域） |
| job | o | 输出 | 排队的作业对象路径 |

**权限：** 会话用户可直接调用

**返回：** 排队作业的对象路径

---

### StartUnitReplace

**说明：** 替换方式启动单元。要求旧单元当前有一个 start 类型的作业正在等待，然后用新单元替换它。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| old_unit | s | 输入 | 当前有等待 start 作业的旧单元名称 |
| new_unit | s | 输入 | 要启动的新单元名称 |
| mode | s | 输入 | 作业模式 |
| job | o | 输出 | 排队的作业对象路径 |

**权限：** 会话用户可直接调用

**返回：** 排队作业的对象路径

---

### StopUnit

**说明：** 停止指定的用户会话单元，等同于 `systemctl --user stop`。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| name | s | 输入 | 要停止的单元名称 |
| mode | s | 输入 | 作业模式（通常使用 `replace`） |
| job | o | 输出 | 排队的作业对象路径 |

**权限：** 会话用户可直接调用

**返回：** 排队作业的对象路径

---

### ReloadUnit

**说明：** 重载指定用户会话单元的配置（不重启进程），等同于 `systemctl --user reload`。仅对支持重载的单元有效（服务必须定义 `ExecReload=`）。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| name | s | 输入 | 要重载的单元名称 |
| mode | s | 输入 | 作业模式 |
| job | o | 输出 | 排队的作业对象路径 |

**权限：** 会话用户可直接调用

**返回：** 排队作业的对象路径

---

### RestartUnit

**说明：** 重启指定用户会话单元（先停止再启动），等同于 `systemctl --user restart`。即使单元当前未运行也会启动它。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| name | s | 输入 | 要重启的单元名称 |
| mode | s | 输入 | 作业模式 |
| job | o | 输出 | 排队的作业对象路径 |

**权限：** 会话用户可直接调用

**返回：** 排队作业的对象路径

---

### TryRestartUnit

**说明：** 尝试重启单元，但仅当单元当前处于运行状态时才重启，等同于 `systemctl --user try-restart`。若单元未运行则忽略。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| name | s | 输入 | 要尝试重启的单元名称 |
| mode | s | 输入 | 作业模式 |
| job | o | 输出 | 排队的作业对象路径 |

**权限：** 会话用户可直接调用

**返回：** 排队作业的对象路径

---

### ReloadOrRestartUnit

**说明：** 如果单元支持重载则重载，否则重启，等同于 `systemctl --user reload-or-restart`。若单元未运行则启动它。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| name | s | 输入 | 单元名称 |
| mode | s | 输入 | 作业模式 |
| job | o | 输出 | 排队的作业对象路径 |

**权限：** 会话用户可直接调用

**返回：** 排队作业的对象路径

---

### ReloadOrTryRestartUnit

**说明：** 如果单元支持重载则重载，否则尝试重启（仅当运行时），等同于 `systemctl --user reload-or-try-restart`。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| name | s | 输入 | 单元名称 |
| mode | s | 输入 | 作业模式 |
| job | o | 输出 | 排队的作业对象路径 |

**权限：** 会话用户可直接调用

**返回：** 排队作业的对象路径

---

### EnqueueUnitJob

**说明：** 向单元的作业队列中添加一个作业，并返回详细的作业信息，包括受影响的相关作业列表。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| name | s | 输入 | 单元名称 |
| job_type | s | 输入 | 作业类型（`start`、`stop`、`reload`、`restart`、`try-restart` 等） |
| job_mode | s | 输入 | 作业模式（同 StartUnit 的 mode） |
| job_id | u | 输出 | 主作业 ID（uint32） |
| job_path | o | 输出 | 主作业对象路径 |
| unit_id | s | 输出 | 单元 ID |
| unit_path | o | 输出 | 单元对象路径 |
| job_type | s | 输出 | 实际排队的作业类型 |
| affected_jobs | a(uosos) | 输出 | 受影响的关联作业数组 |

**权限：** 会话用户可直接调用

**返回：** 主作业信息及受影响的关联作业列表

---

### KillUnit

**说明：** 向用户会话单元进程组（或部分进程）发送指定信号，等同于 `systemctl --user kill`。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| name | s | 输入 | 单元名称 |
| whom | s | 输入 | 发送目标：`main`（主进程）、`control`（控制进程）、`all`（全部进程） |
| signal | i | 输入 | 信号编号（int32，如 15 为 SIGTERM，9 为 SIGKILL） |

**权限：** 会话用户可直接调用

**返回：** 无

---

### KillUnitSubgroup

**说明：** 向单元的指定 cgroup 子组中的进程发送信号。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| name | s | 输入 | 单元名称 |
| whom | s | 输入 | 发送目标（同 KillUnit） |
| subgroup | s | 输入 | cgroup 子组路径 |
| signal | i | 输入 | 信号编号 |

**权限：** 会话用户可直接调用

**返回：** 无

---

### QueueSignalUnit

**说明：** 向单元进程发送实时信号，并携带附加值（sigqueue 语义）。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| name | s | 输入 | 单元名称 |
| whom | s | 输入 | 发送目标（`main`、`control`、`all`） |
| signal | i | 输入 | 信号编号 |
| value | i | 输入 | 随信号发送的附加整数值 |

**权限：** 会话用户可直接调用

**返回：** 无

---

### CleanUnit

**说明：** 清理单元遗留的运行时文件、日志、状态等资源，等同于 `systemctl --user clean`。单元必须已加载且处于非活动状态。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| name | s | 输入 | 单元名称 |
| mask | as | 输入 | 要清理的资源类型列表，如 `["runtime", "state", "cache", "logs", "fdstore"]` |

**权限：** 会话用户可直接调用

**返回：** 无

---

### FreezeUnit

**说明：** 冻结单元（暂停 cgroup 中所有进程）。单元必须处于活动（running）状态且已正确加载。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| name | s | 输入 | 要冻结的单元名称 |

**权限：** 会话用户可直接调用

**返回：** 无

---

### ThawUnit

**说明：** 解冻单元（恢复之前被冻结的单元中的进程），与 `FreezeUnit` 相对。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| name | s | 输入 | 要解冻的单元名称 |

**权限：** 会话用户可直接调用

**返回：** 无

---

### ResetFailedUnit

**说明：** 重置指定单元的失败状态，等同于 `systemctl --user reset-failed <unit>`。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| name | s | 输入 | 要重置失败状态的单元名称 |

**权限：** 会话用户可直接调用

**返回：** 无

---

### SetUnitProperties

**说明：** 动态修改用户会话单元的属性（在运行时或永久修改），等同于 `systemctl --user set-property`。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| name | s | 输入 | 单元名称 |
| runtime | b | 输入 | 是否仅运行时生效（true = 重启后恢复，false = 永久保存到 `~/.config/systemd/user/`） |
| properties | a(sv) | 输入 | 要设置的属性数组，每项为 (属性名, 属性值variant) |

**权限：** 会话用户可直接调用

**返回：** 无

---

### BindMountUnit

**说明：** 向指定的用户会话 service 单元运行时命名空间中添加绑定挂载。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| name | s | 输入 | service 单元名称 |
| source | s | 输入 | 宿主机上的源路径 |
| destination | s | 输入 | 单元命名空间内的目标挂载点 |
| read_only | b | 输入 | 是否只读挂载 |
| mkdir | b | 输入 | 若目标不存在是否自动创建目录 |

**权限：** 会话用户可直接调用

**返回：** 无

---

### MountImageUnit

**说明：** 向指定的用户会话 service 单元运行时命名空间中挂载一个磁盘镜像文件。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| name | s | 输入 | service 单元名称 |
| source | s | 输入 | 镜像文件路径 |
| destination | s | 输入 | 单元命名空间内的目标挂载点 |
| read_only | b | 输入 | 是否只读挂载 |
| mkdir | b | 输入 | 若目标不存在是否自动创建目录 |
| options | a(ss) | 输入 | 挂载选项数组，每项为 (分区, 选项字符串) |

**权限：** 会话用户可直接调用

**返回：** 无

---

### RefUnit

**说明：** 增加单元的引用计数。可防止单元因无人使用而被自动卸载或停止。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| name | s | 输入 | 单元名称 |

**权限：** 会话用户可直接调用

**返回：** 无

---

### UnrefUnit

**说明：** 减少单元的引用计数（与 `RefUnit` 相对）。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| name | s | 输入 | 单元名称 |

**权限：** 会话用户可直接调用

**返回：** 无

---

### StartTransientUnit

**说明：** 创建并启动一个临时用户会话单元（transient unit）。临时单元在停止后自动删除，不需要持久的单元文件。常用于 `systemd-run --user` 创建临时服务/作用域。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| name | s | 输入 | 临时单元名称（必须包含合法的单元类型后缀） |
| mode | s | 输入 | 作业模式 |
| properties | a(sv) | 输入 | 主单元属性数组 |
| aux | a(sa(sv)) | 输入 | 辅助单元数组，每项为 (单元名, 属性数组) |
| job | o | 输出 | 排队的作业对象路径 |

**权限：** 会话用户可直接调用

**返回：** 排队作业的对象路径

**示例：**
```bash
# 创建临时用户服务
systemd-run --user --unit=mytemp.service /usr/bin/myapp
```

---

### GetUnitProcesses

**说明：** 获取指定用户会话单元 cgroup 中所有进程的信息列表。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| name | s | 输入 | 单元名称 |
| processes | a(sus) | 输出 | 进程列表，每项为 (cgroup路径, PID, 进程命令行) |

**权限：** 无需特殊权限

**返回：** 进程信息数组

---

### AttachProcessesToUnit

**说明：** 将指定 PID 列表中的进程附加到用户会话单元的 cgroup 中（可选指定子组）。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| unit_name | s | 输入 | 目标单元名称 |
| subcgroup | s | 输入 | 子 cgroup 路径（空字符串表示根 cgroup） |
| pids | au | 输入 | 要附加的进程 PID 数组（uint32 数组） |

**权限：** 会话用户可直接调用

**返回：** 无

---

### RemoveSubgroupFromUnit

**说明：** 从单元中移除指定的 cgroup 子组。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| unit_name | s | 输入 | 单元名称 |
| subcgroup | s | 输入 | 要移除的子 cgroup 路径 |
| flags | t | 输入 | 标志（uint64，当前传 0） |

**权限：** 会话用户可直接调用

**返回：** 无

---

### AbandonScope

**说明：** 放弃（abandon）一个 scope 类型的用户会话单元。仅适用于 scope 单元。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| name | s | 输入 | scope 单元名称（必须是 `.scope` 后缀） |

**权限：** 会话用户可直接调用

**返回：** 无

---

### GetJob

**说明：** 按作业 ID 获取用户会话中正在运行的作业的 D-Bus 对象路径。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| id | u | 输入 | 作业 ID（uint32） |
| job | o | 输出 | 作业对象路径 |

**权限：** 无需特殊权限

**返回：** 作业对象路径

---

### GetJobAfter

**说明：** 获取在指定作业之后排队等待的所有作业。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| id | u | 输入 | 作业 ID（uint32） |
| jobs | a(usssoo) | 输出 | 作业信息数组 |

**权限：** 无需特殊权限

**返回：** 相关后续作业的信息数组

---

### GetJobBefore

**说明：** 获取在指定作业之前排队等待的所有作业。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| id | u | 输入 | 作业 ID（uint32） |
| jobs | a(usssoo) | 输出 | 作业信息数组 |

**权限：** 无需特殊权限

**返回：** 此作业所等待的前置作业信息数组

---

### CancelJob

**说明：** 取消用户会话中指定 ID 的作业，等同于 `systemctl --user cancel`。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| id | u | 输入 | 要取消的作业 ID（uint32） |

**权限：** 会话用户可直接调用

**返回：** 无

---

### ClearJobs

**说明：** 清除所有当前排队中的用户会话作业。

**参数：** 无

**权限：** 会话用户可直接调用

**返回：** 无

---

### ResetFailed

**说明：** 重置所有用户会话单元的失败状态，等同于 `systemctl --user reset-failed`（不带参数）。

**参数：** 无

**权限：** 会话用户可直接调用

**返回：** 无

---

### SetShowStatus

**说明：** 设置 systemd --user 在控制台上显示状态的模式。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| mode | s | 输入 | 显示模式：`yes`、`no`、`auto`、`error`、`temporary-failure`，空字符串恢复默认 |

**权限：** 会话用户可直接调用

**返回：** 无

---

### ListUnits

**说明：** 列出用户会话中所有已加载的单元信息，等同于 `systemctl --user list-units`。

**参数：** 无

**权限：** 无需特殊权限

**返回：** `a(ssssssouso)` — 单元信息数组，每项包含：
| 序号 | 类型 | 说明 |
|------|------|------|
| 1 | s | 单元主 ID（名称） |
| 2 | s | 单元描述 |
| 3 | s | 加载状态（`loaded`、`not-found`、`masked` 等） |
| 4 | s | 活动状态（`active`、`inactive`、`failed`、`activating` 等） |
| 5 | s | 子状态（`running`、`dead`、`mounted` 等） |
| 6 | s | 若此单元跟随其他单元，则为被跟随单元名 |
| 7 | o | 单元对象路径 |
| 8 | u | 当前排队作业 ID（无作业则为 0） |
| 9 | s | 当前排队作业类型（无则为空字符串） |
| 10 | o | 当前排队作业对象路径（无则为 `/`） |

---

### ListUnitsFiltered

**说明：** 按状态过滤列出用户会话单元。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| states | as | 输入 | 状态过滤列表（如 `["active", "failed"]`），空列表返回所有 |
| units | a(ssssssouso) | 输出 | 单元信息数组 |

**权限：** 无需特殊权限

**返回：** 过滤后的单元信息数组

---

### ListUnitsByPatterns

**说明：** 同时按状态和名称模式过滤列出用户会话单元。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| states | as | 输入 | 状态过滤列表 |
| patterns | as | 输入 | 名称模式列表（支持 glob 通配符） |
| units | a(ssssssouso) | 输出 | 单元信息数组 |

**权限：** 无需特殊权限

**返回：** 过滤后的单元信息数组

---

### ListUnitsByNames

**说明：** 按单元名称列表获取用户会话单元信息。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| names | as | 输入 | 单元名称列表 |
| units | a(ssssssouso) | 输出 | 单元信息数组 |

**权限：** 无需特殊权限

**返回：** 请求单元的信息数组

---

### ListJobs

**说明：** 列出用户会话中所有当前排队或运行中的作业，等同于 `systemctl --user list-jobs`。

**参数：** 无

**权限：** 无需特殊权限

**返回：** `a(usssoo)` — 作业信息数组，每项包含：
| 序号 | 类型 | 说明 |
|------|------|------|
| 1 | u | 作业 ID |
| 2 | s | 单元名 |
| 3 | s | 作业类型（`start`、`stop`、`reload`、`restart` 等） |
| 4 | s | 作业状态（`waiting`、`running`） |
| 5 | o | 作业对象路径 |
| 6 | o | 单元对象路径 |

---

### Subscribe

**说明：** 订阅 systemd --user 事件（信号）。调用后，此连接将接收用户会话 systemd 发出的所有信号（UnitNew、UnitRemoved、JobNew、JobRemoved 等）。

**参数：** 无

**权限：** 无需特殊权限

**返回：** 无

---

### Unsubscribe

**说明：** 取消之前通过 `Subscribe()` 建立的用户会话事件订阅。

**参数：** 无

**权限：** 无需特殊权限

**返回：** 无

---

### Dump

**说明：** 返回用户会话 systemd 当前状态的完整文本转储，等同于 `systemctl --user dump`。

**参数：** 无

**权限：** 无需特殊权限

**返回：**
| 名称 | 类型 | 说明 |
|------|------|------|
| output | s | 状态转储文本 |

---

### DumpUnitsMatchingPatterns

**说明：** 返回用户会话中匹配指定名称模式的单元的状态转储。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| patterns | as | 输入 | 单元名称模式列表（支持 glob） |
| output | s | 输出 | 转储文本 |

**权限：** 无需特殊权限

**返回：** 匹配单元的状态转储文本

---

### DumpByFileDescriptor

**说明：** 与 `Dump` 功能相同，但通过文件描述符（memfd）返回结果，适合处理大量数据。

**参数：** 无

**权限：** 无需特殊权限

**返回：**
| 名称 | 类型 | 说明 |
|------|------|------|
| fd | h | 包含状态转储内容的内存文件描述符 |

---

### DumpUnitsMatchingPatternsByFileDescriptor

**说明：** 与 `DumpUnitsMatchingPatterns` 功能相同，但通过文件描述符返回结果。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| patterns | as | 输入 | 单元名称模式列表 |
| fd | h | 输出 | 包含转储内容的内存文件描述符 |

**权限：** 无需特殊权限

**返回：** 包含转储内容的文件描述符

---

### Reload

**说明：** 重新加载用户会话 systemd 的配置文件（daemon-reload），等同于 `systemctl --user daemon-reload`。重载完成后才返回响应。

**参数：** 无

**权限：** 会话用户可直接调用（无需 polkit）

**返回：** 无（重载完成后返回）

---

### Reexecute

**说明：** 使用户会话 systemd 重新执行自身（daemon-reexec），等同于 `systemctl --user daemon-reexec`。此方法不发送回复，调用方应等待 D-Bus 连接断开然后重连。

**参数：** 无

**权限：** 会话用户可直接调用

**返回：** 无（不发送回复）

---

### Exit

**说明：** 使用户会话 systemd 退出，结束用户会话。调用此方法会导致 `systemd --user` 实例退出，所有用户服务将被停止。

**参数：** 无

**权限：** 会话用户可直接调用

**返回：** 无

---

> **注意：** 以下方法在用户总线上**不存在**：
> - `Reboot`：系统重启（仅系统管理器）
> - `SoftReboot`：软重启（仅系统管理器）
> - `PowerOff`：关闭电源（仅系统管理器）
> - `Halt`：停机（仅系统管理器）
> - `KExec`：kexec 重启（仅系统管理器）
> - `SwitchRoot`：切换根文件系统（仅系统管理器/initrd）

---

### SetEnvironment

**说明：** 向用户会话 systemd 的全局环境变量集合中添加环境变量，等同于 `systemctl --user set-environment`。这些变量会被之后启动的所有用户服务继承。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| assignments | as | 输入 | 环境变量赋值数组（格式 `KEY=VALUE`） |

**权限：** 会话用户可直接调用

**返回：** 无

---

### UnsetEnvironment

**说明：** 从用户会话 systemd 的全局环境变量集合中删除指定变量，等同于 `systemctl --user unset-environment`。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| names | as | 输入 | 要删除的环境变量名称数组（仅变量名，不含 `=` 和值） |

**权限：** 会话用户可直接调用

**返回：** 无

---

### UnsetAndSetEnvironment

**说明：** 原子地执行"先删除后设置"用户会话环境变量操作。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| names | as | 输入 | 要删除的环境变量名称数组 |
| assignments | as | 输入 | 要设置的环境变量赋值数组（格式 `KEY=VALUE`） |

**权限：** 会话用户可直接调用

**返回：** 无

---

### EnqueueMarkedJobs

**说明：** 将所有已标记为需要重启/重载/启动/停止的用户会话单元的对应作业加入队列。

**参数：** 无

**权限：** 会话用户可直接调用

**返回：**
| 名称 | 类型 | 说明 |
|------|------|------|
| jobs | ao | 成功排队的作业对象路径数组 |

---

### ListUnitFiles

**说明：** 列出用户会话中所有已知的单元文件及其状态，等同于 `systemctl --user list-unit-files`。包含 `~/.config/systemd/user/`、`/usr/lib/systemd/user/` 等路径下的单元文件。

**参数：** 无

**权限：** 无需特殊权限

**返回：** `a(ss)` — 单元文件信息数组，每项包含：
| 序号 | 类型 | 说明 |
|------|------|------|
| 1 | s | 单元文件路径 |
| 2 | s | 启用状态（`enabled`、`disabled`、`masked`、`static`、`alias` 等） |

---

### ListUnitFilesByPatterns

**说明：** 按启用状态和名称模式过滤列出用户会话单元文件。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| states | as | 输入 | 启用状态过滤列表 |
| patterns | as | 输入 | 名称模式列表（支持 glob） |
| unit_files | a(ss) | 输出 | 单元文件信息数组 |

**权限：** 无需特殊权限

**返回：** 过滤后的单元文件信息数组

---

### GetUnitFileState

**说明：** 获取指定用户会话单元文件的启用状态，等同于 `systemctl --user is-enabled`。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| file | s | 输入 | 单元名称或单元文件路径 |
| state | s | 输出 | 启用状态字符串 |

**权限：** 无需特殊权限

**返回：** 单元文件的启用状态字符串

---

### EnableUnitFiles

**说明：** 启用指定的用户会话单元文件（在 `~/.config/systemd/user/` 中创建符号链接），等同于 `systemctl --user enable`。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| files | as | 输入 | 单元名称或绝对路径数组 |
| runtime | b | 输入 | 是否仅运行时启用（true = 写入 `/run/systemd/user/`，false = 写入 `~/.config/systemd/user/`） |
| force | b | 输入 | 是否覆盖已有的符号链接 |
| carries_install_info | b | 输出 | 单元文件是否包含 `[Install]` 节信息 |
| changes | a(sss) | 输出 | 执行的变更列表，每项为 (变更类型, 目标路径, 源路径) |

**权限：** 会话用户可直接调用（无需 polkit）

**返回：** 是否包含安装信息 + 变更列表

---

### DisableUnitFiles

**说明：** 禁用指定的用户会话单元文件（删除符号链接），等同于 `systemctl --user disable`。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| files | as | 输入 | 单元名称或绝对路径数组 |
| runtime | b | 输入 | 是否仅禁用运行时启用 |
| changes | a(sss) | 输出 | 执行的变更列表 |

**权限：** 会话用户可直接调用

**返回：** 变更列表

---

### EnableUnitFilesWithFlags

**说明：** 与 `EnableUnitFiles` 功能相同，但使用单个 `flags` 参数（uint64 位域）。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| files | as | 输入 | 单元名称或绝对路径数组 |
| flags | t | 输入 | 标志位域（`UNIT_FILE_RUNTIME=1`、`UNIT_FILE_FORCE=2`、`UNIT_FILE_PORTABLE=4` 等） |
| carries_install_info | b | 输出 | 是否包含安装信息 |
| changes | a(sss) | 输出 | 变更列表 |

**权限：** 会话用户可直接调用

**返回：** 是否包含安装信息 + 变更列表

---

### DisableUnitFilesWithFlags

**说明：** 与 `DisableUnitFiles` 功能相同，但使用 `flags` 参数。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| files | as | 输入 | 单元名称或绝对路径数组 |
| flags | t | 输入 | 标志位域 |
| changes | a(sss) | 输出 | 变更列表 |

**权限：** 会话用户可直接调用

**返回：** 变更列表

---

### DisableUnitFilesWithFlagsAndInstallInfo

**说明：** 与 `DisableUnitFilesWithFlags` 相同，但额外返回 `carries_install_info` 字段。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| files | as | 输入 | 单元名称或绝对路径数组 |
| flags | t | 输入 | 标志位域 |
| carries_install_info | b | 输出 | 是否包含安装信息 |
| changes | a(sss) | 输出 | 变更列表 |

**权限：** 会话用户可直接调用

**返回：** 是否包含安装信息 + 变更列表

---

### ReenableUnitFiles

**说明：** 重新启用用户会话单元文件（先禁用再启用），等同于 `systemctl --user reenable`。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| files | as | 输入 | 单元名称或绝对路径数组 |
| runtime | b | 输入 | 是否仅运行时操作 |
| force | b | 输入 | 是否覆盖已有符号链接 |
| carries_install_info | b | 输出 | 是否包含安装信息 |
| changes | a(sss) | 输出 | 变更列表 |

**权限：** 会话用户可直接调用

**返回：** 是否包含安装信息 + 变更列表

---

### LinkUnitFiles

**说明：** 将指定路径的单元文件链接到用户单元目录中，等同于 `systemctl --user link`。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| files | as | 输入 | 单元文件绝对路径数组 |
| runtime | b | 输入 | 是否仅运行时链接 |
| force | b | 输入 | 是否覆盖已有链接 |
| changes | a(sss) | 输出 | 变更列表 |

**权限：** 会话用户可直接调用

**返回：** 变更列表

---

### PresetUnitFiles

**说明：** 根据用户预设策略（`~/.config/systemd/user-preset/` 或 `/usr/lib/systemd/user-preset/`）启用或禁用单元文件，等同于 `systemctl --user preset`。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| files | as | 输入 | 单元名称或路径数组 |
| runtime | b | 输入 | 是否仅运行时操作 |
| force | b | 输入 | 是否覆盖已有符号链接 |
| carries_install_info | b | 输出 | 是否包含安装信息 |
| changes | a(sss) | 输出 | 变更列表 |

**权限：** 会话用户可直接调用

**返回：** 是否包含安装信息 + 变更列表

---

### PresetUnitFilesWithMode

**说明：** 与 `PresetUnitFiles` 相同，但允许指定预设模式。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| files | as | 输入 | 单元名称或路径数组 |
| mode | s | 输入 | 预设模式：`full`、`enable-only`、`disable-only`，空字符串为 `full` |
| runtime | b | 输入 | 是否仅运行时操作 |
| force | b | 输入 | 是否覆盖已有符号链接 |
| carries_install_info | b | 输出 | 是否包含安装信息 |
| changes | a(sss) | 输出 | 变更列表 |

**权限：** 会话用户可直接调用

**返回：** 是否包含安装信息 + 变更列表

---

### MaskUnitFiles

**说明：** 屏蔽指定用户会话单元文件（将其符号链接到 `/dev/null`），等同于 `systemctl --user mask`。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| files | as | 输入 | 单元名称或路径数组 |
| runtime | b | 输入 | 是否仅运行时屏蔽 |
| force | b | 输入 | 是否强制屏蔽（即使单元正在运行） |
| changes | a(sss) | 输出 | 变更列表 |

**权限：** 会话用户可直接调用

**返回：** 变更列表

---

### UnmaskUnitFiles

**说明：** 取消屏蔽指定用户会话单元文件，等同于 `systemctl --user unmask`。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| files | as | 输入 | 单元名称或路径数组 |
| runtime | b | 输入 | 是否仅取消运行时屏蔽 |
| changes | a(sss) | 输出 | 变更列表 |

**权限：** 会话用户可直接调用

**返回：** 变更列表

---

### RevertUnitFiles

**说明：** 还原用户会话单元文件的本地修改（删除 `~/.config/systemd/user/` 中的覆盖文件），等同于 `systemctl --user revert`。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| files | as | 输入 | 单元名称或路径数组 |
| changes | a(sss) | 输出 | 变更列表 |

**权限：** 会话用户可直接调用

**返回：** 变更列表

---

### SetDefaultTarget

**说明：** 设置用户会话默认启动 target（修改 `default.target` 符号链接），等同于 `systemctl --user set-default`。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| name | s | 输入 | target 单元名称 |
| force | b | 输入 | 是否强制覆盖现有默认 target |
| changes | a(sss) | 输出 | 变更列表 |

**权限：** 会话用户可直接调用

**返回：** 变更列表

---

### GetDefaultTarget

**说明：** 获取当前用户会话默认启动 target，等同于 `systemctl --user get-default`。

**参数：** 无

**权限：** 无需特殊权限

**返回：**
| 名称 | 类型 | 说明 |
|------|------|------|
| name | s | 默认 target 名称 |

---

### PresetAllUnitFiles

**说明：** 对所有已安装的用户会话单元文件应用预设策略，等同于 `systemctl --user preset-all`。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| mode | s | 输入 | 预设模式（`full`、`enable-only`、`disable-only`，空字符串为 `full`） |
| runtime | b | 输入 | 是否仅运行时操作 |
| force | b | 输入 | 是否覆盖已有链接 |
| changes | a(sss) | 输出 | 变更列表 |

**权限：** 会话用户可直接调用

**返回：** 变更列表

---

### AddDependencyUnitFiles

**说明：** 向指定用户会话单元文件添加依赖关系，等同于 `systemctl --user add-wants` 或 `systemctl --user add-requires`。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| files | as | 输入 | 要添加依赖的单元名称或路径数组 |
| target | s | 输入 | 依赖目标（哪个 target 对这些单元产生依赖） |
| type | s | 输入 | 依赖类型：`Wants` 或 `Requires` |
| runtime | b | 输入 | 是否仅运行时操作 |
| force | b | 输入 | 是否覆盖已有链接 |
| changes | a(sss) | 输出 | 变更列表 |

**权限：** 会话用户可直接调用

**返回：** 变更列表

---

### GetUnitFileLinks

**说明：** 获取禁用用户会话单元时会删除的符号链接列表（dry-run 模式）。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| name | s | 输入 | 单元名称 |
| runtime | b | 输入 | 是否查询运行时链接（true）还是持久链接（false） |
| links | as | 输出 | 会被删除的符号链接路径列表 |

**权限：** 无需特殊权限

**返回：** 符号链接路径字符串数组

---

### SetExitCode

**说明：** 设置用户会话 systemd 的退出码（当 `systemd --user` 实例退出时使用的返回值）。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| number | y | 输入 | 退出码（byte，0-255） |

**权限：** 会话用户可直接调用

**返回：** 无

---

### DumpUnitFileDescriptorStore

**说明：** 转储指定用户会话 service 单元的文件描述符存储（fd store）内容。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| name | s | 输入 | service 单元名称 |
| entries | a(suuutuusu) | 输出 | fd store 条目数组 |

**权限：** 无需特殊权限

**返回：** fd store 条目数组

---

## 属性（Properties）

### 系统信息属性

| 属性 | 类型 | 读写 | 说明 |
|------|------|------|------|
| Version | s | 只读 | systemd 版本号字符串（如 `255`） |
| Features | s | 只读 | 编译时启用的功能特性列表 |
| Virtualization | s | 只读 | 检测到的虚拟化类型 |
| ConfidentialVirtualization | s | 只读 | 检测到的机密虚拟化类型 |
| Architecture | s | 只读 | 系统架构（如 `x86-64`、`aarch64`） |
| Tainted | s | 只读 | 系统污点标记字符串 |

### 启动时间戳属性

每个时间戳属性实际展开为两个属性：`XxxTimestamp`（UTC 实时时钟，类型 `t`）和 `XxxTimestampMonotonic`（单调时钟，类型 `t`），单位均为微秒。

| 属性（基础名称） | 读写 | 说明 |
|------|------|------|
| UserspaceTimestamp | 只读 | 用户会话 systemd 启动时间点 |
| FinishTimestamp | 只读 | 用户会话启动完成时间点（default.target 激活后） |
| GeneratorsStartTimestamp | 只读 | 单元生成器（generators）开始运行时间点 |
| GeneratorsFinishTimestamp | 只读 | 单元生成器完成时间点 |
| UnitsLoadStartTimestamp | 只读 | 单元加载开始时间点 |
| UnitsLoadFinishTimestamp | 只读 | 单元加载完成时间点 |

> **注意：** 与系统管理器不同，用户会话管理器没有 `FirmwareTimestamp`、`LoaderTimestamp`、`KernelTimestamp`、`InitRDTimestamp` 等与系统引导相关的时间戳属性。

### 日志控制属性

| 属性 | 类型 | 读写 | 说明 |
|------|------|------|------|
| LogLevel | s | 读写 | 用户会话 systemd 的日志级别（如 `info`、`debug`、`warning`） |
| LogTarget | s | 读写 | 用户会话 systemd 的日志目标（如 `journal`、`console`、`null`） |

### 统计属性

| 属性 | 类型 | 读写 | 说明 |
|------|------|------|------|
| NNames | u | 只读 | 当前加载的用户单元（含别名）总数 |
| NFailedUnits | u | 只读 | 当前处于失败状态的用户单元数 |
| NJobs | u | 只读 | 当前排队的作业总数 |
| NInstalledJobs | u | 只读 | 历史上安装的作业总数（累计） |
| NFailedJobs | u | 只读 | 历史上失败的作业总数（累计） |
| Progress | d | 只读 | 用户会话启动进度（0.0 到 1.0 之间的浮点数） |

### 环境与配置属性

| 属性 | 类型 | 读写 | 说明 |
|------|------|------|------|
| Environment | as | 只读 | 用户会话 systemd 当前的有效环境变量列表（继承自登录会话） |
| ConfirmSpawn | b | 只读 | 是否在启动每个服务前要求确认 |
| ShowStatus | b | 只读 | 当前是否显示启动/停止状态 |
| UnitPath | as | 只读 | 用户会话 systemd 搜索单元文件的路径列表（包含 `~/.config/systemd/user/`、`/usr/lib/systemd/user/` 等） |
| DefaultStandardOutput | s | 只读 | 用户单元的默认标准输出目标（如 `journal`） |
| DefaultStandardError | s | 只读 | 用户单元的默认标准错误输出目标 |

### 系统状态属性

| 属性 | 类型 | 读写 | 说明 |
|------|------|------|------|
| ControlGroup | s | 只读 | 用户会话 systemd 的 cgroup 路径（如 `/user.slice/user-1000.slice/user@1000.service/`） |
| SystemState | s | 只读 | 用户会话当前状态（`initializing`、`starting`、`running`、`degraded`、`maintenance`、`stopping`、`offline`、`unknown`）**注意：此处反映的是用户会话状态，而非整个系统状态** |
| ExitCode | y | 只读 | 当前设置的退出码（byte，通过 SetExitCode 设置） |

### 默认超时属性

| 属性 | 类型 | 读写 | 说明 |
|------|------|------|------|
| DefaultTimerAccuracyUSec | t | 只读 | 默认定时器精度（微秒） |
| DefaultTimeoutStartUSec | t | 只读 | 用户服务启动默认超时（微秒） |
| DefaultTimeoutStopUSec | t | 只读 | 用户服务停止默认超时（微秒） |
| DefaultTimeoutAbortUSec | t | 只读 | 用户服务中止默认超时（微秒） |
| DefaultDeviceTimeoutUSec | t | 只读 | 设备单元默认超时（微秒） |
| DefaultRestartUSec | t | 只读 | 用户服务重启间隔默认值（微秒） |
| DefaultStartLimitIntervalUSec | t | 只读 | 启动频率限制的时间窗口（微秒） |
| DefaultStartLimitBurst | u | 只读 | 启动频率限制的最大次数 |

### 默认资源限制属性（RLIMIT）

以下属性均为 `t` 类型（uint64），只读。每种资源限制有两个属性：硬限制（无 `Soft` 后缀）和软限制（带 `Soft` 后缀）。

| 属性 | 说明 |
|------|------|
| DefaultLimitCPU / DefaultLimitCPUSoft | CPU 时间限制（秒） |
| DefaultLimitFSIZE / DefaultLimitFSIZESoft | 文件大小限制（字节） |
| DefaultLimitDATA / DefaultLimitDATASoft | 数据段大小限制（字节） |
| DefaultLimitSTACK / DefaultLimitSTACKSoft | 栈大小限制（字节） |
| DefaultLimitCORE / DefaultLimitCORESoft | core dump 文件大小限制（字节） |
| DefaultLimitRSS / DefaultLimitRSSSoft | 常驻内存集大小限制（字节） |
| DefaultLimitNOFILE / DefaultLimitNOFILESoft | 打开文件描述符数量限制 |
| DefaultLimitAS / DefaultLimitASSoft | 地址空间大小限制（字节） |
| DefaultLimitNPROC / DefaultLimitNPROCSoft | 进程数量限制 |
| DefaultLimitMEMLOCK / DefaultLimitMEMLOCKSoft | 可锁定内存大小限制（字节） |
| DefaultLimitLOCKS / DefaultLimitLOCKSSoft | 文件锁数量限制 |
| DefaultLimitSIGPENDING / DefaultLimitSIGPENDINGSoft | 挂起信号数量限制 |
| DefaultLimitMSGQUEUE / DefaultLimitMSGQUEUESoft | POSIX 消息队列大小限制（字节） |
| DefaultLimitNICE / DefaultLimitNICESoft | nice 优先级上限 |
| DefaultLimitRTPRIO / DefaultLimitRTPRIOSoft | 实时调度优先级上限 |
| DefaultLimitRTTIME / DefaultLimitRTTIMESoft | 实时调度 CPU 时间限制（微秒） |

### 默认计账与策略属性

| 属性 | 类型 | 读写 | 说明 |
|------|------|------|------|
| DefaultIOAccounting | b | 只读 | 是否默认启用 IO 计账 |
| DefaultIPAccounting | b | 只读 | 是否默认启用 IP 流量计账 |
| DefaultMemoryAccounting | b | 只读 | 是否默认启用内存计账 |
| DefaultTasksAccounting | b | 只读 | 是否默认启用任务计账 |
| DefaultTasksMax | t | 只读 | 默认最大任务数 |
| DefaultOOMPolicy | s | 只读 | OOM 杀手默认策略 |
| DefaultOOMScoreAdjust | i | 只读 | 默认 OOM 评分调整值（int32） |
| DefaultRestrictSUIDSGID | b | 只读 | 是否默认限制 SUID/SGID 位 |

---

## 信号（Signals）

### UnitNew

**说明：** 当新的用户会话单元被加载到 systemd --user 中时发出此信号。

**参数：**
| 名称 | 类型 | 说明 |
|------|------|------|
| id | s | 新单元的 ID（名称，如 `myapp.service`） |
| unit | o | 新单元的 D-Bus 对象路径 |

---

### UnitRemoved

**说明：** 当用户会话单元从 systemd --user 中卸载时发出此信号。

**参数：**
| 名称 | 类型 | 说明 |
|------|------|------|
| id | s | 被移除的单元 ID |
| unit | o | 被移除单元的 D-Bus 对象路径（路径在信号发出后可能已失效） |

---

### JobNew

**说明：** 当新作业被加入用户会话队列时发出此信号。

**参数：**
| 名称 | 类型 | 说明 |
|------|------|------|
| id | u | 新作业的 ID（uint32） |
| job | o | 新作业的 D-Bus 对象路径 |
| unit | s | 此作业关联的单元名称 |

---

### JobRemoved

**说明：** 当用户会话中的作业完成并从队列中移除时发出此信号。

**参数：**
| 名称 | 类型 | 说明 |
|------|------|------|
| id | u | 完成的作业 ID（uint32） |
| job | o | 作业的 D-Bus 对象路径（可能已失效） |
| unit | s | 此作业关联的单元名称 |
| result | s | 作业结果：`done`（成功）、`canceled`（已取消）、`timeout`（超时）、`failed`（失败）、`dependency`（依赖失败）、`skipped`（跳过） |

---

### StartupFinished

**说明：** 当用户会话启动完成（default.target 激活）时发出此信号。

**参数：**
| 名称 | 类型 | 说明 |
|------|------|------|
| firmware | t | 固件阶段耗时（微秒，用户会话通常为 0） |
| loader | t | 引导加载器阶段耗时（微秒，用户会话通常为 0） |
| kernel | t | 内核初始化耗时（微秒，用户会话通常为 0） |
| initrd | t | initrd 阶段耗时（微秒，用户会话通常为 0） |
| userspace | t | 用户会话 systemd 启动耗时（微秒） |
| total | t | 用户会话总启动时间（微秒） |

---

### UnitFilesChanged

**说明：** 当用户会话中的单元文件发生变更时发出此信号。

**参数：** 无

---

### Reloading

**说明：** 当用户会话 systemd 开始或完成重载（daemon-reload）时发出此信号。

**参数：**
| 名称 | 类型 | 说明 |
|------|------|------|
| active | b | true 表示重载开始，false 表示重载完成 |

---

## 使用示例

```bash
# 列出所有用户服务
busctl --user call org.freedesktop.systemd1 \
    /org/freedesktop/systemd1 \
    org.freedesktop.systemd1.Manager \
    ListUnits

# 启动用户服务
busctl --user call org.freedesktop.systemd1 \
    /org/freedesktop/systemd1 \
    org.freedesktop.systemd1.Manager \
    StartUnit ss myapp.service replace

# 停止用户服务
busctl --user call org.freedesktop.systemd1 \
    /org/freedesktop/systemd1 \
    org.freedesktop.systemd1.Manager \
    StopUnit ss myapp.service replace

# 启用用户服务（开机自动启动）
busctl --user call org.freedesktop.systemd1 \
    /org/freedesktop/systemd1 \
    org.freedesktop.systemd1.Manager \
    EnableUnitFiles asbb 1 myapp.service false false

# 重载用户会话配置
busctl --user call org.freedesktop.systemd1 \
    /org/freedesktop/systemd1 \
    org.freedesktop.systemd1.Manager \
    Reload

# 查询用户会话系统状态
busctl --user get-property org.freedesktop.systemd1 \
    /org/freedesktop/systemd1 \
    org.freedesktop.systemd1.Manager \
    SystemState

# 查询用户单元文件搜索路径
busctl --user get-property org.freedesktop.systemd1 \
    /org/freedesktop/systemd1 \
    org.freedesktop.systemd1.Manager \
    UnitPath

# 设置用户会话环境变量
busctl --user call org.freedesktop.systemd1 \
    /org/freedesktop/systemd1 \
    org.freedesktop.systemd1.Manager \
    SetEnvironment as 1 MY_VAR=hello
```

---

## 附录

### D-Bus 类型签名对照表

| 签名 | 类型描述 |
|------|------|
| `s` | 字符串（string） |
| `o` | D-Bus 对象路径（object path） |
| `u` | 32 位无符号整数（uint32） |
| `i` | 32 位有符号整数（int32） |
| `b` | 布尔值（boolean） |
| `t` | 64 位无符号整数（uint64） |
| `x` | 64 位有符号整数（int64） |
| `y` | 字节（byte，uint8） |
| `d` | 双精度浮点数（double） |
| `h` | Unix 文件描述符（unix fd） |
| `v` | 变体类型（variant，任意类型） |
| `as` | 字符串数组 |
| `ao` | 对象路径数组 |
| `at` | uint64 数组 |
| `au` | uint32 数组 |
| `ay` | 字节数组（byte array） |
| `a(ss)` | (字符串, 字符串) 结构体数组 |
| `a(sv)` | (字符串, variant) 结构体数组（属性数组） |
| `a(sa(sv))` | (字符串, 属性数组) 结构体数组 |
| `a(sss)` | (字符串, 字符串, 字符串) 结构体数组（变更列表） |
| `a(us)` | (uint32, 字符串) 结构体数组 |
| `a(usssoo)` | 作业信息结构体数组（id, unit, type, state, job_path, unit_path） |
| `a(ssssssouso)` | 单元信息结构体数组（见 ListUnits） |

### 作业模式（mode 参数）说明

| 模式 | 说明 |
|------|------|
| `replace` | 替换同类型现有作业（最常用） |
| `fail` | 若已有冲突作业则使本次调用失败返回错误 |
| `queue` | 将作业加入队列；若已有完全相同的作业，直接返回现有作业 |
| `isolate` | 仅适用于 target 单元：激活此 target 并停止其他激活单元 |
| `flush` | 先取消所有等待中的作业，然后排队新作业 |
| `ignore-dependencies` | 忽略所有依赖关系直接执行 |
| `ignore-requirements` | 仅忽略 Requires/Wants 依赖，但保留 After/Before 排序依赖 |
| `triggering` | 仅当有其他单元正在等待触发此单元时才排队 |

### 权限说明

在用户总线上，权限模型与系统总线有所不同：

- **无需特殊权限**：会话用户可直接调用，无需 polkit 授权
- **大多数管理操作**：会话用户即可管理自己的用户服务，无需 root 或 polkit
- **系统总线的 `CAP_SYS_BOOT` 操作**：在用户总线上不存在（Reboot、PowerOff 等不可用）
- **SELinux 限制**：若系统启用了 SELinux，仍可能影响哪些单元信息对调用方可见

### 用户总线与系统总线对比

| 特性 | 系统总线 | 用户总线 |
|------|----------|----------|
| 管理实例 | PID 1（系统管理器） | `systemd --user`（每用户一个实例） |
| 管理对象 | 系统服务 | 用户会话服务 |
| 总线地址 | `/run/dbus/system_bus_socket` | `/run/user/<UID>/bus` |
| busctl 选项 | `busctl --system` 或默认 | `busctl --user` |
| systemctl 选项 | `systemctl` 或 `systemctl --system` | `systemctl --user` |
| 单元文件位置 | `/etc/systemd/system/`、`/usr/lib/systemd/system/` | `~/.config/systemd/user/`、`/usr/lib/systemd/user/` |
| 需要 polkit | 是（管理操作） | 否（用户管理自己的会话） |
| 电源操作 | 支持（Reboot、PowerOff 等） | 不支持 |

## 参见

- [`org.freedesktop.systemd1.Unit`](org.freedesktop.systemd1.Unit.md) — 所有单元类型的基础接口（用户总线版本）
- `systemd.unit(5)` — 单元文件配置参考
- `systemd --user` — 用户会话 systemd 实例
- `systemctl --user` — 用户会话服务控制命令
- `systemd-run --user` — 创建临时用户会话服务
