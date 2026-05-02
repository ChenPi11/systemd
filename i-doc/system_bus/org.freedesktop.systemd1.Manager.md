# 接口名称：org.freedesktop.systemd1.Manager

## 接口说明

`org.freedesktop.systemd1.Manager` 是 systemd 的核心 D-Bus 管理接口，暴露于系统总线（system bus）上。通过此接口，客户端可以查询和控制 systemd 守护进程（PID 1）的所有核心功能，包括：

- **单元（Unit）管理**：启动、停止、重载、重启、查询单元状态
- **作业（Job）管理**：查询和取消正在运行的作业
- **单元文件管理**：启用、禁用、屏蔽、预设单元文件
- **系统电源操作**：重启、关机、停机、kexec
- **环境变量管理**：设置、取消设置系统环境变量
- **动态用户查询**：查找 systemd 动态分配的用户
- **守护进程控制**：重载配置、重新执行自身

此接口要求调用者具有适当的权限（通过 polkit 或 SELinux 策略控制），部分方法任何用户均可调用，部分方法需要管理员权限。

## 对象路径

`/org/freedesktop/systemd1`

---

## 方法（Methods）

### GetUnit

**说明：** 按名称获取已加载单元的 D-Bus 对象路径。若单元未加载，则返回错误。传入空字符串时，返回调用方所属单元的路径。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| name | s | 输入 | 单元名称（如 `nginx.service`）。传入空字符串时返回调用方所属单元 |
| unit | o | 输出 | 单元对象路径（如 `/org/freedesktop/systemd1/unit/nginx_2eservice`） |

**权限：** 无需特殊权限（任何人均可调用）

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

**说明：** 按调用 ID（invocation ID）查找单元。每次单元启动时都会分配一个唯一的 128 位 UUID 作为调用 ID。传入全零 ID 时返回调用方所属单元。返回的对象路径以调用 ID 为基础，只要调用 ID 不变就保持有效。

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
| cgroup | s | 输入 | cgroup 绝对路径（如 `/system.slice/nginx.service`） |
| unit | o | 输出 | 单元对象路径 |

**权限：** 无需特殊权限

**返回：** 该 cgroup 路径对应单元的 D-Bus 对象路径

---

### GetUnitByPIDFD

**说明：** 通过 PIDFD（进程文件描述符）查找单元。PIDFD 是 Linux 5.3+ 提供的安全进程引用机制，能避免 PID 重用竞争。查找完成后会验证进程仍然存活且 PID 未改变。

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

**说明：** 按名称加载单元（如果尚未加载则加载），并返回其对象路径。与 `GetUnit` 不同，此方法会触发单元加载。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| name | s | 输入 | 单元名称（如 `nginx.service`） |
| unit | o | 输出 | 单元对象路径 |

**权限：** 无需特殊权限

**返回：** 单元的 D-Bus 对象路径

---

### StartUnit

**说明：** 启动指定单元。如果单元尚未加载，会先加载它。此方法是最常用的单元启动方式，等同于 `systemctl start`。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| name | s | 输入 | 要启动的单元名称 |
| mode | s | 输入 | 作业模式（见下方说明） |
| job | o | 输出 | 排队的作业对象路径 |

**mode 参数可选值：**
- `replace`：替换同类型现有作业（最常用）
- `fail`：若已有冲突作业则失败
- `queue`：将作业加入队列，若已有相同作业则直接返回现有作业
- `isolate`：启动单元并停止所有其他单元（仅适用于 target 类型）
- `flush`：取消所有等待中的作业后再启动
- `ignore-dependencies`：忽略所有依赖关系直接启动
- `ignore-requirements`：忽略 requires/wants 依赖但保留排序依赖

**权限：** 需要 polkit 授权（`org.freedesktop.systemd1.manage-units`）

**返回：** 排队作业的对象路径

---

### StartUnitWithFlags

**说明：** 与 `StartUnit` 相同，但额外支持通过 `flags` 参数传递启动标志（uint64 位域）。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| name | s | 输入 | 要启动的单元名称 |
| mode | s | 输入 | 作业模式（同 StartUnit） |
| flags | t | 输入 | 额外标志（uint64，位域） |
| job | o | 输出 | 排队的作业对象路径 |

**权限：** 需要 polkit 授权

**返回：** 排队作业的对象路径

---

### StartUnitReplace

**说明：** 替换方式启动单元。要求旧单元当前有一个 start 类型的作业正在等待，然后用新单元替换它。这常用于在 socket 激活场景下替换等待中的服务启动。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| old_unit | s | 输入 | 当前有等待 start 作业的旧单元名称 |
| new_unit | s | 输入 | 要启动的新单元名称 |
| mode | s | 输入 | 作业模式 |
| job | o | 输出 | 排队的作业对象路径 |

**权限：** 需要 polkit 授权

**返回：** 排队作业的对象路径

---

### StopUnit

**说明：** 停止指定单元，等同于 `systemctl stop`。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| name | s | 输入 | 要停止的单元名称 |
| mode | s | 输入 | 作业模式（通常使用 `replace`） |
| job | o | 输出 | 排队的作业对象路径 |

**权限：** 需要 polkit 授权

**返回：** 排队作业的对象路径

---

### ReloadUnit

**说明：** 重载指定单元的配置（不重启进程），等同于 `systemctl reload`。仅对支持重载的单元有效（服务必须定义 `ExecReload=`）。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| name | s | 输入 | 要重载的单元名称 |
| mode | s | 输入 | 作业模式 |
| job | o | 输出 | 排队的作业对象路径 |

**权限：** 需要 polkit 授权

**返回：** 排队作业的对象路径

---

### RestartUnit

**说明：** 重启指定单元（先停止再启动），等同于 `systemctl restart`。即使单元当前未运行也会启动它。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| name | s | 输入 | 要重启的单元名称 |
| mode | s | 输入 | 作业模式 |
| job | o | 输出 | 排队的作业对象路径 |

**权限：** 需要 polkit 授权

**返回：** 排队作业的对象路径

---

### TryRestartUnit

**说明：** 尝试重启单元，但仅当单元当前处于运行状态时才重启，等同于 `systemctl try-restart`。若单元未运行则忽略。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| name | s | 输入 | 要尝试重启的单元名称 |
| mode | s | 输入 | 作业模式 |
| job | o | 输出 | 排队的作业对象路径 |

**权限：** 需要 polkit 授权

**返回：** 排队作业的对象路径

---

### ReloadOrRestartUnit

