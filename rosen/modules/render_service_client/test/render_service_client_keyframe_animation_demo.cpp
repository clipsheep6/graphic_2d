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

#include <event_handler.h>
#include <iostream>
#include <surface.h>

#ifndef USE_ROSEN_DRAWING
#include "include/core/SkBitmap.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkColor.h"
#include "include/core/SkImage.h"
#include "include/core/SkImageInfo.h"
#include "include/core/SkPaint.h"
#include "include/core/SkRect.h"
#include "include/core/SkTileMode.h"
#endif
#include "transaction/rs_transaction.h"
#include "ui/rs_canvas_node.h"
#include "ui/rs_surface_node.h"
#include "ui/rs_ui_director.h"
#include "ui/rs_root_node.h"
#include "wm/window.h"
#include "wm/window_option.h"

using namespace OHOS;
using namespace OHOS::Rosen;
using namespace std;

std::shared_ptr<RSNode> rootNode;
std::shared_ptr<RSCanvasNode> canvasNode;

constexpr float RECT_LENTH = 200;
constexpr float WINDOW_WIDTH = 1920;
constexpr float WINDOW_HEIGHT = 2560;
constexpr int DEFAULT_DURATION = 5000;
constexpr float KEYFRAME_FRACTION_1 = 0.25f;
constexpr float KEYFRAME_FRACTION_2 = 0.75f;
constexpr float KEYFRAME_FRACTION_3 = 1.0f;
constexpr float TRANSLATE_POS_1 = 100.0f;
constexpr float TRANSLATE_POS_2 = 400.0f;
constexpr float TRANSLATE_POS_3 = 500.0f;
constexpr float SCALE_VALUE_1 = 0.5f;
constexpr float SCALE_VALUE_2 = 2.0f;
constexpr int64_t START_NUM = 80825861106;
constexpr int64_t SEND_TIMESTAMP_NUM = 100000000;
constexpr int64_t UNIT_NUM = 1000;
constexpr int TEST_TYPE_2 = 2;
constexpr int TIME_CURVE_TYPE = 3;

void Init(std::shared_ptr<RSUIDirector> rsUiDirector, int width, int height)
{
    rootNode = RSRootNode::Create();
    rootNode->SetBounds(0, 0, width, height);
    rootNode->SetFrame(0, 0, width, height);
#ifndef USE_ROSEN_DRAWING
    rootNode->SetBackgroundColor(SK_ColorYELLOW);
#else
    rootNode->SetBackgroundColor(Drawing::Color::COLOR_YELLOW);
#endif
    canvasNode = RSCanvasNode::Create();
    canvasNode->SetBounds(0, 0, RECT_LENTH, RECT_LENTH);
    canvasNode->SetFrame(0, 0, RECT_LENTH, RECT_LENTH);
#ifndef USE_ROSEN_DRAWING
    canvasNode->SetBackgroundColor(SK_ColorBLUE);
#else
    canvasNode->SetBackgroundColor(Drawing::Color::COLOR_BLUE);
#endif
    rootNode->AddChild(canvasNode, -1);
    rsUiDirector->SetRoot(rootNode->GetId());
}

void RunDefaultAnimation(std::shared_ptr<RSUIDirector> rsUiDirector, std::shared_ptr<RSCanvasNode> canvasNode)
{
    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(DEFAULT_DURATION);
    RSNode::OpenImplicitAnimation(protocol, RSAnimationTimingCurve::DEFAULT, []() {
        std::cout<<"custom animation default finish callback"<<std::endl;
    });
    RSNode::AddKeyFrame(KEYFRAME_FRACTION_1, RSAnimationTimingCurve::EASE_IN, [&]() {
        canvasNode->SetTranslate(Vector2f(TRANSLATE_POS_1, TRANSLATE_POS_3));
        canvasNode->SetScale(Vector2f(SCALE_VALUE_2, SCALE_VALUE_2));
    });
    RSNode::AddKeyFrame(KEYFRAME_FRACTION_2, RSAnimationTimingCurve::LINEAR, [&]() {
        canvasNode->SetTranslate(Vector2f(TRANSLATE_POS_2, TRANSLATE_POS_3));
        canvasNode->SetScale(Vector2f(SCALE_VALUE_1, SCALE_VALUE_1));
    });
    RSNode::AddKeyFrame(KEYFRAME_FRACTION_3, RSAnimationTimingCurve::EASE_OUT, [&]() {
        canvasNode->SetTranslate(Vector2f(0.f, 0.f));
        canvasNode->SetScale(Vector2f(1.0f, 1.0f));
    });
    RSNode::CloseImplicitAnimation();

    bool hasRunningAnimation = true;
    int64_t startNum = START_NUM;
    while (hasRunningAnimation) {
        hasRunningAnimation = rsUiDirector->FlushAnimation(startNum);
        rsUiDirector->FlushModifier();
        rsUiDirector->SendMessages();
        startNum += SEND_TIMESTAMP_NUM;
        usleep(SEND_TIMESTAMP_NUM / UNIT_NUM);
    }
    sleep(DEFAULT_DURATION / UNIT_NUM);
}

