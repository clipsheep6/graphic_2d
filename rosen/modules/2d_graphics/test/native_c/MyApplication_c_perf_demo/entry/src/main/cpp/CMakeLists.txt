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
    testcase/rect_bench.cpp
    testcase/aa_clip_bench.cpp
    testcase/test_common.cpp
    testcase/test_base.cpp
    testcase/path_bench.cpp
    testcase/textblob_cache_bench.cpp
    testcase/draw_bitmap_aa_bench.cpp
    testcase/bulk_rect_bench.cpp
    testcase/canvas_save_restore.cpp
    testcase/canvas_matrix_bench.cpp
    testcase/shadow_bench.cpp
    testcase/read_pix_bench.cpp
    testcase/bezier_bench.cpp
    testcase/shader_mf_bench.cpp
    testcase/dash_line_bench.cpp
    testcase/circles_bench.cpp
    testcase/clip_strategy_bench.cpp
    testcase/nested_aa_clip_bench.cpp
    testcase/hairline_path_bench.cpp
    testcase/gradient_bench.cpp
    testcase/xfermode_bench.cpp
    testcase/random_path_bench.cpp
    testcase/blur_bench.cpp
    testcase/path_create_bench.cpp
    testcase/skbench_addpathtest.cpp
    testcase/clear_bench.cpp
)

add_library(myxnode SHARED
    my_xnode.cpp
    testcasefactory.cpp
    testcase/rect_bench.cpp
    testcase/aa_clip_bench.cpp
    testcase/test_common.cpp
    testcase/test_base.cpp
    testcase/path_bench.cpp
    testcase/textblob_cache_bench.cpp
    testcase/draw_bitmap_aa_bench.cpp
    testcase/bulk_rect_bench.cpp
    testcase/canvas_save_restore.cpp
    testcase/canvas_matrix_bench.cpp
    testcase/shadow_bench.cpp
    testcase/read_pix_bench.cpp
    testcase/bezier_bench.cpp
    testcase/shader_mf_bench.cpp
    testcase/dash_line_bench.cpp
    testcase/circles_bench.cpp
    testcase/clip_strategy_bench.cpp
    testcase/nested_aa_clip_bench.cpp
    testcase/hairline_path_bench.cpp
    testcase/gradient_bench.cpp
    testcase/xfermode_bench.cpp
    testcase/random_path_bench.cpp
    testcase/blur_bench.cpp
    testcase/path_create_bench.cpp
    testcase/skbench_addpathtest.cpp
    testcase/clear_bench.cpp
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