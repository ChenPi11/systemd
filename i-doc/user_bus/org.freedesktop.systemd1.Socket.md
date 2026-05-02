# 接口名称：org.freedesktop.systemd1.Socket（用户总线版本）

> **用户总线版本说明：** 本接口通过用户会话总线暴露，由 `systemd --user` 实例提供。此接口适用于用户会话中的套接字单元（`.socket`）。用户套接字单元文件位于 `~/.config/systemd/user/`、`/usr/lib/systemd/user/` 等路径。会话用户无需 polkit 授权即可管理自己的用户套接字单元。

## 接口说明

`org.freedesktop.systemd1.Socket` 是 systemd 中**套接字单元（`.socket`）的 D-Bus 接口**。在用户总线上，套接字单元用于实现基于套接字的用户服务激活（socket-based activation）——`systemd --user` 代为监听套接字，当有连接到达时再启动对应的用户服务单元。

通过此接口，客户端可以：

- **查询套接字配置**：监听地址列表、协议设置、缓冲区大小等
- **查询运行状态**：当前连接数、已接受/拒绝的连接总数、控制进程 PID 等
- **查询频率限制状态**：触发频率限制和轮询频率限制的配置

> **注意**：此接口同时继承 `org.freedesktop.systemd1.Unit` 接口的全部属性和方法，以及执行上下文（ExecuteContext）和 CGroup 上下文的全部属性。

## 总线名称

`org.freedesktop.systemd1`

## 总线类型

用户总线（`busctl --user`）

## 继承关系

```
org.freedesktop.systemd1.Unit
    └── org.freedesktop.systemd1.Socket
```

## 对象路径

`/org/freedesktop/systemd1/unit/<socket_name>`

其中 `<socket_name>` 是套接字单元名称经过 D-Bus 路径转义后的形式（例如 `myapp.socket` 对应 `/org/freedesktop/systemd1/unit/myapp_2esocket`）。

---

## 与系统总线版本的主要差异

- 套接字监听地址通常位于用户运行时目录下（如 `$XDG_RUNTIME_DIR/myapp.sock`，即 `/run/user/<UID>/myapp.sock`），而非系统级路径
- 激活的目标服务为用户服务，而非系统服务
- 所有操作无需 polkit 授权
- busctl 命令使用 `--user` 选项而非默认的系统总线

---

## 属性（Properties）

### 监听配置

