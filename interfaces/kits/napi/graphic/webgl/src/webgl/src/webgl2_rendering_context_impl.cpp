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
#include "context/webgl2_rendering_context_impl.h"

#include "context/webgl2_rendering_context_base.h"
#include "context/webgl_context_attributes.h"
#include "context/webgl_rendering_context.h"
#include "context/webgl_rendering_context_base.h"
#include "context/webgl_rendering_context_basic_base.h"
#include "napi/n_class.h"
#include "napi/n_func_arg.h"
#include "util/egl_manager.h"
#include "util/log.h"
#include "util/util.h"
#include "webgl/webgl_query.h"
#include "webgl/webgl_sampler.h"
#include "webgl/webgl_shader.h"
#include "webgl/webgl_sync.h"
#include "webgl/webgl_transform_feedback.h"
#include "webgl/webgl_vertex_array_object.h"

namespace OHOS {
namespace Rosen {
namespace Impl {
using namespace std;
#define SET_ERROR(error) \
    do {                      \
        LOGE("WebGL2 error code %{public}u", error);    \
        SetError(error);      \
    } while (0)

#define SET_ERROR_WITH_LOG(error, info, ...) \
    do {                      \
        LOGE("WebGL2 error code %{public}u" info, error, ##__VA_ARGS__);    \
        SetError(error);      \
    } while (0)

void WebGL2RenderingContextImpl::Init()
{
    WebGLRenderingContextBaseImpl::Init();
    if (maxSamplerUnit_) {
        return;
    }
    GLint max = 0;
    glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &max);
    maxSamplerUnit_ = static_cast<GLuint>(max);
    samplerUnits_.resize(max);

    glGetIntegerv(GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_ATTRIBS, &max);
    boundIndexedTransformFeedbackBuffers_.clear();
    boundIndexedTransformFeedbackBuffers_.resize(max);

    glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &max);
    boundIndexedUniformBuffers_.clear();
    boundIndexedUniformBuffers_.resize(max);
    maxBoundUniformBufferIndex_ = 0;
}

napi_value WebGL2RenderingContextImpl::CreateQuery(napi_env env)
{
    WebGLQuery* webGlQuery = nullptr;
    napi_value objQuery = WebGLQuery::CreateObjectInstance(env, &webGlQuery).val_;
    if (!objQuery) {
        return NVal::CreateNull(env).val_;
    }

    unsigned int queryId;
    glGenQueries(1, &queryId);
    webGlQuery->SetQuery(queryId);
    AddObject<WebGLQuery>(env, queryId, objQuery);
    LOGI("WebGL2 createQuery queryId = %{public}u", queryId);
    return objQuery;
}

napi_value WebGL2RenderingContextImpl::DeleteQuery(napi_env env, napi_value object)
{
    uint32_t queryId = 0;
    WebGLQuery* webGlQuery = WebGLObject::GetObjectInstance<WebGLQuery>(env, object);
    if (webGlQuery == nullptr) {
        return nullptr;
    }
    queryId = webGlQuery->GetQuery();
    glDeleteQueries(1, &queryId);
    DeleteObject<WebGLQuery>(env, queryId);
    uint32_t index = 0;
    LOGI("WebGL2 deleteQuery target %{public}u", webGlQuery->GetTarget());
    if (CheckQueryTarget(env, webGlQuery->GetTarget(), index)) {
        LOGI("WebGL2 deleteQuery currentQuery_ %{public}u", currentQuery_[index]);
        if (currentQuery_[index] == queryId) {
            currentQuery_[index] = 0;
            glEndQuery(webGlQuery->GetTarget());
        }
    }
    LOGI("WebGL2 deleteQuery queryId = %{public}u", queryId);
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::IsQuery(napi_env env, napi_value object)
{
    GLuint queryId = 0;
    WebGLQuery* webGlQuery = WebGLObject::GetObjectInstance<WebGLQuery>(env, object);
    if (webGlQuery == nullptr) {
        return NVal::CreateBool(env, false).val_;
    }
    queryId = webGlQuery->GetQuery();

    GLboolean returnValue = glIsQuery(queryId);
    bool res = static_cast<bool>(returnValue);
    LOGI("WebGL2 isQuery query %{public}u result %{public}d ", queryId, res);
    return NVal::CreateBool(env, res).val_;
}

napi_value WebGL2RenderingContextImpl::GetQuery(napi_env env, GLenum target, GLenum pName)
{
    LOGI("WebGL2 getQuery target %{public}u %{public}u", target, pName);
    if (pName != WebGL2RenderingContextBase::CURRENT_QUERY) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }
    uint32_t index = 0;
    if (!CheckQueryTarget(env, target, index)) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }

    GLint params;
    glGetQueryiv(target, pName, &params);
    LOGI("WebGL2 getQuery params = %{public}u %{public}u", params, currentQuery_[index]);
    return GetObject<WebGLQuery>(env, params);
}

napi_value WebGL2RenderingContextImpl::BeginQuery(napi_env env, GLenum target, napi_value object)
{
    LOGI("WebGL2 beginQuery target %{public}u", target);
    GLuint queryId = 0;
    WebGLQuery* webGlQuery = WebGLObject::GetObjectInstance<WebGLQuery>(env, object);
    if (webGlQuery == nullptr) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_OPERATION);
        return nullptr;
    }
    queryId = webGlQuery->GetQuery();

    if (webGlQuery->GetTarget() && webGlQuery->GetTarget() != target) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_OPERATION);
        return nullptr;
    }
    uint32_t index = 0;
    if (!CheckQueryTarget(env, target, index)) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }

    if (currentQuery_[index] && currentQuery_[index] != queryId) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_OPERATION);
        return nullptr;
    }
    currentQuery_[index] = queryId;
    webGlQuery->SetTarget(target);

    glBeginQuery(target, queryId);
    LOGI("WebGL2 beginQuery target %{public}u queryId %{public}u result %{public}u", target, queryId, GetError_());
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::EndQuery(napi_env env, GLenum target)
{
    LOGI("WebGL2 endQuery target %{public}u", target);
    uint32_t index = 0;
    if (!CheckQueryTarget(env, target, index)) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }
    if (currentQuery_[index]) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_OPERATION);
        return nullptr;
    }
    currentQuery_[index] = 0;
    glEndQuery(target);
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::GetQueryParameter(napi_env env, napi_value queryObj, GLenum pName)
{
    GLuint queryId = 0;
    WebGLQuery* webGlQuery = WebGLObject::GetObjectInstance<WebGLQuery>(env, queryObj);
    if (webGlQuery == nullptr) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
        return NVal::CreateNull(env).val_;
    }
    queryId = webGlQuery->GetQuery();

    GLuint params;
    if (pName == GL_QUERY_RESULT) {
        glGetQueryObjectuiv(queryId, pName, &params);
        int64_t res = static_cast<int64_t>(params);
        LOGI("WebGL2 getQueryParameter params %{public}u", params);
        return NVal::CreateInt64(env, res).val_;
    } else if (pName == GL_QUERY_RESULT_AVAILABLE) {
        glGetQueryObjectuiv(queryId, pName, &params);
        bool res = (params == GL_FALSE) ? false : true;
        LOGI("WebGL2 getQueryParameter params %{public}u", params);
        return NVal::CreateBool(env, res).val_;
    } else {
        SET_ERROR(WebGLRenderingContextBase::INVALID_ENUM);
        return NVal::CreateNull(env).val_;
    }
}

napi_value WebGL2RenderingContextImpl::CreateSampler(napi_env env)
{
    WebGLSampler* webGlSampler = nullptr;
    napi_value objSampler = WebGLSampler::CreateObjectInstance(env, &webGlSampler).val_;
    if (!objSampler) {
        return NVal::CreateNull(env).val_;
    }
    GLuint samplerId;
    glGenSamplers(1, &samplerId);
    webGlSampler->SetSampler(samplerId);
    LOGI("WebGL2 createSampler samplerId = %{public}u", samplerId);
    AddObject<WebGLSampler>(env, samplerId, objSampler);
    return objSampler;
}

napi_value WebGL2RenderingContextImpl::DeleteSampler(napi_env env, napi_value samplerObj)
{
    WebGLSampler* sampler = WebGLObject::GetObjectInstance<WebGLSampler>(env, samplerObj);
    if (sampler == nullptr) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    GLuint samplerId = sampler->GetSampler();
    LOGI("WebGL2 deleteSampler samplerId = %{public}u", samplerId);
    // delete
    glBindSampler(sampler->GetSampleUnit(), 0);

    samplerUnits_[sampler->GetSampleUnit()] = 0;
    glDeleteSamplers(1, &samplerId);
    sampler->SetSampleUnit(0);
    DeleteObject<WebGLSampler>(env, samplerId);
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::IsSampler(napi_env env, napi_value samplerObj)
{
    WebGLSampler* sampler = WebGLObject::GetObjectInstance<WebGLSampler>(env, samplerObj);
    if (sampler == nullptr) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
        return NVal::CreateBool(env, false).val_;
    }
    GLuint samplerId = sampler->GetSampler();

    GLboolean returnValue = glIsSampler(samplerId);
    bool res = static_cast<bool>(returnValue);
    LOGI("WebGL2 IsSampler samplerId = %{public}u res %{public}u", samplerId, res);
    return NVal::CreateBool(env, res).val_;
}

