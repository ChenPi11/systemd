/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include_next <dirent.h>

/* IFTODT and DTTOIF are GNU extensions not provided by Android bionic's dirent.h.
 * IFTODT converts file type bits from stat(2) st_mode to a DT_xxx dirent d_type value.
 * DTTOIF is the reverse conversion. */
#ifndef IFTODT
#  define IFTODT(mode)  (((mode) & 0170000) >> 12)
#endif
#ifndef DTTOIF
#  define DTTOIF(type)  ((type) << 12)
#endif

/* getdents64() is not declared in Android bionic's dirent.h (it is only available as a raw
 * syscall). Provide a wrapper so code can call it by name. */
#include <sys/syscall.h>
#include <unistd.h>
static inline long getdents64(int fd, void *dirp, unsigned int count) {
        return syscall(__NR_getdents64, fd, dirp, count);
}
