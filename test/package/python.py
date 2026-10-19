"""Run the Python suite against an installed wheel, outside checkout imports.

Install the wheel, then run ``python -I test/package/python.py``.
"""
import importlib.util
from pathlib import Path
import sys
import unittest

import limit_order_book
from limit_order_book import _native


def main():
    """Load checkout tests under the installed package's module namespace."""
    root = Path(__file__).resolve().parents[2]
    package_path = Path(limit_order_book.__file__).resolve()
    if package_path.parent == root / 'limit_order_book':
        raise RuntimeError('test must import an installed wheel, not the checkout')
    print(f'Testing installed extension: {_native.__file__}', flush=True)
    suite = unittest.TestSuite()
    for path in sorted((root / 'limit_order_book' / 'test').glob('test_*.py')):
        name = f'limit_order_book.test.{path.stem}'
        spec = importlib.util.spec_from_file_location(name, path)
        module = importlib.util.module_from_spec(spec)
        sys.modules[name] = module
        spec.loader.exec_module(module)
        suite.addTests(unittest.defaultTestLoader.loadTestsFromModule(module))
    result = unittest.TextTestRunner(verbosity=2).run(suite)
    return 0 if result.wasSuccessful() else 1


if __name__ == '__main__':
    sys.exit(main())
