/*
* Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef OHOS_CACHEDATA_H
#define OHOS_CACHEDATA_H

#include <cstddef>
#include <memory>
#include <vector>
#include <string>
#include <fcntl.h>
#include <unistd.h>
#include <cstdint>

namespace OHOS {
namespace Rosen {
namespace GLCache {
class CacheData {
public:
    CacheData(size_t maxKeySize, size_t maxValueSize, size_t maxTotalSize, const std::string& fileName);

    void Rewrite(const void *key, size_t keySize, const void *value, size_t valueSize);
    
    size_t Get(const void *key, size_t keySize, void *value, size_t valueSize);
    
    size_t SerializedSize() const;
    
    int Serialize(uint8_t *buffer, size_t size) const;

    void WriteToFile();
    
    int DeSerialize(uint8_t const *buffer, size_t size);

    void Clear()
    {
        shaderPointers_.clear();
    }

protected:
    const size_t maxKeySize_;
    const size_t maxValueSize_;
    const size_t maxTotalSize_;

private:
    CacheData(const CacheData&);
    void operator=(const CacheData&);
    
    unsigned short cleanInit_[3];
    size_t cleanThreshold_ = 0;
    void CheckClean(size_t newSize);
    void RandClean(size_t cleanThreshold);
    size_t Clean(int removeIndex);

    static inline size_t Align4(size_t size)
    {
        return (size + ALIGN_FOUR) & ~ALIGN_FOUR;
    }

    class DataPointer {
    public:
        DataPointer(const void *data, size_t size, bool ifOccupy);
        ~DataPointer();

        bool operator<(const DataPointer& rValue) const
        {
            if (size_ == rValue.size_) {
                return memcmp(pointer_, rValue.pointer_, size_) < 0;
            } else {
                return size_ < rValue.size_;
            }
        }
        const void *GetData() const
        {
            return pointer_;
        }
        size_t GetSize() const
        {
            return size_;
        }

    private:
        DataPointer(const DataPointer&);
        void operator=(const DataPointer&);
        const void *pointer_;
        size_t size_;
        bool toFree_;
    };

    class ShaderPointer {
    public:
        ShaderPointer();
        ShaderPointer(const std::shared_ptr<DataPointer>& key, const std::shared_ptr<DataPointer>& value);
        ShaderPointer(const ShaderPointer& sp);
        bool operator<(const ShaderPointer& rValue) const
        {
            return *keyPointer_ < *rValue.keyPointer_;
        }
        const ShaderPointer& operator=(const ShaderPointer& rValue)
        {
            keyPointer_ = rValue.keyPointer_;
            valuePointer_ = rValue.valuePointer_;
            return *this;
        }
        std::shared_ptr<DataPointer> GetKeyPointer() const
        {
            return keyPointer_;
        }
        std::shared_ptr<DataPointer> GetValuePointer() const
        {
            return valuePointer_;
        }
        void SetValue(const std::shared_ptr<DataPointer>& value)
        {
            valuePointer_ = value;
        }

    private:
        std::shared_ptr<DataPointer> keyPointer_;
        std::shared_ptr<DataPointer> valuePointer_;
    };

    struct Header {
        size_t numShaders_;
    };

    struct ShaderData {
        size_t keySize_;
        size_t valueSize_;
        uint8_t data_[];
    };

    size_t totalSize_ = 0;
    std::vector<ShaderPointer> shaderPointers_;
    std::string cacheDir_;

    const size_t CLEAN_TO = 2;
    static const size_t ALIGN_FOUR = 3;
    const int RAND_SHIFT = 16;
    const int RAND_LENGTH = 3;
};
}   // namespace GLCache
}   // namespace Rosen
}   // namespace OHOS
#endif // OHOS_CACHEDATA_H
