#**********************************************************************
#    Copyright (c) 2014 Henry Seurer
#
#    Permission is hereby granted, free of charge, to any person
#    obtaining a copy of this software and associated documentation
#    files (the "Software"), to deal in the Software without
#    restriction, including without limitation the rights to use,
#    copy, modify, merge, publish, distribute, sublicense, and/or sell
#    copies of the Software, and to permit persons to whom the
#    Software is furnished to do so, subject to the following
#    conditions:
#
#    The above copyright notice and this permission notice shall be
#    included in all copies or substantial portions of the Software.
#
#    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
#    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
#    OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
#    NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
#    HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
#    WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
#    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
#    OTHER DEALINGS IN THE SOFTWARE.
#
#**********************************************************************

cmake_minimum_required(VERSION 3.2)
project(dino-library)

find_package (Threads)

set(DINO_LIB_VERSION "dino-server-1.1.0")

# Check to see if we are running on a Raspberry PI
#
if(EXISTS "/opt/vc/include/bcm_host.h")
    MESSAGE("-- bcm_host.h found, we must be on a Raspberry PI")
    set(BCMHOST found)
else()
    MESSAGE("-- bcm_host.h not found we must NOT on in a Raspberry PI")
endif()

# Set The correct compiler
#
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(CMAKE_C_FLAGS "-std=gnu99")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    if(DEFINED BCMHOST)
        set(CMAKE_C_FLAGS "-std=gnu99 -lwiringPi")
    else()
        set(CMAKE_C_FLAGS "-std=gnu99")
    endif()
endif ()

# Versioning
#
configure_file(version_config.h.in ${CMAKE_BINARY_DIR}/generated/version_config.h)
include_directories(${CMAKE_BINARY_DIR}/generated/)

set(SOURCE_FILES
        dino.c
        dino.h
        dino_http.c
        dino_http.h
        dino_strmap.c
        dino_strmap.h
        dino_utils.h
        dino_utils.c
        dino_http_error.h
        dino_string.c
        dino_string.h
        dino_intmap.c
        dino_intmap.h
        dino_template_generator.c
        dino_template_generator.h
        dino_debug.c
        dino_debug.h)

add_library(dino-library STATIC ${SOURCE_FILES})

install(TARGETS dino-library DESTINATION lib)
install(FILES dino.h dino_http_error.h DESTINATION include)

target_include_directories(dino-library PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})