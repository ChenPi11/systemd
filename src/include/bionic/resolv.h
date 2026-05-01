/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include_next <resolv.h>

/* MAXNS (maximum number of name servers) is defined by glibc's <resolv.h> but not by
 * Android bionic's version. Define it to the standard value of 3. */
#ifndef MAXNS
#  define MAXNS 3
#endif

/* MAXDNSRCH (maximum number of search domains) and MAXDNSLENGTH (maximum DNS name length)
 * are also defined by glibc but not by Android bionic. */
#ifndef MAXDNSRCH
#  define MAXDNSRCH 6
#endif
#ifndef MAXDNSLENGTH
#  define MAXDNSLENGTH 256
#endif
