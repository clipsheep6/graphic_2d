/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef RS_PROFILER_MESSAGE_HELPER_H
#define RS_PROFILER_MESSAGE_HELPER_H

#include <bitset>

#ifndef REPLAY_TOOL_CLIENT
#include <securec.h>
#endif

#include "rs_profiler_utils.h"

namespace OHOS::Rosen {

#define PACKAGEID_ENUM(XX)       \
    XX(RS_PROFILER_HEADER)       \
    XX(RS_PROFILER_BINARY)       \
    XX(RS_PROFILER_RS_METRICS)   \
    XX(RS_PROFILER_GFX_METRICS)  \
    XX(RS_PROFILER_PREPARE)      \
    XX(RS_PROFILER_PREPARE_DONE) \
    XX(RS_PROFILER_SKP_BINARY)   \
    XX(RS_PROFILER_RDC_BINARY)   \
    XX(RS_PROFILER_DCL_BINARY)

DEFINE_ENUM(PackageID, PACKAGEID_ENUM);
#undef PACKAGEID_ENUM

class BinaryHelper {
public:
    BinaryHelper() = delete;
    BinaryHelper(const BinaryHelper&) = delete;
    BinaryHelper(BinaryHelper&&) = delete;
    BinaryHelper& operator=(const BinaryHelper&) = delete;
    BinaryHelper& operator=(BinaryHelper&&) = delete;

    static PackageID Type(const char* data)
    {
        return static_cast<PackageID>(data[0]);
    }
    static std::vector<char> Data(const char* data, int len)
    {
        return std::vector<char>(data + 1, data + len);
    }
    static uint32_t BinaryCount(const char* data)
    {
        return *(uint32_t*)(data + 1);
    }
    static uint16_t Pid(const char* data)
    {
        return *(uint16_t*)(data + 1 + sizeof(uint32_t));
    }
};

class Packet {
public:
#define PACKET_TYPE_ENUM(XX) \
    XX(BINARY)               \
    XX(COMMAND_ACKNOWLEDGED) \
    XX(COMMAND)              \
    XX(LOG)                  \
    XX(UNKNOWN)
    DEFINE_ENUM_WITH_TYPE(PacketType, uint8_t, PACKET_TYPE_ENUM);
#undef PACKET_TYPE_ENUM

    enum class Header { TYPE = 0, LENGTH = 1 };

    static constexpr size_t headerSize = sizeof(uint32_t) + sizeof(uint8_t);

    explicit Packet(PacketType type);
    Packet(const Packet&) = default;
    Packet& operator=(const Packet&) = default;
    Packet(Packet&&) = default;
    Packet& operator=(Packet&&) = default;

    bool IsBinary() const;

    bool IsCommand() const;

    char* Begin();

    char* End();

    PacketType GetType() const;
    void SetType(PacketType type);
    uint32_t GetLength() const;
    uint32_t GetPayloadLength() const;

    std::vector<char> Release();

    template<typename T, typename = std::enable_if_t<std::is_trivially_copyable_v<T>>>
    [[maybe_unused]] bool Read(T& value);

    template<typename T, typename = std::enable_if_t<std::is_member_function_pointer_v<decltype(&T::size)>>>
    [[maybe_unused]] bool Read(T& value, size_t size);

    [[maybe_unused]] bool Read(void* value, size_t size);

    template<typename T, typename = std::enable_if_t<std::is_trivially_copyable_v<T>>>
    T Read();

    template<typename T, typename = std::enable_if_t<std::is_member_function_pointer_v<decltype(&T::size)>>>
    T Read(size_t size);

    template<typename T>
    [[maybe_unused]] bool Write(const T& value);

    [[maybe_unused]] bool Write(const void* value, size_t size);

private:
    void SetLength(uint32_t length);

    template<typename T>
    bool WriteTrivial(const T& value);

    template<typename T, Header Offset>
    constexpr const T* GetHeaderFieldPtr() const
    {
        return const_cast<Packet*>(this)->GetHeaderFieldPtr<T, Offset>();
    }
    template<typename T, Header Offset>
    constexpr T* GetHeaderFieldPtr()
    {
        return reinterpret_cast<T*>(data_.data() + static_cast<int>(Offset));
    }

    void InitData(PacketType type);

private:
    size_t readPointer_ = headerSize;
    size_t writePointer_ = headerSize;
    std::vector<char> data_ = { 0, 0, 0, 0, 0 };
};

template<typename T, typename>
[[maybe_unused]] inline bool Packet::Read(T& value)
{
    if (readPointer_ + sizeof(T) > data_.size()) {
        return false;
    }
    value = *reinterpret_cast<T*>(data_.data() + readPointer_);
    readPointer_ += sizeof(T);
    return true;
}

template<typename T, typename>
[[maybe_unused]] inline bool Packet::Read(T& value, size_t size)
{
    value.resize(size);
    return Read((void*)value.data(), size * sizeof(typename T::value_type));
}

[[maybe_unused]] inline bool Packet::Read(void* value, size_t size)
{
    if (readPointer_ + size > data_.size()) {
        return false;
    }
    if (::memcpy_s((void*)value, size, (void*)(data_.data() + readPointer_), size) != 0) {
        return false;
    }
    readPointer_ += size;
    return true;
}

template<typename T, typename>
inline T Packet::Read()
{
    T v {};
    Read(v);
    return v;
}

template<typename T, typename>
inline T Packet::Read(size_t size)
{
    T v {};
    Read(v, size);
    return v;
}

template<typename T>
[[maybe_unused]] inline bool Packet::Write(const T& value)
{
    if constexpr (std::is_trivially_copyable_v<T>) {
        return WriteTrivial(value);
    } else if constexpr (std::is_member_function_pointer_v<decltype(&T::size)>) {
        return Write((const void*)value.data(), value.size() * sizeof(typename T::value_type));
    }
    return false;
}

[[maybe_unused]] inline bool Packet::Write(const void* value, size_t size)
{
    data_.resize(data_.size() + size);
    if (memcpy_s((void*)(data_.data() + writePointer_), size, value, size) != 0) {
        data_.resize(data_.size() - size);
        return false;
    }
    writePointer_ += size;
    SetLength(data_.size());
    return true;
}

template<typename T>
inline bool Packet::WriteTrivial(const T& value)
{
    if (writePointer_ + sizeof(T) > data_.size()) {
        data_.resize(data_.size() + (sizeof(T) - (data_.size() - writePointer_)));
    }
    *reinterpret_cast<T*>((data_.data() + writePointer_)) = value;
    writePointer_ += sizeof(T);
    SetLength(data_.size());
    return true;
}

} // namespace OHOS::Rosen

#endif // RS_PROFILER_MESSAGE_HELPER_H
