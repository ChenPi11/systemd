/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

/* Android/bionic does not have GShadow (group shadow) support in the stock NDK.  Some
 * bionic variants may provide their own gshadow.h.  When meson detected a native gshadow.h
 * (HAVE_GSHADOW_H), delegate to it via #include_next.  Otherwise provide our own struct
 * sgrp definition and function stubs. */

#ifdef HAVE_GSHADOW_H
#  include_next <gshadow.h>
#else

struct sgrp {
        char *sg_namp;
        char *sg_passwd;
        char **sg_adm;
        char **sg_mem;
};

#endif /* HAVE_GSHADOW_H */

/* Provide stubs for any gshadow functions that were not found in the native headers. */
#include <errno.h>
#include <stddef.h>
#include <stdio.h>

#ifndef HAVE_GETSGNAM_R
static inline int getsgnam_r(
                const char *__name,
                struct sgrp *__result_buf,
                char *__buffer,
                size_t __buflen,
                struct sgrp **__result) {

        return EOPNOTSUPP; /* this function returns positive errno in case of error. */
}
#endif

#ifndef HAVE_FGETSGENT
/* fgetsgent() is a GNU extension not available in Android bionic. */
static inline struct sgrp *fgetsgent(FILE *stream) {
        errno = EOPNOTSUPP;
        return NULL;
}
#endif
