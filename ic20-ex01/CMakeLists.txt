cmake_minimum_required(VERSION 3.17)
project(ic20_ex01)

set(CMAKE_CXX_STANDARD 14)

include_directories(src)

add_executable(ic20_ex01
        src/alloc.c
        src/alloc.h
        src/bfio.c
        src/bfio.h
        src/defines.h
        src/heap.c
        src/heap.h
        src/ic20_huffman.c
        src/image_io.c
        src/image_io.h
        Makefile)
