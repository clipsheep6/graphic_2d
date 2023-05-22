/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "memory/rs_tag_tracker.h"

#include "platform/common/rs_log.h"

namespace OHOS::Rosen {
ReleaseGpuResourceType RSTagTracker::releaseGpuResourceEnable_ = ReleaseGpuResourceType::DISABLED;
#if defined(NEW_SKIA)
RSTagTracker::RSTagTracker(GrDirectContext* grContext, RSTagTracker::TAGTYPE tagType)
#else
RSTagTracker::RSTagTracker(GrContext* grContext, RSTagTracker::TAGTYPE tagType)
#endif
    : grContext_(grContext)
{
    if (!grContext_) {
        RS_LOGE("RSTagTracker tag fail, grContext is nullptr");
        return;
    }
    if (releaseGpuResourceEnable_ == ReleaseGpuResourceType::DISABLED) {
        return;
    }
#ifdef RS_ENABLE_GL
    GrGpuResourceTag tag(0, 0, 0, tagType);
    grContext_->setCurrentGrResourceTag(tag);
#endif
}

void RSTagTracker::UpdateReleaseGpuResourceEnable(ReleaseGpuResourceType releaseResEnable)
{
    releaseGpuResourceEnable_ = releaseResEnable;
}

std::string RSTagTracker::TagType2String(TAGTYPE type)
{
    std::string tagType;
    switch(type) {
        case TAG_SAVELAYER_DRAW_NODE :
            tagType = "savelayer_draw_node";
            break;
        case TAG_RESTORELAYER_DRAW_NODE :
            tagType = "restorelayer_draw_node";
            break;
        case TAG_SAVELAYER_COLOR_FILTER :
            tagType = "savelayer_color_filter";
            break;
        case TAG_FILTER :
            tagType = "filter";
            break;
        case TAG_CAPTURE :
            tagType = "capture";
            break;
        case TAG_COLD_START :
            tagType = "cold_start";
            break;
        case TAG_ACQUIRE_SURFACE :
            tagType = "acquire_surface";
            break;
        case TAG_RENDER_FRAME :
            tagType = "render_frame";
            break;
        case TAG_DRAW_SURFACENODE :
            tagType = "draw_surface_node";
            break;
        default :
            tagType = "";
            break;
    }
    return tagType;
}

#if defined(NEW_SKIA)
RSTagTracker::RSTagTracker(GrDirectContext* grContext, NodeId nodeId, RSTagTracker::TAGTYPE tagType)
#else
RSTagTracker::RSTagTracker(GrContext* grContext, NodeId nodeId, RSTagTracker::TAGTYPE tagType)
#endif
    : grContext_(grContext)
{
    if (!grContext_) {
        RS_LOGE("RSTagTracker tag fail, grContext is nullptr");
        return;
    }
    if (releaseGpuResourceEnable_ == ReleaseGpuResourceType::DISABLED) {
        return;
    }
#ifdef RS_ENABLE_GL
    GrGpuResourceTag tag(ExtractPid(nodeId), 0, nodeId, tagType);
    grContext_->setCurrentGrResourceTag(tag);
#endif
}

#if defined(NEW_SKIA)
RSTagTracker::RSTagTracker(GrDirectContext* grContext, GrGpuResourceTag& tag)
#else
RSTagTracker::RSTagTracker(GrContext* grContext, GrGpuResourceTag& tag)
#endif
    : grContext_(grContext)
{
    if (!grContext_) {
        RS_LOGE("RSTagTracker tag fail, grContext is nullptr");
        return;
    }
    if (releaseGpuResourceEnable_ == ReleaseGpuResourceType::DISABLED) {
        return;
    }
#ifdef RS_ENABLE_GL
    grContext_->setCurrentGrResourceTag(tag);
#endif
}

void RSTagTracker::SetTagEnd()
{
    if (!grContext_) {
        RS_LOGE("RSTagTracker tag fail, grContext is nullptr");
        return;
    }
    if (releaseGpuResourceEnable_ == ReleaseGpuResourceType::DISABLED) {
        return;
    }
    isSetTagEnd_ = true;
#ifdef RS_ENABLE_GL
    GrGpuResourceTag tagEnd;
    grContext_->setCurrentGrResourceTag(tagEnd);
#endif
}

RSTagTracker::~RSTagTracker()
{
    if (releaseGpuResourceEnable_ == ReleaseGpuResourceType::DISABLED) {
        return;
    }
#ifdef RS_ENABLE_GL
    // Set empty tag to notify skia that the tag is complete
    if (!isSetTagEnd_ && grContext_) {
        GrGpuResourceTag tagEnd;
        grContext_->setCurrentGrResourceTag(tagEnd);
    }
#endif
}
} // namespace OHOS::Rosen