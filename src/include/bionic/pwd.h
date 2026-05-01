/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include_next <pwd.h>

/* fgetpwent() and putpwent() are GNU extensions not available in stock Android bionic.
 * Provide stubs that return errors, since Android does not use traditional /etc/passwd-style
 * files.  Some bionic variants (e.g. Termux) already provide these; skip the stubs when
 * meson detected them via HAVE_FGETPWENT / HAVE_PUTPWENT. */
#include <errno.h>
#include <stdio.h>

#ifndef HAVE_FGETPWENT
static inline struct passwd *fgetpwent(FILE *stream) {
        errno = EOPNOTSUPP;
        return NULL;
}
#endif

#ifndef HAVE_PUTPWENT
static inline int putpwent(const struct passwd *pw, FILE *stream) {
        errno = EOPNOTSUPP;
        return -1;
}
#endif

/* setpwent(), getpwent(), and endpwent() were added to stock Android bionic headers in API
 * level 26.  Some bionic variants (e.g. Termux) already provide them at any API level.
 * Only provide no-op stubs when meson confirmed they are absent (HAVE_SETPWENT etc.
 * unset); the underlying /etc/passwd database is always empty on Android anyway. */
#ifndef HAVE_SETPWENT
static inline void setpwent(void) {}
#endif
#ifndef HAVE_ENDPWENT
static inline void endpwent(void) {}
#endif
#ifndef HAVE_GETPWENT
static inline struct passwd *getpwent(void) {
        return NULL;
}
#endif
