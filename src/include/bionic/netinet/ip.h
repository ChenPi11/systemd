/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include_next <netinet/ip.h>

/* IPTOS_LOWCOST is an alias for IPTOS_MINCOST (both equal 0x02). glibc defines both names
 * in <netinet/ip.h>, but Android bionic only defines IPTOS_MINCOST. */
#ifndef IPTOS_LOWCOST
#  define IPTOS_LOWCOST IPTOS_MINCOST
#endif

/* IPTOS_CLASS_CSx (DiffServ Class Selector codepoints) are defined on glibc but not in
 * Android bionic's netinet/ip.h. Provide them as they map directly to IPTOS_PREC values. */
#ifndef IPTOS_CLASS_MASK
#  define IPTOS_CLASS_MASK  0xe0
#  define IPTOS_CLASS(c)    ((c) & IPTOS_CLASS_MASK)
#  define IPTOS_CLASS_CS0   0x00
#  define IPTOS_CLASS_CS1   0x20
#  define IPTOS_CLASS_CS2   0x40
#  define IPTOS_CLASS_CS3   0x60
#  define IPTOS_CLASS_CS4   0x80
#  define IPTOS_CLASS_CS5   0xa0
#  define IPTOS_CLASS_CS6   0xc0
#  define IPTOS_CLASS_CS7   0xe0
#endif

/* IPTOS_PREC_* are defined in glibc's <netinet/ip.h> (and <linux/ip.h>) but are absent from
 * Android bionic's <netinet/ip.h>. They are aliases for IPTOS_CLASS_*, defined the same way
 * as glibc does it. */
#ifndef IPTOS_PREC_MASK
#  define IPTOS_PREC_MASK            IPTOS_CLASS_MASK
#  define IPTOS_PREC(tos)            IPTOS_CLASS(tos)
#  define IPTOS_PREC_NETCONTROL      IPTOS_CLASS_CS7
#  define IPTOS_PREC_INTERNETCONTROL IPTOS_CLASS_CS6
#  define IPTOS_PREC_CRITIC_ECP      IPTOS_CLASS_CS5
#  define IPTOS_PREC_FLASHOVERRIDE   IPTOS_CLASS_CS4
#  define IPTOS_PREC_FLASH           IPTOS_CLASS_CS3
#  define IPTOS_PREC_IMMEDIATE       IPTOS_CLASS_CS2
#  define IPTOS_PREC_PRIORITY        IPTOS_CLASS_CS1
#  define IPTOS_PREC_ROUTINE         IPTOS_CLASS_CS0
#endif

/* IPTOS_DSCP_* (DiffServ Codepoint) values are defined on glibc but not in Android bionic. */
#ifndef IPTOS_DSCP_MASK
#  define IPTOS_DSCP_MASK   0xfc
#  define IPTOS_DSCP(x)     ((x) & IPTOS_DSCP_MASK)
#  define IPTOS_DSCP_AF11   0x28
#  define IPTOS_DSCP_AF12   0x30
#  define IPTOS_DSCP_AF13   0x38
#  define IPTOS_DSCP_AF21   0x48
#  define IPTOS_DSCP_AF22   0x50
#  define IPTOS_DSCP_AF23   0x58
#  define IPTOS_DSCP_AF31   0x68
#  define IPTOS_DSCP_AF32   0x70
#  define IPTOS_DSCP_AF33   0x78
#  define IPTOS_DSCP_AF41   0x88
#  define IPTOS_DSCP_AF42   0x90
#  define IPTOS_DSCP_AF43   0x98
#  define IPTOS_DSCP_EF     0xb8
#  define IPTOS_DSCP_VA     0xb0
#  define IPTOS_DSCP_LE     0x04
#endif
