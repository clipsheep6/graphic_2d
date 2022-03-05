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

#ifndef DMS_MOCK_H
#define DMS_MOCK_H

#include <iostream>
#include <surface.h>

#include "transaction/rs_interfaces.h"
#include "ui/rs_display_node.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
using DisplayId = ScreenId;
class DmsMock {
public:
    inline static DmsMock& GetInstance()
    {
        static DmsMock c;
        return c;
    }

    virtual ~DmsMock() noexcept = default;

    DisplayId GetDefaultDisplayId()
    {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        return defaultDisplayId_;
    }

    std::optional<RSScreenModeInfo> GetDisplayActiveMode(DisplayId id) const
    {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        if (displays_.count(id) == 0) {
            std::cout << "DmsMock: No display " << id << "!" << std::endl;
            return {};
        }
        return displays_.at(id).activeMode;
    }

    void OnDisplayConnected(ScreenId id)
    {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        displays_[id] = Display { id, rsInterface_.GetScreenActiveMode(id) };
        std::cout << "DmsMock: Display " << id << " connected." << std::endl;
    }

    void OnDisplayDisConnected(ScreenId id)
    {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        if (displays_.count(id) == 0) {
            std::cout << "DmsMock: No display " << id << "!" << std::endl;
        } else {
            std::cout << "DmsMock: Display " << id << " disconnected." << std::endl;
            displays_.erase(id);
            if (id == defaultDisplayId_) {
                defaultDisplayId_ = rsInterface_.GetDefaultScreenId();
                std::cout << "DmsMock: DefaultDisplayId changed, new DefaultDisplayId is" << defaultDisplayId_ << "."
                          << std::endl;
            }
        }
    }

private:
    struct Display {
        DisplayId id;
        RSScreenModeInfo activeMode;
    };
    std::unordered_map<DisplayId, Display> displays_;
    mutable std::recursive_mutex mutex_;
    RSInterfaces& rsInterface_;
    DisplayId defaultDisplayId_;

    DmsMock() : rsInterface_(RSInterfaces::GetInstance())
    {
        Init();
    }

    void Init()
    {
        std::lock_guard<std::recursive_mutex> lock(mutex_);
        rsInterface_.SetScreenChangeCallback([this](ScreenId id, ScreenEvent event) {
            switch (event) {
                case ScreenEvent::CONNECTED: {
                    this->OnDisplayConnected(id);
                    break;
                }
                case ScreenEvent::DISCONNECTED: {
                    this->OnDisplayDisConnected(id);
                    break;
                }
                default:
                    break;
            }
        });

        defaultDisplayId_ = rsInterface_.GetDefaultScreenId();
        displays_[defaultDisplayId_] =
            Display { defaultDisplayId_, rsInterface_.GetScreenActiveMode(defaultDisplayId_) };
    }
};
} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif // DMS_MOCK_H