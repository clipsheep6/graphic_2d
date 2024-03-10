/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.. All rights reserved.
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
#include <sys/stat.h>
#include <sys/mman.h>
#include <errors.h>
#include <inttypes.h>
#include <unistd.h>
#include <fcntl.h>
#include <thread>

namespace OHOS {
static const char* CACHE_MAGIC = "OSOH";
BlobCache* BlobCache::blobCache_;

BlobCache* BlobCache::Get()
{
    if (BlobCache::blobCache_ == nullptr) {
        BlobCache::blobCache_ = new BlobCache();
    }
    return BlobCache::blobCache_;
}

BlobCache::Blob::Blob(const void *dataIn, size_t size) :dataSize(size)
{
    prev_ = nullptr;
    next_ = nullptr;
    data = nullptr;
    if (size > 0) {
        data = malloc(size);
    }
    if (data != nullptr) {
        memcpy_s(data, size, const_cast<void *>(dataIn), size);
    }
}

BlobCache::Blob::~Blob()
{
    free(data);
}

BlobCache::BlobCache()
{
    head_ = std::make_shared<Blob>(nullptr, 0);
    tail_ = std::make_shared<Blob>(nullptr, 0);
    head_->next_ = tail_;
    tail_->prev_ = head_;
    blobSizeMax_ = MAX_SHADER;
    maxShaderSize_ = MAX_SHADER_SIZE;
    initStatus_ = false;
    fileName_ = std::string("/blobShader");
    saveStatus_ = false;
    blobSize_ = 0;
}

BlobCache::~BlobCache()
{
    if (blobCache_) {
        blobCache_ = nullptr;
    }
}

void BlobCache::terminate()
{
    if (blobCache_) {
        blobCache_->WriteToDisk();
    }
    blobCache_ = nullptr;
}

void BlobCache::Init()
{
    if (!initStatus_) return;
    EglWrapperDispatchTablePtr *table = &gWrapperHook;
    if (table->isLoad && table->egl.eglSetBlobCacheFuncsANDROID) {
        table->egl.eglSetBlobCacheFuncsANDROID(disp_, BlobCache::setBlobFunc, BlobCache::getBlobFunc);
    } else {
        WLOGE("eglSetBlobCacheFuncsANDROID not found.");
    }
}

void BlobCache::setBlobFunc(const void* key, EGLsizeiANDROID keySize, const void* value,
                            EGLsizeiANDROID valueSize)
{
    BlobCache::Get()->setBlob(key, keySize, value, valueSize);
}
EGLsizeiANDROID BlobCache::getBlobFunc(const void *key, EGLsizeiANDROID keySize, void *value,
                                       EGLsizeiANDROID valueSize)
{
    return BlobCache::Get()->getBlob(key, keySize, value, valueSize);
}

void BlobCache::setBlob(const void *key, EGLsizeiANDROID keySize, const void *value,
                        EGLsizeiANDROID valueSize)
{
    std::lock_guard<std::mutex> lock(blobmutex_);
    if (keySize <= 0) {
        return;
    }
    std::shared_ptr<Blob> keyBlob = std::make_shared<Blob>(key, (size_t)keySize);
    auto it = mBlobMap_.find(keyBlob);
    if (it != mBlobMap_.end()) {
        free(it->second->data);
        if (valueSize < 0) {
            WLOGE("valueSize error");
            return;
        }
        it->second->data = malloc(valueSize);
        memcpy_s(it->second->data, valueSize, value, valueSize);
        head_->next_->prev_ = it->first;
        it->first->next_ = head_->next_;
        head_->next_ = it->first;
        it->first->prev_ = head_;
        return;
    }

    if (blobSize_ >= blobSizeMax_) {
        int count = 0;
        while (count <= MAX_SHADER_DELETE) {
            std::shared_ptr<Blob> deleteblob = tail_->prev_;
            deleteblob->prev_->next_ = tail_;
            tail_->prev_ = deleteblob->prev_;
            mBlobMap_.erase(deleteblob);
            --blobSize_;
            ++count;
        }
    }

    std::shared_ptr<Blob> valueBlob = std::make_shared<Blob>(value, (size_t)valueSize);
    mBlobMap_.emplace(keyBlob, valueBlob);
    head_->next_->prev_ = keyBlob;
    keyBlob->next_ = head_->next_;
    head_->next_ = keyBlob;
    keyBlob->prev_ = head_;
    ++blobSize_;
    if (!saveStatus_) {
        saveStatus_ = true;
        std::thread deferSaveThread([this]() {
            sleep(DEFER_SAVE_MIN);
            if(blobCache_) {
                blobCache_->WriteToDisk();
            }
            saveStatus_ = false;
        });
        deferSaveThread.detach();
    }
}

EGLsizeiANDROID BlobCache::getBlob(const void *key, EGLsizeiANDROID keySize, void *value,
                                   EGLsizeiANDROID valueSize)
{
    std::lock_guard<std::mutex> lock(blobmutex_);
    EGLsizeiANDROID ret = 0;
    if (keySize <= 0) {
        return ret;
    }
    std::shared_ptr<Blob> keyBlob = std::make_shared<Blob>(key, (size_t)keySize);
    auto it = mBlobMap_.find(keyBlob);
    if (it != mBlobMap_.end()) {
        ret = (EGLsizeiANDROID)it->second->dataSize;
        if (valueSize < ret) {
            WLOGE("valueSize not enough");
        } else {
            memcpy_s(value, valueSize, it->second->data, it->second->dataSize);
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
    struct stat dirstat;
    if (stat(dir.c_str(), &dirstat) != 0) {
        WLOGE("inputdir not Create");
        initStatus_ = false;
        return;
    }

    struct stat cachedirstat;
    struct stat ddkdirstat;
    cacheDir_ = dir + std::string("/blobShader");
    ddkCacheDir_ = dir + std::string("/ddkShader");

    if (stat(cacheDir_.c_str(), &dirstat) != 0) {
        initStatus_ = false;
        int ret = mkdir(cacheDir_.c_str(), S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH);
        if (ret == -1) {
            initStatus_ = false;
            WLOGE("cacheDir_ not Create");
        } else {
            initStatus_ = true;
        }
    } else {
        initStatus_ = true;
    }

    if (stat(ddkCacheDir_.c_str(), &ddkdirstat) != 0) {
        int ret = mkdir(ddkCacheDir_.c_str(), S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH);
        if (ret == -1 && errno != EEXIST) {
            WLOGE("ddkCacheDir_ not Create");
        }
    }
}

void BlobCache::SetCacheShaderSize(int shadermax)
{
    if (shadermax <= 0 || shadermax > MAX_SHADER) {
        return;
    }
    blobSizeMax_ = shadermax;
}

static inline size_t formatfile(size_t size)
{
    return (size + 3) & ~3;
}

size_t BlobCache::getCacheSize()
{
    size_t headSize = sizeof(CacheHeader);
    size_t ret = 0;
    for (auto item = mBlobMap_.begin(); item != mBlobMap_.end(); ++item) {
        size_t innerSize = headSize + item->first->dataSize + item->second->dataSize;
        ret += formatfile(innerSize);
    }
    return ret;
}

void BlobCache::WriteToDisk()
{
    std::lock_guard<std::mutex> lock(blobmutex_);

    struct stat dirstat;
    if (stat(cacheDir_.c_str(), &dirstat) != 0) {
        WLOGE("cacheDir_ not Create");
        return;
    }

    std::string storefile = cacheDir_ + fileName_;
    int fd = open(storefile.c_str(), O_CREATE | O_EXCL | O_RDWR, 0);
    if (fd == -1) {
        if (errno == EEXIST) {
            if (unlink(storefile.c_str()) == -1) {
                WLOGE("unlink file failed");
                return;
            }
            fd = open(storefile.c_str(), O_CREATE | O_EXCL | O_RDWR, 0);
        }
        if (fd == -1) {
            WLOGE("recreate file failed");
            return;
        }
    }

    size_t filesize = getCacheSize();
    size_t headsize = sizeof(CacheHeader);
    size_t bufsize = filesize + headsize;
    uint8_t *buf = new uint8_t[bufsize];
    size_t offset = headsize;
    for (auto item = mBlobMap_.begin(); item != mBlobMap_.end(); ++item) {
        CacheHeader* eheader = reinterpret_cast<CacheHeader*>(&buf[offset]);
        eheader->keySize_ = item->first->dataSize;
        eheader->valueSize_ = item->second->dataSize;
        memcpy(eheader->mData_, item->first->data ,item->first->dataSize);
        memcpy(eheader->mData_ + item->first->dataSize, item->second->data ,item->second->dataSize);
        size_t innerSize = headsize + item->first->dataSize + item->second->dataSize;
        offset += formatfile(innerSize);
    }


    memcpy(buf, CACHE_MAGIC, 4);
    uint32_t *crc = reinterpret_cast<uint32_t*>(buf + 4);
    *crc = crcGen(buf + 8, filesize);

    if (write(fd, buf, bufsize) == -1) {
        WLOGE("write file failed");
        delete[] buf;
        close(fd);
        return;
    }

    delete[] buf;
    fchmod(fd, S_IRUSR);
    close(fd);
}

void BlobCache::ReadFromDisk()
{
    struct stat dirstat;
    if (stat(cacheDir_.c_str(), &dirstat) != 0) {
        WLOGE("cacheDir_ not Create");
        return;
    }
    std::string storefile = cacheDir_ + fileName_;
    int fd = open(storefile.c_str(), O_RDONLY, 0);
    if (fd == -1) {
        WLOGE("ReadFromDisk open fail");
    }

    struct stat bufstat;
    if (fstat(storefile.c_str(), &bufstat) == -1) {
        WLOGE("ReadFromDisk fstat fail");
        close(fd);
        return;
    }

    size_t filesize = bufstat.st_size;
    if (filesize > maxShaderSize_ * 2) {
        WLOGE("maxShaderSize_");
        close(fd);
    }

    uint8_t *buf = reinterpret_cast<uint8_t*>(mmap(nullptr, filesize, PROT_READ, MAP_PRIVATE, fd, 0));
    if (buf == MAP_FAILED) {
        WLOGE("MAP_FAILED");
        close(fd);
        return;
    }

    if (!validFile(buf, filesize)) {
        munmap(buf, filesize);
        close(fd);
        return;
    }

    size_t headsize = sizeof(CacheHeader);
    size_t byteoffset = headsize;
    while (byteoffset < filesize - headsize) {
        const CacheHeader* eheader = reinterpret_cast<CacheHeader*>(&buf[byteoffset]);
        size_t keysize = eheader->keySize_;
        size_t valuesize = eheader->valueSize_;
        if (byteoffset + headsize + keysize > filesize) {
            break;
        }
        const uint8_t *key = eheader->mData_;
        if (byteoffset + headsize + keysize + valuesize > filesize) {
            break;
        }
        const uint8_t *value = eheader->mData_ + keysize;
        setBlob(key, (EGLsizeiANDROID)keysize, value, (EGLsizeiANDROID)valuesize);
        size_t innerSize = headsize + keysize + valuesize;
        byteoffset += formatfile(innerSize);
    }

    munmap(buf, filesize);
    close(fd);
}

uint32_t BlobCache::crcGen(const uint8_t *buf, size_t len)
{
    const uint32_t polynoimal = 0xEDB88320;
    uint32_t crc = 0xFFFFFFFF;

    for (size_t i = 0; i < len ; ++i) {
        crc ^= (static_cast<uint32_t>(buf[i]) << 24);
        for (size_t j = 0; j < 8; ++j) {
            if (crc & 0x80000000) {
                crc = (crc << 1) ^ polynoimal;
            } else {
                crc <<= 1;
            }
        }
    }
    return crc;
}

bool BlobCache::validFile(uint8_t *buf, size_t len)
{
    if(memcmp(buf, CACHE_MAGIC, 4) != 0) {
        WLOGE("CACHE_MAGIC failed");
        return false;
    }

    uint32_t* crcfile = reinterpret_cast<uint32_t*>(buf + 4);
    uint32_t crccur = crcGen(buf + 8, len -8);
    if (crccur != * crcfile) {
        WLOGE("crc check failed");
        return false;
    }

    return true;
}

void EglSetCacheDir(const std::string dir)
{
    BlobCache::Get()->SetCacheDir(dir);
}
}