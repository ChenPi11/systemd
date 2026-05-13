# 接口名称：org.freedesktop.systemd1.Swap

## 接口说明

`org.freedesktop.systemd1.Swap` 是 systemd 中**交换单元（`.swap`）的 D-Bus 接口**。交换单元用于管理 Linux 交换分区或交换文件的激活与停用，可以对应手动创建的 `.swap` 单元文件，也可以由 systemd 根据 `/etc/fstab` 中带有 `swap` 类型的条目自动生成。

通过此接口，客户端可以：

- **查询交换设备配置**：交换设备路径、优先级、选项等
- **查询运行状态**：控制进程 PID、操作结果等
- **监控交换区状态**：通过 `ActiveState` 属性感知交换区的启用/停用状态

> **注意**：此接口同时继承 `org.freedesktop.systemd1.Unit` 接口的全部属性和方法，以及执行上下文（ExecuteContext）和 CGroup 上下文的全部属性。

## 总线名称

`org.freedesktop.systemd1`

## 继承关系

```
org.freedesktop.systemd1.Unit
    └── org.freedesktop.systemd1.Swap
```

## 对象路径

`/org/freedesktop/systemd1/unit/<swap_name>`

其中 `<swap_name>` 是交换单元名称经过 D-Bus 路径转义后的形式。交换单元名称由交换设备路径转换而来（例如 `/dev/sda2` 对应单元 `dev-sda2.swap`，路径为 `/org/freedesktop/systemd1/unit/dev_2dsda2_2eswap`；交换文件 `/swapfile` 对应 `swapfile.swap`）。

---

## 属性（Properties）

### 交换配置

| 属性名称 | 类型 | 读写 | 说明 |
|---------|------|------|------|
| `What` | `s` | 只读 | 交换设备或交换文件的绝对路径（如 `/dev/sda2`、`/swapfile`） |
| `Priority` | `i` | 只读 | 交换优先级（`-1` 表示由内核自动分配；值越高优先级越高，取值范围 `-1` 到 `32767`） |
| `Options` | `s` | 只读 | 传递给 `swapon(8)` 的额外选项字符串（如 `discard`、`discard=pages`） |
| `TimeoutUSec` | `t` | 只读 | 激活/停用交换区操作的超时时间（微秒）；超时后视为失败 |

### 运行状态

| 属性名称 | 类型 | 读写 | 说明 |
|---------|------|------|------|
| `ControlPID` | `u` | 只读 | 当前控制进程（执行 `swapon`/`swapoff` 的进程）的 PID；无则为 `0` |
| `UID` | `u` | 只读 | 控制进程的有效用户 ID |
| `GID` | `u` | 只读 | 控制进程的有效组 ID |
| `Result` | `s` | 只读 | 交换操作的最终结果：`success` / `resources`（资源不足）/ `timeout`（操作超时）/ `exit-code`（swapon/swapoff 命令异常退出）/ `signal`（被信号终止）/ `core-dump`（产生核心转储） |

---

## 方法（Methods）

交换单元**不定义任何特有方法**。通用控制方法（启动对应激活交换区、停止对应停用交换区）由基础接口 `org.freedesktop.systemd1.Unit` 提供。

---

## 信号（Signals）

交换单元**不定义任何特有信号**。

---

## 交换单元命名规则

交换设备路径到单元名称的转换规则：

| 交换设备/文件 | 对应单元名称 |
|-------------|-------------|
| `/dev/sda2` | `dev-sda2.swap` |
| `/dev/mapper/vg-swap` | `dev-mapper-vg\x2dswap.swap` |
| `/swapfile` | `swapfile.swap` |
| `/var/swap.img` | `var-swap.img.swap` |

---

## 使用示例

```bash
# 查询交换设备路径
busctl get-property org.freedesktop.systemd1 \
    /org/freedesktop/systemd1/unit/dev_2dsda2_2eswap \
    org.freedesktop.systemd1.Swap What

# 查询交换优先级
busctl get-property org.freedesktop.systemd1 \
    /org/freedesktop/systemd1/unit/dev_2dsda2_2eswap \
    org.freedesktop.systemd1.Swap Priority

# 查询操作结果
busctl get-property org.freedesktop.systemd1 \
    /org/freedesktop/systemd1/unit/dev_2dsda2_2eswap \
    org.freedesktop.systemd1.Swap Result

# 通过 Unit 接口激活交换区
busctl call org.freedesktop.systemd1 \
    /org/freedesktop/systemd1/unit/dev_2dsda2_2eswap \
    org.freedesktop.systemd1.Unit Start s "replace"

# 通过 Unit 接口停用交换区
busctl call org.freedesktop.systemd1 \
    /org/freedesktop/systemd1/unit/dev_2dsda2_2eswap \
    org.freedesktop.systemd1.Unit Stop s "replace"

# 列出所有交换单元
systemctl list-units --type=swap
```

## 与 /etc/fstab 的关系

systemd 会自动将 `/etc/fstab` 中类型为 `swap` 的条目转换为对应的 `.swap` 单元。示例 fstab 条目：

```
/dev/sda2   none   swap   sw,pri=1   0   0
/swapfile   none   swap   defaults   0   0
```

`noauto` 选项可阻止交换区在启动时自动激活。

## 参见

- [`org.freedesktop.systemd1.Unit`](org.freedesktop.systemd1.Unit.md) — 所有单元类型的基础接口
- `systemd.swap(5)` — 交换单元配置文件参考手册
- `swapon(8)` — 激活交换区命令
- `swapoff(8)` — 停用交换区命令
- `fstab(5)` — 文件系统静态配置文件格式
