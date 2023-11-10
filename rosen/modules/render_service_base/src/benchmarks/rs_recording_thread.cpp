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

#include "benchmarks/rs_recording_thread.h"

#include <thread>
#include "common/rs_thread_handler.h"
#include "platform/common/rs_system_properties.h"
#include "platform/common/rs_log.h"
#include "rs_trace.h"
#include "transaction/rs_marshalling_helper.h"

namespace OHOS::Rosen {
RSRecordingThread::~RSRecordingThread()
{
    RS_LOGI("RSRecordingThread::~RSRecordingThread()");
    PostTask([this]() {
        DestroyShareEglContext();
    });
}

RSRecordingThread &RSRecordingThread::Instance(RenderContext* context)
{
    static RSRecordingThread instance(context);
    return instance;
}

void RSRecordingThread::Start()
{
    runner_ = AppExecFwk::EventRunner::Create("RRecordingThread");
    handler_ = std::make_shared<AppExecFwk::EventHandler>(runner_);
    PostTask([this]() {
        grContext_ = CreateShareGrContext();
    });
}

#ifndef USE_ROSEN_DRAWING
#ifdef NEW_SKIA
sk_sp<GrDirectContext> RSRecordingThread::CreateShareGrContext()
#else
sk_sp<GrContext> RSRecordingThread::CreateShareGrContext()
#endif
{
    RS_TRACE_NAME("RSRecordingThread::CreateShareGrContext");
    CreateShareEglContext();
    const GrGLInterface *grGlInterface = GrGLCreateNativeInterface();
    sk_sp<const GrGLInterface> glInterface(grGlInterface);
    if (glInterface.get() == nullptr) {
        RS_LOGE("CreateShareGrContext failed");
        return nullptr;
    }

    GrContextOptions options = {};
    options.fGpuPathRenderers &= ~GpuPathRenderers::kCoverageCounting;
    options.fPreferExternalImagesOverES3 = true;
    options.fDisableDistanceFieldPaths = true;

    auto handler = std::make_shared<MemoryHandler>();
    auto glesVersion = reinterpret_cast<const char*>(glGetString(GL_VERSION));
    auto size = glesVersion ? strlen(glesVersion) : 0;
    /* /data/service/el0/render_service is shader cache dir*/
    handler->ConfigureContext(&options, glesVersion, size, "/data/service/el0/render_service", true);

#ifdef NEW_SKIA
    sk_sp<GrDirectContext> grContext = GrDirectContext::MakeGL(std::move(glInterface), options);
#else
    sk_sp<GrContext> grContext = GrContext::MakeGL(std::move(glInterface), options);
#endif
    if (grContext == nullptr) {
        RS_LOGE("nullptr grContext is null");
        return nullptr;
    }
    return grContext;
}
#else
std::shared_ptr<Drawing::GPUContext> RSRecordingThread::CreateShareGrContext()
{
    RS_TRACE_NAME("CreateShareGrContext");
    CreateShareEglContext();
    auto gpuContext = std::make_shared<Drawing::GPUContext>();
    Drawing::GPUContextOptions options;
    auto handler = std::make_shared<MemoryHandler>();
    auto glesVersion = reinterpret_cast<const char*>(glGetString(GL_VERSION));
    auto size = glesVersion ? strlen(glesVersion) : 0;
    /* /data/service/el0/render_service is shader cache dir*/
    handler->ConfigureContext(&options, glesVersion, size, "/data/service/el0/render_service", true);

    if (!gpuContext->BuildFromGL(options)) {
        RS_LOGE("nullptr gpuContext is null");
        return nullptr;
    }
    return gpuContext;
}
#endif

void RSRecordingThread::CreateShareEglContext()
{
#ifdef RS_ENABLE_GL
    if (renderContext_ == nullptr) {
        RS_LOGE("renderContext_ is nullptr");
        return;
    }
    eglShareContext_ = renderContext_->CreateShareContext();
    if (eglShareContext_ == EGL_NO_CONTEXT) {
        RS_LOGE("eglShareContext_ is EGL_NO_CONTEXT");
        return;
    }
    if (!eglMakeCurrent(renderContext_->GetEGLDisplay(), EGL_NO_SURFACE, EGL_NO_SURFACE, eglShareContext_)) {
        RS_LOGE("eglMakeCurrent failed");
        return;
    }
#endif
}

void RSRecordingThread::DestroyShareEglContext()
{
#ifdef RS_ENABLE_GL
    if (renderContext_ != nullptr) {
        eglDestroyContext(renderContext_->GetEGLDisplay(), eglShareContext_);
        eglShareContext_ = EGL_NO_CONTEXT;
        eglMakeCurrent(renderContext_->GetEGLDisplay(), EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    }
#endif
}
bool RSRecordingThread::IsIdle()
{
    return handler_ && handler_->IsIdle();
}
 
void RSRecordingThread::PostTask(const std::function<void()> &task)
{
    if (handler_) {
        handler_->PostTask(task, AppExecFwk::EventQueue::Priority::IMMEDIATE);
    }
}

bool RSRecordingThread::CheckAndRecording()
{
    if (!handler_) {
        Start();
    }
    RSTaskMessage::RSTask task = [this]() {
        std::string line = "RSRecordingThread::CheckAndRecording curDumpFrame = " + std::to_string(curDumpFrame_) +
            ", dumpFrameNum = " + std::to_string(dumpFrameNum_);
        RS_LOGD("%{public}s", line.c_str());
        mode_ = RecordingMode(RSSystemProperties::GetRecordingEnabled());
        // init curDumpFrame
        if (GetRecordingEnabled() && curDumpFrame_ == 0) {
            RS_TRACE_NAME(line);
            dumpFrameNum_ = RSSystemProperties::GetDumpFrameNum();
            fileDir_ = RSSystemProperties::GetRecordingFile();
            if (access(fileDir_.c_str(), F_OK) != 0) {
                mkdir(fileDir_.c_str(), (S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH));
            }
        }
    };
    PostTask(task);
    return GetRecordingEnabled();
}

void RSRecordingThread::FinishRecordingOneFrame()
{
    std::string line = "RSRecordingThread::FinishRecordingOneFrame curDumpFrame = " + std::to_string(curDumpFrame_) +
        ", dumpFrameNum = " + std::to_string(dumpFrameNum_);
    RS_LOGD("%{public}s", line.c_str());
    RS_TRACE_NAME(line);
    if (curDumpFrame_ < dumpFrameNum_) {
        curDumpFrame_++;
        return;
    }
    auto modeSubThread = mode_;
    mode_ = RecordingMode::STOP_RECORDING;
    RSTaskMessage::RSTask task = [this, modeSubThread]() {
        for (int curFrameIndex = 0; curFrameIndex < dumpFrameNum_; curFrameIndex++) {
            std::shared_ptr<MessageParcel> messageParcel = std::make_shared<MessageParcel>();
            std::string opsDescription = "drawing ops no description";
            if (modeSubThread == RecordingMode::HIGH_SPPED_RECORDING) {
                RS_LOGI("RSRecordingThread::High speed!");
                messageParcel->SetMaxCapacity(RECORDING_PARCEL_CAPCITY);
                RSMarshallingHelper::BeginNoSharedMem(std::this_thread::get_id());
                drawCmdListVec_[curFrameIndex]->Marshalling(*messageParcel);
                RSMarshallingHelper::EndNoSharedMem();
#ifndef USE_ROSEN_DRAWING
                opsDescription = drawCmdListVec_[curFrameIndex]-> GetOpsWithDesc();
#endif
            } else if (modeSubThread == RecordingMode::LOW_SPEED_RECORDING) {
                messageParcel = messageParcelVec_[curFrameIndex];
#ifndef USE_ROSEN_DRAWING
                opsDescription = opsDescriptionVec_[curFrameIndex];
#endif
            }
            OHOS::Rosen::Benchmarks::WriteMessageParcelToFile(messageParcel, opsDescription, curFrameIndex, fileDir_);
        }
        drawCmdListVec_.clear();
        messageParcelVec_.clear();
        opsDescriptionVec_.clear();
        curDumpFrame_ = 0;
        dumpFrameNum_ = 0;
        fileDir_ = "";
        RSSystemProperties::SetRecordingDisenabled();
        RS_LOGD("RSRecordingThread::FinishRecordingOneFrame isRecordingEnabled = false");
    };
    PostTask(task);
}

#ifndef USE_ROSEN_DRAWING
void RSRecordingThread::RecordingToFile(const std::shared_ptr<DrawCmdList>& drawCmdList)
#else
void RSRecordingThread::RecordingToFile(const std::shared_ptr<Drawing::DrawCmdList>& drawCmdList)
#endif
{
    if (curDumpFrame_ < 0) {
        return;
    }
    if (mode_ == RecordingMode::HIGH_SPPED_RECORDING) {
        drawCmdListVec_.push_back(drawCmdList);
    } else if (mode_ == RecordingMode::LOW_SPEED_RECORDING) {
#ifndef USE_ROSEN_DRAWING
        std::shared_ptr<MessageParcel> messageParcel = std::make_shared<MessageParcel>();
        messageParcel->SetMaxCapacity(RECORDING_PARCEL_CAPCITY);
        RSMarshallingHelper::BeginNoSharedMem(std::this_thread::get_id());
        drawCmdList->Marshalling(*messageParcel);
        RSMarshallingHelper::EndNoSharedMem();
        opsDescriptionVec_.push_back(drawCmdList->GetOpsWithDesc());
#else
        auto cmdListData = drawCmdList->GetData();
        auto messageParcel = std::make_shared<Drawing::Data>();
        messageParcel->BuildWithCopy(cmdListData.first, cmdListData.second);
#endif
        messageParcelVec_.push_back(messageParcel);
    }

    FinishRecordingOneFrame();
}
}
