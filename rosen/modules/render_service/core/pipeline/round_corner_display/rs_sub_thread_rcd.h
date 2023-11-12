#ifndef RENDER_SERVICE_CORE_PIPELINE_RCD_RENDER_RS_SUB_THREAD_H
#define RENDER_SERVICE_CORE_PIPELINE_RCD_RENDER_RS_SUB_THREAD_H
#pragma once
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include "render_context/render_context.h"
#include "event_handler.h"

namespace OHOS {
namespace Rosen {
class RSSubThreadRCD {
public:
    RSSubThreadRCD(){};
    virtual ~RSSubThreadRCD();
private:
    std::shared_ptr<AppExecFwk::EventRunner> runner_ = nullptr;
    std::shared_ptr<AppExecFwk::EventHandler> handler_ = nullptr;
    RenderContext *renderContext_ = nullptr;
public:
    void Start(RenderContext *context);
    void PostTask(const std::function<void()>& task);

};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_CORE_PIPELINE_RCD_RENDER_RS_SUB_THREAD_H