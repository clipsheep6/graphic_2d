# the minimum version of CMake.
cmake_minimum_required(VERSION 3.4.1)
project(MyApplication_0202sdk)

set(NATIVERENDER_ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR})

include_directories(${NATIVERENDER_ROOT_PATH}
                    ${NATIVERENDER_ROOT_PATH}/include)

add_library(entry SHARED
    hello.cpp
    plugin/plugin_manager.cpp
    my_xcomponent.cpp
    testcasefactory.cpp
    testcase/test_common.cpp
    testcase/test_base.cpp
    testcase/draw_rect.cpp
    testcase/draw_path.cpp
    testcase/draw_textblob.cpp
    testcase/draw_bitmap.cpp
    testcase/draw_bitmaprect.cpp
    testcase/canvas_save_restore.cpp
    testcase/canvas_save_layer.cpp
    testcase/draw_line.cpp
    testcase/draw_path_arc_to.cpp
    testcase/draw_path_line_to.cpp
)

add_library(myxnode SHARED
    my_xnode.cpp
    testcasefactory.cpp
    testcase/test_common.cpp
    testcase/test_base.cpp
    testcase/draw_rect.cpp
    testcase/draw_path.cpp
    testcase/draw_textblob.cpp
    testcase/draw_bitmap.cpp
    testcase/draw_bitmaprect.cpp
    testcase/canvas_save_restore.cpp
    testcase/canvas_save_layer.cpp
    testcase/draw_line.cpp
    testcase/draw_path_arc_to.cpp
    testcase/draw_path_line_to.cpp
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

target_link_libraries(myxnode PUBLIC ${hilog-lib})
target_link_libraries(myxnode PUBLIC libace_napi.z.so)
target_link_libraries(myxnode PUBLIC libace_ndk.z.so)
target_link_libraries(myxnode PUBLIC libnative_window.so)
target_link_libraries(myxnode PUBLIC libnative_drawing.so)
target_link_libraries(myxnode PUBLIC libimage_packer_ndk.z.so)
target_link_libraries(myxnode PUBLIC libpixelmap_ndk.z.so)