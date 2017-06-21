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
project(dino-test)

find_package (Threads)


# Check to see if we are running on a Raspberry PI
#
if(EXISTS "/opt/vc/include/bcm_host.h")
    MESSAGE("-- bcm_host.h found, we must be on a Raspberry PI")
    set(BCMHOST found)
else()
    MESSAGE("-- bcm_host.h not found we must NOT be on a Raspberry PI")
endif()

set(CMAKE_C_FLAGS "-std=gnu99")

set(SOURCE_FILES
    main.c)

add_executable(dino-test ${SOURCE_FILES})

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    target_link_libraries (dino-test LINK_PUBLIC dino-library ${CMAKE_THREAD_LIBS_INIT})
else()
    if(DEFINED BCMHOST)
        target_link_libraries (dino-test LINK_PUBLIC dino-library ${CMAKE_THREAD_LIBS_INIT} rt wiringPi)
    else()
        target_link_libraries (dino-test LINK_PUBLIC dino-library ${CMAKE_THREAD_LIBS_INIT} rt)
    endif()
endif()
