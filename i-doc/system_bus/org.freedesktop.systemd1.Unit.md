# 接口名称：org.freedesktop.systemd1.Unit

## 接口说明

`org.freedesktop.systemd1.Unit` 是 systemd 中**所有单元类型的基础 D-Bus 接口**。无论是服务（`.service`）、套接字（`.socket`）、目标（`.target`）、挂载点（`.mount`）还是其他单元类型，都会暴露此接口。

通过此接口，客户端可以：

- **查询单元状态**：加载状态、活跃状态、子状态、时间戳等
- **控制单元生命周期**：启动、停止、重启、重载
- **查询依赖关系**：Requires、Wants、After、Before 等依赖项
- **管理 cgroup 资源**：查询内存、CPU、IO 等资源使用情况
- **发送信号**：向单元中的进程发送 POSIX 信号
- **设置运行时属性**：动态修改单元属性

> **注意**：各具体单元类型（如 `org.freedesktop.systemd1.Service`）在此接口基础上还会暴露各自特有的属性和方法。

## 总线名称

`org.freedesktop.systemd1`

## 对象路径

`/org/freedesktop/systemd1/unit/<unit_name>`

其中 `<unit_name>` 是单元名称经过转义后的形式（例如 `nginx.service` 对应路径为 `/org/freedesktop/systemd1/unit/nginx_2eservice`，`.` 被转义为 `_2e`）。

---

## 方法（Methods）

### Start

**说明：** 启动单元。将一个启动作业加入队列，返回作业的 D-Bus 对象路径。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| mode | s | 输入 | 作业入队模式（见下方说明） |
| job | o | 输出 | 新创建的作业对象路径 |

**`mode` 可选值：**

| 值 | 说明 |
|----|------|
| `replace` | 若已有同类型的挂起作业，则替换之（最常用） |
| `fail` | 若已有挂起作业，则失败并返回错误 |
| `queue` | 排队等待，不替换现有作业 |
| `replace-irreversibly` | 替换现有作业，且新作业不可取消 |
| `isolate` | 仅激活本单元，停止其他所有非依赖单元（仅对启动操作有效） |
| `flush` | 清除所有挂起作业后再入队 |
| `ignore-dependencies` | 忽略所有依赖关系直接启动（危险操作，仅用于调试） |
| `ignore-requirements` | 忽略 Requires 类依赖，但仍处理排序依赖 |
| `triggering` | 仅在有触发作业时才入队 |
| `restart-dependencies` | 重启所有依赖本单元的其他单元 |

**权限：** 通常需要 `org.freedesktop.systemd1.manage-units` polkit 权限

**示例（busctl）：**
```
busctl call org.freedesktop.systemd1 \
  /org/freedesktop/systemd1/unit/nginx_2eservice \
  org.freedesktop.systemd1.Unit \
  Start s replace
```

---

### Stop

**说明：** 停止单元。将一个停止作业加入队列。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| mode | s | 输入 | 作业入队模式（同 `Start`） |
| job | o | 输出 | 新创建的作业对象路径 |

**权限：** 通常需要 `org.freedesktop.systemd1.manage-units` polkit 权限

---

### Reload

**说明：** 重载单元配置（不重启进程）。单元必须支持重载操作（`CanReload` 为 `true`），否则返回错误。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| mode | s | 输入 | 作业入队模式（同 `Start`） |
| job | o | 输出 | 新创建的作业对象路径 |

**权限：** 通常需要 `org.freedesktop.systemd1.manage-units` polkit 权限

---

### Restart

**说明：** 重启单元。若单元正在运行则先停止再启动；若未运行则直接启动。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| mode | s | 输入 | 作业入队模式（同 `Start`） |
| job | o | 输出 | 新创建的作业对象路径 |

**权限：** 通常需要 `org.freedesktop.systemd1.manage-units` polkit 权限

---

### TryRestart

**说明：** 尝试重启单元。仅当单元**当前正在运行**时才重启，若单元处于 inactive 状态则不执行任何操作。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| mode | s | 输入 | 作业入队模式（同 `Start`） |
| job | o | 输出 | 新创建的作业对象路径 |

**权限：** 通常需要 `org.freedesktop.systemd1.manage-units` polkit 权限

---

### ReloadOrRestart

**说明：** 若单元支持重载（`CanReload` 为 `true`）则重载，否则重启。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| mode | s | 输入 | 作业入队模式（同 `Start`） |
| job | o | 输出 | 新创建的作业对象路径 |

**权限：** 通常需要 `org.freedesktop.systemd1.manage-units` polkit 权限

---

### ReloadOrTryRestart

**说明：** 若单元支持重载则重载；否则仅当单元正在运行时才重启（结合了 `Reload` 和 `TryRestart` 的语义）。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| mode | s | 输入 | 作业入队模式（同 `Start`） |
| job | o | 输出 | 新创建的作业对象路径 |

**权限：** 通常需要 `org.freedesktop.systemd1.manage-units` polkit 权限

---

### EnqueueJob