**说明：** 如果单元支持重载则重载，否则重启，等同于 `systemctl reload-or-restart`。若单元未运行则启动它。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| name | s | 输入 | 单元名称 |
| mode | s | 输入 | 作业模式 |
| job | o | 输出 | 排队的作业对象路径 |

**权限：** 需要 polkit 授权

**返回：** 排队作业的对象路径

---

### ReloadOrTryRestartUnit

**说明：** 如果单元支持重载则重载，否则尝试重启（仅当运行时），等同于 `systemctl reload-or-try-restart`。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| name | s | 输入 | 单元名称 |
| mode | s | 输入 | 作业模式 |
| job | o | 输出 | 排队的作业对象路径 |

**权限：** 需要 polkit 授权

**返回：** 排队作业的对象路径

---

### EnqueueUnitJob

**说明：** 向单元的作业队列中添加一个作业，并返回详细的作业信息，包括受影响的相关作业列表。相比其他启动/停止方法，此方法提供了更丰富的返回信息。

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
| affected_jobs | a(uosos) | 输出 | 受影响的关联作业数组，每项包含：(作业ID, 作业路径, 单元ID, 单元路径, 作业类型) |

**权限：** 需要 polkit 授权

**返回：** 主作业信息及受影响的关联作业列表

---

### KillUnit

**说明：** 向单元进程组（或部分进程）发送指定信号，等同于 `systemctl kill`。即使单元未正确加载也可用于终止其 cgroup 中的进程。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| name | s | 输入 | 单元名称 |
| whom | s | 输入 | 发送目标：`main`（主进程）、`control`（控制进程）、`all`（全部进程） |
| signal | i | 输入 | 信号编号（int32，如 15 为 SIGTERM，9 为 SIGKILL） |

**权限：** 需要 polkit 授权

**返回：** 无

---

### KillUnitSubgroup

**说明：** 向单元的指定 cgroup 子组中的进程发送信号。可以精确针对单元内某个子组，而不影响整个单元。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| name | s | 输入 | 单元名称 |
| whom | s | 输入 | 发送目标（同 KillUnit） |
| subgroup | s | 输入 | cgroup 子组路径 |
| signal | i | 输入 | 信号编号 |

**权限：** 需要 polkit 授权

**返回：** 无

---

### QueueSignalUnit

**说明：** 向单元进程发送实时信号，并携带附加值（sigqueue 语义）。适用于支持实时信号的应用程序。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| name | s | 输入 | 单元名称 |
| whom | s | 输入 | 发送目标（`main`、`control`、`all`） |
| signal | i | 输入 | 信号编号 |
| value | i | 输入 | 随信号发送的附加整数值（sigqueue 的 value 参数） |

**权限：** 需要 polkit 授权

**返回：** 无

---

### CleanUnit

**说明：** 清理单元遗留的运行时文件、日志、状态等资源，等同于 `systemctl clean`。单元必须已加载且处于非活动状态。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| name | s | 输入 | 单元名称 |
| mask | as | 输入 | 要清理的资源类型列表，如 `["runtime", "state", "cache", "logs", "fdstore"]` |

**权限：** 需要 polkit 授权

**返回：** 无

---

### FreezeUnit

**说明：** 冻结单元（暂停 cgroup 中所有进程）。单元必须处于活动（running）状态且已正确加载。冻结后进程不会被调度执行，但仍保留在内存中。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| name | s | 输入 | 要冻结的单元名称 |

**权限：** 需要 polkit 授权

**返回：** 无

---

### ThawUnit

**说明：** 解冻单元（恢复之前被冻结的单元中的进程），与 `FreezeUnit` 相对。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| name | s | 输入 | 要解冻的单元名称 |

**权限：** 需要 polkit 授权

**返回：** 无

---

### ResetFailedUnit

**说明：** 重置指定单元的失败状态，等同于 `systemctl reset-failed <unit>`。将失败的单元的状态清除，使其可以再次启动。即使单元未正确加载（如单元文件已删除）也可调用。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| name | s | 输入 | 要重置失败状态的单元名称 |

**权限：** 需要 polkit 授权

**返回：** 无

---

### SetUnitProperties

**说明：** 动态修改单元的属性（在运行时或永久修改），等同于 `systemctl set-property`。单元必须已正确加载。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| name | s | 输入 | 单元名称 |
| runtime | b | 输入 | 是否仅运行时生效（true = 重启后恢复，false = 永久保存） |
| properties | a(sv) | 输入 | 要设置的属性数组，每项为 (属性名, 属性值variant) |

**权限：** 需要 polkit 授权

**返回：** 无

---

### BindMountUnit

**说明：** 向指定的 service 单元运行时命名空间中添加绑定挂载。单元必须是 service 类型且处于活动状态。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| name | s | 输入 | service 单元名称 |
| source | s | 输入 | 宿主机上的源路径 |
| destination | s | 输入 | 单元命名空间内的目标挂载点 |
| read_only | b | 输入 | 是否只读挂载 |
| mkdir | b | 输入 | 若目标不存在是否自动创建目录 |

**权限：** 需要 polkit 授权

**返回：** 无

---

### MountImageUnit

**说明：** 向指定的 service 单元运行时命名空间中挂载一个磁盘镜像文件（如 `.img`）。单元必须是 service 类型且处于活动状态。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| name | s | 输入 | service 单元名称 |
| source | s | 输入 | 镜像文件路径 |
| destination | s | 输入 | 单元命名空间内的目标挂载点 |
| read_only | b | 输入 | 是否只读挂载 |
| mkdir | b | 输入 | 若目标不存在是否自动创建目录 |
| options | a(ss) | 输入 | 挂载选项数组，每项为 (分区, 选项字符串) |

**权限：** 需要 polkit 授权

**返回：** 无

---

### RefUnit

**说明：** 增加单元的引用计数。可防止单元因无人使用而被自动卸载或停止。单元必须已正确加载，若未加载则会先加载。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| name | s | 输入 | 单元名称 |

**权限：** 需要 polkit 授权

**返回：** 无

---

### UnrefUnit

**说明：** 减少单元的引用计数（与 `RefUnit` 相对）。当引用计数降为零时，若单元被配置为在无引用时停止，则会自动停止。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| name | s | 输入 | 单元名称 |

**权限：** 需要 polkit 授权

**返回：** 无

---

### StartTransientUnit

