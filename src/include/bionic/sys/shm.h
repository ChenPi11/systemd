/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include_next <sys/shm.h>

/* shmctl(), shmget(), shmat(), and shmdt() were added to Android bionic headers in API level 26.
 * On older API levels the libc functions are not declared in headers; provide syscall-based wrappers
 * instead. The underlying kernel syscalls have been available since the beginning of Android.
 *
 * Note: some bionic versions declare these with __RENAME(), so avoid redeclaring them as static
 * inline by using private helper functions and macro redirects instead. */
#if !defined(__ANDROID_API__) || __ANDROID_API__ < 26
#  include <sys/syscall.h>
#  include <unistd.h>
static inline int _systemd_shmctl(int shmid, int cmd, struct shmid_ds *buf) {
        return (int) syscall(__NR_shmctl, shmid, cmd, buf);
}
static inline int _systemd_shmget(key_t key, size_t size, int shmflg) {
        return (int) syscall(__NR_shmget, key, size, shmflg);
}
static inline void *_systemd_shmat(int shmid, const void *shmaddr, int shmflg) {
        return (void *) syscall(__NR_shmat, shmid, shmaddr, shmflg);
}
static inline int _systemd_shmdt(const void *shmaddr) {
        return (int) syscall(__NR_shmdt, shmaddr);
}
#  define shmctl _systemd_shmctl
#  define shmget _systemd_shmget
#  define shmat  _systemd_shmat
#  define shmdt  _systemd_shmdt
#endif
