/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <sys/syscall.h>
#include <unistd.h>

int missing_close_range(unsigned first_fd, unsigned end_fd, unsigned flags) {
        return syscall(__NR_close_range, first_fd, end_fd, flags);
}

int missing_execveat(int dirfd, const char *pathname, char * const argv[], char * const envp[], int flags) {
        return syscall(__NR_execveat, dirfd, pathname, argv, envp, flags);
}
