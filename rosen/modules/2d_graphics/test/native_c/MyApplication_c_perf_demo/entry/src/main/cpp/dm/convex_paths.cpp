
#include "convex_paths.h"
#include <native_drawing/drawing_bitmap.h>
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_canvas.h>
#include <native_drawing/drawing_point.h>
#include <native_drawing/drawing_matrix.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_pen.h>
#include <native_drawing/drawing_mask_filter.h>
#include <native_drawing/drawing_filter.h>
#include <native_drawing/drawing_round_rect.h>
#include <native_drawing/drawing_brush.h>
#include <native_drawing/drawing_shader_effect.h>
#include <vector>
#include "common/log_common.h"
#include "test_common.h"

constexpr static float kRadius = 50.f;
constexpr static int kLength = 100;
constexpr static int kPtsPerSide = (1 << 12);

std::vector<OH_Drawing_Path *> fPaths;

void ConvexPaths::OnTestFunction(OH_Drawing_Canvas *canvas) {
    DRAWING_LOGI("ConvexPaths::OnTestFunction: 6657upup");
    OH_Drawing_Pen *pen = OH_Drawing_PenCreate();
    OH_Drawing_PenSetColor(pen, 0xFF000000);
    OH_Drawing_PenSetAntiAlias(pen, true);
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    DRAWING_LOGD("ConvexPaths MakePath: 6657upup");
    MakePath(path);

    TestRend rand;
    DRAWING_LOGE("ConvexPaths TestRend: 6657upup");
    OH_Drawing_CanvasTranslate(canvas, 20, 20);
    
    // As we've added more paths this has gotten pretty big. Scale the whole thing down.
    OH_Drawing_CanvasScale(canvas, 2.0f/3, 2.0f/3); 

    for(int i = 0; i < fPaths.size(); ++i) {
        OH_Drawing_CanvasSave(canvas);
        // position the path, and make it at off-integer coords.
        OH_Drawing_CanvasTranslate(canvas, 200.0f * (i % 5) + 1.0f/10, 200.0f * (i / 5) + 9.0f/10);
        uint32_t color = rand.nextU();
        color |= 0xff000000;
        OH_Drawing_PenSetColor(pen, color);
        OH_Drawing_CanvasAttachPen(canvas, pen);
        
        OH_Drawing_CanvasDrawPath(canvas, fPaths[i]);
        DRAWING_LOGE("ConvexPaths zhangjing: [%{public}p]", fPaths[i]);
        OH_Drawing_CanvasRestore(canvas);
    }
    DRAWING_LOGW("ConvexPaths overfor: 6657upup");
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_PenDestroy(pen);
    for (int f = 0; f < fPaths.size(); ++f) {
        OH_Drawing_PathDestroy(fPaths[f]);
    }
    DRAWING_LOGE("ConvexPaths Destroy: 6657upup");
}

