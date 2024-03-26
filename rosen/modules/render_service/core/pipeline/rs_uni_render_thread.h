/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_PIPELINE_RS_UNI_RENDER_THREAD_H
#define RENDER_SERVICE_PIPELINE_RS_UNI_RENDER_THREAD_H

#include "common/rs_thread_handler.h"
#include "common/rs_thread_looper.h"
#include "pipeline/rs_base_render_engine.h"
#include "pipeline/rs_context.h"

namespace OHOS {
namespace Rosen {
class RSUniRenderThread {
public:
    static RSUniRenderThread* Instance();

    void Init();
    void Start();
    void InitGrContext();
    void RenderFrames();
    void Sync(std::list<std::shared_ptr<RSRenderNode>> nodes);
    void PostTask(const std::function<void()>& task);
    void Render();
private:
    RSUniRenderThread();
    ~RSUniRenderThread() noexcept;
    RSUniRenderThread(const RSUniRenderThread&) = delete;
    RSUniRenderThread(const RSUniRenderThread&&) = delete;
    RSUniRenderThread& operator=(const RSUniRenderThread&) = delete;
    RSUniRenderThread& operator=(const RSUniRenderThread&&) = delete;

    std::shared_ptr<AppExecFwk::EventRunner> runner_ = nullptr;
    std::shared_ptr<AppExecFwk::EventHandler> handler_ = nullptr;
    
    std::shared_ptr<RSBaseRenderEngine> uniRenderEngine_;
    std::shared_ptr<RSContext> context_;
};
} // namespace OHOS
} // namespace Rosen
#endif // RENDER_SERVICE_PIPELINE_RS_UNI_RENDER_THREAD_H
