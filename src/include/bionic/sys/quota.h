/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include_next <sys/quota.h>

/* quotactl() is not declared in Android bionic's <sys/quota.h> at any API level.
 * The underlying kernel syscall has always been available, so provide a wrapper. */
#include <sys/syscall.h>
#include <unistd.h>
static inline int quotactl(int cmd, const char *special, int id, void *addr) {
        return (int) syscall(__NR_quotactl, cmd, special, id, addr);
}
