# SPDX-License-Identifier: LGPL-2.1-or-later

busctl_sources = files(
        'busctl.c',
)
busctl_extract_sources = files(
        'busctl-introspect.c',
)

executables += [
        executable_template + {
                'name' : 'busctl',
                'public' : true,
                'conditions' : ['ENABLE_BUSCTL'],
                'sources' : busctl_sources,
                'extract' : busctl_extract_sources,
        },
        test_template + {
                'conditions' : ['ENABLE_BUSCTL'],
                'sources' : files('test-busctl-introspect.c'),
                'objects' : ['busctl'],
        },
]
