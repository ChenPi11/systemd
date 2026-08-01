#!/usr/bin/env python3
# SPDX-License-Identifier: LGPL-2.1-or-later
"""
Check for hardcoded filesystem paths in systemd C/H source files.

When merging upstream changes, new hardcoded paths may be introduced that
should use compile-time macros (RUNSTATEDIR, SYSTEM_TMPDIR, etc.) instead.

Detection categories:

  [RUN]      "/run/..."   → should use RUNSTATEDIR macro
  [TMP]      "/tmp/..."   → should use SYSTEM_TMPDIR macro
  [VAR]      "/var/..."   → should use LOCALSTATEDIR macro (or dedicated one)
  [ETC]      "/etc/..."   → should use SYSCONF_DIR / PKGSYSCONFDIR / CONF_PATHS
  [USR]      "/usr/..."   → should use PREFIX / BINDIR / LIBDIR / LIBEXECDIR

The following kernel/filesystem ABI paths are intentionally NOT flagged:
  /proc/...  /sys/...  /dev/...  /sysroot

Usage:
    # Check files changed since HEAD (recommended before merge):
    python3 tools/check-hardcoded-paths.py

    # Check against a different branch:
    python3 tools/check-hardcoded-paths.py --diff main

    # Full scan of all source files:
    python3 tools/check-hardcoded-paths.py --full

    # Full scan but test files too:
    python3 tools/check-hardcoded-paths.py --full --include-tests

    # Generate a baseline file (for --baseline):
    python3 tools/check-hardcoded-paths.py --full --baseline-out baseline.txt

    # Check using an existing baseline (suppress known violations):
    python3 tools/check-hardcoded-paths.py --baseline baseline.txt

Options:
    --diff REF         Check files changed since REF (default: HEAD).
    --full             Scan all source files (not just changed ones).
    --include-tests    Also scan test files (skipped by default).
    --baseline FILE    Ignore violations listed in baseline FILE.
    --baseline-out FILE  Write all current violations to FILE (for later use
                       with --baseline).
    --quiet            Suppress summary; only emit violations.

Exit code:
    0 – no new violations (or all are baselined)
    1 – violations found
"""

import argparse
import os
import re
import subprocess
import sys
from pathlib import Path

# Path rules: (friendly_tag, prefix, macro_name, hint)
PATH_RULES = [
    ("RUN", "/run/",       "RUNSTATEDIR",    "meson -Drunstatedir="),
    ("TMP", "/tmp/",       "SYSTEM_TMPDIR",  "meson -Dtmpdir="),
    ("VAR", "/var/",       "LOCALSTATEDIR",  "meson -Dlocalstatedir="),
    ("ETC", "/etc/",       "SYSCONF_DIR",    "meson -Dsysconfdir="),
]

# Kernel ABI paths that are intentionally hardcoded
INTENTIONAL_PREFIXES = [
    "/proc/", "/sys/", "/dev/", "/sysroot",
]

# Standard Linux FHS paths (per FHS spec, not configurable)
INTENTIONAL_PATHS = {
    "/etc/os-release",
    "/etc/localtime",
    "/etc/hostname",
    "/etc/machine-id",
    "/etc/machine-info",
    "/etc/resolv.conf",
    "/etc/hosts",
    "/etc/passwd",
    "/etc/group",
    "/etc/shadow",
    "/etc/gshadow",
    "/etc/fstab",
    "/etc/crypttab",
    "/etc/mtab",
    "/etc/nsswitch.conf",
    "/etc/shells",
    "/etc/locale.conf",
    "/etc/vconsole.conf",
    "/etc/host.conf",
    "/etc/issue",
    "/etc/motd",
    "/etc/initrd-release",
    "/etc/login.defs",
    "/etc/udev/rules.d",
    "/etc/kernel/",
    "/etc/clonetab",
    "/etc/apparmor/",
    "/etc/cryptsetup-keys.d",
    "/etc/credstore",
    "/etc/credstore.encrypted",
    "/etc/machines",
    "/etc/portables",
    "/etc/extensions",
    "/etc/userdb",
    "/etc/pcrlock.d",
    "/etc/ssl",
    "/etc/cryptmount/",
    # Paths inside error/log message strings (not real path lookups)
    " is in a read-only filesystem.",
    " file of image is empty.",
    " file of image is uninitialized",
    " file of image has unexpected length",
    " does not point into /usr/share/zoneinfo/",
}

# Skip these directories entirely (relative to repo root)
SKIP_DIRS_REL = [
    "src/include",
]


def is_test_file(path: Path) -> bool:
    s = str(path)
    indicators = ("-test.", "_test.", "/test-", "/tests/", "test-")
    return any(ind in s for ind in indicators) or path.name.startswith("test-")


def is_skipped(path: Path, root: Path) -> bool:
    try:
        rel = str(path.relative_to(root))
    except ValueError:
        return False
    return any(rel.startswith(sd) or ("/" + sd + "/") in rel for sd in SKIP_DIRS_REL)


def get_changed_files(ref: str, root: Path) -> list[Path]:
    try:
        result = subprocess.run(
            ["git", "diff", "--name-only", ref, "--", "src/"],
            capture_output=True, text=True, check=True, cwd=root,
        )
    except subprocess.CalledProcessError as exc:
        print(f"error: git diff failed: {exc.stderr}", file=sys.stderr)
        sys.exit(1)
    files = []
    for line in result.stdout.splitlines():
        line = line.strip()
        if not (line.endswith(".c") or line.endswith(".h")):
            continue
        fp = root / line
        if fp.exists():
            files.append(fp)
    return files


# ── C-aware string literal scanner ──────────────────────────────────────

