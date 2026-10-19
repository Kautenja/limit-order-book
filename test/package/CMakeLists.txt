# A consumer project that checks the limit_order_book CMake package works the
# way downstream projects use it. By default it uses find_package against an
# installed copy; with LOB_PACKAGE_TEST_FETCHCONTENT=ON it uses FetchContent.
# With LOB_PACKAGE_TEST_C=ON it also builds a C program against the C API
# library, limit_order_book::c, which must be installed or built with
# LOB_BUILD_C_LIBRARY=ON.
#
# Installed package:
#
#     cmake -S test/package -B .build/package -DCMAKE_PREFIX_PATH=<prefix>
#
# FetchContent of this checkout instead of the GitHub repository:
#
#     cmake -S test/package -B .build/package \
#         -DLOB_PACKAGE_TEST_FETCHCONTENT=ON \
#         -DFETCHCONTENT_SOURCE_DIR_LIMIT_ORDER_BOOK=<checkout>

cmake_minimum_required(VERSION 3.21...4.2)

project(limit_order_book_package_test LANGUAGES C CXX)

option(LOB_PACKAGE_TEST_FETCHCONTENT "Use FetchContent instead of find_package" OFF)
option(LOB_PACKAGE_TEST_C "Build a C consumer of limit_order_book::c" OFF)

if(LOB_PACKAGE_TEST_FETCHCONTENT)
    include(FetchContent)
    FetchContent_Declare(
        limit_order_book
        GIT_REPOSITORY https://github.com/Kautenja/limit-order-book.git
        GIT_TAG master
        GIT_SHALLOW TRUE
    )
    if(LOB_PACKAGE_TEST_C)
        set(LOB_BUILD_C_LIBRARY ON CACHE BOOL "" FORCE)
    endif()
    FetchContent_MakeAvailable(limit_order_book)
    # a FetchContent consumer gets only the targets it asks for
    set(unwanted_targets limit_order_book_tests limit_order_book_benchmarks
        limit_order_book_native)
    if(NOT LOB_PACKAGE_TEST_C)
        list(APPEND unwanted_targets limit_order_book_c)
    endif()
    foreach(target ${unwanted_targets})
        if(TARGET ${target})
            message(FATAL_ERROR "FetchContent consumers should not get ${target}")
        endif()
    endforeach()
elseif(LOB_PACKAGE_TEST_C)
    find_package(limit_order_book 3 CONFIG REQUIRED COMPONENTS c)
else()
    find_package(limit_order_book 3 CONFIG REQUIRED)
endif()

add_executable(package_test main.cpp)
target_link_libraries(package_test PRIVATE limit_order_book::limit_order_book)

enable_testing()
add_test(NAME package_test COMMAND package_test)

if(LOB_PACKAGE_TEST_C)
    add_executable(package_test_c main.c)
    target_link_libraries(package_test_c PRIVATE limit_order_book::c)
    # compile the C API header as strict C99
    set_target_properties(package_test_c PROPERTIES
        C_STANDARD 99
        C_STANDARD_REQUIRED ON
        C_EXTENSIONS OFF
    )
    target_compile_options(package_test_c PRIVATE
        "$<$<C_COMPILER_ID:MSVC>:/W3;/WX>"
        "$<$<NOT:$<C_COMPILER_ID:MSVC>>:-Wall;-Wextra;-pedantic;-Werror>"
    )
    # Windows has no rpath, so the DLL must sit next to the executable
    if(WIN32)
        add_custom_command(TARGET package_test_c POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                "$<TARGET_RUNTIME_DLLS:package_test_c>"
                "$<TARGET_FILE_DIR:package_test_c>"
            COMMAND_EXPAND_LISTS
            VERBATIM
        )
    endif()
    add_test(NAME package_test_c COMMAND package_test_c)
endif()
