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

#include "ui/rs_canvas_node.h"

#include <algorithm>
#include <string>

#include "common/rs_obj_abs_geometry.h"
#include "command/rs_canvas_node_command.h"
#include "command/rs_node_command.h"
#include "platform/common/rs_log.h"
#include "common/rs_obj_geometry.h"
#include "pipeline/rs_draw_cmd_list.h"
#include "pipeline/rs_node_map.h"
#include "transaction/rs_transaction_proxy.h"

namespace OHOS {
namespace Rosen {
RSCanvasNode::SharedPtr RSCanvasNode::Create(bool isRenderServiceNode)
{
    SharedPtr node(new RSCanvasNode(isRenderServiceNode));
    RSNodeMap::MutableInstance().RegisterNode(node);

    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy == nullptr) {
        return node;
    }
    std::unique_ptr<RSCommand> command = std::make_unique<RSCanvasNodeCreate>(node->GetId());
    transactionProxy->AddCommand(command, node->IsRenderServiceNode());
    return node;
}

RSCanvasNode::RSCanvasNode(bool isRenderServiceNode) : RSNode(isRenderServiceNode) {}

RSCanvasNode::~RSCanvasNode() {}

#ifndef USE_ROSEN_DRAWING
SkCanvas* RSCanvasNode::BeginRecording(int width, int height)
{
    recordingCanvas_ = new RSRecordingCanvas(width, height);
    static_cast<RSRecordingCanvas*>(recordingCanvas_)->SetIsCustomTextType(isCustomTextType_);
#else
Drawing::RecordingCanvas* RSCanvasNode::BeginRecording(int width, int height)
{
    recordingCanvas_ = new Drawing::RecordingCanvas(width, height);
#endif
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy == nullptr) {
        return recordingCanvas_;
    }
    if (!recordingUpdated_) {
        return recordingCanvas_;
    }
    recordingUpdated_ = false;
    std::unique_ptr<RSCommand> command = std::make_unique<RSCanvasNodeClearRecording>(GetId());
    transactionProxy->AddCommand(command, IsRenderServiceNode());
    return recordingCanvas_;
}

bool RSCanvasNode::IsRecording() const
{
    return recordingCanvas_ != nullptr;
}

void RSCanvasNode::FinishRecording()
{
    if (!IsRecording()) {
        return;
    }
#ifndef USE_ROSEN_DRAWING
    auto recording = static_cast<RSRecordingCanvas*>(recordingCanvas_)->GetDrawCmdList();
#else
    auto recording = recordingCanvas_->GetDrawCmdList();
#endif
    delete recordingCanvas_;
    recordingCanvas_ = nullptr;
#ifndef USE_ROSEN_DRAWING
    if (recording && recording->GetSize() == 0) {
#else
    if (recording && recording->IsEmpty()) {
#endif
        return;
    }
    if (recording != nullptr && RSSystemProperties::GetDrawTextAsBitmap()) {
        // replace drawOpItem with cached one (generated by CPU)
#ifndef USE_ROSEN_DRAWING
        recording->GenerateCache();
#endif
    }
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy == nullptr) {
        return;
    }
    std::unique_ptr<RSCommand> command = std::make_unique<RSCanvasNodeUpdateRecording>(GetId(), recording,
        drawContentLast_ ? static_cast<uint16_t>(RSModifierType::FOREGROUND_STYLE)
                         : static_cast<uint16_t>(RSModifierType::CONTENT_STYLE));
    transactionProxy->AddCommand(command, IsRenderServiceNode());
    recordingUpdated_ = true;
}

void RSCanvasNode::DrawOnNode(RSModifierType type, DrawFunc func)
{
#ifndef USE_ROSEN_DRAWING
    auto recordingCanvas = std::make_shared<RSRecordingCanvas>(GetPaintWidth(), GetPaintHeight());
    recordingCanvas->SetIsCustomTextType(isCustomTextType_);
#else
    auto recordingCanvas = std::make_shared<Drawing::RecordingCanvas>(GetPaintWidth(), GetPaintHeight());
#endif
    func(recordingCanvas);
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy == nullptr) {
        return;
    }
    auto recording = recordingCanvas->GetDrawCmdList();
#ifndef USE_ROSEN_DRAWING
    if (recording && recording->GetSize() == 0) {
#else
    if (recording && recording->IsEmpty()) {
#endif
        return;
    }
    if (recording != nullptr && RSSystemProperties::GetDrawTextAsBitmap()) {
        // replace drawOpItem with cached one (generated by CPU)
#ifndef USE_ROSEN_DRAWING
        recording->GenerateCache();
#endif
    }
    std::unique_ptr<RSCommand> command =
        std::make_unique<RSCanvasNodeUpdateRecording>(GetId(), recording, static_cast<uint16_t>(type));
    transactionProxy->AddCommand(command, IsRenderServiceNode());
    recordingUpdated_ = true;
}

float RSCanvasNode::GetPaintWidth() const
{
    auto frame = GetStagingProperties().GetFrame();
    return frame.z_ <= 0.f ? GetStagingProperties().GetBounds().z_ : frame.z_;
}

float RSCanvasNode::GetPaintHeight() const
{
    auto frame = GetStagingProperties().GetFrame();
    return frame.w_ <= 0.f ? GetStagingProperties().GetBounds().w_ : frame.w_;
}

void RSCanvasNode::SetFreeze(bool isFreeze)
{
    if (!IsUniRenderEnabled()) {
        ROSEN_LOGE("SetFreeze is not supported in separate render");
        return;
    }
    std::unique_ptr<RSCommand> command = std::make_unique<RSSetFreeze>(GetId(), isFreeze);
    auto transactionProxy = RSTransactionProxy::GetInstance();
    if (transactionProxy != nullptr) {
        transactionProxy->AddCommand(command, true);
    }
}
} // namespace Rosen
} // namespace OHOS