void ConvexPaths::MakePath(OH_Drawing_Path* path) {
    
    OH_Drawing_PathMoveTo(path, 0, 0);
    OH_Drawing_PathQuadTo(path, 50, 100, 0, 100);
    OH_Drawing_PathLineTo(path, 0, 0);
    fPaths.push_back(path);
    
    OH_Drawing_Path* path_2 = OH_Drawing_PathCreate();
    OH_Drawing_PathMoveTo(path_2, 0, 50);
    OH_Drawing_PathQuadTo(path_2, 50, 100, 0, 100);
    OH_Drawing_PathQuadTo(path_2, 50, 100, 0, 50);
    fPaths.push_back(path_2);

    OH_Drawing_Path* rect_path1 = OH_Drawing_PathCreate();
    OH_Drawing_Path* rect_path2 = OH_Drawing_PathCreate();
    DrawRect r = {0, 0, 100, 100};
    DrawRect r1 = {0, 0, 50, 100};
    DrawRect r2 = {0, 0, 100, 5};
    DrawRect r3 = {0, 0, 1, 100};
    OH_Drawing_PathAddRect(rect_path1, r.fLeft, r.fTop, r.fRight, r.fBottom, OH_Drawing_PathDirection::PATH_DIRECTION_CW);
    OH_Drawing_PathAddRect(rect_path2, r.fLeft, r.fTop, r.fRight, r.fBottom, OH_Drawing_PathDirection::PATH_DIRECTION_CCW);
    fPaths.push_back(rect_path1);
    fPaths.push_back(rect_path2);

    OH_Drawing_Path* Circle_path = OH_Drawing_PathCreate();
    auto Circle_bound = OH_Drawing_RectCreate(r.fLeft, r.fTop, r.fRight, r.fBottom);                        // 矩形对象，边框
    OH_Drawing_RoundRect* Round_rect = OH_Drawing_RoundRectCreate(Circle_bound, 2 * kRadius, 2 * kRadius); // 圆角矩形轮廓
    OH_Drawing_PathAddRoundRect(Circle_path, Round_rect, OH_Drawing_PathDirection::PATH_DIRECTION_CW);
    OH_Drawing_PathArcTo(Circle_path, r.fLeft, r.fTop, r.fRight, r.fBottom, 0, 360);
    OH_Drawing_PathClose(Circle_path);
    fPaths.push_back(Circle_path);

    OH_Drawing_Path* Oval_path1 = OH_Drawing_PathCreate();
    OH_Drawing_Path* Oval_path2 = OH_Drawing_PathCreate();
    OH_Drawing_Path* Oval_path3 = OH_Drawing_PathCreate();
    auto Oval_bound1 = OH_Drawing_RectCreate(r1.fLeft, r1.fTop, r1.fRight, r1.fBottom);
    auto Oval_bound2 = OH_Drawing_RectCreate(r2.fLeft, r2.fTop, r2.fRight, r2.fBottom);
    auto Oval_bound3 = OH_Drawing_RectCreate(r3.fLeft, r3.fTop, r3.fRight, r3.fBottom);
    OH_Drawing_PathAddOvalWithInitialPoint(Oval_path1, Oval_bound1, 0, OH_Drawing_PathDirection::PATH_DIRECTION_CW);
    OH_Drawing_PathAddOvalWithInitialPoint(Oval_path2, Oval_bound2, 0, OH_Drawing_PathDirection::PATH_DIRECTION_CCW);
    OH_Drawing_PathAddOvalWithInitialPoint(Oval_path3, Oval_bound3, 0, OH_Drawing_PathDirection::PATH_DIRECTION_CCW);
    OH_Drawing_PathClose(Oval_path1);
    OH_Drawing_PathClose(Oval_path2);
    OH_Drawing_PathClose(Oval_path3);
    fPaths.push_back(OH_Drawing_PathCopy(Oval_path1));
    fPaths.push_back(OH_Drawing_PathCopy(Oval_path2));
    fPaths.push_back(OH_Drawing_PathCopy(Oval_path3));

    OH_Drawing_Path* rrect_path = OH_Drawing_PathCreate();
    OH_Drawing_Rect* rrect = OH_Drawing_RectCreate(r.fLeft, r.fTop, r.fRight, r.fBottom);
    OH_Drawing_RoundRect* round_rect = OH_Drawing_RoundRectCreate(rrect, 40, 20);
    OH_Drawing_PathAddRoundRect(rrect_path, round_rect, OH_Drawing_PathDirection::PATH_DIRECTION_CW);
    fPaths.push_back(rrect_path);

    OH_Drawing_Path* path_3 = OH_Drawing_PathCreate();
    OH_Drawing_PathMoveTo(path_3, 0, 0);
    for (int i = 1; i < kPtsPerSide; ++i) { // skip the first point due to moveTo.
        OH_Drawing_PathLineTo(path, kLength * (float)i / kPtsPerSide, 0);
    }
    for (int i = 0; i < kPtsPerSide; ++i) {
        OH_Drawing_PathLineTo(path_3, kLength, kLength * (float)i / kPtsPerSide);
    }
    for (int i = kPtsPerSide; i > 0; --i) {
        OH_Drawing_PathLineTo(path_3, kLength * (float)i / kPtsPerSide, kLength);
    }
    for (int i = kPtsPerSide; i > 0; --i) {
        OH_Drawing_PathLineTo(path_3, 0, kLength * (float)i / kPtsPerSide);
    }
    fPaths.push_back(path_3);

    // shallow diagonals
    // fPaths.push_back(SkPath::Polygon({{0,0}, {100,1}, {98,100}, {3,96}}, false));
    // Polygon 对应接口未知

    OH_Drawing_Path *rect_path3 = OH_Drawing_PathCreate();
    OH_Drawing_PathArcTo(rect_path3, 0, 0, kRadius, 2*kRadius, 25, 130);
    fPaths.push_back(rect_path3);

    // cubics
    OH_Drawing_Path *cubic_path1 = OH_Drawing_PathCreate();
    OH_Drawing_Path *cubic_path2 = OH_Drawing_PathCreate();
    OH_Drawing_PathCubicTo(cubic_path1, 1, 1, 10, 90, 0, 10);
    OH_Drawing_PathCubicTo(cubic_path2, 100, 50, 20, 100, 0, 0);
    fPaths.push_back(cubic_path1);
    fPaths.push_back(cubic_path2);

    // path that has a cubic with a repeated first control point and
    // a repeated last control point.
    OH_Drawing_Path* path_4 = OH_Drawing_PathCreate();
    OH_Drawing_PathMoveTo(path_4, 10, 10);
    OH_Drawing_PathCubicTo(path_4, 10, 10, 10, 0, 20, 0);
    OH_Drawing_PathLineTo(path_4, 40, 0);
    OH_Drawing_PathCubicTo(path_4, 40, 0, 50, 0, 50, 10);
    fPaths.push_back(path_4);

    // path that has two cubics with repeated middle control points.
    OH_Drawing_Path* path_5 = OH_Drawing_PathCreate();
    OH_Drawing_PathMoveTo(path_5, 10, 10);
    OH_Drawing_PathCubicTo(path_5, 10, 0, 10, 0, 20, 0);
    OH_Drawing_PathLineTo(path_5, 40, 0);
    OH_Drawing_PathCubicTo(path_5, 50, 0, 50, 0, 50, 10);
    fPaths.push_back(path_5);

    // cubic where last three points are almost a line
    OH_Drawing_Path* path_6= OH_Drawing_PathCreate();
    OH_Drawing_PathMoveTo(path_6, 0, 228.0f / 8);
    OH_Drawing_PathCubicTo(path_6, 628.0f / 8, 82.0f / 8,
                           1255.0f / 8, 141.0f / 8,
                           1883.0f / 8, 202.0f / 8);
    fPaths.push_back(path_6);

    // flat cubic where the at end point tangents both point outward.
    OH_Drawing_Path* path_7= OH_Drawing_PathCreate();
    OH_Drawing_PathMoveTo(path_7, 10, 0);
    OH_Drawing_PathCubicTo(path_7, 0, 1, 30, 1, 20, 0);
    fPaths.push_back(path_7);

    // flat cubic where initial tangent is in, end tangent out
    OH_Drawing_Path* path_8= OH_Drawing_PathCreate();
    OH_Drawing_PathMoveTo(path_8, 0, 0);
    OH_Drawing_PathCubicTo(path_8, 10, 1, 30, 1, 20, 0);
    fPaths.push_back(path_8);

    // flat cubic where initial tangent is out, end tangent in
    OH_Drawing_Path* path_9= OH_Drawing_PathCreate();
    OH_Drawing_PathMoveTo(path_9, 10, 0);
    OH_Drawing_PathCubicTo(path_9, 0, 1, 20, 1, 30, 0);
    fPaths.push_back(path_9);

    // triangle where one edge is a degenerate quad
    OH_Drawing_Path* path_10= OH_Drawing_PathCreate();
    OH_Drawing_PathMoveTo(path_10, 8.59375f, 45);
    OH_Drawing_PathQuadTo(path_10, 16.9921875f, 45,
                          31.25f, 45);
    OH_Drawing_PathLineTo(path_10, 100, 100);
    OH_Drawing_PathLineTo(path_10, 8.59375f, 45);
    fPaths.push_back(path_10);

    // triangle where one edge is a quad with a repeated point
    OH_Drawing_Path* path_11= OH_Drawing_PathCreate();
    OH_Drawing_PathMoveTo(path_11, 0, 25);
    OH_Drawing_PathLineTo(path_11, 50, 0);
    OH_Drawing_PathQuadTo(path_11, 50, 50, 50, 50);
    fPaths.push_back(path_11);

    // triangle where one edge is a cubic with a 2x repeated point
    OH_Drawing_Path* path_12= OH_Drawing_PathCreate();
    OH_Drawing_PathMoveTo(path_12, 0, 25);
    OH_Drawing_PathLineTo(path_12, 50, 0);
    OH_Drawing_PathCubicTo(path_12, 50, 0, 50, 50, 50, 50);
    fPaths.push_back(path_12);

    // triangle where one edge is a quad with a nearly repeated point
    OH_Drawing_Path* path_13= OH_Drawing_PathCreate();
    OH_Drawing_PathMoveTo(path_13, 0, 25);
    OH_Drawing_PathLineTo(path_13, 50, 0);
    OH_Drawing_PathQuadTo(path_13, 50, 49.95f, 50, 50);
    fPaths.push_back(OH_Drawing_PathCopy(path_13));

    // triangle where one edge is a cubic with a 3x nearly repeated point
    OH_Drawing_Path* path_14= OH_Drawing_PathCreate();
    OH_Drawing_PathMoveTo(path_14, 0, 25);
    OH_Drawing_PathLineTo(path_14, 50, 0);
    OH_Drawing_PathCubicTo(path_14, 50, 49.95f, 50, 49.97f, 50, 50);
    fPaths.push_back(OH_Drawing_PathCopy(path_14));

    // triangle where there is a point degenerate cubic at one corner
    OH_Drawing_Path* path_15= OH_Drawing_PathCreate();
    OH_Drawing_PathMoveTo(path_15, 0, 25);
    OH_Drawing_PathLineTo(path_15, 50, 0);
    OH_Drawing_PathLineTo(path_15, 50, 50);
    OH_Drawing_PathCubicTo(path_15, 50, 50, 50, 50, 50, 50);
    fPaths.push_back(path_15);

    // point line
    // fPaths.push_back(SkPath::Line({50, 50}, {50, 50}));
    // 该接口调用同上Ploygon，待确定

    // point quad
    OH_Drawing_Path* path_16= OH_Drawing_PathCreate();
    OH_Drawing_PathMoveTo(path_16, 50, 50);
    OH_Drawing_PathQuadTo(path_16, 50, 50, 50, 50);
    fPaths.push_back(path_16);

    // point cubic
    OH_Drawing_Path* path_17= OH_Drawing_PathCreate();
    OH_Drawing_PathMoveTo(path_17, 50, 50);
    OH_Drawing_PathCubicTo(path_17, 50, 50, 50, 50, 50, 50);
    fPaths.push_back(path_17);

    // moveTo only paths
    OH_Drawing_Path* path_18= OH_Drawing_PathCreate();
    OH_Drawing_PathMoveTo(path_18, 0, 0);
    OH_Drawing_PathMoveTo(path_18, 0, 0);
    OH_Drawing_PathMoveTo(path_18, 1, 1);
    OH_Drawing_PathMoveTo(path_18, 1, 1);
    OH_Drawing_PathMoveTo(path_18, 10, 10);
    fPaths.push_back(path_18);

    OH_Drawing_Path* path_19= OH_Drawing_PathCreate();
    OH_Drawing_PathMoveTo(path_19, 0, 0);
    OH_Drawing_PathMoveTo(path_19, 0, 0);
    fPaths.push_back(path_19);

    // line degenerate
    OH_Drawing_Path* path_20= OH_Drawing_PathCreate();
    OH_Drawing_PathLineTo(path_20, 100, 100);
    fPaths.push_back(path_20);
    OH_Drawing_PathQuadTo(path_20, 100, 100, 0, 0);
    fPaths.push_back(path_20);
    OH_Drawing_PathQuadTo(path, 100, 100, 50, 50);
    fPaths.push_back(path_20);
    OH_Drawing_PathQuadTo(path, 50, 50, 100, 100);
    fPaths.push_back(path_20);
    OH_Drawing_PathCubicTo(path, 0, 0, 0, 0, 100, 100);
    fPaths.push_back(path_20);

    // skbug.com/8928
    OH_Drawing_Path* path_21= OH_Drawing_PathCreate();
    OH_Drawing_PathMoveTo(path_21, 16.875f, 192.594f);
    fPaths.push_back(path_21);
    OH_Drawing_Path* path_22= OH_Drawing_PathCreate();
    OH_Drawing_PathCubicTo(path_22, 45.625f, 192.594f, 74.375f, 192.594f, 103.125f, 192.594f);
    fPaths.push_back(path_22);
    OH_Drawing_Path* path_23= OH_Drawing_PathCreate();
    OH_Drawing_PathCubicTo(path_23, 88.75f, 167.708f, 74.375f, 142.823f, 60, 117.938f);
    fPaths.push_back(path_23);
    OH_Drawing_Path* path_24= OH_Drawing_PathCreate();
    OH_Drawing_PathCubicTo(path_24, 45.625f, 142.823f, 31.25f, 167.708f, 16.875f, 192.594f);
    OH_Drawing_PathClose(path_24);
    fPaths.push_back(path_24);


    OH_Drawing_Path* Matrix_path = OH_Drawing_PathCreate();
    OH_Drawing_Matrix* m1= OH_Drawing_MatrixCreate();
    OH_Drawing_MatrixSetMatrix(m1, 0.1f, 0, -1, 0, 0.115207f, -2.64977f, 0, 0, 1);
    OH_Drawing_PathTransform(Matrix_path, m1);
    fPaths.push_back(Matrix_path);
    fPaths.back();

    // small circle. This is listed last so that it has device coords far
    // from the origin (small area relative to x,y values).
    OH_Drawing_Path* Circle_path1 = OH_Drawing_PathCreate();
    auto Circle_bound1 = OH_Drawing_RectCreate(0, 0, 1.2, 1.2); //矩形对象，边框
    OH_Drawing_PathAddArc(Circle_path1, Circle_bound1, 0, 360);
    OH_Drawing_PathClose(Circle_path1);
    fPaths.push_back(Circle_path1);
}