**说明：** 向单元入队一个作业，并返回详细的作业信息，包括受影响的相关作业列表。相比 `Start`/`Stop` 等方法，此方法提供更丰富的返回信息。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| job_type | s | 输入 | 作业类型（`start`、`stop`、`reload`、`restart`、`try-restart`、`reload-or-restart` 等） |
| job_mode | s | 输入 | 作业入队模式（同 `Start` 的 `mode` 参数） |
| job_id | u | 输出 | 新作业的数字 ID |
| job_path | o | 输出 | 新作业的 D-Bus 对象路径 |
| unit_id | s | 输出 | 目标单元的 ID（名称字符串） |
| unit_path | o | 输出 | 目标单元的 D-Bus 对象路径 |
| job_type | s | 输出 | 实际入队的作业类型（可能与输入不同） |
| affected_jobs | a(uosos) | 输出 | 受影响的关联作业列表，每项包含：作业 ID、作业路径、单元 ID、单元路径、作业类型 |

**权限：** 通常需要 `org.freedesktop.systemd1.manage-units` polkit 权限

---

### Kill

**说明：** 向单元中的进程发送指定信号。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| whom | s | 输入 | 信号发送目标（见下方说明） |
| signal | i | 输入 | 信号编号（如 `15` 表示 `SIGTERM`，`9` 表示 `SIGKILL`） |

**`whom` 可选值：**

| 值 | 说明 |
|----|------|
| `main` | 仅向主进程（main process）发送信号 |
| `control` | 仅向控制进程（control process）发送信号 |
| `all` | 向 cgroup 中所有进程发送信号 |

**权限：** 通常需要 `org.freedesktop.systemd1.manage-units` polkit 权限

**示例（发送 SIGTERM 到所有进程）：**
```
busctl call org.freedesktop.systemd1 \
  /org/freedesktop/systemd1/unit/nginx_2eservice \
  org.freedesktop.systemd1.Unit \
  Kill si all 15
```

---

### KillSubgroup

**说明：** 向单元 cgroup 中特定子组的进程发送信号。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| whom | s | 输入 | 信号发送目标（同 `Kill`） |
| subgroup | s | 输入 | 子 cgroup 路径（相对于单元 cgroup 的相对路径） |
| signal | i | 输入 | 信号编号 |

**权限：** 通常需要 `org.freedesktop.systemd1.manage-units` polkit 权限

---

### QueueSignal

**说明：** 向单元进程入队一个 POSIX 实时信号（`SIGRTMIN` 至 `SIGRTMAX`），可携带附加整数值。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| whom | s | 输入 | 信号发送目标（同 `Kill`） |
| signal | i | 输入 | 实时信号编号（`SIGRTMIN`=34 至 `SIGRTMAX`=64） |
| value | i | 输入 | 随信号附带的整数值（通过 `sigqueue(3)` 传递） |

**权限：** 通常需要 `org.freedesktop.systemd1.manage-units` polkit 权限

---

### ResetFailed

**说明：** 将单元的失败状态重置为 inactive（未激活）。当单元处于 `failed` 状态时，可调用此方法清除失败记录，使单元可以重新启动。

**参数：** 无

**权限：** 通常需要 `org.freedesktop.systemd1.manage-units` polkit 权限

**示例：**
```
busctl call org.freedesktop.systemd1 \
  /org/freedesktop/systemd1/unit/nginx_2eservice \
  org.freedesktop.systemd1.Unit \
  ResetFailed
```

---

### SetProperties

**说明：** 在运行时动态设置单元属性。可设置的属性取决于单元类型（如 `MemoryMax`、`CPUQuota` 等资源限制属性）。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| runtime | b | 输入 | 若为 `true`，属性修改仅在本次运行期间生效（重启 systemd 后失效）；若为 `false`，则持久化到磁盘 |
| properties | a(sv) | 输入 | 属性列表，每项为（属性名, 属性值）的键值对，值为变体类型（variant） |

**权限：** 通常需要 `org.freedesktop.systemd1.manage-units` polkit 权限

**示例（设置内存上限为 512MB）：**
```
busctl call org.freedesktop.systemd1 \
  /org/freedesktop/systemd1/unit/nginx_2eservice \
  org.freedesktop.systemd1.Unit \
  SetProperties ba\(sv\) true 1 MemoryMax t 536870912
```

---

### Ref

**说明：** 增加单元的引用计数。持有引用可防止单元因 `StopWhenUnneeded=yes` 而被自动停止。调用方断开连接时引用会自动释放。

**参数：** 无

**权限：** 通常需要 `org.freedesktop.systemd1.manage-units` polkit 权限

---

### Unref

**说明：** 减少单元的引用计数（与 `Ref` 配对使用）。

**参数：** 无

**权限：** 通常需要 `org.freedesktop.systemd1.manage-units` polkit 权限

---

### Clean

**说明：** 清理单元关联的资源（如运行时目录、日志、缓存等）。单元必须处于 inactive 或 failed 状态才能执行清理。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| mask | as | 输入 | 要清理的资源类型列表 |

**`mask` 可选值：**

