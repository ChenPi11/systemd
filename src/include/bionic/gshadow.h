/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

/* Android/bionic does not have GShadow (group shadow) support in the stock NDK, and no
 * known bionic variant ships a standalone gshadow.h.  Always define struct sgrp here and
 * provide function stubs for any functions that meson did not find in the target
 * environment (guarded by HAVE_<FUNC> from config.h). */

#include <errno.h>
#include <stddef.h>
#include <stdio.h>

struct sgrp {
        char *sg_namp;
        char *sg_passwd;
        char **sg_adm;
        char **sg_mem;
};

#ifdef HAVE_GETSGNAM_R
int getsgnam_r(const char *__name, struct sgrp *__result_buf, char *__buffer, size_t __buflen, struct sgrp **__result);
#else
static inline int getsgnam_r(
                const char *__name,
                struct sgrp *__result_buf,
                char *__buffer,
                size_t __buflen,
                struct sgrp **__result) {

        return EOPNOTSUPP; /* this function returns positive errno in case of error. */
}
#endif

#ifdef HAVE_FGETSGENT
/* fgetsgent() is a GNU extension not available in Android bionic. */
struct sgrp *fgetsgent(FILE *stream);
#else
static inline struct sgrp *fgetsgent(FILE *stream) {
        errno = EOPNOTSUPP;
        return NULL;
}
#endif
