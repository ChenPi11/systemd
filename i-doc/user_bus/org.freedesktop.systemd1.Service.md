# 接口名称：org.freedesktop.systemd1.Service（用户总线版本）

> **用户总线版本说明：** 本接口通过用户会话总线暴露，由 `systemd --user` 实例提供。此接口适用于用户会话中的服务单元（`.service`）。用户服务单元文件位于 `~/.config/systemd/user/`、`/usr/lib/systemd/user/` 等路径。会话用户无需 polkit 授权即可管理自己的用户服务。

## 接口说明

`org.freedesktop.systemd1.Service` 是 systemd 中**服务单元（`.service`）的 D-Bus 接口**。在用户总线上，服务单元用于管理用户会话中的守护进程、一次性命令及其他可执行程序的生命周期。

通过此接口，客户端可以：

- **查询用户服务运行状态**：主进程 PID、控制进程 PID、重启次数等
- **查询执行配置**：启动命令、超时设置、重启策略等
- **管理文件描述符存储**：查询或转储 `FileDescriptorStore`
- **动态绑定挂载**：向正在运行的服务命名空间中添加挂载点

> **注意**：此接口同时继承 `org.freedesktop.systemd1.Unit` 接口的全部属性和方法，以及执行上下文（ExecuteContext）和 CGroup 上下文的全部属性。

## 总线名称

`org.freedesktop.systemd1`

## 总线类型

用户总线（`busctl --user`）

## 继承关系

```
org.freedesktop.systemd1.Unit
    └── org.freedesktop.systemd1.Service
```

## 对象路径

`/org/freedesktop/systemd1/unit/<service_name>`

其中 `<service_name>` 是用户服务单元名称经过 D-Bus 路径转义后的形式（例如 `myapp.service` 对应 `/org/freedesktop/systemd1/unit/myapp_2eservice`）。

---

## 属性（Properties）

### 服务类型与行为

| 属性名称 | 类型 | 读写 | 说明 |
|---------|------|------|------|
| `Type` | `s` | 只读 | 服务类型：`simple`（默认）/ `forking`（父进程退出后视为启动完成）/ `oneshot`（一次性）/ `dbus`（D-Bus 服务）/ `notify`（sd_notify 通知）/ `notify-reload`（支持重载通知）/ `idle`（空闲时启动） |
| `ExitType` | `s` | 只读 | 退出类型：`main`（主进程退出即认为服务停止）/ `cgroup`（cgroup 内所有进程退出才停止） |
| `Restart` | `s` | 只读 | 重启策略：`no` / `on-success` / `on-failure` / `on-abnormal` / `on-watchdog` / `on-abort` / `always` |
| `RestartMode` | `s` | 只读 | 重启模式：`normal`（正常重启流程）/ `direct`（跳过停止阶段直接重启） |
| `RemainAfterExit` | `b` | 只读 | 为 `true` 时，即使所有进程退出后服务仍保持 `active` 状态（常用于 `oneshot` 类型） |
| `GuessMainPID` | `b` | 只读 | 为 `true` 时，对 `forking` 类型服务自动猜测主进程 PID |
| `RootDirectoryStartOnly` | `b` | 只读 | 为 `true` 时，`RootDirectory` 仅对 `ExecStart` 命令生效 |
| `OOMPolicy` | `s` | 只读 | OOM 终止策略：`stop`（停止服务）/ `continue`（继续运行）/ `kill`（终止整个 cgroup） |

### PID 文件与 D-Bus 名称

| 属性名称 | 类型 | 读写 | 说明 |
|---------|------|------|------|
| `PIDFile` | `s` | 只读 | PID 文件路径，`Type=forking` 时 systemd 从该文件读取主进程 PID |
| `BusName` | `s` | 只读 | D-Bus 服务名称，`Type=dbus` 时 systemd 等待该服务名出现在**用户会话总线**上才认为启动完成 |
| `NotifyAccess` | `s` | 只读 | `sd_notify()` 通知访问级别：`none`（禁止）/ `main`（仅主进程）/ `exec`（所有被执行的进程）/ `all`（所有进程） |

