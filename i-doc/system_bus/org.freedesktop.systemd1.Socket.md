# 接口名称：org.freedesktop.systemd1.Socket

## 接口说明

`org.freedesktop.systemd1.Socket` 是 systemd 中**套接字单元（`.socket`）的 D-Bus 接口**。套接字单元用于实现基于套接字的服务激活（socket-based activation）——systemd 代为监听套接字，当有连接到达时再启动对应的服务单元。

通过此接口，客户端可以：

- **查询套接字配置**：监听地址列表、协议设置、缓冲区大小等
- **查询运行状态**：当前连接数、已接受/拒绝的连接总数、控制进程 PID 等
- **查询频率限制状态**：触发频率限制和轮询频率限制的配置

> **注意**：此接口同时继承 `org.freedesktop.systemd1.Unit` 接口的全部属性和方法，以及执行上下文（ExecuteContext）和 CGroup 上下文的全部属性。

## 总线名称

`org.freedesktop.systemd1`

## 继承关系

```
org.freedesktop.systemd1.Unit
    └── org.freedesktop.systemd1.Socket
```

## 对象路径

`/org/freedesktop/systemd1/unit/<socket_name>`

其中 `<socket_name>` 是套接字单元名称经过 D-Bus 路径转义后的形式（例如 `sshd.socket` 对应 `/org/freedesktop/systemd1/unit/sshd_2esocket`）。

---

## 属性（Properties）

### 激活模式

| 属性名称 | 类型 | 读写 | 说明 |
|---------|------|------|------|
| `Accept` | `b` | 只读 | 为 `true` 时，每个入站连接都会创建一个独立的服务实例（`inetd` 风格）；为 `false` 时（默认），所有连接共享同一个服务实例 |
| `FlushPending` | `b` | 只读 | 为 `true` 时，在激活服务前先清空套接字上的待处理连接队列 |
| `Writable` | `b` | 只读 | 对于数据报套接字（SOCK_DGRAM），是否以可写模式传递给服务 |

### 监听配置

| 属性名称 | 类型 | 读写 | 说明 |
|---------|------|------|------|
| `Listen` | `a(ss)` | 只读 | 监听地址列表；每项为 `(类型, 地址)`，类型可以是 `ListenStream`、`ListenDatagram`、`ListenSequentialPacket`、`ListenFIFO`、`ListenNetlink`、`ListenSpecial` 等 |
| `Symlinks` | `as` | 只读 | 套接字文件的软链接路径列表（仅对 Unix 域套接字有效） |
| `Backlog` | `u` | 只读 | TCP/Unix 套接字监听队列长度（`listen()` 的 `backlog` 参数） |
| `BindToDevice` | `s` | 只读 | 将套接字绑定到指定网络接口（`SO_BINDTODEVICE`）；为空表示不绑定 |
| `BindIPv6Only` | `s` | 只读 | IPv6 绑定模式：`default`（系统默认）/ `both`（同时监听 IPv4 和 IPv6）/ `ipv6-only`（仅 IPv6） |
| `FileDescriptorName` | `s` | 只读 | 传递给服务的文件描述符名称（通过 `FileDescriptorName=` 配置，默认为套接字单元名称） |
| `SocketProtocol` | `i` | 只读 | 套接字协议号（如 `IPPROTO_UDPLITE`）；`0` 表示使用默认协议 |
| `RemoveOnStop` | `b` | 只读 | 套接字单元停止时是否删除 Unix 域套接字文件 |

### 文件权限

| 属性名称 | 类型 | 读写 | 说明 |
|---------|------|------|------|
| `SocketUser` | `s` | 只读 | Unix 域套接字文件的所属用户 |
| `SocketGroup` | `s` | 只读 | Unix 域套接字文件的所属组 |
| `SocketMode` | `u` | 只读 | Unix 域套接字文件的访问权限（八进制，如 `0666`） |
| `DirectoryMode` | `u` | 只读 | 自动创建套接字目录时使用的权限（八进制） |

