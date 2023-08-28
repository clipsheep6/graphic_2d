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

#include "webgl/webgl_framebuffer.h" // for WebGLFramebuffer

#include "context/webgl2_rendering_context_base.h"
#include "context/webgl_rendering_context_base.h"
#include "napi/n_class.h"    // for NClass
#include "napi/n_func_arg.h" // for NFuncArg, NARG_CNT
#include "napi/n_val.h"      // for NVal
#include "util/util.h"

namespace OHOS {
namespace Rosen {
using namespace std;

napi_value WebGLFramebuffer::Constructor(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(NARG_CNT::ZERO)) {
        return nullptr;
    }

    unique_ptr<WebGLFramebuffer> webGlFramebuffer = make_unique<WebGLFramebuffer>();
    if (!NClass::SetEntityFor<WebGLFramebuffer>(env, funcArg.GetThisVar(), move(webGlFramebuffer))) {
        return nullptr;
    }
    return funcArg.GetThisVar();
}

bool WebGLFramebuffer::Export(napi_env env, napi_value exports)
{
    vector<napi_property_descriptor> props = {};

    string className = GetClassName();
    bool succ = false;
    napi_value clas = nullptr;
    tie(succ, clas) = NClass::DefineClass(exports_.env_, className, WebGLFramebuffer::Constructor, std::move(props));
    if (!succ) {
        return false;
    }
    succ = NClass::SaveClass(exports_.env_, className, clas);
    if (!succ) {
        return false;
    }

    return exports_.AddProp(className, clas);
}

string WebGLFramebuffer::GetClassName()
{
    return WebGLFramebuffer::className;
}

NVal WebGLFramebuffer::CreateObjectInstance(napi_env env, WebGLFramebuffer** instance)
{
    return WebGLObject::CreateObjectInstance<WebGLFramebuffer>(env, instance);
}

WebGLFramebuffer::~WebGLFramebuffer()
{
    auto it = attachments_.begin();
    while (it != attachments_.end()) {
        delete it->second;
        attachments_.erase(it);
        it = attachments_.begin();
    }
}

bool WebGLFramebuffer::AddAttachment(GLenum attachment, GLuint id)
{
    WebGLAttachment* attachmentObject = new WebGLAttachment(AttachmentType::RENDER_BUFFER, attachment, id);
    if (attachmentObject == nullptr) {
        return false;
    }
    attachments_[attachment] = attachmentObject;
    return true;
}

bool WebGLFramebuffer::AddAttachment(GLenum attachment, GLuint id, GLenum target, GLint level)
{
    AttachmentTexture* attachmentObject = new AttachmentTexture(AttachmentType::TEXTURE, attachment, id);
    if (attachmentObject == nullptr) {
        return false;
    }
    attachments_[attachment] = attachmentObject;
    attachmentObject->target = target;
    attachmentObject->level = level;
    return true;
}

void WebGLFramebuffer::RemoveAttachment(GLenum attachment)
{
    auto it = attachments_.find(attachment);
    if (it == attachments_.end()) {
        return;
    }
    delete it->second;
    attachments_.erase(it);
}

void WebGLFramebuffer::RemoveAttachment(GLuint id, AttachmentType type)
{
    for (auto iter = attachments_.begin(); iter != attachments_.end(); iter++) {
        auto object = iter->second;
        if (object != nullptr && object->id == id && type == object->type) {
            delete iter->second;
            iter->second = nullptr;
            attachments_.erase(iter);
            break;
        }
    }
}

WebGLAttachment* WebGLFramebuffer::GetAttachment(GLenum attachment)
{
    auto it = attachments_.find(attachment);
    if (it != attachments_.end()) {
        return it->second;
    }
    return nullptr;
}
} // namespace Rosen
} // namespace OHOS
