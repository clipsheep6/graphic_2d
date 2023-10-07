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
        LOGE("WebGL2 set error code %{public}u", error);    \
        SetError(error);      \
    } while (0)

#define SET_ERROR_WITH_LOG(error, info, ...) \
    do {                      \
        LOGE("WebGL2 set error code %{public}u" info, error, ##__VA_ARGS__);    \
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
    maxBoundTransformFeedbackBufferIndex_ = max;
    glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &max);
    maxBoundUniformBufferIndex_ = max;
    LOGD("WebGL2 Init maxBoundTransformFeedbackBufferIndex_ %{public}d", maxBoundTransformFeedbackBufferIndex_);
    LOGD("WebGL2 Init maxBoundUniformBufferIndex_ %{public}d", maxBoundUniformBufferIndex_);
    LOGD("WebGL2 Init maxSamplerUnit_ %{public}d", max);
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
    LOGD("WebGL2 createQuery queryId = %{public}u", queryId);
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
    LOGD("WebGL2 deleteQuery target %{public}u", webGlQuery->GetTarget());
    if (CheckQueryTarget(env, webGlQuery->GetTarget(), index)) {
        LOGD("WebGL2 deleteQuery currentQuery_ %{public}u", currentQuery_[index]);
        if (currentQuery_[index] == queryId) {
            currentQuery_[index] = 0;
            glEndQuery(webGlQuery->GetTarget());
        }
    }
    LOGD("WebGL2 deleteQuery queryId = %{public}u", queryId);
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
    LOGD("WebGL2 isQuery query %{public}u result %{public}d ", queryId, res);
    return NVal::CreateBool(env, res).val_;
}

napi_value WebGL2RenderingContextImpl::GetQuery(napi_env env, GLenum target, GLenum pName)
{
    LOGD("WebGL2 getQuery target %{public}u %{public}u", target, pName);
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
    LOGD("WebGL2 getQuery params = %{public}u %{public}u", params, currentQuery_[index]);
    return GetObject<WebGLQuery>(env, params);
}

