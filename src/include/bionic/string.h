/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include_next <string.h>

/* Pull in <endian.h> to make htole16/htole32/htole64/le16toh/le32toh/le64toh available.
 * On glibc these byte-order macros are included transitively through <string.h> on many
 * platforms, but Android bionic does not do that. Include it explicitly here so that any
 * file including <string.h> (or our override) gets the byte-order macros automatically. */
#include <endian.h>

char* strerror_r_gnu(int errnum, char *buf, size_t buflen);
#define strerror_r strerror_r_gnu

/* explicit_bzero() is documented as added to Android bionic in API level 28, but the NDK
 * sysroot headers (including r29) do not expose a declaration for it in <string.h>.
 * Provide an inline implementation unconditionally for all Android API levels so that
 * code using explicit_bzero() compiles regardless of the target API level. On API >= 28
 * this inline will simply shadow the libc symbol (both do the same thing, so this is safe).
 * The asm volatile clobbers both "memory" and the buffer itself via a char-array cast to
 * ensure the compiler cannot eliminate the memset as a dead store. */
static inline void explicit_bzero(void *p, size_t n) {
        memset(p, 0, n);
        __asm__ __volatile__("" : : "r"(p), "m"(*(char (*)[n]) p) : "memory");
}
