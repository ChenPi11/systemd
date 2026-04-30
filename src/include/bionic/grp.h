/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include_next <grp.h>

/* fgetgrent() and putgrent() are GNU extensions not available in Android bionic. Provide
 * stubs that return errors, since Android does not use traditional /etc/group-style files. */
#include <errno.h>
#include <stdio.h>

static inline struct group *fgetgrent(FILE *stream) {
        errno = EOPNOTSUPP;
        return NULL;
}

static inline int putgrent(const struct group *gr, FILE *stream) {
        errno = EOPNOTSUPP;
        return -1;
}
