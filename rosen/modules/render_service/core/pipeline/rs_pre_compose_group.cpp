/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#include "pipeline/rs_pre_compose_group.h"
#include "platform/common/rs_log.h"
#include "rs_trace.h"
#include "rs_base_render_util.h"
#include "rs_main_thread.h"

namespace {
    const unsigned HISTORY_QUEUE_MAX_SIZE = 4;
}

namespace OHOS {
namespace Rosen {
RSPreComposeGroup::RSPreComposeGroup() : bufferAge_(HISTORY_QUEUE_MAX_SIZE)
{
    ROSEN_LOGD("RSPreComposeGroup()");
    regionManager_ = std::make_shared<RSPreComposeRegionManager>();
    dirtyHistory_.resize(HISTORY_QUEUE_MAX_SIZE);
}

RSPreComposeGroup::~RSPreComposeGroup()
{
    ROSEN_LOGD("~RSPreComposeGroup()");
    if (handler_) {
        auto task = [this]() {
            ROSEN_LOGD("RSPreComposeThread() Deinit Element");
            last_->Deinit();
            current_->Deinit();
            grContext_ = nullptr;
            eglShareContext_ = EGL_NO_CONTEXT;
        };
        handler_->PostSyncTask(task);
    }
    runner_->Stop();
}

void RSPreComposeGroup::UpdateLastAndCurrentVsync()
{
    if (current_->GetState() == ElementState::ELEMENT_STATE_DONE) {
        std::swap(current_, last_);
        current_->Reset();
        ROSEN_LOGD("RSPreComposeGroup swap element");
    }
    last_->StartCalculateAndDrawImage();
}

void RSPreComposeGroup::Init(ScreenInfo& info)
{
    std::string name = "RSPreComposerThread";
    runner_ = AppExecFwk::EventRunner::Create(name);
    handler_ = std::make_shared<AppExecFwk::EventHandler>(runner_);
    runner_->Run();
    current_ = std::make_shared<RSPreComposeElement>(info, elementCount_++, regionManager_);
    last_ = std::make_shared<RSPreComposeElement>(info, elementCount_++, regionManager_);
    if (handler_) {
        auto current = current_;
        auto last = last_;
        auto task = [this, current, last] () {
            ROSEN_LOGD("RSPreComposerThread Enqueue Start Task");
            CreateShareGrContext();
#if !(defined USE_ROSEN_DRAWING) && (defined NEW_SKIA)
            current->Init(grContext_.get());
            last->Init(grContext_.get());
#endif
        };
        handler_->PostTask(task);
    }
}

void RSPreComposeGroup::StartCurrentVsync(std::list<std::shared_ptr<RSSurfaceRenderNode>>& surfaceNodeList,
    std::shared_ptr<RSUniRenderVisitor> visitor, uint64_t focusNodeId, uint64_t leashFocusId)
{
    if (current_->GetState() == ElementState::ELEMENT_STATE_DOING) {
        ROSEN_LOGW("current_ vsync is update image not end");
        return;
    }
    current_->SetParams(surfaceNodeList, visitor, focusNodeId, leashFocusId);
    auto current = current_;
    if (handler_) {
        auto task = [current]() {
            ROSEN_LOGD("RSPreComposeThread Enqueue Task");
            current->UpdateDirtyRegionAndImage();
        };
        handler_->PostTask(task);
    }
}

void RSPreComposeGroup::UpdateAppWindowNodesByLastVsync(
    std::vector<std::shared_ptr<RSSurfaceRenderNode>>& appWindowNodes)
{
    last_->UpdateAppWindowNodes(appWindowNodes);
}

void RSPreComposeGroup::CreateShareEglContext()
{
#if !(defined USE_ROSEN_DRAWING) && (defined RS_ENABLE_GL)
    if (renderContext_ == nullptr) {
        auto renderEngine = RSMainThread::Instance()->GetRenderEngine();
        renderContext_ = renderEngine->GetRenderContext().get();
    }
    eglShareContext_ = renderContext_->CreateShareContext();
    if (eglShareContext_ == EGL_NO_CONTEXT) {
        ROSEN_LOGE("eglShareContext_ is EGL_NO_CONTEXT");
        return;
    }
    if (!eglMakeCurrent(renderContext_->GetEGLDisplay(), EGL_NO_SURFACE, EGL_NO_SURFACE, eglShareContext_)) {
        ROSEN_LOGE("eglMakeCurrent failed");
    }
#endif
}


void RSPreComposeGroup::CreateShareGrContext()
{
#if !(defined USE_ROSEN_DRAWING) && (defined NEW_SKIA)
    ROSEN_LOGD("CreateShareGrContext start");
    CreateShareEglContext();
    const GrGLInterface *grGlInterface = GrGLCreateNativeInterface();
    sk_sp<const GrGLInterface> glInterface(grGlInterface);
    if (glInterface.get() == nullptr) {
        ROSEN_LOGD("CreateShareGrContext failed");
        return;
    }

    GrContextOptions options = {};
    options.fGpuPathRenderers &= ~GpuPathRenderers::kCoverageCounting;
    options.fPreferExternalImagesOverES3 = true;
    options.fDisableDistanceFieldPaths = true;

    auto handler = std::make_shared<MemoryHandler>();
    auto glesVersion = reinterpret_cast<const char*>(glGetString(GL_VERSION));
    auto size = glesVersion ? strlen(glesVersion) : 0;
    handler->ConfigureContext(&options, glesVersion, size, "/data/service/el0/render_service", true);

    sk_sp<GrDirectContext> grContext = GrDirectContext::MakeGL(std::move(glInterface), options);
    if (grContext == nullptr) {
        ROSEN_LOGD("grContext is nullptr");
        return;
    }
    grContext_ = grContext;
#endif
}

void RSPreComposeGroup::UpdateNodesByLastVsync(std::vector<RSBaseRenderNode::SharedPtr>& curAllSurfaces)
{
    last_->UpdateNodes(curAllSurfaces);
}

void RSPreComposeGroup::UpdateOcclusionByLastVsync(std::shared_ptr<RSSurfaceRenderNode> surfaceNode,
    Occlusion::Region& accumulatedRegion, VisibleData& curVisVec, std::map<uint32_t, bool>& pidVisMap)
{
    last_->UpdateOcclusion(surfaceNode, accumulatedRegion, curVisVec, pidVisMap, current_);
}

bool RSPreComposeGroup::LastVsyncIsDirty()
{
    return last_->IsDirty();
}

void RSPreComposeGroup::GetLastHwcNodes(std::shared_ptr<RSSurfaceRenderNode> surfaceNode,
    std::vector<RSUniRenderVisitor::SurfaceDirtyMgrPair>& prevHwcEnabledNodes)
{
    last_->GetHwcNodes(surfaceNode, prevHwcEnabledNodes);
}

Occlusion::Region RSPreComposeGroup::GetLastVisibleDirtyRegion()
{
    return last_->GetDirtyVisibleRegion();
}

Occlusion::Region RSPreComposeGroup::GetLastVisibleDirtyRegionWithGpuNodes()
{
    auto region = last_->GetVisibleDirtyRegionWithGpuNodes();
    return MergeHistory(bufferAge_, region);
}

bool RSPreComposeGroup::ProcessLastVsyncNode(RSBaseRenderNode& node, std::shared_ptr<RSPaintFilterCanvas>& canvas,
    uint32_t threadIndex)
{
    return last_->ProcessNode(node, canvas, threadIndex);
}

void RSPreComposeGroup::SetBufferAge(int32_t bufferAge)
{
    if (bufferAge < 0) {
        ROSEN_LOGD("change error bufferAge %d to 0", bufferAge);
        bufferAge = 0;
    }
    bufferAge_ = bufferAge;
}

void RSPreComposeGroup::UpdateGlobalDirtyByLastVsync(std::shared_ptr<RSDirtyRegionManager>& dirtyManager)
{
    last_->UpdateGlobalDirty(dirtyManager);
}

Occlusion::Region RSPreComposeGroup::MergeHistory(uint32_t age, Occlusion::Region& dirtyRegion)
{
    PushHistory(dirtyRegion);
    if (age == 0 || age > HISTORY_QUEUE_MAX_SIZE) {
        return dirtyRegion;
    }
    for (uint32_t i = HISTORY_QUEUE_MAX_SIZE; i > HISTORY_QUEUE_MAX_SIZE - age; --i) {
        auto region = GetHistory((i - 1));
        if (region.IsEmpty()) {
            continue;
        }
        dirtyRegion.OrSelf(region);
    }
    return dirtyRegion;
}

void RSPreComposeGroup::PushHistory(Occlusion::Region& dirtyRegion)
{
    int32_t next = (historyHead_ + 1) % HISTORY_QUEUE_MAX_SIZE;
    dirtyHistory_[next] = dirtyRegion;
    historyHead_ = next;
}

Occlusion::Region RSPreComposeGroup::GetHistory(uint32_t i)
{
    i %= HISTORY_QUEUE_MAX_SIZE;
    i = (i + historyHead_) % HISTORY_QUEUE_MAX_SIZE;
    return dirtyHistory_[i];
}
} // namespace Rosen
} // namespace OHOS