/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include_next <signal.h>

/* sigisemptyset() is a GNU extension not available in Android bionic's signal.h.
 * Implement it by comparing the set to an empty set built with sigemptyset(). */
#include <string.h>
static inline int sigisemptyset(const sigset_t *set) {
        sigset_t empty;
        sigemptyset(&empty);
        return memcmp(set, &empty, sizeof(sigset_t)) == 0;
}
