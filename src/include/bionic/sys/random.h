/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include_next <sys/random.h>

/* getrandom() was added to Android bionic in API level 28. On older API levels the libc
 * function is not declared in headers; provide a static inline wrapper using the raw syscall
 * instead. The syscall has been available in the kernel since Linux 3.17. */
#if defined(__ANDROID_API__) && __ANDROID_API__ < 28
#  include <sys/syscall.h>
#  include <unistd.h>
static inline ssize_t getrandom(void *buf, size_t buflen, unsigned int flags) {
        return (ssize_t) syscall(__NR_getrandom, buf, buflen, flags);
}
#endif
