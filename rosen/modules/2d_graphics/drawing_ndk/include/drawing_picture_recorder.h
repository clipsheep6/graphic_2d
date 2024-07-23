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

#ifndef C_INCLUDE_DRAWING_PICTURE_RECORDER_H
#define C_INCLUDE_DRAWING_PICTURE_RECORDER_H

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
 * @file drawing_shadow_layer.h
 *
 * @brief Declares functions related to the <b>ShadowLayer</b> object in the drawing module.
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
 * @brief Creates an <b>OH_Drawing_PictureRecorder</b> object.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @return Returns the pointer to the <b>OH_Drawing_Picture_Recorder</b> object created.
 * @since 12
 * @version 1.0
 */
OH_Drawing_PictureRecorder* OH_Drawing_PictureRecorderCreate(void);

/**
 * @brief Begins the recording process for a drawing operation on a specified canvas.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param recorder Indicates the pointer to an <b>OH_Drawing_RecordingCanvas</b> object.
 * @param width Indicates the width to set, which is a variable.
 * @param height Indicates the height to set, which is a variable.
 * @return Returns the pointer to the <b>OH_Drawing_Canvas</b> object created.
 * @since 12
 * @version 1.0
 */
OH_Drawing_Canvas* OH_Drawing_BeginRecording(OH_Drawing_PictureRecorder* recorder,int32_t width,int32_t height);

/**
 * @brief Finish the recording process of the drawing recorder and obtain the recorded drawing object.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_PictureRecorder Indicates the pointer to an <b>OH_Drawing_PictureRecorder</b> object.
 * @return Returns the pointer to the <b>OH_Drawing_Picture</b> object created.
 * @since 12
 * @version 1.0
 */
OH_Drawing_Picture* OH_Drawing_FinishingRecording(OH_Drawing_PictureRecorder* recorder);


/**
 * @brief Destroys an <b>OH_Drawing_PictureRecorderDestroy</b> object .
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeDrawing
 * @param OH_Drawing_PictureRecorder Indicates the pointer to an <b>OH_Drawing_PictureRecorder</b> object.
 * @since 12
 * @version 1.0
 */
void OH_Drawing_PictureRecorderDestroy(OH_Drawing_PictureRecorder* recorder );


#ifdef __cplusplus
}
#endif
/** @} */
#endif