napi_value WebGL2RenderingContextImpl::BeginQuery(napi_env env, GLenum target, napi_value object)
{
    LOGD("WebGL2 beginQuery target %{public}u", target);
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
    LOGD("WebGL2 beginQuery target %{public}u queryId %{public}u result %{public}u", target, queryId, GetError_());
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::EndQuery(napi_env env, GLenum target)
{
    LOGD("WebGL2 endQuery target %{public}u", target);
    uint32_t index = 0;
    if (!CheckQueryTarget(env, target, index)) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }
    if (!currentQuery_[index]) {
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
        LOGD("WebGL2 getQueryParameter params %{public}u", params);
        return NVal::CreateInt64(env, res).val_;
    } else if (pName == GL_QUERY_RESULT_AVAILABLE) {
        glGetQueryObjectuiv(queryId, pName, &params);
        bool res = (params == GL_FALSE) ? false : true;
        LOGD("WebGL2 getQueryParameter params %{public}u", params);
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
    LOGD("WebGL2 createSampler samplerId = %{public}u", samplerId);
    AddObject<WebGLSampler>(env, samplerId, objSampler);
    return objSampler;
}

napi_value WebGL2RenderingContextImpl::DeleteSampler(napi_env env, napi_value samplerObj)
{
    WebGLSampler* sampler = WebGLObject::GetObjectInstance<WebGLSampler>(env, samplerObj);
    if (sampler == nullptr) {
        return nullptr;
    }
    GLuint samplerId = sampler->GetSampler();
    LOGD("WebGL2 deleteSampler samplerId = %{public}u", samplerId);
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
        return NVal::CreateBool(env, false).val_;
    }
    GLuint samplerId = sampler->GetSampler();

    GLboolean returnValue = glIsSampler(samplerId);
    bool res = static_cast<bool>(returnValue);
    LOGD("WebGL2 IsSampler samplerId = %{public}u res %{public}u", samplerId, res);
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
    LOGD("WebGL2 bindSampler unit = %{public}u samplerId = %{public}u", unit, samplerId);
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::SamplerParameter(
    napi_env env, napi_value samplerObj, GLenum pName, bool isFloat, void* param)
{
    LOGD("WebGL2 samplerParameteri pname %{public}u param %{public}u", pName, param);
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
    LOGD("WebGL2 getSamplerParameter samplerId %{public}u params %{public}u", samplerId, pName);

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
            LOGD("WebGL2 getSamplerParameter samplerId %{public}u params %{public}u", samplerId, params);
            return NVal::CreateInt64(env, res).val_;
        }
        case GL_TEXTURE_MAX_LOD:
        case GL_TEXTURE_MIN_LOD: {
            GLfloat params;
            glGetSamplerParameterfv(static_cast<GLuint>(samplerId), static_cast<GLenum>(pName), &params);
            float res = static_cast<float>(params);
            LOGD("WebGL2 getSamplerParameter samplerId %{public}u params %{public}f", samplerId, params);
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
    LOGD("WebGL2 createVertexArray vertexArraysId = %{public}u", vertexArraysId);
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
    LOGD("WebGL2 deleteVertexArrays vertexArrays %{public}u", vertexArrays);
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
    LOGD("WebGL2 isVertexArray %{public}u %{public}u", vertexArrayId, returnValue);
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
    LOGD("WebGL2 bindVertexArray %{public}u ", vertexArrayId);
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::FenceSync(napi_env env, GLenum condition, GLbitfield flags)
{
    LOGD("WebGL2 fenceSync condition  %{public}u flags %{public}u", condition, flags);
    WebGLSync* webGlSync = nullptr;
    napi_value objSync = WebGLSync::CreateObjectInstance(env, &webGlSync).val_;
    if (!objSync) {
        return NVal::CreateNull(env).val_;
    }
    GLsync returnValue = glFenceSync(condition, flags);
    webGlSync->SetSync(reinterpret_cast<int64_t>(returnValue));
    LOGD("WebGL2 fenceSync syncId %{public}u result %{public}u", returnValue, GetError_());
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
    LOGD("WebGL2 isSync syncId = %{public}ld result %{public}u", syncId, returnValue);
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
    LOGD("WebGL2 deleteSync syncId %{public}ld ", syncId);
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
    GLuint64 timeout64 = (timeout == -1) ? GL_TIMEOUT_IGNORED : static_cast<GLuint64>(timeout);
    GLenum returnValue = glClientWaitSync(reinterpret_cast<GLsync>(syncId), flags, timeout64);
    LOGD("WebGL2 clientWaitSync syncId = %{public}u %{public}u result %{public}u", syncId, returnValue, GetError_());
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
    GLuint64 timeout64 = (timeout == -1) ? GL_TIMEOUT_IGNORED : static_cast<GLuint64>(timeout);

    glWaitSync(reinterpret_cast<GLsync>(syncId), flags, timeout64);
    LOGD("WebGL2 waitSync GL_TIMEOUT_IGNORED %{public}u", GL_TIMEOUT_IGNORED);
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
    LOGD("WebGL2 getSyncParameter syncId %{public}u pname %{public}u ", syncId, pname);
    if (CheckInList(pname, { GL_OBJECT_TYPE, GL_SYNC_STATUS, GL_SYNC_CONDITION, GL_SYNC_FLAGS })) {
        GLint value = 0;
        GLsizei length = -1;
        glGetSynciv(reinterpret_cast<GLsync>(syncId), pname, 1, &length, &value);
        LOGD("WebGL2 getSyncParameter syncId = %{public}u value %{public}d ", syncId, value);
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
    LOGD("WebGL2 createTransformFeedback transformFeedbackId %{public}u", transformFeedbackId);
    AddObject<WebGLTransformFeedback>(env, transformFeedbackId, objTransformFeedback);
    return objTransformFeedback;
}

napi_value WebGL2RenderingContextImpl::DeleteTransformFeedback(napi_env env, napi_value obj)
{
    uint32_t transformFeedbackId = WebGLTransformFeedback::DEFAULT_TRANSFORM_FEEDBACK;
    WebGLTransformFeedback* webGlTransformFeedback = WebGLObject::GetObjectInstance<WebGLTransformFeedback>(env, obj);
    if (webGlTransformFeedback == nullptr) {
        return nullptr;
    }
    transformFeedbackId = webGlTransformFeedback->GetTransformFeedback();
    DeleteObject<WebGLTransformFeedback>(env, transformFeedbackId);
    glDeleteTransformFeedbacks(1, &transformFeedbackId);
    LOGD("WebGL2 deleteTransformFeedback transformFeedbackId %{public}u", transformFeedbackId);
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
    LOGD("WebGL2 isTransformFeedback transformFeedbackId %{public}u bound %{public}u",
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
    if (webGlTransformFeedback != nullptr) {
        transformFeedbackId = webGlTransformFeedback->GetTransformFeedback();
    }
    LOGD("WebGL2 bindTransformFeedback target %{public}u id %{public}u", target, transformFeedbackId);
    if (target != GL_TRANSFORM_FEEDBACK) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }
    if (boundTransformFeedback_ && boundTransformFeedback_ != transformFeedbackId) {
        LOGE("WebGL2 bindTransformFeedback has been bound %{public}u", boundTransformFeedback_);
    }
    boundTransformFeedback_ = transformFeedbackId;
    glBindTransformFeedback(target, transformFeedbackId);
    LOGD("WebGL2 bindTransformFeedback id %{public}u result %{public}u", transformFeedbackId, GetError_());
    if (webGlTransformFeedback) {
        webGlTransformFeedback->SetTarget(target);
    }
    return nullptr;
}

static GLint GetTransformFeedbackVaryingType(GLuint programId, GLint index)
{
    GLint params = 0;
    glGetProgramiv(programId, WebGL2RenderingContextBase::TRANSFORM_FEEDBACK_VARYINGS, &params);
    LOGD("WebGL2 GetTransformFeedbackVaryingType programId %{public}u params %{public}d", programId, params);
    return params;
}

napi_value WebGL2RenderingContextImpl::BeginTransformFeedback(napi_env env, GLenum primitiveMode)
{
    LOGD("WebGL2 beginTransformFeedback primitiveMode %{public}u", primitiveMode);
    if (!CheckInList(primitiveMode, { WebGLRenderingContextBase::POINTS, WebGLRenderingContextBase::LINES,
                                        WebGLRenderingContextBase::TRIANGLES })) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }
    if (GetTransformFeedbackVaryingType(currentProgramId_, boundTransformFeedback_) <= 0) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_OPERATION);
        return nullptr;
    }
    glBeginTransformFeedback(primitiveMode);
    LOGD("WebGL2 beginTransformFeedback result %{public}u", GetError_());
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::EndTransformFeedback(napi_env env)
{
    LOGD("WebGL2 endTransformFeedback");
    if (GetTransformFeedbackVaryingType(currentProgramId_, boundTransformFeedback_) <= 0) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_OPERATION);
        return nullptr;
    }
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
    GLsizei length = 0;
    GLsizei size = 0;
    GLenum type;
    GLchar name[WEBGL_ACTIVE_INFO_NAME_MAX_LENGTH] = { 0 };
    LOGD("WebGL2 getTransformFeedbackVarying index = %{public}u", index);
    glGetTransformFeedbackVarying(programId, index, bufSize, &length, &size, &type, name);
    LOGD("WebGL2 getTransformFeedbackVarying: name '%{public}s' %{public}u length %{public}d %{public}d",
        name, type, length, size);
    if (type == 0) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
        return NVal::CreateNull(env).val_;
    }
    webGLActiveInfo->SetActiveName(name);
    webGLActiveInfo->SetActiveSize(size);
    webGLActiveInfo->SetActiveType(type);
    return objActiveInfo;
}

napi_value WebGL2RenderingContextImpl::TexStorage2D(napi_env env, const TexStorageArg& arg)
{
    arg.Dump("WebGL2 texStorage2D");
    if (arg.target != GL_TEXTURE_2D && arg.target != GL_TEXTURE_CUBE_MAP) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }
    WebGLTexture* texture = GetBoundTexture(env, arg.target, false);
    if (texture == nullptr) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_OPERATION);
        return nullptr;
    }
    GLenum result = CheckTexStorage(env, arg);
    if (result != WebGLRenderingContextBase::NO_ERROR) {
        SET_ERROR(result);
        return nullptr;
    }
    glTexStorage2D(arg.target, arg.levels, arg.internalFormat, arg.width, arg.height);
    texture->SetTexStorageInfo(&arg);
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::TexStorage3D(napi_env env, const TexStorageArg& arg)
{
    arg.Dump("WebGL texStorage3D");
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
    LOGD("WebGL texStorage3D result %{public}u", GetError_());
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
    GLenum result = CheckTextureLevel(imgArg.target, imgArg.level);
    if (result) {
        return result;
    }
    result = CheckTextureFormatAndType(env, imgArg.internalFormat, imgArg.format, imgArg.type, imgArg.level);
    if (result) {
        return result;
    }
    return WebGLRenderingContextBase::NO_ERROR;
}

