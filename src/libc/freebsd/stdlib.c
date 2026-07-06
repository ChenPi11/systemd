/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <stdlib.h>
#include <string.h>

#undef program_invocation_name
#undef program_invocation_short_name

char *program_invocation_name = NULL;
char *program_invocation_short_name = NULL;

static __attribute__((constructor)) void freebsd_init_program_invocation_name(void) {
        const char *name = getprogname();
        const char *p;

        if (!name)
                return;

        program_invocation_name = (char*) name;

        p = strrchr(name, '/');
        program_invocation_short_name = p ? (char*) (p + 1) : (char*) name;
}
