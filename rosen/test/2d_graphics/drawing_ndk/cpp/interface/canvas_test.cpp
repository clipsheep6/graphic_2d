#include "canvas_test.h"
#include <multimedia/image_framework/image_pixel_map_mdk.h>
#include <native_drawing/drawing_color.h>
#include <native_drawing/drawing_brush.h>
#include <native_drawing/drawing_matrix.h>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_pen.h>
#include <native_drawing/drawing_region.h>
#include <native_drawing/drawing_round_rect.h>
#include <native_drawing/drawing_shader_effect.h>
#include <native_drawing/drawing_point.h>
#include <native_drawing/drawing_image.h>
#include <native_drawing/drawing_filter.h>
#include <native_drawing/drawing_sampling_options.h>
#include <native_drawing/drawing_path_effect.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <native_drawing/drawing_text_blob.h>
#include <native_drawing/drawing_rect.h>
#include <native_drawing/drawing_font.h>

#include "test_common.h"

#include "common/log_common.h"

void CanvasDrawRect::OnTestPerformance(OH_Drawing_Canvas *canvas) {
    for (int i = 0; i < testCount_; i++) {
        int l = i % bitmapWidth_;
        int t = (i + 100) % bitmapHeight_;
        int r = ((l + 100) > bitmapWidth_) ? bitmapWidth_ : (l + 100);
        int b = ((t + 100) > bitmapHeight_) ? bitmapHeight_ : (t + 100);
        // 针对rect，每次的绘制起点位置，绘制的宽高大小需要不一致
        OH_Drawing_Rect *rect = OH_Drawing_RectCreate(l, t, r, b);
        OH_Drawing_CanvasDrawRect(canvas, rect);
        OH_Drawing_RectDestroy(rect);
    }
}

void CanvasDrawBitmapRect::OnTestPerformance(OH_Drawing_Canvas *canvas) {
    TestRend rand;
    OH_Drawing_Bitmap *bitmap = OH_Drawing_BitmapCreate();

    OH_Drawing_BitmapFormat cFormat{COLOR_FORMAT_BGRA_8888, ALPHA_FORMAT_OPAQUE};
    OH_Drawing_BitmapBuild(bitmap, 200, 200, &cFormat);
    OH_Drawing_Canvas *bitmapCanvas = OH_Drawing_CanvasCreate();
    OH_Drawing_CanvasBind(bitmapCanvas, bitmap);
    OH_Drawing_CanvasClear(bitmapCanvas, OH_Drawing_ColorSetArgb(0xff, 0x00, 0xff, 0x00));
    OH_Drawing_SamplingOptions *sampling = OH_Drawing_SamplingOptionsCreate(OH_Drawing_FilterMode::FILTER_MODE_LINEAR, OH_Drawing_MipmapMode::MIPMAP_MODE_LINEAR);

    for (int i = 0; i < testCount_; i++) {
        float l = rand.nextULessThan(bitmapWidth_);
        float t = rand.nextULessThan(bitmapHeight_);
        float r = l + rand.nextULessThan(bitmapWidth_);
        float b = t + rand.nextULessThan(bitmapHeight_);
        OH_Drawing_Rect *dst = OH_Drawing_RectCreate(l, t, r, b);
        OH_Drawing_CanvasDrawBitmapRect(canvas, bitmap, nullptr, dst, sampling);

        OH_Drawing_RectDestroy(dst);
    }

    OH_Drawing_SamplingOptionsDestroy(sampling);
    OH_Drawing_BitmapDestroy(bitmap);
    OH_Drawing_CanvasDestroy(bitmapCanvas);
}

void CanvasDrawCircle::OnTestPerformance(OH_Drawing_Canvas *canvas) {
    TestRend rand;

    for (int i = 0; i < testCount_; i++) {
        float x = rand.nextULessThan(bitmapWidth_);
        float y = rand.nextULessThan(bitmapHeight_);
        OH_Drawing_Point *point = OH_Drawing_PointCreate(x, y);
        float z = rand.nextULessThan(bitmapHeight_);

        OH_Drawing_CanvasDrawCircle(canvas, point, z);

        OH_Drawing_PointDestroy(point);
    }
}

