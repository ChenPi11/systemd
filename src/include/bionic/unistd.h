/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

/* getopt() is provided both in getopt.h and unistd.h. Hence, we need to tentatively undefine it. */
#undef getopt

#include_next <unistd.h>

/* bionic's getopt() always behaves in POSIXLY_CORRECT mode, and stops parsing arguments when a non-option
 * string is found. Let's always use getopt_long(). */
int getopt_fix(int argc, char * const *argv, const char *optstring);
#define getopt(argc, argv, optstring) getopt_fix(argc, argv, optstring)

int missing_close_range(unsigned first_fd, unsigned end_fd, unsigned flags);
#define close_range missing_close_range

int missing_execveat(int dirfd, const char *pathname, char * const argv[], char * const envp[], int flags);
#define execveat missing_execveat

/* get_current_dir_name() is a GNU extension not available in Android bionic. Implement it as a
 * wrapper around getcwd(NULL, 0), which bionic supports and allocates the buffer dynamically. */
static inline char *get_current_dir_name(void) {
        return getcwd(NULL, 0);
}

/* getdtablesize() is a legacy POSIX function not declared in Android bionic's unistd.h.
 * Implement it using getrlimit(RLIMIT_NOFILE) to return the current open file limit. */
#include <sys/resource.h>
static inline int getdtablesize(void) {
        struct rlimit rl;
        if (getrlimit(RLIMIT_NOFILE, &rl) < 0)
                return 256; /* POSIX minimum */
        return (int) rl.rlim_cur;
}

/* copy_file_range() was added to Android bionic headers in API level 34. Provide a
 * syscall-based wrapper for older API levels; the syscall has been available since kernel 4.5. */
#if !defined(__ANDROID_API__) || __ANDROID_API__ < 34
#  include <sys/syscall.h>
static inline ssize_t copy_file_range(int fd_in, off64_t *off_in, int fd_out, off64_t *off_out,
                                      size_t len, unsigned int flags) {
        return (ssize_t) syscall(__NR_copy_file_range, fd_in, off_in, fd_out, off_out, len, flags);
}
#endif

/* syncfs() was added to Android bionic headers in API level 28. Provide a syscall-based wrapper
 * for older API levels; the syscall has been available in the kernel since Linux 2.6.39. */
#if !defined(__ANDROID_API__) || __ANDROID_API__ < 28
#  include <sys/syscall.h>
static inline int syncfs(int fd) {
        return (int) syscall(__NR_syncfs, fd);
}
#endif

/* The Linux faccessat(2) syscall never supports AT_SYMLINK_NOFOLLOW (returns EINVAL).
 * faccessat2(2) (Linux 5.8+, syscall 439 on most architectures) does support it.
 * Android bionic wraps faccessat syscall directly without a fallback, so any call with
 * AT_SYMLINK_NOFOLLOW always fails with EINVAL.
 *
 * Provide a shim that routes calls with AT_SYMLINK_NOFOLLOW through faccessat2 via direct
 * syscall, and falls back to faccessat without the flag on kernels that predate faccessat2
 * (graceful degradation — the check follows symlinks but avoids the hard EINVAL). */
#include <sys/syscall.h>
#include <fcntl.h>

/* __NR_faccessat2 may not be defined in older NDK sysroots. Provide per-arch fallbacks. */
#ifndef __NR_faccessat2
#  if defined(__aarch64__) || defined(__arc__) || defined(__arm__)   || \
      defined(__i386__)    || defined(__m68k__) || defined(__riscv)  || \
      defined(__s390__)    || defined(__sh__)   || defined(__sparc__) || \
      defined(__x86_64__)  || defined(__loongarch_lp64) || defined(__hppa__) || \
      defined(__powerpc__)
#    define __NR_faccessat2 439
#  elif defined(__alpha__)
#    define __NR_faccessat2 549
#  elif defined(__ia64__)
#    define __NR_faccessat2 1463
#  elif defined(__mips__)
#    include <asm/sgidefs.h>
#    if _MIPS_SIM == _MIPS_SIM_ABI32
#      define __NR_faccessat2 4439
#    elif _MIPS_SIM == _MIPS_SIM_NABI32
#      define __NR_faccessat2 6439
#    else /* _MIPS_SIM_ABI64 */
#      define __NR_faccessat2 5439
#    endif
#  endif
#endif /* __NR_faccessat2 */

static inline int _bionic_faccessat(int dirfd, const char *pathname, int mode, int flags) {
        /* AT_SYMLINK_NOFOLLOW is rejected by the faccessat syscall; use faccessat2 instead. */
        if (flags & AT_SYMLINK_NOFOLLOW) {
#if defined(__NR_faccessat2)
                int r = (int) syscall(__NR_faccessat2, dirfd, pathname, mode, flags);
                if (r >= 0 || errno != ENOSYS)
                        return r;
                /* faccessat2 not available on this kernel; degrade by dropping AT_SYMLINK_NOFOLLOW.
                 * The check will follow symlinks, but this is preferable to always returning EINVAL. */
                flags &= ~AT_SYMLINK_NOFOLLOW;
#endif
        }
        /* Use __NR_faccessat directly to avoid an infinite loop via the #define below. */
        return (int) syscall(__NR_faccessat, dirfd, pathname, mode, flags);
}
#define faccessat _bionic_faccessat