**说明：** 创建并启动一个临时单元（transient unit）。临时单元在停止后自动删除，不需要持久的单元文件。常用于 `systemd-run` 创建临时服务/作用域，也可用于实现会话管理等功能。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| name | s | 输入 | 临时单元名称（必须包含合法的单元类型后缀） |
| mode | s | 输入 | 作业模式 |
| properties | a(sv) | 输入 | 主单元属性数组 |
| aux | a(sa(sv)) | 输入 | 辅助单元数组，每项为 (单元名, 属性数组) |
| job | o | 输出 | 排队的作业对象路径 |

**权限：** 需要 polkit 授权

**返回：** 排队作业的对象路径

---

### GetUnitProcesses

**说明：** 获取指定单元 cgroup 中所有进程的信息列表。即使单元未正确加载也可调用（只要 cgroup 中有进程）。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| name | s | 输入 | 单元名称 |
| processes | a(sus) | 输出 | 进程列表，每项为 (cgroup路径, PID, 进程命令行) |

**权限：** 无需特殊权限

**返回：** 进程信息数组，每项包含 cgroup 相对路径、PID（uint32）、进程命令行字符串

---

### AttachProcessesToUnit

**说明：** 将指定 PID 列表中的进程附加到单元的 cgroup 中（可选指定子组）。单元必须已正确加载且处于活动状态。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| unit_name | s | 输入 | 目标单元名称 |
| subcgroup | s | 输入 | 子 cgroup 路径（相对路径，空字符串表示根 cgroup） |
| pids | au | 输入 | 要附加的进程 PID 数组（uint32 数组） |

**权限：** 需要 polkit 授权

**返回：** 无

---

### RemoveSubgroupFromUnit

**说明：** 从单元中移除指定的 cgroup 子组。单元必须已正确加载。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| unit_name | s | 输入 | 单元名称 |
| subcgroup | s | 输入 | 要移除的子 cgroup 路径 |
| flags | t | 输入 | 标志（uint64，保留备用，当前传 0） |

**权限：** 需要 polkit 授权

**返回：** 无

---

### AbandonScope

**说明：** 放弃（abandon）一个 scope 类型的单元。仅适用于 scope 单元。调用后 systemd 将不再跟踪该 scope 中的进程，scope 单元会进入失败状态。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| name | s | 输入 | scope 单元名称（必须是 `.scope` 后缀） |

**权限：** 需要 polkit 授权

**返回：** 无

---

### GetJob

**说明：** 按作业 ID 获取正在运行的作业的 D-Bus 对象路径。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| id | u | 输入 | 作业 ID（uint32） |
| job | o | 输出 | 作业对象路径 |

**权限：** 无需特殊权限

**返回：** 作业对象路径

---

### GetJobAfter

**说明：** 获取在指定作业之后排队等待的所有作业（依赖关系中在此作业之后执行的作业）。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| id | u | 输入 | 作业 ID（uint32） |
| jobs | a(usssoo) | 输出 | 作业信息数组，每项为 (作业ID, 单元名, 作业类型, 作业状态, 作业路径, 单元路径) |

**权限：** 无需特殊权限

**返回：** 相关后续作业的信息数组

---

### GetJobBefore

**说明：** 获取在指定作业之前排队等待的所有作业（此作业需要等待它们完成后才能执行的作业）。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| id | u | 输入 | 作业 ID（uint32） |
| jobs | a(usssoo) | 输出 | 作业信息数组（同 GetJobAfter） |

**权限：** 无需特殊权限

**返回：** 此作业所等待的前置作业信息数组

---

### CancelJob

**说明：** 取消指定 ID 的作业，等同于 `systemctl cancel`。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| id | u | 输入 | 要取消的作业 ID（uint32） |

**权限：** 需要 polkit 授权

**返回：** 无

---

### ClearJobs

**说明：** 清除所有当前排队中的作业。此操作需要 `manage-units` 权限，相当于强制清空整个作业队列。

**参数：** 无

**权限：** 需要 polkit 授权（`org.freedesktop.systemd1.manage-units`）

**返回：** 无

---

### ResetFailed

**说明：** 重置所有单元的失败状态，等同于 `systemctl reset-failed`（不带参数）。将所有处于 `failed` 状态的单元清除失败标记。

**参数：** 无

**权限：** 需要 polkit 授权

**返回：** 无

---

### SetShowStatus

**说明：** 设置 systemd 在控制台上显示状态的模式。控制启动/停止过程中是否在终端显示状态信息。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| mode | s | 输入 | 显示模式：`yes`、`no`、`auto`、`error`、`temporary-failure`，空字符串恢复默认 |

**权限：** 需要 polkit 授权

**返回：** 无

---

### ListUnits

**说明：** 列出所有已加载的单元信息，等同于 `systemctl list-units`。任何用户均可调用，但 SELinux 策略可能过滤部分单元。

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

**说明：** 按状态过滤列出单元。只返回活动状态匹配指定过滤器的单元。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| states | as | 输入 | 状态过滤列表（如 `["active", "failed"]`），空列表返回所有 |
| units | a(ssssssouso) | 输出 | 单元信息数组（结构同 ListUnits） |

**权限：** 无需特殊权限

**返回：** 过滤后的单元信息数组

---

### ListUnitsByPatterns

**说明：** 同时按状态和名称模式过滤列出单元。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| states | as | 输入 | 状态过滤列表 |
| patterns | as | 输入 | 名称模式列表（支持 glob 通配符，如 `["*.service", "ssh*"]`） |
| units | a(ssssssouso) | 输出 | 单元信息数组 |

**权限：** 无需特殊权限

**返回：** 过滤后的单元信息数组

---

### ListUnitsByNames

**说明：** 按单元名称列表获取单元信息。若单元未加载则会尝试加载，包含所有请求单元的信息（即使某些不存在）。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| names | as | 输入 | 单元名称列表 |
| units | a(ssssssouso) | 输出 | 单元信息数组 |

**权限：** 无需特殊权限

**返回：** 请求单元的信息数组

---

### ListJobs

**说明：** 列出所有当前排队或运行中的作业，等同于 `systemctl list-jobs`。

**参数：** 无

**权限：** 无需特殊权限（需要 SELinux `status` 访问权限）

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

**说明：** 订阅 systemd 事件（信号）。调用后，此连接将接收 systemd 发出的所有信号（UnitNew、UnitRemoved、JobNew、JobRemoved 等）。在直连（non-bus）连接上默认已订阅，仅在 API bus 连接上需要显式订阅。

**参数：** 无

**权限：** 无需特殊权限（需要 SELinux `status` 访问权限）

**返回：** 无

---

### Unsubscribe

