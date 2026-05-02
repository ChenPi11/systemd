# 接口名称：org.freedesktop.systemd1.Target

## 接口说明

`org.freedesktop.systemd1.Target` 是 systemd 中**目标单元（`.target`）的 D-Bus 接口**。目标单元本身不执行任何操作，而是作为**同步点（synchronization point）**，将其他单元组织成逻辑分组，类似于 SysV init 中的运行级别（runlevel）概念。

目标单元的典型用途包括：

- **表示系统状态**：如 `multi-user.target`（多用户模式）、`graphical.target`（图形界面模式）
- **组织启动顺序**：通过 `Wants=`、`Requires=`、`After=`、`Before=` 等依赖关系控制单元的启动顺序
- **提供激活触发点**：其他单元可以 `WantedBy=` 某个目标，在该目标激活时被一并启动

> **注意**：目标单元接口**没有自己特有的属性和方法**，全部功能均通过继承自 `org.freedesktop.systemd1.Unit` 的接口提供。

## 总线名称

`org.freedesktop.systemd1`

## 继承关系

```
org.freedesktop.systemd1.Unit
    └── org.freedesktop.systemd1.Target
```

## 对象路径

`/org/freedesktop/systemd1/unit/<target_name>`

其中 `<target_name>` 是目标单元名称经过 D-Bus 路径转义后的形式（例如 `multi-user.target` 对应 `/org/freedesktop/systemd1/unit/multi_2duser_2etarget`）。

---

## 属性（Properties）

目标单元**不定义任何特有属性**。所有属性均继承自 `org.freedesktop.systemd1.Unit` 接口，包括但不限于：

| 属性名称（继承自 Unit） | 类型 | 说明 |
|------------------------|------|------|
| `Id` | `s` | 单元的主名称（如 `multi-user.target`） |
| `Names` | `as` | 单元的所有名称（包括别名） |
| `ActiveState` | `s` | 当前活跃状态：`active`（已激活）/ `inactive`（未激活）/ `activating`（激活中）/ `deactivating`（停用中）/ `failed` |
| `LoadState` | `s` | 加载状态：`loaded`（已加载）/ `not-found`（未找到）/ `error`（加载错误）/ `masked`（已屏蔽） |
| `SubState` | `s` | 目标单元的子状态：`dead`（未激活）/ `active`（已激活） |
| `Description` | `s` | 单元的描述文本 |
| `Following` | `s` | 若此单元跟随另一单元的状态，则为那个单元的名称 |
| `Requires` | `as` | 强依赖单元列表 |
| `Wants` | `as` | 弱依赖单元列表 |
| `After` | `as` | 在这些单元之后启动 |
| `Before` | `as` | 在这些单元之前启动 |
| `ActiveEnterTimestamp` | `t` | 进入 `active` 状态的时间戳（微秒，CLOCK_REALTIME） |
| `InactiveEnterTimestamp` | `t` | 进入 `inactive` 状态的时间戳 |
| `CanStop` | `b` | 是否可以被停止 |
| `CanReload` | `b` | 是否可以被重载（目标单元通常为 `false`） |

> 完整属性列表请参见 [`org.freedesktop.systemd1.Unit`](org.freedesktop.systemd1.Unit.md) 接口文档。

---

## 方法（Methods）

目标单元**不定义任何特有方法**。通用控制方法（启动、停止等）均由基础接口 `org.freedesktop.systemd1.Unit` 提供。

---

## 信号（Signals）

目标单元**不定义任何特有信号**。

---

## 常见系统目标单元

| 目标名称 | 说明 |
|---------|------|
| `default.target` | 系统默认启动目标（通常链接到 `graphical.target` 或 `multi-user.target`） |
| `multi-user.target` | 多用户非图形界面模式（对应 SysV runlevel 2、3、4） |
| `graphical.target` | 图形界面多用户模式（对应 SysV runlevel 5） |
| `rescue.target` | 单用户救援模式 |
| `emergency.target` | 紧急模式（最小化系统，仅挂载根文件系统） |
| `sysinit.target` | 系统初始化阶段同步点 |
| `basic.target` | 基础系统初始化完成同步点 |
| `network.target` | 网络初始化完成同步点 |
| `network-online.target` | 网络在线（可路由）同步点 |
| `shutdown.target` | 系统关机同步点 |
| `reboot.target` | 系统重启同步点 |
| `poweroff.target` | 系统关机断电同步点 |

---

## 使用示例

```bash
# 查询 multi-user.target 的当前状态
busctl get-property org.freedesktop.systemd1 \
    /org/freedesktop/systemd1/unit/multi_2duser_2etarget \
    org.freedesktop.systemd1.Unit ActiveState

# 切换到多用户目标（通过 Unit 接口的 Start 方法）
busctl call org.freedesktop.systemd1 \
    /org/freedesktop/systemd1/unit/multi_2duser_2etarget \
    org.freedesktop.systemd1.Unit Start s "isolate"

# 查询目标的 Wants 依赖
busctl get-property org.freedesktop.systemd1 \
    /org/freedesktop/systemd1/unit/multi_2duser_2etarget \
    org.freedesktop.systemd1.Unit Wants
```

## 参见

- [`org.freedesktop.systemd1.Unit`](org.freedesktop.systemd1.Unit.md) — 所有单元类型的基础接口
- `systemd.target(5)` — 目标单元配置文件参考手册
- `systemd-analyze(1)` — 分析系统启动依赖图
