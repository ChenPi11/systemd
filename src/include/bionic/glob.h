/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include_next <glob.h>

/* bionic's glob_t does not have the GLOB_ALTDIRFUNC extension fields (__dummy2 array).
 * Do not define GLOB_ALTDIRFUNC or the gl_* field macros here; code must guard against their absence
 * using #ifdef GLOB_ALTDIRFUNC. */

#ifndef GLOB_BRACE
#define GLOB_BRACE      (1 << 10)
#endif
