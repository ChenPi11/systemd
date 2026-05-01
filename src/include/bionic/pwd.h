/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include_next <pwd.h>

/* fgetpwent() and putpwent() are GNU extensions not available in stock Android bionic.
 * Termux provides them in libc but does not declare them in its pwd.h.  Always provide at
 * least a declaration so callers can use them.  When meson confirmed the function is absent
 * (HAVE_xxx unset), provide a full inline stub that returns an error. */
#include <errno.h>
#include <stdio.h>

#if HAVE_FGETPWENT
struct passwd *fgetpwent(FILE *stream);
#else
static inline struct passwd *fgetpwent(FILE *stream) {
        errno = EOPNOTSUPP;
        return NULL;
}
#endif

#if HAVE_PUTPWENT
int putpwent(const struct passwd *pw, FILE *stream);
#else
static inline int putpwent(const struct passwd *pw, FILE *stream) {
        errno = EOPNOTSUPP;
        return -1;
}
#endif

/* setpwent(), getpwent(), and endpwent() were added to stock Android bionic headers in API
 * level 26.  Some bionic variants (e.g. Termux) already provide them at any API level but
 * may not declare them in pwd.h.  Always provide at least a declaration; provide a no-op
 * stub only when meson confirmed the function is absent. */
#if HAVE_SETPWENT
void setpwent(void);
#else
static inline void setpwent(void) {}
#endif
#if HAVE_ENDPWENT
void endpwent(void);
#else
static inline void endpwent(void) {}
#endif
#if HAVE_GETPWENT
struct passwd *getpwent(void);
#else
static inline struct passwd *getpwent(void) {
        return NULL;
}
#endif
