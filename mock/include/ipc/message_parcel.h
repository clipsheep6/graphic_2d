/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#ifndef MOCK_MESSAGE_PARCEL_H
#define MOCK_MESSAGE_PARCEL_H

#include <ashmem.h>
#include "parcel.h"
#include "refbase.h"
#include <string>

namespace OHOS {
class IRemoteObject;
class MessageParcel : public Parcel {
public:
    MessageParcel();
    ~MessageParcel();
    explicit MessageParcel(Allocator *allocator);
    bool WriteRemoteObject(const sptr<IRemoteObject> &object)
    {
      return false;
    }
    sptr<IRemoteObject> ReadRemoteObject();
    bool WriteFileDescriptor(int fd) 
    {
      return false;
    }
    int ReadFileDescriptor()
    {
      return 0;
    }
    bool ContainFileDescriptors() const
    {
      return false;
    }
    bool WriteInterfaceToken(std::u16string name)
    {
      return 0;
    }
    std::u16string ReadInterfaceToken()
    {
      return {};
    }
    bool WriteRawData(const void *data, size_t size)
    {
      return false;
    }
    const void *ReadRawData(size_t size)
    {
      return nullptr;
    }
    bool RestoreRawData(std::shared_ptr<char> rawData, size_t size)
    {
      return false;
    }
    const void *GetRawData() const
    {
      return nullptr;
    }
    size_t GetRawDataSize() const
    {
      return 0;
    }
    size_t GetRawDataCapacity() const
    {
      return 0;
    }
    void WriteNoException()
    {}
    int32_t ReadException()
    {
      return 0;
    }
    bool WriteAshmem(sptr<Ashmem> ashmem)
    {
      return false;
    }
    sptr<Ashmem> ReadAshmem()
    {
      return nullptr;
    }
    void ClearFileDescriptor()
    {}
    void SetClearFdFlag()
    {
        needCloseFd_ = true;
    };
    bool Append(MessageParcel &data)
    {
      return false;
    }

private:
#ifndef CONFIG_IPC_SINGLE
    bool WriteDBinderProxy(const sptr<IRemoteObject> &object, uint32_t handle, uint64_t stubIndex)
    {
      return false;
    }
#endif
    static constexpr size_t MAX_RAWDATA_SIZE = 128 * 1024 * 1024; // 128M
    static constexpr size_t MIN_RAWDATA_SIZE = 32 * 1024;         // 32k
    bool needCloseFd_ = false;
    std::vector<sptr<Parcelable>> holders_;
    int writeRawDataFd_;
    int readRawDataFd_;
    void *kernelMappedWrite_;
    void *kernelMappedRead_;
    std::shared_ptr<char> rawData_;
    size_t rawDataSize_;
};
} // namespace OHOS
#endif // MOCK_MESSAGE_PARCEL_H
