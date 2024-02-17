# the minimum version of CMake.
cmake_minimum_required(VERSION 3.4.1)
project(MyApplication_0202sdk)

set(NATIVERENDER_ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR})

include_directories(${NATIVERENDER_ROOT_PATH}
                    ${NATIVERENDER_ROOT_PATH}/include)

add_library(entry SHARED
    hello.cpp
    samples/drawing_demo.cpp
    plugin/plugin_manager.cpp
    samples/test_base.cpp
    samples/draw_rect_test.cpp
    samples/draw_path_test.cpp
)
find_library(
    # Sets the name of the path variable.
    hilog-lib
    # Specifies the name of the NDK library that
    # you want CMake to locate.
    hilog_ndk.z
)
target_link_libraries(entry PUBLIC ${hilog-lib})
target_link_libraries(entry PUBLIC libace_napi.z.so)
target_link_libraries(entry PUBLIC libace_ndk.z.so)
target_link_libraries(entry PUBLIC libnative_window.so)
target_link_libraries(entry PUBLIC libnative_drawing.so)
target_link_libraries(entry PUBLIC libimage_packer_ndk.z.so)
target_link_libraries(entry PUBLIC libpixelmap_ndk.z.so)