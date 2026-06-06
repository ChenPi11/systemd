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
 * Provide a shim that checks the running kernel version once at startup (via uname(2)).
 * On kernel >= 5.8 calls with AT_SYMLINK_NOFOLLOW are routed to faccessat2.
 * On older kernels AT_SYMLINK_NOFOLLOW is dropped and faccessat is called instead
 * (graceful degradation — the check follows symlinks but avoids the hard EINVAL). */
#include <errno.h>
#include <sys/syscall.h>
#include <sys/utsname.h>
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

/* Check once whether the running kernel supports faccessat2 (requires Linux >= 5.8).
 * The result is cached in a static variable; the worst case is that uname() is called
 * once per translation unit on the first invocation with AT_SYMLINK_NOFOLLOW.
 * Parsing is done without sscanf to avoid pulling in <stdio.h>. */
static inline int _bionic_faccessat2_supported(void) {
        /* -1: not yet checked; 0: not supported; 1: supported.
         * Multiple threads may race to initialize this, but all will write the same
         * value, so the race is benign. */
        static volatile int _cache = -1;
        int val = _cache;
        if (__builtin_expect(val >= 0, 1))
                return val;

        struct utsname uts;
        int supported = 0;
        if (uname(&uts) == 0) {
                const char *p = uts.release;
                unsigned maj = 0, min = 0;
                while (*p >= '0' && *p <= '9')
                        maj = maj * 10 + (unsigned)(*p++ - '0');
                if (*p == '.') {
                        p++;
                        while (*p >= '0' && *p <= '9')
                                min = min * 10 + (unsigned)(*p++ - '0');
                }
                /* faccessat2 was introduced in Linux 5.8 */
                supported = (maj > 5u || (maj == 5u && min >= 8u)) ? 1 : 0;
        }
        _cache = supported;
        return supported;
}

static inline int _bionic_faccessat(int dirfd, const char *pathname, int mode, int flags) {
        /* AT_SYMLINK_NOFOLLOW is rejected by the faccessat syscall; use faccessat2 instead. */
        if ((flags & AT_SYMLINK_NOFOLLOW) && _bionic_faccessat2_supported()) {
#if defined(__NR_faccessat2)
                return (int) syscall(__NR_faccessat2, dirfd, pathname, mode, flags);
#endif
        }
        /* Strip AT_SYMLINK_NOFOLLOW on kernels < 5.8 to avoid EINVAL, accepting that
         * the check will follow symlinks (graceful degradation). */
        flags &= ~AT_SYMLINK_NOFOLLOW;
        /* Use __NR_faccessat directly to avoid an infinite loop via the #define below. */
        return (int) syscall(__NR_faccessat, dirfd, pathname, mode, flags);
}
#define faccessat _bionic_faccessat
