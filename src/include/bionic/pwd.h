/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include_next <pwd.h>

/* fgetpwent() and putpwent() are GNU extensions not available in Android bionic. Provide
 * stubs that return errors, since Android does not use traditional /etc/passwd-style files. */
#include <errno.h>
#include <stdio.h>

static inline struct passwd *fgetpwent(FILE *stream) {
        errno = EOPNOTSUPP;
        return NULL;
}

static inline int putpwent(const struct passwd *pw, FILE *stream) {
        errno = EOPNOTSUPP;
        return -1;
}

/* setpwent(), getpwent(), and endpwent() were added to Android bionic headers in API level 26.
 * Provide no-op stubs for older API levels; the underlying /etc/passwd database is always empty
 * on Android anyway. */
#if !defined(__ANDROID_API__) || __ANDROID_API__ < 26
static inline void setpwent(void) {}
static inline void endpwent(void) {}
static inline struct passwd *getpwent(void) {
        return NULL;
}
#endif
