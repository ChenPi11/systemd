/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

/* getopt() is provided both in getopt.h and unistd.h. Hence, we need to tentatively undefine it. */
#undef getopt

#include_next <unistd.h>

/* bionic's getopt() always behaves in POSIXLY_CORRECT mode, and stops parsing arguments when a non-option
 * string is found. Let's always use getopt_long(). */
int getopt_fix(int argc, char * const *argv, const char *optstring);
#define getopt(argc, argv, optstring) getopt_fix(argc, argv, optstring)

int missing_close_range(unsigned first_fd, unsigned end_fd, unsigned flags);
#define close_range missing_close_range

int missing_execveat(int dirfd, const char *pathname, char * const argv[], char * const envp[], int flags);
#define execveat missing_execveat

/* get_current_dir_name() is a GNU extension not available in Android bionic. Implement it as a
 * wrapper around getcwd(NULL, 0), which bionic supports and allocates the buffer dynamically. */
static inline char *get_current_dir_name(void) {
        return getcwd(NULL, 0);
}

/* getdtablesize() is a legacy POSIX function not declared in Android bionic's unistd.h.
 * Implement it using getrlimit(RLIMIT_NOFILE) to return the current open file limit. */
#include <sys/resource.h>
static inline int getdtablesize(void) {
        struct rlimit rl;
        if (getrlimit(RLIMIT_NOFILE, &rl) < 0)
                return 256; /* POSIX minimum */
        return (int) rl.rlim_cur;
}

/* copy_file_range() was added to Android bionic headers in API level 34. Provide a
 * syscall-based wrapper for older API levels; the syscall has been available since kernel 4.5. */
#if !defined(__ANDROID_API__) || __ANDROID_API__ < 34
#  include <sys/syscall.h>
static inline ssize_t copy_file_range(int fd_in, off64_t *off_in, int fd_out, off64_t *off_out,
                                      size_t len, unsigned int flags) {
        return (ssize_t) syscall(__NR_copy_file_range, fd_in, off_in, fd_out, off_out, len, flags);
}
#endif
