/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include_next <math.h>

/* exp10() is a GNU extension (pow(10, x)) not available in Android bionic's math.h.
 * Provide an inline implementation using pow(). */
static inline double exp10(double x) {
        return pow(10.0, x);
}

static inline float exp10f(float x) {
        return powf(10.0f, x);
}
