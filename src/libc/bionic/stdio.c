/* SPDX-License-Identifier: LGPL-2.1-or-later */

/* fputc_unlocked(), fputs_unlocked(), and fwrite_unlocked() are declared in Android's <stdio.h>
 * only when __USE_BSD / __USE_GNU are defined. Define them before any include. */
#define __USE_BSD
#define __USE_GNU

#include <errno.h>
#include <stdio.h>
#include <stdio_ext.h>
#include <sys/syscall.h>
#include <unistd.h>

#if !HAVE_RENAMEAT2
int missing_renameat2(int __oldfd, const char *__old, int __newfd, const char *__new, unsigned __flags) {
        return syscall(__NR_renameat2, __oldfd, __old, __newfd, __new, __flags);
}
#endif

#define DEFINE_PUT(func)                                         \
        int func##_check_writable(int c, FILE *stream) {         \
                if (!__fwritable(stream)) {                      \
                        errno = EBADF;                           \
                        return EOF;                              \
                }                                                \
                                                                 \
                return func(c, stream);                          \
        }

#define DEFINE_FPUTS(func)                                       \
        int func##_check_writable(const char *s, FILE *stream) { \
                if (!__fwritable(stream)) {                      \
                        errno = EBADF;                           \
                        return EOF;                              \
                }                                                \
                                                                 \
                return func(s, stream);                          \
        }

#undef putc
#undef putc_unlocked
#undef fputc
#undef fputc_unlocked
#undef fputs
#undef fputs_unlocked
#undef fwrite_unlocked

DEFINE_PUT(putc);
DEFINE_PUT(putc_unlocked);
DEFINE_PUT(fputc);
DEFINE_FPUTS(fputs);

/* fputc_unlocked(), fputs_unlocked(), and fwrite_unlocked() were added to Android bionic
 * in API level 28. On older API levels they are not declared in <stdio.h>; provide
 * _check_writable wrappers that fall back to the locking equivalents instead. */
#if !defined(__ANDROID_API__) || __ANDROID_API__ >= 28
DEFINE_PUT(fputc_unlocked);
DEFINE_FPUTS(fputs_unlocked);

size_t fwrite_unlocked_check_writable(const void *ptr, size_t size, size_t n, FILE *stream) {
        if (!__fwritable(stream)) {
                errno = EBADF;
                return 0;
        }
        return fwrite_unlocked(ptr, size, n, stream);
}
#else
int fputc_unlocked_check_writable(int c, FILE *stream) {
        if (!__fwritable(stream)) {
                errno = EBADF;
                return EOF;
        }
        return fputc(c, stream);
}
int fputs_unlocked_check_writable(const char *s, FILE *stream) {
        if (!__fwritable(stream)) {
                errno = EBADF;
                return EOF;
        }
        return fputs(s, stream);
}
size_t fwrite_unlocked_check_writable(const void *ptr, size_t size, size_t n, FILE *stream) {
        if (!__fwritable(stream)) {
                errno = EBADF;
                return 0;
        }
        return fwrite(ptr, size, n, stream);
}
#endif
