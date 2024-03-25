#include "skbench_addpathtest.h"
#include "random_path_bench.h"
#include <string>
#include <time.h>
#include <bits/alltypes.h>
#include <array>
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_pen.h>
#include <native_drawing/drawing_brush.h>
#include <native_drawing/drawing_bitmap.h>
#include <native_drawing/drawing_canvas.h>
#include <native_drawing/drawing_matrix.h>
#include <native_drawing/drawing_rect.h>
#include <native_drawing/drawing_point.h>
#include "test_common.h"
#include "common/log_common.h"

void SkBench_AddPathTest::SkBench_AddPathTests(int i, int kPathCnt) {
    // 声明名为fPaths0、fPaths1的指针数组，该数组可以存储OH_Drawing_Path类型的指针
    fPaths0 = new OH_Drawing_Path *[kPathCnt];
    fPaths1 = new OH_Drawing_Path *[kPathCnt];
    DRAWING_LOGI("SkBench_AddPathTest_start = %{public}p,%{public}p", fPaths0, fPaths1);
    // 初始化数组中的每个元素为 nullptr
    for (int idx = 0; idx < kPathCnt; ++idx) {
        fPaths0[idx] = nullptr;
        fPaths1[idx] = nullptr;
    }
    DRAWING_LOGI(" SkBench_AddPathTest::SkBench_AddPathTests pointer array initialized to null ");
}

void SkBench_AddPathTest::cleanup() {
    for (int idx = 0; idx < kPathCnt; ++idx) {
        OH_Drawing_PathDestroy(fPaths0[idx]);
        OH_Drawing_PathDestroy(fPaths1[idx]);
        fPaths0[idx] = nullptr;
        fPaths1[idx] = nullptr;
    }
    DRAWING_LOGI(" SkBench_AddPathTest::~SkBench_AddPathTest Automatically destroy objects at the end of their lifecycle ");
    delete[] reinterpret_cast<char *>(fPaths0); // 释放fPaths0数组所占用的内存
    delete[] reinterpret_cast<char *>(fPaths1); // 释放fPaths1数组所占用的内存
    fPaths0 = nullptr;
    fPaths1 = nullptr;
    DRAWING_LOGI("SkBench_AddPathTest::~SkBench_AddPathTest delete");
}

