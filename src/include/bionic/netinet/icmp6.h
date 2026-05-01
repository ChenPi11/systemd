/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include_next <netinet/icmp6.h>

/* struct nd_opt_home_agent_info is defined in glibc's <netinet/icmp6.h> but not in Android
 * bionic's version. It is part of the Mobile IPv6 Home Agent Discovery option (RFC 3775). */
#ifndef ND_OPT_HOME_AGENT_INFO
#  define ND_OPT_HOME_AGENT_INFO 24

struct nd_opt_home_agent_info {
        uint8_t  nd_opt_home_agent_info_type;
        uint8_t  nd_opt_home_agent_info_len;
        uint16_t nd_opt_home_agent_info_reserved;
        uint16_t nd_opt_home_agent_info_preference;
        uint16_t nd_opt_home_agent_info_lifetime;
};
#endif
