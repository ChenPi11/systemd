/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

/* Pull in the override shim layer, which in turn pulls in the system bionic <spawn.h>.
 * The include order for bionic builds is:
 *   src/include/bionic/spawn.h  (this file)
 *   → src/include/override/spawn.h  (via #include_next)
 *   → system bionic <spawn.h>        (via override's #include_next)
 *
 * On Android/bionic, pidfd_spawn() and posix_spawnattr_setcgroup_np() are GNU extensions
 * that are absent at all currently-supported API levels
 * (HAVE_PIDFD_SPAWN=0, HAVE_POSIX_SPAWNATTR_SETCGROUP_NP=0 from meson probes).
 *
 * The override/spawn.h shim layer handles these functions by:
 *   - providing forward declarations for pidfd_spawn_shim() / posix_spawnattr_setcgroup_np_shim()
 *   - redirecting all call sites via #define to the *_shim() wrappers
 *   - the wrappers (compiled in src/libc/spawn.c) attempt dlsym() at DSO-load time; on bionic
 *     dlsym() returns NULL for both functions, so all calls return ENOSYS — the correct
 *     behaviour for an API that is genuinely absent.
 *
 * POSIX_SPAWN_SETCGROUP (0x100) and POSIX_SPAWN_SETSID are defined by override/spawn.h
 * when not already provided by the system header. */
#include_next <spawn.h>        /* IWYU pragma: export */
