/*
* Vulkan Example - Basic indexed triangle rendering
*
* Note:
*	This is a "pedal to the metal" example to show off how to get Vulkan up and displaying something
*	Contrary to the other examples, this one won't make use of helper functions or initializers
*	Except in a few cases (swap chain setup e.g.)
*
* Copyright (C) 2016-2017 by Sascha Willems - www.saschawillems.de
*
* This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
*/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <iostream>
#include <string>
#include <vector>
#include <exception>

#include <vulkan/vulkan.h>
#include "surface.h"
#include "refbase.h"
#include "render_context/render_context.h"
#include "transaction/rs_transaction.h"
#include "ui/rs_surface_extractor.h"
#include "ui/rs_surface_node.h"
#include "wm/window.h"
#include "window.h"
#include "vulkanexamplebase.h"
#include "vulkanexample.h"
#include "VulkanUtils.h"


VulkanExample *vulkanExample;

constexpr int DEFAULT_DISPLAY_ID = 0;
constexpr int WINDOW_LEFT = 100;
constexpr int WINDOW_TOP = 200;
constexpr int WINDOW_WIDTH = 360;
constexpr int WINDOW_HEIGHT = 360;
int main(const int argc, const char *argv[])
{
    // Create Window
    OHOS::sptr<OHOS::Rosen::WindowOption> option(new OHOS::Rosen::WindowOption());
    option->SetDisplayId(DEFAULT_DISPLAY_ID);
    option->SetWindowRect( {WINDOW_LEFT, WINDOW_TOP, WINDOW_WIDTH, WINDOW_HEIGHT} );
    // option->SetWindowType(OHOS::Rosen::WindowType::APP_MAIN_WINDOW_BASE);
    option->SetWindowType(OHOS::Rosen::WindowType::WINDOW_TYPE_POINTER);
    option->SetWindowMode(OHOS::Rosen::WindowMode::WINDOW_MODE_FLOATING);
    option->SetWindowName("vulkan_triangle_window");
    OHOS::sptr<OHOS::Rosen::Window> window_ = OHOS::Rosen::Window::Create(option->GetWindowName(), option);
    if (window_ == nullptr) {
        std::cout << "Create OHOS::Rosen::Window failed" << std::endl;
        return -1;
    }
    auto surfaceNodeWindow = window_->GetSurfaceNode();
    // Create SurfaceNode
    // OHOS::Rosen::RSSurfaceNodeConfig surfaceNodeConfig = {"vulkan_triangle_demo"};
    // auto surfaceNode = OHOS::Rosen::RSSurfaceNode::Create(surfaceNodeConfig, false);

    // surfaceNode->SetAbilityBGAlpha(255);
    // surfaceNode->SetBounds(0,0, WINDOW_WIDTH, WINDOW_HEIGHT);
    // surfaceNodeWindow->AddChild(surfaceNode, -1);


    OHOS::Rosen::RSTransaction::FlushImplicitTransaction();
    window_->Show();

    OHOS::sptr<OHOS::Surface> surf = surfaceNodeWindow->GetSurface();
    OHNativeWindow* nativeWindow = CreateNativeWindowFromSurface(&surf);
    if (nativeWindow == nullptr) {
        std::cout << "CreateNativeWindowFromSurface Failed" << std::endl;
        return -1;
    }

    vulkanExample = new VulkanExample();
    vulkanExample->initVulkan();
    vulkanExample->setupWindow(nativeWindow);
    vulkanExample->prepare();
    vulkanExample->renderLoop();

    delete(vulkanExample);
    window_->Hide();
    window_->Destroy();

    return 0;
}

