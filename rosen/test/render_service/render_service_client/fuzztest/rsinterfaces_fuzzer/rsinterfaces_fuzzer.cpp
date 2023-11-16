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

#include "rsinterfaces_fuzzer.h"

#include <securec.h>

#include "transaction/rs_interfaces.h"

namespace OHOS {
namespace Rosen {
namespace {
const uint8_t* data_ = nullptr;
size_t size_ = 0;
size_t pos;
} // namespace

/*
 * describe: get data from outside untrusted data(data_) which size is according to sizeof(T)
 * tips: only support basic type
 */
template<class T>
T GetData()
{
    T object {};
    size_t objectSize = sizeof(object);
    if (data_ == nullptr || objectSize > size_ - pos) {
        return object;
    }
    errno_t ret = memcpy_s(&object, objectSize, data_ + pos, objectSize);
    if (ret != EOK) {
        return {};
    }
    pos += objectSize;
    return object;
}

bool RSPhysicalScreenFuzzTest(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    data_ = data;
    size_ = size;
    pos = 0;

    // get data
    uint64_t id = GetData<uint64_t>();
    uint32_t modeId = GetData<uint32_t>();
    uint32_t status = GetData<uint32_t>();
    uint32_t level = GetData<uint32_t>();
    int32_t modeIdx = GetData<uint32_t>();
    uint32_t skipFrameInterval = GetData<uint32_t>();
    uint32_t rate = GetData<uint32_t>();
    uint32_t defaultWidth = GetData<uint32_t>();
    uint32_t defaultHeight = GetData<uint32_t>();
    int32_t sceneId = GetData<uint32_t>();
    auto consumerSurface = IConsumerSurface::Create();
    auto producer = consumerSurface->GetProducer();
    auto producerSurface = Surface::CreateSurfaceAsProducer(producer);
    uint32_t windowNum = GetData<uint32_t>();

    // test
    auto& rsInterfaces = RSInterfaces::GetInstance();
    rsInterfaces.SetScreenActiveMode(static_cast<ScreenId>(id), modeId);
    rsInterfaces.SetScreenPowerStatus(static_cast<ScreenId>(id), static_cast<ScreenPowerStatus>(status));
    rsInterfaces.SetScreenBacklight(static_cast<ScreenId>(id), level);
    rsInterfaces.SetScreenColorGamut(static_cast<ScreenId>(id), modeIdx);
    ScreenGamutMap mapMode = ScreenGamutMap::GAMUT_MAP_CONSTANT;
    rsInterfaces.SetScreenGamutMap(static_cast<ScreenId>(id), mapMode);
    rsInterfaces.SetScreenSkipFrameInterval(static_cast<ScreenId>(id), skipFrameInterval);
    
    rsInterfaces.GetScreenActiveMode(static_cast<ScreenId>(id));
    rsInterfaces.GetScreenSupportedModes(static_cast<ScreenId>(id));
    rsInterfaces.GetScreenCapability(static_cast<ScreenId>(id));
    rsInterfaces.GetScreenPowerStatus(static_cast<ScreenId>(id));
    rsInterfaces.GetScreenData(static_cast<ScreenId>(id));
    rsInterfaces.GetScreenBacklight(static_cast<ScreenId>(id));
    std::vector<ScreenColorGamut> modes = { ScreenColorGamut::COLOR_GAMUT_INVALID };
    rsInterfaces.GetScreenSupportedColorGamuts(static_cast<ScreenId>(id), modes);
    std::vector<ScreenHDRMetadataKey> keys = {ScreenHDRMetadataKey::MATAKEY_RED_PRIMARY_X};
    rsInterfaces.GetScreenSupportedMetaDataKeys(static_cast<ScreenId>(id), keys);
    ScreenColorGamut mode = ScreenColorGamut::COLOR_GAMUT_INVALID;
    rsInterfaces.GetScreenColorGamut(static_cast<ScreenId>(id), mode);
    rsInterfaces.GetScreenGamutMap(static_cast<ScreenId>(id), mapMode);
    RSScreenHDRCapability screenHdrCapability;
    rsInterfaces.GetScreenHDRCapability(static_cast<ScreenId>(id), screenHdrCapability);
    RSScreenType screenType = RSScreenType::BUILT_IN_TYPE_SCREEN;
    rsInterfaces.GetScreenType(static_cast<ScreenId>(id), screenType);
    SurfaceOcclusionChangeCallback surfaceOcclusionCb = [](float) {};
    std::vector<float> partitionPoints;
    rsInterfaces.RegisterSurfaceOcclusionChangeCallback(static_cast<NodeId>(id), surfaceOcclusionCb, partitionPoints);
    rsInterfaces.UnRegisterSurfaceOcclusionChangeCallback(static_cast<NodeId>(id));

    rsInterfaces.GetDefaultScreenId();
    rsInterfaces.GetAllScreenIds();
    rsInterfaces.SetVirtualScreenResolution(static_cast<ScreenId>(id), defaultWidth, defaultHeight);
    rsInterfaces.SetVirtualScreenSurface(static_cast<ScreenId>(id), producerSurface);
    rsInterfaces.RemoveVirtualScreen(static_cast<ScreenId>(id));
    bool callbacked = false;
    ScreenId screenId = INVALID_SCREEN_ID;
    ScreenEvent screenEvent = ScreenEvent::UNKNOWN;
    auto callback = [&screenId, &screenEvent, &callbacked](ScreenId id, ScreenEvent event) {
        screenId = static_cast<ScreenId>(id);
        screenEvent = event;
        callbacked = true;
    };
    rsInterfaces.SetScreenChangeCallback(callback);
    rsInterfaces.SetScreenRefreshRate(static_cast<ScreenId>(id), sceneId, rate);
    rsInterfaces.SetRefreshRateMode(rate);
    rsInterfaces.GetScreenCurrentRefreshRate(static_cast<ScreenId>(id));
    rsInterfaces.GetCurrentRefreshRateMode();
    rsInterfaces.GetScreenSupportedRefreshRates(static_cast<ScreenId>(id));
    rsInterfaces.GetVirtualScreenResolution(static_cast<ScreenId>(id));
    OcclusionChangeCallback occlusionChangeCallback = [](std::shared_ptr<RSOcclusionData> data) {};
    rsInterfaces.RegisterOcclusionChangeCallback(occlusionChangeCallback);
    rsInterfaces.SetAppWindowNum(windowNum);
    std::shared_ptr<Media::PixelMap> watermarkImg;
    bool isShow = GetData<bool>();
    rsInterfaces.ShowWatermark(watermarkImg, isShow);
    int pid = GetData<int>();
    rsInterfaces.GetMemoryGraphic(pid);
    rsInterfaces.GetMemoryGraphics();
    float cpuMemSize = GetData<float>();
    float gpuMemSize = GetData<float>();
    rsInterfaces.GetTotalAppMemSize(cpuMemSize, gpuMemSize);
    rsInterfaces.ReportJankStats();
    DataBaseRs info;
    rsInterfaces.ReportEventResponse(info);
    rsInterfaces.ReportEventComplete(info);
    rsInterfaces.ReportEventJankFrame(info);
    HgmConfigChangeCallback HgmConfigChangeCallback = [](std::shared_ptr<RSHgmConfigData> data) {};
    rsInterfaces.RegisterHgmConfigChangeCallback(HgmConfigChangeCallback);
    rsInterfaces.EnableCacheForRotation();
    rsInterfaces.DisableCacheForRotation();
    FocusAppInfo focusAppInfo;
    rsInterfaces.SetFocusAppInfo(focusAppInfo);

    sleep(1);

    return true;
}
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::RSPhysicalScreenFuzzTest(data, size);
    return 0;
}