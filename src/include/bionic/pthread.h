/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include_next <pthread.h>

/* pthread_setaffinity_np() was added to Android bionic headers in API level 36. Provide
 * an implementation for older API levels using sched_setaffinity() with the thread's TID,
 * obtained via pthread_gettid_np() which has been available since API 1. */
#if !defined(__ANDROID_API__) || __ANDROID_API__ < 36
#  include <sched.h>
static inline int pthread_setaffinity_np(pthread_t thread, size_t cpusetsize, const cpu_set_t *cpuset) {
        return sched_setaffinity(pthread_gettid_np(thread), cpusetsize, cpuset);
}
static inline int pthread_getaffinity_np(pthread_t thread, size_t cpusetsize, cpu_set_t *cpuset) {
        return sched_getaffinity(pthread_gettid_np(thread), cpusetsize, cpuset);
}
#endif