| 值 | 说明 |
|----|------|
| `runtime` | 清理运行时目录（`RuntimeDirectory=`） |
| `cache` | 清理缓存目录（`CacheDirectory=`） |
| `logs` | 清理日志目录（`LogsDirectory=`） |
| `state` | 清理状态目录（`StateDirectory=`） |
| `configuration` | 清理配置目录（`ConfigurationDirectory=`） |
| `fdstore` | 清理文件描述符存储 |
| `all` | 清理所有以上资源 |

**权限：** 通常需要 `org.freedesktop.systemd1.manage-units` polkit 权限

---

### Freeze

**说明：** 冻结单元，暂停其 cgroup 中的所有进程（通过 cgroup freezer 机制）。冻结的单元保持活跃状态，但所有进程均被暂停执行。

**参数：** 无

**权限：** 通常需要 `org.freedesktop.systemd1.manage-units` polkit 权限

---

### Thaw

**说明：** 解冻单元，恢复其 cgroup 中所有被暂停进程的执行（与 `Freeze` 配对使用）。

**参数：** 无

**权限：** 通常需要 `org.freedesktop.systemd1.manage-units` polkit 权限

---

### GetProcesses

**说明：** 获取单元 cgroup 中当前所有进程的列表。（属于 cgroup 扩展 vtable，需要单元关联 cgroup）

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| processes | a(sus) | 输出 | 进程列表，每项包含：cgroup 子路径（s）、PID（u）、进程名（s） |

**权限：** 通常需要 `org.freedesktop.systemd1.manage-units` polkit 权限

**示例：**
```
busctl call org.freedesktop.systemd1 \
  /org/freedesktop/systemd1/unit/nginx_2eservice \
  org.freedesktop.systemd1.Unit \
  GetProcesses
```

---

### AttachProcesses

**说明：** 将指定 PID 的进程迁移到单元的 cgroup（或其子 cgroup）中。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| subcgroup | s | 输入 | 目标子 cgroup 路径（相对于单元 cgroup；传入空字符串表示单元根 cgroup） |
| pids | au | 输入 | 要迁移的 PID 列表（uint32 数组） |

**权限：** 通常需要 `org.freedesktop.systemd1.manage-units` polkit 权限

---

### RemoveSubgroup

**说明：** 移除单元下的一个子 cgroup。

**参数：**
| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| subcgroup | s | 输入 | 要移除的子 cgroup 路径（相对于单元 cgroup） |
| flags | t | 输入 | 标志位（目前保留，传入 `0`） |

**权限：** 通常需要 `org.freedesktop.systemd1.manage-units` polkit 权限

---

## 属性（Properties）

### 标识与命名

| 属性名 | 类型 | 说明 |
|--------|------|------|
| `Id` | `s` | 单元的主要名称（如 `nginx.service`） |
| `Names` | `as` | 单元的所有名称，包括主名称和所有别名 |
| `Following` | `s` | 若本单元跟随（合并到）另一个单元，则为那个单元的名称；否则为空字符串 |
| `Description` | `s` | 单元的文字描述（对应单元文件中的 `Description=`） |
| `Documentation` | `as` | 文档 URL 列表（对应 `Documentation=`，支持 `http://`、`https://`、`man:` 等格式） |
| `AccessSELinuxContext` | `s` | 访问此单元所需的 SELinux 安全上下文（为空表示无限制） |

---

### 加载与状态

| 属性名 | 类型 | 说明 |
|--------|------|------|
| `LoadState` | `s` | 单元文件的加载状态 |
| `ActiveState` | `s` | 单元的活跃状态 |
| `FreezerState` | `s` | 单元的冻结状态 |
| `SubState` | `s` | 更细粒度的子状态（依单元类型而异） |
| `FragmentPath` | `s` | 主单元文件的绝对路径（如 `/lib/systemd/system/nginx.service`） |
| `SourcePath` | `s` | 若单元文件由其他文件生成（如 SysV init 脚本），则为源文件路径 |
| `DropInPaths` | `as` | 所有 drop-in 覆盖文件的路径列表 |
| `UnitFileState` | `s` | 单元文件的启用状态 |
| `UnitFilePreset` | `s` | 预设策略状态（`enabled` 或 `disabled`） |

**`LoadState` 可能值：**

| 值 | 说明 |
|----|------|
| `stub` | 单元尚未完整加载（仅作为依赖项存在） |
| `loaded` | 已成功加载单元文件 |
| `not-found` | 找不到单元文件 |
| `bad-setting` | 单元文件存在但包含无效配置 |
| `error` | 加载时发生其他错误 |
| `merged` | 已合并到另一个单元（见 `Following`） |
| `masked` | 单元文件被屏蔽（mask） |

**`ActiveState` 可能值：**

| 值 | 说明 |
|----|------|
| `active` | 单元正在运行/已激活 |
| `reloading` | 单元正在重载配置 |
| `inactive` | 单元已停止，处于非活跃状态 |
| `failed` | 单元以失败状态停止 |
| `activating` | 单元正在启动过程中 |
| `deactivating` | 单元正在停止过程中 |
| `maintenance` | 单元处于维护状态 |

