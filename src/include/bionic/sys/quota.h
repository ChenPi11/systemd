/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include_next <sys/quota.h>

/* quotactl() was absent from Android bionic's <sys/quota.h> on older NDKs but was added
 * at API level 26 (__INTRODUCED_IN(26)).  Provide a syscall-based wrapper only when meson
 * confirmed the function is absent (HAVE_QUOTACTL=0); on NDK 26+ the declaration from the
 * system header is used and the static inline is suppressed to avoid a conflicting
 * "static declaration follows non-static declaration" error. */
#if !HAVE_QUOTACTL
#  include <sys/syscall.h>
#  include <unistd.h>
static inline int quotactl(int cmd, const char *special, int id, void *addr) {
        return (int) syscall(__NR_quotactl, cmd, special, id, addr);
}
#endif