void CanvasDrawOval::OnTestPerformance(OH_Drawing_Canvas *canvas) {
    TestRend rand;

    for (int i = 0; i < testCount_; i++) {
        float l = rand.nextULessThan(bitmapWidth_);
        float t = rand.nextULessThan(bitmapHeight_);
        float r = l + rand.nextULessThan(bitmapWidth_);
        float b = t + rand.nextULessThan(bitmapHeight_);
        OH_Drawing_Rect *rect = OH_Drawing_RectCreate(l, t, r, b);

        OH_Drawing_CanvasDrawOval(canvas, rect);

        OH_Drawing_RectDestroy(rect);
    }
}

void CanvasDrawArc::OnTestPerformance(OH_Drawing_Canvas *canvas) {
    TestRend rand;

    for (int i = 0; i < testCount_; i++) {
        float l = rand.nextULessThan(bitmapWidth_);
        float t = rand.nextULessThan(bitmapHeight_);
        float r = l + rand.nextULessThan(bitmapWidth_);
        float b = t + rand.nextULessThan(bitmapHeight_);
        OH_Drawing_Rect *rect = OH_Drawing_RectCreate(l, t, r, b);
        float startAngle = rand.nextULessThan(bitmapWidth_);
        float sweepAngle = rand.nextULessThan(bitmapHeight_);

        OH_Drawing_CanvasDrawArc(canvas, rect, startAngle, sweepAngle);

        OH_Drawing_RectDestroy(rect);
    }
}

void CanvasDrawRoundRect::OnTestPerformance(OH_Drawing_Canvas *canvas) {
    TestRend rand;

    for (int i = 0; i < testCount_; i++) {
        float l = rand.nextULessThan(bitmapWidth_);
        float t = rand.nextULessThan(bitmapHeight_);
        float r = l + rand.nextULessThan(bitmapWidth_);
        float b = t + rand.nextULessThan(bitmapHeight_);
        OH_Drawing_Rect *rect = OH_Drawing_RectCreate(l, t, r, b);
        float xRad = rand.nextULessThan(bitmapWidth_);
        float yRad = rand.nextULessThan(bitmapHeight_);
        OH_Drawing_RoundRect *roundRect = OH_Drawing_RoundRectCreate(rect, xRad, yRad);

        OH_Drawing_CanvasDrawRoundRect(canvas, roundRect);

        OH_Drawing_RectDestroy(rect);
    }
}

void CanvasDrawTextBlob::OnTestPerformance(OH_Drawing_Canvas *canvas) {
    std::string text = "textblob";
    int len = text.length();
    OH_Drawing_Font *font = OH_Drawing_FontCreate();
    TestRend rand;
    for (int i = 0; i < testCount_; i++) {
        int x = rand.nextRangeF(0, bitmapWidth_);
        int y = rand.nextRangeF(0, bitmapHeight_);

        OH_Drawing_TextBlob *blob = OH_Drawing_TextBlobCreateFromText(text.c_str(), len, font, TEXT_ENCODING_UTF8);
        OH_Drawing_CanvasDrawTextBlob(canvas, blob, x, y);
        OH_Drawing_TextBlobDestroy(blob);
    }
}

void CanvasClipRect::OnTestPerformance(OH_Drawing_Canvas *canvas) {
    TestRend rand;

    for (int i = 0; i < testCount_; i++) {
        float l = rand.nextULessThan(bitmapWidth_);
        float t = rand.nextULessThan(bitmapHeight_);
        float r = l + rand.nextULessThan(bitmapWidth_);
        float b = t + rand.nextULessThan(bitmapHeight_);
        OH_Drawing_Rect *rect = OH_Drawing_RectCreate(l, t, r, b);

        OH_Drawing_CanvasDrawRect(canvas, rect);

        OH_Drawing_CanvasClipRect(canvas, rect, OH_Drawing_CanvasClipOp::DIFFERENCE, true);

        OH_Drawing_RectDestroy(rect);
    }
}

void CanvasClipRoundRect::OnTestPerformance(OH_Drawing_Canvas *canvas) {
    TestRend rand;

    for (int i = 0; i < testCount_; i++) {
        float l = rand.nextULessThan(bitmapWidth_);
        float t = rand.nextULessThan(bitmapHeight_);
        float r = l + rand.nextULessThan(bitmapWidth_);
        float b = t + rand.nextULessThan(bitmapHeight_);
        OH_Drawing_Rect *rect = OH_Drawing_RectCreate(l, t, r, b);
        float xRad = rand.nextULessThan(bitmapWidth_);
        float yRad = rand.nextULessThan(bitmapHeight_);
        OH_Drawing_RoundRect *roundRect = OH_Drawing_RoundRectCreate(rect, xRad, yRad);

        OH_Drawing_CanvasDrawRoundRect(canvas, roundRect);

        OH_Drawing_CanvasClipRoundRect(canvas, roundRect, OH_Drawing_CanvasClipOp::DIFFERENCE, true);

        OH_Drawing_RectDestroy(rect);
    }
}