### 进程 PID

| 属性名称 | 类型 | 读写 | 说明 |
|---------|------|------|------|
| `MainPID` | `u` | 只读 | 服务主进程的 PID；服务未运行时为 `0` |
| `ControlPID` | `u` | 只读 | 当前控制进程（如执行 ExecStartPre 的进程）的 PID；无控制进程时为 `0` |
| `UID` | `u` | 只读 | 服务进程的有效用户 ID（UID）；用户服务通常为会话用户的 UID |
| `GID` | `u` | 只读 | 服务进程的有效组 ID（GID） |

### 超时设置

| 属性名称 | 类型 | 读写 | 说明 |
|---------|------|------|------|
| `TimeoutStartUSec` | `t` | 只读 | 启动超时时间（微秒） |
| `TimeoutStopUSec` | `t` | 只读 | 停止超时时间（微秒） |
| `TimeoutAbortUSec` | `t` | 只读 | 中止超时时间（微秒） |
| `TimeoutStartFailureMode` | `s` | 只读 | 启动超时失败模式：`terminate`（发送 SIGTERM）/ `abort`（发送 SIGABRT）/ `kill`（发送 SIGKILL） |
| `TimeoutStopFailureMode` | `s` | 只读 | 停止超时失败模式，取值与 `TimeoutStartFailureMode` 相同 |
| `RuntimeMaxUSec` | `t` | 只读 | 最大运行时间（微秒）；超过后服务被终止，`0` 表示无限制 |
| `RuntimeRandomizedExtraUSec` | `t` | 只读 | 在 `RuntimeMaxUSec` 基础上随机增加的额外时间（微秒） |
| `WatchdogUSec` | `t` | 只读 | 看门狗超时时间（微秒）；服务需在此时间内调用 `sd_notify("WATCHDOG=1")` |

### 重启延迟

| 属性名称 | 类型 | 读写 | 说明 |
|---------|------|------|------|
| `RestartUSec` | `t` | 只读 | 重启前的等待时间（微秒） |
| `RestartSteps` | `u` | 只读 | 指数退避重启的步数；`0` 表示禁用指数退避 |
| `RestartMaxDelayUSec` | `t` | 只读 | 指数退避时重启延迟的最大值（微秒） |
| `RestartUSecNext` | `t` | 只读 | 下次重启将等待的延迟时间（微秒） |
| `NRestarts` | `u` | 只读 | 服务自上次成功启动以来的重启次数 |

### 退出状态过滤

| 属性名称 | 类型 | 读写 | 说明 |
|---------|------|------|------|
| `RestartPreventExitStatus` | `(aiai)` | 只读 | 阻止重启的退出状态列表，格式为 `(退出码数组, 信号号码数组)` |
| `RestartForceExitStatus` | `(aiai)` | 只读 | 强制触发重启的退出状态列表，格式与上相同 |
| `SuccessExitStatus` | `(aiai)` | 只读 | 视为成功退出的状态列表，格式与上相同 |

### 状态信息

| 属性名称 | 类型 | 读写 | 说明 |
|---------|------|------|------|
| `StatusText` | `s` | 只读 | 服务通过 `sd_notify("STATUS=...")` 设置的状态文本描述 |
| `StatusErrno` | `i` | 只读 | 服务通过 `sd_notify("ERRNO=...")` 设置的 errno 值；`0` 表示无错误 |
| `StatusBusError` | `s` | 只读 | 服务通过 `sd_notify` 设置的 D-Bus 错误名称 |
| `StatusVarlinkError` | `s` | 只读 | 服务通过 `sd_notify` 设置的 Varlink 错误名称 |
| `Result` | `s` | 只读 | 服务最终结果：`success` / `exit-code` / `signal` / `core-dump` / `watchdog` / `timeout` 等 |
| `ReloadResult` | `s` | 只读 | 最近一次重载操作的结果 |
| `CleanResult` | `s` | 只读 | 最近一次清理（`systemctl --user clean`）操作的结果 |

