"""The compilation script for this project using SCons."""
import os
import fnmatch


# the root directory for all build outputs
BUILD_DIR = '.build'
# the source directories that compile into variant directories under BUILD_DIR
SOURCE_DIRS = ['src', 'test', 'benchmark', 'main']
for source_dir in SOURCE_DIRS:
    VariantDir(os.path.join(BUILD_DIR, source_dir), source_dir, duplicate=0)
# keep the SCons signature database with the other build outputs
SConsignFile(os.path.join(BUILD_DIR, 'sconsign'))


# the compiler and linker flags for the testing C++ environment
TEST_FLAGS = [
    '-std=c++17',
    '-pthread',
    '-march=native',
    '-pipe',
    '-pedantic',
    '-Wall'
]


# includes for use in production code
INCLUDES = [
    '#include',
    '#vendor/binary-search-tree/include',
    '#vendor/cpptqdm/include',
    '#vendor/doubly-linked-list/include',
    '#vendor/hopscotch_map/include',
    '#vendor/robin-map/include',
    '#vendor/sparse-map/include',
]
# includes for use in testing and benchmarking code
TEST_INCLUDES = ['#vendor/Catch2/single_include/catch2']


# Create the testing environment
TESTING_ENV = Environment(
    ENV=os.environ,
    CXX='g++',
    CPPFLAGS=['-Wno-unused-value'],
    CXXFLAGS=TEST_FLAGS,
    LINKFLAGS=TEST_FLAGS,
    CPPPATH=INCLUDES + TEST_INCLUDES,
)


# the compiler and linker flags for the production C++ environment
PROD_FLAGS = [
    '-std=c++17',
    '-pthread',
    '-O2',
    '-march=native',
    '-pipe',
    '-pedantic',
    '-Wall'
]


# Create the production environment
PRODUCTION_ENV = Environment(
    ENV=os.environ,
    CXX='g++',
    CPPFLAGS=['-Wno-unused-value'],
    CXXFLAGS=PROD_FLAGS,
    LINKFLAGS=PROD_FLAGS,
    CPPPATH=INCLUDES,
)


# Create the benchmarking environment
BENCHMARK_ENV = Environment(
    ENV=os.environ,
    CXX='g++',
    CPPFLAGS=['-Wno-unused-value'],
    CXXFLAGS=PROD_FLAGS,
    LINKFLAGS=PROD_FLAGS,
    CPPPATH=INCLUDES + TEST_INCLUDES,
)


def find_source_files(src_dir):
    """
    Find all the source files in the given directory.

    Args:
        src_dir: the source directory to search through

    Returns:
        a sorted list of paths to cpp files in the variant directory of
        src_dir under BUILD_DIR

    """
    files = []
    for root, dirnames, filenames in os.walk(src_dir):
        root = os.path.join(BUILD_DIR, root)
        for filename in fnmatch.filter(filenames, '*.cpp'):
            files.append(os.path.join(root, filename))
    return sorted(files)


def source_name(build_file):
    """
    Return the source path of a file in a variant directory.

    Args:
        build_file: a path to a file in a variant directory under BUILD_DIR

    Returns:
        the path to the file relative to BUILD_DIR, e.g., test/test_lob.cpp

    """
    return os.path.relpath(build_file, BUILD_DIR)


# Locate all the C++ source files
SRC = find_source_files('src')
# create separate object files for testing and production environments
TEST_SRC = [TESTING_ENV.Object(f.replace('.cpp', '') + '-test', f) for f in SRC]
PROD_SRC = [PRODUCTION_ENV.Object(f.replace('.cpp', '') + '-prod', f) for f in SRC]
BENCHMARK_SRC = [BENCHMARK_ENV.Object(f.replace('.cpp', '') + '-bench', f) for f in SRC]


#
# MARK: Unit Tests
#


# create a list to store all the test target aliases in
UNIT_TEST_ALIASES = []
for file in find_source_files('test'):  # iterate over the test source files
    program = TESTING_ENV.Program(file.replace('.cpp', ''), [file] + TEST_SRC)
    # create an alias that builds and runs the test suite, e.g.,
    # `scons test/test_limit_tree.cpp`
    UNIT_TEST_ALIASES.append(Alias(source_name(file), [program], program[0].path))
    AlwaysBuild(UNIT_TEST_ALIASES[-1])


# create an alias to run all test suites
Alias('test', UNIT_TEST_ALIASES)


#
# MARK: Benchmarks
#


# create a list to store all the benchmark target aliases in
BENCHMARK_ALIASES = []
for file in find_source_files('benchmark'):
    program = BENCHMARK_ENV.Program(file.replace('.cpp', ''), [file] + BENCHMARK_SRC)
    # create an alias that builds and runs the benchmark, e.g.,
    # `scons benchmark/benchmark_lob.cpp`
    BENCHMARK_ALIASES.append(Alias(source_name(file), [program], program[0].path))
    AlwaysBuild(BENCHMARK_ALIASES[-1])


# create an alias to run all benchmarks
Alias('benchmark', BENCHMARK_ALIASES)


#
# MARK: Main
#


# create a list to store all the executable programs in
MAIN_PROGRAMS = []
for file in find_source_files('main'):
    program = PRODUCTION_ENV.Program(file.replace('.cpp', ''), [file] + PROD_SRC)
    MAIN_PROGRAMS.append(program)
    # create an alias that builds and runs the executable, e.g.,
    # `scons main/dummy_matching.cpp`
    AlwaysBuild(Alias(source_name(file), [program], program[0].path))


# create an alias to build (but not run) all executables
Alias('main', MAIN_PROGRAMS)


#
# MARK: Shared Library
#


# Create a shared library (it will add "lib" to the front automatically)
lib = PRODUCTION_ENV.SharedLibrary(os.path.join(BUILD_DIR, 'lib', '_limit_order_book.so'), SRC)
AlwaysBuild(lib)
# copy the shared library into the Python package
package_lib = Command(
    target='limit_order_book/${SOURCE.file}',
    source=lib,
    action=Copy('$TARGET', '$SOURCE'),
)
Alias('limit_order_book', package_lib)
# build the shared library and copy it into the package by default
Default(package_lib)
