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

#ifndef FRAMEWORKS_CURSOR_INCLUDE_CURSOR_H
#define FRAMEWORKS_CURSOR_INCLUDE_CURSOR_H

#include <functional>
#include <unistd.h>
#include <vector>
#include <touch_event_handler.h>
#include <ipc_object_stub.h>
#include <window_manager.h>
#include <refbase.h>
#include <raw_parser.h>
#include <cursor_log.h>


namespace OHOS {
class Cursor : public MMI::TouchEventHandler{
public:
    int Init(std::string fileName);
    bool OnTouch(const TouchEvent &event) override;
    void draw(void *vaddr, uint32_t width, uint32_t height, uint32_t count);
    int ListenMouse();
private:
    sptr<Window> window = nullptr;
    sptr<IRemoteObject> token_ = nullptr;
    int32_t width = 200;
    int32_t height = 200;
    int32_t x = 200;
    int32_t y = 200;
    int downX = 0;
    int downY = 0;
    std::vector<struct WMDisplayInfo> displays;
    RawParser resource;
};
} // namespace OHOS
#endif // FRAMEWORKS_CURSOR_INCLUDE_CURSOR_H


