/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include <iostream>
#include <surface.h>

#include "display_type.h"
#include "include/core/SkColor.h"
#include "include/core/SkPaint.h"
#include "property/rs_properties_def.h"
#include "wm/window.h"

#include "include/core/SkCanvas.h"
#include "include/core/SkImageInfo.h"
#include "transaction/rs_transaction.h"
#include "ui/rs_root_node.h"
#include "ui/rs_canvas_node.h"
#include "ui/rs_display_node.h"
#include "ui/rs_surface_node.h"
#include "ui/rs_surface_extractor.h"
#include "ui/rs_ui_director.h"

using namespace OHOS;
using namespace OHOS::Rosen;
using namespace std;

std::shared_ptr<RSNode> rootNode;
std::vector<std::shared_ptr<RSCanvasNode>> nodes;

void Init(std::shared_ptr<RSUIDirector> rsUiDirector, int width, int height)
{
    std::cout << "rs app demo Init Rosen Backend!" << std::endl;

    rootNode = RSRootNode::Create();
    rootNode->SetBounds(0, 0, width, height);
    rootNode->SetFrame(0, 0, width, height);
    rootNode->SetBackgroundColor(SK_ColorBLUE);

    rsUiDirector->SetRoot(rootNode->GetId());
    auto node = RSCanvasNode::Create();
    node->SetBounds(0, 0, width, height);
    node->SetFrame(0, 0, width, height);

    auto canvas = node->BeginRecording(width, height);

    SkPaint paint;
    paint.setColor(SK_ColorYELLOW);
    canvas->drawRect({100, 100, 200, 200}, paint);

    node->FinishRecording();
    rootNode->AddChild(node, -1);
    nodes.push_back(node);
}

int main()
{
    std::cout << "rs app demo start!" << std::endl;
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowType(WindowType::WINDOW_TYPE_FLOAT);
    option->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    option->SetWindowRect({0, 112, 2560, 1396});
    auto window = Window::Create("app_demo", option);
    
    window->Show();
    auto surfaceNode = window->GetSurfaceNode();

    surfaceNode->SetFrameGravity(Gravity::CENTER);
    RSTransaction::FlushImplicitTransaction();
    sleep(1);

    auto rsUiDirector = RSUIDirector::Create();
    rsUiDirector->Init();
    std::cout << "rs app demo stage 1 " << std::endl;
    rsUiDirector->SetRSSurfaceNode(surfaceNode);
    Init(rsUiDirector, 100, 200);
    rsUiDirector->SendMessages();
    sleep(10);
    std::cout << "rs app demo end!" << std::endl;
    window->Hide();
    window->Destroy();
    return 0;
}
