/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

/* Android's <linux/tcp.h> references struct sockaddr_storage but does not include the header
 * that defines it (<bits/sockaddr_storage.h>). Include it first so that the struct is defined
 * when linux/tcp.h is parsed. */
#include <bits/sockaddr_storage.h>
#include_next <linux/tcp.h>
