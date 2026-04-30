/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include_next <sys/resource.h>

/* Android's linux/resource.h defines RLIM_NLIMITS (the kernel name) but not RLIMIT_NLIMITS
 * (the glibc/userspace name). Provide the alias so code using RLIMIT_NLIMITS compiles. */
#ifndef RLIMIT_NLIMITS
#  ifdef RLIM_NLIMITS
#    define RLIMIT_NLIMITS RLIM_NLIMITS
#  else
#    define RLIMIT_NLIMITS 16
#  endif
#endif
