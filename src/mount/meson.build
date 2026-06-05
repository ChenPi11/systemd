# SPDX-License-Identifier: LGPL-2.1-or-later

if conf.get('HAVE_LIBMOUNT') != 1
        subdir_done()
endif

executables += [
        executable_template + {
                'name' : 'systemd-mount',
                'public' : true,
                'conditions' : ['ENABLE_SYSTEMD_MOUNT'],
                'sources' : files('mount-tool.c'),
                'dependencies' : libmount_cflags,
        },
]

if conf.get('ENABLE_SYSTEMD_MOUNT') == 1
        install_symlink('systemd-umount',
                        pointing_to : 'systemd-mount',
                        install_dir : bindir)
endif
