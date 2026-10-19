"""The setup script for building the limit-order-book C++ extension.

Package metadata lives in pyproject.toml; this script only declares the
shared library that the Python package loads through ctypes.
"""
from glob import glob
import os
import sys
from setuptools import setup, Extension


# set the compiler for the C++ framework
os.environ['CC'] = 'g++'
os.environ['CCX'] = 'g++'


# The prefix name for the .so library to build. It will follow the format
# lib_lob.*.so where the * changes depending on the build system
LIB_NAME = 'limit_order_book.lib_limit_order_book'
# The source files for building the extension.
SOURCES = glob('src/*.cpp')
# The directories pointing to header files used by the extension. They have
# to be included using MANIFEST.in too to include the headers with sdist
INCLUDE_DIRS = [
    'include',
    'vendor/binary-search-tree/include',
    'vendor/cpptqdm/include',
    'vendor/doubly-linked-list/include',
    'vendor/hopscotch_map/include',
    'vendor/robin-map/include',
    'vendor/sparse-map/include',
]
# Build arguments to pass to the compiler. Wheels are built on CI machines
# and run elsewhere, so do not tune for the build host with -march=native.
if sys.platform == 'win32':
    EXTRA_COMPILE_ARGS = ['/std:c++14', '/O2']
else:
    EXTRA_COMPILE_ARGS = ['-std=c++1y', '-pipe', '-O3']
# The official extension using the name, source, headers, and build args
LIBRARY = Extension(LIB_NAME,
    sources=SOURCES,
    include_dirs=INCLUDE_DIRS,
    extra_compile_args=EXTRA_COMPILE_ARGS,
)


setup(ext_modules=[LIBRARY])
