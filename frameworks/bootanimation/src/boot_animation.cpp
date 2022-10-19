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
#include "boot_animation.h"
#include "event_handler.h"
#include "rs_trace.h"
#include "transaction/rs_render_service_client.h"
#include "transaction/rs_interfaces.h"
#include "config_policy_utils.h"

using namespace OHOS;
static const std::string BOOT_CUSTOM_PATHSUFFIX = "etc/bootanimation/bootanimation_custom_config.json";
static const std::string BOOT_PIC_ZIP = "/system/etc/init/bootpic.zip";
static const std::string BOOT_SOUND_URI = "file://system/etc/init/bootsound.wav";


void BootAnimation::OnDraw(SkCanvas* canvas, int32_t curNo)
{
    if (canvas == nullptr) {
        LOGE("OnDraw canvas is nullptr");
        return;
    }
    if (curNo > (imgVecSize_ - 1) || curNo < 0) {
        return;
    }
    std::shared_ptr<ImageStruct> imgstruct = imageVector_[curNo];
    sk_sp<SkImage> image = imgstruct->imageData;

    ROSEN_TRACE_BEGIN(HITRACE_TAG_GRAPHIC_AGP, "BootAnimation::OnDraw in drawRect");
    SkPaint backPaint;
    backPaint.setColor(SK_ColorBLACK);
    canvas->drawRect(SkRect::MakeXYWH(0.0, 0.0, windowWidth_, windowHeight_), backPaint);
    ROSEN_TRACE_END(HITRACE_TAG_GRAPHIC_AGP);
    ROSEN_TRACE_BEGIN(HITRACE_TAG_GRAPHIC_AGP, "BootAnimation::OnDraw in drawImageRect");
    SkPaint paint;
    SkRect rect;
    rect.setXYWH(pointX_, pointY_, realWidth_, realHeight_);
    canvas->drawImageRect(image.get(), rect, &paint);
    ROSEN_TRACE_END(HITRACE_TAG_GRAPHIC_AGP);
}

void BootAnimation::Draw()
{
    if (picCurNo_ < (imgVecSize_ - 1)) {
        picCurNo_ = picCurNo_ + 1;
    } else {
        CheckExitAnimation();
        return;
    }
    ROSEN_TRACE_BEGIN(HITRACE_TAG_GRAPHIC_AGP, "BootAnimation::Draw RequestFrame");
    auto frame = rsSurface_->RequestFrame(windowWidth_, windowHeight_);
    if (frame == nullptr) {
        LOGE("Draw frame is nullptr");
        return;
    }
    ROSEN_TRACE_END(HITRACE_TAG_GRAPHIC_AGP);
    framePtr_ = std::move(frame);
    auto canvas = framePtr_->GetCanvas();
    OnDraw(canvas, picCurNo_);
    ROSEN_TRACE_BEGIN(HITRACE_TAG_GRAPHIC_AGP, "BootAnimation::Draw FlushFrame");
    rsSurface_->FlushFrame(framePtr_);
    ROSEN_TRACE_END(HITRACE_TAG_GRAPHIC_AGP);
}

bool BootAnimation::CheckFrameRateValid(int32_t ratevalue)
{
    std::vector<int> freqs = {60, 30};
    int nCount = std::count(freqs.begin(), freqs.end(), ratevalue);
    if (nCount <= 0) {
        return false;
    }
    return true;
}

void BootAnimation::Init(int32_t width, int32_t height)
{
    windowWidth_ = width;
    windowHeight_ = height;
    LOGI("Init enter, width: %{public}d, height: %{public}d", width, height);

    InitPicCoordinates();
    InitBootWindow();
    if (bootVideoEnabled_) {
        LOGI("Init end");
        return;
    }

    auto& rsClient = OHOS::Rosen::RSInterfaces::GetInstance();
    while (receiver_ == nullptr) {
        receiver_ = rsClient.CreateVSyncReceiver("BootAnimation", mainHandler_);
    }
    VsyncError ret = receiver_->Init();
    if (ret) {
        LOGE("vsync receiver init failed: %{public}d", ret);
        PostTask(std::bind(&AppExecFwk::EventRunner::Stop, runner_));
        return;
    }

    InitRsSurface();
    ReadPicZipFile();

    OHOS::Rosen::VSyncReceiver::FrameCallback fcb = {
        .userData_ = this,
        .callback_ = std::bind(&BootAnimation::OnVsync, this),
    };
    int32_t changefreq = static_cast<int32_t>((1000.0 / freq_) / 16);
    ret = receiver_->SetVSyncRate(fcb, changefreq);
    if (ret) {
        PostTask(std::bind(&AppExecFwk::EventRunner::Stop, runner_));
        LOGE("SetVSyncRate failed: %{public}d %{public}d %{public}d", ret, freq_, changefreq);
        return;
    } else {
        LOGI("SetVSyncRate success: %{public}d %{public}d", freq_, changefreq);
    }
}