void SkBench_AddPathTest::OnTestFunction(OH_Drawing_Canvas *canvas) {
    OH_Drawing_Pen *pen = OH_Drawing_PenCreate();
    OH_Drawing_Brush *brush = OH_Drawing_BrushCreate();
    OH_Drawing_PenSetAntiAlias(pen, true);
    OH_Drawing_PenSetWidth(pen, width);
    OH_Drawing_CanvasAttachPen(canvas, pen);

    OH_Drawing_Matrix *m = OH_Drawing_MatrixCreate();
    OH_Drawing_Matrix *m_kAddMatrix = OH_Drawing_MatrixCreate();
    OH_Drawing_MatrixRotate(m_kAddMatrix, 60 * SK_Scalar1, 0, 0);
    OH_Drawing_MatrixSetMatrix(m_kAddMatrix, 1, 0.1, 0, 0, 1, 0, 0.0001, 0.001, 1);
    SkBench_AddPathTests(i, kPathCnt);
    createData(10, 100, true, nullptr);
    for (int i = 0; i < kPathCnt; ++i) {
        fPaths0[i] = OH_Drawing_PathCreate();
        fPaths1[i] = OH_Drawing_PathCreate();
        makePath(fPaths0[i]);
        makePath(fPaths1[i]);
    }
    restartMakingPaths();

    switch (fType) {
    case kAdd_AddType:
        for (int i = 0; i < testCount_; i++) {
            int idx = i & (kPathCnt - 1);
            OH_Drawing_PathAddPath(fPaths0[idx], fPaths1[idx], m);
            if (bType == PAINTBRUSH_RECT) {
                OH_Drawing_BrushSetColor(brush, colors[i % RAND_SIZE]);
                OH_Drawing_BrushSetAntiAlias(brush, true);
                OH_Drawing_CanvasAttachBrush(canvas, brush);
                OH_Drawing_CanvasDrawPath(canvas, fPaths0[idx]);
                OH_Drawing_CanvasDetachBrush(canvas);
            } else if (bType == NO_BRUSH_RECT) {
                OH_Drawing_CanvasDrawPath(canvas, fPaths0[idx]);
                OH_Drawing_CanvasDetachBrush(canvas);
            }
        }
        DRAWING_LOGI("SkBench_AddPathTest::OnTestFunction_kAdd = %{public}p,%{public}p", fPaths0[i], fPaths1[i]);
        break;
    case kAddTrans_AddType:
        for (int i = 0; i < testCount_; i++) {
            int idx = i & (kPathCnt - 1);
            OH_Drawing_Path *result = fPaths0[idx];
            OH_Drawing_MatrixTranslate(m, horizontal_distance * SK_Scalar1, vertical_distance * SK_Scalar1);
            OH_Drawing_PathAddPath(result, fPaths1[idx], m);
            if (bType == PAINTBRUSH_RECT) {
                OH_Drawing_BrushSetColor(brush, colors[i % RAND_SIZE]);
                OH_Drawing_BrushSetAntiAlias(brush, true);
                OH_Drawing_CanvasAttachBrush(canvas, brush);
                OH_Drawing_CanvasDrawPath(canvas, result);
                OH_Drawing_CanvasDetachBrush(canvas);
            } else if (bType == NO_BRUSH_RECT) {
                OH_Drawing_CanvasDrawPath(canvas, result);
                OH_Drawing_CanvasDetachBrush(canvas);
            }
        }
        DRAWING_LOGI("SkBench_AddPathTest::OnTestFunction_kAddTrans = %{public}p,%{public}p", fPaths0[i], fPaths1[i]);
        break;
    case kAddMatrix_AddType:
        for (int i = 0; i < testCount_; i++) {
            int idx = i & (kPathCnt - 1);
            OH_Drawing_PathAddPath(fPaths0[idx], fPaths1[idx], m_kAddMatrix);
            if (bType == PAINTBRUSH_RECT) {
                OH_Drawing_BrushSetColor(brush, colors[i % RAND_SIZE]);
                OH_Drawing_BrushSetAntiAlias(brush, true);
                OH_Drawing_CanvasAttachBrush(canvas, brush);
                OH_Drawing_CanvasDrawPath(canvas, fPaths0[idx]);
                OH_Drawing_CanvasDetachBrush(canvas);
            } else if (bType == NO_BRUSH_RECT) {
                OH_Drawing_CanvasDrawPath(canvas, fPaths0[idx]);
                OH_Drawing_CanvasDetachBrush(canvas);
            }
        }
        DRAWING_LOGI("SkBench_AddPathTest::OnTestFunction_kAddMatrix = %{public}p,%{public}p", fPaths0[i], fPaths1[i]);
        break;
    }
    cleanup();
    OH_Drawing_MatrixDestroy(m);
    OH_Drawing_MatrixDestroy(m_kAddMatrix);
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_BrushDestroy(brush);
    OH_Drawing_PenDestroy(pen);
    pen = nullptr;

    DRAWING_LOGI(" SkBench_AddPathTest::OnTestFunction end");
}


