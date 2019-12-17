"""The compilation script for this project using SCons."""
import os
import fnmatch
import re


# create a separate build directory
VariantDir('build_benchmark', 'benchmark', duplicate=0)
VariantDir('build_test', 'test', duplicate=0)


# the compiler and linker flags for the testing C++ environment
TEST_FLAGS = [
    '-std=c++11',
    '-march=native',
    '-pipe',
    '-pedantic',
    '-Wall'
]


# includes for use in production code
INCLUDES = ['#include']
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
    '-std=c++11',
    '-O2',
    '-march=native',
    '-pipe',
    '-pedantic',
    '-Wall'
]


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
BENCHMARK_SRC = [BENCHMARK_ENV.Object(f.replace('.cpp', '') + '-bench', f) for f in SRC]


#
# MARK: Unit Tests
#


# the path to the main test file
MAIN_TEST_FILE = 'test/main.cpp'
MAIN_TEST_FILE_BUILD = 'build_test/main.cpp'


def create_main_test_file():
    """Create the main test file by locating files to '#include'."""
    # delete the main.cpp file if it exists
    if os.path.isfile(MAIN_TEST_FILE):
        os.remove(MAIN_TEST_FILE)
    files = find_source_files('test/', '')
    # iterate over the files to create the include script as a string
    includes = '\n'.join(map(lambda x: '#include "{}"'.format(x), files))
    # create the contents of the file from the string of includes and template
    with open(MAIN_TEST_FILE, 'w') as test_runner:
        test_runner.write("#define CATCH_CONFIG_MAIN\n{}".format(includes))


# create the test runner
create_main_test_file()
UNIT_TEST_PROGRAM = TESTING_ENV.Program('build_test/test', [MAIN_TEST_FILE_BUILD] + TEST_SRC)
# create a scons alias for the binary
UNIT_TEST_ALIAS = Alias('test', [UNIT_TEST_PROGRAM], UNIT_TEST_PROGRAM[0].path)
AlwaysBuild(UNIT_TEST_ALIAS)


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