void BootAnimation::Run(std::vector<sptr<OHOS::Rosen::Display>>& displays)
{    
    LOGI("Run enter");
    std::string file = GetCustomCfgFile();
    if (!file.empty()) {
        ReadCustomBootConfig(file, custConfig_);
        CheckBootVideoEnabled();
    }

    runner_ = AppExecFwk::EventRunner::Create(false);
    mainHandler_ = std::make_shared<AppExecFwk::EventHandler>(runner_);
    mainHandler_->PostTask(std::bind(&BootAnimation::Init, this, displays[0]->GetWidth(), displays[0]->GetHeight()));
    if (bootVideoEnabled_) {
        mainHandler_->PostTask(std::bind(&BootAnimation::PlayVideo, this));
    } else {
        mainHandler_->PostTask(std::bind(&BootAnimation::PlaySound, this));
    }
    runner_->Run();
}

void BootAnimation::InitBootWindow()
{
    sptr<OHOS::Rosen::WindowOption> option = new OHOS::Rosen::WindowOption();
    option->SetWindowType(OHOS::Rosen::WindowType::WINDOW_TYPE_BOOT_ANIMATION);
    option->RemoveWindowFlag(OHOS::Rosen::WindowFlag::WINDOW_FLAG_NEED_AVOID);
    option->SetTouchable(false);
    if (bootVideoEnabled_) {
        option->SetWindowMode(Rosen::WindowMode::WINDOW_MODE_FLOATING);
        option->SetWindowRect({pointX_, pointY_, realWidth_, realHeight_});
    } else {
        option->SetWindowRect({0, 0, windowWidth_, windowHeight_} );
    }
    int displayId = 0;
    sptr<OHOS::Rosen::IWindowLifeCycle> listener = nullptr;
    scene_ = new OHOS::Rosen::WindowScene();
    scene_->Init(displayId, nullptr, listener, option);
    window_ = scene_->GetMainWindow();
    while (window_ == nullptr) {
        LOGI("window is nullptr, continue to init window");
        scene_->Init(displayId, nullptr, listener, option);
        window_ = scene_->GetMainWindow();
        sleep(1);
    }
    scene_->GoForeground();
}

void BootAnimation::InitRsSurface()
{
    rsSurface_ = OHOS::Rosen::RSSurfaceExtractor::ExtractRSSurface(window_->GetSurfaceNode());
    if (rsSurface_ == nullptr) {
        LOGE("rsSurface is nullptr");
        return;
    }
#ifdef ACE_ENABLE_GL
    rc_ = OHOS::Rosen::RenderContextFactory::GetInstance().CreateEngine();
    if (rc_ == nullptr) {
        LOGE("InitilizeEglContext failed");
        return;
    } else {
        LOGI("init egl context");
        rc_->InitializeEglContext();
        rsSurface_->SetRenderContext(rc_);
    }
#endif
    if (rc_ == nullptr) {
        LOGI("rc is nullptr, use cpu");
    }
}

BootAnimation::~BootAnimation()
{
    window_->Destroy();
}

void BootAnimation::InitPicCoordinates()
{
    if (windowWidth_ >= windowHeight_) {
        realHeight_ = windowHeight_;
        realWidth_ = realHeight_;
        pointX_ = (windowWidth_ - realWidth_) / 2;
    } else {
        realWidth_ = windowWidth_;
        realHeight_ = realWidth_;
        pointY_ = (windowHeight_ - realHeight_) / 2;
    }
}

void BootAnimation::OnVsync()
{
    PostTask(std::bind(&BootAnimation::Draw, this));
}

void BootAnimation::CheckExitAnimation()
{
    if (!setBootEvent_) {
        LOGI("CheckExitAnimation set bootevent parameter");
        system::SetParameter("bootevent.bootanimation.started", "true");
        setBootEvent_ = true;
    }
    std::string windowInit = system::GetParameter("bootevent.wms.fullscreen.ready", "false");
    if (windowInit == "true") {
        PostTask(std::bind(&AppExecFwk::EventRunner::Stop, runner_));
        LOGI("CheckExitAnimation read windowInit is true");
        return;
    }
}

