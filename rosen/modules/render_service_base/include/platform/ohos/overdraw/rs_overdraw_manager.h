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

#ifndef RS_OVERDRAW_MANAGER
#define RS_OVERDRAW_MANAGER

#include <array>
#include <functional>

#include "common/rs_macros.h"
#include "pipeline/rs_paint_filter_canvas.h"

namespace OHOS {
namespace Rosen {

constexpr int overdrawColorArrayLength = 6;
using OverdrawColorArray = std::array<Drawing::ColorQuad, overdrawColorArrayLength>;

class RSB_EXPORT RSOverdrawManager {
public:
    static RSOverdrawManager &GetInstance();
    void StartOverDraw(std::shared_ptr<RSPaintFilterCanvas> canvas);
    void FinishOverDraw(std::shared_ptr<RSPaintFilterCanvas> canvas);
    bool IsEnabled() const;
    void SetRepaintCallback(std::function<void()> repaintCallback);

private:
    RSOverdrawManager();
    ~RSOverdrawManager() = default;
    RSOverdrawManager(RSOverdrawManager &&) = delete;
    RSOverdrawManager(const RSOverdrawManager &) = delete;
    RSOverdrawManager &operator =(RSOverdrawManager &&) = delete;
    RSOverdrawManager &operator =(const RSOverdrawManager &) = delete;

    static void OnOverdrawEnableCallback(const char *key, const char *value, void *context);
    
    bool enabled_ = false;
    std::shared_ptr<Drawing::Surface> overdrawSurface_ = nullptr;
    std::shared_ptr<Drawing::OverDrawCanvas> overdrawCanvas_ = nullptr;
    std::function<void()> repaintCallback_ = nullptr;
};
} // namespace Rosen
} // namespace OHOS

#endif // RS_OVERDRAW_MANAGER