void CanvasRotate::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    float_t min = 0.0f; // 最小值
    float_t max = 360.0f; // 最大值
    float_t readTime = renderer.nextRangeF(min, max);// 生成随机浮点数
    float randNumberx = renderer.nextF();
    float randNumbery = renderer.nextF();
    
    for (int i = 0; i < testCount_; i++) {
        int l = i % bitmapWidth_;
        int t = (i + 100) % bitmapHeight_;
        int r = ((l + 100) > bitmapWidth_) ? bitmapWidth_ : (l + 100);
        int b = ((t + 100) > bitmapHeight_) ? bitmapHeight_ : (t + 100);
        OH_Drawing_Canvas *drawingCanvas = OH_Drawing_CanvasCreate();
        OH_Drawing_CanvasRotate(drawingCanvas, readTime, randNumberx, randNumbery);
        OH_Drawing_Rect* rect = OH_Drawing_RectCreate(l, t, r, b); 
        OH_Drawing_CanvasDrawOval(canvas, rect);
        OH_Drawing_RectDestroy(rect);
        OH_Drawing_CanvasDestroy(drawingCanvas);
    }
}


void CanvasTranslate::OnTestPerformance(OH_Drawing_Canvas * canvas) {
    
    float randNumberx = renderer.nextF();
    float randNumbery = renderer.nextF();
    for (int i = 0; i < testCount_; i++) {
        int l = i % bitmapWidth_;
        int t = (i + 100) % bitmapHeight_;
        int r = ((l + 100) > bitmapWidth_) ? bitmapWidth_ : (l + 100);
        int b = ((t + 100) > bitmapHeight_) ? bitmapHeight_ : (t + 100);
        OH_Drawing_Canvas *drawingCanvas = OH_Drawing_CanvasCreate();
        OH_Drawing_Rect* rect = OH_Drawing_RectCreate(l, t, r, b);
        OH_Drawing_CanvasDrawRect(canvas, rect);
        OH_Drawing_RectDestroy(rect);
        OH_Drawing_CanvasTranslate(drawingCanvas, randNumberx, randNumbery);
        OH_Drawing_CanvasDestroy(drawingCanvas);
    }
}

void CanvasScale::OnTestPerformance(OH_Drawing_Canvas * canvas) 
{
    float randNumberx = renderer.nextF();
    float randNumbery = renderer.nextF();
    for (int i = 0; i < testCount_; i++) {
        int l = i % bitmapWidth_;
        int t = (i + 100) % bitmapHeight_;
        int r = ((l + 100) > bitmapWidth_) ? bitmapWidth_ : (l + 100);
        int b = ((t + 100) > bitmapHeight_) ? bitmapHeight_ : (t + 100);
        OH_Drawing_Canvas *drawingCanvas = OH_Drawing_CanvasCreate();
        OH_Drawing_Rect* rect = OH_Drawing_RectCreate(l, t, r, b);
        OH_Drawing_CanvasDrawRect(canvas, rect);
        OH_Drawing_CanvasScale(drawingCanvas, randNumberx, randNumbery);
        OH_Drawing_RectDestroy(rect);
        OH_Drawing_CanvasDestroy(drawingCanvas);
    }
}


void CanvasConcatMatrix::OnTestPerformance(OH_Drawing_Canvas * canvas) 
{
    float randNumberx = renderer.nextF();
    float randNumbery = renderer.nextF();
    for (int i = 0; i < testCount_; i++) {
        int l = i % bitmapWidth_;
        int t = (i + 100) % bitmapHeight_;
        int r = ((l + 100) > bitmapWidth_) ? bitmapWidth_ : (l + 100);
        int b = ((t + 100) > bitmapHeight_) ? bitmapHeight_ : (t + 100);
        OH_Drawing_Canvas *drawingCanvas = OH_Drawing_CanvasCreate();
        OH_Drawing_Rect* rect = OH_Drawing_RectCreate(l, t, r, b);
        OH_Drawing_Matrix *matrix = OH_Drawing_MatrixCreateTranslation(randNumberx, randNumbery);
        OH_Drawing_CanvasConcatMatrix(canvas, matrix);
        OH_Drawing_RectDestroy(rect);
        OH_Drawing_CanvasDestroy(drawingCanvas);
    }
}

