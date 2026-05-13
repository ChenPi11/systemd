# 接口名称：org.freedesktop.systemd1.Job

## 接口说明

`org.freedesktop.systemd1.Job` 是 systemd 中**作业对象的 D-Bus 接口**。作业（Job）是 systemd 内部用于执行单元状态转换的任务单位——每次对单元执行启动、停止、重启等操作时，systemd 都会创建一个对应的作业对象加入执行队列。

通过此接口，客户端可以：

- **查询作业状态**：作业类型、当前状态、关联单元等
- **取消待执行的作业**：取消尚未开始执行的作业
- **查询作业依赖顺序**：获取在此作业之前/之后需要完成的其他作业

> **注意**：作业对象是**临时对象**，作业完成（无论成功或失败）后，对应的 D-Bus 对象会被销毁。与单元对象不同，作业对象**不继承**任何其他接口。

## 总线名称

`org.freedesktop.systemd1`

## 对象路径

`/org/freedesktop/systemd1/job/<job_id>`

其中 `<job_id>` 是作业的数字 ID（如 `/org/freedesktop/systemd1/job/42`）。

---

## 属性（Properties）

| 属性名称 | 类型 | 读写 | 说明 |
|---------|------|------|------|
| `Id` | `u` | 只读 | 作业的唯一数字标识符。在 systemd 实例的整个生命周期内单调递增，不会重复 |
| `Unit` | `(so)` | 只读 | 此作业关联的单元，格式为 `(单元名称, 单元对象路径)` |
| `JobType` | `s` | 只读 | 作业类型（见下方说明） |
| `State` | `s` | 只读 | 作业当前状态：`waiting`（在队列中等待执行）/ `running`（正在执行） |
| `ActivationDetails` | `a(ss)` | 只读 | 触发此作业的激活详情，格式为键值对数组 `(键, 值)`。例如：触发原因、触发来源等元数据 |

### JobType 可选值

| 值 | 说明 |
|----|------|
| `start` | 启动单元 |
| `stop` | 停止单元 |
| `reload` | 重载单元配置（不重启进程） |
| `restart` | 重启单元（停止后重新启动） |
| `try-restart` | 若单元当前正在运行则重启，否则不操作 |
| `reload-or-start` | 若单元正在运行则重载，否则启动 |
| `verify-active` | 验证单元当前处于活跃状态（不实际执行操作，用于依赖检查） |
| `nop` | 空操作（用于依赖满足检查） |

---

## 方法（Methods）

### Cancel

**说明：** 取消此作业。只能取消处于 `waiting` 状态的作业；处于 `running` 状态的作业无法取消。

**参数：** 无

**返回：** 无

**错误：**
- 若作业已处于 `running` 状态，返回 `org.freedesktop.systemd1.JobTypeNotApplicable` 错误

---

### GetAfter

**说明：** 获取在此作业执行**完成之后**才能运行的作业列表（即依赖于此作业的后继作业）。

**参数：** 无

**返回：**

| 名称 | 类型 | 说明 |
|------|------|------|
| `jobs` | `a(usssoo)` | 后继作业数组（见下方格式说明） |

---

### GetBefore

**说明：** 获取在此作业执行**开始之前**必须先完成的作业列表（即此作业依赖的前驱作业）。

**参数：** 无

**返回：**

| 名称 | 类型 | 说明 |
|------|------|------|
| `jobs` | `a(usssoo)` | 前驱作业数组（见下方格式说明） |

### 作业数组元素格式 `(usssoo)`

`GetAfter` 和 `GetBefore` 返回的作业数组中，每个元素包含以下字段：

| 索引 | 类型 | 字段名称 | 说明 |
|------|------|---------|------|
| 0 | `u` | `job_id` | 作业 ID |
| 1 | `s` | `unit_id` | 关联单元名称（如 `nginx.service`） |
| 2 | `s` | `job_type` | 作业类型（如 `start`、`stop`） |
| 3 | `s` | `job_state` | 作业状态（`waiting` 或 `running`） |
| 4 | `o` | `job_path` | 作业对象路径（如 `/org/freedesktop/systemd1/job/42`） |
| 5 | `o` | `unit_path` | 单元对象路径（如 `/org/freedesktop/systemd1/unit/nginx_2eservice`） |

---

## 信号（Signals）

作业接口本身不定义额外信号。作业完成时，Manager 接口会发出 `JobRemoved` 信号（带有作业 ID、路径、单元名称和结果）。

---

## 使用示例

```bash
# 查询所有当前排队的作业（通过 Manager 接口）
busctl call org.freedesktop.systemd1 \
    /org/freedesktop/systemd1 \
    org.freedesktop.systemd1.Manager ListJobs

# 查询指定作业的类型和状态（假设作业 ID 为 42）
busctl get-property org.freedesktop.systemd1 \
    /org/freedesktop/systemd1/job/42 \
    org.freedesktop.systemd1.Job JobType

busctl get-property org.freedesktop.systemd1 \
    /org/freedesktop/systemd1/job/42 \
    org.freedesktop.systemd1.Job State

# 查询作业关联的单元
busctl get-property org.freedesktop.systemd1 \
    /org/freedesktop/systemd1/job/42 \
    org.freedesktop.systemd1.Job Unit

# 取消作业
busctl call org.freedesktop.systemd1 \
    /org/freedesktop/systemd1/job/42 \
    org.freedesktop.systemd1.Job Cancel

# 获取前驱作业列表
busctl call org.freedesktop.systemd1 \
    /org/freedesktop/systemd1/job/42 \
    org.freedesktop.systemd1.Job GetBefore

# 获取后继作业列表
busctl call org.freedesktop.systemd1 \
    /org/freedesktop/systemd1/job/42 \
    org.freedesktop.systemd1.Job GetAfter
```

## 监听作业事件

可通过订阅 Manager 接口的信号来监控作业生命周期：

```bash
# 监听作业创建和完成事件
busctl monitor org.freedesktop.systemd1 \
    --match "interface=org.freedesktop.systemd1.Manager,member=JobNew" \
    --match "interface=org.freedesktop.systemd1.Manager,member=JobRemoved"
```

`JobRemoved` 信号的 `result` 参数取值：

| 值 | 说明 |
|----|------|
| `done` | 作业成功完成 |
| `canceled` | 作业被取消 |
| `timeout` | 作业执行超时 |
| `failed` | 作业执行失败 |
| `dependency` | 依赖的作业失败导致此作业失败 |
| `skipped` | 作业被跳过（条件不满足） |

## 参见

- [`org.freedesktop.systemd1.Manager`](org.freedesktop.systemd1.Manager.md) — 管理器接口（`ListJobs`、`GetJob`、`JobNew`、`JobRemoved` 等）
- [`org.freedesktop.systemd1.Unit`](org.freedesktop.systemd1.Unit.md) — 单元基础接口（`Start`、`Stop` 等返回作业路径）
- `systemctl(1)` — systemd 命令行控制工具
