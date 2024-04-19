# the minimum version of CMake.
cmake_minimum_required(VERSION 3.4.1)
project(MyApplication_0202sdk)

set(NATIVERENDER_ROOT_PATH ${CMAKE_CURRENT_SOURCE_DIR})

include_directories(${NATIVERENDER_ROOT_PATH}
                    ${NATIVERENDER_ROOT_PATH}/include)
include_directories(${NATIVERENDER_ROOT_PATH}/skia)
include_directories(${NATIVERENDER_ROOT_PATH}/skia/include/core)
include_directories(${NATIVERENDER_ROOT_PATH}/skia/include/effects)
include_directories(${NATIVERENDER_ROOT_PATH}/skia/include/gpu)
include_directories(${NATIVERENDER_ROOT_PATH}/skia/include/gpu/gl)

add_library(entry SHARED
    hello.cpp
    plugin/plugin_manager.cpp
    my_xcomponent.cpp
    test_base.cpp
    test_common.cpp
    testcasefactory.cpp
#    bench/rect_bench.cpp
#    bench/aa_clip_bench.cpp
    bench/path_bench.cpp
#    bench/textblob_cache_bench.cpp
#    bench/draw_bitmap_aa_bench.cpp
#    bench/bulk_rect_bench.cpp
#    bench/canvas_save_restore.cpp
#    bench/canvas_matrix_bench.cpp
#    bench/shadow_bench.cpp
#    bench/read_pix_bench.cpp
#    bench/bezier_bench.cpp
#    bench/shader_mf_bench.cpp
#    bench/dash_line_bench.cpp
#    bench/circles_bench.cpp
#    bench/clip_strategy_bench.cpp
#    bench/nested_aa_clip_bench.cpp
#    bench/hairline_path_bench.cpp
#    bench/gradient_bench.cpp
#    bench/xfermode_bench.cpp
#    bench/random_path_bench.cpp
#    bench/blur_bench.cpp
#    bench/path_create_bench.cpp
#    bench/skbench_addpathtest.cpp
#    bench/clear_bench.cpp
#    bench/big_path_bench.cpp
#    dm/aa_rect_modes.cpp
#    dm/blur_circles_gm.cpp
#    dm/blur_large_rrects.cpp
#    dm/add_arc.cpp
#    dm/font_regen.cpp
#    dm/alpha_image.cpp
#    dm/shadowutils.cpp
#    dm/clipped_cubic.cpp
#    dm/circular_arcs.cpp
#    dm/largeclippedpath.cpp
#    dm/filltypes.cpp
#    dm/dashing.cpp
#    dm/path_interior.cpp
#    dm/strokes.cpp
#    dm/alpha_gradients.cpp
#    dm/convex_paths.cpp
#    dm/gradient_dirty_laundry.cpp
#    dm/fill_types_persp.cpp
#    dm/arcofzorro.cpp
#    dm/stroke_rect_shader.cpp
#    dm/luma_filter.cpp
#    dm/bugfix7792.cpp
#    dm/points_mask_filter.cpp
#    dm/clip_cubic_gm.cpp
#    dm/points.cpp
#    interface/canvas_test.cpp
#    dm/conic_paths.cpp
)