**说明：** 取消之前通过 `Subscribe()` 建立的事件订阅。

**参数：** 无

**权限：** 无需特殊权限

**返回：** 无

---

### Dump

**说明：** 返回 systemd 当前状态的完整文本转储（所有单元、作业、依赖关系等），等同于 `systemctl dump`。可能产生大量输出，有速率限制保护（未授权用户）。

**参数：** 无

**权限：** 无需特殊权限（高频调用需要额外权限绕过速率限制）

**返回：**
| 名称 | 类型 | 说明 |
|------|------|------|
| output | s | 状态转储文本 |

---

### DumpUnitsMatchingPatterns

**说明：** 返回匹配指定名称模式的单元的状态转储，比 `Dump` 更有针对性。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| patterns | as | 输入 | 单元名称模式列表（支持 glob） |
| output | s | 输出 | 转储文本 |

**权限：** 无需特殊权限（高频调用有速率限制）

**返回：** 匹配单元的状态转储文本

---

### DumpByFileDescriptor

**说明：** 与 `Dump` 功能相同，但通过文件描述符（memfd）返回结果，适合处理大量数据，避免 D-Bus 消息大小限制。

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

**说明：** 重新加载 systemd 的配置文件（daemon-reload），等同于 `systemctl daemon-reload`。重载完成后才返回响应，因此调用方可以感知重载完成时机。有速率限制保护。

**参数：** 无

**权限：** 需要 polkit 授权（`org.freedesktop.systemd1.reload-daemon`）

**返回：** 无（重载完成后返回）

---

### Reexecute

**说明：** 使 systemd 重新执行自身（daemon-reexec），等同于 `systemctl daemon-reexec`。systemd 将会重启自身进程，继承所有状态。此方法不发送回复（`METHOD_NO_REPLY`），调用方应等待 D-Bus 连接断开然后重连。有速率限制保护。

**参数：** 无

**权限：** 需要 polkit 授权

**返回：** 无（不发送回复）

---

### Exit

**说明：** 使 systemd 退出。在容器或用户实例中执行实际的 exit()，在宿主机系统管理器中会退出并最终触发 reboot()。

**参数：** 无

**权限：** 需要 SELinux `halt` 访问权限

**返回：** 无

---

### Reboot

**说明：** 触发系统重启，等同于 `systemctl reboot`。仅在系统管理器实例中有效（非用户实例）。

**参数：** 无

**权限：** 需要 `CAP_SYS_BOOT` 能力

**返回：** 无

---

### SoftReboot

**说明：** 执行软重启（soft-reboot）：systemd 将用户空间重启到新的根文件系统，但内核继续运行，无需硬件重置。常用于更新后的快速重启。仅在系统管理器实例中有效。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| new_root | s | 输入 | 新根文件系统路径（绝对路径），空字符串使用默认配置 |

**权限：** 需要 `CAP_SYS_BOOT` 能力

**返回：** 无

---

### PowerOff

**说明：** 关闭系统电源，等同于 `systemctl poweroff`。仅在系统管理器实例中有效。

**参数：** 无

**权限：** 需要 `CAP_SYS_BOOT` 能力

**返回：** 无

---

### Halt

**说明：** 停止系统（halt，停机但不断电），等同于 `systemctl halt`。仅在系统管理器实例中有效。

**参数：** 无

**权限：** 需要 `CAP_SYS_BOOT` 能力

**返回：** 无

---

### KExec

**说明：** 使用 kexec 机制重启到新内核，等同于 `systemctl kexec`。不通过 BIOS 引导，直接加载新内核。仅在系统管理器实例中有效。

**参数：** 无

**权限：** 需要 `CAP_SYS_BOOT` 能力

**返回：** 无

---

### SwitchRoot

**说明：** 切换根文件系统（用于 initrd 到实际根的切换），等同于 `systemctl switch-root`。仅在处于 initrd 环境中且目标路径包含合法 OS 树时有效。新根必须包含 `os-release` 文件。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| new_root | s | 输入 | 新根文件系统路径（空字符串默认为 `/sysroot`） |
| init | s | 输入 | 新根中 init 程序的路径（空字符串使用默认 init） |

**权限：** 需要 `CAP_SYS_BOOT` 能力

**返回：** 无

---

### SetEnvironment

**说明：** 向 systemd 的全局环境变量集合中添加环境变量，等同于 `systemctl set-environment`。这些变量会被之后启动的所有单元继承。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| assignments | as | 输入 | 环境变量赋值数组（格式 `KEY=VALUE`） |

**权限：** 需要 polkit 授权（`org.freedesktop.systemd1.set-environment`）

**返回：** 无

---

### UnsetEnvironment

**说明：** 从 systemd 的全局环境变量集合中删除指定变量，等同于 `systemctl unset-environment`。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| names | as | 输入 | 要删除的环境变量名称数组（仅变量名，不含 `=` 和值） |

**权限：** 需要 polkit 授权

**返回：** 无

---

### UnsetAndSetEnvironment

**说明：** 原子地执行"先删除后设置"环境变量操作，等同于 `systemctl unset-environment` 后紧跟 `systemctl set-environment`，但保证原子性。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| names | as | 输入 | 要删除的环境变量名称数组 |
| assignments | as | 输入 | 要设置的环境变量赋值数组（格式 `KEY=VALUE`） |

**权限：** 需要 polkit 授权

**返回：** 无

---

### EnqueueMarkedJobs

**说明：** 将所有已标记为需要重启/重载/启动/停止的单元的对应作业加入队列。标记机制允许在不立即触发的情况下将单元标记为待处理状态，然后通过此方法统一处理。

**参数：** 无

**权限：** 需要 polkit 授权

**返回：**
| 名称 | 类型 | 说明 |
|------|------|------|
| jobs | ao | 成功排队的作业对象路径数组 |

---

### ListUnitFiles

**说明：** 列出系统中所有已知的单元文件及其状态，等同于 `systemctl list-unit-files`。

**参数：** 无

**权限：** 无需特殊权限（需要 SELinux `status` 访问权限）

**返回：** `a(ss)` — 单元文件信息数组，每项包含：
| 序号 | 类型 | 说明 |
|------|------|------|
| 1 | s | 单元文件路径 |
| 2 | s | 启用状态（`enabled`、`disabled`、`masked`、`static`、`alias` 等） |

---

### ListUnitFilesByPatterns