void SkBench_AddPathTest::OnTestPerformance(OH_Drawing_Canvas *canvas) {
    // 创建一个画笔pen画刷brush对象
    OH_Drawing_Pen *pen = OH_Drawing_PenCreate();
    OH_Drawing_Brush *brush = OH_Drawing_BrushCreate();
    OH_Drawing_PenSetAntiAlias(pen, true);
    OH_Drawing_PenSetWidth(pen, width);
    OH_Drawing_CanvasAttachPen(canvas, pen);

    // 创建对象设置变换矩阵为单位矩阵
    OH_Drawing_Matrix *m = OH_Drawing_MatrixCreate();
    OH_Drawing_Matrix *m_kAddMatrix = OH_Drawing_MatrixCreate();
    OH_Drawing_MatrixRotate(m_kAddMatrix, 60 * SK_Scalar1, 0, 0);
    OH_Drawing_MatrixSetMatrix(m_kAddMatrix, 1, 0.1, 0, 0, 1, 0, 0.0001, 0.001, 1);

    SkBench_AddPathTests(i, kPathCnt);
    createData(10, 100, true, nullptr);

    for (int i = 0; i < kPathCnt; ++i) {
        fPaths0[i] = OH_Drawing_PathCreate();
        fPaths1[i] = OH_Drawing_PathCreate();
        makePath(fPaths0[i]);
        makePath(fPaths1[i]);
    }
    restartMakingPaths();

    switch (fType) {
        DRAWING_LOGI(" SkBench_AddPathTest::fType start");
    case kAdd_AddType:
        DRAWING_LOGI(" SkBench_AddPathTest::kAdd_AddType start");
        for (int i = 0; i < testCount_; i++) {
            int idx = i & (kPathCnt - 1);
            OH_Drawing_PathAddPath(fPaths0[idx], fPaths1[idx], m);
            if (bType == PAINTBRUSH_RECT) {
                OH_Drawing_BrushSetColor(brush, colors[i % RAND_SIZE]);
                OH_Drawing_BrushSetAntiAlias(brush, true);
                OH_Drawing_CanvasAttachBrush(canvas, brush);
                OH_Drawing_CanvasDrawPath(canvas, fPaths0[idx]);
                OH_Drawing_CanvasDetachBrush(canvas);
            } else if (bType == NO_BRUSH_RECT) {
                OH_Drawing_CanvasDrawPath(canvas, fPaths0[idx]);
                OH_Drawing_CanvasDetachBrush(canvas);
            }
        }
        DRAWING_LOGI("SkBench_AddPathTest::OnTestPerformance_kAdd = %{public}p,%{public}p", fPaths0[i], fPaths1[i]);
        break;
    case kAddTrans_AddType:
        DRAWING_LOGI(" SkBench_AddPathTest::kAddTrans_AddType start");
        for (int i = 0; i < testCount_; i++) {
            int idx = i & (kPathCnt - 1);
            OH_Drawing_Path *result = fPaths0[idx];
            OH_Drawing_MatrixTranslate(m, horizontal_distance * SK_Scalar1, vertical_distance * SK_Scalar1);
            OH_Drawing_PathAddPath(result, fPaths1[idx], m);
            if (bType == PAINTBRUSH_RECT) {
                OH_Drawing_BrushSetColor(brush, colors[i % RAND_SIZE]);
                OH_Drawing_BrushSetAntiAlias(brush, true);
                OH_Drawing_CanvasAttachBrush(canvas, brush);
                OH_Drawing_CanvasDrawPath(canvas, result);
                OH_Drawing_CanvasDetachBrush(canvas);
            } else if (bType == NO_BRUSH_RECT) {
                OH_Drawing_CanvasDrawPath(canvas, result);
                OH_Drawing_CanvasDetachBrush(canvas);
            }
        }
        DRAWING_LOGI("SkBench_AddPathTest::OnTestPerformance_kAddTrans = %{public}p,%{public}p", fPaths0[i], fPaths1[i]);
        break;
    case kAddMatrix_AddType:
        DRAWING_LOGI(" SkBench_AddPathTest::kAddMatrix_AddType start");
        for (int i = 0; i < testCount_; i++) {
            int idx = i & (kPathCnt - 1);
            OH_Drawing_PathAddPath(fPaths0[idx], fPaths1[idx], m_kAddMatrix);
            if (bType == PAINTBRUSH_RECT) {
                OH_Drawing_BrushSetColor(brush, colors[i % RAND_SIZE]);
                OH_Drawing_BrushSetAntiAlias(brush, true);
                OH_Drawing_CanvasAttachBrush(canvas, brush);
                OH_Drawing_CanvasDrawPath(canvas, fPaths0[idx]);
                OH_Drawing_CanvasDetachBrush(canvas);
            } else if (bType == NO_BRUSH_RECT) {
                OH_Drawing_CanvasDrawPath(canvas, fPaths0[idx]);
                OH_Drawing_CanvasDetachBrush(canvas);
            }
        }
        DRAWING_LOGI("SkBench_AddPathTest::OnTestPerformance_kAddMatrix = %{public}p,%{public}p", fPaths0[i], fPaths1[i]);
        break;
    }
    cleanup();
    OH_Drawing_MatrixDestroy(m);
    OH_Drawing_MatrixDestroy(m_kAddMatrix);
    OH_Drawing_CanvasDetachPen(canvas);
    OH_Drawing_BrushDestroy(brush);
    OH_Drawing_PenDestroy(pen);
    pen = nullptr;

    DRAWING_LOGI(" SkBench_AddPathTest::OnTestPerformance end");
}
