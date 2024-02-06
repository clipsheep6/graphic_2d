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
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <pair>
#include <memory>

namespace OHOS {
class BlobCache {
    public:
    class Blob{
        void *data_;
        size_t data_size_;
        bool operator<(const Blob& rhs) const;
    }
    static BlobCache* get();
    static void setBlobFunc(const void* key, EGLsizeiANDROID keySize, const void* value,
                    EGLsizeiANDROID valueSize);
    static EGLsizeiANDROID getBlobFunc(const void* key, EGLsizeiANDROID keySize, void* value,
                               EGLsizeiANDROID valueSize);

    void setBlob(const void* key, EGLsizeiANDROID keySize, const void* value,
                    EGLsizeiANDROID valueSize);
    EGLsizeiANDROID getBlob(const void* key, EGLsizeiANDROID keySize, void* value,
                               EGLsizeiANDROID valueSize);
    void initialize();

    void setEglFilePath(const char* filePath);
    private:
    static BlobCache *blob_cache_;
    std::vector<std::pair<Blob>> mblob_;
    char* file_path_;
};
}