**说明：** 按启用状态和名称模式过滤列出单元文件，等同于 `systemctl list-unit-files` 带过滤参数。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| states | as | 输入 | 启用状态过滤列表（如 `["enabled", "disabled"]`） |
| patterns | as | 输入 | 名称模式列表（支持 glob） |
| unit_files | a(ss) | 输出 | 单元文件信息数组 |

**权限：** 无需特殊权限

**返回：** 过滤后的单元文件信息数组

---

### GetUnitFileState

**说明：** 获取指定单元文件的启用状态，等同于 `systemctl is-enabled`。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| file | s | 输入 | 单元名称或单元文件路径 |
| state | s | 输出 | 启用状态字符串（`enabled`、`disabled`、`masked`、`static`、`alias`、`indirect`、`linked` 等） |

**权限：** 无需特殊权限

**返回：** 单元文件的启用状态字符串

---

### EnableUnitFiles

**说明：** 启用指定的单元文件（创建符号链接），等同于 `systemctl enable`。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| files | as | 输入 | 单元名称或绝对路径数组 |
| runtime | b | 输入 | 是否仅运行时启用（true = 写入 `/run`，false = 写入 `/etc`） |
| force | b | 输入 | 是否覆盖已有的符号链接 |
| carries_install_info | b | 输出 | 单元文件是否包含 `[Install]` 节信息 |
| changes | a(sss) | 输出 | 执行的变更列表，每项为 (变更类型, 目标路径, 源路径) |

**权限：** 需要 polkit 授权（`org.freedesktop.systemd1.manage-unit-files`）

**返回：** 是否包含安装信息 + 变更列表

---

### DisableUnitFiles

**说明：** 禁用指定的单元文件（删除符号链接），等同于 `systemctl disable`。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| files | as | 输入 | 单元名称或绝对路径数组 |
| runtime | b | 输入 | 是否仅禁用运行时启用（与 `EnableUnitFiles` 的 `runtime` 对应） |
| changes | a(sss) | 输出 | 执行的变更列表 |

**权限：** 需要 polkit 授权

**返回：** 变更列表

---

### EnableUnitFilesWithFlags

**说明：** 与 `EnableUnitFiles` 功能相同，但使用单个 `flags` 参数（uint64 位域）代替多个布尔值参数，提供更灵活的控制。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| files | as | 输入 | 单元名称或绝对路径数组 |
| flags | t | 输入 | 标志位域（`UNIT_FILE_RUNTIME=1`、`UNIT_FILE_FORCE=2`、`UNIT_FILE_PORTABLE=4` 等） |
| carries_install_info | b | 输出 | 是否包含安装信息 |
| changes | a(sss) | 输出 | 变更列表 |

**权限：** 需要 polkit 授权

**返回：** 是否包含安装信息 + 变更列表

---

### DisableUnitFilesWithFlags

**说明：** 与 `DisableUnitFiles` 功能相同，但使用 `flags` 参数。注意：不支持 `UNIT_FILE_FORCE` 标志。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| files | as | 输入 | 单元名称或绝对路径数组 |
| flags | t | 输入 | 标志位域 |
| changes | a(sss) | 输出 | 变更列表 |

**权限：** 需要 polkit 授权

**返回：** 变更列表

---

### DisableUnitFilesWithFlagsAndInstallInfo

**说明：** 与 `DisableUnitFilesWithFlags` 相同，但额外返回 `carries_install_info` 字段，指示单元文件是否包含 `[Install]` 节。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| files | as | 输入 | 单元名称或绝对路径数组 |
| flags | t | 输入 | 标志位域 |
| carries_install_info | b | 输出 | 是否包含安装信息 |
| changes | a(sss) | 输出 | 变更列表 |

**权限：** 需要 polkit 授权

**返回：** 是否包含安装信息 + 变更列表

---

### ReenableUnitFiles

**说明：** 重新启用单元文件（先禁用再启用），等同于 `systemctl reenable`。用于在单元的 `[Install]` 节更改后更新符号链接。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| files | as | 输入 | 单元名称或绝对路径数组 |
| runtime | b | 输入 | 是否仅运行时操作 |
| force | b | 输入 | 是否覆盖已有符号链接 |
| carries_install_info | b | 输出 | 是否包含安装信息 |
| changes | a(sss) | 输出 | 变更列表 |

**权限：** 需要 polkit 授权

**返回：** 是否包含安装信息 + 变更列表

---

### LinkUnitFiles

**说明：** 将指定路径的单元文件链接到单元目录中，等同于 `systemctl link`。使不在标准搜索路径中的单元文件可被 systemd 识别。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| files | as | 输入 | 单元文件绝对路径数组 |
| runtime | b | 输入 | 是否仅运行时链接 |
| force | b | 输入 | 是否覆盖已有链接 |
| changes | a(sss) | 输出 | 变更列表 |

**权限：** 需要 polkit 授权

**返回：** 变更列表

---

### PresetUnitFiles

**说明：** 根据预设策略（`/etc/systemd/system-preset/`）启用或禁用单元文件，等同于 `systemctl preset`。预设策略决定单元默认是否启用。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| files | as | 输入 | 单元名称或路径数组 |
| runtime | b | 输入 | 是否仅运行时操作 |
| force | b | 输入 | 是否覆盖已有符号链接 |
| carries_install_info | b | 输出 | 是否包含安装信息 |
| changes | a(sss) | 输出 | 变更列表 |

**权限：** 需要 polkit 授权

**返回：** 是否包含安装信息 + 变更列表

---

### PresetUnitFilesWithMode

**说明：** 与 `PresetUnitFiles` 相同，但允许指定预设模式，控制预设策略的应用方式。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| files | as | 输入 | 单元名称或路径数组 |
| mode | s | 输入 | 预设模式：`full`（全部应用）、`enable-only`（仅启用）、`disable-only`（仅禁用），空字符串为 `full` |
| runtime | b | 输入 | 是否仅运行时操作 |
| force | b | 输入 | 是否覆盖已有符号链接 |
| carries_install_info | b | 输出 | 是否包含安装信息 |
| changes | a(sss) | 输出 | 变更列表 |

**权限：** 需要 polkit 授权

**返回：** 是否包含安装信息 + 变更列表

---

### MaskUnitFiles

**说明：** 屏蔽指定单元文件（将其符号链接到 `/dev/null`），等同于 `systemctl mask`。屏蔽后的单元无法被启动。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| files | as | 输入 | 单元名称或路径数组 |
| runtime | b | 输入 | 是否仅运行时屏蔽 |
| force | b | 输入 | 是否强制屏蔽（即使单元正在运行） |
| changes | a(sss) | 输出 | 变更列表 |

