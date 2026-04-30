/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include_next <malloc.h>

#include <errno.h>
#include <stdio.h>

/* mallinfo2() was added to Android bionic in API level 28. On older API levels or non-Android
 * platforms where the configure check did not find it, provide a stub returning zeroes. */
#if !defined(HAVE_MALLINFO2) && (!defined(__ANDROID_API__) || __ANDROID_API__ < 28)
struct mallinfo2 {
        size_t arena;    /* non-mmapped space allocated from system */
        size_t ordblks;  /* number of free chunks */
        size_t smblks;   /* number of fastbin blocks */
        size_t hblks;    /* number of mmapped regions */
        size_t hblkhd;   /* space in mmapped regions */
        size_t usmblks;  /* always 0, preserved for backwards compatibility */
        size_t fsmblks;  /* space available in freed fastbin blocks */
        size_t uordblks; /* total allocated space */
        size_t fordblks; /* total free space */
        size_t keepcost; /* top-most, releasable (via malloc_trim) space */
};

static inline struct mallinfo2 mallinfo2(void) {
        return (struct mallinfo2) {};
}
#endif

/* malloc_info() was added to Android bionic in API level 23. On older API levels or non-Android
 * platforms where the configure check did not find it, provide a stub returning -EOPNOTSUPP. */
#if !defined(HAVE_MALLOC_INFO) && (!defined(__ANDROID_API__) || __ANDROID_API__ < 23)
static inline int malloc_info(int options, FILE *stream) {
        if (options != 0)
                errno = EINVAL;
        else
                errno = EOPNOTSUPP;
        return -1;
}
#endif

/* malloc_trim() has been in Android bionic since API level 1, so no stub is needed on Android.
 * On other platforms (e.g. musl), provide a no-op stub if the configure check did not find it. */
#if !defined(HAVE_MALLOC_TRIM) && !defined(__ANDROID_API__)
static inline int malloc_trim(size_t pad) {
        return 0;
}
#endif