napi_value WebGL2RenderingContextImpl::TexImage3D(napi_env env, const TexImageArg& arg, napi_value source)
{
    TexImageArg imgArg(arg);
    imgArg.Dump("WebGL2 texImage3D");
    WebGLTexture* texture = GetBoundTexture(env, imgArg.target, true);
    if (texture == nullptr) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }

    GLenum result = CheckTexImage3D(env, imgArg);
    if (result) {
        SET_ERROR(result);
        return nullptr;
    }
    GLvoid* data = nullptr;
    WebGLImageSource imageSource(env, version_, unpackFlipY_, unpackPremultiplyAlpha_);
    if (!NVal(env, source).IsNull()) {
        result = imageSource.GenImageSource(
            { imgArg.format, imgArg.type, imgArg.width, imgArg.height, imgArg.depth }, source);
        if (result) {
            SET_ERROR(result);
            return nullptr;
        }
        data = imageSource.GetImageSourceData();
        imgArg.width = imageSource.GetWidth();
        imgArg.height = imageSource.GetHeight();
    }

    glTexImage3D(imgArg.target, imgArg.level, imgArg.internalFormat, imgArg.width, imgArg.height, imgArg.depth,
        imgArg.border, imgArg.format, imgArg.type, data);
    texture->SetTextureLevel({ imgArg.target, imgArg.level,
        imgArg.internalFormat, imgArg.width, imgArg.height, imgArg.depth, imgArg.type });
    LOGD("WebGL2 texImage3D result %{public}u", GetError_());
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

    GLvoid* data = nullptr;
    WebGLImageSource imageSource(env, version_, unpackFlipY_, unpackPremultiplyAlpha_);
    if (!NVal(env, dataObj).IsNull()) {
        result = imageSource.GenImageSource(
            { imgArg.format, imgArg.type, imgArg.width, imgArg.height, imgArg.depth }, dataObj, srcOffset);
        if (result) {
            SET_ERROR(result);
            return nullptr;
        }
        data = imageSource.GetImageSourceData();
    }
    if (data != nullptr) {
        GLenum result = CheckTextureDataBuffer(imgArg, imageSource.GetWebGLReadBuffer());
        if (result) {
            SET_ERROR(result);
            return nullptr;
        }
    }
    WebGLTexture* texture = GetBoundTexture(env, imgArg.target, true);
    if (texture == nullptr) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    glTexImage3D(imgArg.target, imgArg.level, imgArg.internalFormat, imgArg.width, imgArg.height, imgArg.depth,
        imgArg.border, imgArg.format, imgArg.type, data);
    texture->SetTextureLevel(imgArg);
    LOGD("WebGL2 texImage3D result %{public}u", GetError_());
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
    LOGD("WebGL2 texImage3D result %{public}u", GetError_());
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
    GLenum result = CheckTextureLevel(arg.target, arg.level);
    if (result) {
        return result;
    }
    WebGLTexture* texture = GetBoundTexture(env, arg.target, false);
    if (texture == nullptr) {
        return WebGLRenderingContextBase::INVALID_VALUE;
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
    result = CheckTextureFormatAndType(env, internalFormat, arg.format, arg.type, arg.level);
    if (result) {
        return result;
    }
    return WebGLRenderingContextBase::NO_ERROR;
}