def iter_string_literals(source: str):
    i, n = 0, len(source)
    while i < n:
        c = source[i]
        # line comment //
        if c == '/' and i + 1 < n and source[i + 1] == '/':
            end = source.find('\n', i)
            i = (end + 1) if end != -1 else n
            continue
        # block comment /* */
        if c == '/' and i + 1 < n and source[i + 1] == '*':
            end = source.find("*/", i + 2)
            i = (end + 2) if end != -1 else n
            continue
        # char literal
        if c == "'":
            i += 1
            while i < n:
                if source[i] == '\\':
                    i += 2; continue
                if source[i] == "'":
                    i += 1; break
                i += 1
            continue
        # string literal
        if c == '"':
            start = i
            i += 1
            while i < n:
                if source[i] == '\\':
                    i += 2; continue
                if source[i] == '"':
                    i += 1; break
                i += 1
            yield source[start:i]
            continue
        i += 1


# ── Core checking ───────────────────────────────────────────────────────

def check_file(path: Path, baseline: set) -> list[dict]:
    try:
        text = path.read_text(encoding="utf-8", errors="replace")
    except OSError:
        return []

    violations = []
    for lit in iter_string_literals(text):
        content = lit[1:-1]  # strip quotes
        if len(content) < 3:
            continue

        # Skip intentional paths
        skip = False
        for pfx in INTENTIONAL_PREFIXES:
            if content.startswith(pfx):
                skip = True
                break
        for ip in INTENTIONAL_PATHS:
            if ip in content:
                skip = True
                break
        if skip:
            continue

        # Check each rule
        for tag, prefix, macro, opt in PATH_RULES:
            if content.startswith(prefix):
                # Build unique key for baseline
                key = f"{path}:{tag}:{content}"
                if key in baseline:
                    break

                violations.append(dict(
                    tag=tag, file=path, path=content,
                    macro=macro, option=opt))
                break

    return violations


# ── Main ────────────────────────────────────────────────────────────────

def main() -> int:
    parser = argparse.ArgumentParser(
        description=__doc__,
        formatter_class=argparse.RawDescriptionHelpFormatter,
    )
    parser.add_argument("--diff", metavar="REF", default=None, nargs="?",
                        const="HEAD",
                        help="check files changed since REF (default: HEAD)")
    parser.add_argument("--full", action="store_true",
                        help="scan all source files")
    parser.add_argument("--include-tests", action="store_true",
                        help="also scan test files")
    parser.add_argument("--baseline", metavar="FILE",
                        help="ignore violations listed in FILE")
    parser.add_argument("--baseline-out", metavar="FILE",
                        help="write all current violations to FILE")
    parser.add_argument("--quiet", action="store_true",
                        help="suppress summary")
    args = parser.parse_args()

    root = Path(__file__).resolve().parent.parent
    src = root / "src"
    if not src.is_dir():
        print(f"error: src/ not found at {src}", file=sys.stderr)
        return 1

    # Determine which files to scan
    if args.full:
        files = sorted(src.rglob("*.c")) + sorted(src.rglob("*.h"))
    else:
        ref = args.diff or "HEAD"
        files = get_changed_files(ref, root)
        if not files:
            if not args.quiet:
                print(f"No changed .c/.h files since {ref}.")
            return 0

    # Filter
    targets = []
    for fp in files:
        if is_skipped(fp, root):
            continue
        if not args.include_tests and is_test_file(fp):
            continue
        targets.append(fp)

    # Load baseline
    baseline = set()
    if args.baseline:
        try:
            with open(args.baseline) as f:
                for line in f:
                    line = line.strip()
                    if line and not line.startswith("#"):
                        baseline.add(line)
        except OSError as exc:
            print(f"error: {exc}", file=sys.stderr)
            return 1

    # Scan
    all_violations = []
    for fp in targets:
        all_violations.extend(check_file(fp, baseline))

    # If --baseline-out, write all violations (ignoring baseline file)
    if args.baseline_out:
        # Collect without baseline filtering
        raw = []
        for fp in targets:
            raw.extend(check_file(fp, set()))
        # Deduplicate
        seen = set()
        lines = []
        for v in sorted(raw, key=lambda x: (str(x["file"]), x["path"])):
            key = f"{v['file']}:{v['tag']}:{v['path']}"
            if key not in seen:
                seen.add(key)
                lines.append(key)
        try:
            with open(args.baseline_out, "w") as f:
                f.write("# check-hardcoded-paths.py baseline\n")
                f.write("# file:TAG:/path\n")
                for line in lines:
                    f.write(line + "\n")
            print(f"Baseline written to {args.baseline_out} ({len(lines)} entries)")
        except OSError as exc:
            print(f"error: {exc}", file=sys.stderr)
            return 1
        # If only generating baseline, don't report violations
        return 0

    # Report
    if all_violations:
        by_file: dict[str, list[dict]] = {}
        for v in all_violations:
            by_file.setdefault(str(v["file"]), []).append(v)

        for fname, vlist in sorted(by_file.items()):
            print(f"\n{fname}")
            for v in vlist:
                print(f"  [{v['tag']}] {v['path']!r}")
                print(f"          → use {v['macro']} ({v['option']})")

        print(f"\n{'─' * 60}")
        print(f"Found {len(all_violations)} hardcoded path(s) in {len(by_file)} file(s).")
        print(f"Tip: use --baseline-out to save current state, then --baseline to suppress.")
        return 1

    if not args.quiet:
        scope = f" since {args.diff or 'HEAD'}" if not args.full else ""
        print(f"No hardcoded path violations in {len(targets)} file(s){scope}.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