**`FreezerState` 可能值：**

| 值 | 说明 |
|----|------|
| `running` | 进程正常运行（未冻结） |
| `freezing` | 正在冻结过程中 |
| `frozen` | 已冻结（所有进程暂停） |
| `thawing` | 正在解冻过程中 |

**`UnitFileState` 可能值：**

| 值 | 说明 |
|----|------|
| `enabled` | 已通过符号链接启用 |
| `enabled-runtime` | 已启用，但仅对本次运行有效（`/run` 中的符号链接） |
| `linked` | 通过 `link` 命令链接到非标准位置 |
| `linked-runtime` | 临时链接 |
| `alias` | 为另一个单元的别名 |
| `masked` | 已屏蔽（链接到 `/dev/null`） |
| `masked-runtime` | 临时屏蔽 |
| `static` | 无 `[Install]` 节，不可通过 enable/disable 控制 |
| `disabled` | 未启用 |
| `indirect` | 通过其他单元间接启用 |
| `generated` | 由生成器（generator）生成 |
| `transient` | 运行时动态创建的临时单元 |
| `bad` | 单元文件无效 |

---

### 状态变更时间戳

所有时间戳均以 **微秒**（µs）为单位，使用 `uint64`（`t`）类型。

| 属性名 | 时钟类型 | 说明 |
|--------|----------|------|
| `StateChangeTimestamp` | 实时时钟（CLOCK_REALTIME） | 活跃状态最后一次变更的时间 |
| `StateChangeTimestampMonotonic` | 单调时钟（CLOCK_MONOTONIC） | 同上（单调时钟版本） |
| `InactiveExitTimestamp` | 实时时钟 | 上次从 `inactive` 状态退出的时间（即开始激活的时间） |
| `InactiveExitTimestampMonotonic` | 单调时钟 | 同上（单调时钟版本） |
| `ActiveEnterTimestamp` | 实时时钟 | 上次进入 `active` 状态的时间 |
| `ActiveEnterTimestampMonotonic` | 单调时钟 | 同上（单调时钟版本） |
| `ActiveExitTimestamp` | 实时时钟 | 上次从 `active` 状态退出的时间 |
| `ActiveExitTimestampMonotonic` | 单调时钟 | 同上（单调时钟版本） |
| `InactiveEnterTimestamp` | 实时时钟 | 上次进入 `inactive` 状态的时间 |
| `InactiveEnterTimestampMonotonic` | 单调时钟 | 同上（单调时钟版本） |

> **提示**：时间戳为 `0` 表示该状态转换尚未发生过。可使用 `date -d @$((timestamp/1000000))` 将微秒时间戳转换为可读格式。

---

### 依赖关系

以下属性列出单元之间的各类依赖关系，均为字符串数组（`as`）。

#### 正向依赖

| 属性名 | 对应单元文件选项 | 说明 |
|--------|----------------|------|
| `Requires` | `Requires=` | 强依赖：本单元激活时，列出的单元也必须被激活；若任意列出单元停止，本单元也会停止 |
| `Requisite` | `Requisite=` | 前置依赖：本单元激活时，列出的单元必须**已经**处于活跃状态，否则本单元立即失败 |
| `Wants` | `Wants=` | 弱依赖：本单元激活时尝试激活列出的单元，但即使失败也不影响本单元 |
| `BindsTo` | `BindsTo=` | 绑定依赖：比 `Requires` 更严格，列出单元进入 inactive 时本单元也立即停止 |
| `PartOf` | `PartOf=` | 从属关系：本单元跟随列出单元的停止和重启操作（但不触发列出单元的激活） |
| `Upholds` | `Upholds=` | 维持关系：systemd 会持续确保列出单元处于活跃状态（若停止则重新启动） |
| `Conflicts` | `Conflicts=` | 冲突关系：本单元启动时，列出的单元会被停止（反之亦然） |
| `Before` | `Before=` | 排序：本单元在列出的单元**之前**启动 |
| `After` | `After=` | 排序：本单元在列出的单元**之后**启动 |

#### 反向依赖（由其他单元指向本单元）

| 属性名 | 说明 |
|--------|------|
| `RequiredBy` | 哪些单元通过 `Requires=` 依赖本单元 |
| `RequisiteOf` | 哪些单元通过 `Requisite=` 依赖本单元 |
| `WantedBy` | 哪些单元通过 `Wants=` 依赖本单元 |
| `BoundBy` | 哪些单元通过 `BindsTo=` 绑定到本单元 |
| `UpheldBy` | 哪些单元通过 `Upholds=` 维持本单元 |
| `ConsistsOf` | 哪些单元是本单元的组成部分（`PartOf=` 的反向） |
| `ConflictedBy` | 哪些单元与本单元冲突 |

#### 其他依赖关系

