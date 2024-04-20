#ifndef TEXT_BLOB_TEST_H
#define TEXT_BLOB_TEST_H
#include "../test_base.h"
#include <bits/alltypes.h>
#include <native_drawing/drawing_rect.h>
#include <native_drawing/drawing_canvas.h>

class TextBlobBuilderCreate : public TestBase {
public:
    TextBlobBuilderCreate(int type):TestBase(type) {
        fileName_ = "TextBlobBuilderCreate";
    };
    ~TextBlobBuilderCreate() = default;

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;//614 ms 1000次
};

class TextBlobCreateFromText : public TestBase {
public:
    TextBlobCreateFromText(int type):TestBase(type) {
        fileName_ = "TextBlobCreateFromText";
    };
    ~TextBlobCreateFromText() = default;

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;//440 ms 100次
};

class TextBlobCreateFromPosText : public TestBase {
public:
    TextBlobCreateFromPosText(int type):TestBase(type) {
        fileName_ = "TextBlobCreateFromPosText";
    };
    ~TextBlobCreateFromPosText() = default;

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;//230 ms 100次
};

class TextBlobCreateFromString : public TestBase {
public:
    TextBlobCreateFromString(int type):TestBase(type) {
        fileName_ = "TextBlobCreateFromString";
    };
    ~TextBlobCreateFromString() = default;

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;//486 ms 100次
};

class TextBlobGetBounds : public TestBase {
public:
    TextBlobGetBounds(int type):TestBase(type) {
        fileName_ = "TextBlobGetBounds";
    };
    ~TextBlobGetBounds() = default;

protected:
    void OnTestPerformance(OH_Drawing_Canvas* canvas) override;//365 ms 100次
};
#endif // TEXT_BLOB_CREATE_FROM_TEXT_H