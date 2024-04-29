#ifndef CANVAS_TEST_H
#define CANVAS_TEST_H
#include "../test_base.h"
#include <bits/alltypes.h>
#include <multimedia/player_framework/native_avscreen_capture_base.h>
#include <native_drawing/drawing_rect.h>
#include <native_drawing/drawing_canvas.h>
#include "test_common.h"
class CanvasDrawRect : public TestBase {
public:
    CanvasDrawRect(int type):TestBase(type) {};
    ~CanvasDrawRect() = default;

protected:
    int rectWidth_ = 0;
    int rectHeight_ = 0;
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

//林江涛用例
class CanvasDrawImageRect : public TestBase {
public:
    CanvasDrawImageRect(int type):TestBase(type) {};
    ~CanvasDrawImageRect() = default;

protected:
    int rectWidth_ = 0;
    int rectHeight_ = 0;
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
    private:
};

/*
 * 测试void OH_Drawing_CanvasRotate
 */
class CanvasRotate :public TestBase {
   public:   
    CanvasRotate(int type):TestBase(type) {};
    ~CanvasRotate() = default;
    protected:
    int rectWidth_ = 0;
    int rectHeight_ = 0;
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
    private:
    TestRend renderer;
};


//测试void OH_Drawing_CanvasTranslate
class CanvasTranslate : public TestBase {
public:    
    CanvasTranslate(int type):TestBase(type) {};
    ~CanvasTranslate() = default;

protected:
    int rectWidth_ = 0;
    int rectHeight_ = 0;
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
    private:
    TestRend renderer;
};

//void OH_Drawing_CanvasSkew
class CanvasScale : public TestBase {
    public:
    CanvasScale(int type) : TestBase(type){};
    ~CanvasScale() = default;
    protected:
    int rectWidth_ = 0;
    int rectHeight_ = 0;
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
    private:
    TestRend renderer;
};




class CanvasConcatMatrix : public TestBase {
    public:
    CanvasConcatMatrix(int type) : TestBase(type){};
    ~CanvasConcatMatrix() = default;
    protected:
    int rectWidth_ = 0;
    int rectHeight_ = 0;
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
    private:
    TestRend renderer;
};


//清理画布
class CanvasClear : public TestBase {
    public:
    CanvasClear(int type) : TestBase(type) {};
    ~CanvasClear() = default;
    
