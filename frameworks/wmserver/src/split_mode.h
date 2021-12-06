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

#ifndef FRAMEWORKS_WMSERVER_SRC_SPLIT_MODE_H
#define FRAMEWORKS_WMSERVER_SRC_SPLIT_MODE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

int32_t IAnimationServiceInit();
int32_t IAnimationServiceSplitModeCreateBackground();
int32_t IAnimationServiceSplitModeCreateMiddleLine();

struct wl_client;
struct wl_resource;
struct WindowSurface;
void ControllerSetAdjacentMode(struct wl_client *client,
                               struct wl_resource *resource,
                               uint32_t type, int32_t x, int32_t y);
bool ResetAdjacentWindowProp(struct wl_resource *resource,
                             struct WindowSurface *windowSurface);

#ifdef __cplusplus
}
#endif

#endif // FRAMEWORKS_WMSERVER_SRC_SPLIT_MODE_H
