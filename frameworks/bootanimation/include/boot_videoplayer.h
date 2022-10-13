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

#ifndef FRAMEWORKS_BOOTANIMATION_INCLUDE_BOOT_VIDEOPLAYER_H
#define FRAMEWORKS_BOOTANIMATION_INCLUDE_BOOT_VIDEOPLAYER_H

#include <media_errors.h>
#include <foundation/window/window_manager/interfaces/innerkits/wm/window_manager.h>
#include <parameters.h>
#include <ui/rs_surface_extractor.h>
#include <window_scene.h>
#include "event_handler.h"
#include "player.h"

namespace OHOS {
class BootAnimation;
class BootVideoPlayer {
public:
    void SetParent(BootAnimation* parent);
    void SetVideoPath(const std::string& path);
    void SetPlayerWindow(const OHOS::sptr<OHOS::Rosen::Window>& window);
    void PlayVideo();
    void StopVideo();
private:
    std::shared_ptr<Media::Player> videoPlayer_;
    OHOS::sptr<OHOS::Rosen::Window> window_;
    std::string videopath_;
    BootAnimation* parent_;

    friend class VideoPlayerCallback;
};

class VideoPlayerCallback : public Media::PlayerCallback, public NoCopyable {
public:
    VideoPlayerCallback(BootVideoPlayer* boot) {
        boot_ = boot;
    };
    virtual ~VideoPlayerCallback() = default;

    void OnError(Media::PlayerErrorType errorType, int32_t errorCode) override;
    void OnInfo(Media::PlayerOnInfoType type, int32_t extra, const Media::Format &infoBody) override;
private:
    BootVideoPlayer* boot_;
};
} // namespace OHOS

#endif // FRAMEWORKS_BOOTANIMATION_INCLUDE_BOOT_VIDEOPLAYER_H
