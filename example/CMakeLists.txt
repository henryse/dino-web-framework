cmake_minimum_required(VERSION 3.2)
project(DinoExample)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c99")

set(SOURCE_FILES
    main.c)

add_executable(DinoExample ${SOURCE_FILES})
target_link_libraries (DinoExample LINK_PUBLIC DinoLibrary)