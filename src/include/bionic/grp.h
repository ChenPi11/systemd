/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include_next <grp.h>

/* fgetgrent() and putgrent() are GNU extensions not available in stock Android bionic.
 * Provide stubs that return errors, since Android does not use traditional /etc/group-style
 * files.  Some bionic variants (e.g. Termux) already provide these; skip the stubs when
 * meson detected them via HAVE_FGETGRENT / HAVE_PUTGRENT. */
#include <errno.h>
#include <stdio.h>

#ifndef HAVE_FGETGRENT
static inline struct group *fgetgrent(FILE *stream) {
        errno = EOPNOTSUPP;
        return NULL;
}
#endif

#ifndef HAVE_PUTGRENT
static inline int putgrent(const struct group *gr, FILE *stream) {
        errno = EOPNOTSUPP;
        return -1;
}
#endif

/* setgrent(), getgrent(), and endgrent() were added to stock Android bionic headers in API
 * level 26.  Some bionic variants (e.g. Termux) already provide them at any API level.
 * Only provide no-op stubs when meson confirmed they are absent (HAVE_SETGRENT etc.
 * unset); the underlying /etc/group database is always empty on Android anyway. */
#ifndef HAVE_SETGRENT
static inline void setgrent(void) {}
#endif
#ifndef HAVE_ENDGRENT
static inline void endgrent(void) {}
#endif
#ifndef HAVE_GETGRENT
static inline struct group *getgrent(void) {
        return NULL;
}
#endif
