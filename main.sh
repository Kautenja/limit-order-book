#!/bin/bash
#
# Usage:
#     ./main.sh [options] [command] [args...]
#
# Options:
#     -h    Print documentation about this script
#
# Commands:
#     python              Print the Python interpreter used by this project
#     install             Create/use .venv and install development tools
#     configure           Configure the CMake build in .build/cmake
#     build               Build the C++ tests, benchmarks, and the
#                         Cython extension (copied into limit_order_book/)
#     test                Run the C++ and Python test suites
#     unittest            Alias for test
#     test-cpp            Build and run the Catch2 test suite with ctest
#     test-python         Build the Cython extension and run the Python suite
#     test-package        Install the headers and C API library into
#                         .build/prefix and build the C++ and C consumers
#                         with find_package and with FetchContent
#     benchmark           Build and run the Catch2 benchmarks
#     benchmark-python    Build and benchmark the public Python interface
#     install-cpp         Install the headers and CMake package; pass extra
#                         args to `cmake --install`, e.g., --prefix /opt/lob
#     clean               Remove build artifacts and Python bytecode
#     deployment          Clean, then build source and wheel distributions
#     dist                Alias for deployment
#     package             Alias for deployment
#     ship                Run tests and build distributions for a release
#     release             Alias for ship
#     all                 Run tests and build distributions
#     *                   Execute the command directly from the project root
#
# Environment:
#     PYTHON              The Python interpreter when .venv does not exist
#     CMAKE_BUILD_TYPE    The build configuration (default: Release)
#
# Examples:
#     ./main.sh install
#     ./main.sh test
#     ./main.sh benchmark
#     ./main.sh install-cpp --prefix "${HOME}/.local"
#     ./main.sh deployment
#

set -euo pipefail

print_help() {
  sed -ne '/^#/!q;s/.\{1,2\}//;1d;p' < "$0"
}

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "${SCRIPT_DIR}"
# Git Bash on Windows reports /c/... paths that CMake cannot read; convert
# them to C:/... paths, which both Bash and CMake understand
if command -v cygpath > /dev/null 2>&1; then
  SCRIPT_DIR="$(cygpath -m "${SCRIPT_DIR}")"
fi

# the virtual environment interpreter differs between POSIX and Windows
if [[ -x "${SCRIPT_DIR}/.venv/bin/python" ]]; then
  PYTHON="${SCRIPT_DIR}/.venv/bin/python"
elif [[ -x "${SCRIPT_DIR}/.venv/Scripts/python.exe" ]]; then
  PYTHON="${SCRIPT_DIR}/.venv/Scripts/python.exe"
else
  PYTHON="${PYTHON:-python3}"
fi

# C++ and Python build outputs; the report keeps its own local .build directory
BUILD_ROOT="${SCRIPT_DIR}/.build"
BUILD_DIR="${BUILD_ROOT}/cmake"
BUILD_TYPE="${CMAKE_BUILD_TYPE:-Release}"

while getopts ":h" optname; do
  case "$optname" in
  "h")
    print_help
    exit 0
    ;;
  "?")
    echo "Unknown option ${OPTARG}" >&2
    exit 1
    ;;
  ":")
    echo "No argument value for option ${OPTARG}" >&2
    exit 1
    ;;
  *)
    echo "Unknown error while processing options" >&2
    exit 1
    ;;
  esac
done

shift $((OPTIND - 1))

