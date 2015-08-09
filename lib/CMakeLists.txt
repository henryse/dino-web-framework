cmake_minimum_required(VERSION 3.2)
project(DinoLibrary)

set(DINO_LIB_VERSION "1.0.0")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c99")

set(SOURCE_FILES
    dino.c
    dino.h
    dino_buffer.c
    dino_buffer.h
    dino_http.c
    dino_http.h
    dino_strmap.c
    dino_strmap.h
    dino_utils.h
    dino_utils.c
    dino_http_error.h)

add_library(DinoLibrary STATIC ${SOURCE_FILES})

install(TARGETS DinoLibrary DESTINATION lib)
install(FILES dino.h dino_http_error.h DESTINATION include)