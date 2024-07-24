//
// Created on 2024/7/19.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

#ifndef MYAPPLICATION_DRAWING_TYPE_H
#define MYAPPLICATION_DRAWING_TYPE_H
typedef enum  DrawingType {
    DrawingTypeAttachBoth = 0,
    DrawingTypeAttachPen,
    DrawingTypeAttachBrush,
    DrawingTypeEnd,
};
#endif //MYAPPLICATION_DRAWING_TYPE_H
