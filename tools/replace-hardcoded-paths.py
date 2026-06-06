#!/usr/bin/env python3
# SPDX-License-Identifier: LGPL-2.1-or-later
"""
Replace hardcoded runtime paths in systemd C/H source files with compile-time macros.

Replacements performed:
  "/run/..."  →  RUNSTATEDIR "/..."    (defined in config.h via meson 'runstatedir' option, default /run)
  "/tmp/..."  →  SYSTEM_TMPDIR "/..."  (defined in config.h via meson 'tmpdir' option, default /tmp)

The macro RUNSTATEDIR expands to the string "/run" (or whatever runstatedir is set to),
so RUNSTATEDIR "/foo" becomes "/run/foo" via C string literal concatenation.

Usage:
    # Dry run – only report what would change, no files modified:
    python3 tools/replace-hardcoded-paths.py --dry-run [FILES...]

    # Apply replacements:
    python3 tools/replace-hardcoded-paths.py [FILES...]

    # Process entire src/ tree (default when no files given):
    python3 tools/replace-hardcoded-paths.py

Options:
    --dry-run        Report replacements without modifying any file.
    --include-tests  Also process test files (skipped by default).
    --report FILE    Write a machine-readable CSV report to FILE.
    FILES...         Explicit list of files to process (overrides auto-discovery).

Exit code:
    0  – success (even if nothing was replaced)
    1  – one or more files could not be read/written
"""

import argparse
import csv
import os
import re
import sys
from pathlib import Path

# ---------------------------------------------------------------------------
# Macros we inject
# ---------------------------------------------------------------------------
MACRO_RUN = "RUNSTATEDIR"
MACRO_TMP = "SYSTEM_TMPDIR"

# ---------------------------------------------------------------------------
# Regex patterns
#
# We match a complete C string literal whose text starts with /run or /tmp.
# The pattern captures the suffix after the root directory:
#   Group 1: the trailing part including the leading slash, e.g. "/systemd/io.foo"
#            or empty string when the path is exactly "/run" or "/tmp".
#
# We deliberately do NOT match:
#   /sys/   – Linux sysfs (not configurable)
#   /proc/  – Linux procfs (not configurable)
#   /dev/   – device filesystem (not configurable)
# ---------------------------------------------------------------------------
_RUN_RE = re.compile(r'"/run(/[^"\\]*|)"')
_TMP_RE = re.compile(r'"/tmp(/[^"\\]*|)"')

# ---------------------------------------------------------------------------
# Files/directories that are intentionally skipped by default
# ---------------------------------------------------------------------------
_DEFAULT_SKIP_DIRS = {
    "src/include",          # uapi headers – not our code
    "src/test",             # test helpers – paths tested literally
}

_TEST_FILENAME_SUFFIXES = (
    "-test.",
    "_test.",
    "/test-",
    "/tests/",
)


def _is_test_file(path: Path) -> bool:
    s = str(path)
    return any(pat in s for pat in _TEST_FILENAME_SUFFIXES) or path.name.startswith("test-")


# ---------------------------------------------------------------------------
# Line-level processing
# ---------------------------------------------------------------------------

def _make_replacement(macro: str, suffix: str) -> str:
    """Build the replacement token sequence for a matched path."""
    if suffix:
        # e.g. suffix = "/systemd/io.foo"  →  RUNSTATEDIR "/systemd/io.foo"
        return f'{macro} "{suffix}"'
    else:
        # The path was exactly "/run" or "/tmp"
        return macro


def _process_line(line: str) -> tuple[str, list[tuple[str, str]]]:
    """
    Apply all substitutions to *line* and return (new_line, changes).

    changes is a list of (original_match, replacement) pairs.
    Lines (or portions of lines) that are inside C comments are left alone.
    """
    changes: list[tuple[str, str]] = []

    # We need to avoid replacing inside comments.  We do a single left-to-right
    # scan, keeping track of whether we are inside a // or /* */ comment, and
    # inside a string literal.  Only string literals outside comments are
    # candidates for replacement.
    result: list[str] = []
    i = 0
    n = len(line)

    while i < n:
        # --- line comment: rest of line is untouched ---
        if line[i] == '/' and i + 1 < n and line[i + 1] == '/':
            result.append(line[i:])
            break

        # --- string literal ---
        if line[i] == '"':
            # Collect the full string literal (handling escape sequences)
            j = i + 1
            while j < n:
                if line[j] == '\\':
                    j += 2
                    continue
                if line[j] == '"':
                    j += 1
                    break
                j += 1
            literal = line[i:j]

            # Try substitutions on the literal
            new_literal = literal
            for pattern, macro in ((_RUN_RE, MACRO_RUN), (_TMP_RE, MACRO_TMP)):
                def _repl(m, _macro=macro):
                    old = m.group(0)
                    new = _make_replacement(_macro, m.group(1))
                    if old != new:
                        changes.append((old, new))
                    return new

                new_literal = pattern.sub(_repl, new_literal)

            result.append(new_literal)
            i = j
            continue

        # --- character literal: skip ---
        if line[i] == "'":
            j = i + 1
            while j < n:
                if line[j] == '\\':
                    j += 2
                    continue
                if line[j] == "'":
                    j += 1
                    break
                j += 1
            result.append(line[i:j])
            i = j
            continue

        result.append(line[i])
        i += 1

    return "".join(result), changes