void CanvasClear::OnTestPerformance(OH_Drawing_Canvas * canvas) 
{
    uint32_t color = 0xFF0000FF;
    for (int i = 0; i < testCount_; i++) {
        int l = i % bitmapWidth_;
        int t = (i + 100) % bitmapHeight_;
        int r = ((l + 100) > bitmapWidth_) ? bitmapWidth_ : (l + 100);
        int b = ((t + 100) > bitmapHeight_) ? bitmapHeight_ : (t + 100);
        OH_Drawing_Canvas *drawingCanvas = OH_Drawing_CanvasCreate();
        OH_Drawing_Rect* rect = OH_Drawing_RectCreate(l, t, r, b);
        OH_Drawing_CanvasClear(canvas, color);
        OH_Drawing_RectDestroy(rect);
        OH_Drawing_CanvasDestroy(drawingCanvas);
    }
}


void CanvasSetMatrix::OnTestPerformance(OH_Drawing_Canvas * canvas) 
{
    OH_Drawing_Matrix* matRix =  OH_Drawing_MatrixCreate ();
    for (int i = 0; i < testCount_; i++) {
        int l = i % bitmapWidth_;
        int t = (i + 100) % bitmapHeight_;
        int r = ((l + 100) > bitmapWidth_) ? bitmapWidth_ : (l + 100);
        int b = ((t + 100) > bitmapHeight_) ? bitmapHeight_ : (t + 100);
        //针对rect，每次的绘制起点位置，绘制的宽高大小需要不一致
        OH_Drawing_Rect* rect = OH_Drawing_RectCreate(l, t, r, b);
        OH_Drawing_CanvasSetMatrix (canvas , matRix);
        OH_Drawing_CanvasDrawRect(canvas, rect);
        OH_Drawing_RectDestroy(rect);
    }
}

void CanvasResetMatrix::OnTestPerformance(OH_Drawing_Canvas * canvas) 
{
    OH_Drawing_Matrix* matRix =  OH_Drawing_MatrixCreate ();
    for (int i = 0; i < testCount_; i++) {
        int l = i % bitmapWidth_;
        int t = (i + 100) % bitmapHeight_;
        int r = ((l + 100) > bitmapWidth_) ? bitmapWidth_ : (l + 100);
        int b = ((t + 100) > bitmapHeight_) ? bitmapHeight_ : (t + 100);
        //针对rect，每次的绘制起点位置，绘制的宽高大小需要不一致
        OH_Drawing_Rect* rect = OH_Drawing_RectCreate(l, t, r, b);
        OH_Drawing_CanvasSetMatrix (canvas , matRix);
        OH_Drawing_CanvasDrawRect(canvas, rect);
        OH_Drawing_CanvasResetMatrix(canvas);
        OH_Drawing_RectDestroy(rect);
    }
    
}



void CanvasDrawImageRectWithSrc::OnTestPerformance(OH_Drawing_Canvas * canvas) 
{

    OH_Drawing_Image *Image =  OH_Drawing_ImageCreate ();  //参数2
    // 创建采样选项对象
    OH_Drawing_FilterMode filterMode = FILTER_MODE_LINEAR;
    OH_Drawing_MipmapMode mipmapMode = MIPMAP_MODE_NEAREST;
    OH_Drawing_SamplingOptions* samplingOptions = OH_Drawing_SamplingOptionsCreate(filterMode , mipmapMode); //参数5
    for (int i = 0; i < testCount_; i++) {
        int l = i % bitmapWidth_;
        int t = (i + 100) % bitmapHeight_;
        int r = ((l + 100) > bitmapWidth_) ? bitmapWidth_ : (l + 100);
        int b = ((t + 100) > bitmapHeight_) ? bitmapHeight_ : (t + 100);
        //针对rect，每次的绘制起点位置，绘制的宽高大小需要不一致
        OH_Drawing_Rect* recty = OH_Drawing_RectCreate(l, t, r, b);  //参数3
        OH_Drawing_Rect* rectm = OH_Drawing_RectCreate(l + 10, t + 10, r + 10, b + 10);  //参数4
        OH_Drawing_SrcRectConstraint enumConstr = i % 2 == 0 ? STRICT_SRC_RECT_CONSTRAINT : FAST_SRC_RECT_CONSTRAINT;
        OH_Drawing_CanvasDrawImageRectWithSrc(canvas, Image, recty, rectm, samplingOptions, enumConstr);
        OH_Drawing_CanvasDrawRect(canvas, recty);
        OH_Drawing_CanvasDrawRect(canvas, rectm);
        OH_Drawing_RectDestroy(recty);
        OH_Drawing_RectDestroy(rectm);
    }
}

