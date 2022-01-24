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
#include "util.h"

using namespace OHOS;
static const std::string BOOT_PIC_ZIP = "/system/etc/init/bootpic.zip";
static const std::string DST_FILE_PATH = "/data/media/bootpic";
static const std::string BOOT_PIC_DIR = "/data/media/bootpic/OpenHarmony_";
static const int32_t EXIT_TIME = 10 * 1000;

void BootAnimation::OnDraw(SkCanvas* canvas)
{
    std::string imgPath = BOOT_PIC_DIR + std::to_string(bootPicCurNo_) + ".jpg";
    // pic is named from 0
    if (bootPicCurNo_ != (maxPicNum_ - 1)) {
        bootPicCurNo_ = bootPicCurNo_ + 1;
    }
    std::unique_ptr<FILE, decltype(&fclose)> file(fopen(imgPath.c_str(), "rb"), fclose);
    if (file == nullptr) {
        LOG("OnDraw file is nullptr");
        return;
    }

    auto skData = SkData::MakeFromFILE(file.get());
    if (!skData) {
        LOG("skdata memory data is null. update data failed");
        return;
    }
    auto codec = SkCodec::MakeFromData(skData);
    sk_sp<SkImage> image = SkImage::MakeFromEncoded(skData);
    SkPaint backPaint;
    backPaint.setColor(SK_ColorBLACK);
    canvas->drawRect(SkRect::MakeXYWH(0.0, 0.0, windowWidth_, windowHeight_), backPaint);
    SkPaint paint;
    SkRect rect;
    rect.setXYWH(pointX_, pointY_, realWidth_, realHeight_);
    canvas->drawImageRect(image.get(), rect, &paint);

    rsSurface_->FlushFrame(framePtr_);
}

void BootAnimation::Draw()
{
    auto frame = rsSurface_->RequestFrame(windowWidth_, windowHeight_);
    if (frame == nullptr) {
        LOG("OnDraw frame is nullptr");
        return;
    }
    framePtr_ = std::move(frame);
    auto canvas = framePtr_->GetCanvas();
    OnDraw(canvas);
    RequestNextVsync();
}

void BootAnimation::Init(int32_t width, int32_t height)
{
    windowWidth_ = width;
    windowHeight_ = height;
    LOG("Init enter, width: %{public}d, height: %{public}d", width, height);

    InitBootWindow();
    InitRsSurface();
    InitPicCoordinates();

    std::vector<uint32_t> freqs;
    VsyncHelper::Current()->GetSupportedVsyncFrequencys(freqs);
    if (freqs.size() >= 0x2) {
        freq_ = freqs[1];
    }

    LOG("ready to unzip pics");
    UnzipFile(BOOT_PIC_ZIP, DST_FILE_PATH);
    CountPicNum(DST_FILE_PATH.c_str(), maxPicNum_);
    LOG("unzip pics finish, maxPicNum: %{public}d", maxPicNum_);

    Draw();
    PostTask(std::bind(&BootAnimation::ExitAnimation, this), EXIT_TIME);
}

void BootAnimation::InitBootWindow()
{
    sptr<OHOS::Rosen::WindowOption> option = new OHOS::Rosen::WindowOption();
    option->SetWindowType(OHOS::Rosen::WindowType::SYSTEM_WINDOW_END);
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
    struct FrameCallback cb = {
        .frequency_ = freq_,
        .timestamp_ = 0,
        .userdata_ = nullptr,
        .callback_ = std::bind(&BootAnimation::Draw, this),
    };

    GSError ret = VsyncHelper::Current()->RequestFrameCallback(cb);
    if (ret) {
        LOG("RequestFrameCallback inner %{public}d", ret);
    }
}

void BootAnimation::ExitAnimation()
{
    LOG("ExitAnimation enter");
    while (true) {
        std::string windowInit = system::GetParameter("persist.window.boot.inited", "0");
        if (windowInit == "1") {
            LOG("ExitAnimation read windowInit is 1");
            break;
        }
        LOG("ExitAnimation continue check windowInit");
        sleep(1);
    }
    window_->Destroy();
    int delRet = RemoveDir(DST_FILE_PATH.c_str());
    LOG("clean resources and exit animation, delRet: %{public}d", delRet);
    exit(0);
}

