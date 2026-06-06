/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

/* <linux/openat2.h> was added to the Android NDK in r22b (targeting API level 33).
 * Older NDK sysroots do not ship this header, which causes build failures when
 * override/fcntl.h unconditionally includes it.  Intercept the include here (bionic/
 * has higher priority in the -isystem chain) and either forward to the real header
 * when available, or provide the minimal definitions needed by systemd's openat2()
 * wrapper (DEFINE_SYSCALL_SHIM in src/libc/fcntl.c). */

#if __has_include_next(<linux/openat2.h>)
#  include_next <linux/openat2.h>    /* IWYU pragma: export */
#else
#  include <linux/types.h>

/* Matches the kernel ABI from linux/openat2.h (added in Linux 5.6). */
struct open_how {
        __u64 flags;   /* O_* flags */
        __u64 mode;    /* Mode for O_CREAT / O_TMPFILE */
        __u64 resolve; /* RESOLVE_* flags */
};

/* RESOLVE_* flags for open_how.resolve — kernel v5.6+. */
#  define RESOLVE_NO_XDEV       0x01  /* Block mount-point crossings */
#  define RESOLVE_NO_MAGICLINKS 0x02  /* Block magic-link traversal */
#  define RESOLVE_NO_SYMLINKS   0x04  /* Block all symlink traversal */
#  define RESOLVE_BENEATH       0x08  /* Block escapes from dirfd's subtree */
#  define RESOLVE_IN_ROOT       0x10  /* Pretend dirfd is FS root */
#  define RESOLVE_CACHED        0x20  /* Only complete if fully cached */
#endif