| 属性名称 | 类型 | 读写 | 说明 |
|---------|------|------|------|
| `BindIPv6Only` | `s` | 只读 | IPv6 套接字绑定模式：`default`（系统默认）/ `both`（同时监听 IPv4 和 IPv6）/ `ipv6-only`（仅 IPv6） |
| `Backlog` | `u` | 只读 | 监听队列长度（`listen()` 的 backlog 参数） |
| `SocketMode` | `u` | 只读 | 套接字文件权限（八进制，仅适用于 Unix 域套接字）；用户套接字通常设置为 `0600` 或 `0660` |
| `DirectoryMode` | `u` | 只读 | 自动创建的套接字目录的权限 |
| `Accept` | `b` | 只读 | 是否在接受每个连接时启动一个新的服务实例（`true` = 每连接一个实例，`false` = 共享实例） |
| `FlushPending` | `b` | 只读 | 激活服务之前是否清空待处理连接 |
| `Writable` | `b` | 只读 | 是否只在套接字可写时才激活服务（通常用于 FIFO/特殊文件类型的套接字） |
| `KeepAlive` | `b` | 只读 | 是否为 TCP 套接字启用 keep-alive |
| `KeepAliveTimeUSec` | `t` | 只读 | TCP keep-alive 空闲超时时间（微秒） |
| `KeepAliveIntervalUSec` | `t` | 只读 | TCP keep-alive 探测间隔（微秒） |
| `KeepAliveProbes` | `u` | 只读 | TCP keep-alive 最大探测次数 |
| `NoDelay` | `b` | 只读 | 是否禁用 Nagle 算法（TCP_NODELAY） |
| `Priority` | `i` | 只读 | 套接字优先级（`SO_PRIORITY`） |
| `ReceiveBuffer` | `t` | 只读 | 套接字接收缓冲区大小（字节） |
| `SendBuffer` | `t` | 只读 | 套接字发送缓冲区大小（字节） |
| `IPTOS` | `i` | 只读 | IP TOS 字段值 |
| `IPTTL` | `i` | 只读 | IP TTL 值 |
| `PipeSize` | `t` | 只读 | FIFO 管道大小（字节） |
| `FreeBind` | `b` | 只读 | 是否允许绑定到非本地 IP 地址（`IP_FREEBIND`） |
| `Transparent` | `b` | 只读 | 是否启用 IP 透明代理（`IP_TRANSPARENT`） |
| `Broadcast` | `b` | 只读 | 是否启用 UDP 广播 |
| `PassCredentials` | `b` | 只读 | 是否在 Unix 套接字上传递凭证（`SO_PASSCRED`） |
| `PassSecurity` | `b` | 只读 | 是否在 Unix 套接字上传递安全上下文 |
| `PassPacketInfo` | `b` | 只读 | 是否传递数据包信息（`IP_PKTINFO`） |
| `TCPCongestion` | `s` | 只读 | TCP 拥塞控制算法名称（如 `cubic`） |
| `ReusePort` | `b` | 只读 | 是否启用 `SO_REUSEPORT`（多个进程绑定同一端口） |
| `MessageQueueMaxMessages` | `x` | 只读 | POSIX 消息队列最大消息数（`-1` 表示系统默认） |
| `MessageQueueMessageSize` | `x` | 只读 | POSIX 消息队列单条消息最大字节数（`-1` 表示系统默认） |
| `RemoveOnStop` | `b` | 只读 | 套接字单元停止时是否删除套接字文件 |
| `Timestamping` | `s` | 只读 | 套接字时间戳模式：`off`、`us`、`ns` |

### 监听地址

| 属性名称 | 类型 | 读写 | 说明 |
|---------|------|------|------|
| `Listen` | `a(ss)` | 只读 | 监听地址列表，每项为 `(类型, 地址)` 格式。类型如 `Stream`（TCP/Unix流）、`Datagram`（UDP/Unix报文）、`SequentialPacket`（Unix顺序包）、`Netlink`（netlink）、`FIFO`、`Special` 等；用户套接字地址示例：`("Stream", "/run/user/1000/myapp.sock")` |

### 运行状态

| 属性名称 | 类型 | 读写 | 说明 |
|---------|------|------|------|
| `ControlPID` | `u` | 只读 | 当前控制进程（如执行 `ExecStartPre` 的进程）的 PID；无控制进程时为 `0` |
| `Result` | `s` | 只读 | 套接字单元的最终结果：`success`、`resources`、`timeout`、`exit-code`、`signal`、`core-dump`、`watchdog`、`start-limit-hit`、`trigger-limit-hit` |
| `NAccepted` | `u` | 只读 | 自套接字单元启动以来已接受的连接总数（含已关闭连接）。仅当 `Accept=yes` 时有意义 |
| `NConnections` | `u` | 只读 | 当前活跃连接数（仅当 `Accept=yes` 时有意义） |
| `NRefused` | `u` | 只读 | 因连接数超限而被拒绝的连接总数 |
| `UID` | `u` | 只读 | 套接字进程的有效用户 ID |
| `GID` | `u` | 只读 | 套接字进程的有效组 ID |

### 连接数限制

| 属性名称 | 类型 | 读写 | 说明 |
|---------|------|------|------|
| `MaxConnections` | `u` | 只读 | 最大并发连接数（仅当 `Accept=yes` 时有效），超出后新连接被拒绝 |
| `MaxConnectionsPerSource` | `u` | 只读 | 来自同一源地址的最大并发连接数，`0` 表示无限制 |

### 触发频率限制

