/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include_next <fcntl.h>

/* bionic's fcntl.h does not define AT_* and F_* constants itself — it expects them from
 * <linux/fcntl.h>. When systemd builds with the libc vendor override directory, the override
 * <linux/fcntl.h> shadows the real one (it only adds the PIDFD_* constants, since glibc's own
 * fcntl.h already defines everything else). On bionic that leaves AT_* and F_* undefined, so
 * re-provide them here, mirroring the values from bionic's linux/fcntl.h. */
#include <asm/fcntl.h>
#ifndef AT_FDCWD
#define AT_FDCWD -100
#endif
#ifndef AT_SYMLINK_NOFOLLOW
#define AT_SYMLINK_NOFOLLOW 0x100
#endif
#ifndef AT_SYMLINK_FOLLOW
#define AT_SYMLINK_FOLLOW 0x400
#endif
#ifndef AT_NO_AUTOMOUNT
#define AT_NO_AUTOMOUNT 0x800
#endif
#ifndef AT_EMPTY_PATH
#define AT_EMPTY_PATH 0x1000
#endif
#ifndef AT_EACCESS
#define AT_EACCESS 0x200
#endif
#ifndef AT_REMOVEDIR
#define AT_REMOVEDIR 0x200
#endif
#ifndef AT_HANDLE_FID
#define AT_HANDLE_FID 0x200
#endif
#ifndef AT_RECURSIVE
#define AT_RECURSIVE 0x8000
#endif
#ifndef AT_STATX_SYNC_TYPE
#define AT_STATX_SYNC_TYPE 0x6000
#endif
#ifndef AT_STATX_SYNC_AS_STAT
#define AT_STATX_SYNC_AS_STAT 0x0000
#endif
#ifndef AT_STATX_FORCE_SYNC
#define AT_STATX_FORCE_SYNC 0x2000
#endif
#ifndef AT_STATX_DONT_SYNC
#define AT_STATX_DONT_SYNC 0x4000
#endif
#ifndef AT_RENAME_NOREPLACE
#define AT_RENAME_NOREPLACE 0x0001
#endif
#ifndef AT_RENAME_EXCHANGE
#define AT_RENAME_EXCHANGE 0x0002
#endif
#ifndef AT_RENAME_WHITEOUT
#define AT_RENAME_WHITEOUT 0x0004
#endif

#ifndef F_SETLEASE
#define F_SETLEASE (F_LINUX_SPECIFIC_BASE + 0)
#endif
#ifndef F_GETLEASE
#define F_GETLEASE (F_LINUX_SPECIFIC_BASE + 1)
#endif
#ifndef F_NOTIFY
#define F_NOTIFY (F_LINUX_SPECIFIC_BASE + 2)
#endif
#ifndef F_CANCELLK
#define F_CANCELLK (F_LINUX_SPECIFIC_BASE + 5)
#endif
#ifndef F_DUPFD_CLOEXEC
#define F_DUPFD_CLOEXEC (F_LINUX_SPECIFIC_BASE + 6)
#endif
#ifndef F_SETPIPE_SZ
#define F_SETPIPE_SZ (F_LINUX_SPECIFIC_BASE + 7)
#endif
#ifndef F_GETPIPE_SZ
#define F_GETPIPE_SZ (F_LINUX_SPECIFIC_BASE + 8)
#endif
#ifndef F_ADD_SEALS
#define F_ADD_SEALS (F_LINUX_SPECIFIC_BASE + 9)
#endif
#ifndef F_GET_SEALS
#define F_GET_SEALS (F_LINUX_SPECIFIC_BASE + 10)
#endif
#ifndef F_SEAL_SEAL
#define F_SEAL_SEAL 0x0001
#endif
#ifndef F_SEAL_SHRINK
#define F_SEAL_SHRINK 0x0002
#endif
#ifndef F_SEAL_GROW
#define F_SEAL_GROW 0x0004
#endif
#ifndef F_SEAL_WRITE
#define F_SEAL_WRITE 0x0008
#endif
#ifndef F_SEAL_FUTURE_WRITE
#define F_SEAL_FUTURE_WRITE 0x0010
#endif
#ifndef F_SEAL_EXEC
#define F_SEAL_EXEC 0x0020
#endif
#ifndef DN_ACCESS
#define DN_ACCESS 0x00000001
#endif
#ifndef DN_MODIFY
#define DN_MODIFY 0x00000002
#endif
#ifndef DN_CREATE
#define DN_CREATE 0x00000004
#endif
#ifndef DN_DELETE
#define DN_DELETE 0x00000008
#endif
#ifndef DN_RENAME
#define DN_RENAME 0x00000010
#endif
#ifndef DN_ATTRIB
#define DN_ATTRIB 0x00000020
#endif
#ifndef DN_MULTISHOT
#define DN_MULTISHOT 0x80000000
#endif

/* struct file_handle and open_by_handle_at() are GNU extensions provided by glibc's fcntl.h
 * via bits/fcntl-linux.h. Android bionic does not expose them in its NDK headers, even though
 * the underlying syscalls exist. Provide the definitions here. */

#ifndef __file_handle_defined
#define __file_handle_defined 1

#include <sys/syscall.h>
#include <unistd.h>

struct file_handle {
        unsigned int handle_bytes;
        int handle_type;
        unsigned char f_handle[0];
};

static inline int name_to_handle_at(
                int dfd,
                const char *name,
                struct file_handle *handle,
                int *mnt_id,
                int flags) {
        return (int) syscall(__NR_name_to_handle_at, dfd, name, handle, mnt_id, flags);
}

static inline int open_by_handle_at(int mountdirfd, struct file_handle *handle, int flags) {
        return (int) syscall(__NR_open_by_handle_at, mountdirfd, handle, flags);
}

#endif /* __file_handle_defined */

/* openat2() was added in Linux 5.6 (syscall number 437 on most architectures).
 * Android NDK r22b+ kernel sysroots define __NR_openat2; older sysroots do not.
 * DEFINE_SYSCALL_SHIM(openat2, …) in src/libc/fcntl.c falls back to
 * syscall(__NR_openat2, …) when the libc symbol is absent at runtime, so the
 * number must be defined at compile time.  Provide per-arch fallback definitions
 * here so that builds against old NDK sysroots do not fail. */
#ifndef __NR_openat2
#  if defined(__aarch64__)
#    define __NR_openat2  437
#  elif defined(__arm__)
#    define __NR_openat2  437
#  elif defined(__i386__)
#    define __NR_openat2  437
#  elif defined(__x86_64__)
#    define __NR_openat2  437
#  elif defined(__riscv)
#    define __NR_openat2  437
#  elif defined(__mips__)
#    include <asm/sgidefs.h>
#    if _MIPS_SIM == _MIPS_SIM_ABI32
#      define __NR_openat2  4437
#    elif _MIPS_SIM == _MIPS_SIM_NABI32
#      define __NR_openat2  6437
#    else /* _MIPS_SIM_ABI64 */
#      define __NR_openat2  5437
#    endif
#  endif
#endif /* __NR_openat2 */