void CanvasDrawShadow::OnTestPerformance(OH_Drawing_Canvas *canvas) 
{
    OH_Drawing_Path *pathCreate = OH_Drawing_PathCreate();  // 用于创建一个路径对象。
    
    OH_Drawing_Point3D planeParams, devLightPos;
    planeParams.x = 1.2f;
    planeParams.y = 2.0f;
    planeParams.z =3.0f;
    devLightPos.x = 4.0f;
    devLightPos.y = 5.0f;
    devLightPos.z = 6.0f;
    
    float lightRadius = 10.0f;
    uint32_t ambientColor = 11;
    uint32_t spotColor = 12;
    OH_Drawing_CanvasShadowFlags flag = SHADOW_FLAGS_TRANSPARENT_OCCLUDER;

    for (int i = 0; i < testCount_; i++) {
        int l = i % bitmapWidth_;
        int t = (i + 100) % bitmapHeight_;
        int r = ((l + 100) > bitmapWidth_) ? bitmapWidth_ : (l + 100);
        int b = ((t + 100) > bitmapHeight_) ? bitmapHeight_ : (t + 100);
        // 针对 rect，每次的绘制起点位置，绘制的宽高大小需要不一致
        OH_Drawing_Rect* rect = OH_Drawing_RectCreate(l, t, r, b);
        
        OH_Drawing_CanvasDrawRect(canvas, rect);
        
        OH_Drawing_CanvasDrawShadow(canvas, pathCreate, planeParams, devLightPos, lightRadius, ambientColor, spotColor, flag);
        OH_Drawing_RectDestroy(rect);
    }
    // 在使用完路径后销毁它
    OH_Drawing_PathDestroy(pathCreate);
}

void CanvasSkew::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    float sx = renderer.nextF();
    float sy = renderer.nextF();
    float randNumberx = renderer.nextF();
    float randNumbery = renderer.nextF();
    for (int i = 0; i < testCount_; i++) {
        int l = i % bitmapWidth_;
        int t = (i + 100) % bitmapHeight_;
        int r = ((l + 100) > bitmapWidth_) ? bitmapWidth_ : (l + 100);
        int b = ((t + 100) > bitmapHeight_) ? bitmapHeight_ : (t + 100);
        //针对rect，每次的绘制起点位置，绘制的宽高大小需要不一致
        OH_Drawing_Canvas *drawingCanvas = OH_Drawing_CanvasCreate();
        OH_Drawing_Rect* rect = OH_Drawing_RectCreate(l, t, r, b);
        OH_Drawing_CanvasTranslate(drawingCanvas, randNumberx, randNumbery);
        OH_Drawing_CanvasDestroy(drawingCanvas);
        OH_Drawing_CanvasSkew (canvas ,  sx, sy );
        OH_Drawing_CanvasDrawRect(canvas, rect);
        OH_Drawing_RectDestroy(rect);
    }
}

void CanvasDrawImageRect::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    uint32_t w = 64;
    uint32_t h = 64;
    OH_Drawing_Bitmap* bm= OH_Drawing_BitmapCreate();
    OH_Drawing_BitmapFormat format = {COLOR_FORMAT_ALPHA_8, ALPHA_FORMAT_OPAQUE};
    OH_Drawing_BitmapBuild(bm, w, h, &format);
    OH_Drawing_Image* image = OH_Drawing_ImageCreate();
    OH_Drawing_ImageBuildFromBitmap(image, bm);
    OH_Drawing_SamplingOptions* samplingOptions = OH_Drawing_SamplingOptionsCreate(FILTER_MODE_LINEAR , MIPMAP_MODE_NEAREST);
    for (int i = 0; i < testCount_; i++) {
        int l = i % bitmapWidth_;
        int t = (i + 100) % bitmapHeight_;
        int r = ((l + 100) > bitmapWidth_) ? bitmapWidth_ : (l + 100);
        int b = ((t + 100) > bitmapHeight_) ? bitmapHeight_ : (t + 100);
        OH_Drawing_Rect* rect = OH_Drawing_RectCreate(l, t, r, b);
        OH_Drawing_CanvasDrawImageRect(canvas, image, rect, samplingOptions);
        OH_Drawing_RectDestroy(rect);
    }
    OH_Drawing_ImageDestroy(image);
    OH_Drawing_SamplingOptionsDestroy(samplingOptions);
    OH_Drawing_BitmapDestroy(bm);
}