### TCP/IP 选项

| 属性名称 | 类型 | 读写 | 说明 |
|---------|------|------|------|
| `KeepAlive` | `b` | 只读 | 是否启用 TCP keepalive（`SO_KEEPALIVE`） |
| `KeepAliveTimeUSec` | `t` | 只读 | TCP keepalive 首次探测前的空闲等待时间（微秒） |
| `KeepAliveIntervalUSec` | `t` | 只读 | TCP keepalive 探测间隔（微秒） |
| `KeepAliveProbes` | `u` | 只读 | TCP keepalive 最大探测次数 |
| `NoDelay` | `b` | 只读 | 是否启用 `TCP_NODELAY`（禁用 Nagle 算法） |
| `DeferAcceptUSec` | `t` | 只读 | 延迟接受超时（微秒），对应 `TCP_DEFER_ACCEPT`；`0` 表示禁用 |
| `FreeBind` | `b` | 只读 | 是否允许绑定到尚不存在的 IP 地址（`IP_FREEBIND`） |
| `Transparent` | `b` | 只读 | 是否启用 IP 透明代理模式（`IP_TRANSPARENT`） |
| `Broadcast` | `b` | 只读 | 是否启用广播（`SO_BROADCAST`） |
| `IPTOS` | `i` | 只读 | IP 服务类型字段值（`IP_TOS`） |
| `IPTTL` | `i` | 只读 | IP TTL 值（`IP_TTL`） |
| `ReusePort` | `b` | 只读 | 是否启用端口复用（`SO_REUSEPORT`） |
| `TCPCongestion` | `s` | 只读 | TCP 拥塞控制算法名称（如 `cubic`、`bbr`） |
| `Mark` | `i` | 只读 | 套接字标记值（`SO_MARK`），用于 netfilter 规则匹配 |

### 缓冲区与消息队列

| 属性名称 | 类型 | 读写 | 说明 |
|---------|------|------|------|
| `ReceiveBuffer` | `t` | 只读 | 套接字接收缓冲区大小（字节，`SO_RCVBUF`） |
| `SendBuffer` | `t` | 只读 | 套接字发送缓冲区大小（字节，`SO_SNDBUF`） |
| `PipeSize` | `t` | 只读 | FIFO 管道缓冲区大小（字节） |
| `MessageQueueMaxMessages` | `x` | 只读 | POSIX 消息队列（mq）最大消息数 |
| `MessageQueueMessageSize` | `x` | 只读 | POSIX 消息队列（mq）单条消息最大字节数 |
| `Priority` | `i` | 只读 | 套接字优先级（`SO_PRIORITY`） |

### 凭据与安全

| 属性名称 | 类型 | 读写 | 说明 |
|---------|------|------|------|
| `PassCredentials` | `b` | 只读 | 是否启用 Unix 凭据传递（`SO_PASSCRED`） |
| `PassPIDFD` | `b` | 只读 | 是否在接受连接时传递对端进程的 pidfd |
| `PassSecurity` | `b` | 只读 | 是否启用 SELinux 安全上下文传递（`SO_PASSSEC`） |
| `PassPacketInfo` | `b` | 只读 | 是否传递数据包元信息（`IP_PKTINFO`） |
| `AcceptFileDescriptors` | `b` | 只读 | 是否接受通过 `SCM_RIGHTS` 传递的文件描述符（`SO_PASSFDS`） |
| `PassFileDescriptorsToExec` | `b` | 只读 | 是否将套接字文件描述符传递给通过 `exec` 调用的进程 |
| `Timestamping` | `s` | 只读 | 套接字时间戳模式：`off`（关闭）/ `us`（微秒精度）/ `ns`（纳秒精度） |
| `SmackLabel` | `s` | 只读 | SMACK 安全标签 |
| `SmackLabelIPIn` | `s` | 只读 | 入站连接的 SMACK 安全标签 |
| `SmackLabelIPOut` | `s` | 只读 | 出站连接的 SMACK 安全标签 |

