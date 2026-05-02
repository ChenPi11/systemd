# 接口名称：org.freedesktop.systemd1.Automount

## 接口说明

`org.freedesktop.systemd1.Automount` 是 systemd 中**自动挂载单元（`.automount`）的 D-Bus 接口**。自动挂载单元实现**按需挂载（on-demand mounting）**——当有进程访问指定挂载点时，systemd 才自动触发对应的 `.mount` 单元完成挂载。

自动挂载单元的典型用途包括：

- **延迟挂载网络文件系统**：避免系统启动时因网络不可达导致挂载超时而阻塞启动
- **按需挂载大型文件系统**：减少不必要的资源占用
- **空闲自动卸载**：通过 `TimeoutIdleUSec` 配置，在一段时间无访问后自动卸载

> **注意**：自动挂载单元需要与同名的 `.mount` 单元配合使用。例如 `home.automount` 会在访问 `/home` 时触发 `home.mount`。

## 总线名称

`org.freedesktop.systemd1`

## 继承关系

```
org.freedesktop.systemd1.Unit
    └── org.freedesktop.systemd1.Automount
```

## 对象路径

`/org/freedesktop/systemd1/unit/<automount_name>`

其中 `<automount_name>` 是自动挂载单元名称经过 D-Bus 路径转义后的形式（例如 `home.automount` 对应 `/org/freedesktop/systemd1/unit/home_2eautomount`）。

---

## 属性（Properties）

| 属性名称 | 类型 | 读写 | 说明 |
|---------|------|------|------|
| `Where` | `s` | 只读 | 自动挂载监控的挂载点绝对路径（如 `/home`、`/mnt/nfs`）。当有进程访问此路径时，systemd 将触发对应的 `.mount` 单元 |
| `ExtraOptions` | `s` | 只读 | 传递给内核 autofs 驱动的额外选项字符串 |
| `DirectoryMode` | `u` | 只读 | 自动创建挂载点目录时使用的文件权限（八进制，如 `0755`） |
| `Result` | `s` | 只读 | 自动挂载单元的最终结果：`success` / `resources`（资源不足）/ 其他错误状态 |
| `TimeoutIdleUSec` | `t` | 只读 | 空闲超时时间（微秒）。挂载点在此时间内无活跃引用后，systemd 将自动卸载对应的 `.mount` 单元。`0` 表示禁用自动卸载，挂载后一直保持 |

---

## 方法（Methods）

自动挂载单元**不定义任何特有方法**。通用控制方法由基础接口 `org.freedesktop.systemd1.Unit` 提供。

> **注意**：启动自动挂载单元（`Start`）会在挂载点处安装 autofs 监控点；停止（`Stop`）则会移除监控点。实际的文件系统挂载/卸载由关联的 `.mount` 单元完成。

---

## 信号（Signals）

自动挂载单元**不定义任何特有信号**。

---

## 工作原理

```
进程访问 /home
    │
    ▼
内核 autofs 拦截访问并暂停进程
    │
    ▼
systemd 收到通知，启动 home.mount
    │
    ▼
home.mount 挂载完成
    │
    ▼
内核恢复被暂停的进程，访问成功
    │
    ▼
（若配置了 TimeoutIdleUSec）空闲超时后自动卸载 home.mount
```

## 使用示例

```bash
# 查询自动挂载点路径
busctl get-property org.freedesktop.systemd1 \
    /org/freedesktop/systemd1/unit/home_2eautomount \
    org.freedesktop.systemd1.Automount Where

# 查询空闲自动卸载超时时间
busctl get-property org.freedesktop.systemd1 \
    /org/freedesktop/systemd1/unit/home_2eautomount \
    org.freedesktop.systemd1.Automount TimeoutIdleUSec

# 查询自动挂载单元结果
busctl get-property org.freedesktop.systemd1 \
    /org/freedesktop/systemd1/unit/home_2eautomount \
    org.freedesktop.systemd1.Automount Result

# 启动自动挂载单元（开始监控挂载点）
busctl call org.freedesktop.systemd1 \
    /org/freedesktop/systemd1/unit/home_2eautomount \
    org.freedesktop.systemd1.Unit Start s "replace"
```

## 配置示例

创建 `/etc/systemd/system/mnt-nfs.automount`：

```ini
[Unit]
Description=NFS Share 自动挂载

[Automount]
Where=/mnt/nfs
TimeoutIdleSec=300

[Install]
WantedBy=multi-user.target
```

同时创建 `/etc/systemd/system/mnt-nfs.mount`：

```ini
[Unit]
Description=NFS Share

[Mount]
What=server:/export/share
Where=/mnt/nfs
Type=nfs
Options=defaults,_netdev
```

## 参见

- [`org.freedesktop.systemd1.Unit`](org.freedesktop.systemd1.Unit.md) — 所有单元类型的基础接口
- [`org.freedesktop.systemd1.Mount`](org.freedesktop.systemd1.Mount.md) — 挂载单元接口
- `systemd.automount(5)` — 自动挂载单元配置文件参考手册
- `autofs(5)` — Linux 内核 autofs 文件系统