| 属性名称 | 类型 | 读写 | 说明 |
|---------|------|------|------|
| `TriggerLimitIntervalUSec` | `t` | 只读 | 触发频率限制的时间窗口（微秒）。在此窗口内触发次数超过 `TriggerLimitBurst` 时，套接字进入失败状态 |
| `TriggerLimitBurst` | `u` | 只读 | 在时间窗口 `TriggerLimitIntervalUSec` 内允许的最大触发次数 |

### 轮询频率限制

| 属性名称 | 类型 | 读写 | 说明 |
|---------|------|------|------|
| `PollLimitIntervalUSec` | `t` | 只读 | 轮询频率限制的时间窗口（微秒）。防止套接字被过于频繁地轮询 |
| `PollLimitBurst` | `u` | 只读 | 在时间窗口内允许的最大轮询次数 |

### 执行命令

| 属性名称 | 类型 | 读写 | 说明 |
|---------|------|------|------|
| `ExecStartPre` | `a(sasbttttuii)` | 只读 | 套接字开始监听之前执行的命令列表 |
| `ExecStartPost` | `a(sasbttttuii)` | 只读 | 套接字开始监听之后执行的命令列表 |
| `ExecStopPre` | `a(sasbttttuii)` | 只读 | 套接字停止之前执行的命令列表 |
| `ExecStopPost` | `a(sasbttttuii)` | 只读 | 套接字停止之后执行的命令列表 |

### SELinux 上下文

| 属性名称 | 类型 | 读写 | 说明 |
|---------|------|------|------|
| `SELinuxContextFromNet` | `b` | 只读 | 是否从网络连接的对端获取 SELinux 上下文（用于多标签网络） |
| `SmackLabel` | `s` | 只读 | SMACK 安全标签 |
| `SmackLabelIPIn` | `s` | 只读 | 传入 IP 数据包的 SMACK 标签 |
| `SmackLabelIPOut` | `s` | 只读 | 传出 IP 数据包的 SMACK 标签 |

---

## 方法（Methods）

此接口本身不定义额外方法。所有通用控制方法（Start、Stop、Reload、Kill 等）通过继承的 `org.freedesktop.systemd1.Unit` 接口提供。

---

## 信号（Signals）

此接口本身不定义额外信号。通用信号由 D-Bus 标准接口 `org.freedesktop.DBus.Properties` 提供。

---

## 使用示例

```bash
# 查询用户套接字的监听地址
busctl --user get-property org.freedesktop.systemd1 \
    /org/freedesktop/systemd1/unit/myapp_2esocket \
    org.freedesktop.systemd1.Socket Listen

# 查询当前活跃连接数
busctl --user get-property org.freedesktop.systemd1 \
    /org/freedesktop/systemd1/unit/myapp_2esocket \
    org.freedesktop.systemd1.Socket NConnections

# 查询已接受连接总数
busctl --user get-property org.freedesktop.systemd1 \
    /org/freedesktop/systemd1/unit/myapp_2esocket \
    org.freedesktop.systemd1.Socket NAccepted

# 启动套接字单元（开始监听）
busctl --user call org.freedesktop.systemd1 \
    /org/freedesktop/systemd1/unit/myapp_2esocket \
    org.freedesktop.systemd1.Unit \
    Start s replace
```

## 用户套接字单元文件示例

```ini
# ~/.config/systemd/user/myapp.socket
[Unit]
Description=My Application Socket

[Socket]
ListenStream=%t/myapp.sock
# %t 展开为 $XDG_RUNTIME_DIR，即 /run/user/<UID>
SocketMode=0600

[Install]
WantedBy=sockets.target
```

对应服务单元 `~/.config/systemd/user/myapp.service` 将在连接到达时自动激活。

## 参见

- [`org.freedesktop.systemd1.Unit`](org.freedesktop.systemd1.Unit.md) — 所有单元类型的基础接口（用户总线版本）
- `systemd.socket(5)` — 套接字单元配置文件参考手册
- `systemd --user` — 用户会话 systemd 实例
