cmake_minimum_required(VERSION 3.22)
project(lyric-finder-server LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# install oat++
include(FetchContent)
FetchContent_Declare(
    oatpp
    GIT_REPOSITORY https://github.com/oatpp/oatpp.git
    GIT_TAG 1.3.0
)
FetchContent_GetProperties(oatpp)

if(NOT oatpp_POPULATED)
    FetchContent_Populate(oatpp)
    add_subdirectory(${oatpp_SOURCE_DIR} ${oatpp_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()

set(OATPP_BUILD_TESTS OFF CACHE INTERNAL "")

# find CUDA
find_package(CUDAToolkit)

if(CUDAToolkit_FOUND)
    enable_language(CUDA)
    message(STATUS "CUDA found, compiling with GPU support")
    set(CMAKE_CUDA_STANDARD 14)
    set(CMAKE_CUDA_STANDARD_REQUIRED ON)
    add_definitions(-DUSE_CUDA)
else()
    message(STATUS "CUDA not found, compiling without GPU support")
endif()

set(SOURCE_FILES src/app.cpp
    third-party/easylogging++.cc
    src/engine/ISearchEngine.cpp
    src/engine/search_engine_cpu.cpp
    src/engine/song.cpp
    src/engine/cache.cpp
)

if(CUDAToolkit_FOUND)
    list(APPEND SOURCE_FILES
        src/engine/search_engine_gpu.cpp
        src/engine/query_kernel.cu)
endif()

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

# add the include and third_party directories
target_include_directories(${PROJECT_NAME} PRIVATE include third-party)

if(CUDAToolkit_FOUND)
    target_include_directories(${PROJECT_NAME} PRIVATE ${CUDAToolkit_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME} PRIVATE ${CUDAToolkit_LIBRARY_DIRS})
    set_target_properties(${PROJECT_NAME} PROPERTIES CUDA_SEPARABLE_COMPILATION ON)
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE oatpp)