void RunCustomAnimation(std::shared_ptr<RSUIDirector> rsUiDirector, std::shared_ptr<RSCanvasNode> canvasNode)
{
    int duration = 0;
    std::cout << "please input total duration(unit is ms(1000)):"  << std::endl;
    cin >> duration;
    RSAnimationTimingProtocol protocol;
    protocol.SetDuration(duration);

    int keyframeTimes = -1;
    std::cout << "please input animation keyframe times:(1-10)" << std::endl;
    cin >> keyframeTimes;
    int times = 1;
    float lastFraction = 0.f;
    std::cout << "start input keyframe parameter: total times is " << keyframeTimes << std::endl;
    RSNode::OpenImplicitAnimation(protocol, RSAnimationTimingCurve::DEFAULT, []() {
        std::cout<<"custom animation custom finish callback"<<std::endl;
    });
    while (times <= keyframeTimes) {
        float fraction = 0.f;
        std::cout << "start input keyframe" << times << " param:"  << std::endl;
        std::cout << "please input fraction(0-1.0f):" << std::endl;
        if (times >= keyframeTimes) {
            std::cout << "lastest time fraction is 1.0f"  << std::endl;
            fraction = 1.0f;
        } else {
            cin >> fraction;
            if (fraction > 1.0f) {
                std::cout << "fraction input error, please reinput"  << std::endl;
                continue;
            }
            if (fraction <= lastFraction) {
                std::cout << "current fraction must large than last fraction"  << std::endl;
                continue;
            }
        }
        lastFraction = fraction;
        std::cout << "please input timingCurve(1:LINEAR 2:EASE_IN 3:EASE_OUT):" << std::endl;
        int timingCurve = 1;
        cin >> timingCurve;
        if (timingCurve < 1 || timingCurve > TIME_CURVE_TYPE) {
            std::cout << "fraction input error, please reinput"  << std::endl;
            continue;
        }
        std::cout << "please input node SetTranslate:" << std::endl;
        float translateX, translateY;
        std::cout << "please input translateX:";
        cin >> translateX;
        std::cout << "please input translateY:";
        cin >> translateY;
        std::cout << "please input node SetScale:" << std::endl;
        float scale;
        std::cout << "please input scale:";
        auto curve = timingCurve == 1 ? RSAnimationTimingCurve::LINEAR :
            (timingCurve == TIME_CURVE_TYPE ? RSAnimationTimingCurve::EASE_OUT : RSAnimationTimingCurve::EASE_IN);
        RSNode::AddKeyFrame(fraction, curve, [&]() {
            canvasNode->SetTranslate(Vector2f(translateX, translateY));
            canvasNode->SetScale(Vector2f(scale, scale));
        });
        times++;
    }
    RSNode::CloseImplicitAnimation();

    bool hasRunningAnimation = true;
    int64_t startNum = START_NUM;
    while (hasRunningAnimation) {
        hasRunningAnimation = rsUiDirector->FlushAnimation(startNum);
        rsUiDirector->FlushModifier();
        rsUiDirector->SendMessages();
        startNum += SEND_TIMESTAMP_NUM;
        usleep(SEND_TIMESTAMP_NUM / UNIT_NUM);
    }
    sleep(duration / UNIT_NUM + 1);
}

int main()
{
    std::cout << "rs keyframe demo start!" << std::endl;
    sptr<WindowOption> option = new WindowOption();
    option->SetWindowType(WindowType::WINDOW_TYPE_FLOAT);
    option->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
    option->SetWindowRect({ 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT });
    auto window = Window::Create("keyframe_demo", option);
    window->Show();
    sleep(1);
    auto rect = window->GetRect();
    while (rect.width_ == 0 && rect.height_ == 0) {
        std::cout << "rs app demo create window failed: " << rect.width_ << " " << rect.height_ << std::endl;
        window->Hide();
        window->Destroy();
        window = Window::Create("keyframe_demo", option);
        window->Show();
        rect = window->GetRect();
    }
    auto surfaceNode = window->GetSurfaceNode();
    auto rsUiDirector = RSUIDirector::Create();
    rsUiDirector->Init();
    auto runner = OHOS::AppExecFwk::EventRunner::Create(true);
    auto handler = std::make_shared<OHOS::AppExecFwk::EventHandler>(runner);
    rsUiDirector->SetUITaskRunner(
        [handler](const std::function<void()>& task) {
            handler->PostTask(task);
        });
    runner->Run();
    RSTransaction::FlushImplicitTransaction();
    rsUiDirector->SetRSSurfaceNode(surfaceNode);
    Init(rsUiDirector, rect.width_, rect.height_);
    std::cout << "rs keyframe demo init finish"  << std::endl;
    canvasNode->SetTranslate(Vector2f(0.f, 0.f));
    rsUiDirector->SendMessages();
    sleep(1);

    int testType = -1;
    std::cout << "please input testType:(0:exit demo, 1:default keyframe, 2:custom ketframe)"  << std::endl;
    cin >> testType;
    if (testType == 1) {
        RunDefaultAnimation(rsUiDirector, canvasNode);
    } else if (testType == TEST_TYPE_2) {
        RunCustomAnimation(rsUiDirector, canvasNode);
    }

    std::cout << "rs keyframe demo exit!"  << std::endl;
    sleep(1);
    window->Hide();
    window->Destroy();
    return 0;
}
