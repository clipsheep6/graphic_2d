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
#include "egl_blob_cache.h"
#include "../wrapper_log.h"
namespace OHOS{
BlobCache* BlobCache::blob_cache_;
BlobCache* BlobCache::get() {
    return BlobCache::blob_cache_;
}

void BlobCache::setBlobFunc(const void* key, EGLsizeiANDROID keySize, const void* value,
                    EGLsizeiANDROID valueSize) {
    BlobCache::get()->setBlob(key, keySize, value, valueSize);
}

EGLsizeiANDROID BlobCache::getBlobFunc(const void* key, EGLsizeiANDROID keySize, void* value,
                               EGLsizeiANDROID valueSize) {
    return BlobCache::get()->getBlob(key, keySize, value, valueSize);
}

void BlobCache::setBlob(const void* key, EGLsizeiANDROID keySize, const void* value,
                    EGLsizeiANDROID valueSize) {
    WLOGE("wxt set");

}

EGLsizeiANDROID BlobCache::getBlob(const void* key, EGLsizeiANDROID keySize, void* value,
                               EGLsizeiANDROID valueSize) {
    WLOGE("wxt get");
                                return 1;
}

void BlobCache::initialize(EGLDisplay dpy) {
    EglWrapperDispatchTablePtr table = &gWrapperHook;
    table->egl.eglSetBlobCacheFuncsANDROID(display->GetEglDisplay(), BlobCache::setBlobFunc,  BlobCache::getBlobFunc);
}

}