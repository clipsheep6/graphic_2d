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

#include <event_handler.h>
#include <iostream>
#include <surface.h>

#include "include/core/SkCanvas.h"
#include "include/core/SkColor.h"
#include "wm/window.h"

#include "modifier/rs_extended_modifier.h"
#include "modifier/rs_property_modifier.h"
#include "render/rs_border.h"
#include "transaction/rs_transaction.h"
#include "ui/rs_display_node.h"
#include "ui/rs_root_node.h"
#include "ui/rs_surface_extractor.h"
#include "ui/rs_surface_node.h"
#include "ui/rs_ui_director.h"

using namespace OHOS;
using namespace OHOS::Rosen;
using namespace std;

std::shared_ptr<RSNode> rootNode;
std::vector<std::shared_ptr<RSCanvasNode>> nodes;
std::shared_ptr<RSTranslateModifier> translateXY_;
std::shared_ptr<RSAlphaModifier> alpha_;

void Init(std::shared_ptr<RSUIDirector> rsUiDirector, int width, int height)
{
    std::cout << "rs app demo Init Rosen Backend!" << std::endl;

    rootNode = RSRootNode::Create();
    rootNode->SetBounds(0, 0, width, height);
    rootNode->SetFrame(0, 0, width, height);
    rootNode->SetBackgroundColor(SK_ColorYELLOW);

    nodes.emplace_back(RSCanvasNode::Create());
    nodes[0]->SetBounds(10, 10, 100, 100);
    nodes[0]->SetFrame(10, 10, 100, 100);
    nodes[0]->SetBackgroundColor(SK_ColorBLUE);

    rootNode->AddChild(nodes[0], -1);

    auto propertyXY = std::make_shared<RSAnimatableProperty<Vector2f>>(Vector2f(0.f, 0.f));
    translateXY_ = std::make_shared<RSTranslateModifier>(propertyXY);
    nodes[0]->AddModifier(translateXY_);

    auto propertyAlpha = std::make_shared<RSAnimatableProperty<float>>(1.0f);
    alpha_ = std::make_shared<RSAlphaModifier>(propertyAlpha);
    nodes[0]->AddModifier(alpha_);

    rsUiDirector->SetRoot(rootNode->GetId());
}

int main()
{
    int cnt = 0;

    // Init demo env
    std::cout << "rs app demo start!" << std::endl;
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowType(WindowType::WINDOW_TYPE_FLOAT);
    option->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    option->SetWindowRect({ 0, 0, 720, 1280 });
    auto window = Window::Create("app_demo", option);
    window->Show();
    sleep(1);
    auto rect = window->GetRequestRect();
    while (rect.width_ == 0 && rect.height_ == 0) {
        std::cout << "rs app demo create window failed: " << rect.width_ << " " << rect.height_ << std::endl;
        window->Hide();
        window->Destroy();
        window = Window::Create("app_demo", option);
        window->Show();
        sleep(1);
        rect = window->GetRequestRect();
    }
    std::cout << "rs app demo create window " << rect.width_ << " " << rect.height_ << std::endl;
    auto surfaceNode = window->GetSurfaceNode();

    // Build rosen renderThread & create nodes
    std::cout << "rs app demo stage " << cnt++ << std::endl;
    auto rsUiDirector = RSUIDirector::Create();
    rsUiDirector->Init();
    auto runner = OHOS::AppExecFwk::EventRunner::Create(true);
    auto handler = std::make_shared<OHOS::AppExecFwk::EventHandler>(runner);
    rsUiDirector->SetUITaskRunner([handler](const std::function<void()>& task) { handler->PostTask(task); });
    runner->Run();
    RSTransaction::FlushImplicitTransaction();
    rsUiDirector->SetRSSurfaceNode(surfaceNode);
    Init(rsUiDirector, rect.width_, rect.height_);

    Vector2f finalTrans = { 0.f, 800.f };
    float finalAlpha = 0.5;

    std::cout << "rs app demo stage " << cnt++ << std::endl;

    RSAnimationTimingProtocol protocol;
    // duration is not effective when the curve is interpolatingSpring
    protocol.SetDuration(3000);

    auto timingCurve = RSAnimationTimingCurve::CreateInterpolatingSpring(1.0f, 225.0f, 30.0f, 2.0f);

    // create property animation
    RSNode::Animate(
        protocol, timingCurve,
        [&]() {
            auto propertyXY = std::static_pointer_cast<RSAnimatableProperty<Vector2f>>(translateXY_->GetProperty());
            if (propertyXY) {
                propertyXY->Set({ finalTrans.data_[0], finalTrans.data_[1] });
            }

            auto propertyAlpha = std::static_pointer_cast<RSAnimatableProperty<float>>(alpha_->GetProperty());
            if (propertyAlpha) {
                propertyAlpha->Set(finalAlpha);
            }
        },
        []() { std::cout << "the first animation finish callback" << std::endl; });

    int64_t startNum = 80825861106;
    int64_t interruptNum = 80825861106 + 100000000 * 10;

    bool hasRunningAnimation = true;
    bool stopSignal = true;

    while (hasRunningAnimation) {
        hasRunningAnimation = rsUiDirector->FlushAnimation(startNum);
        rsUiDirector->FlushModifier();
        rsUiDirector->SendMessages();

        // simulate to stop animation by gesture
        if ((startNum >= interruptNum) && stopSignal) {
            // set duration to 0
            protocol.SetDuration(0);

            // send cancel request
            RSNode::Animate(
                protocol, RSAnimationTimingCurve::EASE_IN_OUT,
                [&]() {
                    auto propertyXY =
                        std::static_pointer_cast<RSAnimatableProperty<Vector2f>>(translateXY_->GetProperty());
                    if (propertyXY) {
                        propertyXY->RequestCancelAnimation();
                    }
                    auto propertyAlpha = std::static_pointer_cast<RSAnimatableProperty<float>>(alpha_->GetProperty());
                    if (propertyAlpha) {
                        propertyAlpha->RequestCancelAnimation();
                    }
                },
                [&]() {
                    // this finish callback will also execute, though this closure create no animation
                    std::cout << "request cancel animation finish callback" << std::endl;
                });

            stopSignal = false;
        }
        startNum += 100000000;
        usleep(100000);
    }

    // dump property via modifiers
    std::cout << "rs app demo stage " << cnt++ << std::endl;
    std::cout << nodes[0]->GetStagingProperties().Dump() << std::endl;
    std::cout << "rs app demo end!" << std::endl;
    sleep(3);

    window->Hide();
    window->Destroy();
    return 0;
}