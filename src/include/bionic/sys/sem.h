/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include_next <sys/sem.h>

/* semctl(), semget(), and semop() were added to Android bionic headers in API level 26.
 * On older API levels the libc functions are not declared in headers; provide syscall-based wrappers
 * instead. The underlying kernel syscalls have been available since the beginning of Android.
 *
 * Note: bionic declares semctl() with __RENAME(libandroid_semctl), so a static inline with the
 * same name would cause a "static declaration of 'libandroid_semctl' follows non-static
 * declaration" conflict. Use private helper functions and macro redirects instead. */
#if !defined(__ANDROID_API__) || __ANDROID_API__ < 26
#  include <stdarg.h>
#  include <sys/syscall.h>
#  include <unistd.h>
static inline int _systemd_semctl(int semid, int semnum, int cmd, ...) {
        /* The optional 4th argument is a union semun; pass it through as an unsigned long.
         * For commands that do not use the argument (e.g. IPC_RMID) the kernel ignores it. */
        va_list ap;
        unsigned long arg = 0;
        va_start(ap, cmd);
        arg = va_arg(ap, unsigned long);
        va_end(ap);
        return (int) syscall(__NR_semctl, semid, semnum, cmd, arg);
}
static inline int _systemd_semget(key_t key, int nsems, int semflg) {
        return (int) syscall(__NR_semget, key, nsems, semflg);
}
static inline int _systemd_semop(int semid, struct sembuf *sops, size_t nsops) {
        return (int) syscall(__NR_semop, semid, sops, nsops);
}
#  define semctl _systemd_semctl
#  define semget _systemd_semget
#  define semop  _systemd_semop
#endif
