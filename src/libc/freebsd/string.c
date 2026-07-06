/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <errno.h>
#include <stdio.h>
#include <string.h>

#define ERRNO_MAX 4095
#define ERRNO_BUF_LEN 1024

#undef strerror_r

char* strerror_r_gnu(int errnum, char *buf, size_t buflen) {
        if (errnum == 0)
                return (char*) "Success";

        if (buflen == 0)
                return (char*) "Unknown error";

        if (errnum < 0 || errnum > ERRNO_MAX)
                goto fallback;

        if (strerror_r(errnum, buf, buflen) != 0)
                goto fallback;

        char buf_0[ERRNO_BUF_LEN];
        if (strerror_r(0, buf_0, sizeof buf_0) != 0)
                goto fallback;

        size_t n = (buflen < ERRNO_BUF_LEN ? buflen : ERRNO_BUF_LEN) - 1;
        if (strncmp(buf, buf_0, n) != 0)
                return buf;

fallback:
        snprintf(buf, buflen, "Unknown error %i", errnum);
        return buf;
}