**权限：** 需要 polkit 授权

**返回：** 变更列表

---

### UnmaskUnitFiles

**说明：** 取消屏蔽指定单元文件，等同于 `systemctl unmask`。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| files | as | 输入 | 单元名称或路径数组 |
| runtime | b | 输入 | 是否仅取消运行时屏蔽 |
| changes | a(sss) | 输出 | 变更列表 |

**权限：** 需要 polkit 授权

**返回：** 变更列表

---

### RevertUnitFiles

**说明：** 还原单元文件的本地修改（删除 `/etc/systemd/system/` 中的覆盖文件），等同于 `systemctl revert`。恢复到包中附带的原始版本。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| files | as | 输入 | 单元名称或路径数组 |
| changes | a(sss) | 输出 | 变更列表 |

**权限：** 需要 polkit 授权

**返回：** 变更列表

---

### SetDefaultTarget

**说明：** 设置系统默认启动 target（修改 `default.target` 符号链接），等同于 `systemctl set-default`。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| name | s | 输入 | target 单元名称（如 `graphical.target`） |
| force | b | 输入 | 是否强制覆盖现有默认 target |
| changes | a(sss) | 输出 | 变更列表 |

**权限：** 需要 polkit 授权（`org.freedesktop.systemd1.manage-unit-files`）

**返回：** 变更列表

---

### GetDefaultTarget

**说明：** 获取当前系统默认启动 target，等同于 `systemctl get-default`。

**参数：** 无

**权限：** 无需特殊权限（需要 SELinux `status` 权限）

**返回：**
| 名称 | 类型 | 说明 |
|------|------|------|
| name | s | 默认 target 名称（如 `graphical.target`） |

---

### PresetAllUnitFiles

**说明：** 对所有已安装单元文件应用预设策略，等同于 `systemctl preset-all`。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| mode | s | 输入 | 预设模式（`full`、`enable-only`、`disable-only`，空字符串为 `full`） |
| runtime | b | 输入 | 是否仅运行时操作 |
| force | b | 输入 | 是否覆盖已有链接 |
| changes | a(sss) | 输出 | 变更列表 |

**权限：** 需要 polkit 授权（`org.freedesktop.systemd1.manage-unit-files`）

**返回：** 变更列表

---

### AddDependencyUnitFiles

**说明：** 向指定单元文件添加依赖关系（在 `.wants/` 或 `.requires/` 目录中创建符号链接），等同于 `systemctl add-wants` 或 `systemctl add-requires`。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| files | as | 输入 | 要添加依赖的单元名称或路径数组 |
| target | s | 输入 | 依赖目标（哪个 target 对这些单元产生依赖） |
| type | s | 输入 | 依赖类型：`Wants` 或 `Requires` |
| runtime | b | 输入 | 是否仅运行时操作 |
| force | b | 输入 | 是否覆盖已有链接 |
| changes | a(sss) | 输出 | 变更列表 |

**权限：** 需要 polkit 授权

**返回：** 变更列表

---

### GetUnitFileLinks

**说明：** 获取禁用单元时会删除的符号链接列表（dry-run 模式），用于查看哪些文件会受到禁用操作的影响。

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

**说明：** 设置 systemd 的退出码（当 systemd 最终退出时使用的返回值）。在容器或用户会话中，控制 systemd 退出时的返回码。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| number | y | 输入 | 退出码（byte，0-255） |

**权限：** 需要 SELinux `exit` 访问权限

**返回：** 无

---

### LookupDynamicUserByName

**说明：** 按用户名查找 systemd 动态分配的用户 UID。动态用户（DynamicUser=yes）是 systemd 为服务临时分配的系统用户。仅在系统管理器实例中有效。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| name | s | 输入 | 动态用户名称 |
| uid | u | 输出 | 用户 UID（uint32） |

**权限：** 无需特殊权限

**返回：** 动态用户的 UID

---

### LookupDynamicUserByUID

**说明：** 按 UID 反查 systemd 动态分配的用户名。与 `LookupDynamicUserByName` 方向相反。仅在系统管理器实例中有效。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| uid | u | 输入 | 用户 UID（uint32） |
| name | s | 输出 | 动态用户名称 |

**权限：** 无需特殊权限

**返回：** 动态用户的用户名

---

### GetDynamicUsers

**说明：** 获取所有当前已实例化（已分配 UID）的动态用户列表。仅在系统管理器实例中有效。

**参数：** 无

**权限：** 无需特殊权限

**返回：** `a(us)` — 动态用户数组，每项为 (UID, 用户名)

---

### DumpUnitFileDescriptorStore

**说明：** 转储指定 service 单元的文件描述符存储（fd store）内容。fd store 允许服务在重启后保留文件描述符。单元必须是 service 类型且已正确加载。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| name | s | 输入 | service 单元名称 |
| entries | a(suuutuusu) | 输出 | fd store 条目数组，每项包含：(fd名称, fd编号, 设备ID major, 设备ID minor, 节点inode, uid, gid, fd类型描述, 只读标志) |

**权限：** 无需特殊权限

**返回：** fd store 条目数组

---

## 属性（Properties）

### 系统信息属性

| 属性 | 类型 | 读写 | 说明 |
|------|------|------|------|
| Version | s | 只读 | systemd 版本号字符串（如 `255`） |
| Features | s | 只读 | 编译时启用的功能特性列表 |
| Virtualization | s | 只读 | 检测到的虚拟化类型（如 `kvm`、`docker`，无则为空字符串） |
| ConfidentialVirtualization | s | 只读 | 检测到的机密虚拟化类型（如 `sev`，无则为空字符串） |
| Architecture | s | 只读 | 系统架构（如 `x86-64`、`aarch64`） |
| Tainted | s | 只读 | 系统污点标记字符串（逗号分隔，描述系统中不受支持的配置） |

### 启动时间戳属性

每个时间戳属性实际展开为两个属性：`XxxTimestamp`（UTC 实时时钟，类型 `t`）和 `XxxTimestampMonotonic`（单调时钟，类型 `t`），单位均为微秒。

