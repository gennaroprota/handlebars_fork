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
option(BOOST_HANDLEBARS_ENABLE_TESTS "Enable tests" ON)

# Default build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# Collect source files
file(GLOB_RECURSE BOOST_HANDLEBARS_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    source_file_names.cpp
)

# Create library target
add_library(boost_handlebars ${BOOST_HANDLEBARS_SOURCES})

# C++ standard
target_compile_features(boost_handlebars PUBLIC cxx_std_23)

# Include directories
target_include_directories(boost_handlebars
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# Shared build macro
target_compile_definitions(boost_handlebars
    PRIVATE
    -DBOOST_HANDLEBARS_STATIC_LINK
)

if (WIN32)
    target_compile_definitions(
        boost_handlebars
        PUBLIC
        -D_WIN32_WINNT=0x0601
        -D_CRT_SECURE_NO_WARNINGS
    )

    if(MSVC)
        target_compile_options(
            boost_handlebars
            PUBLIC
            /permissive-     # strict C++
            /Zc:preprocessor # new preprocessor
            /W4              # enable all warnings
            /MP              # multi-processor compilation
            /EHs             # C++ exception handling
            $<$<CONFIG:Debug>:/Oy-> # disable frame pointer omission
        )
    endif()
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

# Tests
enable_testing()
add_subdirectory(test)
