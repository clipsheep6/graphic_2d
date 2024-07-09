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

#ifndef C_INCLUDE_DRAWING_PIXMAP_H
#define C_INCLUDE_DRAWING_PIXMAP_H

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

#include "drawing_error_code.h"
#include "drawing_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Creates an <b>OH_Drawing_Pixmap</b> object.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @return Returns the pointer to the <b>OH_Drawing_Pixmap</b> object created.
 * @since 12
 * @version 1.0
 */
OH_Drawing_Pixmap* OH_Drawing_PixmapCreate(void);

/**
 * @brief Destroys an <b>OH_Drawing_Pixmap</b> object and reclaims the memory occupied by the object.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Pixmap Indicates the pointer to an <b>OH_Drawing_Pixmap</b> object.
 * @since 12
 * @version 1.0
 */
void OH_Drawing_PixmapDestroy(OH_Drawing_Pixmap*);

/**
 * @brief Obtains the width of a pixmap.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Pixmap Indicates the pointer to an <b>OH_Drawing_Pixmap</b> object.
 * @param width Indicates the width of a pixmap.
 * @return Returns the error code.
 *         Returns {@link OH_DRAWING_SUCCESS} if the operation is successful.
 *         Returns {@link OH_DRAWING_ERROR_INVALID_PARAMETER} if cPixmap or width is nullptr.
 * @since 12
 * @version 1.0
 */
OH_Drawing_ErrorCode OH_Drawing_PixmapGetWidth(OH_Drawing_Pixmap* cPixmap, uint32_t* width);

/**
 * @brief Obtains the height of a pixmap.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_Pixmap Indicates the pointer to an <b>OH_Drawing_Pixmap</b> object.
 * @param height Indicates the height of a pixmap.
 * @return Returns the error code.
 *         Returns {@link OH_DRAWING_SUCCESS} if the operation is successful.
 *         Returns {@link OH_DRAWING_ERROR_INVALID_PARAMETER} if cPixmap or height is nullptr.
 * @since 12
 * @version 1.0
 */
OH_Drawing_ErrorCode OH_Drawing_PixmapGetHeight(OH_Drawing_Pixmap* cPixmap, uint32_t* height);

#ifdef __cplusplus
}
#endif
/** @} */
#endif