| 属性（基础名称） | 读写 | 说明 |
|------|------|------|
| FirmwareTimestamp | 只读 | 固件启动时间点 |
| LoaderTimestamp | 只读 | 引导加载器（bootloader）时间点 |
| KernelTimestamp | 只读 | 内核初始化时间点 |
| InitRDTimestamp | 只读 | initrd 启动时间点 |
| UserspaceTimestamp | 只读 | 用户空间（systemd PID 1）启动时间点 |
| FinishTimestamp | 只读 | 系统启动完成时间点（default.target 激活后） |
| ShutdownStartTimestamp | 只读 | 关机开始时间点 |
| SecurityStartTimestamp | 只读 | 安全模块初始化开始时间点 |
| SecurityFinishTimestamp | 只读 | 安全模块初始化完成时间点 |
| GeneratorsStartTimestamp | 只读 | 单元生成器（generators）开始运行时间点 |
| GeneratorsFinishTimestamp | 只读 | 单元生成器完成时间点 |
| UnitsLoadStartTimestamp | 只读 | 单元加载开始时间点 |
| UnitsLoadFinishTimestamp | 只读 | 单元加载完成时间点 |
| UnitsLoadTimestamp | 只读 | 单元加载用时（该轮次） |
| InitRDSecurityStartTimestamp | 只读 | initrd 安全模块初始化开始时间点 |
| InitRDSecurityFinishTimestamp | 只读 | initrd 安全模块初始化完成时间点 |
| InitRDGeneratorsStartTimestamp | 只读 | initrd 生成器开始时间点 |
| InitRDGeneratorsFinishTimestamp | 只读 | initrd 生成器完成时间点 |
| InitRDUnitsLoadStartTimestamp | 只读 | initrd 单元加载开始时间点 |
| InitRDUnitsLoadFinishTimestamp | 只读 | initrd 单元加载完成时间点 |

### 日志控制属性

| 属性 | 类型 | 读写 | 说明 |
|------|------|------|------|
| LogLevel | s | 读写 | 当前日志级别（如 `info`、`debug`、`warning`、`err`）。写入空字符串恢复默认 |
| LogTarget | s | 读写 | 当前日志目标（如 `journal`、`console`、`kmsg`、`null`）。写入空字符串恢复默认 |

### 统计属性

| 属性 | 类型 | 读写 | 说明 |
|------|------|------|------|
| NNames | u | 只读 | 当前加载的单元（含别名）总数 |
| NFailedUnits | u | 只读 | 当前处于失败状态的单元数（状态变化时发出 PropertiesChanged 信号） |
| NJobs | u | 只读 | 当前排队的作业总数 |
| NInstalledJobs | u | 只读 | 历史上安装的作业总数（累计） |
| NFailedJobs | u | 只读 | 历史上失败的作业总数（累计） |
| TransactionsWithOrderingCycle | at | 只读 | 发现排序循环的事务时间戳集合 |
| Progress | d | 只读 | 系统启动进度（0.0 到 1.0 之间的浮点数） |

### 环境与配置属性

| 属性 | 类型 | 读写 | 说明 |
|------|------|------|------|
| Environment | as | 只读 | systemd 当前的有效环境变量列表（`KEY=VALUE` 格式） |
| ConfirmSpawn | b | 只读 | 是否在启动每个服务前要求确认（调试用途） |
| ShowStatus | b | 只读 | 当前是否在控制台显示启动/停止状态 |
| UnitPath | as | 只读 | systemd 搜索单元文件的路径列表 |
| DefaultStandardOutput | s | 只读 | 单元的默认标准输出目标（如 `journal`） |
| DefaultStandardError | s | 只读 | 单元的默认标准错误输出目标 |

### 看门狗属性

| 属性 | 类型 | 读写 | 说明 |
|------|------|------|------|
| WatchdogDevice | s | 只读 | 当前使用的硬件看门狗设备路径 |
| WatchdogLastPingTimestamp | t | 只读 | 上次看门狗心跳的实时时钟时间戳（微秒） |
| WatchdogLastPingTimestampMonotonic | t | 只读 | 上次看门狗心跳的单调时钟时间戳（微秒） |
| RuntimeWatchdogUSec | t | 读写 | 运行时看门狗超时（微秒）。设为 0 禁用 |
| RuntimeWatchdogPreUSec | t | 读写 | 看门狗预超时时间（微秒），用于提前触发 governor 动作 |
| RuntimeWatchdogPreGovernor | s | 读写 | 看门狗预超时 governor 名称（如 `panic`、`noop`） |
| RebootWatchdogUSec | t | 读写 | 重启过程看门狗超时（微秒），防止重启卡住 |
| KExecWatchdogUSec | t | 读写 | kexec 过程看门狗超时（微秒） |
| ServiceWatchdogs | b | 读写 | 是否为服务启用看门狗机制（全局开关） |

### 系统状态属性

| 属性 | 类型 | 读写 | 说明 |
|------|------|------|------|
| ControlGroup | s | 只读 | systemd 使用的 cgroup 根路径（如 `/`） |
| SystemState | s | 只读 | 系统当前状态（`initializing`、`starting`、`running`、`degraded`、`maintenance`、`stopping`、`offline`、`unknown`） |
| ExitCode | y | 只读 | 当前设置的退出码（byte，通过 SetExitCode 设置） |
| SoftRebootsCount | u | 只读 | 已执行的软重启次数 |

### 默认超时属性

| 属性 | 类型 | 读写 | 说明 |
|------|------|------|------|
| DefaultTimerAccuracyUSec | t | 只读 | 默认定时器精度（微秒） |
| DefaultTimeoutStartUSec | t | 只读 | 服务启动默认超时（微秒） |
| DefaultTimeoutStopUSec | t | 只读 | 服务停止默认超时（微秒） |
| DefaultTimeoutAbortUSec | t | 只读 | 服务中止默认超时（微秒） |
| DefaultDeviceTimeoutUSec | t | 只读 | 设备单元默认超时（微秒） |
| DefaultRestartUSec | t | 只读 | 服务重启间隔默认值（微秒） |
| DefaultStartLimitIntervalUSec | t | 只读 | 启动频率限制的时间窗口（微秒） |
| DefaultStartLimitBurst | u | 只读 | 启动频率限制的最大次数 |

### 默认资源限制属性（RLIMIT）

以下属性均为 `t` 类型（uint64），只读。`RLIMIT_INFINITY` 对应 `(uint64_t)-1`。每种资源限制有两个属性：硬限制（无 `Soft` 后缀）和软限制（带 `Soft` 后缀）。

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
| DefaultTasksMax | t | 只读 | 默认最大任务数（uint64，`(uint64_t)-1` 表示无限制） |
| DefaultOOMPolicy | s | 只读 | OOM 杀手默认策略（`continue`、`stop`、`kill`） |
| DefaultOOMScoreAdjust | i | 只读 | 默认 OOM 评分调整值（int32） |
| DefaultRestrictSUIDSGID | b | 只读 | 是否默认限制 SUID/SGID 位 |
| DefaultMemoryZSwapWriteback | b | 只读 | 是否默认启用 zswap 内存写回 |

