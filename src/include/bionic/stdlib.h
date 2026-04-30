/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include_next <stdlib.h>

#include <sys/auxv.h>
#include <stddef.h>
#include <errno.h>

long long strtoll_fallback(const char *nptr, char **endptr, int base);
#define strtoll strtoll_fallback

/* secure_getenv() is a GNU extension not available in Android bionic. Implement it using
 * getauxval(AT_SECURE): if the process has elevated privileges (setuid/setgid), return NULL. */
static inline char *secure_getenv(const char *name) {
        if (getauxval(AT_SECURE))
                return NULL;
        return getenv(name);
}

/* reallocarray() was added to Android bionic in API level 29. Provide a safe implementation
 * for older API levels that checks for multiplication overflow before calling realloc(). */
#if !defined(__ANDROID_API__) || __ANDROID_API__ < 29
#  include <stdint.h>
static inline void *reallocarray(void *ptr, size_t nmemb, size_t size) {
        if (size != 0 && nmemb > SIZE_MAX / size) {
                errno = ENOMEM;
                return NULL;
        }
        return realloc(ptr, nmemb * size);
}
#endif

/* qsort_r() was added to Android bionic headers in API level 36. Provide an implementation
 * for older API levels using a thread-local variable to pass the comparison context. */
#if !defined(__ANDROID_API__) || __ANDROID_API__ < 36
static __thread struct {
        int (*compar)(const void *, const void *, void *);
        void *arg;
} _bionic_qsort_r_ctx;

static inline int _bionic_qsort_r_cmp(const void *a, const void *b) {
        return _bionic_qsort_r_ctx.compar(a, b, _bionic_qsort_r_ctx.arg);
}

static inline void qsort_r(void *base, size_t nmemb, size_t size,
                            int (*compar)(const void *, const void *, void *),
                            void *arg) {
        _bionic_qsort_r_ctx.compar = compar;
        _bionic_qsort_r_ctx.arg = arg;
        qsort(base, nmemb, size, _bionic_qsort_r_cmp);
}
#endif

/* program_invocation_name and program_invocation_short_name are GNU extensions provided by
 * glibc's CRT startup code. On Android, declare them here; they are defined and initialized
 * in src/libc/bionic/stdlib.c via a constructor attribute. */
extern char *program_invocation_name;
extern char *program_invocation_short_name;