napi_value WebGL2RenderingContextImpl::BindSampler(napi_env env, GLuint unit, napi_value samplerObj)
{
    WebGLSampler* sampler = WebGLObject::GetObjectInstance<WebGLSampler>(env, samplerObj);
    if (sampler == nullptr) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_OPERATION);
        return nullptr;
    }
    GLuint samplerId = sampler->GetSampler();
    if (unit >= samplerUnits_.size()) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    sampler->SetSampleUnit(unit);
    glBindSampler(unit, samplerId);
    samplerUnits_[unit] = samplerId;
    LOGI("WebGL2 bindSampler unit = %{public}u samplerId = %{public}u", unit, samplerId);
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::SamplerParameter(
    napi_env env, napi_value samplerObj, GLenum pName, bool isFloat, void* param)
{
    LOGI("WebGL2 samplerParameteri pname %{public}u param %{public}u", pName, param);
    WebGLSampler* sampler = WebGLObject::GetObjectInstance<WebGLSampler>(env, samplerObj);
    if (sampler == nullptr) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_OPERATION);
        return nullptr;
    }
    GLuint samplerId = sampler->GetSampler();
    switch (pName) {
        case GL_TEXTURE_COMPARE_FUNC:
        case GL_TEXTURE_COMPARE_MODE:
        case GL_TEXTURE_MAG_FILTER:
        case GL_TEXTURE_MIN_FILTER:
        case GL_TEXTURE_WRAP_R:
        case GL_TEXTURE_WRAP_S:
        case GL_TEXTURE_WRAP_T:
        case GL_TEXTURE_MAX_LOD:
        case GL_TEXTURE_MIN_LOD:
            break;
        default:
            SET_ERROR(WebGLRenderingContextBase::INVALID_ENUM);
            return nullptr;
    }
    if (isFloat) {
        GLfloat v = *static_cast<GLfloat*>(param);
        glSamplerParameterf(samplerId, pName, v);
    } else {
        GLint v = *static_cast<GLint*>(param);
        glSamplerParameteri(samplerId, pName, v);
    }
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::GetSamplerParameter(napi_env env, napi_value samplerObj, GLenum pName)
{
    WebGLSampler* sampler = WebGLObject::GetObjectInstance<WebGLSampler>(env, samplerObj);
    if (sampler == nullptr) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_OPERATION);
        return NVal::CreateNull(env).val_;
    }
    GLuint samplerId = sampler->GetSampler();
    LOGI("WebGL2 getSamplerParameter samplerId %{public}u params %{public}u", samplerId, pName);

    switch (pName) {
        case GL_TEXTURE_COMPARE_FUNC:
        case GL_TEXTURE_COMPARE_MODE:
        case GL_TEXTURE_MAG_FILTER:
        case GL_TEXTURE_MIN_FILTER:
        case GL_TEXTURE_WRAP_R:
        case GL_TEXTURE_WRAP_S:
        case GL_TEXTURE_WRAP_T: {
            GLint params;
            glGetSamplerParameteriv(static_cast<GLuint>(samplerId), static_cast<GLenum>(pName), &params);
            int64_t res = static_cast<int64_t>(params);
            LOGI("WebGL2 getSamplerParameter samplerId %{public}u params %{public}u", samplerId, params);
            return NVal::CreateInt64(env, res).val_;
        }
        case GL_TEXTURE_MAX_LOD:
        case GL_TEXTURE_MIN_LOD: {
            GLfloat params;
            glGetSamplerParameterfv(static_cast<GLuint>(samplerId), static_cast<GLenum>(pName), &params);
            float res = static_cast<float>(params);
            LOGI("WebGL2 getSamplerParameter samplerId %{public}u params %{public}f", samplerId, params);
            return NVal::CreateDouble(env, (double)res).val_;
        }
        default:
            SET_ERROR(WebGLRenderingContextBase::INVALID_ENUM);
            return NVal::CreateNull(env).val_;
    }
}

napi_value WebGL2RenderingContextImpl::CreateVertexArray(napi_env env)
{
    WebGLVertexArrayObject* webGLVertexArrayObject = nullptr;
    napi_value objVertexArrayObject = WebGLVertexArrayObject::CreateObjectInstance(env, &webGLVertexArrayObject).val_;
    if (!objVertexArrayObject) {
        return NVal::CreateNull(env).val_;
    }
    unsigned int vertexArraysId;
    glGenVertexArrays(1, &vertexArraysId);

    webGLVertexArrayObject->SetVertexArrays(vertexArraysId);
    LOGI("WebGL2 createVertexArray vertexArraysId = %{public}u", vertexArraysId);
    AddObject<WebGLVertexArrayObject>(env, vertexArraysId, objVertexArrayObject);
    return objVertexArrayObject;
}

