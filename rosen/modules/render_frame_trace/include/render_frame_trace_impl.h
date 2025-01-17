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

#ifndef OHOS_RENDER_FRAME_TRACE_IMPL_H
#define OHOS_RENDER_FRAME_TRACE_IMPL_H

#include <string>
#include "render_frame_trace.h"

namespace FRAME_TRACE {

class RenderFrameTraceImpl : public RenderFrameTrace {
public:
    ~RenderFrameTraceImpl() override = default;
    RenderFrameTraceImpl(const RenderFrameTraceImpl& fm) = delete;
    RenderFrameTraceImpl& operator=(const RenderFrameTraceImpl& fm) = delete;
    RenderFrameTraceImpl(RenderFrameTrace&& fm) = delete;
    RenderFrameTraceImpl& operator=(RenderFrameTrace&& fm) = delete;
    void RenderStartFrameTrace(const std::string& traceTag) override;
    void RenderEndFrameTrace(const std::string& traceTag) override;
    bool RenderFrameTraceIsOpen() override;
    bool RenderFrameTraceOpen() override;
    bool RenderFrameTraceClose() override;

private:
    RenderFrameTraceImpl();
    bool AccessFrameTrace();
 
    static RenderFrameTraceImpl* instance_;
};

} // namespace FRAME_TRACE
#endif // OHOS_RENDER_FRAME_TRACE_IMPL_H
