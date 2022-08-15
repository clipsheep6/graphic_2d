/*
* Copyright (c) 2021 Huawei Device Co., Ltd.
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

#include <iostream>
#include <cstdint>
#include <thread>
#include <optional>
#include <string>

#include <surface.h>
#include <wm_common.h>
#include <window.h>
#include <window_option.h>
#include <render_context/render_context.h>
#include <ui/rs_surface_node.h>
#include <screen_manager/rs_screen_mode_info.h>
#include <transaction/rs_interfaces.h>

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

using namespace OHOS;
using namespace OHOS::Rosen;
using namespace std;

struct NativeWindow;

extern "C" {
NativeWindow *CreateNativeWindowFromSurface(void *pSurface);
void DestoryNativeWindow(NativeWindow *nativeWindow);
int32_t NativeWindowHandleOpt(NativeWindow *window, int code, ...);
enum NativeWindowOperation {
    SET_BUFFER_GEOMETRY,
    GET_BUFFER_GEOMETRY,
    GET_FORMAT,
    SET_FORMAT,
    GET_USAGE,
    SET_USAGE,
    SET_STRIDE,
    GET_STRIDE,
    SET_SWAP_INTERVAL,
    GET_SWAP_INTERVAL,
    SET_COLOR_GAMUT,
    GET_COLOR_GAMUT,
    SET_TRANSFORM,
    GET_TRANSFORM,
    SET_UI_TIMESTAMP,
};
}

namespace OHOS::Rosen {
class RSHelloTriangle {
public:
    inline static RSHelloTriangle& GetInstance()
    {
        static RSHelloTriangle c;
        return c;
    }

    ~RSHelloTriangle()
    {
        #ifdef ACE_ENABLE_GPU
        // frees inside of own factory
        rc_ = nullptr;

        DestoryNativeWindow(nativeWindow_);
        nativeWindow_ = nullptr;
        #endif
    }

    static void Hello()
    {
        cout « "RS Hello Native GL Triangle Start\n";
        cout « "If you want to get more information, \n";
        cout « "please type 'hilog | grep Rs' in another hdc shell window\n";
        cout « "-------------------------------------------------------\n";
    }

    void Init()
    {
        cout « "-------------------------------------------------------\n";
        cout « "RS Hello Native GL Triangle Init Start\n";

#ifdef ACE_ENABLE_GPU
        cout « "ACE_ENABLE_GPU is enabled\n";
#else // ACE_ENABLE_GPU
        cout « "ACE_ENABLE_GPU is disabled\n";
#endif // ACE_ENABLE_GPU

        // init display
        auto& rs_interface = RSInterfaces::GetInstance();
        RSScreenModeInfo mode =
        rs_interface.GetScreenActiveMode(rs_interface.GetDefaultScreenId());

        screenWidth_ = mode.GetScreenWidth();
        screenHeight_ = mode.GetScreenHeight();
        {
            sptr<WindowOption> option = new WindowOption();
            option->SetWindowRect({0, 0, screenWidth_, screenHeight_});
            option->SetWindowMode(WindowMode::WINDOW_MODE_FLOATING);
            option->SetWindowType(WindowType::WINDOW_TYPE_APP_LAUNCHING);
            previewWindow_ = Window::Create("RSHelloNativeGLTriangle_window", option);
        }
        previewWindow_->Show();
        auto surface = previewWindow_->GetSurfaceNode()->GetSurface();
        nativeWindow_ = CreateNativeWindowFromSurface(&surface);
        NativeWindowHandleOpt(nativeWindow_, SET_BUFFER_GEOMETRY, screenWidth_, screenHeight_);

        RenderContextInit();
        cout « "RS Hello Native GL Triangle Init end\n";
        cout « "-------------------------------------------------------\n";
        return;
    }

    void Run()
    {
        cout « "-------------------------------------------------------\n";
        cout « "RS Hello Native GL Triangle Run Start\n";
#ifdef ACE_ENABLE_GPU
        glViewport(0, 0, screenWidth_, screenHeight_);
        glUseProgram(program_);

        glClearColor(.3f, .5f, .7f, 1.f);
        glDisable(GL_CULL_FACE);
        glFrontFace(GL_CW);

        glBindVertexArray(vao_);
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

        for (int32_t i = 0; i < 100; ++i)
        {
            glClear(GL_COLOR_BUFFER_BIT);
            glDrawArrays(GL_TRIANGLES, 0, 3);

            rc_->SwapBuffers(surface_);
            this_thread::sleep_for(100ms);
        }
#endif // ACE_ENABLE_GPU
        cout « "RS Hello Native GL Triangle Run end\n";
        cout «
        "-------------------------------------------------------\n";
    }

    void Deinit()
    {
        cout « "-------------------------------------------------------\n";
        cout « "RS Hello Native GL Triangle Deinit Start\n";
        RenderContextDeinit();
        cout « "RS Hello Native GL Triangle Deinit end\n";
        cout « "-------------------------------------------------------\n";
    }
private:
    RSHelloTriangle() = default;
    void RenderContextInit()
    {
        cout « "Init RenderContext start.\n";
#ifdef ACE_ENABLE_GPU
        cout « "ACE_ENABLE_GPU is true.\n";

        rc_ = RenderContextFactory::GetInstance().CreateEngine();
        if (rc_) {
        cout « "Init RenderContext success.\n";
        rc_->InitializeEglContext();
        surface_ = rc_->CreateEGLSurface(nativeWindow_);
        rc_->MakeCurrent(surface_);

        // create program
        {
            GLuint vs = glCreateShader(GL_VERTEX_SHADER);
            GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);

            auto compileShader = [](GLuint shader_id, const char *source) {
                GLint status;
                glShaderSource(shader_id, 1, &source, nullptr);
                glCompileShader(shader_id);
                glGetShaderiv(shader_id, GL_COMPILE_STATUS, &status);
                if (status != GL_TRUE) {
                    GLint log_len;
                    glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &log_len);
                    vector<char> shader_log(log_len + 1);
                    glGetShaderInfoLog(shader_id, log_len, nullptr, &shader_log[0]);
                    fprintf(stderr, "Shader %d was not compiled successfully: %s\n", shader_id, shader_log.data());
                    fprintf(stderr, "Shader source code:\n%s\n", source);
                }
            };

            compileShader(vs, R"(#version 100
attribute vec3 position;
varying highp vec3 o_value;
void main() {
gl_Position.xyz = position;
gl_Position.w = 1.0;
o_value = vec3(position.x, position.y, 0);
}
)");
            compileShader(fs, R"(#version 100
varying highp vec3 o_value;
void main() {
gl_FragColor = vec4(o_value.xyz, 1.0);
}
)");

            program_ = glCreateProgram();
            glAttachShader(program_, vs);
            glAttachShader(program_, fs);
            glLinkProgram(program_);

            {
                GLint status;
                glGetProgramiv(program_, GL_LINK_STATUS, &status);
                if (status != GL_TRUE) {
                    GLint log_len;
                    glGetProgramiv(program_, GL_INFO_LOG_LENGTH, &log_len);
                    vector<char> program_log(log_len + 1);
                    glGetProgramInfoLog(program_, log_len, nullptr, &program_log[0]);
                    fprintf(stderr, "Program was not linked successfully: %s", program_log.data());
                }
            }

            glDeleteShader(vs);
            glDeleteShader(fs);
        }

            glGenVertexArrays(1, &vao_);
            glGenBuffers(1, &vbo_);
            glBindBuffer(GL_ARRAY_BUFFER, vbo_);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_buffer_data), vertex_buffer_data, GL_STATIC_DRAW);
        } else {
            cout « "Init RenderContext failed, RenderContext is nullptr.\n";
        }
#endif // ACE_ENABLE_GPU
        cout « "Init RenderContext end.\n";
    }

    void RenderContextDeinit()
    {
        cout « "Deinit RenderContext start.\n";
#ifdef ACE_ENABLE_GPU
        cout « "ACE_ENABLE_GPU is true.\n";

        if (program_) {
        glDeleteProgram(program_);
        program_ = 0;
        }
        if (vao_) {
        glDeleteVertexArrays(1, &vao_);
        vao_ = 0;
        }
        if (vbo_) {
        glDeleteBuffers(1, &vbo_);
        vbo_ = 0;
        }

        rc_->MakeCurrent(EGL_NO_SURFACE);
        rc_->DestroyEGLSurface(surface_);
#endif // ACE_ENABLE_GPU
        cout « "Deinit RenderContext end.\n";
    }

#ifdef ACE_ENABLE_GPU
    static constexpr GLfloat vertex_buffer_data[] = {
    .9f, -0.9f, -0.9f,
    .9f, .9f, -0.9f,
    -0.9f, .9f, -0.9f
    };
    GLuint program_ = 0;
    GLuint vao_ = 0;
    GLuint vbo_ = 0;
    RenderContext *rc_ = nullptr;
#endif // ACE_ENABLE_GPU
    int screenWidth_ = 0;
    int screenHeight_ = 0;

    NativeWindow *nativeWindow_ = nullptr;
    OHOS::sptr<OHOS::Rosen::Window> previewWindow_ = nullptr;
    EGLSurface surface_ = EGL_NO_SURFACE;
}; // class RSHelloTriangle
} // namespace OHOS::Rosen

int main()
{
    RSHelloTriangle::Hello();
    RSHelloTriangle::GetInstance().Init();
    RSHelloTriangle::GetInstance().Run();
    RSHelloTriangle::GetInstance().Deinit();
    return 0;
}
