/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include_next <sys/socket.h>

/* AF_MPLS (Multi-Protocol Label Switching) is not defined in Android bionic's
 * socket headers, but the kernel supports it. Define it here with the standard Linux value. */
#ifndef AF_MPLS
#  define AF_MPLS 28
#endif