napi_value WebGL2RenderingContextImpl::TexSubImage3D(napi_env env, const TexSubImage3DArg& arg, napi_value source)
{
    TexSubImage3DArg imgArg(arg);
    imgArg.Dump("WebGL2 texSubImage3D source");
    GLvoid* data = nullptr;
    WebGLImageSource imageSource(env, version_, unpackFlipY_, unpackPremultiplyAlpha_);
    if (!NVal(env, source).IsNull()) {
        GLenum result = imageSource.GenImageSource(
            { imgArg.format, imgArg.type, imgArg.width, imgArg.height, imgArg.depth }, source);
        if (result) {
            SET_ERROR(result);
            return nullptr;
        }
        data = imageSource.GetImageSourceData();
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
    LOGD("WebGL2 texSubImage3D result %{public}u", GetError_());
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::TexSubImage3D(
    napi_env env, const TexSubImage3DArg& imgArg, napi_value dataObj, GLuint srcOffset)
{
    imgArg.Dump("WebGL2 texSubImage3D data buffer");

    WebGLImageSource imageSource(env, version_, unpackFlipY_, unpackPremultiplyAlpha_);
    GLvoid* data = nullptr;
    bool changeUnpackAlignment = false;
    if (!NVal(env, dataObj).IsNull()) {
        GLenum result = imageSource.GenImageSource(
            { imgArg.format, imgArg.type, imgArg.width, imgArg.height, imgArg.depth }, dataObj, srcOffset);
        if (result) {
            SET_ERROR(result);
            return nullptr;
        }
        data = imageSource.GetImageSourceData();
        changeUnpackAlignment = unpackFlipY_ || unpackPremultiplyAlpha_;
    } else {
        SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }

    GLenum result = CheckTexSubImage3D(env, imgArg);
    if (result) {
        SET_ERROR(result);
        return nullptr;
    }

    if (changeUnpackAlignment) {
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    }
    glTexSubImage3D(imgArg.target, imgArg.level, imgArg.xOffset, imgArg.yOffset, imgArg.zOffset, imgArg.width,
        imgArg.height, imgArg.depth, imgArg.format, imgArg.type, data);
    if (changeUnpackAlignment) {
        glPixelStorei(GL_UNPACK_ALIGNMENT, unpackAlignment_);
    }
    LOGD("WebGL2 texSubImage3D result %{public}u", GetError_());
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::TexSubImage3D(napi_env env, const TexSubImage3DArg& imgArg, GLintptr pboOffset)
{
    imgArg.Dump("WebGL2 texSubImage3D pboOffset");
    GLenum result = CheckTexSubImage3D(env, imgArg);
    if (result) {
        SET_ERROR(result);
        return nullptr;
    }
    glTexSubImage3D(imgArg.target, imgArg.level, imgArg.xOffset, imgArg.yOffset, imgArg.zOffset, imgArg.width,
        imgArg.height, imgArg.depth, imgArg.format, imgArg.type, reinterpret_cast<void*>(pboOffset));
    LOGD("WebGL2 texSubImage3D result %{public}u", GetError_());
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
    result = CheckReadBufferAndGetInfo(env, &frameBufferId, nullptr, nullptr);
    if (result) {
        SET_ERROR(result);
        return nullptr;
    }
    glCopyTexSubImage3D(imgArg.target, imgArg.level, imgArg.xOffset, imgArg.yOffset, imgArg.zOffset, imgArg.x, imgArg.y,
        imgArg.width, imgArg.height);
    LOGD("WebGL2 copyTexSubImage3D result %{public}u", GetError_());
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::CompressedTexImage3D(
    napi_env env, const TexImageArg& imgArg, GLsizei imageSize, GLintptr offset)
{
    imgArg.Dump("WebGL2 compressedTexImage3D");
    WebGLTexture* texture = GetBoundTexture(env, imgArg.target, true);
    if (texture == nullptr) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_OPERATION);
        return nullptr;
    }
    if (texture->CheckImmutable()) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_OPERATION);
        return nullptr;
    }

    glCompressedTexImage3D(imgArg.target, imgArg.level, imgArg.internalFormat, imgArg.width, imgArg.height,
        imgArg.depth, imgArg.border, imageSize, reinterpret_cast<void*>(offset));
    LOGD("WebGL2 copyTexSubImage3D result %{public}u", GetError_());
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::CompressedTexImage3D(
    napi_env env, const TexImageArg& imgArg, napi_value dataObj, GLuint srcOffset, GLuint srcLengthOverride)
{
    imgArg.Dump("WebGL2 compressedTexImage3D");
    WebGLTexture* texture = GetBoundTexture(env, imgArg.target, true);
    if (texture == nullptr) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_OPERATION);
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
    LOGD("WebGL2 compressedTexImage3D result %{public}u", GetError_());
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::CompressedTexSubImage3D(
    napi_env env, const TexSubImage3DArg& imgArg, GLsizei imageSize, GLintptr offset)
{
    imgArg.Dump("WebGL2 compressedTexSubImage3D");
    WebGLTexture* texture = GetBoundTexture(env, imgArg.target, true);
    if (texture == nullptr) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_OPERATION);
        return nullptr;
    }
    if (imgArg.format != texture->GetInternalFormat(imgArg.target, imgArg.level)) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_OPERATION);
        return nullptr;
    }
    glCompressedTexSubImage3D(imgArg.target, imgArg.level, imgArg.xOffset, imgArg.yOffset, imgArg.zOffset, imgArg.width,
        imgArg.height, imgArg.depth, imgArg.format, imageSize, reinterpret_cast<void*>(offset));
    LOGD("WebGL2 compressedTexSubImage3D result %{public}u", GetError_());
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::CompressedTexSubImage3D(
    napi_env env, const TexSubImage3DArg& imgArg, napi_value dataObj, GLuint srcOffset, GLuint srcLengthOverride)
{
    imgArg.Dump("WebGL2 compressedTexSubImage3D");
    WebGLTexture* texture = GetBoundTexture(env, imgArg.target, true);
    if (texture == nullptr) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_OPERATION);
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
    LOGD("WebGL2 compressedTexSubImage3D result %{public}u", GetError_());
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::ClearBufferV(
    napi_env env, GLenum buffer, GLint drawBuffer, napi_value value, int64_t srcOffset, BufferDataType type)
{
    LOGD("WebGL2 clearBuffer buffer %{public}u %{public}d srcOffset %{public}u", buffer, drawBuffer, srcOffset);
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
    LOGD("WebGL2 clearBuffer buffer %{public}u result %{public}u", buffer, GetError_());
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::ClearBufferfi(
    napi_env env, GLenum buffer, GLint drawBuffer, GLfloat depth, GLint stencil)
{
    LOGD("WebGL2 clearBufferfi buffer %{public}u %{public}d depth %{public}f %{public}d",
        buffer, drawBuffer, depth, stencil);
    if (buffer != WebGLRenderingContextBase::DEPTH_STENCIL) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }
    glClearBufferfi(buffer, drawBuffer, depth, stencil);
    LOGD("WebGL2 clearBufferfi buffer %{public}u result %{public}u", buffer, GetError_());
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::GetIndexedParameter(napi_env env, GLenum target, GLuint index)
{
    LOGD("WebGL2 getIndexedParameter index = %{public}u", index);
    if (target == GL_TRANSFORM_FEEDBACK_BUFFER_BINDING) {
        if (index >= maxBoundTransformFeedbackBufferIndex_) {
            return NVal::CreateNull(env).val_;
        }
        return GetObject<WebGLBuffer>(env, boundIndexedTransformFeedbackBuffers_[index]);
    } else if (target == GL_UNIFORM_BUFFER_BINDING) {
        if (index >= maxBoundUniformBufferIndex_) {
            return NVal::CreateNull(env).val_;
        }
        return GetObject<WebGLBuffer>(env, boundIndexedUniformBuffers_[index]);
    } else if (target == GL_TRANSFORM_FEEDBACK_BUFFER_SIZE || target == GL_UNIFORM_BUFFER_SIZE) {
        int64_t data;
        glGetInteger64i_v(target, index, &data);
        LOGD("WebGL getIndexedParameter end");
        return NVal::CreateInt64(env, data).val_;
    } else if (target == GL_UNIFORM_BUFFER_SIZE || target == GL_UNIFORM_BUFFER_START) {
        int64_t data;
        glGetInteger64i_v(target, index, &data);
        LOGD("WebGL getIndexedParameter end");
        return NVal::CreateInt64(env, data).val_;
    } else {
        LOGD("WebGL getIndexedParameter end");
        return nullptr;
    }
}

napi_value WebGL2RenderingContextImpl::GetFragDataLocation(napi_env env, napi_value programObj, const std::string& name)
{
    GLuint program = WebGLProgram::DEFAULT_PROGRAM_ID;
    WebGLProgram* webGLProgram = WebGLProgram::GetObjectInstance(env, programObj);
    if (webGLProgram == nullptr) {
        return NVal::CreateInt64(env, -1).val_;
    }
    program = webGLProgram->GetProgramId();

    LOGD("WebGL2 getFragDataLocation name %{public}s", name.c_str());
    GLint res = glGetFragDataLocation(program, const_cast<char*>(name.c_str()));
    LOGD("WebGL2 getFragDataLocation result %{public}d", res);
    return NVal::CreateInt64(env, res).val_;
}

napi_value WebGL2RenderingContextImpl::VertexAttribI4i(napi_env env, GLuint index, GLint* data)
{
    VertexAttribInfo* info = GetVertexAttribInfo(index);
    if (info == nullptr) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    LOGD("WebGL2 vertexAttribI4i index %{public}u [%{public}d %{public}d %{public}d %{public}d]",
        index, data[0], data[1], data[2], data[3]); // 2, 3 index for data
    glVertexAttribI4i(index, data[0], data[1], data[2], data[3]); // 2, 3 index for data
    info->type = BUFFER_DATA_INT_32;
    LOGD("WebGL2 vertexAttribI4i result %{public}u", GetError_());
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::VertexAttribI4ui(napi_env env, GLuint index, GLuint* data)
{
    VertexAttribInfo* info = GetVertexAttribInfo(index);
    if (info == nullptr) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    LOGD("WebGL2 vertexAttribI4ui index %{public}u [%{public}u %{public}u %{public}u %{public}u]",
        index, data[0], data[1], data[2], data[3]); // 2, 3 index for data
    glVertexAttribI4ui(index, data[0], data[1], data[2], data[3]); // 2, 3 index for data
    info->type = BUFFER_DATA_UINT_32;
    LOGD("WebGL2 vertexAttribI4ui result %{public}u", GetError_());
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

napi_value WebGL2RenderingContextImpl::VertexAttribIPointer(napi_env env, const VertexAttribArg& vertexInfo)
{
    vertexInfo.Dump("WebGL2 vertexAttribPointer");
    if (!CheckGLenum(vertexInfo.type,
            { GL_BYTE, GL_UNSIGNED_BYTE, GL_SHORT, GL_UNSIGNED_SHORT, GL_INT, GL_UNSIGNED_INT }, {})) {
        LOGE("WebGL vertexAttribPointer invalid type %{public}d", vertexInfo.type);
        SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
    }

    GLenum result = CheckVertexAttribPointer(env, vertexInfo);
    if (result) {
        SET_ERROR(result);
        return nullptr;
    }

    glVertexAttribIPointer(vertexInfo.index, vertexInfo.size, vertexInfo.type, vertexInfo.stride,
        reinterpret_cast<GLvoid*>(vertexInfo.offset));
    LOGD("WebGL vertexAttribPointer index %{public}u offset %{public}u", vertexInfo.index, vertexInfo.offset);
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::VertexAttribDivisor(napi_env env, GLuint index, GLuint divisor)
{
    LOGD("WebGL2 vertexAttribDivisor index %{public}d divisor %{public}u", index, divisor);
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
    LOGD("WebGL drawArraysInstanced mode %{public}u %{public}u %{public}u %{public}u",
        mode, first, count, instanceCount);
    GLenum result = CheckDrawArrays(env, mode, first, count);
    if (result != WebGLRenderingContextBase::NO_ERROR) {
        SET_ERROR(result);
        return nullptr;
    }
    if (instanceCount < 0) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    glDrawArraysInstanced(mode, first, count, instanceCount);
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::DrawElementsInstanced(
    napi_env env, const DrawElementArg& arg, GLsizei instanceCount)
{
    LOGD("WebGL2 drawElementsInstanced mode %{public}u %{public}u %{public}u", arg.mode, arg.count, arg.type);
    GLenum result = CheckDrawElements(env, arg.mode, arg.count, arg.type, arg.offset);
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
    LOGD("WebGL2 drawRangeElements mode %{public}u %{public}u %{public}u start [%{public}u %{public}u] %{public}d",
        arg.mode, arg.count, arg.type, start, end, static_cast<int>(arg.offset));
    GLenum result = CheckDrawElements(env, arg.mode, arg.count, arg.type, arg.offset);
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
    LOGD("WebGL2 copyBufferSubData targets [%{public}u %{public}u] offset [%{public}u %{public}u] %{public}u",
        targets[0], targets[1], static_cast<uint32_t>(readOffset),
        static_cast<uint32_t>(writeOffset), static_cast<uint32_t>(size));
    if (!WebGLArg::CheckNoneNegInt<GLint>(readOffset) || !WebGLArg::CheckNoneNegInt<GLint>(writeOffset) ||
        !WebGLArg::CheckNoneNegInt<GLint>(size)) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    WebGLBuffer* readBuffer = GetBoundBuffer(env, targets[0]); // read
    if (readBuffer == nullptr || readBuffer->GetBufferSize() == 0) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_OPERATION);
        return nullptr;
    }
    WebGLBuffer* writeBuffer = GetBoundBuffer(env, targets[1]); // write
    if (writeBuffer == nullptr || writeBuffer->GetBufferSize() == 0) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_OPERATION);
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
    LOGD("WebGL2 copyBufferSubData result %{public}u", GetError_());
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::GetBufferSubData(
    napi_env env, GLenum target, int64_t offset, napi_value data, const BufferExt& ext)
{
    LOGD("WebGL2 getBufferSubData target %{public}u %{public}d ", target, static_cast<GLint>(offset));
    WebGLReadBufferArg bufferData(env);
    napi_status status = bufferData.GenBufferData(data);
    if (status != 0 || bufferData.GetBufferType() == BUFFER_ARRAY) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    if (!WebGLArg::CheckNoneNegInt<GLint>(offset)) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    WebGLBuffer* writeBuffer = GetBoundBuffer(env, target);
    if (writeBuffer == nullptr || writeBuffer->GetBufferSize() == 0) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_OPERATION, "no buffer");
        return nullptr;
    }
    if (offset + bufferData.GetBufferLength() > writeBuffer->GetBufferSize()) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_OPERATION);
        return nullptr;
    }
    GLsizeiptr dstSize = (ext.length == 0) ? static_cast<GLsizeiptr>(bufferData.GetBufferLength())
                                           : static_cast<GLsizeiptr>(ext.length * bufferData.GetBufferDataSize());
    GLuint dstOffset = static_cast<GLuint>(ext.offset * bufferData.GetBufferDataSize());
    LOGD("WebGL2 getBufferSubData dstSize %{public}u dstOffset %{public}p", dstSize, writeBuffer->bufferData_);

    void *mapBuffer = glMapBufferRange(target, static_cast<GLintptr>(offset), dstSize, GL_MAP_READ_BIT);
    if (mapBuffer == nullptr ||
        memcpy_s(bufferData.GetBuffer() + dstOffset, bufferData.GetBufferLength(), mapBuffer, dstSize) != 0) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_OPERATION);
    }
    glUnmapBuffer(target);
    LOGD("WebGL2 getBufferSubData target %{public}u result %{public}u ", target, GetError_());
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::BlitFrameBuffer(napi_env env, GLint data[8], GLbitfield mask, GLenum filter)
{
    // 0,1,2,3,4,5,6,7 srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1
    glBlitFramebuffer(data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7], mask, filter);
    LOGD("WebGL2 blitFrameBuffer filter %{public}u result %{public}u ", filter, GetError_());
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
    LOGD("WebGL frameBufferTextureLayer texture %{public}u", textureId);
    glFramebufferTextureLayer(target, attachment, textureId, layer.level, layer.layer);
    LOGD("WebGL frameBufferTextureLayer texture %{public}u result %{public}u", textureId, GetError_());

    WebGLFramebuffer* frameBuffer = GetBoundFrameBuffer(env, target);
    if (frameBuffer == nullptr) {
        SET_ERROR_WITH_LOG(WebGLRenderingContextBase::INVALID_ENUM, "can not find bind frame buffer");
        return nullptr;
    }
    frameBuffer->AddAttachment(
        target, attachment, static_cast<GLuint>(textureId), webGlTexture->GetTarget(), layer.level);
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::ReadBuffer(napi_env env, GLenum mode)
{
    LOGD("WebGL2 readBuffer mode %{public}u", mode);
    if (!CheckReadBufferMode(mode)) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }

    WebGLFramebuffer* readFrameBuffer = GetBoundFrameBuffer(env, WebGL2RenderingContextBase::READ_FRAMEBUFFER);
    if (readFrameBuffer == nullptr) {
        if (mode != WebGLRenderingContextBase::BACK && mode != WebGLRenderingContextBase::NONE) {
            SET_ERROR(WebGLRenderingContextBase::INVALID_OPERATION);
            return nullptr;
        }
        defaultReadBufferMode_ = mode;
        if (mode == WebGLRenderingContextBase::BACK)
            mode = WebGLRenderingContextBase::COLOR_ATTACHMENT0;
    } else {
        if (mode == WebGLRenderingContextBase::BACK) {
            SET_ERROR(WebGLRenderingContextBase::INVALID_OPERATION);
            return nullptr;
        }
        readFrameBuffer->SetReadBufferMode(mode);
    }
    glReadBuffer(mode);
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::RenderBufferStorageMultiSample(
    napi_env env, const TexStorageArg& arg, GLsizei samples)
{
    arg.Dump("WebGL2 renderbufferStorageMultisample");
    LOGD("WebGL2 renderbufferStorageMultisample samples %{public}d", samples);

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
        if (CheckInList(arg.internalFormat, WebGLTexture::GetSupportInternalFormatGroup1())) {
            if (samples > 0) {
                SET_ERROR(WebGLRenderingContextBase::INVALID_OPERATION);
                return nullptr;
            }
        } else if (!CheckInList(arg.internalFormat, WebGLTexture::GetSupportInternalFormatGroup2())) {
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
    LOGD("WebGL2 renderbufferStorageMultisample result %{public}u", GetError_());
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::BindBuffer(napi_env env, GLenum target, napi_value object)
{
    LOGD("WebGL2 bindBuffer target %{public}u ", target);
    // support default value
    uint32_t bufferId = WebGLBuffer::DEFAULT_BUFFER;
    WebGLBuffer* webGlBuffer = GetValidBuffer(env, object);
    if (webGlBuffer != nullptr) {
        bufferId = webGlBuffer->GetBufferId();
    }
    LOGD("WebGL2 bindBuffer target %{public}u bufferId %{public}u", target, bufferId);
    uint32_t index = BoundBufferType::ARRAY_BUFFER;
    if (!CheckBufferTarget(env, target, index)) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }
    if (webGlBuffer != nullptr && !CheckBufferTargetCompatibility(env, target, webGlBuffer)) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_OPERATION);
        return nullptr;
    }
    if (boundBufferIds_[index] && boundBufferIds_[index] != bufferId) {
        LOGD("WebGL2 bindBuffer has been bound bufferId %{public}u", boundBufferIds_[index]);
    }
    boundBufferIds_[index] = bufferId;

    glBindBuffer(target, static_cast<GLuint>(bufferId));
    if (webGlBuffer) {
        webGlBuffer->SetTarget(target);
    }
    LOGD("WebGL2 bindBuffer bufferId %{public}u result %{public}u", bufferId, GetError_());
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::BindBufferBase(napi_env env, const BufferBaseArg& arg, napi_value bufferObj)
{
    GLuint bufferId = WebGLBuffer::DEFAULT_BUFFER;
    WebGLBuffer* webGlBuffer = GetValidBuffer(env, bufferObj);
    if (webGlBuffer != nullptr) {
        bufferId = webGlBuffer->GetBufferId();
    }
    if (!CheckBufferBindTarget(arg.target)) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }
    if (webGlBuffer != nullptr && !CheckBufferTargetCompatibility(env, arg.target, webGlBuffer)) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_OPERATION);
        return nullptr;
    }
    if (!UpdateBaseTargetBoundBuffer(env, arg.target, arg.index, bufferId)) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_OPERATION);
        return nullptr;
    }
    if (webGlBuffer != nullptr && !webGlBuffer->GetTarget()) {
        webGlBuffer->SetTarget(arg.target);
    }
    LOGD("WebGL2 bindBufferBase target %{public}u %{public}u %{public}u", arg.target, arg.index, bufferId);
    glBindBufferBase(arg.target, arg.index, bufferId);
    LOGD("WebGL2 bindBufferBase target %{public}u result %{public}u ", arg.target, GetError_());
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::BindBufferRange(
    napi_env env, const BufferBaseArg& arg, napi_value bufferObj, GLintptr offset, GLsizeiptr size)
{
    GLuint bufferId = WebGLBuffer::DEFAULT_BUFFER;
    WebGLBuffer* webGlBuffer = GetValidBuffer(env, bufferObj);
    if (webGlBuffer != nullptr) {
        bufferId = webGlBuffer->GetBufferId();
    }
    if (!CheckBufferBindTarget(arg.target)) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }
    if (webGlBuffer != nullptr && !CheckBufferTargetCompatibility(env, arg.target, webGlBuffer)) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_OPERATION);
        return nullptr;
    }
    if (!UpdateBaseTargetBoundBuffer(env, arg.target, arg.index, bufferId)) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_OPERATION);
        return nullptr;
    }
    if (webGlBuffer != nullptr && !webGlBuffer->GetTarget()) {
        webGlBuffer->SetTarget(arg.target);
    }
    LOGD("WebGL2 bindBufferRange target %{public}u %{public}u %{public}u", arg.target, arg.index, bufferId);
    glBindBufferRange(arg.target, arg.index, bufferId, offset, size);
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::DeleteBuffer(napi_env env, napi_value object)
{
    return WebGLRenderingContextBaseImpl::DeleteBuffer(env, object);
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
    if (!WebGLArg::CheckString(uniformBlockName)) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
        return NVal::CreateInt64(env, -1).val_;
    }
    LOGD("WebGL2 getUniformBlockIndex programId %{public}u", programId);
    GLuint returnValue = glGetUniformBlockIndex(programId, uniformBlockName.c_str());
    LOGD("WebGL2 getUniformBlockIndex name %{public}s %{public}u", uniformBlockName.c_str(), returnValue);
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
    LOGD("WebGL2 uniformBlockBinding programId %{public}u %{public}u %{public}u",
        programId, uniformBlockIndex, uniformBlockBinding);
    glUniformBlockBinding(programId, uniformBlockIndex, uniformBlockBinding);
    LOGD("WebGL2 uniformBlockBinding programId %{public}u result %{public}u", programId, GetError_());
    return nullptr;
}

