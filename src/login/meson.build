# SPDX-License-Identifier: LGPL-2.1-or-later

if conf.get('ENABLE_LOGIND') != 1 and conf.get('ENABLE_SYSTEMD_LOGIND') != 1
        subdir_done()
endif

systemd_logind_sources = files(
        'logind.c',
)
systemd_logind_extract_sources = files(
        'logind-action.c',
        'logind-brightness.c',
        'logind-button.c',
        'logind-core.c',
        'logind-dbus.c',
        'logind-device.c',
        'logind-inhibit.c',
        'logind-polkit.c',
        'logind-seat-dbus.c',
        'logind-seat.c',
        'logind-session-dbus.c',
        'logind-session-device.c',
        'logind-session.c',
        'logind-shutdown.c',
        'logind-user-dbus.c',
        'logind-user.c',
        'logind-utmp.c',
        'logind-varlink.c',
        'logind-wall.c',
)

logind_gperf_c = custom_target(
        input : 'logind-gperf.gperf',
        output : 'logind-gperf.c',
        command : [gperf, '@INPUT@', '--output-file', '@OUTPUT@'])

generated_sources += logind_gperf_c
systemd_logind_extract_sources += [logind_gperf_c]

loginctl_sources = files(
        'loginctl.c',
        'sysfs-show.c',
)

executables += [
        libexec_template + {
                'name' : 'systemd-logind',
                'dbus' : true,
                'conditions' : ['ENABLE_SYSTEMD_LOGIND'],
                'sources' : systemd_logind_sources,
                'extract' : systemd_logind_extract_sources,
                'dependencies' : [
                        libacl_cflags,
                ],
        },
        executable_template + {
                'name' : 'loginctl',
                'public' : true,
                'conditions' : ['ENABLE_LOGIND'],
                'sources' : loginctl_sources,
                'dependencies' : [
                        liblz4_cflags,
                        libxz_cflags,
                        libzstd_cflags,
                ],
        },
        executable_template + {
                'name' : 'systemd-inhibit',
                'public' : true,
                'conditions' : ['ENABLE_LOGIND'],
                'sources' : files('inhibit.c'),
        },
        libexec_template + {
                'name' : 'systemd-user-runtime-dir',
                'conditions' : ['ENABLE_SYSTEMD_LOGIND'],
                'sources' : files('user-runtime-dir.c'),
        },
        test_template + {
                'conditions' : ['ENABLE_SYSTEMD_LOGIND'],
                'sources' : files('test-inhibit.c'),
                'type' : 'manual',
        },
        test_template + {
                'conditions' : ['ENABLE_SYSTEMD_LOGIND'],
                'sources' : files('test-login-tables.c'),
                'objects' : ['systemd-logind'],
        },
        test_template + {
                'conditions' : ['ENABLE_SYSTEMD_LOGIND'],
                'sources' : files('test-session-properties.c'),
                'type' : 'manual',
        },
]

simple_tests += files(
        'test-login-shared.c'
)

modules += [
        pam_template + {
                'name' : 'pam_systemd',
                'conditions' : ['HAVE_PAM', 'ENABLE_SYSTEMD_LOGIND'],
                'sources' : files('pam_systemd.c'),
                'version-script' : meson.current_source_dir() / 'pam_systemd.sym',
        },
        pam_template + {
                'name' : 'pam_systemd_loadkey',
                'conditions' : ['HAVE_PAM', 'ENABLE_SYSTEMD_LOGIND'],
                'sources' : files('pam_systemd_loadkey.c'),
                'version-script' : meson.current_source_dir() / 'pam_systemd_loadkey.sym',
        },
]

custom_target(
        input : 'logind.conf.in',
        output : 'logind.conf',
        command : [jinja2_cmdline, '@INPUT@', '@OUTPUT@'],
        install : install_sysconfdir_samples,
        install_dir : pkgconfigfiledir)

custom_target(
        input : 'systemd-user.in',
        output : 'systemd-user',
        command : [jinja2_cmdline, '@INPUT@', '@OUTPUT@'],
        install : pamconfdir != 'no',
        install_dir : pamconfdir)

if conf.get('ENABLE_SYSTEMD_LOGIND') == 1
        install_data(
                'org.freedesktop.login1.conf',
                install_dir : dbuspolicydir)
        install_data(
                'org.freedesktop.login1.service',
                install_dir : dbussystemservicedir)
        install_data(
                'org.freedesktop.login1.policy',
                install_dir : polkitpolicydir)
        install_data(
                '10-systemd-logind-root-ignore-inhibitors.rules.example',
                install_dir : polkitrulesdir)
endif
