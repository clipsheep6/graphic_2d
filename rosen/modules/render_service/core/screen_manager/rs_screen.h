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

#ifndef RS_SCREEN
#define RS_SCREEN

#include <memory>
#include <optional>

#include <display_type.h>
#include <hdi_output.h>
#include <hdi_screen.h>
#include <hilog/log.h>
#include <screen_manager/screen_types.h>

namespace OHOS {
namespace Rosen {
struct VirtualSreenConfigs {
    ScreenId id = INVALID_SCREEN_ID;
    ScreenId mirrorId = INVALID_SCREEN_ID;
    std::string name;
    uint32_t width = 0;
    uint32_t height = 0;
    sptr<Surface> surface = nullptr;
    int32_t flags = 0; // reserve flag.
};

class RSScreen {
public:
    RSScreen() = default;
    virtual ~RSScreen() noexcept = default;

    virtual ScreenId Id() const = 0;
    virtual ScreenId MirrorId() const = 0;
    virtual void SetMirror(ScreenId mirrorId) = 0;
    virtual const std::string& Name() const = 0;
    virtual uint32_t Width() const = 0;
    virtual uint32_t Height() const = 0;
    virtual bool IsEnable() const = 0;
    virtual bool IsVirtual() const = 0;
    virtual void SetActiveMode(uint32_t modeId) = 0;
    virtual void SetPowerStatus(uint32_t powerStatus) = 0;
    virtual std::optional<DisplayModeInfo> GetActiveMode() const = 0;
    virtual const std::vector<DisplayModeInfo>& GetSupportedModes() const = 0;
    virtual const DisplayCapability& GetCapability() const = 0;
    virtual uint32_t GetPowerStatus() const = 0;
    virtual std::shared_ptr<HdiOutput> GetOutput() const = 0;
    virtual sptr<Surface> GetProducerSurface() const = 0;
};

namespace impl {
class RSScreen : public OHOS::Rosen::RSScreen {
public:
    RSScreen(
        ScreenId id,
        bool isVirtual,
        std::shared_ptr<HdiOutput> output,
        sptr<Surface> surface);
    RSScreen(const VirtualSreenConfigs &configs);
    ~RSScreen() noexcept override;

    RSScreen(const RSScreen &) = delete;
    RSScreen &operator=(const RSScreen &) = delete;

    ScreenId Id() const override;
    ScreenId MirrorId() const override;
    void SetMirror(ScreenId mirrorId) override;
    const std::string& Name() const override;
    uint32_t Width() const override;
    uint32_t Height() const override;
    bool IsEnable() const override;
    bool IsVirtual() const override;
    void SetActiveMode(uint32_t modeId) override;
    void SetPowerStatus(uint32_t powerStatus) override;
    std::optional<DisplayModeInfo> GetActiveMode() const override;
    const std::vector<DisplayModeInfo>& GetSupportedModes() const override;
    const DisplayCapability& GetCapability() const override;
    uint32_t GetPowerStatus() const override;
    std::shared_ptr<HdiOutput> GetOutput() const override;
    sptr<Surface> GetProducerSurface() const override;

private:
    // TODO: fixme -- domain 0 only for debug.
    static constexpr HiviewDFX::HiLogLabel LOG_LABEL = { LOG_CORE, 0, "RSScreen" };

    // create hdiScreen and get some information from drivers.
    void PhysicalScreenInit() noexcept;

    // ScreenId for this screen.
    ScreenId id_ = INVALID_SCREEN_ID;
    // If this screen is the mirror of other screen, this member would be a valid id.
    ScreenId mirrorId_ = INVALID_SCREEN_ID;

    std::string name_;

    int32_t width_ = 0;
    int32_t height_ = 0;

    bool isVirtual_ = true;
    std::shared_ptr<HdiOutput> hdiOutput_; // has value if the screen is physical
    std::unique_ptr<HdiScreen> hdiScreen_; // has value if the screen is physical
    std::vector<DisplayModeInfo> supportedModes_;
    DisplayCapability capability_;
    sptr<Surface> producerSurface_;  // has value if the screen is virtual
};
} // namespace impl
} // namespace Rosen
} // namespace OHOS

#endif // RS_SCREEN