void CanvasDrawVertices::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_Point2D positions[] = {
            {100, 100},
            {200, 100},
            {150, 200}};
        OH_Drawing_Point2D texs[] = {
            {0.0f, 0.0f},
            {1.0f, 0.0f},
            {0.5f, 1.0f}};
        const uint16_t indices[] = {
            0, 1, 2
        };
        uint32_t colors[] = {0xFFFF0000, 0xFF00FF00, 0xFF0000FF};
        int32_t indexCount = sizeof(indices) / sizeof(indices[0]);
        int32_t vertexCount = sizeof(positions) / sizeof(positions[0]);
        OH_Drawing_CanvasDrawVertices(canvas, OH_Drawing_VertexMode::VERTEX_MODE_TRIANGLES, vertexCount, positions, texs,
                                      colors, indexCount, indices, OH_Drawing_BlendMode::BLEND_MODE_SRC);
    }
}

void CanvasReadPixels::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    int32_t w = 64;
    int32_t h = 64;
    OH_Drawing_Bitmap* bm= OH_Drawing_BitmapCreate();
    OH_Drawing_Image_Info imageInfo {w,h,COLOR_FORMAT_RGBA_8888, ALPHA_FORMAT_PREMUL} ;
    OH_Drawing_BitmapFormat cFormat{COLOR_FORMAT_RGBA_8888, ALPHA_FORMAT_PREMUL};
    OH_Drawing_BitmapBuild(bm, w, h, &cFormat);
    for (int i = 0; i < testCount_; i++) {
        int l = i % bitmapWidth_;
        int t = (i + 100) % bitmapHeight_;
        int r = ((l + 100) > bitmapWidth_) ? bitmapWidth_ : (l + 100);
        int b = ((t + 100) > bitmapHeight_) ? bitmapHeight_ : (t + 100);
        OH_Drawing_Rect* rect = OH_Drawing_RectCreate(l, t, r, b);
        OH_Drawing_CanvasDrawRect(canvas, rect);
        OH_Drawing_CanvasReadPixels(canvas, &imageInfo, OH_Drawing_BitmapGetPixels(bm), w, w, h);
        OH_Drawing_RectDestroy(rect);
    }
    OH_Drawing_BitmapDestroy(bm);
}

void CanvasReadPixelsToBitmap::OnTestPerformance(OH_Drawing_Canvas* canvas)
{
    int32_t w = 64;
    int32_t h = 64;
    OH_Drawing_Bitmap* bm= OH_Drawing_BitmapCreate();
    OH_Drawing_BitmapFormat cFormat{COLOR_FORMAT_RGBA_8888, ALPHA_FORMAT_PREMUL};
    OH_Drawing_BitmapBuild(bm, w, h, &cFormat);
    for (int i = 0; i < testCount_; i++) {
        int l = i % bitmapWidth_;
        int t = (i + 100) % bitmapHeight_;
        int r = ((l + 100) > bitmapWidth_) ? bitmapWidth_ : (l + 100);
        int b = ((t + 100) > bitmapHeight_) ? bitmapHeight_ : (t + 100);
        OH_Drawing_Rect* rect = OH_Drawing_RectCreate(l, t, r, b);
        OH_Drawing_CanvasDrawRect(canvas, rect);
        OH_Drawing_CanvasReadPixelsToBitmap(canvas, bm, l, h);
        OH_Drawing_RectDestroy(rect);
    }
    OH_Drawing_BitmapDestroy(bm);
}

