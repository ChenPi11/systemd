/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include_next <grp.h>

/* fgetgrent() and putgrent() are GNU extensions not available in stock Android bionic.
 * Termux provides them in libc but does not declare them in its grp.h.  Always provide at
 * least a declaration so callers can use them.  When meson confirmed the function is absent
 * (HAVE_xxx unset), provide a full inline stub that returns an error. */
#include <errno.h>
#include <stdio.h>

#ifdef HAVE_FGETGRENT
struct group *fgetgrent(FILE *stream);
#else
static inline struct group *fgetgrent(FILE *stream) {
        errno = EOPNOTSUPP;
        return NULL;
}
#endif

#ifdef HAVE_PUTGRENT
int putgrent(const struct group *gr, FILE *stream);
#else
static inline int putgrent(const struct group *gr, FILE *stream) {
        errno = EOPNOTSUPP;
        return -1;
}
#endif

/* setgrent(), getgrent(), and endgrent() were added to stock Android bionic headers in API
 * level 26.  Some bionic variants (e.g. Termux) already provide them at any API level but
 * may not declare them in grp.h.  Always provide at least a declaration; provide a no-op
 * stub only when meson confirmed the function is absent. */
#ifdef HAVE_SETGRENT
void setgrent(void);
#else
static inline void setgrent(void) {}
#endif
#ifdef HAVE_ENDGRENT
void endgrent(void);
#else
static inline void endgrent(void) {}
#endif
#ifdef HAVE_GETGRENT
struct group *getgrent(void);
#else
static inline struct group *getgrent(void) {
        return NULL;
}
#endif
