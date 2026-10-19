#!/usr/bin/env python3
"""Import pinned robin-map Git blobs or verify the vendored snapshot offline."""

import argparse
import hashlib
import json
import re
import subprocess
import sys
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
DESTINATION = ROOT / 'vendor/robin-map'
MANIFEST = ROOT / 'vendor/robin-map.json'
REPOSITORY = 'https://github.com/Tessil/robin-map.git'
FILES = (
    'LICENSE',
    'include/tsl/robin_growth_policy.h',
    'include/tsl/robin_hash.h',
    'include/tsl/robin_map.h',
    'include/tsl/robin_set.h',
)


def digest(content):
    """Return the SHA-256 of unmodified upstream bytes."""
    return hashlib.sha256(content).hexdigest()


def validate_manifest(manifest):
    """Reject incomplete provenance or an unexpected import file set."""
    if manifest['repository'] != REPOSITORY:
        raise ValueError('unexpected upstream repository')
    if not re.fullmatch(r'[0-9a-f]{40}', manifest['commit']):
        raise ValueError('commit must be a full lowercase Git SHA-1')
    if not re.fullmatch(r'[0-9]+\.[0-9]+\.[0-9]+', manifest['version']):
        raise ValueError('version must have the form 1.3.0')
    if set(manifest['files']) != set(FILES):
        raise ValueError('manifest must list all four headers and LICENSE')
    if any(not re.fullmatch(r'[0-9a-f]{64}', value)
           for value in manifest['files'].values()):
        raise ValueError('file hashes must be lowercase SHA-256 values')


def verify(manifest):
    """Check the exact file set and contents without Git or network access."""
    validate_manifest(manifest)
    paths = list(DESTINATION.rglob('*'))
    if DESTINATION.is_symlink() or any(path.is_symlink() for path in paths):
        raise ValueError('vendored snapshot must not contain symlinks')
    actual = {path.relative_to(DESTINATION).as_posix()
              for path in paths if path.is_file()}
    if actual != set(FILES):
        raise ValueError(f'vendored file set differs: {sorted(actual ^ set(FILES))}')
    for name, expected in manifest['files'].items():
        if digest((DESTINATION / name).read_bytes()) != expected:
            raise ValueError(f'checksum mismatch: {name}')


def git_blob(source, commit, name):
    """Read a committed blob, ignoring modifications in the upstream checkout."""
    return subprocess.run(
        ['git', '-C', str(source), 'show', f'{commit}:{name}'],
        check=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE,
    ).stdout


def import_snapshot(source, manifest, updating):
    """Validate every source blob before writing the destination snapshot."""
    validate_manifest(manifest)
    cmake = git_blob(source, manifest['commit'], 'CMakeLists.txt').decode('utf-8')
    version = re.search(r'project\(tsl-robin-map\s+VERSION\s+([0-9.]+)', cmake)
    if not version or version.group(1) != manifest['version']:
        raise ValueError('upstream CMake version does not match the manifest')
    contents = {name: git_blob(source, manifest['commit'], name) for name in FILES}
    hashes = {name: digest(content) for name, content in contents.items()}
    if updating:
        manifest['files'] = hashes
    elif hashes != manifest['files']:
        raise ValueError('upstream blob checksums do not match the manifest')

    paths = list(DESTINATION.rglob('*'))
    if DESTINATION.is_symlink() or any(path.is_symlink() for path in paths):
        raise ValueError('refusing to replace a snapshot containing symlinks')
    for name, content in contents.items():
        destination = DESTINATION / name
        destination.parent.mkdir(parents=True, exist_ok=True)
        destination.write_bytes(content)
    # Only this dependency directory is pruned; the sibling manifest is retained.
    for path in sorted(paths, key=lambda item: len(item.parts), reverse=True):
        if path.is_file() and path.relative_to(DESTINATION).as_posix() not in FILES:
            path.unlink()
        elif path.is_dir() and not any(path.iterdir()):
            path.rmdir()
    MANIFEST.write_text(json.dumps(manifest, indent=2) + '\n', encoding='utf-8')
    verify(manifest)


def main():
    """Reproduce the manifest by default; change pins only with explicit options."""
    parser = argparse.ArgumentParser(description=__doc__)
    mode = parser.add_mutually_exclusive_group(required=True)
    mode.add_argument('--check', action='store_true', help='Verify local files only.')
    mode.add_argument('--source', type=Path, help='Upstream Git clone containing the commit.')
    parser.add_argument('--version', help='New upstream release, for example 1.3.0.')
    parser.add_argument('--commit', help='New full upstream commit SHA-1.')
    args = parser.parse_args()
    if bool(args.version) != bool(args.commit) or (args.check and args.version):
        parser.error('--version and --commit must be used together with --source')
    if args.version:
        manifest = dict(repository=REPOSITORY, version=args.version,
                        commit=args.commit, files={name: '0' * 64 for name in FILES})
    else:
        manifest = json.loads(MANIFEST.read_text(encoding='utf-8'))
    if args.check:
        verify(manifest)
    else:
        import_snapshot(args.source, manifest, updating=bool(args.version))
    print(f"Verified robin-map {manifest['version']} ({manifest['commit']}): "
          f'{len(FILES)} files, no local patches.')


if __name__ == '__main__':
    try:
        main()
    except (OSError, ValueError, KeyError, TypeError, subprocess.CalledProcessError) as error:
        print(f'robin-map update failed: {error}', file=sys.stderr)
        sys.exit(1)
