#
#              Copyright (c) 2025 Gennaro Prota
#
# Distributed under the Boost Software License, Version 1.0.
#    (See accompanying file LICENSE_1_0.txt or copy at
#          https://www.boost.org/LICENSE_1_0.txt)
#
# Official repository: https://github.com/cppalliance/handlebars
#

cmake_minimum_required(VERSION 3.11)

project(boost_handlebars LANGUAGES CXX)

# Module name
set(BOOST_HANDLEBARS_MODULE_NAME "handlebars")

# Options
option(BOOST_HANDLEBARS_BUILD_TESTS "Build tests" ON)

# Default build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# Shared compiler settings
set(BOOST_HANDLEBARS_CXX_STANDARD 23)
set(BOOST_HANDLEBARS_COMPILE_DEFS
    -DBOOST_HANDLEBARS_STATIC_LINK
)

if(WIN32)
    list(APPEND BOOST_HANDLEBARS_COMPILE_DEFS
        -D_WIN32_WINNT=0x0601
        -D_CRT_SECURE_NO_WARNINGS
    )
endif()

# Shared include paths
set(BOOST_HANDLEBARS_INCLUDE_DIRS
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Shared MSVC flags
if(MSVC)
    set(BOOST_HANDLEBARS_MSVC_FLAGS
        /permissive-
        /Zc:preprocessor
        /W4
        /MP
        /EHs
        $<$<CONFIG:Debug>:/Oy->
    )
endif()

# --------------------------------------------------
# Library target
# --------------------------------------------------

file(GLOB_RECURSE BOOST_HANDLEBARS_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    source_file_names.cpp
)

add_library(boost_handlebars ${BOOST_HANDLEBARS_SOURCES})
target_compile_features(boost_handlebars PUBLIC cxx_std_${BOOST_HANDLEBARS_CXX_STANDARD})
target_compile_definitions(boost_handlebars PRIVATE ${BOOST_HANDLEBARS_COMPILE_DEFS})
target_include_directories(boost_handlebars PUBLIC
    ${BOOST_HANDLEBARS_INCLUDE_DIRS}
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)
if(MSVC)
    target_compile_options(boost_handlebars PUBLIC ${BOOST_HANDLEBARS_MSVC_FLAGS})
endif()

# Install rules
include(GNUInstallDirs)

install(TARGETS boost_handlebars
    EXPORT boost_handlebars_targets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(DIRECTORY boost/handlebars
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/boost
)

# --------------------------------------------------
# Unit tests
# --------------------------------------------------
enable_testing()

if(BOOST_HANDLEBARS_BUILD_TESTS)
    include(CTest)

    find_package(Boost REQUIRED COMPONENTS container)

    file(GLOB_RECURSE BOOST_HANDLEBARS_TEST_SOURCES CONFIGURE_DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/test/*.cpp
    )

    add_executable(handlebars-test ${BOOST_HANDLEBARS_TEST_SOURCES})
    target_compile_features(handlebars-test PUBLIC cxx_std_${BOOST_HANDLEBARS_CXX_STANDARD})
    target_link_libraries(handlebars-test PRIVATE
        Boost::container
        boost_handlebars)
    target_include_directories(handlebars-test PRIVATE
        ${BOOST_HANDLEBARS_INCLUDE_DIRS}
        ${CMAKE_CURRENT_SOURCE_DIR}
    )
    target_compile_definitions(handlebars-test PRIVATE
        ${BOOST_HANDLEBARS_COMPILE_DEFS}
        -DBOOST_HANDLEBARS_TEST_FILES_DIR="${CMAKE_CURRENT_SOURCE_DIR}/test/test-files"
    )
    if(MSVC)
        target_compile_options(handlebars-test PRIVATE ${BOOST_HANDLEBARS_MSVC_FLAGS})
        target_compile_options(handlebars-test PRIVATE /bigobj)
    endif()

    add_test(NAME handlebars-unit-tests COMMAND handlebars-test)
endif()

