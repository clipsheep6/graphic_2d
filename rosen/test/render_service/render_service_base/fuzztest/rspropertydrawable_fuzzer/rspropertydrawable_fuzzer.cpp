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

#include "rspropertydrawable_fuzzer.h"

#include <climits>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <hilog/log.h>
#include <memory>
#include <securec.h>
#include <unistd.h>

#include "recording/draw_cmd_list.h"
#include "common/rs_obj_geometry.h"
#include "pipeline/rs_render_node.h"
#include "property/rs_property_drawable.h"
#include "render/rs_filter.h"
#include "render/rs_shader_filter.h"
#include "render/rs_drawing_filter.h"
#include "pipeline/rs_recording_canvas.h"
namespace OHOS {
namespace Rosen {
namespace {
const uint8_t* g_data = nullptr;
size_t g_size = 0;
size_t g_pos;
int g_number0 = 0;
int g_number1 = 1;
int g_number3 = 3;
float g_zero = 0.0f;
float g_one = 1.0f;
} // namespace

template<class T>
T GetData()
{
    T object {};
    size_t objectSize = sizeof(object);
    if (g_data == nullptr || objectSize > g_size - g_pos) {
        return object;
    }
    errno_t ret = memcpy_s(&object, objectSize, g_data + g_pos, objectSize);
    if (ret != EOK) {
        return {};
    }
    g_pos += objectSize;
    return object;
}
bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    auto drawCmdList = Drawing::DrawCmdList::CreateFromData({ nullptr, g_number0 }, false);
    DrawableV2::RSPropertyDrawable rsPropertyDrawable(std::move(drawCmdList));
    rsPropertyDrawable.OnSync();

    float blurRadiusX = GetData<float>();
    float blurRadiusY = GetData<float>();

    uint64_t id = GetData<uint64_t>();
    RSRenderNode node(id);
    DrawableV2::RSFrameOffsetDrawable rsFrameOffsetDrawable(std::move(drawCmdList));
    DrawableV2::RSFrameOffsetDrawable::OnGenerate(node);
    rsFrameOffsetDrawable.OnUpdate(node);

    DrawableV2::RSClipToBoundsDrawable rsClipToBoundsDrawable(std::move(drawCmdList));
    DrawableV2::RSClipToBoundsDrawable::OnGenerate(node);
    rsClipToBoundsDrawable.OnUpdate(node);

    DrawableV2::RSClipToFrameDrawable rsClipToFrameDrawable(std::move(drawCmdList));
    DrawableV2::RSClipToFrameDrawable::OnGenerate(node);
    rsClipToFrameDrawable.OnUpdate(node);

