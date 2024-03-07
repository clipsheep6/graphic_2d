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

#ifndef RENDER_SERVICE_BASE_DRAWABLE_RS_PROPERTY_DRAWABLE_BACKGROUND_H
#define RENDER_SERVICE_BASE_DRAWABLE_RS_PROPERTY_DRAWABLE_BACKGROUND_H

#include "drawable/rs_property_drawable.h"

namespace OHOS::Rosen {
class RSProperties;
class RSFilter;
namespace Drawing {
class RuntimeEffect;
}

class RSShadowDrawable : public RSPropertyDrawable {
public:
    RSShadowDrawable(std::shared_ptr<Drawing::DrawCmdList>&& drawCmdList) : RSPropertyDrawable(std::move(drawCmdList))
    {}
    RSShadowDrawable() = default;
    static RSDrawable::Ptr OnGenerate(const RSRenderNode& node);
    bool OnUpdate(const RSRenderNode& node) override;

private:
    static void DrawColorfulShadowInner(const RSProperties& properties, Drawing::Canvas& canvas, Drawing::Path& path);
    static void DrawShadowInner(const RSProperties& properties, Drawing::Canvas& canvas, Drawing::Path& path);
};

class RSMaskDrawable : public RSPropertyDrawable {
public:
    RSMaskDrawable(std::shared_ptr<Drawing::DrawCmdList>&& drawCmdList) : RSPropertyDrawable(std::move(drawCmdList)) {}
    RSMaskDrawable() = default;
    static RSDrawable::Ptr OnGenerate(const RSRenderNode& node);
    bool OnUpdate(const RSRenderNode& node) override;

private:
};

// ============================================================================
// Background
class RSBackgroundColorDrawable : public RSPropertyDrawable {
public:
    RSBackgroundColorDrawable(std::shared_ptr<Drawing::DrawCmdList>&& drawCmdList)
        : RSPropertyDrawable(std::move(drawCmdList))
    {}
    RSBackgroundColorDrawable() = default;
    static RSDrawable::Ptr OnGenerate(const RSRenderNode& node);
    bool OnUpdate(const RSRenderNode& node) override;

private:
};

class RSBackgroundShaderDrawable : public RSPropertyDrawable {
public:
    RSBackgroundShaderDrawable(std::shared_ptr<Drawing::DrawCmdList>&& drawCmdList)
        : RSPropertyDrawable(std::move(drawCmdList))
    {}
    RSBackgroundShaderDrawable() = default;
    static RSDrawable::Ptr OnGenerate(const RSRenderNode& node);
    bool OnUpdate(const RSRenderNode& node) override;

private:
};

class RSBackgroundImageDrawable : public RSPropertyDrawable {
public:
    RSBackgroundImageDrawable(std::shared_ptr<Drawing::DrawCmdList>&& drawCmdList)
        : RSPropertyDrawable(std::move(drawCmdList))
    {}
    RSBackgroundImageDrawable() = default;
    static RSDrawable::Ptr OnGenerate(const RSRenderNode& node);
    bool OnUpdate(const RSRenderNode& node) override;

private:
};

class RSBackgroundFilterDrawable : public RSDrawable {
public:
    RSBackgroundFilterDrawable() = default;
    ~RSBackgroundFilterDrawable() override = default;

    static RSDrawable::Ptr OnGenerate(const RSRenderNode& node);
    bool OnUpdate(const RSRenderNode& node) override;
    void OnSync() override;
    Drawing::RecordingCanvas::DrawFunc CreateDrawFunc() const override;

private:
    bool needSync_ = false;
    std::shared_ptr<RSFilter> filter_;
    std::shared_ptr<RSFilter> stagingFilter_;
};

class RSDynamicLightUpDrawable : public RSPropertyDrawable {
public:
    RSDynamicLightUpDrawable(std::shared_ptr<Drawing::DrawCmdList>&& drawCmdList)
        : RSPropertyDrawable(std::move(drawCmdList))
    {}
    RSDynamicLightUpDrawable() = default;
    static RSDrawable::Ptr OnGenerate(const RSRenderNode& node);
    bool OnUpdate(const RSRenderNode& node) override;

private:
    static std::shared_ptr<Drawing::RuntimeEffect> dynamicLightUpBlenderEffect_;
    static std::shared_ptr<Drawing::Blender> MakeDynamicLightUpBlender(
        float dynamicLightUpRate, float dynamicLightUpDeg);
};

} // namespace OHOS::Rosen
#endif // RENDER_SERVICE_BASE_DRAWABLE_RS_PROPERTY_DRAWABLE_BACKGROUND_H
