/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef FRAMEWORKS_RS_TEST_ST_RS_INTERFACES_TEST_UTILS_H
#define FRAMEWORKS_RS_TEST_ST_RS_INTERFACES_TEST_UTILS_H

#include <securec.h>

#include "display_manager.h"
#include "screen_manager.h"
#include "display_property.h"
#include "display.h"
#include "screen.h"
#include "display_info.h"
#include "window.h"
#include "window_option.h"
#include "wm_common.h"
#include "dm_common.h"
#include "window_manager_hilog.h"
#include "unique_fd.h"
#include "ui/rs_display_node.h"
#include "ui/rs_root_node.h"
#include "ui/rs_surface_node.h"
#include "ui/rs_ui_director.h"

namespace OHOS {
namespace Rosen {
class RSInterfaceTestUtils {
public:
    ~RSInterfaceTestUtils();
    bool CreateSurface();
    void RootNodeInit(std::shared_ptr<RSUIDirector> rsUiDirector, int width, int height);
    sptr<Window> CreateWindowByDisplayParam(DisplayId displayId, std::string name,
        int32_t x, int32_t y, int32_t w, int32_t h);
    class BufferListener : public IBufferConsumerListener {
    public:
        explicit BufferListener(RSInterfaceTestUtils &rsInterfaceTestUtils): utils_(rsInterfaceTestUtils)
        {
        }
        ~BufferListener() noexcept override = default;
        void OnBufferAvailable() override
        {
            utils_.OnVsync();
        }

    private:
        RSInterfaceTestUtils &utils_;
    };
    friend class BufferListener;
    
    void OnVsync();
    uint32_t successCount_ = 0;
    uint32_t failCount_ = 0;
    uint32_t defaultWidth_ = 0;
    uint32_t defaultHeight_ = 0;
    sptr<IBufferConsumerListener> listener_ = nullptr;
    sptr<Surface> csurface_ = nullptr; // cosumer surface
    sptr<Surface> psurface_ = nullptr; // producer surface
    sptr<SurfaceBuffer> prevBuffer_ = nullptr;
    BufferHandle *bufferHandle_ = nullptr;
    std::shared_ptr<RSNode> rootNode_ = nullptr;

private:
    std::mutex mutex_;
};
} // namespace ROSEN
} // namespace OHOS
#endif // FRAMEWORKS_RS_TEST_ST_RS_INTERFACES_TEST_UTILS_H