void BootAnimation::ReadPicZipFile()
{
    ROSEN_TRACE_BEGIN(HITRACE_TAG_GRAPHIC_AGP, "BootAnimation::preload");
    BootAniConfig jsonConfig;
    std::string picZipPath = GetPicZipPath();
    ReadZipFile(picZipPath, imageVector_, jsonConfig);
    imgVecSize_ = static_cast<int32_t>(imageVector_.size());
    if (imgVecSize_ <= 0) {
        PostTask(std::bind(&AppExecFwk::EventRunner::Stop, runner_));
        LOGE("zip pic num is 0.");
        return;
    }
    SortZipFile(imageVector_);
    if (CheckFrameRateValid(jsonConfig.frameRate)) {
        freq_ = jsonConfig.frameRate;
    } else {
        LOGI("Only Support 30, 60 frame rate: %{public}d", jsonConfig.frameRate);
    }
    LOGI("end to Readzip pics freq: %{public}d totalPicNum: %{public}d", freq_, imgVecSize_);
    ROSEN_TRACE_END(HITRACE_TAG_GRAPHIC_AGP);
}

void BootAnimation::CheckBootVideoEnabled()
{
    if (custConfig_.custVideoPath.empty()) {
        LOGI("video path not config");
        return;
    }

    if (IsFileExisted(custConfig_.custVideoPath)) {
        LOGI("video enable %{public}s", custConfig_.custVideoPath.c_str());
        bootVideoEnabled_ = true;
    }
}

std::string BootAnimation::GetPicZipPath()
{
    std::string retstr = BOOT_PIC_ZIP;
    if (custConfig_.custPicZipPath.empty()) {
        LOGI("custpic path not config");
        return retstr;
    }

    if (IsFileExisted(custConfig_.custPicZipPath)) {
        LOGI("custpic path %{public}s", custConfig_.custPicZipPath.c_str());
        retstr = custConfig_.custPicZipPath;
    }
    return retstr;
}

std::string BootAnimation::GetSoundUrl()
{
    std::string retstr = BOOT_SOUND_URI;
    if (custConfig_.custSoundsPath.empty()) {
        LOGI("custsound path not config");
        return retstr;
    }

    if (IsFileExisted(custConfig_.custSoundsPath)) {
        LOGI("custsound path %{public}s", custConfig_.custSoundsPath.c_str());
        retstr = "file:/" + custConfig_.custSoundsPath;
    }
    return retstr;
}

std::string BootAnimation::GetCustomCfgFile()
{
    std::string ret = "";
    char buf[MAX_PATH_LEN] = {0};
    char *pfile = GetOneCfgFile(BOOT_CUSTOM_PATHSUFFIX.c_str(), buf, MAX_PATH_LEN);
    if (pfile != nullptr) {
        LOGI("get one filePath:%{public}s", pfile);
        return pfile;
    } else {
        LOGI("not find %{public}s", BOOT_CUSTOM_PATHSUFFIX.c_str());
    }
    return ret;
}

void BootAnimation::PlaySound()
{
    LOGI("PlaySound start");
    bool bootSoundEnabled = system::GetBoolParameter("persist.graphic.bootsound.enabled", true);
    if (bootSoundEnabled == true) {
        LOGI("PlaySound read bootSoundEnabled is true");
        if (soundPlayer_ == nullptr) {
            soundPlayer_ = Media::PlayerFactory::CreatePlayer();
        }
        std::string uri = GetSoundUrl();
        soundPlayer_->SetSource(uri);
        soundPlayer_->SetLooping(false);
        soundPlayer_->PrepareAsync();
        soundPlayer_->Play();
    }
    LOGI("PlaySound end");
}

void BootAnimation::PlayVideo()
{
    LOGI("PlayVideo start w:%{public}d h:%{public}d", windowWidth_, windowHeight_);
    if (!bootVideoEnabled_) {
        return;
    }
    videoPlayer_.SetVideoPath(custConfig_.custVideoPath);
    videoPlayer_.SetPlayerWindow(window_);
    videoPlayer_.SetParent(this);
    videoPlayer_.PlayVideo();
}

void BootAnimation::CloseVidePlayer()
{
    mainHandler_->PostTask(std::bind(&AppExecFwk::EventRunner::Stop, runner_));
}