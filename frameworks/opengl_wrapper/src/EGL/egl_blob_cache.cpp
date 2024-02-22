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

namespace OHOS
{

BlobCache* BlobCache::blobCache_;

BlobCache* BlobCache::get()
{
    if (BlobCache::blobCache_ == nullptr) {
        BlobCache::blobCache_ = new BlobCache();
    }
    return BlobCache::blobCache_;
}

BlobCache::Blob::Blob(const void *data, size_t size)
                      : prev_(nullptr), next_(nullptr), dataSize_(size)
{
    data_ = malloc(size), 
    if (data != nullptr) {
        memcpy_s(data_, size, const_cast<void *>(data), size);
    }
}

BlobCache::Blob::~Blob()
{
    free(data_);
}

BlobCache::BlobCache()
{
    head_ = std::make_shared<Blob>(nullptr, 0);
    tail_ = std::make_shared<Blob>(nullptr, 0);
    head_->next_ = tail_;
    tail_->prev_ = head_;
    blobSizeMax_ = MAX_SHADER;
}

BlobCache::~BlobCache()
{
}

void BlobCache::init()
{
}

void BlobCache::setBlobFunc(const void* key, EGLsizeiANDROID keySize, const void* value,
                            EGLsizeiANDROID valueSize)
{
    BlobCache::get()->setBlob(key, keySize, value, valueSize);
}
EGLsizeiANDROID BlobCache::getBlobFunc(const void *key, EGLsizeiANDROID keySize, void *value,
                                       EGLsizeiANDROID valueSize)
{
    return BlobCache::get()->getBlob(key, keySize, value, valueSize);
}

void BlobCache::setBlob(const void *key, EGLsizeiANDROID keySize, const void *value,
                        EGLsizeiANDROID valueSize)
{
    std::shared_ptr<Blob> keyBlob = std::make_shared<Blob>(key, (size_t)keySize);
    auto it = mblob_map_.find(keyBlob);

    if (it != mblob_map_.end())
    {
        free(it->second->data_);
        if (valueSize < 0) {
            WLOGE("valueSize error");
            return;
        }
        it->second->data_ = malloc(valueSize);
        memcpy_s(it->second->data_, valueSize, value, valueSize);
        return;
    }

    if (blobSize_ >= blobSizeMax_)
    {
        std::shared_ptr<Blob> deleteblob = tail_->prev_;
        deleteblob->prev_->next_ = tail_;
        tail_->prev_ = deleteblob->prev_;
        mblob_map_.erase(deleteblob);
        --blobSize_;
    }

    std::shared_ptr<Blob> valueBlob = std::make_shared<Blob>(value, (size_t)valueSize);
    mblob_map_.emplace(keyBlob, valueBlob);
    head_->next_->prev_ = keyBlob;
    keyBlob->next_ = head_->next_;
    head_->next_ = keyBlob;
    keyBlob->prev_ = head_;
    ++blobSize_;
}

EGLsizeiANDROID BlobCache::getBlob(const void *key, EGLsizeiANDROID keySize, void *value,
                                   EGLsizeiANDROID valueSize)
{
    EGLsizeiANDROID ret = 0;
    std::shared_ptr<Blob> keyBlob = std::make_shared<Blob>(key, (size_t)keySize);
    auto it = mblob_map_.find(keyBlob);
    if (it != mblob_map_.end()) {
        ret = (EGLsizeiANDROID)it->second->dataSize_;
        if (valueSize < ret) {
            WLOGE("valueSize not enough");
        } else {
            memcpy_s(value, valueSize, it->second->data_, it->second->dataSize_);
            auto moveblob = it->first;
            moveblob->prev_->next_ = moveblob->next_;
            moveblob->next_->prev_ = moveblob->prev_;
            head_->next_->prev_ = moveblob;
            moveblob->next_ = head_->next_;
            head_->next_ = moveblob;
            moveblob->prev_ = head_;
        }
    }

    return ret;
}

void BlobCache::SetCacheDir(const std::string dir)
{
    cacheDir_ = dir;
}

void EglSetCacheDir(const std::string dir)
{
    BlobCache::get()->SetCacheDir(dir);
}
}