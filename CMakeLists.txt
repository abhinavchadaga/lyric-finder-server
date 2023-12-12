cmake_minimum_required(VERSION 3.22)
project(server LANGUAGES CXX CUDA)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CUDA_STANDARD 17)

include_directories(include)

add_executable(server main.cpp
        src/ISearchEngine.cpp
        src/song.cpp
        src/search_kernel.cu)

set_target_properties(server PROPERTIES
        CUDA_SEPARABLE_COMPILATION ON)