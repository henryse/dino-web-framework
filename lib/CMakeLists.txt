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
project(DinoLibrary)

set(DINO_LIB_VERSION "1.0.0")

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(CMAKE_C_FLAGS "-std=gnu99")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(CMAKE_C_FLAGS "-std=gnu99 -pthread -luuid")
endif()

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
        dino_string_buffer.c
        dino_string_buffer.h)

add_library(DinoLibrary STATIC ${SOURCE_FILES})

install(TARGETS DinoLibrary DESTINATION lib)
install(FILES dino.h dino_http_error.h DESTINATION include)

target_include_directories (DinoLibrary PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})