/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include_next <fcntl.h>

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

