# The CMake build for the header-only C++ limit order book, its Catch2 tests
# and benchmarks, and the C API shared library loaded by the Python package.
#
# Consumers can use the library in three ways:
#
# -   FetchContent or add_subdirectory, linking limit_order_book::limit_order_book
# -   find_package(limit_order_book CONFIG) after `cmake --install`
# -   pkg-config with limit_order_book.pc after `cmake --install`
#
# With LOB_BUILD_C_LIBRARY=ON, the C API in include/lib_lob.h is also built
# as a shared library, exported as limit_order_book::c, and described by
# limit_order_book_c.pc.
#
# The project version is the single source of truth for the C++ package and
# the Python distribution; pyproject.toml reads it from this file.

cmake_minimum_required(VERSION 3.21...4.2)

project(limit_order_book
    VERSION 2.0.0
    DESCRIPTION "A limit order book with a price-time priority matching policy"
    HOMEPAGE_URL "https://github.com/Kautenja/limit-order-book"
    LANGUAGES CXX
)

include(GNUInstallDirs)

# MARK: Options

# scikit-build-core sets SKBUILD when building the Python distribution
if(DEFINED SKBUILD)
    set(LOB_IS_PYTHON_BUILD ON)
else()
    set(LOB_IS_PYTHON_BUILD OFF)
endif()
# development targets default on only for a top-level, non-Python build, so
# FetchContent consumers and wheel builds compile nothing but what they need
if(PROJECT_IS_TOP_LEVEL AND NOT LOB_IS_PYTHON_BUILD)
    set(LOB_IS_DEVELOPER_BUILD ON)
else()
    set(LOB_IS_DEVELOPER_BUILD OFF)
endif()
if(PROJECT_IS_TOP_LEVEL OR LOB_IS_PYTHON_BUILD)
    set(LOB_DEFAULT_BUILD_PYTHON_LIBRARY ON)
else()
    set(LOB_DEFAULT_BUILD_PYTHON_LIBRARY OFF)
endif()

option(LOB_BUILD_TESTS "Build the Catch2 test suite" ${LOB_IS_DEVELOPER_BUILD})
option(LOB_BUILD_BENCHMARKS "Build the Catch2 benchmarks" OFF)
option(LOB_BUILD_EXAMPLES "Build the executables in main/" OFF)
option(
    LOB_BUILD_PYTHON_LIBRARY
    "Build the C API shared library that the Python package loads"
    ${LOB_DEFAULT_BUILD_PYTHON_LIBRARY}
)
option(
    LOB_BUILD_C_LIBRARY
    "Build and install the C API shared library as limit_order_book::c"
    OFF
)
option(
    LOB_INSTALL
    "Generate install rules for the headers, CMake package, and pkg-config file"
    ${LOB_IS_DEVELOPER_BUILD}
)

# default single-config top-level builds to an optimized build
if(PROJECT_IS_TOP_LEVEL AND NOT CMAKE_CONFIGURATION_TYPES AND
        NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "The build type" FORCE)
endif()

# MARK: Header-Only Library

# the vendored header-only dependencies that the public headers include
set(LOB_VENDOR_INCLUDE_DIRS
    "${PROJECT_SOURCE_DIR}/vendor/binary-search-tree/include"
    "${PROJECT_SOURCE_DIR}/vendor/doubly-linked-list/include"
    "${PROJECT_SOURCE_DIR}/vendor/robin-map/include"
)
# the directory, relative to the install prefix, that holds every header so
# that `#include "limit_order_book.hpp"` works the same as in the source tree
set(LOB_INSTALL_INCLUDEDIR "${CMAKE_INSTALL_INCLUDEDIR}/limit_order_book")

add_library(limit_order_book INTERFACE)
add_library(limit_order_book::limit_order_book ALIAS limit_order_book)
target_include_directories(limit_order_book INTERFACE
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
    "$<BUILD_INTERFACE:${LOB_VENDOR_INCLUDE_DIRS}>"
    "$<INSTALL_INTERFACE:${LOB_INSTALL_INCLUDEDIR}>"
)
# the headers are C++14 so the MSVC default standard can build the wheels
target_compile_features(limit_order_book INTERFACE cxx_std_14)

# MARK: First-Party Targets

# build first-party targets as C++14 to keep the headers C++14-clean
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Apply the project warning flags to a first-party compiled target.
function(lob_configure_target target_name)
    target_link_libraries(${target_name} PRIVATE limit_order_book)
    target_compile_options(${target_name} PRIVATE
        "$<$<CXX_COMPILER_ID:MSVC>:/W3>"
        "$<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall;-pedantic>"
    )
endfunction()

# MARK: C API Shared Library

