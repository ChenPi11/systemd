/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include_next <sys/auxv.h>

/* On glibc, <sys/auxv.h> includes <bits/hwcap.h> which defines HWCAP_*
 * constants. Bionic does not have <bits/hwcap.h>; the base HWCAP constants
 * come from kernel <asm/hwcap.h> via <bits/auxvec.h>. Include the override
 * <bits/hwcap.h> explicitly so that newer HWCAP_* / HWCAP2_* / HWCAP3_*
 * constants (GCS, CMPBR, MTE_FAR, etc.) which are not yet in bionic's
 * kernel headers are made available. */
#include <bits/hwcap.h>
