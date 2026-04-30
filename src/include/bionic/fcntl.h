/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include_next <fcntl.h>

/* struct file_handle and open_by_handle_at() are GNU extensions provided by glibc's fcntl.h
 * via bits/fcntl-linux.h. Android bionic does not expose them in its NDK headers, even though
 * the underlying syscalls exist. Provide the definitions here. */

#ifndef __file_handle_defined
#define __file_handle_defined 1

#include <sys/syscall.h>
#include <unistd.h>

struct file_handle {
        unsigned int handle_bytes;
        int handle_type;
        unsigned char f_handle[0];
};

static inline int name_to_handle_at(
                int dfd,
                const char *name,
                struct file_handle *handle,
                int *mnt_id,
                int flags) {
        return (int) syscall(__NR_name_to_handle_at, dfd, name, handle, mnt_id, flags);
}

static inline int open_by_handle_at(int mountdirfd, struct file_handle *handle, int flags) {
        return (int) syscall(__NR_open_by_handle_at, mountdirfd, handle, flags);
}

#endif /* __file_handle_defined */
