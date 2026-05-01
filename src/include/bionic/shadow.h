/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

/* Android/bionic does not have shadow password support in the stock NDK.  Some bionic
 * variants (e.g. Termux) do provide their own shadow.h with stub implementations.
 * When meson detected a native shadow.h (HAVE_SHADOW_H), delegate to it via #include_next
 * so that its struct spwd definition and any functions it declares are used directly.
 * Otherwise, provide our own struct spwd definition and full function stubs. */

#ifdef HAVE_SHADOW_H
#  include_next <shadow.h>
#else

#include <stddef.h>

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

#endif /* HAVE_SHADOW_H */

/* Provide stubs for any shadow functions that were not found in the native headers.
 * Each stub is guarded by HAVE_<FUNCNAME> set by meson's cc.has_function() probe. */
#include <errno.h>
#include <stdio.h>

#ifndef HAVE_GETSPNAM
static inline struct spwd *getspnam(const char *name) {
        errno = EOPNOTSUPP;
        return NULL;
}
#endif

#ifndef HAVE_GETSPNAM_R
static inline int getspnam_r(
                const char *name,
                struct spwd *spbuf,
                char *buf,
                size_t buflen,
                struct spwd **spbufp) {
        return EOPNOTSUPP;
}
#endif

#ifndef HAVE_FGETSPENT
static inline struct spwd *fgetspent(FILE *stream) {
        errno = EOPNOTSUPP;
        return NULL;
}
#endif

#ifndef HAVE_PUTSPENT
static inline int putspent(const struct spwd *sp, FILE *stream) {
        errno = EOPNOTSUPP;
        return -1;
}
#endif