COMMAND="${1:-}"
if [[ $# -gt 0 ]]; then
  shift
fi

run_install() {
  if [[ ! -x "${SCRIPT_DIR}/.venv/bin/python" &&
        ! -x "${SCRIPT_DIR}/.venv/Scripts/python.exe" ]]; then
    echo "Creating ${SCRIPT_DIR}/.venv"
    if ! "${PYTHON}" -m venv "${SCRIPT_DIR}/.venv"; then
      echo "Failed to create .venv. Install python3-venv if your platform splits it out." >&2
      exit 1
    fi
  fi

  local venv_python="${SCRIPT_DIR}/.venv/bin/python"
  if [[ ! -x "${venv_python}" ]]; then
    venv_python="${SCRIPT_DIR}/.venv/Scripts/python.exe"
  fi
  "${venv_python}" -m pip install --upgrade pip
  "${venv_python}" -m pip install -r requirements.txt
}

run_configure() {
  cmake -S . -B "${BUILD_DIR}" \
    -DCMAKE_BUILD_TYPE="${BUILD_TYPE}" \
    -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
    -DLOB_BUILD_TESTS=ON \
    -DLOB_BUILD_BENCHMARKS=ON \
    -DLOB_BUILD_PYTHON_LIBRARY=OFF \
    "$@"
}

run_build() {
  run_configure -DLOB_BUILD_PYTHON_LIBRARY=ON \
    -DPython_EXECUTABLE="$("${PYTHON}" -c 'import sys; print(sys.executable)')"
  cmake --build "${BUILD_DIR}" --config "${BUILD_TYPE}" --parallel "$@"
}

run_test_cpp() {
  run_configure
  cmake --build "${BUILD_DIR}" --config "${BUILD_TYPE}" --parallel
  ctest --test-dir "${BUILD_DIR}" --build-config "${BUILD_TYPE}" --output-on-failure "$@"
}

run_build_python() {
  run_configure -DLOB_BUILD_TESTS=OFF -DLOB_BUILD_BENCHMARKS=OFF \
    -DLOB_BUILD_PYTHON_LIBRARY=ON \
    -DPython_EXECUTABLE="$("${PYTHON}" -c 'import sys; print(sys.executable)')"
  cmake --build "${BUILD_DIR}" --config "${BUILD_TYPE}" --target limit_order_book_native
}

run_test_python() {
  run_build_python
  "${PYTHON}" -m unittest discover . "$@"
}

run_test_package() {
  local prefix="${BUILD_ROOT}/prefix"
  local build_dir="${BUILD_ROOT}/package-build"
  local fetch_args=(
    -DLOB_PACKAGE_TEST_FETCHCONTENT=ON
    -DFETCHCONTENT_SOURCE_DIR_LIMIT_ORDER_BOOK="${SCRIPT_DIR}"
  )
  # install the header-only package and the C API library from a build tree
  # that skips the tests, which `./main.sh test` covers
  cmake -S . -B "${build_dir}" \
    -DCMAKE_BUILD_TYPE="${BUILD_TYPE}" \
    -DLOB_BUILD_TESTS=OFF \
    -DLOB_BUILD_PYTHON_LIBRARY=OFF \
    -DLOB_BUILD_C_LIBRARY=ON
  cmake --build "${build_dir}" --config "${BUILD_TYPE}"
  rm -rf "${prefix}"
  cmake --install "${build_dir}" --config "${BUILD_TYPE}" --prefix "${prefix}"

  # consume the installed C++ and C packages with find_package
  run_package_consumer package-find -DCMAKE_PREFIX_PATH="${prefix}" -DLOB_PACKAGE_TEST_C=ON
  # consume this checkout with FetchContent, header-only and with the C API
  run_package_consumer package-fetch "${fetch_args[@]}"
  run_package_consumer package-fetch-c "${fetch_args[@]}" -DLOB_PACKAGE_TEST_C=ON
}

# Configure, build, and test test/package in .build/<name> with extra args.
run_package_consumer() {
  local build_dir="${BUILD_ROOT}/$1"
  shift
  cmake -S test/package -B "${build_dir}" -DCMAKE_BUILD_TYPE="${BUILD_TYPE}" "$@"
  cmake --build "${build_dir}" --config "${BUILD_TYPE}"
  ctest --test-dir "${build_dir}" --build-config "${BUILD_TYPE}" --output-on-failure
}

run_benchmark() {
  run_configure
  cmake --build "${BUILD_DIR}" --config "${BUILD_TYPE}" --target limit_order_book_benchmarks
  # multi-config generators (Visual Studio, Xcode) nest binaries by config,
  # and Windows binaries end in .exe
  local binary
  for binary in \
      "${BUILD_DIR}/limit_order_book_benchmarks" \
      "${BUILD_DIR}/${BUILD_TYPE}/limit_order_book_benchmarks" \
      "${BUILD_DIR}/${BUILD_TYPE}/limit_order_book_benchmarks.exe"; do
    if [[ -x "${binary}" ]]; then
      "${binary}" "$@"
      return
    fi
  done
  echo "Could not find the limit_order_book_benchmarks binary" >&2
  exit 1
}

run_install_cpp() {
  run_configure
  cmake --build "${BUILD_DIR}" --config "${BUILD_TYPE}"
  cmake --install "${BUILD_DIR}" --config "${BUILD_TYPE}" "$@"
}

run_clean() {
  rm -rf "${BUILD_ROOT}" "${SCRIPT_DIR}/docs/whitepaper/.build"
  find limit_order_book -type f -name "*.pyc" -exec rm -f {} +
  find limit_order_book -type d -name "__pycache__" -prune -exec rm -rf {} +
  find limit_order_book -maxdepth 1 -type f \
    \( -name "lib_limit_order_book*" -o -name "_native*.so" -o -name "_native*.pyd" \) \
    -exec rm -f {} +
}

run_test() {
  run_build
  ctest --test-dir "${BUILD_DIR}" --build-config "${BUILD_TYPE}" --output-on-failure
  "${PYTHON}" -m unittest discover . "$@"
}

run_deployment() {
  run_clean
  "${PYTHON}" -m build --outdir "${BUILD_ROOT}/dist"
}

run_ship() {
  run_test
  run_deployment
  cat <<'EOF'

Distributions are ready in .build/dist/.
Publish releases through the "Publish to PyPI" GitHub Actions workflow by
creating a GitHub release from a version-matching tag.
EOF
}

case "${COMMAND}" in

"")
  print_help
  exit 0
  ;;

"python")
  echo "${PYTHON}"
  exit 0
  ;;

"install")
  run_install
  exit 0
  ;;

"configure")
  run_configure "$@"
  exit 0
  ;;

"build")
  run_build "$@"
  exit 0
  ;;

"test" | "unittest")
  run_test "$@"
  exit 0
  ;;

"test-cpp")
  run_test_cpp "$@"
  exit 0
  ;;

"test-python")
  run_test_python "$@"
  exit 0
  ;;

"test-package")
  run_test_package
  exit 0
  ;;

"benchmark")
  run_benchmark "$@"
  exit 0
  ;;

"benchmark-python")
  run_build_python
  "${PYTHON}" benchmark/benchmark_python.py "$@"
  exit 0
  ;;

"install-cpp")
  run_install_cpp "$@"
  exit 0
  ;;

"clean")
  run_clean
  exit 0
  ;;

"deployment" | "dist" | "package")
  run_deployment
  exit 0
  ;;

"ship" | "release")
  run_ship
  exit 0
  ;;

"all")
  run_test
  run_deployment
  exit 0
  ;;

*)
  "${COMMAND}" "$@"
  exit 0
  ;;

esac
