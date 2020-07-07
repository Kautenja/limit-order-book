"""The compilation script for this project using SCons."""
import os
import fnmatch
import re


# create a separate build directory
VariantDir('build', 'src', duplicate=0)
VariantDir('build_benchmark', 'benchmark', duplicate=0)
VariantDir('build_test', 'test', duplicate=0)
VariantDir('build_main', 'main', duplicate=0)


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


def find_source_files(src_dir, build_dir):
    """
    Find all the source files in the given directory.

    Args:
        src_dir: the source directory to search through
        build_dir: the build directory (to replace src_dir with)

    Returns:
        a list of paths to cpp files where src_dir is replaced by build_dir
    """
    files = []
    for root, dirnames, filenames in os.walk(src_dir):
        root = re.sub(src_dir, build_dir, root)
        for filename in fnmatch.filter(filenames, '*.cpp'):
            files.append(os.path.join(root, filename))
    return sorted(files)


# Locate all the C++ source files
SRC = find_source_files('src', 'build')
# create separate object files for testing and production environments
TEST_SRC = [TESTING_ENV.Object(f.replace('.cpp', '') + '-test', f) for f in SRC]
PROD_SRC = [PRODUCTION_ENV.Object(f.replace('.cpp', '') + '-prod', f) for f in SRC]
BENCHMARK_SRC = [BENCHMARK_ENV.Object(f.replace('.cpp', '') + '-bench', f) for f in SRC]


#
# MARK: Unit Tests
#


# locate all the testing source files
TEST_FILES = find_source_files('test/', 'build_test/')
# create a list to store all the test target aliases in
UNIT_TEST_ALIASES = []
for file in TEST_FILES:  # iterate over all the test source files
    UNIT_TEST_PROGRAM = TESTING_ENV.Program(file.replace('.cpp', ''), [file] + TEST_SRC)
    UNIT_TEST_ALIASES.append(Alias('test/' + file.replace('build_test/', ''), [UNIT_TEST_PROGRAM], UNIT_TEST_PROGRAM[0].path))
    AlwaysBuild(UNIT_TEST_ALIASES[-1])


# create an alias to run all test suites
Alias("test", UNIT_TEST_ALIASES)


#
# MARK: Benchmarks
#


for benchmark in find_source_files('benchmark', 'build_benchmark'):
    # create the binary
    binary_name = benchmark.replace('.cpp', '')
    program = BENCHMARK_ENV.Program(binary_name, [benchmark] + BENCHMARK_SRC)
    # create a scons alias for the binary
    shell_name = benchmark.replace('build_', '')
    benchmark_alias = Alias(shell_name, [program], program[0].path)
    AlwaysBuild(benchmark_alias)


#
# MARK: Main
#


for main in find_source_files('main', 'build_main'):
    # create the binary
    binary_name = main.replace('.cpp', '')
    program = PRODUCTION_ENV.Program(binary_name, [main] + PROD_SRC)
    # create a scons alias for the binary
    shell_name = main.replace('build_', '')
    benchmark_alias = Alias(shell_name, [program], program[0].path)
    AlwaysBuild(benchmark_alias)


# Create a shared library (it will add "lib" to the front automatically)
lib = PRODUCTION_ENV.SharedLibrary('_limit_order_book.so', SRC)
AlwaysBuild(lib)
# copy the so file to the lob package
Command(target="limit_order_book", source=lib, action=Copy("$TARGET", "$SOURCE"))
