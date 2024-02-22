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
void setEglFilePath(const char* filePath) {
    BlobCache::get()->setEglFilePath(filePath);
}

BlobCache* BlobCache::blob_cache_;
BlobCache* BlobCache::get() {
    if (BlobCache::blob_cache_ == nullptr) {
        BlobCache::blob_cache_ = new BlobCache();
    }
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

BlobCache::Blob::Blob(const void* data, size_t size)
      : data_(malloc(size)), data_size_(size) {
    if (data != nullptr) {
        memcpy(data_, data, size);
    }
}

BlobCache::Blob::~Blob() {
    free(data_);
}

bool BlobCache::Blob::operator<(const Blob& rhs) const {
    if (data_size_ == rhs.data_size_) {
        return memcmp(data_, rhs.data_, data_size_) < 0;
    } else {
        return data_size_ < rhs.data_size_;
    }
}

void BlobCache::setBlob(const void* key, EGLsizeiANDROID keySize, const void* value,
                    EGLsizeiANDROID valueSize) {
    WLOGE("wxt set");
    mblob_.emplace_back(std::pair<Blob, Blob>(Blob(key, (size_t)keySize), Blob(value, (size_t)valueSize)));
}

EGLsizeiANDROID BlobCache::getBlob(const void* key, EGLsizeiANDROID keySize, void* value,
                               EGLsizeiANDROID valueSize) {
    WLOGE("wxt get");
    auto index = std::find_if(mblob_.begin(), mblob_.end(), [&key, keySize](std::pair<Blob, Blob> &blobPair))-> bool {
        if (keySize == blobPair.first.data_size_) {
            return memcmp(key, blobPair.first.data_, keySize) < 0;
        } else {
            return false;
        }
    }
    EGLsizeiANDROID ret = -1;
    if (index != mblob_.end()) {
        value = malloc(index->second.data_size_);
        memcpy(value, index->second.data_, index->second.data_size_);
        ret = index->second.data_size_;
    }
    return ret;
}

BlobCache::BlobCache() {
}

BlobCache::~BlobCache() {
}

void BlobCache::initialize() {

}

void BlobCache::setEglFilePath(const char* filePath) {
    file_path_ = filePath;
}

}