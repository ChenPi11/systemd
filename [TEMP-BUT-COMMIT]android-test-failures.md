# Termux/Android 单元测试失败分析

> 测试环境: Termux on Android (aarch64, bionic libc)
> 编译配置: systemd 262, `-Dlibc=bionic`, clang 编译器
> 测试命令: `meson test -C build --timeout-multiplier 2 --suite systemd`
> 总测试: 4661 | 通过: 1452 | 失败: 3173 | 跳过: 35 | 超时: 1

---

## 一、可修复的失败

### 1.1 SYSTEM_TMPDIR 指向不可写的 `/tmp`（约 50+ 测试受影响）

**根因**: `SYSTEM_TMPDIR` 硬编码为 `/tmp`，但 Android 上 `/tmp` 不可写（Permission denied）。
Termux 的临时目录是 `$TMPDIR` → `/data/data/com.termux/files/usr/tmp`。

**修复方案**: 在 meson setup 时传入 `-Dtmpdir=$TMPDIR`。

**受影响的测试**（均因 `mkostemp_safe`/`mkdtemp_malloc`/`fmkostemp_safe` 等调用返回 EACCES）：

| 测试 | 具体断言位置 | 错误信息 |
|------|-------------|---------|
| `test-async` | `src/test/test-async.c:41` | `fd` → EACCES |
| `test-fileio` | `src/test/test-fileio.c:31` | `fmkostemp_safe(t, "w", &f)` → EACCES |
| `test-kexec` | `src/test/test-kexec.c:90` | `mkostemp_safe(path)` → EACCES |
| `test-clock` | `src/test/test-clock.c:44` | `fmkostemp_safe(adjtime, "w", &f)` → EACCES |
| `test-rm-rf` | `src/test/test-rm-rf.c:20` | `mkdtemp_malloc("/tmp/test-rm-rf.XXXXXXX", &d)` → EACCES |
| `test-catalog` | `src/libsystemd/sd-journal/test-catalog.c:211` | `mkostemp_safe(db)` → EACCES |
| `test-path` | `src/shared/tests.c:41` | `mkdtemp_malloc(SYSTEM_TMPDIR "/fake-xdg-runtime-XXXXXX", &t)` → EACCES |
| `test-varlink` | `src/libsystemd/sd-varlink/test-varlink.c:356` | `mkdtemp_malloc(SYSTEM_TMPDIR "/varlink-test-XXXXXX", &tmpdir)` → EACCES |
| `test-chase` | `src/test/test-chase.c:46` | `mkdtemp(temp)` → EACCES |
| `test-env-file` | `src/test/test-env-file.c:64` | `write_tmpfile(name, env_file_1)` → EACCES |
| `test-sigbus` | `src/test/test-sigbus.c:38` | `mkostemp(template, ...)` → EACCES |
| `test-pretty-print` | `src/test/test-pretty-print.c:62` | `mkdtemp_malloc("/tmp/test-cat-files-XXXXXX", &tmp)` → EACCES |
| `test-notify-recv` | `src/test/test-notify-recv.c:99` | `fd1` → EACCES |
| `test-copy` | `src/test/test-copy.c:40` | `fd >= 0` → EACCES |
| 所有 fuzz-* 测试 | 各种 temp 文件操作 | EACCES |

**可修复性**: ✅ **高** — 只需调整 `-Dtmpdir=` 编译选项或设置 `TMPDIR` 环境变量。

---

### 1.2 缺失共享库（约 3 个测试受影响）

**根因**: 某些共享库在 Termux 上未安装或版本名不同。

| 测试 | 错误信息 |
|------|---------|
| `test-dlopen-so` | `libbz2.so.1` not found → EOPNOTSUPP |
| `test-compress` | `compression failed: No buffer space available`（可能与 lz4/lzma 配置有关） |

**修复方案**: 安装缺失的 bzip2 库 (`pkg install bzip2`)。compress 的 No buffer space available 可能是 Android 的内存限制或有问题的 liblzma 版本。

**可修复性**: ✅ **中** — 安装 bzip2 即可修复 dlopen-so。compress 可能需要排查 liblzma 版本兼容性。

---

### 1.3 dist 类测试 — help/version 检查（约 100+ 测试受影响）

**根因**: `check-help-*` 和 `check-version-*` 测试找不到构建出的二进制文件，退出码 3。
这通常是因为 `PATH` 环境变量未包含构建目录，或者二进制文件运行时缺少依赖。

**修复方案**: 确保 `PATH` 包含构建目录。

**可修复性**: ✅ **高** — 运行测试前正确设置 PATH。

---

## 二、可部分修复的失败

### 2.1 bionic/glibc 行为差异

