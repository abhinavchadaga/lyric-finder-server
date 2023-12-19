cmake_minimum_required(VERSION 3.22)
project(server LANGUAGES CXX CUDA)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CUDA_STANDARD 14)
set(CMAKE_CUDA_STANDARD_REQUIRED ON)

find_package(oatpp 1.3.0 REQUIRED)

# headers
include_directories(include)
include_directories(third-party)
include_directories(${CMAKE_CUDA_TOOLKIT_INCLUDE_DIRECTORIES})

set(SOURCE_FILES src/app.cpp
        third-party/easylogging++.cc
        src/engine/ISearchEngine.cpp
        src/engine/search_engine_cpu.cpp
        src/engine/song.cpp
        src/engine/search_engine_gpu.cpp
        src/engine/query_kernel.cu
        include/dto/song.hpp
        include/controller/controller.hpp
        include/dto/lyric.hpp
        include/dto/song_list.hpp)

add_executable(server ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} PRIVATE ${CMAKE_CUDA_STANDARD_LIBRARIES} oatpp::oatpp)
set_target_properties(server PROPERTIES CUDA_SEPARABLE_COMPILATION ON)