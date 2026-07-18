#!/usr/bin/env python3
# SPDX-License-Identifier: LGPL-2.1-or-later
"""
Replace hardcoded runtime paths in systemd C/H source files with compile-time macros.

Replacements performed:
  "/run/..."   →  RUNSTATEDIR "/..."     (configurable via -Drunstatedir=)
  "/tmp/..."   →  SYSTEM_TMPDIR "/..."   (configurable via -Dtmpdir=)
  "/var/..."   →  LOCALSTATEDIR "/..."   (configurable via -Dlocalstatedir=)
  "/etc/systemd/..."  →  PKGSYSCONFDIR "/..."  (configurable via -Dsysconfdir=)
  "/etc/..."   →  SYSCONF_DIR "/..."     (configurable via -Dsysconfdir=)

Usage:
    python3 tools/replace-hardcoded-paths.py [--dry-run] [--include-tests]

Options:
    --dry-run        Report replacements without modifying any file.
    --include-tests  Also process test files (skipped by default).
"""

import argparse
import csv
import os
import re
import sys
from pathlib import Path

# Macros we inject
MACRO_RUN = "RUNSTATEDIR"
MACRO_TMP = "SYSTEM_TMPDIR"
MACRO_VAR = "LOCALSTATEDIR"
MACRO_ETC_SYSTEMD = "PKGSYSCONFDIR"
MACRO_ETC = "SYSCONF_DIR"

# Regex patterns for string literals
# Match any string literal starting with a known prefix.
# Inside string literals we accept anything except an unescaped ".
# This also matches \\0 (escaped NUL) and other escape sequences.
_RUN_RE = re.compile(r'"/run(/[^"]*|)"')
_TMP_RE = re.compile(r'"/tmp(/[^"]*|)"')
_VAR_RE = re.compile(r'"/var(/[^"]*|)"')
_ETC_SYSTEMD_RE = re.compile(r'"/etc/systemd(/[^"]*|)"')
# NOTE: _ETC_RE must come AFTER _ETC_SYSTEMD_RE in processing order
# so the more specific /etc/systemd/... match takes precedence.
_ETC_RE = re.compile(r'"/etc(/[^"]*|)"')

_DEFAULT_SKIP_DIRS = {
    "src/include",
}

_TEST_FILENAME_SUFFIXES = ("-test.", "_test.", "/test-", "/tests/",)


def _is_test_file(path: Path) -> bool:
    s = str(path)
    return any(pat in s for pat in _TEST_FILENAME_SUFFIXES) or path.name.startswith("test-")


def _replacement(macro: str, suffix: str) -> str:
    return f'{macro} "{suffix}"' if suffix else macro


def _process_line(line: str) -> tuple[str, list]:
    changes = []
    result = []
    i, n = 0, len(line)

    while i < n:
        if line[i] == '/' and i + 1 < n and line[i + 1] == '/':
            result.append(line[i:])
            break
        if line[i] == '"':
            j = i + 1
            while j < n:
                if line[j] == '\\':
                    j += 2; continue
                if line[j] == '"':
                    j += 1; break
                j += 1
            literal = line[i:j]

            new_literal = literal
            for pattern, macro in [
                    (_RUN_RE, MACRO_RUN),
                    (_TMP_RE, MACRO_TMP),
                    (_VAR_RE, MACRO_VAR),
                    (_ETC_SYSTEMD_RE, MACRO_ETC_SYSTEMD),
                    (_ETC_RE, MACRO_ETC),
            ]:
                def _repl(m, _m=macro):
                    old = m.group(0)
                    new = _replacement(_m, m.group(1))
                    if old != new:
                        changes.append((old, new))
                    return new
                new_literal = pattern.sub(_repl, new_literal)

            result.append(new_literal)
            i = j
            continue
        if line[i] == "'":
            j = i + 1
            while j < n:
                if line[j] == '\\':
                    j += 2; continue
                if line[j] == "'":
                    j += 1; break
                j += 1
            result.append(line[i:j])
            i = j
            continue
        result.append(line[i])
        i += 1

    return "".join(result), changes


def _process_file_lines(lines: list[str]) -> tuple[list[str], list]:
    new_lines = []
    all_changes = []
    in_block_comment = False

    for lineno, line in enumerate(lines, start=1):
        if in_block_comment:
            idx = line.find("*/")
            if idx != -1:
                in_block_comment = False
                prefix = line[:idx + 2]
                rest = line[idx + 2:]
                new_rest, changes = _process_line(rest)
                new_lines.append(prefix + new_rest)
                for old, new in changes:
                    all_changes.append((lineno, old, new))
            else:
                new_lines.append(line)
            continue

        new_line, changes = _process_line(line)
        new_lines.append(new_line)
        for old, new in changes:
            all_changes.append((lineno, old, new))

        i, n = 0, len(line)
        while i < n:
            if line[i] == '"':
                i += 1
                while i < n:
                    if line[i] == '\\':
                        i += 2; continue
                    if line[i] == '"':
                        i += 1; break
                    i += 1
                continue
            if line[i] == '/' and i + 1 < n:
                if line[i + 1] == '/':
                    break
                if line[i + 1] == '*':
                    end = line.find("*/", i + 2)
                    if end == -1:
                        in_block_comment = True; break
                    else:
                        i = end + 2; continue
            i += 1

    return new_lines, all_changes


def _discover_files(root: Path, include_tests: bool) -> list[Path]:
    files = []
    for dirpath, dirnames, filenames in os.walk(root):
        dp = Path(dirpath)
        dirnames[:] = [d for d in dirnames if not d.startswith(".")]
        rel = str(dp.relative_to(root.parent)) if root.parent != root else str(dp)
        if any(rel.endswith(sd) or ("/" + sd + "/") in rel for sd in _DEFAULT_SKIP_DIRS):
            dirnames.clear(); continue
        for fname in filenames:
            if not (fname.endswith(".c") or fname.endswith(".h")):
                continue
            fp = dp / fname
            if not include_tests and _is_test_file(fp):
                continue
            files.append(fp)
    return sorted(files)


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__,
        formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--dry-run", "-n", action="store_true")
    parser.add_argument("--include-tests", action="store_true")
    args = parser.parse_args()

    root = Path(__file__).resolve().parent.parent
    src_dir = root / "src"
    if not src_dir.is_dir():
        print(f"ERROR: src/ not found at {src_dir}", file=sys.stderr); return 1

    targets = _discover_files(src_dir, args.include_tests)
    total_changes = 0
    error_count = 0

    for fp in targets:
        try:
            text = fp.read_text(encoding="utf-8", errors="replace")
        except OSError as exc:
            print(f"ERROR: cannot read {fp}: {exc}", file=sys.stderr)
            error_count += 1; continue
        lines = text.splitlines(keepends=True)
        new_lines, changes = _process_file_lines(lines)
        if changes:
            total_changes += len(changes)
            for lineno, old, new in changes:
                print(f"{fp}:{lineno}: {old!r}  →  {new}")
            if not args.dry_run:
                try:
                    fp.write_text("".join(new_lines), encoding="utf-8")
                except OSError as exc:
                    print(f"ERROR: cannot write {fp}: {exc}", file=sys.stderr)
                    error_count += 1

    mode = "Would replace" if args.dry_run else "Replaced"
    print(f"\n{mode} {total_changes} occurrence(s) across {len(targets)} file(s) examined.")
    return 1 if error_count else 0


if __name__ == "__main__":
    sys.exit(main())
