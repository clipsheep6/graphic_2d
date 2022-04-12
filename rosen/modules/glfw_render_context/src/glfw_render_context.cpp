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
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "glfw_render_context.h"

#include <mutex>

#include <GLFW/glfw3.h>

namespace OHOS::Rosen {
std::shared_ptr<GlfwRenderContext> GlfwRenderContext::GetGlobal()
{
    if (global_ == nullptr) {
        static std::mutex mutex;
        std::lock_guard lock(mutex);
        if (global_ == nullptr) {
            global_ = std::make_shared<GlfwRenderContext>();
        }
    }

    return global_;
}

int GlfwRenderContext::Init()
{
    return glfwInit();
}

void GlfwRenderContext::Terminate()
{
    glfwTerminate();
}

int GlfwRenderContext::CreateWindow(int32_t width, int32_t height, bool visible)
{
    if (window_ != nullptr) {
        return 0;
    }

    auto flag = visible ? GLFW_TRUE : GLFW_FALSE;
    glfwWindowHint(GLFW_DECORATED, flag);
    glfwWindowHint(GLFW_VISIBLE, flag);
    window_ = glfwCreateWindow(width, height, "glfw window", nullptr, nullptr);
    if (window_ != nullptr) {
        return 0;
    }

    return 1;
}

void GlfwRenderContext::DestroyWindow()
{
    if (window_ != nullptr) {
        glfwDestroyWindow(window_);
    }
}

int GlfwRenderContext::WindowShouldClose()
{
    return glfwWindowShouldClose(window_);
}

void GlfwRenderContext::WaitForEvents()
{
    glfwWaitEvents();
}

void GlfwRenderContext::GetWindowSize(int32_t &width, int32_t &height)
{
    glfwGetWindowSize(window_, &width, &height);
}

void GlfwRenderContext::SetWindowSize(int32_t width, int32_t height)
{
    glfwSetWindowSize(window_, width, height);
}

void GlfwRenderContext::SetWindowTitle(const std::string &title)
{
    glfwSetWindowTitle(window_, title.c_str());
}

void GlfwRenderContext::MakeCurrent()
{
    glfwMakeContextCurrent(window_);
}

void GlfwRenderContext::SwapBuffers()
{
    glfwSwapBuffers(window_);
}
} // namespace OHOS::Rosen