napi_value WebGL2RenderingContextImpl::InvalidateFrameBuffer(napi_env env, GLenum target, napi_value data)
{
    LOGD("WebGL2 invalidate Framebuffer target %{public}u", target);
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
    LOGD("WebGL2 getInternalformatParameter target %{public}u %{public}u %{public}u", target, internalFormat, pname);
    WebGLWriteBufferArg writeBuffer(env);
    if (CheckInList(internalFormat, WebGLTexture::GetSupportInternalFormatGroup1())) {
        return writeBuffer.ToExternalArray(BUFFER_DATA_INT_32);
    } else if (!CheckInList(internalFormat, WebGLTexture::GetSupportInternalFormatGroup2())) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_ENUM);
        return NVal::CreateNull(env).val_;
    }

    GLint length = -1;
    if (pname == GL_SAMPLES) {
        glGetInternalformativ(target, internalFormat, GL_NUM_SAMPLE_COUNTS, 1, &length);
        LOGD("WebGL2 getInternalformatParameter length %{public}u", length);
    } else {
        SET_ERROR(WebGLRenderingContextBase::INVALID_ENUM);
        return NVal::CreateNull(env).val_;
    }
    if (length > 0) {
        GLint* params = reinterpret_cast<GLint*>(writeBuffer.AllocBuffer(length * sizeof(GLint)));
        if (params == nullptr) {
            return NVal::CreateNull(env).val_;
        }
        glGetInternalformativ(target, internalFormat, pname, length, params);
        writeBuffer.DumpBuffer(BUFFER_DATA_INT_32);
    }
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
    if (bufferMode != GL_INTERLEAVED_ATTRIBS && bufferMode != GL_SEPARATE_ATTRIBS) {
        SET_ERROR(WebGLRenderingContextBase::INVALID_ENUM);
        return nullptr;
    }
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
    LOGD("WebGL2 getUniformIndices uniformCount %{public}u", list.size());

    WebGLWriteBufferArg writeBuffer(env);
    napi_value result = NVal::CreateNull(env).val_;
    GLuint* uniformIndices = reinterpret_cast<GLuint*>(writeBuffer.AllocBuffer(list.size() * sizeof(GLuint)));
    if (uniformIndices != nullptr) {
        glGetUniformIndices(programId, static_cast<GLsizei>(list.size()), const_cast<const GLchar**>(list.data()),
            static_cast<GLuint*>(uniformIndices));
        writeBuffer.DumpBuffer(BUFFER_DATA_UINT_32);
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
    LOGD("WebGL2 getActiveUniforms programId %{public}u pname %{public}u", programId, pname);
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
    LOGD("WebGL2 getActiveUniformBlockParameter programId %{public}u %{public}u %{public}u",
        programId, uniformBlockIndex, pname);
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
            LOGD("WebGL2 getActiveUniformBlockParameter uniformCount %{public}d", uniformCount);
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
        SET_ERROR(WebGLRenderingContextBase::INVALID_VALUE);
        return nullptr;
    }
    glGetActiveUniformBlockName(programId, uniformBlockIndex, length, &size, buf.get());
    LOGD("WebGL2 getActiveUniformBlockName programId %{public}u uniformBlockIndex  %{public}u name %{public}s",
        programId, uniformBlockIndex, buf.get());
    string str(buf.get(), size);
    return NVal::CreateUTF8String(env, str).val_;
}

