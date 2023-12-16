cmake_minimum_required(VERSION 3.22)
project(server LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(oatpp 1.3.0 REQUIRED)

# Enable CUDA if found
include(CheckLanguage)
check_language(CUDA)

if (CMAKE_CUDA_COMPILER)
    message("using CUDA")
    enable_language(CUDA)
    set(CMAKE_CUDA_STANDARD 20)
    add_definitions(-DUSE_CUDA)
else ()
    message("CUDA not found")
endif ()

# headers
include_directories(include)
include_directories(third-party)

set(SOURCE_FILES src/server.cpp
        third-party/easylogging++.cc
        src/ISearchEngine.cpp
        src/search_engine_cpu.cpp
        src/song.cpp)
if (CMAKE_CUDA_COMPILER)
    list(APPEND SOURCE_FILES src/query_kernel.cu src/search_engine_gpu.cpp)
endif ()

add_executable(server ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} oatpp::oatpp)

# conditionally set cuda properties
if (CMAKE_CUDA_COMPILER)
    set_target_properties(server PROPERTIES CUDA_SEPARABLE_COMPILATION ON)
endif ()