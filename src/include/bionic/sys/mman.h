/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include_next <sys/mman.h>

/* memfd_create() was added to Android bionic headers in API level 30. Provide a syscall-based
 * inline for older API levels; the underlying kernel syscall has been available since kernel 3.17
 * on all architectures supported by Android. */
#if !defined(__ANDROID_API__) || __ANDROID_API__ < 30
#  include <sys/syscall.h>
#  include <unistd.h>
static inline int memfd_create(const char *name, unsigned flags) {
        return (int) syscall(__NR_memfd_create, name, flags);
}
#endif