# one shared library serves both C consumers and the Python package, which
# loads it through ctypes under the name lib_limit_order_book
if(LOB_BUILD_C_LIBRARY OR LOB_BUILD_PYTHON_LIBRARY)
    add_library(limit_order_book_c SHARED "${PROJECT_SOURCE_DIR}/src/lib_lob.cpp")
    add_library(limit_order_book::c ALIAS limit_order_book_c)
    lob_configure_target(limit_order_book_c)
    target_include_directories(limit_order_book_c PUBLIC
        "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
        "$<INSTALL_INTERFACE:${LOB_INSTALL_INCLUDEDIR}>"
    )
    # lib_lob.h exports the C functions; hide every other symbol
    target_compile_definitions(limit_order_book_c PRIVATE LOB_C_BUILDING_LIBRARY)
    set_target_properties(limit_order_book_c PROPERTIES
        EXPORT_NAME c
        CXX_VISIBILITY_PRESET hidden
        VISIBILITY_INLINES_HIDDEN ON
    )
    if(LOB_BUILD_C_LIBRARY)
        # a conventional, versioned library: liblimit_order_book.so.2 or
        # limit_order_book.dll
        set_target_properties(limit_order_book_c PROPERTIES
            OUTPUT_NAME limit_order_book
            VERSION "${PROJECT_VERSION}"
            SOVERSION "${PROJECT_VERSION_MAJOR}"
        )
    else()
        # a plain library named lib_limit_order_book on every platform
        set_target_properties(limit_order_book_c PROPERTIES
            PREFIX ""
            OUTPUT_NAME lib_limit_order_book
        )
    endif()
    # the file name that limit_order_book/library.py loads
    set(LOB_PYTHON_LIBRARY_NAME "lib_limit_order_book${CMAKE_SHARED_LIBRARY_SUFFIX}")
    if(LOB_BUILD_PYTHON_LIBRARY AND LOB_IS_PYTHON_BUILD)
        install(FILES "$<TARGET_FILE:limit_order_book_c>"
            DESTINATION limit_order_book
            RENAME "${LOB_PYTHON_LIBRARY_NAME}"
        )
    elseif(LOB_BUILD_PYTHON_LIBRARY)
        # copy the library into the source package so that
        # `python -m unittest discover .` runs against the fresh build
        add_custom_command(TARGET limit_order_book_c POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                "$<TARGET_FILE:limit_order_book_c>"
                "${PROJECT_SOURCE_DIR}/limit_order_book/${LOB_PYTHON_LIBRARY_NAME}"
            COMMENT "Copying the C API library into the Python package"
            VERBATIM
        )
    endif()
endif()

# MARK: Tests And Benchmarks

if(LOB_BUILD_TESTS OR LOB_BUILD_BENCHMARKS)
    include(FetchContent)
    FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.8.1
        GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(Catch2)
    list(APPEND CMAKE_MODULE_PATH "${catch2_SOURCE_DIR}/extras")
endif()

