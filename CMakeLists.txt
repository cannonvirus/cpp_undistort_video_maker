cmake_minimum_required(VERSION 3.5)

project(cpp_undistort_video)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wall")
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

find_package(vpi 1.0 REQUIRED)
find_package(OpenCV REQUIRED)
if (OpenCV_FOUND)
    message(STATUS "OpenCV library status:")
    message(STATUS "    version: ${OpenCV_VERSION}")
    message(STATUS "    include path: ${OpenCV_INCLUDE_DIRS}")
else ()
    message(FATAL_ERROR "Could not locate OpenCV")
endif()

find_package(RapidJSON)

include_directories(${PROJECT_SOURCE_DIR}/include)
file(GLOB SOURCE_FILES src/*.cpp)

# add_executable(${PROJECT_NAME} main.cpp)
add_executable(${PROJECT_NAME} main.cpp ${SOURCE_FILES})

target_link_libraries(
    ${PROJECT_NAME} 
    ${OpenCV_LIBS}
    vpi
    )
