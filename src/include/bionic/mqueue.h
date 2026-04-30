/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

/* Android bionic does not expose a userspace <mqueue.h> even though the kernel supports POSIX
 * message queues. Provide the necessary type definitions and syscall-based wrappers. */

#include <linux/mqueue.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>

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
