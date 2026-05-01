/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <stdlib.h>
#include <string.h>

/* The header stdlib.h overrides strtoll with strtoll_fallback, hence we need to undef it here. */
#undef strtoll
/* The header stdlib.h declares program_invocation_name as extern; define and initialize it here. */
#undef program_invocation_name
#undef program_invocation_short_name
#undef secure_getenv

long long strtoll_fallback(const char *nptr, char **endptr, int base) {
        /* glibc returns 0 if the first character is '.' without error, but bionic may return an error.
         * As our code assumes the glibc behavior, let's accept strings start with '.'. */
        if (nptr && *nptr == '.') {
                if (endptr)
                        *endptr = (char*) nptr;
                return 0;
        }

        /* Otherwise, use the native strtoll(). */
        return strtoll(nptr, endptr, base);
}

/* program_invocation_name and program_invocation_short_name are GNU extensions provided by
 * glibc's CRT. On Android bionic, define them here and initialize them at startup via a
 * constructor so they point into the program name string, as glibc would. */
char *program_invocation_name = NULL;
char *program_invocation_short_name = NULL;

static __attribute__((constructor)) void bionic_init_program_invocation_name(void) {
        const char *name = getprogname();
        if (!name)
                return;

        program_invocation_name = (char *) name;

        const char *p = strrchr(name, '/');
        program_invocation_short_name = p ? (char *)(p + 1) : (char *) name;
}
