/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef C_INCLUDE_DRAWING_DRAW_CMD_LIST_H
#define C_INCLUDE_DRAWING_DRAW_CMD_LIST_H

/**
 * @addtogroup Drawing
 * @{
 *
 * @brief Provides functions such as 2D graphics rendering, text drawing, and image display.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 *
 * @since 8
 * @version 1.0
 */

/**
 * @file drawing_picture.h
 *
 * @brief Declares functions related to the <b>picture</b> object in the drawing module.
 *
 * @library libnative_drawing.so
 * @since 12
 * @version 1.0
 */

#include "drawing_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Creates an <b>OH_Drawing_RecordingCanvas</b> object.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @return Returns the pointer to the <b>OH_Drawing_RecordingCanvas</b> object created.
 * @since 12
 * @version 1.0
 */
OH_Drawing_DrawCmdList* OH_Drawing_DrawCmdListCreate(void);

/**
 * @brief Convert picture to CMDList format.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Canvas Indicates the pointer to an <b>OH_Drawing_Canvas</b> object.
 * @since 12
 * @version 1.0
 */
void OH_Drawing_DrawPicture(OH_Drawing_Picture*);

#ifdef __cplusplus
}
#endif
/** @} */
#endif