| 属性名 | 说明 |
|--------|------|
| `OnSuccess` | 本单元成功完成后自动激活的单元列表 |
| `OnSuccessOf` | 哪些单元成功完成后会激活本单元 |
| `OnFailure` | 本单元进入 `failed` 状态后自动激活的单元列表 |
| `OnFailureOf` | 哪些单元失败后会激活本单元 |
| `Triggers` | 本单元触发的单元列表（如 `.socket` 触发 `.service`） |
| `TriggeredBy` | 哪些单元触发本单元 |
| `PropagatesReloadTo` | 本单元重载时，也会触发重载的其他单元 |
| `ReloadPropagatedFrom` | 从哪些单元接收重载传播 |
| `PropagatesStopTo` | 本单元停止时，也会触发停止的其他单元 |
| `StopPropagatedFrom` | 从哪些单元接收停止传播 |
| `JoinsNamespaceOf` | 与哪些单元共享 Linux 命名空间 |
| `SliceOf` | 本单元属于哪些切片单元（`Slice=` 的反向） |
| `RequiresMountsFor` | 本单元运行所需的挂载点路径列表（对应 `RequiresMountsFor=`） |
| `WantsMountsFor` | 本单元想要的挂载点路径列表（对应 `WantsMountsFor=`） |

---

### 能力标志

| 属性名 | 类型 | 说明 |
|--------|------|------|
| `CanStart` | `b` | 是否可以手动启动（`false` 表示如模板单元等不可直接启动） |
| `CanStop` | `b` | 是否可以手动停止 |
| `CanReload` | `b` | 是否支持重载操作（如 `nginx -s reload`） |
| `CanIsolate` | `b` | 是否可以作为隔离目标（对应 `AllowIsolate=`） |
| `CanClean` | `as` | 可清理的资源类型列表（如 `["runtime", "cache"]`，空列表表示不支持） |
| `CanFreeze` | `b` | 是否支持冻结/解冻操作（需要 cgroup freezer 支持） |
| `CanLiveMount` | `b` | 是否支持实时挂载操作 |

---

### 行为控制

| 属性名 | 类型 | 说明 |
|--------|------|------|
| `StopWhenUnneeded` | `b` | 对应 `StopWhenUnneeded=`：当没有其他活跃单元依赖本单元时自动停止 |
| `RefuseManualStart` | `b` | 对应 `RefuseManualStart=`：拒绝手动启动（只能通过依赖关系间接启动） |
| `RefuseManualStop` | `b` | 对应 `RefuseManualStop=`：拒绝手动停止 |
| `AllowIsolate` | `b` | 对应 `AllowIsolate=`：允许以隔离模式激活本单元 |
| `DefaultDependencies` | `b` | 对应 `DefaultDependencies=`：是否添加默认依赖关系（通常为 `true`） |
| `SurviveFinalKillSignal` | `b` | 是否在系统关机最终 kill 信号后仍然存活 |
| `OnSuccessJobMode` | `s` | `OnSuccess=` 触发时的作业入队模式 |
| `OnFailureJobMode` | `s` | `OnFailure=` 触发时的作业入队模式（如 `replace`、`fail`） |
| `IgnoreOnIsolate` | `b` | 对应 `IgnoreOnIsolate=`：执行隔离操作时忽略（不停止）本单元 |
| `CollectMode` | `s` | 对应 `CollectMode=`：单元垃圾回收模式（`inactive` 或 `inactive-or-failed`） |

---

### 作业与超时

| 属性名 | 类型 | 说明 |
|--------|------|------|
| `Job` | `(uo)` | 当前关联的作业；若无正在进行的作业，则为 `(0, "/")` |
| `NeedDaemonReload` | `b` | 单元文件自上次 `daemon-reload` 后是否被修改（需要重新加载） |
| `Markers` | `as` | 当前激活的标记列表（如 `needs-reload`、`needs-restart`） |
| `JobTimeoutUSec` | `t` | 对应 `JobTimeoutSec=`：作业整体超时时间（微秒），`UINT64_MAX` 表示无限制 |
| `JobRunningTimeoutUSec` | `t` | 对应 `JobRunningTimeoutSec=`：作业运行超时（微秒） |
| `JobTimeoutAction` | `s` | 超过作业超时后的动作（`none`、`reboot`、`reboot-force`、`reboot-immediate`、`poweroff` 等） |
| `JobTimeoutRebootArgument` | `s` | 超时触发重启时传递的内核参数 |

---

### 启动频率限制

| 属性名 | 类型 | 说明 |
|--------|------|------|
| `StartLimitIntervalUSec` | `t` | 对应 `StartLimitIntervalSec=`：启动频率限制的时间窗口（微秒） |
| `StartLimitBurst` | `u` | 对应 `StartLimitBurst=`：在时间窗口内允许的最大启动次数 |
| `StartLimitAction` | `s` | 超过启动频率限制后的动作（同 `JobTimeoutAction` 的可选值） |
| `RebootArgument` | `s` | 触发重启动作时的内核参数 |

---

### 失败与成功动作