void CanvasDrawPath::OnTestPerformance(OH_Drawing_Canvas* canvas) {
    OH_Drawing_Path* path = OH_Drawing_PathCreate();
    OH_Drawing_PathMoveTo(path, 0, 0);
    TestRend rand = TestRend();
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_PathLineTo(path, rand.nextRangeF(0,720), rand.nextRangeF(0,720));
        OH_Drawing_CanvasDrawPath(canvas, path);
    }
    OH_Drawing_PathDestroy(path);
}

void CanvasRestoreToCount::OnTestPerformance(OH_Drawing_Canvas* canvas) {
    fSave_Count = OH_Drawing_CanvasGetSaveCount(canvas);
    OH_Drawing_CanvasSave(canvas);
    TestRend rand;
    for (int i = 0; i < testCount_; i++) {
        
        float x1 = rand.nextULessThan(bitmapWidth_);
        float y1 = rand.nextULessThan(bitmapHeight_);
        float x2 = rand.nextULessThan(bitmapWidth_);
        float y2 = rand.nextULessThan(bitmapHeight_);
        //针对rect，每次的绘制起点位置，绘制的宽高大小需要不一致
        OH_Drawing_Rect* rect = OH_Drawing_RectCreate(x1, y1, x2, y2);
        OH_Drawing_CanvasDrawRect(canvas, rect);
        OH_Drawing_CanvasTranslate(canvas, x1, y1);
        OH_Drawing_CanvasDrawRect(canvas, rect);
        OH_Drawing_CanvasRestoreToCount(canvas, fSave_Count);
        OH_Drawing_RectDestroy(rect);
    }
}

void CanvasDrawPoints::OnTestPerformance(OH_Drawing_Canvas* canvas) {
    OH_Drawing_Point2D pts[N];
    TestRend rend = TestRend();
    
    for (int i = 0; i < testCount_; i++) {
        float x = TestRend().nextUScalar1() * 640;
        float y = TestRend().nextUScalar1() * 480;
        pts[i].x = x;
        pts[i].y = y;
        OH_Drawing_CanvasDrawPoints(canvas, OH_Drawing_PointMode::POINT_MODE_POLYGON, N, pts);
    }
}

void CanvasDrawLine::OnTestPerformance(OH_Drawing_Canvas* canvas) {
    TestRend rand;
    
    for (int i = 0; i < testCount_; i++) {
        float x1 = rand.nextULessThan(bitmapWidth_);
        float y1 = rand.nextULessThan(bitmapHeight_);
        float x2 = rand.nextULessThan(bitmapWidth_);
        float y2 = rand.nextULessThan(bitmapHeight_);
        OH_Drawing_CanvasDrawLine(canvas, x1, y1, x2, y2);
    }
}

void CanvasDrawLayer::OnTestPerformance(OH_Drawing_Canvas* canvas) {
    TestRend rand;
    
    uint32_t colors[] = {0xFFFF0000, 0xFF00FF00, 0xFF0000FF};
    for (int i = 0; i < testCount_; i++) {
        float x1 = rand.nextULessThan(bitmapWidth_);
        float y1 = rand.nextULessThan(bitmapHeight_);
        float x2 = rand.nextULessThan(bitmapWidth_);
        float y2 = rand.nextULessThan(bitmapHeight_);
        OH_Drawing_Rect* bounds = OH_Drawing_RectCreate(x1, y1, x2, y2);
        OH_Drawing_CanvasSave(canvas);
        OH_Drawing_CanvasSaveLayer(canvas, bounds, styleBrush_);
        OH_Drawing_CanvasClear(canvas, colors[i]);
        OH_Drawing_CanvasRestore(canvas);
        OH_Drawing_RectDestroy(bounds);
    }
    
}

void CanvasDrawBitmap::OnTestPerformance(OH_Drawing_Canvas* canvas) {
    OH_Drawing_Bitmap *bitmap = OH_Drawing_BitmapCreate();
    OH_Drawing_BitmapFormat cFormat{COLOR_FORMAT_BGRA_8888, ALPHA_FORMAT_OPAQUE};
    OH_Drawing_BitmapBuild(bitmap, 200, 200, &cFormat);
    OH_Drawing_Canvas *bitmapCanvas = OH_Drawing_CanvasCreate();
    OH_Drawing_CanvasBind(bitmapCanvas, bitmap);
    OH_Drawing_CanvasClear(bitmapCanvas, OH_Drawing_ColorSetArgb(0xff, 0x00, 0xff, 0x00));
    TestRend rand;
    
    for (int i = 0; i < testCount_; i++) {
        float x = rand.nextULessThan(bitmapWidth_);
        float y = rand.nextULessThan(bitmapHeight_);
        OH_Drawing_CanvasDrawBitmap(canvas, bitmap, x, y);
        
    }
    OH_Drawing_CanvasDestroy(bitmapCanvas);
    OH_Drawing_BitmapDestroy(bitmap);
}

