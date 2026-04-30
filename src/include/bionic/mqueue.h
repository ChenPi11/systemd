/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

/* Android bionic does not expose a userspace <mqueue.h> even though the kernel supports POSIX
 * message queues. Provide the necessary type definitions and syscall-based wrappers.
 * Note: we do NOT include <linux/mqueue.h> here because that header defines NOTIFY_NONE,
 * NOTIFY_WOKENUP, and NOTIFY_REMOVED macros that conflict with enum values in systemd's
 * unit-def.h. Define struct mq_attr directly instead. */

#include <sys/syscall.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <stddef.h>

struct mq_attr {
        long mq_flags;   /* Message queue flags */
        long mq_maxmsg;  /* Maximum number of messages */
        long mq_msgsize; /* Maximum message size */
        long mq_curmsgs; /* Number of messages currently queued */
        long __reserved[4];
};

typedef int mqd_t;

static inline int mq_getattr(mqd_t mqdes, struct mq_attr *attr) {
        return (int) syscall(__NR_mq_getsetattr, mqdes, NULL, attr);
}

static inline int mq_setattr(mqd_t mqdes, const struct mq_attr *newattr, struct mq_attr *oldattr) {
        return (int) syscall(__NR_mq_getsetattr, mqdes, newattr, oldattr);
}

static inline ssize_t mq_receive(mqd_t mqdes, char *msg_ptr, size_t msg_len, unsigned *msg_prio) {
        return (ssize_t) syscall(__NR_mq_timedreceive, mqdes, msg_ptr, msg_len, msg_prio, NULL);
}

static inline int mq_unlink(const char *name) {
        return (int) syscall(__NR_mq_unlink, name);
}

static inline mqd_t mq_open(const char *name, int oflag, ...) {
        /* mq_open() is variadic (mode and attr are optional). As a syscall wrapper we use a fixed
         * signature; callers with mode/attr arguments will still work because the extra args are
         * placed in the registers that the syscall reads. */
        return (mqd_t) syscall(__NR_mq_open, name, oflag, 0666, NULL);
}