GLenum WebGL2RenderingContextImpl::CheckClearBuffer(napi_env env, GLenum buffer, const WebGLReadBufferArg& bufferData)
{
    size_t size = bufferData.GetBufferLength() / bufferData.GetBufferDataSize();
    switch (buffer) {
        case WebGL2RenderingContextBase::COLOR:
        case WebGLRenderingContextBase::FRONT:
        case WebGLRenderingContextBase::BACK:
        case WebGLRenderingContextBase::FRONT_AND_BACK:
            if (size < 4) { // 4 max element
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
        LOGE("CheckTexStorage %{public}u", arg.target);
        return WebGLRenderingContextBase::INVALID_OPERATION;
    }

    if (!CheckStorageInternalFormat(env, arg.internalFormat)) {
        return WebGLRenderingContextBase::INVALID_ENUM;
    }

    if (arg.width <= 0 || arg.height <= 0 || arg.depth <= 0) {
        LOGE("CheckTexStorage invalid width %{public}d, height %{public}d, depth %{public}d",
            arg.width, arg.height, arg.depth);
        return WebGLRenderingContextBase::INVALID_VALUE;
    }

    if (arg.levels <= 0) {
        LOGE("CheckTexStorage invalid levels %{public}d", arg.levels);
        return WebGLRenderingContextBase::INVALID_VALUE;
    }

    if (arg.target == GL_TEXTURE_3D) {
        if (arg.levels > log2(std::max(std::max(arg.width, arg.height), arg.depth)) + 1) {
            LOGE("CheckTexStorage invalid levels %{public}u", arg.levels);
            return WebGLRenderingContextBase::INVALID_OPERATION;
        }
    } else {
        if (arg.levels > log2(std::max(arg.width, arg.height)) + 1) {
            LOGE("CheckTexStorage invalid %{public}u %{public}u",
                arg.levels, log2(std::max(arg.width, arg.height)) + 1);
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

bool WebGL2RenderingContextImpl::CheckTransformFeedbackBuffer(GLenum target, WebGLBuffer* buffer)
{
    if (target == GL_TRANSFORM_FEEDBACK_BUFFER) {
        for (size_t i = 0; i < sizeof(boundBufferIds_) / boundBufferIds_[0]; i++) {
            if (i == BoundBufferType::TRANSFORM_FEEDBACK_BUFFER) {
                continue;
            }
            if (boundBufferIds_[i] == buffer->GetBufferId()) {
                LOGD("boundBufferIds_ %{public}u %{public}zu", boundBufferIds_[i], i);
                return false;
            }
        }
        LOGD("boundIndexedUniformBuffers_ %{public}u", boundIndexedUniformBuffers_.size());
        for (size_t i = 0; i < boundIndexedUniformBuffers_.size(); ++i) {
            if (boundIndexedUniformBuffers_[i] == buffer->GetBufferId()) {
                return false;
            }
        }
    } else {
        LOGD("Get TRANSFORM_FEEDBACK_BUFFER id %{public}u ",
            boundBufferIds_[BoundBufferType::TRANSFORM_FEEDBACK_BUFFER]);
        if (boundBufferIds_[BoundBufferType::TRANSFORM_FEEDBACK_BUFFER] == buffer->GetBufferId()) {
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

bool WebGL2RenderingContextImpl::CheckBufferTargetCompatibility(napi_env env, GLenum target, WebGLBuffer* buffer)
{
    LOGD("buffer target %{public}u %{public}u id %{public}u", buffer->GetTarget(), target, buffer->GetBufferId());
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
    return CheckTransformFeedbackBuffer(target, buffer);
}

bool WebGL2RenderingContextImpl::UpdateBaseTargetBoundBuffer(
    napi_env env, GLenum target, GLuint index, GLuint bufferId)
{
    if (target == GL_TRANSFORM_FEEDBACK_BUFFER) {
        if (index >= maxBoundTransformFeedbackBufferIndex_) {
            LOGE("Out of bound indexed transform feedback buffer %{public}u", index);
            return false;
        }
        boundIndexedTransformFeedbackBuffers_[index] = bufferId;
        boundBufferIds_[BoundBufferType::TRANSFORM_FEEDBACK_BUFFER] = bufferId;
        if (!bufferId) { // for delete
            boundIndexedTransformFeedbackBuffers_.erase(index);
        }
        return true;
    }
    if (target == GL_UNIFORM_BUFFER) {
        if (index >= maxBoundUniformBufferIndex_) {
            LOGE("Out of bound indexed uniform buffer %{public}u", index);
            return false;
        }
        boundIndexedUniformBuffers_[index] = bufferId;
        boundBufferIds_[BoundBufferType::UNIFORM_BUFFER] = bufferId;
        if (!bufferId) { // for delete
            boundIndexedUniformBuffers_.erase(index);
        }
        return true;
    }
    return false;
}

bool WebGL2RenderingContextImpl::CheckStorageInternalFormat(napi_env env, GLenum internalFormat)
{
    return CheckInList(internalFormat, {
        GL_R8,
        GL_R8_SNORM,
        GL_R16F,
        GL_R32F,
        GL_R8UI,
        GL_R8I,
        GL_R16UI,
        GL_R16I,
        GL_R32UI,
        GL_R32I,
        GL_RG8,
        GL_RG8_SNORM,
        GL_RG16F,
        GL_RG32F,
        GL_RG8UI,
        GL_RG8I,
        GL_RG16UI,
        GL_RG16I,
        GL_RG32UI,
        GL_RG32I,
        GL_RGB8,
        GL_SRGB8,
        GL_RGB565,
        GL_RGB8_SNORM,
        GL_R11F_G11F_B10F,
        GL_RGB9_E5,
        GL_RGB16F,
        GL_RGB32F,
        GL_RGB8UI,
        GL_RGB8I,
        GL_RGB16UI,
        GL_RGB16I,
        GL_RGB32UI,
        GL_RGB32I,
        GL_RGBA8,
        GL_SRGB8_ALPHA8,
        GL_RGBA8_SNORM,
        GL_RGB5_A1,
        GL_RGBA4,
        GL_RGB10_A2,
        GL_RGBA16F,
        GL_RGBA32F,
        GL_RGBA8UI,
        GL_RGBA8I,
        GL_RGB10_A2UI,
        GL_RGBA16UI,
        GL_RGBA16I,
        GL_RGBA32UI,
        GL_RGBA32I,
        GL_DEPTH_COMPONENT16,
        GL_DEPTH_COMPONENT24,
        GL_DEPTH_COMPONENT32F,
        GL_DEPTH24_STENCIL8,
        GL_DEPTH32F_STENCIL8,
        GL_COMPRESSED_R11_EAC,
        GL_COMPRESSED_SIGNED_R11_EAC,
        GL_COMPRESSED_RG11_EAC,
        GL_COMPRESSED_SIGNED_RG11_EAC,
        GL_COMPRESSED_RGB8_ETC2,
        GL_COMPRESSED_SRGB8_ETC2,
        GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2,
        GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2,
        GL_COMPRESSED_RGBA8_ETC2_EAC,
        GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC,
    });
}

} // namespace Impl
} // namespace Rosen
} // namespace OHOS
