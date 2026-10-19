# A consumer project that checks the limit_order_book CMake package works the
# way downstream projects use it. By default it uses find_package against an
# installed copy; with LOB_PACKAGE_TEST_FETCHCONTENT=ON it uses FetchContent.
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

project(limit_order_book_package_test LANGUAGES CXX)

option(LOB_PACKAGE_TEST_FETCHCONTENT "Use FetchContent instead of find_package" OFF)

if(LOB_PACKAGE_TEST_FETCHCONTENT)
    include(FetchContent)
    FetchContent_Declare(
        limit_order_book
        GIT_REPOSITORY https://github.com/Kautenja/limit-order-book.git
        GIT_TAG master
        GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(limit_order_book)
    # a FetchContent consumer must not build the project's development targets
    foreach(target limit_order_book_tests limit_order_book_c)
        if(TARGET ${target})
            message(FATAL_ERROR "FetchContent consumers should not get ${target}")
        endif()
    endforeach()
else()
    find_package(limit_order_book 2 CONFIG REQUIRED)
endif()

add_executable(package_test main.cpp)
target_link_libraries(package_test PRIVATE limit_order_book::limit_order_book)

enable_testing()
add_test(NAME package_test COMMAND package_test)
