/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

/* Android/bionic does not have shadow password support in the stock NDK.  Some bionic
 * variants (e.g. Termux) provide stub implementations of shadow functions, but do not
 * ship a standalone shadow.h header.  Always define struct spwd here so callers get a
 * consistent definition, and provide function stubs for any functions that meson did not
 * find in the target environment (guarded by HAVE_<FUNC> from config.h). */

#include <errno.h>
#include <stddef.h>
#include <stdio.h>

struct spwd {
        char   *sp_namp;   /* Login name */
        char   *sp_pwdp;   /* Encrypted password */
        long    sp_lstchg; /* Date of last change (days since epoch) */
        long    sp_min;    /* Min # of days between changes */
        long    sp_max;    /* Max # of days between changes */
        long    sp_warn;   /* # of days before pwd expires to warn user */
        long    sp_inact;  /* # of days after pwd expiry until acct disabled */
        long    sp_expire; /* Date when account expires */
        unsigned long sp_flag; /* Reserved */
};

/* Termux provides shadow functions in libc but does not ship a shadow.h that declares them.
 * Always provide at least a declaration so callers can use them.  When meson confirmed the
 * function is absent (HAVE_xxx unset), provide a full inline stub that returns an error. */

#if HAVE_GETSPNAM
struct spwd *getspnam(const char *name);
#else
static inline struct spwd *getspnam(const char *name) {
        errno = EOPNOTSUPP;
        return NULL;
}
#endif

#if HAVE_GETSPNAM_R
int getspnam_r(const char *name, struct spwd *spbuf, char *buf, size_t buflen, struct spwd **spbufp);
#else
static inline int getspnam_r(
                const char *name,
                struct spwd *spbuf,
                char *buf,
                size_t buflen,
                struct spwd **spbufp) {
        return EOPNOTSUPP;
}
#endif

#if HAVE_FGETSPENT
struct spwd *fgetspent(FILE *stream);
#else
static inline struct spwd *fgetspent(FILE *stream) {
        errno = EOPNOTSUPP;
        return NULL;
}
#endif

#if HAVE_PUTSPENT
int putspent(const struct spwd *sp, FILE *stream);
#else
static inline int putspent(const struct spwd *sp, FILE *stream) {
        errno = EOPNOTSUPP;
        return -1;
}
#endif
