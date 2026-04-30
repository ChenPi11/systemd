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

/* setgrent(), getgrent(), and endgrent() were added to Android bionic headers in API level 26.
 * Provide no-op stubs for older API levels; the underlying /etc/group database is always empty
 * on Android anyway. */
#if !defined(__ANDROID_API__) || __ANDROID_API__ < 26
static inline void setgrent(void) {}
static inline void endgrent(void) {}
static inline struct group *getgrent(void) {
        return NULL;
}
#endif
