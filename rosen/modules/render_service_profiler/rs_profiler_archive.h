/*
 * Copyright (c) Huawei Device Co., Ltd.
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

#ifndef RENDER_SERVICE_PROFILER_ARCHIVE_H
#define RENDER_SERVICE_PROFILER_ARCHIVE_H

#include <cstring>
#include <iostream>
#include <string>
#include <vector>
#include <securec.h>

namespace OHOS::Rosen {

class Archive {
public:
    bool IsReading() const
    {
        return isReading_;
    }

    void Serialize(float& value);
    void Serialize(double& value);
    void Serialize(int32_t& value);
    void Serialize(uint32_t& value);
    void Serialize(size_t& value);
    void Serialize(std::string& value);

    template<typename T>
    void Serialize(std::vector<T>& vector)
    {
        SerializeVectorBase(vector);
        for (T& value : vector)
            Serialize(value);
    }

    template<typename T>
    void Serialize(std::vector<T>& vector, void (*serializer)(Archive&, T&))
    {
        SerializeVectorBase(vector);
        for (T& value : vector)
            serializer(*this, value);
    }

    template<typename T>
    void SerializeNonFlat(std::vector<T>& vector)
    {
        SerializeVectorBase(vector);
        for (T& value : vector)
            value.Serialize(*this);
    }

    virtual void Serialize(void* data, size_t size) = 0;

protected:
    explicit Archive(bool reader)
        : isReading_(reader)
    {}

    virtual ~Archive() = default;

    template<typename T>
    void SerializeVectorBase(std::vector<T>& vector)
    {
        size_t size = vector.size();
        Serialize(size);

        if (IsReading())
            vector.resize(size);
    }

private:

    bool isReading_ = true;
};

// Data archives

class DataArchive : public Archive {
public:

    ~DataArchive() = default;

protected:

    explicit DataArchive(bool reader, size_t offset = 0)
        : Archive(reader)
        , offset_(offset)
    {}

protected:
    size_t offset_ = 0;
};

class DataReader final : public DataArchive {
public:
    DataReader(const std::vector<char>& data, size_t offset = 0)
        : DataArchive(true, offset)
        , data_(data)
    {}

    void Serialize(void* data, size_t size) override
    {
        if (data && (size > 0) && (offset_ + size <= data_.size())) {
            if (::memmove_s(data, size, data_.data() + offset_, size) == 0)
                offset_ += size;
        }
    }

protected:
    const std::vector<char>& data_;
};

class DataWriter final : public DataArchive {
public:
    DataWriter(std::vector<char>& data, size_t offset = 0)
        : DataArchive(false, offset)
        , data_(data)
    {}

    void Serialize(void* data, size_t size) override
    {
        if (data && (size > 0)) {
            data_.resize(data_.size() + size);
            if(::memmove_s(data_.data() + offset_, data_.size(), data, size) == 0)
                offset_ += size;
        }
    }

protected:
    std::vector<char>& data_;
};

// File archives

template<bool reader = true>
class FileArchive final : public Archive {
public:
    explicit FileArchive(FILE* file)
        : Archive(reader) 
        , file_(file)
        , external_(true)
    {}

    explicit FileArchive(const std::string& path)
        : Archive(reader) 
        , file_(fopen(path.data(), reader ? "rb" : "wb"))
        , external_(false)
    {}

    ~FileArchive()
    {
        if (file_ && !external_)
            fclose(file_);
    }

    void Serialize(void* data, size_t size) override
    {
        if (file_ && data && (size > 0)) {
            if (IsReading())
                fread(data, size, 1, file_);
            else
                fwrite(data, size, 1, file_);
        }
    }

protected:
    FILE* file_ = nullptr;
    bool external_ = false;
};

using FileReader = FileArchive<true>;
using FileWriter = FileArchive<false>;

} // namespace OHOS::Rosen

#endif // RENDER_SERVICE_PROFILER_ARCHIVE_H