#add_library(myxnode SHARED
#    my_xnode.cpp
#    test_base.cpp
#    testcasefactory.cpp
#    test_common.cpp
#    bench/rect_bench.cpp
#    bench/aa_clip_bench.cpp
#    bench/path_bench.cpp
#    bench/textblob_cache_bench.cpp
#    bench/draw_bitmap_aa_bench.cpp
#    bench/bulk_rect_bench.cpp
#    bench/canvas_save_restore.cpp
#    bench/canvas_matrix_bench.cpp
#    bench/shadow_bench.cpp
#    bench/read_pix_bench.cpp
#    bench/bezier_bench.cpp
#    bench/shader_mf_bench.cpp
#    bench/dash_line_bench.cpp
#    bench/circles_bench.cpp
#    bench/clip_strategy_bench.cpp
#    bench/nested_aa_clip_bench.cpp
#    bench/hairline_path_bench.cpp
#    bench/gradient_bench.cpp
#    bench/xfermode_bench.cpp
#    bench/random_path_bench.cpp
#    bench/blur_bench.cpp
#    bench/path_create_bench.cpp
#    bench/skbench_addpathtest.cpp
#    bench/clear_bench.cpp
#    bench/big_path_bench.cpp
#    dm/aa_rect_modes.cpp
#    dm/blur_circles_gm.cpp
#    dm/blur_large_rrects.cpp
#    dm/add_arc.cpp
#    dm/font_regen.cpp
#    dm/alpha_image.cpp
#    dm/shadowutils.cpp
#    dm/clipped_cubic.cpp
#    dm/circular_arcs.cpp
#    dm/largeclippedpath.cpp
#    dm/filltypes.cpp
#    dm/dashing.cpp
#    dm/path_interior.cpp
#    dm/strokes.cpp
#    dm/alpha_gradients.cpp
#    dm/convex_paths.cpp
#    dm/gradient_dirty_laundry.cpp
#    dm/fill_types_persp.cpp
#    dm/arcofzorro.cpp
#    dm/stroke_rect_shader.cpp
#    dm/luma_filter.cpp
#    dm/bugfix7792.cpp
#    dm/points_mask_filter.cpp
#    dm/clip_cubic_gm.cpp
#    dm/points.cpp
#    interface/canvas_test.cpp
#    dm/conic_paths.cpp
#)

find_library(
    # Sets the name of the path variable.
    hilog-lib
    # Specifies the name of the NDK library that
    # you want CMake to locate.
    hilog_ndk.z
)

add_definitions(-std=c++17)
add_definitions(-Wno-nonportable-include-path)
add_definitions(-Wno-unused-command-line-argument)

add_definitions(-DGR_TEST_UTILS=1)
add_definitions(-DSK_ENABLE_SKSL)
add_definitions(-DSK_ENABLE_SMALL_PAGE)
add_definitions(-DSK_ALLOW_STATIC_GLOBAL_INITIALIZERS=1)
add_definitions(-DSK_GL)
add_definitions(-DSK_BUILD_FONT_MGR_FOR_OHOS)

target_link_libraries(entry PUBLIC ${hilog-lib})
target_link_libraries(entry PUBLIC libace_napi.z.so)
target_link_libraries(entry PUBLIC libace_ndk.z.so)
target_link_libraries(entry PUBLIC libnative_window.so)
#target_link_libraries(entry PUBLIC libnative_drawing.so)
target_link_libraries(entry PUBLIC libimage_packer_ndk.z.so)
target_link_libraries(entry PUBLIC libpixelmap_ndk.z.so)
target_link_libraries(entry PUBLIC ${NATIVERENDER_ROOT_PATH}/../../../libs/armeabi-v7a/libskia_canvaskit.z.so)
target_link_libraries(entry PUBLIC ${NATIVERENDER_ROOT_PATH}/../../../libs/armeabi-v7a/libbegetutil.z.so)
target_link_libraries(entry PUBLIC ${NATIVERENDER_ROOT_PATH}/../../../libs/armeabi-v7a/libhitrace_meter.so)
target_link_libraries(entry PUBLIC ${NATIVERENDER_ROOT_PATH}/../../../libs/armeabi-v7a/libsec_shared.z.so)
target_link_libraries(entry PUBLIC ${NATIVERENDER_ROOT_PATH}/../../../libs/armeabi-v7a/libc++.so)
target_link_libraries(entry PUBLIC ${NATIVERENDER_ROOT_PATH}/../../../libs/armeabi-v7a/libhitracechain.so)

#target_link_libraries(myxnode PUBLIC ${hilog-lib})
#target_link_libraries(myxnode PUBLIC libace_napi.z.so)
#target_link_libraries(myxnode PUBLIC libace_ndk.z.so)
#target_link_libraries(myxnode PUBLIC libnative_window.so)
#target_link_libraries(myxnode PUBLIC libnative_drawing.so)
#target_link_libraries(myxnode PUBLIC libimage_packer_ndk.z.so)
#target_link_libraries(myxnode PUBLIC libpixelmap_ndk.z.so)