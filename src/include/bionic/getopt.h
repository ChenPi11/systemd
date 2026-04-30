/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

/* getopt() is provided both in getopt.h and unistd.h. Hence, we need to tentatively undefine it. */
#undef getopt

#include_next <getopt.h>

/* bionic's getopt() always behaves in POSIXLY_CORRECT mode, and stops parsing arguments when a non-option
 * string is found. Let's always use getopt_long(). */
int getopt_fix(int argc, char * const *argv, const char *optstring);
#define getopt(argc, argv, optstring) getopt_fix(argc, argv, optstring)

int getopt_long_fix(
                int argc,
                char * const *argv,
                const char *optstring,
                const struct option *longopts,
                int *longindex);

#define getopt_long(argc, argv, optstring, longopts, longindex)         \
        getopt_long_fix(argc, argv, optstring, longopts, longindex)
