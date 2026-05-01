/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include_next <sys/msg.h>

/* msgctl(), msgget(), msgsnd(), and msgrcv() were added to Android bionic headers in API level 26.
 * On older API levels the libc functions are not declared in headers; provide syscall-based wrappers
 * instead. The underlying kernel syscalls have been available since the beginning of Android.
 *
 * Note: some bionic versions declare these with __RENAME(), so avoid redeclaring them as static
 * inline by using private helper functions and macro redirects instead. */
#if !defined(__ANDROID_API__) || __ANDROID_API__ < 26
#  include <sys/syscall.h>
#  include <unistd.h>
static inline int _systemd_msgctl(int msqid, int cmd, struct msqid_ds *buf) {
        return (int) syscall(__NR_msgctl, msqid, cmd, buf);
}
static inline int _systemd_msgget(key_t key, int msgflg) {
        return (int) syscall(__NR_msgget, key, msgflg);
}
static inline ssize_t _systemd_msgrcv(int msqid, void *msgp, size_t msgsz, long msgtyp, int msgflg) {
        return (ssize_t) syscall(__NR_msgrcv, msqid, msgp, msgsz, msgtyp, msgflg);
}
static inline int _systemd_msgsnd(int msqid, const void *msgp, size_t msgsz, int msgflg) {
        return (int) syscall(__NR_msgsnd, msqid, msgp, msgsz, msgflg);
}
#  define msgctl _systemd_msgctl
#  define msgget _systemd_msgget
#  define msgrcv _systemd_msgrcv
#  define msgsnd _systemd_msgsnd
#endif
