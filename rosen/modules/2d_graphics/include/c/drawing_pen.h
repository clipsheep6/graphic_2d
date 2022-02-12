/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef C_INCLUDE_DRAWING_PEN_H
#define C_INCLUDE_DRAWING_PEN_H

#include "drawing_types.h"

#ifdef __cplusplus
extern "C" {
#endif

OH_Drawing_Pen* OH_Drawing_PenCreate(void);

void OH_Drawing_PenDestory(OH_Drawing_Pen*);

bool OH_Drawing_PenIsAntiAlias(const OH_Drawing_Pen*);

void OH_Drawing_PenSetAntiAlias(OH_Drawing_Pen*, bool);

OH_Drawing_Color OH_Drawing_PenGetColor(const OH_Drawing_Pen*);

void OH_Drawing_PenSetColor(OH_Drawing_Pen*, OH_Drawing_Color);

float OH_Drawing_PenGetWidth(const OH_Drawing_Pen*);

void OH_Drawing_PenSetWidth(OH_Drawing_Pen*, float width);

float OH_Drawing_PenGetMiterLimit(const OH_Drawing_Pen*);

void OH_Drawing_PenSetMiterLimit(OH_Drawing_Pen*, float miter);

typedef enum { LINE_FLAT_CAP, LINE_SQUARE_CAP, LINE_ROUND_CAP } OH_Drawing_PenLineCapStyle;

OH_Drawing_PenLineCapStyle OH_Drawing_PenGetCap(const OH_Drawing_Pen*);

void OH_Drawing_PenSetCap(OH_Drawing_Pen*, OH_Drawing_PenLineCapStyle);

typedef enum { LINE_MITER_JOIN, LINE_ROUND_JOIN, LINE_BEVEL_JOIN } OH_Drawing_PenLineJoinStyle;

OH_Drawing_PenLineJoinStyle OH_Drawing_PenGetJoin(const OH_Drawing_Pen*);

void OH_Drawing_PenSetJoin(OH_Drawing_Pen*, OH_Drawing_PenLineJoinStyle);

#ifdef __cplusplus
}
#endif

#endif