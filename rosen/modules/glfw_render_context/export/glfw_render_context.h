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

#ifndef GLFW_RENDER_CONTEXT_EXPORT_GLFW_RENDER_CONTEXT
#define GLFW_RENDER_CONTEXT_EXPORT_GLFW_RENDER_CONTEXT

#include <functional>
#include <memory>
#include <string>

struct GLFWwindow;
namespace OHOS::Rosen {
class GlfwRenderContext {
public:
    // GlfwRenderContext isn't a singleton.
    static std::shared_ptr<GlfwRenderContext> GetGlobal();

    /* before CreateWindow */
    int Init();
    void Terminate();

    /* before window operation */
    int CreateWindow(int32_t width, int32_t height, bool visible);
    void DestroyWindow();

    /* window operation */
    int WindowShouldClose();
    void WaitForEvents();
    void GetWindowSize(int32_t &width, int32_t &height);
    void SetWindowSize(int32_t width, int32_t height);
    void SetWindowTitle(const std::string &title);
    void MakeCurrent();
    void SwapBuffers();

private:
    static inline std::shared_ptr<GlfwRenderContext> global_ = nullptr;
    GLFWwindow *window_ = nullptr;
};
} // namespace OHOS::Rosen

#endif // GLFW_RENDER_CONTEXT_EXPORT_GLFW_RENDER_CONTEXT
