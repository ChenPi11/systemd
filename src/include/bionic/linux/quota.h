/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

/* Android bionic's <sys/quota.h> uses a '#define if_dqblk dqblk' trick before including
 * <linux/quota.h> so that "struct dqblk" becomes available. However, systemd's quota-util.h
 * includes <linux/quota.h> directly (before <sys/quota.h>) so the macro trick never applies
 * and "struct dqblk" is never defined. Apply the alias defines here so that any code that
 * includes <linux/quota.h> also gets "struct dqblk" and "struct dqinfo". */
#define if_dqblk dqblk
#define if_dqinfo dqinfo
#include_next <linux/quota.h>
#undef if_dqblk
#undef if_dqinfo