### 文件描述符存储

| 属性名称 | 类型 | 读写 | 说明 |
|---------|------|------|------|
| `FileDescriptorStoreMax` | `u` | 只读 | 文件描述符存储的最大容量 |
| `NFileDescriptorStore` | `u` | 只读 | 当前文件描述符存储中持有的 FD 数量 |
| `FileDescriptorStorePreserve` | `s` | 只读 | FD 存储的保留策略：`no`（服务停止时清空）/ `yes`（始终保留）/ `restart`（仅重启时保留） |
| `ExtraFileDescriptorNames` | `as` | 只读 | 传递给服务的额外文件描述符的名称列表 |

### 打开文件与信号

| 属性名称 | 类型 | 读写 | 说明 |
|---------|------|------|------|
| `OpenFile` | `a(sst)` | 只读 | 服务启动时自动打开并传入的文件列表；每项为 `(文件路径, FD 名称, 标志位)` |
| `ReloadSignal` | `i` | 只读 | 执行重载操作时发送给主进程的信号号码 |

### 继承自执行上下文（ExecuteContext）的关键属性

以下属性来自 systemd 执行上下文，控制用户服务进程的运行环境：

| 属性名称 | 类型 | 读写 | 说明 |
|---------|------|------|------|
| `ExecCondition` | `a(sasbttttuii)` | 只读 | 预检命令列表；任一命令失败则跳过服务 |
| `ExecStartPre` | `a(sasbttttuii)` | 只读 | 启动前执行的命令列表 |
| `ExecStart` | `a(sasbttttuii)` | 只读 | 主启动命令列表 |
| `ExecStartPost` | `a(sasbttttuii)` | 只读 | 启动后执行的命令列表 |
| `ExecReload` | `a(sasbttttuii)` | 只读 | 重载时执行的命令列表 |
| `ExecStop` | `a(sasbttttuii)` | 只读 | 停止时执行的命令列表 |
| `ExecStopPost` | `a(sasbttttuii)` | 只读 | 停止后执行的命令列表 |
| `WorkingDirectory` | `s` | 只读 | 进程工作目录（用户服务默认通常为用户主目录） |
| `RootDirectory` | `s` | 只读 | 根目录（chroot） |
| `User` | `s` | 只读 | 运行进程的用户名或 UID（用户服务通常为会话用户） |
| `Group` | `s` | 只读 | 运行进程的组名或 GID |
| `SupplementaryGroups` | `as` | 只读 | 附加用户组列表 |
| `Environment` | `as` | 只读 | 环境变量列表（`KEY=VALUE` 形式） |
| `EnvironmentFiles` | `a(sb)` | 只读 | 环境变量文件列表 |

### 继承自 CGroup 上下文的关键属性

| 属性名称 | 类型 | 读写 | 说明 |
|---------|------|------|------|
| `CPUAccounting` | `b` | 只读 | 是否启用 CPU 使用量统计 |
| `CPUWeight` | `t` | 只读 | CPU 调度权重（1–10000） |
| `MemoryAccounting` | `b` | 只读 | 是否启用内存使用量统计 |
| `MemoryMax` | `t` | 只读 | 内存使用硬上限（字节） |
| `MemorySwapMax` | `t` | 只读 | 内存+交换合计硬上限（字节） |
| `TasksAccounting` | `b` | 只读 | 是否启用任务数统计 |
| `TasksMax` | `t` | 只读 | 最大任务数（进程数）上限 |
| `IOAccounting` | `b` | 只读 | 是否启用 I/O 使用量统计 |
| `IOWeight` | `t` | 只读 | I/O 调度权重（1–10000） |
| `Delegate` | `b` | 只读 | 是否将 cgroup 控制委托给服务自身 |
| `Slice` | `s` | 只读 | 所属的 slice 单元名称（用户会话中通常为 `app.slice`） |

---

## 方法（Methods）

### BindMount

