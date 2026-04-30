/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include_next <string.h>

char* strerror_r_gnu(int errnum, char *buf, size_t buflen);
#define strerror_r strerror_r_gnu

/* explicit_bzero() is only declared in bionic's string.h from Android API level 28 onwards
 * (__INTRODUCED_IN(28)). Provide a fallback inline implementation using a compiler barrier to
 * prevent the compiler from optimizing away the zeroing operation. */
#if !defined(__ANDROID_API__) || __ANDROID_API__ < 28
static inline void explicit_bzero(void *p, size_t n) {
        if (n > 0) {
                memset(p, 0, n);
                __asm__ __volatile__("" : : "r"(p) : "memory");
        }
}
#endif
