/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include_next <stdlib.h>

#include <errno.h>
#include <stddef.h>
#include <unistd.h>

static inline char *secure_getenv(const char *name) {
        if (issetugid())
                return NULL;

        return getenv(name);
}

typedef struct QSortRContext {
        int (*compar)(const void*, const void*, void*);
        void *userdata;
} QSortRContext;

static inline int qsort_r_compar_freebsd_adapter(void *arg, const void *a, const void *b) {
        QSortRContext *context = arg;

        return context->compar(a, b, context->userdata);
}

static inline void qsort_r_glibc(void *base, size_t nmemb, size_t size,
                                 int (*compar)(const void *, const void *, void *),
                                 void *arg) {
        QSortRContext context = {
                .compar = compar,
                .userdata = arg,
        };

        qsort_r(base, nmemb, size, &context, qsort_r_compar_freebsd_adapter);
}

#define qsort_r qsort_r_glibc
