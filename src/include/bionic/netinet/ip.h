/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include_next <netinet/ip.h>

/* IPTOS_LOWCOST is an alias for IPTOS_MINCOST (both equal 0x02). glibc defines both names
 * in <netinet/ip.h>, but Android bionic only defines IPTOS_MINCOST. */
#ifndef IPTOS_LOWCOST
#  define IPTOS_LOWCOST IPTOS_MINCOST
#endif
