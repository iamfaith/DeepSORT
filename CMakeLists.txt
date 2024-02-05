cmake_minimum_required(VERSION 3.5)

project(DeepSORT LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

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





set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN 1)
# set(CMAKE_VERBOSE_MAKEFILE on) # compile full command
# set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # 



# 设置可执行文件的属性，使其可以打包动态库
set_target_properties(DeepSORT PROPERTIES
    # 设置可执行文件的安装路径
    INSTALL_RPATH "\$ORIGIN"
    # 设置可执行文件的链接标志，使其可以查找动态库
    LINK_FLAGS "-Wl,-rpath,\\\$ORIGIN"
)

# make install
# 设置安装的目标目录
set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/build)

# 安装可执行文件到 bin 目录
install(TARGETS DeepSORT RUNTIME DESTINATION bin)

add_custom_command(TARGET ${PROJECT_NAME}
                   POST_BUILD
                   COMMAND chmod +x ${CMAKE_SOURCE_DIR}/copylib.sh
                   COMMAND rm -rf ${CMAKE_BINARY_DIR}/bin
                   COMMAND mkdir -p ${CMAKE_BINARY_DIR}/bin
                   COMMAND ${CMAKE_SOURCE_DIR}/copylib.sh DeepSORT bin
                   COMMAND cd ${CMAKE_BINARY_DIR}/bin && rm -rf libc.so.* libgcc_s.so.* libpthread.so.* librt.so.*
                   COMMENT "Copying dependency libraries..."
                   COMMAND cp ${CMAKE_SOURCE_DIR}/coco_80_labels_list.txt ${CMAKE_BINARY_DIR}/bin
                   )

add_definitions(-DEMBED_MODEL)


add_executable(test_detector
    detector/YOLOv5/src/YOLOv5Detector.cpp
    test_detector.cpp

    ${SRC_CODE}
    )
target_link_libraries(test_detector PRIVATE  ${OpenCV_LIBS} ${ONNXRUNTIME_LIBRARY} Eigen3::Eigen)
install(TARGETS test_detector RUNTIME DESTINATION bin)


# tools
add_subdirectory(tools)


############################################# old
# file(GLOB ONNXRUNTIME_SO "${ONNXRUNTIME_DIR}/libonnxruntime.so*")
# MACRO(COPY_SO)
#         ADD_CUSTOM_COMMAND (
#         TARGET ${PROJECT_NAME}
#         POST_BUILD
#         COMMAND ${CMAKE_COMMAND} ARGS -E copy "/usr/lib/x86_64-linux-gnu/libopencv*.so*" ${CMAKE_CURRENT_BINARY_DIR}/bin
#         COMMAND ${CMAKE_COMMAND} ARGS -E copy "/lib/x86_64-linux-gnu/libheim*.so*" ${CMAKE_CURRENT_BINARY_DIR}/bin
#         COMMAND ${CMAKE_COMMAND} ARGS -E copy "/lib/x86_64-linux-gnu/libgtk*.so*" ${CMAKE_CURRENT_BINARY_DIR}/bin
#     )
# ENDMACRO(COPY_SO)
# COPY_SO()

# file(COPY ${ONNXRUNTIME_SO} DESTINATION ${CMAKE_BINARY_DIR}/bin)
# 安装动态库到 lib 目录
# install(TARGETS ${ONNXRUNTIME_SO} LIBRARY DESTINATION bin)


# file(COPY /usr/local/lib/libEigen3.so DESTINATION ${CMAKE_BINARY_DIR}/bin)
# 假设 opencv 和 Eigen3 的 so 文件在 /usr/local/lib 目录下
# add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
#                    COMMAND ${CMAKE_COMMAND} -E copy /usr/local/lib/libopencv_core.so ${CMAKE_BINARY_DIR}/bin
#                    COMMAND ${CMAKE_COMMAND} -E copy /usr/local/lib/libEigen3.so ${CMAKE_BINARY_DIR}/bin)


# 假设 opencv 和 Eigen3 的库文件已经通过 find_package 或者 add_subdirectory 导入
# install(TARGETS ${OpenCV_LIBS} 
#         RUNTIME DESTINATION ${CMAKE_BINARY_DIR}/bin
#         LIBRARY DESTINATION ${CMAKE_BINARY_DIR}/bin
#         ARCHIVE DESTINATION ${CMAKE_BINARY_DIR}/bin)



# message(STATUS "OpenCV library: ${OpenCV_INSTALL_PATH}")
# message(STATUS "    version: ${OpenCV_VERSION}")
# message(STATUS "    libraries: ${OpenCV_LIBS}")
# message(STATUS "    libraries: ${OpenCV_LIBRARIES}")
# message(STATUS "    include path: ${OpenCV_INCLUDE_DIRS}")