### 内存压力属性

| 属性 | 类型 | 读写 | 说明 |
|------|------|------|------|
| DefaultMemoryPressureThresholdUSec | t | 只读 | 默认内存压力阈值（微秒） |
| DefaultMemoryPressureWatch | s | 只读 | 默认内存压力监控模式（`off`、`auto`、`on`、`skip`） |
| DefaultCPUPressureThresholdUSec | t | 只读 | 默认 CPU 压力阈值（微秒） |
| DefaultCPUPressureWatch | s | 只读 | 默认 CPU 压力监控模式 |
| DefaultIOPressureThresholdUSec | t | 只读 | 默认 IO 压力阈值（微秒） |
| DefaultIOPressureWatch | s | 只读 | 默认 IO 压力监控模式 |

### 其他系统属性

| 属性 | 类型 | 读写 | 说明 |
|------|------|------|------|
| TimerSlackNSec | t | 只读 | 进程定时器松弛值（纳秒，从内核读取） |
| CtrlAltDelBurstAction | s | 只读 | 快速按 Ctrl+Alt+Del（burst 模式）时触发的紧急动作（如 `reboot-force`、`poweroff-force`） |

---

## 信号（Signals）

### UnitNew

**说明：** 当新单元被加载到 systemd 中时发出此信号。监听此信号可追踪单元生命周期。

**参数：**
| 名称 | 类型 | 说明 |
|------|------|------|
| id | s | 新单元的 ID（名称，如 `nginx.service`） |
| unit | o | 新单元的 D-Bus 对象路径 |

---

### UnitRemoved

**说明：** 当单元从 systemd 中卸载（unload）时发出此信号。

**参数：**
| 名称 | 类型 | 说明 |
|------|------|------|
| id | s | 被移除的单元 ID |
| unit | o | 被移除单元的 D-Bus 对象路径（路径在信号发出后可能已失效） |

---

### JobNew

**说明：** 当新作业被加入队列时发出此信号。

**参数：**
| 名称 | 类型 | 说明 |
|------|------|------|
| id | u | 新作业的 ID（uint32） |
| job | o | 新作业的 D-Bus 对象路径 |
| unit | s | 此作业关联的单元名称 |

---

### JobRemoved

**说明：** 当作业完成（无论成功或失败）并从队列中移除时发出此信号。通过 `result` 字段可判断作业的最终结果。

**参数：**
| 名称 | 类型 | 说明 |
|------|------|------|
| id | u | 完成的作业 ID（uint32） |
| job | o | 作业的 D-Bus 对象路径（可能已失效） |
| unit | s | 此作业关联的单元名称 |
| result | s | 作业结果：`done`（成功）、`canceled`（已取消）、`timeout`（超时）、`failed`（失败）、`dependency`（依赖失败）、`skipped`（跳过） |

---

### StartupFinished

**说明：** 当系统启动完成（default.target 激活）时发出此信号。包含各启动阶段的时间信息，可用于分析启动性能。

**参数：**
| 名称 | 类型 | 说明 |
|------|------|------|
| firmware | t | 固件阶段耗时（微秒） |
| loader | t | 引导加载器阶段耗时（微秒） |
| kernel | t | 内核初始化耗时（微秒） |
| initrd | t | initrd 阶段耗时（微秒） |
| userspace | t | 用户空间启动耗时（微秒） |
| total | t | 总启动时间（微秒，从固件到启动完成） |

---

### UnitFilesChanged

**说明：** 当系统中的单元文件发生变更（启用、禁用、屏蔽等操作后）时发出此信号。监听此信号可在单元文件配置改变时刷新缓存。

**参数：** 无

---

### Reloading

**说明：** 当 systemd 开始或完成重载（daemon-reload）时发出此信号。可用于追踪重载状态。

**参数：**
| 名称 | 类型 | 说明 |
|------|------|------|
| active | b | true 表示重载开始，false 表示重载完成 |

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
| `(uo)` | (uint32, 对象路径) 结构体 |
| `a(uosos)` | (uint32, 对象路径, 字符串, 对象路径, 字符串) 结构体数组 |
| `a(usssoo)` | 作业信息结构体数组（id, unit, type, state, job_path, unit_path） |
| `a(ssssssouso)` | 单元信息结构体数组（见 ListUnits） |
| `a(sus)` | (字符串, uint32, 字符串) 结构体数组（进程信息） |

### 作业模式（mode 参数）说明

| 模式 | 说明 |
|------|------|
| `replace` | 替换同类型现有作业（最常用，如果已有 start 作业则替换之） |
| `fail` | 若已有冲突作业则使本次调用失败返回错误 |
| `queue` | 将作业加入队列；若已有完全相同的作业，直接返回现有作业而不新建 |
| `isolate` | 仅适用于 target 单元：激活此 target 并停止所有其他激活单元 |
| `flush` | 先取消所有等待中的作业，然后排队新作业 |
| `ignore-dependencies` | 忽略所有依赖关系（Requires、Wants、After、Before 等）直接执行 |
| `ignore-requirements` | 仅忽略 Requires/Wants 依赖，但保留 After/Before 排序依赖 |
| `triggering` | 仅当有其他单元正在等待触发此单元时才排队 |

### 变更列表（a(sss)）结构说明

单元文件操作方法（Enable/Disable/Mask 等）返回的变更列表中，每个 `(sss)` 结构体包含：

| 字段 | 说明 |
|------|------|
| type | 变更类型：`symlink`（创建符号链接）、`unlink`（删除符号链接）、`mkdir`（创建目录）等 |
| path | 操作的目标文件路径 |
| source | 符号链接的源路径（对 `unlink` 类型为空） |

### 权限说明

- **无需权限（Unprivileged）**：标记为 `SD_BUS_VTABLE_UNPRIVILEGED` 的方法，任何用户均可调用（可能受 SELinux 策略约束）
- **polkit 授权**：需要 polkit 策略允许，通常通过 `org.freedesktop.systemd1.*` 动作名控制
- **CAP_SYS_BOOT**：标记为 `SD_BUS_VTABLE_CAPABILITY(CAP_SYS_BOOT)` 的方法，需要进程具有 `CAP_SYS_BOOT` 能力（通常只有 root）
- **SELinux 限制**：独立于权限系统的强制访问控制，影响哪些单元信息对调用方可见
