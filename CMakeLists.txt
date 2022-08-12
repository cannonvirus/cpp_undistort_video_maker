cmake_minimum_required(VERSION 3.5)

project(video_maker)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wall")
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
# set(CUDA_TOOLKIT_ROOT_DIR "/usr/local/cuda-10.2")

find_package(CUDA REQUIRED)
if (CUDA_FOUND)
    message(STATUS "CUDA library status:")
    message(STATUS "    version: ${CUDA_VERSION}")
    message(STATUS "    include path: ${CUDA_INCLUDE_DIRS}")
else ()
    message(FATAL_ERROR "Could not locate CUDA")
endif()

# find_package(vpi1 REQUIRED)
# if (vpi_FOUND)
#     message(STATUS "VPI library status:")
#     message(STATUS "    version: ${vpi_VERSION}")
#     message(STATUS "    include path: ${vpi_INCLUDE_DIRS}")
# else ()
#     message(FATAL_ERROR "Could not locate Vpi")
# endif()

find_package(RapidJSON)
find_package(OpenCV REQUIRED)
if (OpenCV_FOUND)
    message(STATUS "OpenCV library status:")
    message(STATUS "    version: ${OpenCV_VERSION}")
    message(STATUS "    include path: ${OpenCV_INCLUDE_DIRS}")
else ()
    message(FATAL_ERROR "Could not locate OpenCV")
endif()

include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(/opt/nvidia/vpi1/include)
file(GLOB SOURCE_FILES src/*.cpp)

# add_executable(${PROJECT_NAME} main.cpp)
add_executable(${PROJECT_NAME} main.cpp ${SOURCE_FILES})

target_link_libraries(
    ${PROJECT_NAME} 
    ${OpenCV_LIBS}
    /opt/nvidia/vpi1/lib64/libnvvpi.so
    cuda
    )

# target_link_libraries(
#     ${PROJECT_NAME} 
#     ${OpenCV_LIBS}
#     vpi
#     cuda
#     )