| 属性名 | 类型 | 说明 |
|--------|------|------|
| `FailureAction` | `s` | 对应 `FailureAction=`：单元失败后执行的系统动作 |
| `FailureActionExitStatus` | `i` | 失败动作的退出码（`-1` 表示未设置） |
| `SuccessAction` | `s` | 对应 `SuccessAction=`：单元成功完成后执行的系统动作 |
| `SuccessActionExitStatus` | `i` | 成功动作的退出码（`-1` 表示未设置） |

**动作可选值：**

| 值 | 说明 |
|----|------|
| `none` | 不执行任何动作 |
| `reboot` | 优雅重启系统 |
| `reboot-force` | 强制重启（跳过常规关机流程） |
| `reboot-immediate` | 立即重启（直接调用 `reboot(2)` 系统调用） |
| `poweroff` | 优雅关机 |
| `poweroff-force` | 强制关机 |
| `poweroff-immediate` | 立即关机 |
| `exit` | 退出 systemd（用于非 init 实例） |
| `exit-force` | 强制退出 |
| `soft-reboot` | 软重启（仅重新执行用户空间） |
| `kexec` | 通过 kexec 重启内核 |
| `halt` | 停止系统 |

---

### 条件与断言

条件（Condition）和断言（Assert）的区别：条件检查失败时单元被跳过（skip），断言检查失败时单元进入 failed 状态。

| 属性名 | 类型 | 说明 |
|--------|------|------|
| `ConditionResult` | `b` | 上次条件检查的最终结果（`true` 表示所有条件均满足） |
| `AssertResult` | `b` | 上次断言检查的最终结果 |
| `ConditionTimestamp` | `t` | 上次条件检查的实时时钟时间戳（微秒） |
| `ConditionTimestampMonotonic` | `t` | 上次条件检查的单调时钟时间戳（微秒） |
| `AssertTimestamp` | `t` | 上次断言检查的实时时钟时间戳（微秒） |
| `AssertTimestampMonotonic` | `t` | 上次断言检查的单调时钟时间戳（微秒） |
| `Conditions` | `a(sbbsi)` | 条件详情列表（见下方说明） |
| `Asserts` | `a(sbbsi)` | 断言详情列表（见下方说明） |

**`Conditions` / `Asserts` 结构体字段（`(sbbsi)`）：**

| 索引 | 类型 | 说明 |
|------|------|------|
| 0 | `s` | 条件/断言类型名（如 `ConditionPathExists`、`AssertFileIsExecutable`） |
| 1 | `b` | 是否为触发器（trigger）：`true` 表示此条件可以单独触发整体结果 |
| 2 | `b` | 是否取反（negate）：`true` 表示条件前有 `!` 前缀 |
| 3 | `s` | 条件参数（如路径名、内核版本等） |
| 4 | `i` | 检查结果：`0` 表示未检查，`1` 表示满足，`-1` 表示不满足 |

---

### 其他属性

| 属性名 | 类型 | 说明 |
|--------|------|------|
| `LoadError` | `(ss)` | 加载错误信息：`(错误名称, 错误消息)`；无错误时为 `("", "")` |
| `Transient` | `b` | 是否为通过 D-Bus API 动态创建的临时单元 |
| `Perpetual` | `b` | 是否为永久单元（如 `init.scope`），永久单元无法被停止或删除 |
| `InvocationID` | `ay` | 当前调用的 128 位 UUID（16 字节数组）；每次单元启动时重新生成 |
| `Refs` | `as` | 当前持有本单元引用（通过 `Ref()` 方法）的 D-Bus 客户端列表 |
| `ActivationDetails` | `a(ss)` | 激活详情键值对（如触发本次激活的原因、触发者等） |
| `DebugInvocation` | `b` | 本次调用是否为调试模式调用 |

---

## cgroup 资源属性

以下属性仅在单元关联了 cgroup 时才可用（通常是 service、scope、slice 等类型的单元）。

### cgroup 基本信息

| 属性名 | 类型 | 说明 |
|--------|------|------|
| `Slice` | `s` | 本单元所属的切片单元名称（如 `system.slice`） |
| `ControlGroup` | `s` | 本单元在 cgroup v2 层级中的路径（如 `/system.slice/nginx.service`） |
| `ControlGroupId` | `t` | cgroup 的内核数字 ID（inode 编号） |

### 内存资源

| 属性名 | 类型 | 说明 |
|--------|------|------|
| `MemoryCurrent` | `t` | 当前内存使用量（字节），`UINT64_MAX` 表示无数据 |
| `MemoryPeak` | `t` | 本次调用期间的内存峰值（字节） |
| `MemorySwapCurrent` | `t` | 当前 swap 使用量（字节） |
| `MemorySwapPeak` | `t` | 本次调用期间的 swap 峰值（字节） |
| `MemoryZSwapCurrent` | `t` | 当前 zswap 使用量（字节，需要内核 zswap 支持） |
| `MemoryAvailable` | `t` | 在当前内存限制下还可使用的内存（字节） |
| `EffectiveMemoryMax` | `t` | 有效内存上限（考虑父切片限制后的实际上限，字节） |
| `EffectiveMemoryHigh` | `t` | 有效内存高水位（字节） |