if(LOB_BUILD_TESTS)
    enable_testing()
    file(GLOB LOB_TEST_SOURCES CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/test/*.cpp")
    add_executable(limit_order_book_tests ${LOB_TEST_SOURCES})
    lob_configure_target(limit_order_book_tests)
    target_link_libraries(limit_order_book_tests PRIVATE Catch2::Catch2WithMain)
    include(Catch)
    catch_discover_tests(limit_order_book_tests)
endif()

if(LOB_BUILD_BENCHMARKS)
    file(GLOB LOB_BENCHMARK_SOURCES CONFIGURE_DEPENDS
        "${PROJECT_SOURCE_DIR}/benchmark/*.cpp"
    )
    add_executable(limit_order_book_benchmarks ${LOB_BENCHMARK_SOURCES})
    lob_configure_target(limit_order_book_benchmarks)
    target_link_libraries(limit_order_book_benchmarks PRIVATE Catch2::Catch2WithMain)
endif()

# MARK: Examples

if(LOB_BUILD_EXAMPLES)
    file(GLOB LOB_EXAMPLE_SOURCES CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/main/*.cpp")
    foreach(source ${LOB_EXAMPLE_SOURCES})
        get_filename_component(example_name "${source}" NAME_WE)
        add_executable(${example_name} "${source}")
        lob_configure_target(${example_name})
    endforeach()
endif()

# MARK: Install

if(LOB_INSTALL)
    include(CMakePackageConfigHelpers)
    # the directory for the CMake package files, which are arch-independent
    # unless the package includes the C API shared library
    if(LOB_BUILD_C_LIBRARY)
        set(LOB_INSTALL_CMAKEDIR "${CMAKE_INSTALL_LIBDIR}/cmake/limit_order_book")
        set(LOB_PACKAGE_ARCH_INDEPENDENT "")
    else()
        set(LOB_INSTALL_CMAKEDIR "${CMAKE_INSTALL_DATADIR}/cmake/limit_order_book")
        set(LOB_PACKAGE_ARCH_INDEPENDENT ARCH_INDEPENDENT)
    endif()
    # the directory for the arch-independent pkg-config file
    set(LOB_INSTALL_PKGCONFIGDIR "${CMAKE_INSTALL_DATADIR}/pkgconfig")

    # install the first-party and vendored headers into one directory
    install(DIRECTORY "${PROJECT_SOURCE_DIR}/include/"
        DESTINATION "${LOB_INSTALL_INCLUDEDIR}"
        FILES_MATCHING PATTERN "*.hpp"
    )
    # the trailing slash installs each directory's contents, not the directory
    foreach(vendor_dir ${LOB_VENDOR_INCLUDE_DIRS})
        install(DIRECTORY "${vendor_dir}/"
            DESTINATION "${LOB_INSTALL_INCLUDEDIR}"
            FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp"
        )
    endforeach()

    # install the license for this project and each vendored dependency
    install(FILES "${PROJECT_SOURCE_DIR}/LICENSE" "${PROJECT_SOURCE_DIR}/LICENSING.md"
        DESTINATION "${CMAKE_INSTALL_DOCDIR}"
    )
    install(FILES "${PROJECT_SOURCE_DIR}/vendor/binary-search-tree/LICENSE"
        DESTINATION "${CMAKE_INSTALL_DOCDIR}/licenses/binary-search-tree"
    )
    install(FILES "${PROJECT_SOURCE_DIR}/vendor/doubly-linked-list/LICENSE"
        DESTINATION "${CMAKE_INSTALL_DOCDIR}/licenses/doubly-linked-list"
    )
    install(FILES "${PROJECT_SOURCE_DIR}/vendor/robin-map/LICENSE"
        DESTINATION "${CMAKE_INSTALL_DOCDIR}/licenses/robin-map"
    )

    # export the header-only target as limit_order_book::limit_order_book
    install(TARGETS limit_order_book EXPORT limit_order_book_targets)
    # export the C API library as limit_order_book::c, with its C header
    if(LOB_BUILD_C_LIBRARY)
        install(TARGETS limit_order_book_c EXPORT limit_order_book_targets)
        install(FILES "${PROJECT_SOURCE_DIR}/include/lib_lob.h"
            DESTINATION "${LOB_INSTALL_INCLUDEDIR}"
        )
    endif()
    install(EXPORT limit_order_book_targets
        FILE limit_order_bookTargets.cmake
        NAMESPACE limit_order_book::
        DESTINATION "${LOB_INSTALL_CMAKEDIR}"
    )
    configure_package_config_file(
        "${PROJECT_SOURCE_DIR}/cmake/limit_order_bookConfig.cmake.in"
        "${PROJECT_BINARY_DIR}/limit_order_bookConfig.cmake"
        INSTALL_DESTINATION "${LOB_INSTALL_CMAKEDIR}"
    )
    write_basic_package_version_file(
        "${PROJECT_BINARY_DIR}/limit_order_bookConfigVersion.cmake"
        COMPATIBILITY SameMajorVersion
        ${LOB_PACKAGE_ARCH_INDEPENDENT}
    )
    install(FILES
        "${PROJECT_BINARY_DIR}/limit_order_bookConfig.cmake"
        "${PROJECT_BINARY_DIR}/limit_order_bookConfigVersion.cmake"
        DESTINATION "${LOB_INSTALL_CMAKEDIR}"
    )

    # the pkg-config prefix is relative to the .pc file so that
    # `cmake --install --prefix` relocates it correctly
    file(RELATIVE_PATH LOB_PKGCONFIG_TO_PREFIX
        "/prefix/${LOB_INSTALL_PKGCONFIGDIR}" "/prefix"
    )
    string(REGEX REPLACE "/$" "" LOB_PKGCONFIG_TO_PREFIX "${LOB_PKGCONFIG_TO_PREFIX}")
    configure_file(
        "${PROJECT_SOURCE_DIR}/cmake/limit_order_book.pc.in"
        "${PROJECT_BINARY_DIR}/limit_order_book.pc"
        @ONLY
    )
    install(FILES "${PROJECT_BINARY_DIR}/limit_order_book.pc"
        DESTINATION "${LOB_INSTALL_PKGCONFIGDIR}"
    )
    # the C API library is arch-specific, so its pkg-config file is too
    if(LOB_BUILD_C_LIBRARY)
        set(LOB_INSTALL_PKGCONFIGDIR_C "${CMAKE_INSTALL_LIBDIR}/pkgconfig")
        file(RELATIVE_PATH LOB_PKGCONFIG_C_TO_PREFIX
            "/prefix/${LOB_INSTALL_PKGCONFIGDIR_C}" "/prefix"
        )
        string(REGEX REPLACE "/$" "" LOB_PKGCONFIG_C_TO_PREFIX "${LOB_PKGCONFIG_C_TO_PREFIX}")
        configure_file(
            "${PROJECT_SOURCE_DIR}/cmake/limit_order_book_c.pc.in"
            "${PROJECT_BINARY_DIR}/limit_order_book_c.pc"
            @ONLY
        )
        install(FILES "${PROJECT_BINARY_DIR}/limit_order_book_c.pc"
            DESTINATION "${LOB_INSTALL_PKGCONFIGDIR_C}"
        )
    endif()
endif()
