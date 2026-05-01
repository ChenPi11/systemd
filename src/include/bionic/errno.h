/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include_next <errno.h>

/* program_invocation_name and program_invocation_short_name are GNU extensions. On glibc they
 * are declared in <errno.h> (via bits/types/error_t.h). On Android bionic they are not provided
 * at all. Declare them here so any code that includes <errno.h> (directly or transitively via
 * basic-forward.h) has access to them. They are defined in src/libc/bionic/stdlib.c. */
extern char *program_invocation_name;
extern char *program_invocation_short_name;
