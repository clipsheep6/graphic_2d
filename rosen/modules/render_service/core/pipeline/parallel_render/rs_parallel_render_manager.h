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
 * WITHOUT WARRaANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef RS_PARALLEL_RENDER_MANAGER_H
#define RS_PARALLEL_RENDER_MANAGER_H

#include <condition_variable>
#include <memory>
#include <mutex>
#include "EGL/egl.h"
#include "rs_parallel_sub_thread.h"
#include "rs_parallel_pack_visitor.h"
#include "rs_parallel_task_manager.h"
#include "pipeline/parallel_render/rs_render_task.h"
#include "pipeline/rs_base_render_engine.h"
#include "pipeline/rs_base_render_node.h"

namespace OHOS {
namespace Rosen {
class RSUniRenderVisitor;
class RSSurfaceRenderNode;
class RSDisplayRenderNode;

enum class ParallelRenderType {
    DRAW_IMAGE = 0;
    FLUSH_ONE_BUFFER
};

enum class ParallelStatus {
    OFF = 0,
    ON,
    FIRSTFLUSH,
    WAITFIRSTFLUSH
};

class RSParallelRenderManager {
public:
    static RSParallelRenderManager *Instance();
    void StartSubRenderThread(uint32_t threadNum, RenderContext *context);
    void EndSubRenderThread();
    ParallelStatus isParallelRendering();
    void CopyVisitorAndPackTask(RSUniRenderVisitor &visitor, RSDisplayRenderNode &node);
    void PackRenderTask(RSSurfaceRenderNode &node);
    void LoadBalanceAndNotify();
    void UpdataTaskEvalCost();
    void MergeRenderResult(std::shared_ptr<SkCanvas> canvas);
    void SetFrameSize(int height, int width);
    void GetFrameSize(int &height, int &width);
    void SubmitSuperTask(int taskIndex, std::unique_ptr<RSSuperRenderTask> superRenderTask);
    void WaitTaskReady();
    void WaitRenderFinish();
    void SetRenderTaskCost(uint32_t subMainThreadIdx, uint64_t loadId, float cost);
    bool ParallelRenderExtEnable();
    void TryEnableParellerRendering(ParallelRenderingType parallelEnableOption);
    bool ReadySubThreadNumIncrement();

private:
    RSParallelRenderManager() = default;
    ~RSParallelRenderManager() = default;
    RSParallelRenderManager(const RSParelleRenderManager &) = delete;
    RSParallelRenderManager(const RSParelleRenderManager &&) = delete;
    RSParallelRenderManager &operator = (const RSParallelRenderManager &) = delete;
    RSParallelRenderManager &operator = (const RSParallelRenderManager &&) = delete;

    std::shared_ptr<RSParallelPackVisitor> packVisitor_;
    std::vector<std::unique_ptr<RSParallelSubThread>> threadList_;
    RSParallelRenderManager taskManager_;
    int height_;
    int width_;
    std::mutex taskMutex_;
    std::condition_variable cvTask_;
    RenderContext *renderContext_;
    ParallelRenderType renderType_ = ParallelRenderType::DRAW_IMAGE;
    std::shared_ptr<RSBaseRenderNode> displayNode_ = 0;
    bool isTaskReady_ = false;
    uint32_t expectedSubThreaNum_= 0;
    std::atomic<uint32_t> readySubThreadNum_ = 0;
    bool firstFlush_ = false;
};
} // namespace Rosen
} // namespace OHOS
#endif // RS_PARALLEL_RENDER_MANAGER_H