void CanvasDrawBackground::OnTestPerformance(OH_Drawing_Canvas *canvas) {
    
    OH_Drawing_Matrix *mat = OH_Drawing_MatrixCreate();
    OH_Drawing_MatrixSetMatrix(mat, 1, 0, 0,
                                   0, 1, 0,
                                   0, 1.0 / 1000, 1);
        for (int i = 0; i < testCount_; i++) {
            OH_Drawing_CanvasConcatMatrix(canvas, mat);
            OH_Drawing_CanvasDrawBackground(canvas, styleBrush_);
            OH_Drawing_CanvasRestore(canvas);
        }
        OH_Drawing_MatrixDestroy(mat);
}

void CanvasDrawRegion::OnTestPerformance(OH_Drawing_Canvas *canvas) {
    TestRend rand;
    
    for (int i = 0; i < testCount_; i++) {
        float x1 = rand.nextULessThan(bitmapWidth_);
        float y1 = rand.nextULessThan(bitmapHeight_);
        float x2 = rand.nextULessThan(bitmapWidth_);
        float y2 = rand.nextULessThan(bitmapHeight_);
        OH_Drawing_Rect * rect = OH_Drawing_RectCreate(x1, y1, x2, y2);
        OH_Drawing_Region * region = OH_Drawing_RegionCreate();
        OH_Drawing_RegionSetRect(region, rect);
        OH_Drawing_CanvasDrawRegion(canvas, region);
        OH_Drawing_RegionDestroy(region);
        OH_Drawing_RectDestroy(rect);
    }
    
}

 void CanvasDrawPixelMapRect::OnTestPerformance(OH_Drawing_Canvas *canvas) {

     OH_Drawing_Rect * rect = OH_Drawing_RectCreate(0, 0, 640, 640);

    void *bitmapAddr = OH_Drawing_BitmapGetPixels(bitmap_);
//    OH_PixelMap_CreatePixelMap(napi_env env, OhosPixelMapCreateOps info, void *buf, size_t len, napi_value *res)

//    int32_t res = OH_PixelMap_CreatePixelMap(env, createOps, (uint8_t *)bitmapAddr, bufferSize, &pixelMap);
//     OH_Drawing_SamplingOptions * sampling = OH_Drawing_SamplingOptionsCreate(OH_Drawing_FilterMode::FILTER_MODE_NEAREST, OH_Drawing_MipmapMode::MIPMAP_MODE_NEAREST);
//     for (int i = 0; i < testCount_; i++) {
//             OH_Drawing_CanvasDrawPixelMapRect(canvas, OH_Drawing_PixelMap *, nullptr, rect, sampling);
//     }
 }

void CanvasSave::OnTestPerformance(OH_Drawing_Canvas *canvas) {
    TestRend rand;
    OH_Drawing_Path* Path = OH_Drawing_PathCreate();
    OH_Drawing_PathMoveTo(Path, 0, 0);
    
    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_CanvasSave(canvas);
        float x = rand.nextULessThan(bitmapWidth_);
        float y = rand.nextULessThan(bitmapHeight_);
        OH_Drawing_CanvasDrawLine(canvas, x, y, x-y, y-x);
        OH_Drawing_CanvasTranslate(canvas, y, x);
    }
    OH_Drawing_PathDestroy(Path);
}
void CanvasClipPath::OnTestPerformance(OH_Drawing_Canvas *canvas) {
    TestRend rand;

    for (int i = 0; i < testCount_; i++) {
        OH_Drawing_Path *path = OH_Drawing_PathCreate();
        float x = rand.nextULessThan(bitmapWidth_);
        float y = rand.nextULessThan(bitmapHeight_);
        OH_Drawing_PathLineTo(path, x, y);

        OH_Drawing_CanvasDrawPath(canvas, path);

        OH_Drawing_CanvasClipPath(canvas, path, OH_Drawing_CanvasClipOp::INTERSECT, true);

        OH_Drawing_PathDestroy(path);
    }
}
