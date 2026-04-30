/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include <stddef.h>

/* bionic does not provide libcrypt; provide stub declarations. */

struct crypt_data {
        char output[512];
        char setting[512];
        char input[512];
        char reserved[16];
        char initialized;
};

static inline char* crypt_r(const char *phrase, const char *setting, struct crypt_data *data) {
        return NULL;
}

const char* missing_crypt_preferred_method(void);
#define crypt_preferred_method missing_crypt_preferred_method

char* missing_crypt_gensalt_ra(const char *prefix, unsigned long count, const char *rbytes, int nrbytes);
#define crypt_gensalt_ra missing_crypt_gensalt_ra

char* missing_crypt_ra(const char *phrase, const char *setting, void **data, int *size);
#define crypt_ra missing_crypt_ra
