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
#include "rs_trace.h"
#include "transaction/rs_render_service_client.h"
#include "transaction/rs_interfaces.h"

using namespace OHOS;
static const std::string BOOT_PIC_ZIP = "/system/etc/init/bootpic.zip";
static const int32_t EXIT_TIME = 10 * 1000;
static const std::string BOOT_SOUND_URI = "file://system/etc/init/bootsound.wav";

void BootAnimation::OnDraw(SkCanvas* canvas)
{
    ROSEN_TRACE_BEGIN(BYTRACE_TAG_GRAPHIC_AGP, "BootAnimation::OnDraw before codec");
    if (bootPicCurNo_ < (maxPicNum_ - 1)) {
        bootPicCurNo_ = bootPicCurNo_ + 1;
    }
    ImageStruct imgstruct = bgImageVector_[bootPicCurNo_];
    sk_sp<SkImage> image = imgstruct.imageData;

    ROSEN_TRACE_END(BYTRACE_TAG_GRAPHIC_AGP);
    ROSEN_TRACE_BEGIN(BYTRACE_TAG_GRAPHIC_AGP, "BootAnimation::OnDraw in drawimage");
    SkPaint backPaint;
    backPaint.setColor(SK_ColorBLACK);
    canvas->drawRect(SkRect::MakeXYWH(0.0, 0.0, windowWidth_, windowHeight_), backPaint);
    SkPaint paint;
    SkRect rect;
    rect.setXYWH(pointX_, pointY_, realWidth_, realHeight_);
    canvas->drawImageRect(image.get(), rect, &paint);
    ROSEN_TRACE_END(BYTRACE_TAG_GRAPHIC_AGP);

    ROSEN_TRACE_BEGIN(BYTRACE_TAG_GRAPHIC_AGP, "BootAnimation::OnDraw in FlushFrame");
    rsSurface_->FlushFrame(framePtr_);
    ROSEN_TRACE_END(BYTRACE_TAG_GRAPHIC_AGP);
}

void BootAnimation::Draw()
{
    ROSEN_TRACE_BEGIN(BYTRACE_TAG_GRAPHIC_AGP, "BootAnimation::Draw");
    auto frame = rsSurface_->RequestFrame(windowWidth_, windowHeight_);
    if (frame == nullptr) {
        LOG("OnDraw frame is nullptr");
        RequestNextVsync();
        return;
    }
    framePtr_ = std::move(frame);
    auto canvas = framePtr_->GetCanvas();
    ROSEN_TRACE_END(BYTRACE_TAG_GRAPHIC_AGP);
    if (maxPicNum_ > 0) {
        OnDraw(canvas);
    }
    RequestNextVsync();
}

void BootAnimation::Init(int32_t width, int32_t height, const std::shared_ptr<AppExecFwk::EventHandler>& handler)
{
    ROSEN_TRACE_BEGIN(BYTRACE_TAG_GRAPHIC_AGP, "BootAnimation::Init");
    windowWidth_ = width;
    windowHeight_ = height;
    LOG("Init enter, width: %{public}d, height: %{public}d", width, height);

    auto& rsClient = OHOS::Rosen::RSInterfaces::GetInstance();
    while (receiver_ == nullptr) {
        receiver_ = rsClient.CreateVSyncReceiver("BootAnimation", handler);
    }
    receiver_->Init();

    InitBootWindow();
    InitRsSurface();
    InitPicCoordinates();
    LOG("begin to Readzip pics");
    int xmlfreq = 0;
    ReadzipFile(BOOT_PIC_ZIP, bgImageVector_, xmlfreq);
    if (bgImageVector_.size() > 0) {
        sort(bgImageVector_.begin(), bgImageVector_.end(), [](const ImageStruct &image1, const ImageStruct &image2) -> bool {
            return image1.fileName < image2.fileName;});
    }
    maxPicNum_ = bgImageVector_.size();
    std::vector<int> freqs = {60, 30, 120};
    int nCount = std::count(freqs.begin(), freqs.end(), xmlfreq);
    if (nCount <= 0) {
        LOG("Only Support 30, 60, 120 frame rate: %{public}d", xmlfreq);
    } else {
        freq_ = xmlfreq;
    }
    LOG("end to Readzip pics freq: %{public}d picNum: %{public}d", xmlfreq, maxPicNum_);
    ROSEN_TRACE_END(BYTRACE_TAG_GRAPHIC_AGP);

    Draw();
    PostTask(std::bind(&BootAnimation::CheckExitAnimation, this), EXIT_TIME);
}

void BootAnimation::InitBootWindow()
{
    sptr<OHOS::Rosen::WindowOption> option = new OHOS::Rosen::WindowOption();
    option->SetWindowType(OHOS::Rosen::WindowType::WINDOW_TYPE_BOOT_ANIMATION);
    option->RemoveWindowFlag(OHOS::Rosen::WindowFlag::WINDOW_FLAG_NEED_AVOID);
    option->SetWindowRect({0, 0, windowWidth_, windowHeight_} );
    int displayId = 0;
    sptr<OHOS::Rosen::IWindowLifeCycle> listener = nullptr;
    scene_ = new OHOS::Rosen::WindowScene();
    scene_->Init(displayId, nullptr, listener, option);
    window_ = scene_->GetMainWindow();
    while (window_ == nullptr) {
        LOG("window is nullptr, continue to init window");
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
        LOG("rsSurface is nullptr");
        return;
    }
#ifdef ACE_ENABLE_GL
    rc_ = OHOS::Rosen::RenderContextFactory::GetInstance().CreateEngine();
    if (rc_ == nullptr) {
        LOG("InitilizeEglContext failed");
        return;
    } else {
        LOG("init egl context");
        rc_->InitializeEglContext();
        rsSurface_->SetRenderContext(rc_);
    }
#endif
    if (rc_ == nullptr) {
        LOG("rc is nullptr, use cpu");
    }
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


void BootAnimation::RequestNextVsync()
{
    if (needCheckExit) {
        CheckExitAnimation();
    }

    OHOS::Rosen::VSyncReceiver::FrameCallback fcb = {
        .userData_ = this,
        .callback_ = std::bind(&BootAnimation::Draw, this),
    };
    receiver_->RequestNextVSync(fcb);
    receiver_->SetVSyncRate(fcb, freq_);
}

void BootAnimation::CheckExitAnimation()
{
    LOG("CheckExitAnimation enter");
    std::string windowInit = system::GetParameter("persist.window.boot.inited", "0");
    if (windowInit == "1") {
        LOG("CheckExitAnimation read windowInit is 1");
        window_->Destroy();
        exit(0);
    }
    needCheckExit = true;
}

void BootAnimation::PlaySound()
{
    LOG("PlaySound start");
    if (soundPlayer_ == nullptr) {
        soundPlayer_ = Media::PlayerFactory::CreatePlayer();
    }
    std::string uri = BOOT_SOUND_URI;
    soundPlayer_->SetSource(uri);
    soundPlayer_->SetLooping(false);
    soundPlayer_->Prepare();
    soundPlayer_->Play();
    LOG("PlaySound end");
}