| 测试 | 错误信息 | 根因分析 |
|------|---------|---------|
| `test-calendarspec` | `Assertion 'y == x'` at `test_timestamp()` | bionic 的 `localtime_r` / 时区处理与 glibc 在亚秒级精度上有差异 |
| `test-condition` | `condition_test(condition, environ)` 返回 0 | 测试依赖的系统路径（如 `/proc`、`/sys`）在 Android 上布局不同 |
| `test-hostname-setup` | `_fd_ >= 0` at `test_read_etc_hostname()` | `/etc/hostname` 在 Android/Termux 上不存在 |

**可修复性**: 🟡 **低** — 需要为 bionic 适配测试用例的期望值。涉及条件编译或修改测试断言，改动较大。

---

## 三、不可修复的失败（Android 平台根本限制）

### 3.1 CGroup 不可用（约 6 个 core 测试）

**根因**: Android 内核未挂载 systemd 所需的 cgroup 层级。

**受影响的测试**: `test-engine`, `test-load-fragment`, `test-unit-name`, `test-unit-serialize`, `test-path`, `test-sched-prio`

全部在 `cg_mask_supported(&supported)` 处返回 `ESRCH`。

**可修复性**: ❌ **不可修复** — Android 不使用 systemd 作为 init 系统。

### 3.2 D-Bus 不可用

**根因**: systemd 不是 PID 1，D-Bus 未运行。

**受影响的测试**: `test-bus-creds`, `test-engine`, `test-load-fragment`, `test-unit-serialize`

报错: `Failed to connect to system bus: No medium found` 或 `sd_bus_creds_new_from_pid: No such process`

**可修复性**: ❌ **不可修复** — D-Bus 不是 Android 的基础设施。

### 3.3 User/Network Namespace 不支持

| 测试 | 错误信息 |
|------|---------|
| `test-loopback` | `Failed to create user+network namespace: Invalid argument` |

**可修复性**: ❌ **不可修复** — Android 内核未启用 `CONFIG_USER_NS`。

### 3.4 logind/sd-login 不可用

**受影响的测试**: `test-sd-login`, `test-login`

全部在 `sd_pid_get_session(0)` → `ESRCH`。logind 未运行。

**可修复性**: ❌ **不可修复** — logind 需要 systemd 用户会话。

### 3.5 所有集成测试

`TEST-01-BASIC` 到 `TEST-93-CLONESETUP` 等全部约 45+ 个集成测试全部失败。

**可修复性**: ❌ **不可修复** — 这些测试需要 systemd 作为 PID 1 在虚拟机或容器中运行。

### 3.6 平台内核特性差异

| 测试 | 错误信息 | 根因 |
|------|---------|------|
| `test-id128` | `Unknown signal 31` | 内核或 bionic 不支持 `MADV_COLD`/`pkey_*` 等高级内存管理 API |
| `test-os-util` | `Unknown signal 31` | 同上 — 内核接口差异 |
| `test-stat-util` | `Unknown signal 31` | 同上 |
| `test-fd-util` | `fd_validate(fds[2]) >= 0` | Android 的 RLIMIT_NOFILE 或 fd 表行为与 glibc 不同 |
| `test-procfs-util` | `procfs_cpu_get_usage(&nsec) >= 0` | `/proc/stat` 格式可能不同 |
| `test-binfmt-util` | `binfmt_mounted_and_writable()` → ENOENT | Android 未挂载 `binfmt_misc` |
| `test-sd-device-thread` | udev 相关测试 | Android 无 udev |
| `test-udev-device-thread` | 同上 | 同上 |
| `test-argv-util` | `test_rename_process_now` 断言失败 | `prctl(PR_SET_NAME)` 行为差异 |

**可修复性**: ❌ **不可修复** — 这些是 Android 内核与 glibc/Linux 系统的根本差异。

---

## 四、总结

| 类别 | 数量 | 可修复 |
|------|------|--------|
| SYSTEM_TMPDIR 不可写 | ~50+ | ✅ 是（改编译选项） |
| 缺失共享库 | ~3 | ✅ 是（安装包） |
| dist help/version 检查 | ~100 | ✅ 是（设 PATH） |
| bionic/glibc 差异 | ~4 | 🟡 部分 |
| CGroup/D-Bus/logind | ~15 | ❌ 否 |
| 命名空间 | ~1 | ❌ 否 |
| 平台内核差异 | ~10 | ❌ 否 |
| 集成测试 | ~45 | ❌ 否 |
| clang-tidy 警告检查 | ~3100 | ❌ 否（与功能无关） |

> 注: clang-tidy 测试占 3173 个失败中的绝大多数（约 3100 个），这些不是功能测试，
> 而是代码静态分析检查，在 Termux 的 clang-tidy 配置下大概率因找不到正确的系统头文件而失败。