### 连接限制

| 属性名称 | 类型 | 读写 | 说明 |
|---------|------|------|------|
| `MaxConnections` | `u` | 只读 | 同时允许的最大连接数（`Accept=yes` 模式下有效） |
| `MaxConnectionsPerSource` | `u` | 只读 | 每个源 IP 地址允许的最大并发连接数 |

### 运行状态

| 属性名称 | 类型 | 读写 | 说明 |
|---------|------|------|------|
| `ControlPID` | `u` | 只读 | 当前控制进程（如执行 `ExecStartPre` 的进程）的 PID；无则为 `0` |
| `NConnections` | `u` | 只读 | 当前活跃连接数 |
| `NAccepted` | `u` | 只读 | 自启动以来已成功接受的连接总数 |
| `NRefused` | `u` | 只读 | 自启动以来因连接数超限而被拒绝的连接总数 |
| `UID` | `u` | 只读 | 套接字进程的有效用户 ID |
| `GID` | `u` | 只读 | 套接字进程的有效组 ID |
| `Result` | `s` | 只读 | 套接字单元最终结果：`success` / `resources`（资源不足）/ `timeout`（超时）/ `exit-code`（异常退出）/ `signal`（被信号终止）/ `core-dump`（产生核心转储）/ `service-failed`（服务启动失败）/ `trigger-limit-hit`（触发频率超限）/ `service-start-limit-hit`（服务启动次数超限） |

### 频率限制

| 属性名称 | 类型 | 读写 | 说明 |
|---------|------|------|------|
| `TriggerLimitIntervalUSec` | `t` | 只读 | 触发频率限制的时间窗口（微秒）；在此窗口内触发次数超过 `TriggerLimitBurst` 则停止套接字 |
| `TriggerLimitBurst` | `u` | 只读 | 触发频率限制的最大触发次数 |
| `PollLimitIntervalUSec` | `t` | 只读 | 轮询事件频率限制的时间窗口（微秒） |
| `PollLimitBurst` | `u` | 只读 | 轮询事件频率限制的最大次数 |

### 延迟触发

| 属性名称 | 类型 | 读写 | 说明 |
|---------|------|------|------|
| `DeferTrigger` | `s` | 只读 | 延迟触发模式配置 |
| `DeferTriggerMaxUSec` | `t` | 只读 | 最大延迟触发等待时间（微秒） |
| `TimeoutUSec` | `t` | 只读 | 套接字激活超时时间（微秒）；超时后停止套接字单元 |

---

## 方法（Methods）

此接口本身不定义额外方法。通用控制方法（启动、停止、重启等）由基础接口 `org.freedesktop.systemd1.Unit` 提供。

---

## 信号（Signals）

此接口本身不定义额外信号。通用信号（如属性变更通知）由 D-Bus 标准接口 `org.freedesktop.DBus.Properties` 提供。

---

## 使用示例

```bash
# 查询 sshd.socket 的监听地址列表
busctl get-property org.freedesktop.systemd1 \
    /org/freedesktop/systemd1/unit/sshd_2esocket \
    org.freedesktop.systemd1.Socket Listen

# 查询当前连接数
busctl get-property org.freedesktop.systemd1 \
    /org/freedesktop/systemd1/unit/sshd_2esocket \
    org.freedesktop.systemd1.Socket NConnections

# 查询是否启用 Accept 模式
busctl get-property org.freedesktop.systemd1 \
    /org/freedesktop/systemd1/unit/sshd_2esocket \
    org.freedesktop.systemd1.Socket Accept
```

## 参见

- [`org.freedesktop.systemd1.Unit`](org.freedesktop.systemd1.Unit.md) — 所有单元类型的基础接口
- `systemd.socket(5)` — 套接字单元配置文件参考手册
- `sd_listen_fds(3)` — 从 systemd 接收套接字文件描述符的 API