def _process_file_lines(
    lines: list[str],
) -> tuple[list[str], list[tuple[int, str, str]]]:
    """
    Process all lines of a file.

    Returns (new_lines, all_changes) where all_changes is a list of
    (1-based line number, original_match, replacement).
    """
    new_lines: list[str] = []
    all_changes: list[tuple[int, str, str]] = []
    in_block_comment = False

    for lineno, line in enumerate(lines, start=1):
        if in_block_comment:
            # Look for end of block comment
            idx = line.find("*/")
            if idx != -1:
                in_block_comment = False
                # The rest of the line after */ may have code – process it
                prefix = line[: idx + 2]
                rest = line[idx + 2 :]
                new_rest, changes = _process_line(rest)
                new_lines.append(prefix + new_rest)
                for old, new in changes:
                    all_changes.append((lineno, old, new))
            else:
                new_lines.append(line)
            continue

        # Check for start of block comment somewhere on this line
        # We need to handle the line in segments: code / block-comment / code …
        # For simplicity we process the whole line with _process_line first,
        # then check if a block comment starts and remains open.
        new_line, changes = _process_line(line)
        new_lines.append(new_line)
        for old, new in changes:
            all_changes.append((lineno, old, new))

        # Update block-comment state (rough: count /* and */ outside strings)
        # This mirrors what _process_line does but tracks block-comment open/close.
        i = 0
        n = len(line)
        while i < n:
            if line[i] == '"':
                i += 1
                while i < n:
                    if line[i] == '\\':
                        i += 2
                        continue
                    if line[i] == '"':
                        i += 1
                        break
                    i += 1
                continue
            if line[i] == '/' and i + 1 < n:
                if line[i + 1] == '/':
                    break  # rest is line comment
                if line[i + 1] == '*':
                    # Find matching */
                    end = line.find("*/", i + 2)
                    if end == -1:
                        in_block_comment = True
                        break
                    else:
                        i = end + 2
                        continue
            i += 1

    return new_lines, all_changes


# ---------------------------------------------------------------------------
# File discovery
# ---------------------------------------------------------------------------

def _discover_files(root: Path, include_tests: bool) -> list[Path]:
    files: list[Path] = []
    for dirpath, dirnames, filenames in os.walk(root):
        dp = Path(dirpath)
        # Skip hidden dirs
        dirnames[:] = [d for d in dirnames if not d.startswith(".")]
        # Skip configured skip dirs
        rel = dp.relative_to(root.parent) if root.parent != root else dp
        rel_str = str(rel)
        if any(rel_str.endswith(sd) or ("/" + sd + "/") in rel_str for sd in _DEFAULT_SKIP_DIRS):
            dirnames.clear()
            continue
        for fname in filenames:
            if not (fname.endswith(".c") or fname.endswith(".h")):
                continue
            fp = dp / fname
            if not include_tests and _is_test_file(fp):
                continue
            files.append(fp)
    return sorted(files)


# ---------------------------------------------------------------------------
# Main
# ---------------------------------------------------------------------------

def main() -> int:
    parser = argparse.ArgumentParser(
        description=__doc__,
        formatter_class=argparse.RawDescriptionHelpFormatter,
    )
    parser.add_argument(
        "--dry-run", "-n",
        action="store_true",
        help="report changes without modifying files",
    )
    parser.add_argument(
        "--include-tests",
        action="store_true",
        help="also process test files (skipped by default)",
    )
    parser.add_argument(
        "--report",
        metavar="FILE",
        help="write CSV report to FILE (columns: file,line,original,replacement)",
    )
    parser.add_argument(
        "files",
        nargs="*",
        metavar="FILE",
        help="files to process (default: all .c/.h under src/)",
    )
    args = parser.parse_args()

    # Locate repository root
    script_dir = Path(__file__).resolve().parent
    repo_root = script_dir.parent

    # Collect files to process
    if args.files:
        targets = [Path(f) for f in args.files]
    else:
        src_dir = repo_root / "src"
        if not src_dir.is_dir():
            print(f"ERROR: src/ directory not found at {src_dir}", file=sys.stderr)
            return 1
        targets = _discover_files(src_dir, args.include_tests)

    # Open CSV report if requested
    report_writer = None
    report_file = None
    if args.report:
        report_file = open(args.report, "w", newline="", encoding="utf-8")
        report_writer = csv.writer(report_file)
        report_writer.writerow(["file", "line", "original", "replacement"])

    total_changes = 0
    error_count = 0

    for fp in targets:
        try:
            text = fp.read_text(encoding="utf-8", errors="replace")
        except OSError as exc:
            print(f"ERROR: cannot read {fp}: {exc}", file=sys.stderr)
            error_count += 1
            continue

        lines = text.splitlines(keepends=True)
        new_lines, changes = _process_file_lines(lines)

        if changes:
            total_changes += len(changes)
            for lineno, old, new in changes:
                print(f"{fp}:{lineno}: {old!r}  →  {new}")
                if report_writer:
                    report_writer.writerow([str(fp), lineno, old, new])

            if not args.dry_run:
                try:
                    fp.write_text("".join(new_lines), encoding="utf-8")
                except OSError as exc:
                    print(f"ERROR: cannot write {fp}: {exc}", file=sys.stderr)
                    error_count += 1

    if report_file:
        report_file.close()

    mode = "Would replace" if args.dry_run else "Replaced"
    print(f"\n{mode} {total_changes} occurrence(s) across {len(targets)} file(s) examined.")
    if args.report and not args.dry_run:
        print(f"Report written to: {args.report}")

    return 1 if error_count else 0


if __name__ == "__main__":
    sys.exit(main())
