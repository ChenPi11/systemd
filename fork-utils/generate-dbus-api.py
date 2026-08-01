#!/usr/bin/env python3
# Copyright (C) 2026 ChenPi11
# SPDX-License-Identifier: LGPL-2.1-or-later
"""Generate D-Bus API documentation (YAML) from systemd's --bus-introspect output.

Usage:
    python3 fork-utils/generate-dbus-api.py <path-to-systemd-binary> [output-dir]
"""

import subprocess
import sys
import xml.etree.ElementTree as ET
from pathlib import Path

import yaml

DEFAULT_OUT = Path(__file__).resolve().parent / 'dbus-api'

yaml.Dumper.ignore_aliases = lambda self, data: True  # type: ignore[assignment]


def _str_repr(dumper: yaml.Dumper, data: str) -> yaml.Node:
    if '\n' in data:
        return dumper.represent_scalar('tag:yaml.org,2002:str', data, style='|')
    return dumper.represent_scalar('tag:yaml.org,2002:str', data)


yaml.add_representer(str, _str_repr)


def get_interfaces(systemd_bin: str) -> list[tuple[str, str]]:
    """List all registered D-Bus object paths and interface names."""
    out = subprocess.check_output(  # noqa: S603
        [systemd_bin, '--bus-introspect=list'],
        text=True,
        stderr=subprocess.STDOUT,
    )
    result = []
    for line in out.strip().splitlines():
        path, iface = line.split('\t', 1)
        result.append((path, iface))
    return result


def get_introspect_xml(systemd_bin: str, name: str) -> str:
    """Get D-Bus introspection XML for a given path or interface name."""
    return subprocess.check_output(  # noqa: S603
        [systemd_bin, '--bus-introspect', name],
        text=True,
        stderr=subprocess.STDOUT,
    )


def _annotations(elem: ET.Element) -> dict:
    result = {}
    for a in elem.findall('./annotation'):
        n = a.get('name', '')
        v = a.get('value', '')
        if n == 'org.freedesktop.DBus.Property.EmitsChangedSignal' and v == 'const':
            continue
        if n == 'org.freedesktop.systemd1.Privileged':
            result['privileged'] = True
        if n == 'org.freedesktop.DBus.Method.NoReply':
            result['no_reply'] = True
    return result


def _collect_std_interfaces(root: ET.Element) -> list:
    """Collect standard D-Bus interfaces (Peer, Introspectable, Properties)."""
    result = []
    for std_name in (
        'org.freedesktop.DBus.Peer',
        'org.freedesktop.DBus.Introspectable',
        'org.freedesktop.DBus.Properties',
    ):
        iface = root.find(f"./interface[@name='{std_name}']")
        if iface is None:
            continue
        obj: dict = {'interface': std_name, 'methods': [], 'signals': []}
        for m in iface.findall('./method'):
            meth = {
                'name': m.get('name'),
                'in_args': [
                    {'name': a.get('name'), 'type': a.get('type')}
                    for a in m.findall("./arg[@direction='in']")
                ],
                'out_args': [
                    {'name': a.get('name'), 'type': a.get('type')}
                    for a in m.findall("./arg[@direction='out']")
                ],
                'annotations': _annotations(m),
            }
            obj['methods'].append(meth)
        for s in iface.findall('./signal'):
            sig = {
                'name': s.get('name'),
                'args': [
                    {'name': a.get('name'), 'type': a.get('type')}
                    for a in s.findall('./arg')
                ],
            }
            obj['signals'].append(sig)
        result.append(obj)
    return result


def build_data(iface_name: str, obj_path: str, xml_str: str) -> dict:
    """Build a dict representing the full D-Bus interface definition."""
    root = ET.fromstring(xml_str)  # noqa: S314
    y: dict = {
        'interface': iface_name,
        'object_path': obj_path,
        'properties': [],
        'methods': [],
        'signals': [],
        'standard_interfaces': [],
    }

    target = root.find(f"./interface[@name='{iface_name}']")
    if target is None:
        return y

    for p in target.findall('./property'):
        y['properties'].append(
            {
                'name': p.get('name'),
                'type': p.get('type'),
                'access': p.get('access'),
                'annotations': _annotations(p),
            },
        )

    for m in target.findall('./method'):
        meth = {
            'name': m.get('name'),
            'in_args': [
                {'name': a.get('name'), 'type': a.get('type')}
                for a in m.findall("./arg[@direction='in']")
            ],
            'out_args': [
                {'name': a.get('name'), 'type': a.get('type')}
                for a in m.findall("./arg[@direction='out']")
            ],
            'annotations': _annotations(m),
        }
        y['methods'].append(meth)

    for s in target.findall('./signal'):
        sig = {
            'name': s.get('name'),
            'args': [
                {'name': a.get('name'), 'type': a.get('type')}
                for a in s.findall('./arg')
            ],
        }
        y['signals'].append(sig)

    y['standard_interfaces'] = _collect_std_interfaces(root)
    return y


MIN_ARGS = 2


def main() -> None:
    """Generate D-Bus API documentation (YAML) from systemd's --bus-introspect output."""
    if len(sys.argv) < MIN_ARGS:
        sys.stderr.write(f'Usage: {sys.argv[0]} <systemd-binary> [output-dir]\n')
        sys.exit(1)

    systemd_bin = sys.argv[1]
    out_dir = Path(sys.argv[2]) if len(sys.argv) > MIN_ARGS else DEFAULT_OUT
    system_bus_dir = out_dir / 'system_bus'
    user_bus_dir = out_dir / 'user_bus'
    system_bus_dir.mkdir(parents=True, exist_ok=True)
    user_bus_dir.mkdir(parents=True, exist_ok=True)

    interfaces = get_interfaces(systemd_bin)
    seen: set[str] = set()
    unique_interfaces = {i for _, i in interfaces}

    for obj_path, iface_name in interfaces:
        if iface_name in seen:
            continue
        seen.add(iface_name)

        print(f'[{len(seen)}/{len(unique_interfaces)}] {iface_name} ...')  # noqa: T201

        xml_str = get_introspect_xml(systemd_bin, iface_name)
        data = build_data(iface_name, obj_path, xml_str)

        fname = f'{iface_name}.yaml'
        yml = yaml.dump(
            data,
            default_flow_style=False,
            allow_unicode=True,
            sort_keys=True,
        )
        (system_bus_dir / fname).write_text(yml, encoding='utf-8')
        (user_bus_dir / fname).write_text(yml, encoding='utf-8')

    print(  # noqa: T201
        f'\nDone. {len(seen)} interface(s) written to {out_dir}/{{system,user}}_bus/',
    )


if __name__ == '__main__':
    main()