napi_value WebGL2RenderingContextImpl::DeleteVertexArray(napi_env env, napi_value object)
{
    unsigned int vertexArrays = WebGLVertexArrayObject::DEFAULT_VERTEX_ARRAY_OBJECT;
    WebGLVertexArrayObject* webGLVertexArrayObject =
        WebGLObject::GetObjectInstance<WebGLVertexArrayObject>(env, object);
    if (webGLVertexArrayObject == nullptr) {
        return nullptr;
    }
    vertexArrays = webGLVertexArrayObject->GetVertexArrays();
    if (boundVertexArrayId_ && boundVertexArrayId_ == vertexArrays) {
        boundVertexArrayId_ = 0;
    }
    glDeleteVertexArrays(1, &vertexArrays);
    LOGI("WebGL2 deleteVertexArrays vertexArrays %{public}u", vertexArrays);
    DeleteObject<WebGLVertexArrayObject>(env, vertexArrays);
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::IsVertexArray(napi_env env, napi_value object)
{
    GLuint vertexArrayId = WebGLVertexArrayObject::DEFAULT_VERTEX_ARRAY_OBJECT;
    WebGLVertexArrayObject* webGLVertexArrayObject =
        WebGLObject::GetObjectInstance<WebGLVertexArrayObject>(env, object);
    if (webGLVertexArrayObject == nullptr) {
        return NVal::CreateBool(env, false).val_;
    }
    vertexArrayId = webGLVertexArrayObject->GetVertexArrays();
    GLboolean returnValue = glIsVertexArray(vertexArrayId);
    LOGI("WebGL2 isVertexArray %{public}u %{public}u", vertexArrayId, returnValue);
    return NVal::CreateBool(env, returnValue).val_;
}

napi_value WebGL2RenderingContextImpl::BindVertexArray(napi_env env, napi_value object)
{
    GLuint vertexArrayId = WebGLVertexArrayObject::DEFAULT_VERTEX_ARRAY_OBJECT;
    WebGLVertexArrayObject* webGLVertexArrayObject =
        WebGLObject::GetObjectInstance<WebGLVertexArrayObject>(env, object);
    if (webGLVertexArrayObject == nullptr) {
        return nullptr;
    }
    vertexArrayId = webGLVertexArrayObject->GetVertexArrays();
    glBindVertexArray(vertexArrayId);
    boundVertexArrayId_ = vertexArrayId;
    LOGI("WebGL2 bindVertexArray %{public}u ", vertexArrayId);
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::FenceSync(napi_env env, GLenum condition, GLbitfield flags)
{
    LOGI("WebGL2 fenceSync condition  %{public}u flags %{public}u", condition, flags);
    WebGLSync* webGlSync = nullptr;
    napi_value objSync = WebGLSync::CreateObjectInstance(env, &webGlSync).val_;
    if (!objSync) {
        return NVal::CreateNull(env).val_;
    }
    GLsync returnValue = glFenceSync(condition, flags);
    webGlSync->SetSync(reinterpret_cast<int64_t>(returnValue));
    LOGI("WebGL2 fenceSync syncId %{public}u", returnValue);
    return objSync;
}

napi_value WebGL2RenderingContextImpl::IsSync(napi_env env, napi_value syncObj)
{
    WebGLSync* webGlSync = WebGLObject::GetObjectInstance<WebGLSync>(env, syncObj);
    if (webGlSync == nullptr) {
        return NVal::CreateBool(env, false).val_;
    }
    int64_t syncId = webGlSync->GetSync();
    GLboolean returnValue = glIsSync(reinterpret_cast<GLsync>(syncId));
    LOGI("WebGL2 isSync syncId = %{public}ld result %{public}u", syncId, returnValue);
    return NVal::CreateBool(env, static_cast<bool>(returnValue)).val_;
}

napi_value WebGL2RenderingContextImpl::DeleteSync(napi_env env, napi_value syncObj)
{
    WebGLSync* webGlSync = WebGLObject::GetObjectInstance<WebGLSync>(env, syncObj);
    if (webGlSync == nullptr) {
        return nullptr;
    }
    int64_t syncId = webGlSync->GetSync();

    glDeleteSync(reinterpret_cast<GLsync>(syncId));
    LOGI("WebGL2 deleteSync syncId %{public}ld ", syncId);
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::ClientWaitSync(
    napi_env env, napi_value syncObj, GLbitfield flags, GLint64 timeout)
{
    WebGLSync* webGlSync = WebGLObject::GetObjectInstance<WebGLSync>(env, syncObj);
    if (webGlSync == nullptr) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
        return NVal::CreateInt64(env, WebGL2RenderingContextBase::WAIT_FAILED).val_;
    }
    int64_t syncId = webGlSync->GetSync();
    if (timeout < WebGL2RenderingContextBase::TIMEOUT_IGNORED) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
        return NVal::CreateInt64(env, WebGL2RenderingContextBase::WAIT_FAILED).val_;
    }

    GLuint64 timeout64 = static_cast<GLuint64>(timeout);
    GLint maxTimeout = 0;
    glGetIntegerv(WebGL2RenderingContextBase::MAX_CLIENT_WAIT_TIMEOUT_WEBGL, &maxTimeout);
    LOGI("WebGL2 clientWaitSync maxTimeout %{public}u ", maxTimeout);
    if (timeout64 > static_cast<GLuint64>(maxTimeout)) {
        timeout64 = maxTimeout;
    }

    GLenum returnValue = glClientWaitSync(reinterpret_cast<GLsync>(syncId), flags, timeout64);
    LOGI("WebGL2 clientWaitSync syncId = %{public}u result %{public}u", syncId, returnValue);
    return NVal::CreateInt64(env, static_cast<int64_t>(returnValue)).val_;
}

napi_value WebGL2RenderingContextImpl::WaitSync(napi_env env, napi_value syncObj, GLbitfield flags, GLint64 timeout)
{
    WebGLSync* webGlSync = WebGLObject::GetObjectInstance<WebGLSync>(env, syncObj);
    if (webGlSync == nullptr) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    int64_t syncId = webGlSync->GetSync();
    if (timeout < WebGL2RenderingContextBase::TIMEOUT_IGNORED) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    GLuint64 timeout64 = static_cast<GLuint64>(timeout);
    GLint maxTimeout = 0;
    glGetIntegerv(WebGL2RenderingContextBase::MAX_CLIENT_WAIT_TIMEOUT_WEBGL, &maxTimeout);
    LOGI("WebGL2 clientWaitSync maxTimeout %{public}u ", maxTimeout);
    if (timeout64 > static_cast<GLuint64>(maxTimeout)) {
        timeout64 = maxTimeout;
    }

    glWaitSync(reinterpret_cast<GLsync>(syncId), flags, timeout64);
    LOGI("WebGL2 waitSync syncId %{public}u", syncId);
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::GetSyncParameter(napi_env env, napi_value syncObj, GLenum pname)
{
    WebGLSync* webGlSync = WebGLObject::GetObjectInstance<WebGLSync>(env, syncObj);
    if (webGlSync == nullptr) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
        return NVal::CreateNull(env).val_;
    }
    int64_t syncId = webGlSync->GetSync();
    LOGI("WebGL2 getSyncParameter syncId %{public}u pname %{public}u ", syncId, pname);
    if (CheckInList(env, pname, { GL_OBJECT_TYPE, GL_SYNC_STATUS, GL_SYNC_CONDITION, GL_SYNC_FLAGS })) {
        GLint value = 0;
        GLsizei length = -1;
        glGetSynciv(reinterpret_cast<GLsync>(syncId), pname, 1, &length, &value);
        LOGI("WebGL2 getSyncParameter syncId = %{public}u value %{public}d ", syncId, value);
        return NVal::CreateInt64(env, value).val_;
    } else {
        SET_ERROR(WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }
}

napi_value WebGL2RenderingContextImpl::CreateTransformFeedback(napi_env env)
{
    WebGLTransformFeedback* webGlTransformFeedback = nullptr;
    napi_value objTransformFeedback = WebGLTransformFeedback::CreateObjectInstance(env, &webGlTransformFeedback).val_;
    if (!objTransformFeedback) {
        return NVal::CreateNull(env).val_;
    }
    GLuint transformFeedbackId;
    glGenTransformFeedbacks(1, &transformFeedbackId);
    webGlTransformFeedback->SetTransformFeedback(transformFeedbackId);
    LOGI("WebGL2 createTransformFeedback transformFeedbackId %{public}u", transformFeedbackId);
    AddObject<WebGLTransformFeedback>(env, transformFeedbackId, objTransformFeedback);
    return objTransformFeedback;
}

napi_value WebGL2RenderingContextImpl::DeleteTransformFeedback(napi_env env, napi_value obj)
{
    uint32_t transformFeedbackId = WebGLTransformFeedback::DEFAULT_TRANSFORM_FEEDBACK;
    WebGLTransformFeedback* webGlTransformFeedback = WebGLObject::GetObjectInstance<WebGLTransformFeedback>(env, obj);
    if (webGlTransformFeedback == nullptr) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    transformFeedbackId = webGlTransformFeedback->GetTransformFeedback();
    DeleteObject<WebGLTransformFeedback>(env, transformFeedbackId);
    glDeleteTransformFeedbacks(1, &transformFeedbackId);
    LOGI("WebGL2 deleteTransformFeedback transformFeedbackId %{public}u", transformFeedbackId);
    if (boundTransformFeedback_ != transformFeedbackId) {
        LOGE("WebGL2 bindTransformFeedback bound %{public}u", boundTransformFeedback_);
    }
    boundTransformFeedback_ = 0;
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::IsTransformFeedback(napi_env env, napi_value obj)
{
    GLuint transformFeedbackId = WebGLTransformFeedback::DEFAULT_TRANSFORM_FEEDBACK;
    WebGLTransformFeedback* webGlTransformFeedback = WebGLObject::GetObjectInstance<WebGLTransformFeedback>(env, obj);
    if (webGlTransformFeedback == nullptr) {
        return NVal::CreateBool(env, false).val_;
    }
    transformFeedbackId = webGlTransformFeedback->GetTransformFeedback();
    LOGI("WebGL2 isTransformFeedback transformFeedbackId %{public}u bound %{public}u",
        transformFeedbackId, boundTransformFeedback_);
    if (GetObjectInstance<WebGLTransformFeedback>(env, transformFeedbackId) != nullptr &&
        boundTransformFeedback_ == transformFeedbackId) {
        return NVal::CreateBool(env, true).val_;
    }
    return NVal::CreateBool(env, false).val_;
}

napi_value WebGL2RenderingContextImpl::BindTransformFeedback(napi_env env, napi_value obj, GLenum target)
{
    GLuint transformFeedbackId = WebGLTransformFeedback::DEFAULT_TRANSFORM_FEEDBACK;
    WebGLTransformFeedback* webGlTransformFeedback = WebGLObject::GetObjectInstance<WebGLTransformFeedback>(env, obj);
    if (webGlTransformFeedback == nullptr) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_OPERATION);
        return nullptr;
    }
    transformFeedbackId = webGlTransformFeedback->GetTransformFeedback();
    LOGE("WebGL2 bindTransformFeedback target %{public}u transformFeedbackId %{public}u", target, transformFeedbackId);
    if (target != GL_TRANSFORM_FEEDBACK) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }
    if (boundTransformFeedback_ && boundTransformFeedback_ != transformFeedbackId) {
        LOGE("WebGL2 bindTransformFeedback has been bound %{public}u", boundTransformFeedback_);

    }
    boundTransformFeedback_ = transformFeedbackId;
    glBindTransformFeedback(target, transformFeedbackId);
    LOGI("WebGL2 bindTransformFeedback transformFeedbackId %{public}u target %{public}u", transformFeedbackId, target);
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::BeginTransformFeedback(napi_env env, GLenum primitiveMode)
{
    LOGI("WebGL2 beginTransformFeedback primitiveMode %{public}u", primitiveMode);
    if (!CheckInList(env, primitiveMode,
            { WebGLRenderingContextBase::POINTS, WebGLRenderingContextBase::LINES,
                WebGLRenderingContextBase::TRIANGLES })) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }
    glBeginTransformFeedback(primitiveMode);
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::EndTransformFeedback(napi_env env)
{
    LOGI("WebGL2 endTransformFeedback");
    glEndTransformFeedback();
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::GetTransformFeedbackVarying(napi_env env, napi_value programObj, GLuint index)
{
    GLuint programId = WebGLProgram::DEFAULT_PROGRAM_ID;
    WebGLProgram* webGLProgram = WebGLProgram::GetObjectInstance(env, programObj);
    if (webGLProgram == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    programId = webGLProgram->GetProgramId();

    WebGLActiveInfo* webGLActiveInfo = nullptr;
    napi_value objActiveInfo = WebGLActiveInfo::CreateObjectInstance(env, &webGLActiveInfo).val_;
    if (!objActiveInfo) {
        return NVal::CreateNull(env).val_;
    }

    GLsizei bufSize = WEBGL_ACTIVE_INFO_NAME_MAX_LENGTH;
    GLsizei length;
    GLsizei size;
    GLenum type;
    GLchar name[WEBGL_ACTIVE_INFO_NAME_MAX_LENGTH] = { 0 };
    LOGI("WebGL2 getTransformFeedbackVarying index = %{public}u", index);
    glGetTransformFeedbackVarying(programId, index, bufSize, &length, &size, &type, name);
    if (length > WEBGL_ACTIVE_INFO_NAME_MAX_LENGTH) {
        LOGE("WebGL: GetTransformFeedbackVarying: [error] bufSize exceed!");
    }
    webGLActiveInfo->SetActiveName(name);
    webGLActiveInfo->SetActiveSize(size);
    webGLActiveInfo->SetActiveType(type);
    return objActiveInfo;
}

napi_value WebGL2RenderingContextImpl::TexStorage2D(napi_env env, const TexStorageArg& arg)
{
    LOGI("WebGL2 texStorage2D target %{public}u", arg.target);
    if (arg.target != GL_TEXTURE_2D && arg.target != GL_TEXTURE_CUBE_MAP) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }
    GLenum result = CheckTexStorage(env, arg);
    if (result != WebGLRenderingContextBase::NO_ERROR) {
        SET_ERROR(result);
        return nullptr;
    }
    WebGLTexture* texture = GetBoundTexture(env, arg.target, false);
    if (texture == nullptr) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    glTexStorage2D(arg.target, arg.levels, arg.internalFormat, arg.width, arg.height);
    texture->SetTexStorageInfo(&arg);
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::TexStorage3D(napi_env env, const TexStorageArg& arg)
{
    if (arg.target != GL_TEXTURE_3D && arg.target != GL_TEXTURE_2D_ARRAY) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }
    GLenum result = CheckTexStorage(env, arg);
    if (result != WebGLRenderingContextBase::NO_ERROR) {
        SET_ERROR(result);
        return nullptr;
    }
    WebGLTexture* texture = GetBoundTexture(env, arg.target, false);
    if (texture == nullptr) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    glTexStorage3D(arg.target, arg.levels, arg.internalFormat, arg.width, arg.height, arg.depth);
    texture->SetTexStorageInfo(&arg);
    return nullptr;
}

GLenum WebGL2RenderingContextImpl::CheckTexImage3D(napi_env env, const TexImageArg& imgArg)
{
    switch (imgArg.target) {
        case GL_TEXTURE_3D:
        case GL_TEXTURE_2D_ARRAY:
            break;
        default:
            return WebGLRenderingContextBase::INVALID_ENUM;
    }
    if (!CheckTextureLevel(imgArg.target, imgArg.level)) {
        return WebGLRenderingContextBase::INVALID_ENUM;
    }
    if (!WebGLTexture::CheckTextureFormatAndType(
            imgArg.internalFormat, imgArg.format, imgArg.type, imgArg.level, true)) {
        return WebGLRenderingContextBase::INVALID_VALUE;
    }
    return WebGLRenderingContextBase::NO_ERROR;
}

napi_value WebGL2RenderingContextImpl::TexImage3D(napi_env env, const TexImageArg& arg, napi_value source)
{
    TexImageArg imgArg(arg);
    imgArg.Dump("WebGL2 texImage3D");
    GLenum result = CheckTexImage3D(env, imgArg);
    if (result) {
        SET_ERROR(result);
        return nullptr;
    }
    GLvoid* data = nullptr;
    WebGLImageSource imageSource(env, version_);
    if (!NVal(env, source).IsNull()) {
        result = imageSource.GenImageSource({ imgArg.format, imgArg.type, imgArg.width, imgArg.height }, source);
        if (result) {
            SET_ERROR(result);
            return nullptr;
        }
        data = imageSource.GetImageSourceData(unpackFlipY_, unpackPremultiplyAlpha_);
        imgArg.width = imageSource.GetWidth();
        imgArg.height = imageSource.GetHeight();
    }

    glTexImage3D(imgArg.target, imgArg.level, imgArg.internalFormat, imgArg.width, imgArg.height, imgArg.depth,
        imgArg.border, imgArg.format, imgArg.type, data);
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::TexImage3D(
    napi_env env, const TexImageArg& imgArg, napi_value dataObj, GLuint srcOffset)
{
    imgArg.Dump("WebGL2 texImage3D");
    GLenum result = CheckTexImage3D(env, imgArg);
    if (result) {
        SET_ERROR(result);
        return nullptr;
    }

    WebGLReadBufferArg bufferData(env);
    GLvoid* data = nullptr;
    if (!NVal(env, dataObj).IsNull()) {
        napi_status status = bufferData.GenBufferData(dataObj);
        if (status != napi_ok) {
            SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
            return nullptr;
        }
        data = reinterpret_cast<void*>(bufferData.GetBuffer());
    }
    if (data != nullptr) {
        GLenum result = CheckTextureDataBuffer(imgArg, &bufferData);
        if (result) {
            SET_ERROR(result);
            return nullptr;
        }
    }
    glTexImage3D(imgArg.target, imgArg.level, imgArg.internalFormat, imgArg.width, imgArg.height, imgArg.depth,
        imgArg.border, imgArg.format, imgArg.type, data);
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::TexImage3D(napi_env env, const TexImageArg& imgArg, GLintptr pboOffset)
{
    imgArg.Dump("WebGL2 texImage3D");
    GLenum result = CheckTexImage3D(env, imgArg);
    if (result) {
        SET_ERROR(result);
        return nullptr;
    }
    glTexImage3D(imgArg.target, imgArg.level, imgArg.internalFormat, imgArg.width, imgArg.height, imgArg.depth,
        imgArg.border, imgArg.format, imgArg.type, reinterpret_cast<void*>(pboOffset));
    return nullptr;
}

GLenum WebGL2RenderingContextImpl::CheckTexSubImage3D(napi_env env, const TexSubImage3DArg& arg)
{
    switch (arg.target) {
        case GL_TEXTURE_3D:
        case GL_TEXTURE_2D_ARRAY:
            break;
        default:
            return WebGLRenderingContextBase::INVALID_ENUM;
    }

    WebGLTexture* texture = GetBoundTexture(env, arg.target, false);
    if (texture == nullptr) {
        return WebGLRenderingContextBase::INVALID_VALUE;
    }

    if (!CheckTextureLevel(arg.target, arg.level)) {
        return WebGLRenderingContextBase::INVALID_ENUM;
    }

    if (!texture->CheckValid(arg.target, arg.level)) {
        return WebGLRenderingContextBase::INVALID_OPERATION;
    }

    if (!WebGLTexture::CheckTextureSize(arg.xOffset, arg.width, texture->GetWidth(arg.target, arg.level)) ||
        !WebGLTexture::CheckTextureSize(arg.yOffset, arg.height, texture->GetHeight(arg.target, arg.level)) ||
        !WebGLTexture::CheckTextureSize(arg.zOffset, arg.depth, texture->GetDepth(arg.target, arg.level))) {
        return WebGLRenderingContextBase::INVALID_VALUE;
    }
    GLenum internalFormat = texture->GetInternalFormat(arg.target, arg.level);
    if (!WebGLTexture::CheckTextureFormatAndType(internalFormat, arg.format, arg.type, arg.level, true)) {
        return WebGLRenderingContextBase::INVALID_VALUE;
    }
    return WebGLRenderingContextBase::NO_ERROR;
}

napi_value WebGL2RenderingContextImpl::TexSubImage3D(napi_env env, const TexSubImage3DArg& arg, napi_value source)
{
    TexSubImage3DArg imgArg(arg);
    imgArg.Dump("WebGL2 texSubImage3D");

    GLvoid* data = nullptr;
    WebGLImageSource imageSource(env, version_);
    if (!NVal(env, source).IsNull()) {
        GLenum result = imageSource.GenImageSource({ imgArg.format, imgArg.type, imgArg.width, imgArg.height }, source);
        if (result) {
            SET_ERROR(result);
            return nullptr;
        }
        data = imageSource.GetImageSourceData(unpackFlipY_, unpackPremultiplyAlpha_);
        imgArg.width = imageSource.GetWidth();
        imgArg.height = imageSource.GetHeight();
    } else {
        SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    GLenum result = CheckTexSubImage3D(env, imgArg);
    if (result) {
        SET_ERROR(result);
        return nullptr;
    }

    glTexSubImage3D(imgArg.target, imgArg.level, imgArg.xOffset, imgArg.yOffset, imgArg.zOffset, imgArg.width,
        imgArg.height, imgArg.depth, imgArg.format, imgArg.type, data);
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::TexSubImage3D(
    napi_env env, const TexSubImage3DArg& imgArg, napi_value dataObj, GLuint srcOffset)
{
    imgArg.Dump("WebGL2 texSubImage3D");
    WebGLImageSource imageSource(env, version_);
    GLvoid* data = nullptr;
    if (!NVal(env, dataObj).IsNull()) {
        GLenum result = imageSource.GenImageSource({ imgArg.format, imgArg.type, imgArg.width, imgArg.height }, dataObj, srcOffset);
        if (result) {
            SET_ERROR(result);
            return nullptr;
        }
        data = imageSource.GetImageSourceData(unpackFlipY_, unpackPremultiplyAlpha_);
    } else {
        SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }

    GLenum result = CheckTexSubImage3D(env, imgArg);
    if (result) {
        SET_ERROR(result);
        return nullptr;
    }

    glTexSubImage3D(imgArg.target, imgArg.level, imgArg.xOffset, imgArg.yOffset, imgArg.zOffset, imgArg.width,
        imgArg.height, imgArg.depth, imgArg.format, imgArg.type, data);
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::TexSubImage3D(napi_env env, const TexSubImage3DArg& imgArg, GLintptr pboOffset)
{
    // TODO
    imgArg.Dump("WebGL2 texSubImage3D");
    GLenum result = CheckTexSubImage3D(env, imgArg);
    if (result) {
        SET_ERROR(result);
        return nullptr;
    }

    glTexSubImage3D(imgArg.target, imgArg.level, imgArg.xOffset, imgArg.yOffset, imgArg.zOffset, imgArg.width,
        imgArg.height, imgArg.depth, imgArg.format, imgArg.type, reinterpret_cast<void*>(pboOffset));
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::CopyTexSubImage3D(napi_env env, const CopyTexSubImage3DArg& imgArg)
{
    imgArg.Dump("WebGL2 copyTexSubImage3D");
    GLenum result = CheckCopyTexSubImage(env, imgArg);
    if (result) {
        SET_ERROR(result);
        return nullptr;
    }
    GLuint frameBufferId = 0;
    if (!CheckReadBufferAndGetInfo(env, frameBufferId, nullptr, nullptr)) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_OPERATION);
        return nullptr;
    }

    // TODO clearIfComposited();
    // ScopedDrawingBufferBinder binder(drawingBuffer(), readFramebufferBinding);

    glCopyTexSubImage3D(imgArg.target, imgArg.level, imgArg.xOffset, imgArg.yOffset, imgArg.zOffset, imgArg.x, imgArg.y,
        imgArg.width, imgArg.height);
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::CompressedTexImage3D(
    napi_env env, const TexImageArg& imgArg, GLsizei imageSize, GLintptr offset)
{
    imgArg.Dump("WebGL2 compressedTexImage3D");
    WebGLTexture* texture = GetBoundTexture(env, imgArg.target, true);
    if (texture == nullptr) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    if (texture->CheckImmutable()) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_OPERATION);
        return nullptr;
    }

    glCompressedTexImage3D(imgArg.target, imgArg.level, imgArg.internalFormat, imgArg.width, imgArg.height,
        imgArg.depth, imgArg.border, imageSize, reinterpret_cast<void*>(offset));
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::CompressedTexImage3D(
    napi_env env, const TexImageArg& imgArg, napi_value dataObj, GLuint srcOffset, GLuint srcLengthOverride)
{
    imgArg.Dump("WebGL2 compressedTexImage3D");

    WebGLTexture* texture = GetBoundTexture(env, imgArg.target, true);
    if (texture == nullptr) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    if (texture->CheckImmutable()) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_OPERATION);
        return nullptr;
    }
    WebGLReadBufferArg readData(env);
    GLvoid* data = nullptr;
    GLsizei length = 0;
    if (NVal(env, dataObj).IsNull()) {
        napi_status status = readData.GenBufferData(dataObj, BUFFER_DATA_FLOAT_32);
        if (status != napi_ok) {
            SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
            return nullptr;
        }
        readData.DumpBuffer(readData.GetBufferDataType());
        data = reinterpret_cast<void*>(readData.GetBuffer() + srcOffset);
        length = static_cast<GLsizei>((srcLengthOverride == 0) ? readData.GetBufferLength() : srcLengthOverride);
    }
    glCompressedTexImage3D(imgArg.target, imgArg.level, imgArg.internalFormat, imgArg.width, imgArg.height,
        imgArg.depth, imgArg.border, length, data);
    texture->SetTextureLevel({ imgArg.target, imgArg.level, imgArg.internalFormat, imgArg.width, imgArg.height,
        imgArg.depth, GL_UNSIGNED_BYTE });
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::CompressedTexSubImage3D(
    napi_env env, const TexSubImage3DArg& imgArg, GLsizei imageSize, GLintptr offset)
{
    imgArg.Dump("WebGL2 compressedTexSubImage3D");
    WebGLTexture* texture = GetBoundTexture(env, imgArg.target, true);
    if (texture == nullptr) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    if (imgArg.format != texture->GetInternalFormat(imgArg.target, imgArg.level)) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_OPERATION);
        return nullptr;
    }
    glCompressedTexSubImage3D(imgArg.target, imgArg.level, imgArg.xOffset, imgArg.yOffset, imgArg.zOffset, imgArg.width,
        imgArg.height, imgArg.depth, imgArg.format, imageSize, reinterpret_cast<void*>(offset));
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::CompressedTexSubImage3D(
    napi_env env, const TexSubImage3DArg& imgArg, napi_value dataObj, GLuint srcOffset, GLuint srcLengthOverride)
{
    imgArg.Dump("WebGL2 compressedTexSubImage3D");
    WebGLTexture* texture = GetBoundTexture(env, imgArg.target, true);
    if (texture == nullptr) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    if (imgArg.format != texture->GetInternalFormat(imgArg.target, imgArg.level)) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_OPERATION);
        return nullptr;
    }

    WebGLReadBufferArg readData(env);
    GLvoid* data = nullptr;
    GLsizei length = 0;
    if (NVal(env, dataObj).IsNull()) {
        napi_status status = readData.GenBufferData(dataObj, BUFFER_DATA_FLOAT_32);
        if (status != napi_ok) {
            SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
            return nullptr;
        }
        readData.DumpBuffer(readData.GetBufferDataType());
        data = reinterpret_cast<void*>(readData.GetBuffer() + srcOffset);
        length = static_cast<GLsizei>((srcLengthOverride == 0) ? readData.GetBufferLength() : srcLengthOverride);
    }

    glCompressedTexSubImage3D(imgArg.target, imgArg.level, imgArg.xOffset, imgArg.yOffset, imgArg.zOffset, imgArg.width,
        imgArg.height, imgArg.depth, imgArg.format, length, data);
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::ClearBufferV(
    napi_env env, GLenum buffer, GLint drawBuffer, napi_value value, int64_t srcOffset, BufferDataType type)
{
    LOGI("WebGL2 clearBuffer buffer %{public}u %{public}d srcOffset %{public}u", buffer, drawBuffer, srcOffset);
    WebGLReadBufferArg bufferData(env);
    napi_status status = bufferData.GenBufferData(value, type);
    if (status != 0) {
        LOGE("WebGL2 clearBuffer failed to getbuffer data");
        SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    if (bufferData.GetBufferDataType() != type) {
        LOGE("WebGL2 clearBuffer invalid buffer data type %{public}d", bufferData.GetBufferDataType());
        SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }

    GLenum result = CheckClearBuffer(env, buffer, bufferData);
    if (result != WebGLRenderingContextBase::NO_ERROR) {
        LOGE("WebGL2 clearBuffer invalid clear buffer");
        SET_ERROR(result);
        return nullptr;
    }

    bufferData.DumpBuffer(bufferData.GetBufferDataType());
    switch (type) {
        case BUFFER_DATA_FLOAT_32:
            glClearBufferfv(buffer, drawBuffer, reinterpret_cast<GLfloat*>(bufferData.GetBuffer() + srcOffset));
            break;
        case BUFFER_DATA_INT_32:
            glClearBufferiv(buffer, drawBuffer, reinterpret_cast<GLint*>(bufferData.GetBuffer() + srcOffset));
            break;
        case BUFFER_DATA_UINT_32:
            glClearBufferuiv(buffer, drawBuffer, reinterpret_cast<GLuint*>(bufferData.GetBuffer() + srcOffset));
            break;
        default:
            break;
    }
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::ClearBufferfi(
    napi_env env, GLenum buffer, GLint drawBuffer, GLfloat depth, GLint stencil)
{
    LOGE("WebGL2 clearBuffer buffer %{public}u %{public}d depth %{public}f %{public}d", buffer, drawBuffer, depth,
        stencil);
    if (buffer != WebGLRenderingContextBase::DEPTH_STENCIL) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }
    glClearBufferfi(buffer, drawBuffer, depth, stencil);
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::GetIndexedParameter(napi_env env, GLenum target, GLuint index)
{
    LOGI("WebGL2 getIndexedParameter index = %{public}u", index);
    if (target == GL_TRANSFORM_FEEDBACK_BUFFER_BINDING || target == GL_UNIFORM_BUFFER_BINDING) {
        return GetObject<WebGLBuffer>(env, index);
    } else if (target == GL_TRANSFORM_FEEDBACK_BUFFER_SIZE || target == GL_UNIFORM_BUFFER_SIZE) {
        int64_t data;
        glGetInteger64i_v(target, index, &data);
        LOGI("WebGL getIndexedParameter end");
        return NVal::CreateInt64(env, data).val_;
    } else if (target == GL_UNIFORM_BUFFER_SIZE || target == GL_UNIFORM_BUFFER_START) {
        int64_t data;
        glGetInteger64i_v(target, index, &data);
        LOGI("WebGL getIndexedParameter end");
        return NVal::CreateInt64(env, data).val_;
    } else {
        LOGI("WebGL getIndexedParameter end");
        return nullptr;
    }
}

napi_value WebGL2RenderingContextImpl::GetFragDataLocation(napi_env env, napi_value programObj, const std::string& name)
{
    GLuint program = WebGLProgram::DEFAULT_PROGRAM_ID;
    WebGLProgram* webGLProgram = WebGLProgram::GetObjectInstance(env, programObj);
    if (webGLProgram == nullptr) {
        return NVal::CreateInt64(env, -1).val_;
        ;
    }
    program = webGLProgram->GetProgramId();

    LOGI("WebGL2 getFragDataLocation name %{public}s", name.c_str());
    GLint res = glGetFragDataLocation(program, const_cast<char*>(name.c_str()));
    LOGI("WebGL2 getFragDataLocation result %{public}d", res);
    return NVal::CreateInt64(env, res).val_;
}

napi_value WebGL2RenderingContextImpl::VertexAttribI4i(napi_env env, GLuint index, GLint* data)
{
    VertexAttribInfo* info = GetVertexAttribInfo(index);
    if (info == nullptr) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    LOGI("WebGL2 vertexAttribI4i index %{public}u [%{public}d %{public}d %{public}d %{public}d]",
        index, data[0], data[1], data[2], data[3]); // 2, 3 index for data
    glVertexAttribI4i(index, data[0], data[1], data[2], data[3]); // 2, 3 index for data
    info->type = BUFFER_DATA_INT_32;
    LOGI("WebGL2 vertexAttribI4i result %{public}u", GetError_());
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::VertexAttribI4ui(napi_env env, GLuint index, GLuint* data)
{
    VertexAttribInfo* info = GetVertexAttribInfo(index);
    if (info == nullptr) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    LOGI("WebGL2 vertexAttribI4ui index %{public}u [%{public}u %{public}u %{public}u %{public}u]",
        index, data[0], data[1], data[2], data[3]); // 2, 3 index for data
    glVertexAttribI4ui(index, data[0], data[1], data[2], data[3]); // 2, 3 index for data
    info->type = BUFFER_DATA_UINT_32;
    LOGI("WebGL2 vertexAttribI4ui result %{public}u", GetError_());
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::VertexAttribI4iv(napi_env env, GLuint index, napi_value data)
{
    VertexAttribInfo* info = GetVertexAttribInfo(index);
    if (info == nullptr) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }

    WebGLReadBufferArg bufferData(env);
    napi_status status = bufferData.GenBufferData(data, BUFFER_DATA_INT_32);
    if (status != 0) {
        LOGE("WebGL vertexAttribI4iv failed to getbuffer data");
        return nullptr;
    }
    bufferData.DumpBuffer(bufferData.GetBufferDataType());
    if (bufferData.GetBufferDataType() != BUFFER_DATA_INT_32) {
        LOGE("WebGL vertexAttribI4iv invalid data type %{public}u", bufferData.GetBufferDataType());
        return nullptr;
    }
    glVertexAttribI4iv(index, reinterpret_cast<GLint*>(bufferData.GetBuffer()));
    info->type = BUFFER_DATA_INT_32;
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::VertexAttribI4uiv(napi_env env, GLuint index, napi_value data)
{
    VertexAttribInfo* info = GetVertexAttribInfo(index);
    if (info == nullptr) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    WebGLReadBufferArg bufferData(env);
    napi_status status = bufferData.GenBufferData(data, BUFFER_DATA_UINT_32);
    if (status != 0) {
        LOGE("WebGL vertexAttribI4uiv failed to getbuffer data");
        return nullptr;
    }
    bufferData.DumpBuffer(bufferData.GetBufferDataType());
    if (bufferData.GetBufferDataType() != BUFFER_DATA_UINT_32) {
        LOGE("WebGL2 vertexAttribI4uiv invalid data type %{public}u", bufferData.GetBufferDataType());
        return nullptr;
    }
    glVertexAttribI4uiv(index, reinterpret_cast<const GLuint*>(bufferData.GetBuffer()));
    info->type = BUFFER_DATA_UINT_32;
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::VertexAttribIPointer(napi_env env, const VertexAttribArg* vertexInfo)
{
    LOGI("WebGL vertexAttribPointer index %{public}u size %{public}u type %{public}u", vertexInfo->index,
        vertexInfo->size, vertexInfo->type);

    if (!CheckGLenum(env, vertexInfo->type,
            { GL_BYTE, GL_UNSIGNED_BYTE, GL_SHORT, GL_UNSIGNED_SHORT, GL_INT, GL_UNSIGNED_INT }, {})) {
        LOGE("WebGL vertexAttribPointer invalid type %{public}d", vertexInfo->type);
        SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
    }

    GLenum result = CheckVertexAttribPointer(env, vertexInfo);
    if (result) {
        SET_ERROR(result);
        return nullptr;
    }

    glVertexAttribIPointer(vertexInfo->index, vertexInfo->size, vertexInfo->type, vertexInfo->stride,
        reinterpret_cast<GLvoid*>(vertexInfo->offset));
    LOGI("WebGL vertexAttribPointer index %{public}u offset %{public}u", vertexInfo->index, vertexInfo->offset);
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::VertexAttribDivisor(napi_env env, GLuint index, GLuint divisor)
{
    if (index >= GetMaxVertexAttribs()) {
        LOGE("WebGL2 vertexAttribDivisor invalid index %{public}d", index);
        SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    glVertexAttribDivisor(index, divisor);
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::DrawBuffers(napi_env env, napi_value dataObj)
{
    WebGLReadBufferArg bufferData(env);
    napi_status status = bufferData.GenBufferData(dataObj, BUFFER_DATA_GLENUM);
    if (status != 0) {
        LOGE("WebGL glDrawBuffers failed to getbuffer data");
        return nullptr;
    }
    if (bufferData.GetBufferType() != BUFFER_ARRAY) {
        LOGE("WebGL glDrawBuffers invalid buffer type %d", bufferData.GetBufferType());
        return nullptr;
    }
    bufferData.DumpBuffer(bufferData.GetBufferDataType());
    GLenum* data = reinterpret_cast<GLenum*>(bufferData.GetBuffer());
    GLsizei length = static_cast<GLsizei>(bufferData.GetBufferLength() / sizeof(GLenum));
    glDrawBuffers(length, data);
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::DrawArraysInstanced(
    napi_env env, GLenum mode, GLint first, GLsizei count, GLsizei instanceCount)
{
    LOGI("WebGL drawArraysInstanced mode %{public}u %{public}u %{public}u", mode, first, count);
    GLenum result = CheckDrawArrays(env, mode, first, count);
    if (result != WebGLRenderingContextBase::NO_ERROR) {
        SET_ERROR(result);
        return nullptr;
    }
    if (instanceCount < 0) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    // TODO
    glDrawArraysInstanced(mode, first, count, instanceCount);
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::DrawElementsInstanced(
    napi_env env, const DrawElementArg& arg, GLsizei instanceCount)
{
    LOGI("WebGL2 drawElementsInstanced mode %{public}u %{public}u %{public}u", arg.mode, arg.count, arg.type);
    GLenum result = CheckDrawElements(env, arg.mode, arg.count, arg.type, static_cast<int64_t>(arg.offset));
    if (result != WebGLRenderingContextBase::NO_ERROR) {
        SET_ERROR(result);
        return nullptr;
    }
    if (instanceCount < 0) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    glDrawElementsInstanced(arg.mode, arg.count, arg.type, reinterpret_cast<GLvoid*>(arg.offset), instanceCount);
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::DrawRangeElements(
    napi_env env, const DrawElementArg& arg, GLuint start, GLuint end)
{
    LOGI("WebGL2 drawRangeElements mode %{public}u %{public}u %{public}u", arg.mode, arg.count, arg.type);
    GLenum result = CheckDrawElements(env, arg.mode, arg.count, arg.type, static_cast<int64_t>(arg.offset));
    if (result != WebGLRenderingContextBase::NO_ERROR) {
        SET_ERROR(result);
        return nullptr;
    }
    glDrawRangeElements(arg.mode, start, end, arg.count, arg.type, reinterpret_cast<GLvoid*>(arg.offset));
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::CopyBufferSubData(
    napi_env env, GLenum targets[2], int64_t readOffset, int64_t writeOffset, int64_t size)
{
    if (!WebGLArg::CheckNoneNegInt<GLint>(readOffset) || !WebGLArg::CheckNoneNegInt<GLint>(writeOffset) ||
        !WebGLArg::CheckNoneNegInt<GLint>(size)) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    WebGLBuffer* readBuffer = GetBoundBuffer(env, targets[1]);
    if (readBuffer == nullptr || readBuffer->GetBufferSize() == 0) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    WebGLBuffer* writeBuffer = GetBoundBuffer(env, targets[0]);
    if (writeBuffer == nullptr || writeBuffer->GetBufferSize() == 0) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    if (readOffset + size > readBuffer->GetBufferSize() ||
        writeOffset + size > writeBuffer->GetBufferSize()) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    if ((writeBuffer->GetTarget() == GL_ELEMENT_ARRAY_BUFFER && readBuffer->GetTarget() != GL_ELEMENT_ARRAY_BUFFER) ||
        (writeBuffer->GetTarget() != GL_ELEMENT_ARRAY_BUFFER && readBuffer->GetTarget() == GL_ELEMENT_ARRAY_BUFFER)) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_OPERATION);
        return nullptr;
    }
    if (writeBuffer->GetTarget() == 0) {
        writeBuffer->SetTarget(readBuffer->GetTarget());
    }
    glCopyBufferSubData(targets[0], targets[1], static_cast<GLintptr>(readOffset), static_cast<GLintptr>(writeOffset),
        static_cast<GLsizeiptr>(size));
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::GetBufferSubData(
    napi_env env, GLenum target, int64_t offset, napi_value data, const BufferExt& ext)
{
    LOGI("WebGL2 getBufferSubData target %{public}u %{public}d ", target, static_cast<GLint>(offset));
    WebGLReadBufferArg bufferData(env);
    napi_status status = bufferData.GenBufferData(data);
    if (status != 0) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    if (!WebGLArg::CheckNoneNegInt<GLint>(offset)) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    WebGLBuffer* writeBuffer = GetBoundBuffer(env, target);
    if (writeBuffer == nullptr || writeBuffer->GetBufferSize() == 0) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_VALUE, "writeBuffer %{public}p", writeBuffer);
        return nullptr;
    }
    if (offset + bufferData.GetBufferLength() > writeBuffer->GetBufferSize()) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    GLsizeiptr dstSize = (ext.length == 0) ? static_cast<GLsizeiptr>(bufferData.GetBufferLength())
                                           : static_cast<GLsizeiptr>(ext.length * bufferData.GetBufferDataSize());
    GLuint dstOffset = static_cast<GLuint>(ext.dstOffset * bufferData.GetBufferDataSize());
    LOGI("WebGL2 getBufferSubData target %{public}u %{public}d dstSize %{public}u dstOffset %{public}u",
        target, static_cast<GLintptr>(offset), dstSize, dstOffset);
    glBufferSubData(target,
        static_cast<GLintptr>(offset), dstSize, reinterpret_cast<void*>(bufferData.GetBuffer() + dstOffset));
    LOGI("WebGL2 getBufferSubData target %{public}u result %{public}u ", target, GetError_());
    return nullptr;
}

// 8 GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1
napi_value WebGL2RenderingContextImpl::BlitFrameBuffer(napi_env env, GLint data[8], GLbitfield mask, GLenum filter)
{
    // 0,1,2,3,4,5,6,7 srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1
    glBlitFramebuffer(data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7], mask, filter);
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::FrameBufferTextureLayer(
    napi_env env, GLenum target, GLenum attachment, napi_value textureObj, const TextureLayerArg& layer)
{
    GLuint textureId = WebGLTexture::DEFAULT_TEXTURE;
    WebGLTexture* webGlTexture = WebGLTexture::GetObjectInstance(env, textureObj);
    if (webGlTexture == nullptr) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    textureId = webGlTexture->GetTexture();
    LOGI("WebGL frameBufferTextureLayer texture %{public}u", textureId);
    glFramebufferTextureLayer(target, attachment, textureId, layer.level, layer.layer);
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::ReadBuffer(napi_env env, GLenum mode)
{
    LOGI("WebGL2 readBuffer mode %{public}u", mode);
    if (!CheckReadBufferMode(mode)) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }

    WebGLFramebuffer* readFrameBuffer = GetBoundFrameBuffer(env, WebGL2RenderingContextBase::READ_FRAMEBUFFER);
    if (!readFrameBuffer) {
        if (mode != WebGLRenderingContextBase::BACK && mode != WebGLRenderingContextBase::NONE) {
            SET_ERROR(WebGLRenderingContextBase::INVALID_OPERATION);
            return nullptr;
        }
        // TODO
        // m_readBufferOfDefaultFramebuffer = mode;
        // translate GL_BACK to GL_COLOR_ATTACHMENT0, because the default
        // framebuffer for WebGL is not fb 0, it is an internal fbo.
        if (mode == WebGLRenderingContextBase::BACK)
            mode = WebGLRenderingContextBase::COLOR_ATTACHMENT0;
    } else {
        if (mode == WebGLRenderingContextBase::BACK) {
            SET_ERROR(WebGLRenderingContextBase::INVALID_OPERATION);
            return nullptr;
        }
    }
    glReadBuffer(mode);
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::RenderBufferStorageMultiSample(
    napi_env env, const TexStorageArg& arg, GLsizei samples)
{
    arg.Dump("WebGL2 renderbufferStorageMultisample");
    WebGLRenderbuffer* renderBuffer = CheckRenderBufferStorage(env, arg);
    if (renderBuffer == nullptr) {
        return nullptr;
    }
    if (samples < 0) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    if (arg.internalFormat == GL_DEPTH_STENCIL) {
        if (samples > 0) {
            SET_ERROR(WebGLRenderingContextBase::INVALID_ENUM);
            return nullptr;
        }
        glRenderbufferStorage(arg.target, GL_DEPTH24_STENCIL8, arg.width, arg.height);
    } else {
        if (CheckInList(env, arg.internalFormat, WebGLTexture::GetSupportInternalFormatGroup1())) {
            if (samples > 0) {
                SET_ERROR(WebGLRenderingContextBase::INVALID_OPERATION);
                return nullptr;
            }
        } else if (!CheckInList(env, arg.internalFormat, WebGLTexture::GetSupportInternalFormatGroup2())) {
            SET_ERROR(WebGLRenderingContextBase::INVALID_ENUM);
            return nullptr;
        }
        if (samples == 0) {
            glRenderbufferStorage(arg.target, arg.internalFormat, arg.width, arg.height);
        } else {
            glRenderbufferStorageMultisample(arg.target, samples, arg.internalFormat, arg.width, arg.height);
        }
    }
    renderBuffer->SetInternalFormat(arg.internalFormat);
    renderBuffer->SetSize(arg.width, arg.height);
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::BindBufferBase(napi_env env, const BufferBaseArg& arg, napi_value bufferObj)
{
    GLuint bufferId = WebGLBuffer::DEFAULT_BUFFER;
    WebGLBuffer* webGlBuffer = WebGLBuffer::GetObjectInstance(env, bufferObj);
    if (webGlBuffer != nullptr) {
        bufferId = webGlBuffer->GetBufferId();
    }
    if (!CheckBufferBindTarget(arg.target)) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }
    if (!UpdateBaseTargetBoundBuffer(env, arg.target, arg.index, webGlBuffer)) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_OPERATION);
        return nullptr;
    }
    LOGI("WebGL2 bindBufferBase target %{public}u %{public}u %{public}u", arg.target, arg.index, bufferId);
    glBindBufferBase(arg.target, arg.index, bufferId);
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::BindBufferRange(
    napi_env env, const BufferBaseArg& arg, napi_value bufferObj, GLintptr offset, GLsizeiptr size)
{
    GLuint bufferId = WebGLBuffer::DEFAULT_BUFFER;
    WebGLBuffer* webGlBuffer = WebGLBuffer::GetObjectInstance(env, bufferObj);
    if (webGlBuffer != nullptr) {
        bufferId = webGlBuffer->GetBufferId();
    }
    if (!CheckBufferBindTarget(arg.target)) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }
    if (!UpdateBaseTargetBoundBuffer(env, arg.target, arg.index, webGlBuffer)) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_OPERATION);
        return nullptr;
    }
    LOGI("WebGL2 bindBufferRange target %{public}u %{public}u %{public}u", arg.target, arg.index, bufferId);
    glBindBufferRange(arg.target, arg.index, bufferId, offset, size);
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::GetUniformBlockIndex(
    napi_env env, napi_value programObj, const std::string& uniformBlockName)
{
    GLuint programId = WebGLProgram::DEFAULT_PROGRAM_ID;
    WebGLProgram* webGLProgram = WebGLProgram::GetObjectInstance(env, programObj);
    if (webGLProgram == nullptr) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
        return NVal::CreateInt64(env, -1).val_;
    }
    programId = webGLProgram->GetProgramId();
    if (!CheckString(uniformBlockName)) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
        return NVal::CreateInt64(env, -1).val_;
    }
    LOGI("WebGL2 getUniformBlockIndex programId %{public}u", programId);
    GLuint returnValue = glGetUniformBlockIndex(programId, uniformBlockName.c_str());
    return NVal::CreateInt64(env, returnValue).val_;
}

napi_value WebGL2RenderingContextImpl::UniformBlockBinding(
    napi_env env, napi_value programObj, GLuint uniformBlockIndex, GLuint uniformBlockBinding)
{
    GLuint programId = WebGLProgram::DEFAULT_PROGRAM_ID;
    WebGLProgram* webGLProgram = WebGLProgram::GetObjectInstance(env, programObj);
    if (webGLProgram == nullptr) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    programId = webGLProgram->GetProgramId();
    LOGI("WebGL2 uniformBlockBinding programId %{public}u %{public}u %{public}u",
        programId, uniformBlockIndex, uniformBlockBinding);
    glUniformBlockBinding(programId, uniformBlockIndex, uniformBlockBinding);
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::InvalidateFrameBuffer(napi_env env, GLenum target, napi_value data)
{
    LOGE("WebGL2 invalidateFramebuffer target %{public}u", target);
    WebGLReadBufferArg bufferData(env);
    napi_status status = bufferData.GenBufferData(data, BUFFER_DATA_GLENUM);
    if (status != 0) {
        LOGE("WebGL2 invalidateFramebuffer failed to getbuffer data");
        SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    bufferData.DumpBuffer(bufferData.GetBufferDataType());
    if (bufferData.GetBufferDataType() != BUFFER_DATA_GLENUM) {
        LOGE("WebGL2 invalidateFramebuffer invalid data type %{public}u", bufferData.GetBufferDataType());
        SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    if (bufferData.GetBufferType() != BUFFER_ARRAY) {
        LOGE("WebGL2 invalidateFramebuffer invalid type %{public}u", bufferData.GetBufferType());
        SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    glInvalidateFramebuffer(target, static_cast<GLsizei>(bufferData.GetBufferLength() / sizeof(GLenum)),
        reinterpret_cast<GLenum*>(bufferData.GetBuffer()));
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::InvalidateSubFrameBuffer(
    napi_env env, GLenum target, napi_value data, const BufferPosition& position, const BufferSize& size)
{
    WebGLReadBufferArg bufferData(env);
    napi_status status = bufferData.GenBufferData(data, BUFFER_DATA_GLENUM);
    if (status != 0) {
        LOGE("WebGL2 invalidateFramebuffer failed to getbuffer data");
        return nullptr;
    }
    bufferData.DumpBuffer(bufferData.GetBufferDataType());
    if (bufferData.GetBufferDataType() != BUFFER_DATA_GLENUM) {
        LOGE("WebGL2 invalidateFramebuffer invalid data type %{public}u", bufferData.GetBufferDataType());
        return nullptr;
    }
    if (bufferData.GetBufferType() != BUFFER_ARRAY) {
        LOGE("WebGL2 invalidateFramebuffer invalid type %{public}u", bufferData.GetBufferType());
        return nullptr;
    }

    glInvalidateSubFramebuffer(target, static_cast<GLsizei>(bufferData.GetBufferLength() / sizeof(GLenum)),
        reinterpret_cast<GLenum*>(bufferData.GetBuffer()), position.x, position.y, size.width, size.height);
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::GetInternalFormatParameter(
    napi_env env, GLenum target, GLenum internalFormat, GLenum pname)
{
    if (target != WebGLRenderingContextBase::RENDERBUFFER) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_ENUM);
        return NVal::CreateNull(env).val_;
    }
    LOGI("WebGL2 getInternalformatParameter target %{public}u %{public}u %{public}u", target, internalFormat, pname);
    WebGLWriteBufferArg writeBuffer(env);
    GLint length = -1;
    if (CheckInList(env, internalFormat, WebGLTexture::GetSupportInternalFormatGroup1())) {
        length = 1;
    } else if (!CheckInList(env, internalFormat, WebGLTexture::GetSupportInternalFormatGroup2())) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_ENUM);
        return NVal::CreateNull(env).val_;
    }
    if (pname == GL_SAMPLES) {
        glGetInternalformativ(target, internalFormat, GL_NUM_SAMPLE_COUNTS, 1, &length);
        LOGI("WebGL2 getInternalformatParameter length %{public}u", length);
    }
    if (length <= 0) {
        return NVal::CreateNull(env).val_;
    }
    GLint* params = reinterpret_cast<GLint*>(writeBuffer.AllocBuffer(length * sizeof(GLint)));
    if (params == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    glGetInternalformativ(target, internalFormat, pname, length, params);
    return writeBuffer.ToExternalArray(BUFFER_DATA_INT_32);
}

napi_value WebGL2RenderingContextImpl::TransformFeedbackVaryings(
    napi_env env, napi_value programObj, napi_value data, GLenum bufferMode)
{
    GLuint programId = WebGLProgram::DEFAULT_PROGRAM_ID;
    WebGLProgram* webGLProgram = WebGLProgram::GetObjectInstance(env, programObj);
    if (webGLProgram == nullptr) {
        return nullptr;
    }
    programId = webGLProgram->GetProgramId();

    std::vector<char*> list = {};
    bool succ = WebGLArg::GetStringList(env, data, list);
    if (!succ) {
        WebGLArg::FreeStringList(list);
        return nullptr;
    }
    glTransformFeedbackVaryings(programId, static_cast<GLsizei>(list.size()), list.data(), bufferMode);
    WebGLArg::FreeStringList(list);
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::GetUniformIndices(napi_env env, napi_value programObj, napi_value data)
{
    GLuint programId = WebGLProgram::DEFAULT_PROGRAM_ID;
    WebGLProgram* webGLProgram = WebGLProgram::GetObjectInstance(env, programObj);
    if (webGLProgram == nullptr) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
        return NVal::CreateNull(env).val_;
    }
    programId = webGLProgram->GetProgramId();

    std::vector<char*> list = {};
    bool succ = WebGLArg::GetStringList(env, data, list);
    if (!succ) {
        WebGLArg::FreeStringList(list);
        return NVal::CreateNull(env).val_;
    }
    LOGI("WebGL2 getUniformIndices uniformCount %{public}u", list.size());

    WebGLWriteBufferArg writeBuffer(env);
    napi_value result = NVal::CreateNull(env).val_;
    GLuint* uniformIndices = reinterpret_cast<GLuint*>(writeBuffer.AllocBuffer(list.size() * sizeof(GLuint)));
    if (uniformIndices != nullptr) {
        glGetUniformIndices(programId, static_cast<GLsizei>(list.size()), const_cast<const GLchar**>(list.data()),
            static_cast<GLuint*>(uniformIndices));
        result = writeBuffer.ToNormalArray(BUFFER_DATA_UINT_32, BUFFER_DATA_UINT_32);
    }
    WebGLArg::FreeStringList(list);
    return result;
}

napi_value WebGL2RenderingContextImpl::GetActiveUniforms(
    napi_env env, napi_value programObj, napi_value data, GLenum pname)
{
    GLuint programId = WebGLProgram::DEFAULT_PROGRAM_ID;
    WebGLProgram* webGLProgram = WebGLProgram::GetObjectInstance(env, programObj);
    if (webGLProgram == nullptr) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
        return NVal::CreateNull(env).val_;
    }
    if (pname == GL_UNIFORM_NAME_LENGTH) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_ENUM);
        return NVal::CreateNull(env).val_;
    }

    programId = webGLProgram->GetProgramId();
    LOGI("WebGL2 getActiveUniforms programId %{public}u pname %{public}u", programId, pname);
    WebGLReadBufferArg bufferData(env);
    napi_status status = bufferData.GenBufferData(data, BUFFER_DATA_UINT_32);
    if (status != 0) {
        LOGE("WebGL2 invalidateFramebuffer failed to getbuffer data");
        SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    bufferData.DumpBuffer(bufferData.GetBufferDataType());
    GLsizei size = static_cast<GLsizei>(bufferData.GetBufferLength() / bufferData.GetBufferDataSize());

    WebGLWriteBufferArg writeBuffer(env);
    GLint* params = reinterpret_cast<GLint*>(writeBuffer.AllocBuffer(size * sizeof(GLint)));
    if (params == nullptr) {
        return NVal::CreateNull(env).val_;
    }
    glGetActiveUniformsiv(
        programId, size, reinterpret_cast<GLuint*>(bufferData.GetBuffer()), pname, reinterpret_cast<GLint*>(params));
    if (pname == GL_UNIFORM_TYPE) {
        return writeBuffer.ToNormalArray(BUFFER_DATA_INT_32, BUFFER_DATA_BIGUINT_64);
    } else if (pname == GL_UNIFORM_SIZE) {
        return writeBuffer.ToNormalArray(BUFFER_DATA_INT_32, BUFFER_DATA_BIGUINT_64);
    } else if (pname == GL_UNIFORM_BLOCK_INDEX || pname == GL_UNIFORM_OFFSET || pname == GL_UNIFORM_ARRAY_STRIDE ||
               pname == GL_UNIFORM_MATRIX_STRIDE) {
        return writeBuffer.ToNormalArray(BUFFER_DATA_INT_32, BUFFER_DATA_INT_32);
    } else if (pname == GL_UNIFORM_IS_ROW_MAJOR) {
        return writeBuffer.ToNormalArray(BUFFER_DATA_INT_32, BUFFER_DATA_BOOLEAN);
    }
    return NVal::CreateNull(env).val_;
}

napi_value WebGL2RenderingContextImpl::GetActiveUniformBlockParameter(
    napi_env env, napi_value programObj, GLuint uniformBlockIndex, GLenum pname)
{
    GLuint programId = WebGLProgram::DEFAULT_PROGRAM_ID;
    WebGLProgram* webGLProgram = WebGLProgram::GetObjectInstance(env, programObj);
    if (webGLProgram == nullptr) {
        return NVal::CreateInt64(env, -1).val_;
    }
    programId = webGLProgram->GetProgramId();
    LOGI("WebGL2 getActiveUniformBlockParameter programId %{public}u %{public}u %{public}u", programId,
        uniformBlockIndex, pname);
    switch (pname) {
        case GL_UNIFORM_BLOCK_BINDING:
        case GL_UNIFORM_BLOCK_DATA_SIZE:
        case GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS: {
            GLint params;
            glGetActiveUniformBlockiv(programId, uniformBlockIndex, pname, &params);
            return NVal::CreateInt64(env, params).val_;
        }
        case GL_UNIFORM_BLOCK_REFERENCED_BY_VERTEX_SHADER:
        case GL_UNIFORM_BLOCK_REFERENCED_BY_FRAGMENT_SHADER: {
            GLint params;
            glGetActiveUniformBlockiv(programId, uniformBlockIndex, pname, &params);
            return NVal::CreateBool(env, params != GL_FALSE).val_;
        }
        case GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES: {
            WebGLWriteBufferArg writeBuffer(env);
            GLint uniformCount = 1;
            glGetActiveUniformBlockiv(programId, uniformBlockIndex, GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS, &uniformCount);
            LOGI("WebGL2 getActiveUniformBlockParameter uniformCount %{public}d", uniformCount);
            GLint* params = reinterpret_cast<GLint*>(writeBuffer.AllocBuffer(uniformCount * sizeof(GLint)));
            if (params == nullptr) {
                return nullptr;
            }
            glGetActiveUniformBlockiv(programId, uniformBlockIndex, pname, params);
            return writeBuffer.ToNormalArray(BUFFER_DATA_INT_32, BUFFER_DATA_UINT_32);
        }
        default:
            SET_ERROR(WebGLRenderingContextBase::INVALID_ENUM);
            return nullptr;
    }
    return NVal::CreateInt64(env, -1).val_;
}

napi_value WebGL2RenderingContextImpl::GetActiveUniformBlockName(
    napi_env env, napi_value programObj, GLuint uniformBlockIndex)
{
    GLuint programId = WebGLProgram::DEFAULT_PROGRAM_ID;
    WebGLProgram* webGLProgram = WebGLProgram::GetObjectInstance(env, programObj);
    if (webGLProgram == nullptr) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
        return NVal::CreateUTF8String(env, "").val_;
    }
    programId = webGLProgram->GetProgramId();

    GLint length = 0;
    GLsizei size = 0;
    glGetProgramiv(programId, GL_ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH, &length);
    if (length <= 0) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
        return NVal::CreateUTF8String(env, "").val_;
    }
    std::unique_ptr<char[]> buf = std::make_unique<char[]>(length + 1);
    if (buf == nullptr) {
        return nullptr;
    }
    glGetActiveUniformBlockName(programId, uniformBlockIndex, length, &size, buf.get());
    LOGI("WebGL2 getActiveUniformBlockName programId %{public}u name %{public}s size %{public}d", programId, buf.get(),
        size);
    string str(buf.get(), size);
    return NVal::CreateUTF8String(env, str).val_;
}

GLenum WebGL2RenderingContextImpl::CheckClearBuffer(napi_env env, GLenum buffer, const WebGLReadBufferArg& bufferData)
{
    size_t size = bufferData.GetBufferLength() / bufferData.GetBufferDataSize();
    switch (buffer) {
        case WebGL2RenderingContextBase::COLOR:
            /*
            case WebGLRenderingContextBase::FRONT:
            case WebGLRenderingContextBase::BACK:
            case WebGLRenderingContextBase::FRONT_AND_BACK:
            */
            if (size < 4) {
                return WebGLRenderingContextBase::INVALID_VALUE;
            }
            break;
        case WebGL2RenderingContextBase::DEPTH:
        case WebGL2RenderingContextBase::STENCIL:
        case WebGLRenderingContextBase::DEPTH_STENCIL:
            if (size < 1) {
                return WebGLRenderingContextBase::INVALID_VALUE;
            }
            break;
        default:
            return WebGLRenderingContextBase::INVALID_ENUM;
    }
    return WebGLRenderingContextBase::NO_ERROR;
}

bool WebGL2RenderingContextImpl::CheckQueryTarget(napi_env env, GLenum target, uint32_t& index)
{
    switch (target) {
        case WebGL2RenderingContextBase::ANY_SAMPLES_PASSED:
        case WebGL2RenderingContextBase::ANY_SAMPLES_PASSED_CONSERVATIVE:
            index = BoundQueryType::ANY_SAMPLES_PASSED;
            break;
        case WebGL2RenderingContextBase::TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN:
            index = BoundQueryType::TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN;
            break;
        default:
            return false;
    }
    return true;
}

GLenum WebGL2RenderingContextImpl::CheckTexStorage(napi_env env, const TexStorageArg& arg)
{
    WebGLTexture* texture = GetBoundTexture(env, arg.target, false);
    if (!texture) {
        return WebGLRenderingContextBase::INVALID_VALUE;
    }

    // internalFormat check
    if (!CheckStorageInternalFormat(arg.internalFormat)) {
        return WebGLRenderingContextBase::INVALID_ENUM;
    }

    if (arg.width <= 0 || arg.height <= 0 || arg.depth <= 0) {
        return WebGLRenderingContextBase::INVALID_VALUE;
    }

    if (arg.levels <= 0) {
        return WebGLRenderingContextBase::INVALID_VALUE;
    }

    if (arg.target == GL_TEXTURE_3D) {
        if (arg.levels > log2(std::max(std::max(arg.width, arg.height), arg.depth)) + 1) {
            return WebGLRenderingContextBase::INVALID_OPERATION;
        }
    } else {
        if (arg.levels > log2(std::max(arg.width, arg.height)) + 1) {
            return WebGLRenderingContextBase::INVALID_OPERATION;
        }
    }
    return WebGLRenderingContextBase::NO_ERROR;
}

bool WebGL2RenderingContextImpl::CheckBufferBindTarget(GLenum target)
{
    if (target == GL_TRANSFORM_FEEDBACK_BUFFER || target == GL_UNIFORM_BUFFER) {
        return true;
    }
    return false;
}

bool WebGL2RenderingContextImpl::CheckBufferTargetCompatibility(napi_env env, GLenum target, WebGLBuffer* buffer)
{
    switch (buffer->GetTarget()) {
        case GL_ELEMENT_ARRAY_BUFFER:
            switch (target) {
                case GL_ARRAY_BUFFER:
                case GL_PIXEL_PACK_BUFFER:
                case GL_PIXEL_UNPACK_BUFFER:
                case GL_TRANSFORM_FEEDBACK_BUFFER:
                case GL_UNIFORM_BUFFER:
                    return false;
                default:
                    break;
            }
            break;
        case GL_ARRAY_BUFFER:
        case GL_COPY_READ_BUFFER:
        case GL_COPY_WRITE_BUFFER:
        case GL_PIXEL_PACK_BUFFER:
        case GL_PIXEL_UNPACK_BUFFER:
        case GL_UNIFORM_BUFFER:
        case GL_TRANSFORM_FEEDBACK_BUFFER:
            if (target == GL_ELEMENT_ARRAY_BUFFER) {
                return false;
            }
            break;
        default:
            break;
    }

    if (target == GL_TRANSFORM_FEEDBACK_BUFFER) {
        for (size_t i = 0; i < sizeof(boundBufferIds_) / boundBufferIds_[0]; i++) {
            if (i == BoundBufferType::TRANSFORM_FEEDBACK_BUFFER) {
                continue;
            }
            if (boundBufferIds_[i] == buffer->GetBufferId()) {
                return false;
            }
        }
        for (size_t i = 0; i <= maxBoundUniformBufferIndex_; ++i) {
            if (boundIndexedUniformBuffers_[i] == buffer->GetBufferId()) {
                return false;
            }
        }
    } else {
        // has bound
        if (GetBoundBuffer(env, WebGL2RenderingContextBase::TRANSFORM_FEEDBACK_BUFFER) != nullptr) {
            return false;
        }
        for (size_t i = 0; i < boundIndexedTransformFeedbackBuffers_.size(); ++i) {
            if (boundIndexedTransformFeedbackBuffers_[i] == buffer->GetBufferId()) {
                return false;
            }
        }
    }
    return true;
}

// validateAndUpdateBufferBindBaseTarget
bool WebGL2RenderingContextImpl::UpdateBaseTargetBoundBuffer(
    napi_env env, GLenum target, GLuint index, WebGLBuffer* buffer)
{
    if (!CheckBufferTargetCompatibility(env, target, buffer)) {
        return false;
    }
    switch (target) {
        case GL_TRANSFORM_FEEDBACK_BUFFER:
            if (index >= boundIndexedTransformFeedbackBuffers_.size()) {
                LOGE("Out of bound indexed transform feedback buffer %{public}u", index);
                return false;
            }
            boundIndexedTransformFeedbackBuffers_[index] = buffer->GetBufferId();
            boundBufferIds_[BoundBufferType::TRANSFORM_FEEDBACK_BUFFER] = buffer->GetBufferId();
            break;
        case GL_UNIFORM_BUFFER:
            if (index >= boundIndexedUniformBuffers_.size()) {
                LOGE("Out of bound indexed uniform buffer %{public}u", index);
                return false;
            }
            boundIndexedUniformBuffers_[index] = buffer->GetBufferId();
            boundBufferIds_[BoundBufferType::UNIFORM_BUFFER] = buffer->GetBufferId();
            maxBoundUniformBufferIndex_ = index > maxBoundUniformBufferIndex_ ? index : maxBoundUniformBufferIndex_;
            break;
        default:
            return false;
    }

    if (!buffer->GetTarget()) {
        buffer->SetTarget(target);
    }
    return true;
}

bool WebGL2RenderingContextImpl::CheckStorageInternalFormat(GLenum internalFormat)
{
    // if (m_supportedInternalFormatsStorage.find(internalformat) == m_supportedInternalFormatsStorage.end()) {
    //     return false;
    // }
    return true;
}

} // namespace Impl
} // namespace Rosen
} // namespace OHOS