    auto rsFilter = RSFilter::CreateBlurFilter(blurRadiusX, blurRadiusY);
    DrawableV2::RSFilterDrawable rsFilterDrawable;
    rsFilterDrawable.IsForeground();
    rsFilterDrawable.MarkFilterRegionChanged();
    rsFilterDrawable.MarkFilterRegionInteractWithDirty();
    rsFilterDrawable.MarkFilterRegionIsLargeArea();
    rsFilterDrawable.MarkFilterForceUseCache();
    rsFilterDrawable.MarkFilterForceClearCache();
    rsFilterDrawable.MarkRotationChanged();
    rsFilterDrawable.MarkNodeIsOccluded(true);
    rsFilterDrawable.IsFilterCacheValid();
    rsFilterDrawable.NeedPendingPurge();
    rsFilterDrawable.OnSync();
    rsFilterDrawable.GetFilterCachedRegion();
    return true;
}
bool DoRSPropertyDrawable(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<DrawableV2::RSPropertyDrawable> propertyDrawable =
        std::make_shared<DrawableV2::RSPropertyDrawable>();

    propertyDrawable->needSync_ = false;
    propertyDrawable->OnSync();

    propertyDrawable->needSync_ = true;
    propertyDrawable->drawCmdList_ = nullptr;
    propertyDrawable->stagingDrawCmdList_ = nullptr;
    propertyDrawable->OnSync();

    std::shared_ptr<Drawing::DrawCmdList> drawCmdList = std::make_shared<Drawing::DrawCmdList>();
    propertyDrawable->drawCmdList_ = drawCmdList;
    Drawing::Canvas canvas;
    Drawing::Rect rect(g_zero, g_zero, g_zero, g_zero);
    propertyDrawable->propertyDescription_ = "RSPropertyDrawable test";
    propertyDrawable->CreateDrawFunc()(&canvas, &rect);
    return true;
}
bool DoRSPropertyDrawCmdListUpdater(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    DrawableV2::RSPropertyDrawable targetTest1;
    std::shared_ptr<DrawableV2::RSPropertyDrawCmdListUpdater> propertyDrawCmdListUpdaterTest1 =
        std::make_shared<DrawableV2::RSPropertyDrawCmdListUpdater>(g_number0, g_number0, &targetTest1);

    std::unique_ptr<ExtendRecordingCanvas> recordingCanvas = std::make_unique<ExtendRecordingCanvas>(g_number0, g_number0);
    propertyDrawCmdListUpdaterTest1->recordingCanvas_ = std::move(recordingCanvas);
    DrawableV2::RSPropertyDrawable target;
    propertyDrawCmdListUpdaterTest1->target_ = &target;

    DrawableV2::RSPropertyDrawable targetTest2;
    std::shared_ptr<DrawableV2::RSPropertyDrawCmdListUpdater> propertyDrawCmdListUpdaterTest2 =
        std::make_shared<DrawableV2::RSPropertyDrawCmdListUpdater>(g_number0, g_number0, &targetTest2);
    propertyDrawCmdListUpdaterTest2->recordingCanvas_ = nullptr;
    return true;
}
bool DoRSFrameOffsetDrawable(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<DrawableV2::RSFrameOffsetDrawable> frameOffsetDrawable =
        std::make_shared<DrawableV2::RSFrameOffsetDrawable>();

    RSRenderNode node(g_zero);
    node.renderContent_->renderProperties_.frameOffsetX_ = g_zero;
    node.renderContent_->renderProperties_.frameOffsetY_ = g_zero;
    frameOffsetDrawable->OnGenerate(node);

    node.renderContent_->renderProperties_.frameOffsetX_ = g_one;
    frameOffsetDrawable->OnGenerate(node);
    return true;
}
bool DoRSClipToBoundsDrawable(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<DrawableV2::RSClipToBoundsDrawable> clipToBoundsDrawable =
        std::make_shared<DrawableV2::RSClipToBoundsDrawable>();

    RSRenderNode nodeTest1(g_number0);
    nodeTest1.renderContent_->renderProperties_.clipPath_ = std::make_shared<RSPath>();
    clipToBoundsDrawable->OnUpdate(nodeTest1);

    RSRenderNode nodeTest2(g_number0);
    RectT<float> rect(g_one, g_one, g_one, g_one);
    RRectT<float> rectt(rect, g_one, g_one);
    nodeTest2.renderContent_->renderProperties_.clipRRect_ = rectt;
    clipToBoundsDrawable->OnUpdate(nodeTest2);

    RSRenderNode nodeTest3(g_number0);
    nodeTest3.renderContent_->renderProperties_.cornerRadius_ = g_one;
    clipToBoundsDrawable->OnUpdate(nodeTest3);

    RSRenderNode nodeTest4(g_number0);
    clipToBoundsDrawable->OnUpdate(nodeTest4);
    return true;
}
bool DoRSClipToFrameDrawable(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<DrawableV2::RSClipToFrameDrawable> clipToFrameDrawable =
        std::make_shared<DrawableV2::RSClipToFrameDrawable>();

    RSRenderNode node(g_number0);
    node.renderContent_->renderProperties_.clipToFrame_ = false;
    clipToFrameDrawable->OnGenerate(node);

    std::shared_ptr<RSObjGeometry> geometry = std::make_shared<RSObjGeometry>();
    geometry->width_ = g_one;
    geometry->height_ = g_one;
    node.renderContent_->renderProperties_.frameGeo_ = geometry;
    node.renderContent_->renderProperties_.clipToFrame_ = true;
    clipToFrameDrawable->OnGenerate(node);
    return true;
}
bool DoRSFilterDrawable(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<DrawableV2::RSFilterDrawable> filterDrawable = std::make_shared<DrawableV2::RSFilterDrawable>();
    filterDrawable->needSync_ = true;
    filterDrawable->stagingFilter_ = nullptr;
    filterDrawable->isFilterCacheValid_ = true;
    filterDrawable->OnSync();

    std::shared_ptr<RSShaderFilter> shaderFilter = std::make_shared<RSShaderFilter>();
    shaderFilter->type_ = RSShaderFilter::LINEAR_GRADIENT_BLUR;
    filterDrawable->filter_ = std::make_shared<RSDrawingFilter>(shaderFilter);
    filterDrawable->filter_->type_ = RSFilter::LINEAR_GRADIENT_BLUR;
    std::unique_ptr<RSFilterCacheManager> cacheManager = std::make_unique<RSFilterCacheManager>();
    filterDrawable->cacheManager_ = std::move(cacheManager);
    Drawing::Canvas canvas;
    Drawing::Rect rect(g_zero, g_zero, g_one, g_one);
    filterDrawable->CreateDrawFunc()(&canvas, &rect);
    return true;
}
bool DoClearCacheIfNeededTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<DrawableV2::RSFilterDrawable> filterDrawable = std::make_shared<DrawableV2::RSFilterDrawable>();
    filterDrawable->cacheManager_ = nullptr;
    filterDrawable->ClearCacheIfNeeded();
    std::unique_ptr<RSFilterCacheManager> cacheManager = std::make_unique<RSFilterCacheManager>();
    filterDrawable->cacheManager_ = std::move(cacheManager);
    filterDrawable->isFilterCacheValid_ = true;
    filterDrawable->forceClearCacheWithLastFrame_ = true;
    filterDrawable->ClearCacheIfNeeded();
    filterDrawable->forceClearCacheWithLastFrame_ = false;
    filterDrawable->lastCacheType_ = FilterCacheType::NONE;
    filterDrawable->ClearCacheIfNeeded();
    filterDrawable->lastCacheType_ = FilterCacheType::SNAPSHOT;
    filterDrawable->forceUseCache_ = true;
    filterDrawable->ClearCacheIfNeeded();
    filterDrawable->forceUseCache_ = false;
    filterDrawable->forceClearCache_ = true;
    filterDrawable->ClearCacheIfNeeded();
    filterDrawable->forceClearCache_ = false;
    filterDrawable->filterRegionChanged_ = true;
    filterDrawable->rotationChanged_ = false;
    filterDrawable->ClearCacheIfNeeded();
    filterDrawable->filterRegionChanged_ = false;
    filterDrawable->filterInteractWithDirty_ = false;
    filterDrawable->pendingPurge_ = true;
    filterDrawable->ClearCacheIfNeeded();
    filterDrawable->filterInteractWithDirty_ = true;
    filterDrawable->cacheUpdateInterval_ = g_number0;
    filterDrawable->ClearCacheIfNeeded();
    filterDrawable->filterInteractWithDirty_ = false;
    filterDrawable->pendingPurge_ = false;
    filterDrawable->rotationChanged_ = true;
    filterDrawable->cacheUpdateInterval_ = g_number0;
    filterDrawable->ClearCacheIfNeeded();
    filterDrawable->cacheUpdateInterval_ = g_number1;
    filterDrawable->ClearCacheIfNeeded();
    return true;
}
bool DoRecordFilterInfosTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<DrawableV2::RSFilterDrawable> filterDrawable = std::make_shared<DrawableV2::RSFilterDrawable>();

    std::shared_ptr<RSDrawingFilter> rsFilter = nullptr;
    filterDrawable->RecordFilterInfos(rsFilter);
    std::shared_ptr<RSShaderFilter> shaderFilter = std::make_shared<RSShaderFilter>();
    filterDrawable->cachedFilterHash_ = g_number1;
    rsFilter = std::make_shared<RSDrawingFilter>(shaderFilter);
    filterDrawable->RecordFilterInfos(rsFilter);
    filterDrawable->ClearFilterCache();
    filterDrawable->pendingPurge_ = true;
    filterDrawable->UpdateFlags(FilterCacheType::NONE, false);
    filterDrawable->filterInteractWithDirty_ = true;
    filterDrawable->cacheUpdateInterval_ = g_number3;
    filterDrawable->UpdateFlags(FilterCacheType::NONE, true);
    filterDrawable->filterInteractWithDirty_ = false;
    filterDrawable->rotationChanged_ = true;
    filterDrawable->UpdateFlags(FilterCacheType::NONE, true);
    filterDrawable->rotationChanged_ = false;
    filterDrawable->filterType_ = RSFilter::AIBAR;
    filterDrawable->UpdateFlags(FilterCacheType::NONE, true);
    return true;
}
bool DoIsAIBarCacheValid(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    std::shared_ptr<DrawableV2::RSFilterDrawable> filterDrawable = std::make_shared<DrawableV2::RSFilterDrawable>();
    filterDrawable->IsAIBarCacheValid();
    filterDrawable->filterType_ = RSFilter::AIBAR;
    filterDrawable->IsAIBarCacheValid();
    filterDrawable->cacheUpdateInterval_ = g_number1;
    filterDrawable->forceClearCacheWithLastFrame_ = true;
    filterDrawable->IsAIBarCacheValid();
    filterDrawable->forceClearCacheWithLastFrame_ = false;
    filterDrawable->IsAIBarCacheValid();
    return true;
}
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::DoSomethingInterestingWithMyAPI(data, size);
    OHOS::Rosen::DoRSPropertyDrawable(data, size);
    OHOS::Rosen::DoRSPropertyDrawCmdListUpdater(data, size);
    OHOS::Rosen::DoRSFrameOffsetDrawable(data, size);
    OHOS::Rosen::DoRSClipToBoundsDrawable(data, size);
    OHOS::Rosen::DoRSClipToFrameDrawable(data, size);
    OHOS::Rosen::DoRSFilterDrawable(data, size);
    OHOS::Rosen::DoClearCacheIfNeededTest(data, size);
    OHOS::Rosen::DoRecordFilterInfosTest(data, size);
    OHOS::Rosen::DoIsAIBarCacheValid(data, size);
    return 0;
}