### CPU 资源

| 属性名 | 类型 | 说明 |
|--------|------|------|
| `CPUUsageNSec` | `t` | 单元进程已使用的 CPU 时间（纳秒），`UINT64_MAX` 表示无数据 |
| `EffectiveCPUs` | `ay` | 允许使用的 CPU 位掩码（字节数组，`cpuset` 格式） |
| `EffectiveMemoryNodes` | `ay` | 允许使用的 NUMA 内存节点位掩码（字节数组） |

### 任务数

| 属性名 | 类型 | 说明 |
|--------|------|------|
| `TasksCurrent` | `t` | 当前 cgroup 中的任务（线程/进程）数量，`UINT64_MAX` 表示无数据 |
| `EffectiveTasksMax` | `t` | 有效任务数上限（考虑父切片限制后的实际上限） |

### 网络流量

| 属性名 | 类型 | 说明 |
|--------|------|------|
| `IPIngressBytes` | `t` | 入站 IP 流量总字节数，`UINT64_MAX` 表示无数据（需要 eBPF 支持） |
| `IPIngressPackets` | `t` | 入站 IP 数据包总数 |
| `IPEgressBytes` | `t` | 出站 IP 流量总字节数 |
| `IPEgressPackets` | `t` | 出站 IP 数据包总数 |

### 磁盘 IO

| 属性名 | 类型 | 说明 |
|--------|------|------|
| `IOReadBytes` | `t` | cgroup 读取的总字节数，`UINT64_MAX` 表示无数据 |
| `IOReadOperations` | `t` | cgroup 读取操作的总次数 |
| `IOWriteBytes` | `t` | cgroup 写入的总字节数 |
| `IOWriteOperations` | `t` | cgroup 写入操作的总次数 |

### OOM 统计

| 属性名 | 类型 | 说明 |
|--------|------|------|
| `OOMKills` | `t` | 由内核 OOM Killer 在本 cgroup 中执行的 kill 次数 |
| `ManagedOOMKills` | `t` | 由 systemd 的 `systemd-oomd` 管理的 OOM kill 次数 |

---

## 信号（Signals）

`org.freedesktop.systemd1.Unit` 接口本身不定义信号。单元状态变更通过标准 D-Bus 属性变更机制（`org.freedesktop.DBus.Properties.PropertiesChanged`）通知客户端。

### 监听属性变更示例

```bash
# 监听 nginx.service 的属性变更
busctl monitor org.freedesktop.systemd1 \
  --match "type='signal',interface='org.freedesktop.DBus.Properties',path='/org/freedesktop/systemd1/unit/nginx_2eservice'"
```

---

## D-Bus 类型对照表

| 类型符号 | D-Bus 类型 | 说明 |
|----------|-----------|------|
| `s` | STRING | UTF-8 字符串 |
| `b` | BOOLEAN | 布尔值（`true`/`false`） |
| `u` | UINT32 | 32 位无符号整数 |
| `i` | INT32 | 32 位有符号整数 |
| `t` | UINT64 | 64 位无符号整数（常用于时间戳和字节数） |
| `o` | OBJECT_PATH | D-Bus 对象路径 |
| `h` | UNIX_FD | Unix 文件描述符 |
| `as` | ARRAY of STRING | 字符串数组 |
| `au` | ARRAY of UINT32 | 32 位无符号整数数组 |
| `ay` | ARRAY of BYTE | 字节数组 |
| `(uo)` | STRUCT(UINT32, OBJECT_PATH) | 结构体 |
| `(ss)` | STRUCT(STRING, STRING) | 结构体 |
| `a(ss)` | ARRAY of STRUCT(STRING, STRING) | 结构体数组 |
| `a(sv)` | ARRAY of STRUCT(STRING, VARIANT) | 属性键值对数组 |
| `a(sus)` | ARRAY of STRUCT(STRING, UINT32, STRING) | 进程信息结构体数组 |
| `a(sbbsi)` | ARRAY of STRUCT(STRING, BOOLEAN, BOOLEAN, STRING, INT32) | 条件/断言结构体数组 |
| `a(uosos)` | ARRAY of STRUCT(UINT32, OBJECT_PATH, STRING, OBJECT_PATH, STRING) | 受影响作业结构体数组 |
| `v` | VARIANT | 变体类型（可包含任意类型） |

---

## 使用示例

### 使用 busctl 查询单元状态

```bash
# 查询 nginx.service 的活跃状态
busctl get-property org.freedesktop.systemd1 \
  /org/freedesktop/systemd1/unit/nginx_2eservice \
  org.freedesktop.systemd1.Unit \
  ActiveState

# 查询所有依赖关系
busctl get-property org.freedesktop.systemd1 \
  /org/freedesktop/systemd1/unit/nginx_2eservice \
  org.freedesktop.systemd1.Unit \
  Requires

# 查询内存使用情况
busctl get-property org.freedesktop.systemd1 \
  /org/freedesktop/systemd1/unit/nginx_2eservice \
  org.freedesktop.systemd1.Unit \
  MemoryCurrent

# 一次性列出所有属性
busctl introspect org.freedesktop.systemd1 \
  /org/freedesktop/systemd1/unit/nginx_2eservice \
  org.freedesktop.systemd1.Unit
```

