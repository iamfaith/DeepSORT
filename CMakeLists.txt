cmake_minimum_required(VERSION 3.5)

project(DeepSORT LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


find_package (Eigen3 3.3 REQUIRED NO_MODULE)


# set(ONNXRUNTIME_DIR "/home/faith/onnxruntime/test_c/lib")
set(ONNXRUNTIME_DIR "/home/faith/onnxruntime/build")
# set(ONNXRUNTIME_DIR "/home/faith/DeepSORT/onnxruntime-linux-x64-1.12.1/lib_1.12.1")

# include_directories("${ONNXRUNTIME_DIR}/include"
#     )
find_library(ONNXRUNTIME_LIBRARY onnxruntime ${ONNXRUNTIME_DIR})


find_package(OpenCV 4 REQUIRED )


include_directories(
    ${OpenCV_INCLUDE_DIRS}/include
    ${CMAKE_SOURCE_DIR}/tracker/deepsort/include
    ${CMAKE_SOURCE_DIR}/tracker/bytetrack/include
    ${CMAKE_SOURCE_DIR}/detector/YOLOv5/include

    ${CMAKE_SOURCE_DIR}/onnx_runtime_include
    ${CMAKE_SOURCE_DIR}/include
    )

include_directories(
    ${CMAKE_SOURCE_DIR}/build
)


FILE(GLOB SRC_CODE ${CMAKE_SOURCE_DIR}/src/*)
message(STATUS "SRC_CODE ${SRC_CODE}")


add_executable(DeepSORT
    ${SRC_CODE}

    detector/YOLOv5/src/YOLOv5Detector.cpp

    tracker/deepsort/src/FeatureTensor.cpp
    tracker/deepsort/src/model.cpp
    tracker/deepsort/src/kalmanfilter.cpp
    tracker/deepsort/src/linear_assignment.cpp
    tracker/deepsort/src/nn_matching.cpp
    tracker/deepsort/src/track.cpp
    tracker/deepsort/src/tracker.cpp
    tracker/deepsort/src/munkres.cpp
    tracker/deepsort/src/hungarianoper.cpp

    tracker/bytetrack/src/BytekalmanFilter.cpp
    tracker/bytetrack/src/BYTETracker.cpp
    tracker/bytetrack/src/lapjv.cpp
    tracker/bytetrack/src/STrack.cpp
    tracker/bytetrack/src/utils.cpp
    
    main.cpp)
# target_link_libraries(DeepSORT PRIVATE "${ONNXRUNTIME_DIR}/libonnxruntime.so" ${OpenCV_LIBS} Eigen3::Eigen)
# opencv_dnn


target_link_libraries(DeepSORT PRIVATE ${OpenCV_LIBS} ${ONNXRUNTIME_LIBRARY} Eigen3::Eigen)

# 在 add_library 命令中指定 STATIC 关键字，例如：add_library(mylib STATIC mylib.cpp)，这样就会生成一个静态库文件，如 libmylib.a 或 libmylib.lib 。
# 在 target_link_libraries 命令中使用静态库的目标名或全路径，例如：target_link_libraries(myexe PRIVATE mylib) 或 target_link_libraries(myexe PRIVATE /path/to/libmylib.a)，这样就会将静态库链接到可执行文件中。



add_definitions(-DEMBED_MODEL)

add_executable(test_detector
    detector/YOLOv5/src/YOLOv5Detector.cpp
    test_detector.cpp

    ${SRC_CODE}
    )
target_link_libraries(test_detector PRIVATE  ${OpenCV_LIBS} ${ONNXRUNTIME_LIBRARY} Eigen3::Eigen opencv_dnn)



# tools
add_subdirectory(tools)