**说明：** 向正在运行的用户服务的挂载命名空间中动态添加一个绑定挂载。服务必须已启动且具有私有挂载命名空间（`PrivateMounts=yes` 或类似设置）。

**参数：**

| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| `source` | `s` | 输入 | 宿主机上的源路径 |
| `destination` | `s` | 输入 | 服务挂载命名空间中的目标路径 |
| `read_only` | `b` | 输入 | 是否以只读方式绑定挂载 |
| `mkdir` | `b` | 输入 | 目标路径不存在时是否自动创建 |

**返回：** 无

---

### MountImage

**说明：** 向正在运行的用户服务的挂载命名空间中动态挂载一个磁盘镜像文件（如 squashfs、ext4 等）。

**参数：**

| 名称 | 类型 | 方向 | 说明 |
|------|------|------|------|
| `source` | `s` | 输入 | 宿主机上镜像文件的路径 |
| `destination` | `s` | 输入 | 服务挂载命名空间中的目标挂载点 |
| `read_only` | `b` | 输入 | 是否以只读方式挂载镜像 |
| `mkdir` | `b` | 输入 | 目标路径不存在时是否自动创建 |
| `options` | `a(ss)` | 输入 | 额外挂载选项列表，每项为 `(选项名, 选项值)` |

**返回：** 无

---

### DumpFileDescriptorStore

**说明：** 转储当前文件描述符存储中保存的所有 FD 信息。

**参数：** 无

**返回：**

| 名称 | 类型 | 说明 |
|------|------|------|
| `entries` | `a(suuutuusu)` | FD 条目数组；每项字段依次为：`(FD名称, PID, UID, GID, 创建时间戳, 标志位, 挂载ID, 路径, 类型)` |

---

## 信号（Signals）

此接口本身不定义额外信号。通用信号（如属性变更通知 `PropertiesChanged`）由 D-Bus 标准接口 `org.freedesktop.DBus.Properties` 提供。

---

## 使用示例

```bash
# 查询用户会话 myapp.service 的主进程 PID
busctl --user get-property org.freedesktop.systemd1 \
    /org/freedesktop/systemd1/unit/myapp_2eservice \
    org.freedesktop.systemd1.Service MainPID

# 查询服务类型
busctl --user get-property org.freedesktop.systemd1 \
    /org/freedesktop/systemd1/unit/myapp_2eservice \
    org.freedesktop.systemd1.Service Type

# 查询重启次数
busctl --user get-property org.freedesktop.systemd1 \
    /org/freedesktop/systemd1/unit/myapp_2eservice \
    org.freedesktop.systemd1.Service NRestarts

# 查询服务状态文本
busctl --user get-property org.freedesktop.systemd1 \
    /org/freedesktop/systemd1/unit/myapp_2eservice \
    org.freedesktop.systemd1.Service StatusText

# 转储 FD 存储内容
busctl --user call org.freedesktop.systemd1 \
    /org/freedesktop/systemd1/unit/myapp_2eservice \
    org.freedesktop.systemd1.Service DumpFileDescriptorStore

# 查询所有 Service 接口属性
busctl --user introspect org.freedesktop.systemd1 \
    /org/freedesktop/systemd1/unit/myapp_2eservice \
    org.freedesktop.systemd1.Service
```

## 用户服务单元文件示例

```ini
# ~/.config/systemd/user/myapp.service
[Unit]
Description=My User Application
After=default.target

[Service]
Type=simple
ExecStart=/usr/bin/myapp --config %h/.config/myapp/config.toml
Restart=on-failure
RestartSec=5s
Environment=MYAPP_ENV=production

[Install]
WantedBy=default.target
```

启用并启动：
```bash
systemctl --user enable --now myapp.service
```

## 参见

- [`org.freedesktop.systemd1.Unit`](org.freedesktop.systemd1.Unit.md) — 所有单元类型的基础接口（用户总线版本）
- `systemd.service(5)` — 服务单元配置文件参考手册
- `sd_notify(3)` — 服务状态通知 API
- `systemctl --user` — 用户会话服务控制命令
