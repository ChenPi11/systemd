# 接口名称：org.freedesktop.systemd1.Job（用户总线版本）

此接口与系统总线上的同名接口相同，但在用户会话上下文中操作。

## 概述

`org.freedesktop.systemd1.Job` 是 systemd 作业（Job）的 D-Bus 接口。作业代表一个**待执行或正在执行的单元状态转换任务**（如启动、停止、重载等）。每次通过 D-Bus 请求 `StartUnit`、`StopUnit` 等操作时，`systemd --user` 会创建一个 Job 对象并返回其路径。

## 总线名称

`org.freedesktop.systemd1`

## 总线类型

用户总线（`busctl --user`）

## 对象路径

`/org/freedesktop/systemd1/job/<job_id>`

其中 `<job_id>` 是作业的数字 ID，由 systemd 分配（例如 `/org/freedesktop/systemd1/job/42`）。

## 用户会话的关键差异

- **busctl 命令**：使用 `busctl --user`
- **无需 polkit**：会话用户操作用户服务产生的 Job 对象无需授权
- **作业生命周期**：与系统总线相同，作业完成后被销毁，对象路径不再有效
- **作业来源**：用户总线上的作业来自用户对用户单元的控制操作（如 `systemctl --user start myapp.service`）

## 属性（Properties）

| 属性名称 | 类型 | 读写 | 说明 |
|---------|------|------|------|
| `Id` | `u` | 只读 | 作业的唯一数字 ID |
| `Unit` | `(so)` | 只读 | 该作业所操作的单元，格式为 `(单元名称, 对象路径)` |
| `JobType` | `s` | 只读 | 作业类型：`start`（启动）/ `stop`（停止）/ `reload`（重载）/ `restart`（重启）/ `try-restart`（尝试重启）/ `reload-or-start`（重载或启动）/ `verify-active`（验证激活状态） |
| `State` | `s` | 只读 | 作业当前状态：`waiting`（等待执行）/ `running`（正在执行） |

## 方法（Methods）

### Cancel

**说明：** 取消一个正在等待或运行中的作业。

**参数：** 无

**返回：** 无

**用法：**
```bash
# 取消某个用户作业
busctl --user call org.freedesktop.systemd1 \
    /org/freedesktop/systemd1/job/42 \
    org.freedesktop.systemd1.Job Cancel
```

### GetAfter

**说明：** 获取此作业之后才能执行的作业列表（即此作业的依赖作业）。

**参数：** 无

**返回：**

| 名称 | 类型 | 说明 |
|------|------|------|
| `jobs` | `a(usssoo)` | 作业信息数组，每项为 `(ID, 单元名称, 作业类型, 状态, 作业路径, 单元路径)` |

### GetBefore

**说明：** 获取在此作业之前执行的作业列表（即此作业依赖的前置作业）。

**参数：** 无

**返回：**

| 名称 | 类型 | 说明 |
|------|------|------|
| `jobs` | `a(usssoo)` | 作业信息数组，格式与 `GetAfter` 相同 |

## 信号（Signals）

此接口本身不定义额外信号。Manager 对象会发出作业相关的信号（`JobNew`、`JobRemoved`）——详见 [`org.freedesktop.systemd1.Manager`](org.freedesktop.systemd1.Manager.md)。

## 使用示例

```bash
# 启动用户服务并获取 Job 对象路径
JOB_PATH=$(busctl --user call org.freedesktop.systemd1 \
    /org/freedesktop/systemd1 \
    org.freedesktop.systemd1.Manager \
    StartUnit ss myapp.service replace \
    | awk '{print $2}' | tr -d '"')

echo "Job path: $JOB_PATH"

# 查询作业类型和状态
busctl --user get-property org.freedesktop.systemd1 \
    "$JOB_PATH" \
    org.freedesktop.systemd1.Job JobType

busctl --user get-property org.freedesktop.systemd1 \
    "$JOB_PATH" \
    org.freedesktop.systemd1.Job State

# 取消作业
busctl --user call org.freedesktop.systemd1 \
    "$JOB_PATH" \
    org.freedesktop.systemd1.Job Cancel
```

## 注意事项

- 作业对象是**临时的**：作业完成（成功或失败）后，对应的 D-Bus 对象路径立即失效
- 若需要在作业完成后得到通知，应在发起请求前订阅 `org.freedesktop.systemd1.Manager` 的 `JobRemoved` 信号
- 可通过 `busctl --user call ... Manager ListJobs` 列出当前所有待处理的用户作业

## 参见

- [`org.freedesktop.systemd1.Manager`](org.freedesktop.systemd1.Manager.md) — 管理接口中的作业相关方法和信号（用户总线版本）
- [`org.freedesktop.systemd1.Unit`](org.freedesktop.systemd1.Unit.md) — 单元基础接口（用户总线版本）
- 系统总线版本：`../system_bus/org.freedesktop.systemd1.Job.md`
