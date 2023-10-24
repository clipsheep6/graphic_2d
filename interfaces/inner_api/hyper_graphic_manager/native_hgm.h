/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef NDK_INCLUDE_NATIVE_HGM_H_
#define NDK_INCLUDE_NATIVE_HGM_H_

/**
 * @addtogroup NativeHgm
 * @{
 *
 * @brief Provides the native hyper_graphic_manager capability.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeHgm
 * @since 10
 * @version 1.0
 */

/**
 * @file native_hgm.h
 *
 * @brief Defines the functions for obtaining and using a native hyper_graphic_manager.
 *
 * @library libnative_hgm.so
 * @since 10
 * @version 1.0
 */
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Creates a <b>NativeHgm</b> instance.\n
 * A new <b>NativeHgm</b> instance is created each time this function is called.
 *
 * @syscap SystemCapability.Graphic.Graphic2D.NativeHgm
 * @param name Indicates the hyper_graphic_manager connection name.
 * @param length Indicates the name's length.
 * @return Returns int32_t, return value == 0, success, otherwise, failed.
 * @since 10
 * @version 1.0
 */
int OH_NativeHgm_GetScreenCurrentRefreshRate(int32_t screen);
#ifdef __cplusplus
}
#endif

#endif