"""Exercise the import command in a disposable checkout with local Git sources."""

import hashlib
import json
import shutil
import subprocess
import sys
import tempfile
import unittest
from pathlib import Path


class UpdateRobinMapTests(unittest.TestCase):
    """Check reproducibility and failure behavior without touching the real vendor tree."""

    def setUp(self):
        self.scratch = tempfile.TemporaryDirectory(prefix='lob-vendor-test-')
        self.addCleanup(self.scratch.cleanup)
        self.root = Path(self.scratch.name)
        (self.root / 'scripts').mkdir()
        shutil.copyfile(Path(__file__).with_name('update_robin_map.py'),
                        self.root / 'scripts/update_robin_map.py')
        self.source = self.root / 'upstream'
        self.source.mkdir()
        subprocess.run(['git', 'init', '--quiet', str(self.source)], check=True)
        subprocess.run(['git', '-C', str(self.source), 'config', 'core.autocrlf', 'false'],
                       check=True)
        self.files = ['LICENSE', 'include/tsl/robin_map.h',
                      'include/tsl/robin_set.h', 'include/tsl/robin_hash.h',
                      'include/tsl/robin_growth_policy.h']
        for name in self.files:
            path = self.source / name
            path.parent.mkdir(parents=True, exist_ok=True)
            path.write_bytes(b'upstream bytes\r\n')
        (self.source / 'CMakeLists.txt').write_text(
            'project(tsl-robin-map VERSION 1.3.0 LANGUAGES CXX)\n')
        subprocess.run(['git', '-C', str(self.source), 'add', '.'], check=True)
        subprocess.run(['git', '-C', str(self.source), '-c', 'user.name=Vendor Test',
                        '-c', 'user.email=vendor-test@example.invalid',
                        '-c', 'commit.gpgsign=false', 'commit', '--quiet', '-m', 'fixture'],
                       check=True)
        self.commit = subprocess.check_output(
            ['git', '-C', str(self.source), 'rev-parse', 'HEAD'], text=True).strip()
        self.snapshot = self.root / 'vendor/robin-map'
        self.manifest = self.root / 'vendor/robin-map.json'
        self.run_tool('--source', str(self.source), '--version', '1.3.0',
                      '--commit', self.commit)

    def run_tool(self, *args, success=True):
        result = subprocess.run(
            [sys.executable, str(self.root / 'scripts/update_robin_map.py'), *args],
            capture_output=True, text=True,
        )
        self.assertEqual(result.returncode == 0, success, result.stderr)
        return result

    def snapshot_bytes(self):
        return {str(path.relative_to(self.root)): path.read_bytes()
                for path in (self.root / 'vendor').rglob('*') if path.is_file()}

    def test_reimport_uses_committed_bytes_and_prunes_only_dependency(self):
        before = self.snapshot_bytes()
        (self.source / 'LICENSE').write_text('uncommitted upstream edit')
        (self.snapshot / 'discard').mkdir()
        (self.snapshot / 'discard/README.md').write_text('not retained')
        sibling = self.root / 'vendor/keep.txt'
        sibling.write_text('another dependency')
        self.run_tool('--source', str(self.source))
        self.assertEqual(sibling.read_text(), 'another dependency')
        sibling.unlink()
        self.assertEqual(self.snapshot_bytes(), before)
        self.run_tool('--check')
        manifest = json.loads(self.manifest.read_text())
        for name in self.files:
            self.assertEqual(manifest['files'][name],
                             hashlib.sha256(b'upstream bytes\r\n').hexdigest())

    def test_check_detects_altered_missing_and_extra_files(self):
        license_path = self.snapshot / 'LICENSE'
        original = license_path.read_bytes()
        license_path.write_bytes(b'changed')
        self.run_tool('--check', success=False)
        license_path.unlink()
        self.run_tool('--check', success=False)
        license_path.write_bytes(original)
        (self.snapshot / 'extra.txt').write_text('unexpected')
        self.run_tool('--check', success=False)

    def test_checksum_failure_does_not_change_any_files(self):
        manifest = json.loads(self.manifest.read_text())
        manifest['files']['LICENSE'] = '0' * 64
        self.manifest.write_text(json.dumps(manifest))
        before = self.snapshot_bytes()
        self.run_tool('--source', str(self.source), success=False)
        self.assertEqual(self.snapshot_bytes(), before)

    def test_invalid_version_or_missing_commit_leaves_snapshot_unchanged(self):
        before = self.snapshot_bytes()
        for version, commit in [('1.4.0', self.commit), ('1.3.0', '0' * 40),
                                ('1.3.0', '--help')]:
            self.run_tool('--source', str(self.source), '--version', version,
                          '--commit', commit, success=False)
            self.assertEqual(self.snapshot_bytes(), before)

    def test_integrity_check_does_not_require_the_upstream_clone(self):
        shutil.rmtree(self.source)
        self.run_tool('--check')

    def test_autocrlf_checkout_preserves_vendored_bytes(self):
        attributes = Path(__file__).resolve().parents[1] / '.gitattributes'
        shutil.copyfile(attributes, self.root / '.gitattributes')
        subprocess.run(['git', 'init', '--quiet', str(self.root)], check=True)
        subprocess.run(['git', '-C', str(self.root), 'config', 'core.autocrlf', 'true'],
                       check=True)
        subprocess.run(['git', '-C', str(self.root), 'add', '.gitattributes',
                        'vendor/robin-map'], check=True, stderr=subprocess.PIPE)
        checkout = self.root / 'checkout'
        subprocess.run(['git', '-C', str(self.root), 'checkout-index', '--all',
                        '--prefix=' + checkout.as_posix() + '/'], check=True)
        for name in self.files:
            self.assertEqual((checkout / 'vendor/robin-map' / name).read_bytes(),
                             (self.snapshot / name).read_bytes())


if __name__ == '__main__':
    unittest.main()