### 使用 Python + dasbus 控制单元

```python
from dasbus.connection import SystemMessageBus

bus = SystemMessageBus()
systemd = bus.get_proxy(
    "org.freedesktop.systemd1",
    "/org/freedesktop/systemd1/unit/nginx_2eservice"
)

# 获取活跃状态
print("ActiveState:", systemd.ActiveState)

# 获取内存使用量（字节）
mem = systemd.MemoryCurrent
print(f"MemoryCurrent: {mem / 1024 / 1024:.2f} MB")

# 重启单元
job_path = systemd.Restart("replace")
print("Job:", job_path)
```

### 使用 Python + dbus-python 设置属性

```python
import dbus

bus = dbus.SystemBus()
obj = bus.get_object(
    "org.freedesktop.systemd1",
    "/org/freedesktop/systemd1/unit/nginx_2eservice"
)
unit = dbus.Interface(obj, "org.freedesktop.systemd1.Unit")

# 运行时设置内存上限为 512MB
unit.SetProperties(
    True,   # runtime=True（临时，重启后失效）
    [("MemoryMax", dbus.UInt64(512 * 1024 * 1024))]
)
```

### 使用 gdbus 命令行工具

```bash
# 启动单元
gdbus call --system \
  --dest org.freedesktop.systemd1 \
  --object-path /org/freedesktop/systemd1/unit/nginx_2eservice \
  --method org.freedesktop.systemd1.Unit.Start \
  "replace"

# 获取进程列表
gdbus call --system \
  --dest org.freedesktop.systemd1 \
  --object-path /org/freedesktop/systemd1/unit/nginx_2eservice \
  --method org.freedesktop.systemd1.Unit.GetProcesses
```

---

## 对象路径转义规则

单元名称中的特殊字符在转换为 D-Bus 对象路径时需进行转义，规则如下：

- 字母（`a-z`、`A-Z`）和数字（`0-9`）保持不变
- `-`（连字符）保持不变
- 其他字符（包括 `.`、`@`、`:`、`_` 等）转义为 `_XX`（`XX` 为十六进制 ASCII 码）

**常见转义示例：**

| 单元名称 | 对象路径后缀 |
|----------|-------------|
| `nginx.service` | `nginx_2eservice` |
| `sshd.service` | `sshd_2eservice` |
| `dbus.service` | `dbus_2eservice` |
| `getty@tty1.service` | `getty_40tty1_2eservice` |
| `dev-sda1.device` | `dev_2dsda1_2edevice` |
| `home.mount` | `home_2emount` |
| `user@1000.service` | `user_401000_2eservice` |

可使用 `systemd-escape` 命令进行转换：

```bash
# 将单元名转为对象路径格式
systemd-escape --path nginx.service
# 输出：nginx_2eservice
```

---

## 相关接口

| 接口名称 | 说明 |
|----------|------|
| `org.freedesktop.systemd1.Manager` | systemd 主管理接口，提供单元枚举、全局控制等功能 |
| `org.freedesktop.systemd1.Service` | Service 单元特有属性（继承自 Unit 接口） |
| `org.freedesktop.systemd1.Socket` | Socket 单元特有属性 |
| `org.freedesktop.systemd1.Target` | Target 单元特有属性 |
| `org.freedesktop.systemd1.Mount` | Mount 单元特有属性 |
| `org.freedesktop.systemd1.Automount` | Automount 单元特有属性 |
| `org.freedesktop.systemd1.Swap` | Swap 单元特有属性 |
| `org.freedesktop.systemd1.Timer` | Timer 单元特有属性 |
| `org.freedesktop.systemd1.Path` | Path 单元特有属性 |
| `org.freedesktop.systemd1.Slice` | Slice 单元特有属性 |
| `org.freedesktop.systemd1.Scope` | Scope 单元特有属性 |
| `org.freedesktop.systemd1.Job` | 作业对象接口 |
| `org.freedesktop.DBus.Properties` | 标准 D-Bus 属性接口（支持 `Get`、`Set`、`GetAll`、`PropertiesChanged`） |
| `org.freedesktop.DBus.Introspectable` | 标准 D-Bus 内省接口 |
| `org.freedesktop.DBus.Peer` | 标准 D-Bus 节点接口 |

---

## 参考资料

- [systemd D-Bus API 官方文档](https://www.freedesktop.org/software/systemd/man/latest/org.freedesktop.systemd1.html)
- [systemd.unit(5) 手册页](https://www.freedesktop.org/software/systemd/man/latest/systemd.unit.html)
- [systemd.resource-control(5) 手册页](https://www.freedesktop.org/software/systemd/man/latest/systemd.resource-control.html)
- [D-Bus 规范](https://dbus.freedesktop.org/doc/dbus-specification.html)
- systemd 源码：`src/core/dbus-unit.c`
