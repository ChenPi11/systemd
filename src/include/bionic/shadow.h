/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

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

static inline struct spwd *getspnam(const char *name) {
        errno = EOPNOTSUPP;
        return NULL;
}

static inline int getspnam_r(
                const char *name,
                struct spwd *spbuf,
                char *buf,
                size_t buflen,
                struct spwd **spbufp) {
        return EOPNOTSUPP;
}

static inline struct spwd *fgetspent(FILE *stream) {
        errno = EOPNOTSUPP;
        return NULL;
}

static inline int putspent(const struct spwd *sp, FILE *stream) {
        errno = EOPNOTSUPP;
        return -1;
}