    protected:
    int rectWidth_ = 0;
    int rectHeight_ = 0;
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

//设置画布的矩阵状态。
class CanvasSetMatrix : public TestBase 
{
public:
    CanvasSetMatrix(int type) : TestBase(type) {};
    ~CanvasSetMatrix() = default;
protected:
    int rectWidth_ = 0;
    int rectHeight_ = 0;
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
    
};

class CanvasResetMatrix : public TestBase {
    public:
        CanvasResetMatrix(int type) : TestBase(type) {};
        ~CanvasResetMatrix() = default;
    protected:
        int rectWidth_ = 0;
        int rectHeight_ = 0;
        void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class CanvasDrawImageRectWithSrc : public TestBase{
     public:
    CanvasDrawImageRectWithSrc(int type) : TestBase(type) {};
    ~CanvasDrawImageRectWithSrc() = default;
    protected:
    int rectWidth_ = 0;
    int rectHeight_ = 0;
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

//3
class CanvasReadPixels : public TestBase {
public:
    CanvasReadPixels(int type):TestBase(type) {};
    ~CanvasReadPixels() = default;

protected:
    int rectWidth_ = 0;
    int rectHeight_ = 0;
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};
//4
class CanvasReadPixelsToBitmap : public TestBase {
public:
    CanvasReadPixelsToBitmap(int type):TestBase(type) {};
    ~CanvasReadPixelsToBitmap() = default;

protected:
    int rectWidth_ = 0;
    int rectHeight_ = 0;
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};


class CanvasDrawShadow :public TestBase{
    public:
     CanvasDrawShadow(int type):TestBase(type) {};
    ~CanvasDrawShadow() = default;
    protected:
    int rectWidth_ = 0;
    int rectHeight_ = 0;
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
private:
    TestRend renderer;
};

class CanvasSkew : public TestBase{
    public:
    CanvasSkew(int type):TestBase(type) {};
    ~CanvasSkew() = default;
    protected:
    int rectWidth_ = 0;
    int rectHeight_ = 0;
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
    private:
    TestRend renderer;
};

class CanvasDrawVertices : public TestBase{
    public:
    CanvasDrawVertices(int type):TestBase(type) {};
    ~CanvasDrawVertices() = default;
    protected:
    int rectWidth_ = 0;
    int rectHeight_ = 0;
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
    private:
    TestRend renderer;
};

class CanvasDrawPath : public TestBase {
public:
    CanvasDrawPath(int type):TestBase(type) {};
    ~CanvasDrawPath() = default;

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;    
};

class CanvasRestoreToCount : public TestBase {
public:
    CanvasRestoreToCount(int type):TestBase(type) {};
    ~CanvasRestoreToCount() = default;

protected:
    uint32_t fSave_Count = 0;
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class CanvasDrawPoints : public TestBase {
public:
    CanvasDrawPoints(int type):TestBase(type) {};
    ~CanvasDrawPoints() = default;
    
protected:
    const int N = 30;
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class CanvasDrawLine : public TestBase {
public:
    CanvasDrawLine(int type):TestBase(type) {};
    ~CanvasDrawLine() = default;

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class CanvasDrawLayer : public TestBase {
public:
    CanvasDrawLayer(int type):TestBase(type) {};
    ~CanvasDrawLayer() = default;

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class CanvasDrawBitmap : public TestBase {
public:
    CanvasDrawBitmap(int type):TestBase(type) {};
    ~CanvasDrawBitmap() = default;
    
protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;
};

class CanvasDrawBackground : public TestBase {
public:
    CanvasDrawBackground(int type):TestBase(type) {};
    ~CanvasDrawBackground() = default;
    
protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;    
};

class CanvasDrawRegion : public TestBase {
public:
    CanvasDrawRegion(int type):TestBase(type) {};
    ~CanvasDrawRegion() = default;
        
protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;    
};

class CanvasDrawPixelMapRect : public TestBase {
public:
    CanvasDrawPixelMapRect(int type):TestBase(type) {};
    ~CanvasDrawPixelMapRect() = default;
            
protected:
        void OnTestPerformance(OH_Drawing_Canvas* canvas) override;    
};

class CanvasSave : public TestBase {
      public:
        CanvasSave(int type) : TestBase(type){};
    ~CanvasSave() = default;

      protected:
        void OnTestPerformance(OH_Drawing_Canvas *canvas) override;

};

class CanvasDrawBitmapRect : public TestBase {
  public:
    CanvasDrawBitmapRect(int type) : TestBase(type) {
        fileName_ = "CanvasDrawBitmapRect";
    };
    ~CanvasDrawBitmapRect() = default;

  protected:
    void OnTestPerformance(OH_Drawing_Canvas *canvas) override; // ms
};

class CanvasDrawCircle : public TestBase {
  public:
    CanvasDrawCircle(int type) : TestBase(type) {
        fileName_ = "CanvasDrawCircle";
    };
    ~CanvasDrawCircle() = default;

  protected:
    void OnTestPerformance(OH_Drawing_Canvas *canvas) override; // ms
};

class CanvasDrawOval : public TestBase {
  public:
    CanvasDrawOval(int type) : TestBase(type) {
        fileName_ = "CanvasDrawOval";
    };
    ~CanvasDrawOval() = default;

  protected:
    void OnTestPerformance(OH_Drawing_Canvas *canvas) override; // ms
};

class CanvasDrawArc : public TestBase {
  public:
    CanvasDrawArc(int type) : TestBase(type) {
        fileName_ = "CanvasDrawArc";
    };
    ~CanvasDrawArc() = default;

  protected:
    void OnTestPerformance(OH_Drawing_Canvas *canvas) override; // ms
};

class CanvasDrawRoundRect : public TestBase {
  public:
    CanvasDrawRoundRect(int type) : TestBase(type) {
        fileName_ = "CanvasDrawRoundRect";
    };
    ~CanvasDrawRoundRect() = default;

  protected:
    void OnTestPerformance(OH_Drawing_Canvas *canvas) override; // ms
};

class CanvasDrawTextBlob : public TestBase {
  public:
    CanvasDrawTextBlob(int type) : TestBase(type) {
        fileName_ = "CanvasDrawTextBlob";
    };
    ~CanvasDrawTextBlob() = default;

  protected:
    void OnTestPerformance(OH_Drawing_Canvas *canvas) override; // ms
};

class CanvasClipRect : public TestBase {
  public:
    CanvasClipRect(int type) : TestBase(type) {
        fileName_ = "CanvasClipRect";
    };
    ~CanvasClipRect() = default;

  protected:
    void OnTestPerformance(OH_Drawing_Canvas *canvas) override; // ms
};

class CanvasClipRoundRect : public TestBase {
  public:
    CanvasClipRoundRect(int type) : TestBase(type) {
        fileName_ = "CanvasClipRoundRect";
    };
    ~CanvasClipRoundRect() = default;

  protected:
    void OnTestPerformance(OH_Drawing_Canvas *canvas) override; // ms
};

class CanvasClipPath : public TestBase {
  public:
    CanvasClipPath(int type) : TestBase(type) {
        fileName_ = "CanvasClipPath";
    };
    ~CanvasClipPath() = default;

  protected:
    void OnTestPerformance(OH_Drawing_Canvas *canvas) override; // ms
};
#endif // CANVAS_TEST_H
