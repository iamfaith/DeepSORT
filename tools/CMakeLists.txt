# $ cmake --version
cmake_minimum_required(VERSION 3.5)

# Set the project name
project (onnx2mem LANGUAGES CXX)

message(STATUS "CMAKE_CURRENT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}")

# set(CMAKE_CXX_FLAGS_RELEASE "-O3")

set_source_files_properties(${PROJECT_NAME}.cpp PROPERTIES LANGUAGE CXX )

FILE(GLOB SRC_CODE ${CMAKE_SOURCE_DIR}/src/*)

# Add an executable
add_executable(${PROJECT_NAME} ${SRC_CODE} ${PROJECT_NAME}.cpp)

# find_package(ZLIB)

target_link_libraries( ${PROJECT_NAME}
    PRIVATE 
    ${ONNXRUNTIME_LIBRARY} 
    # ZLIB::ZLIB
)

include_directories(
    ${CMAKE_SOURCE_DIR}/build
    ${CMAKE_SOURCE_DIR}/include
)

add_executable(test_mem ${SRC_CODE} test_mem.cpp)
target_link_libraries( test_mem
    PRIVATE 
    ${ONNXRUNTIME_LIBRARY}
)

