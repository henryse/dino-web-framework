cmake_minimum_required(VERSION 3.2)
project(Dino)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
    src/dino.c
    src/dino.h
    src/dino_buffer.c
    src/dino_buffer.h
    src/dino_http.c
    src/dino_http.h
    src/dino_strmap.c
    src/dino_strmap.h
    src/dino_utils.h
    src/dino_utils.c
    src/http_error.h
    src/main.c)

add_executable(Dino ${SOURCE_FILES})