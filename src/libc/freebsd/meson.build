# SPDX-License-Identifier: LGPL-2.1-or-later

if get_option('libc') != 'freebsd'
        subdir_done()
endif

libc_wrapper_sources += files(
        'stdlib.c',
        'string.c',
)
