/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef FRAMEWORKS_SURFACE_INCLUDE_SURFACE_BUFFER_IMPL_H
#define FRAMEWORKS_SURFACE_INCLUDE_SURFACE_BUFFER_IMPL_H

#include <any>
#include <map>

#include <buffer_extra_data.h>
#include <buffer_handle_parcel.h>
#include <buffer_handle_utils.h>
#include <surface_buffer.h>

#include "egl_data.h"

namespace OHOS {
enum ExtraDataType {
    EXTRA_DATA_TYPE_MIN,
    EXTRA_DATA_TYPE_INT32,
    EXTRA_DATA_TYPE_INT64,
    EXTRA_DATA_TYPE_MAX,
};

typedef struct {
    std::any value;
    ExtraDataType type;
} ExtraData;

class MessageParcel;
class SurfaceBufferImpl : public SurfaceBuffer {
public:
    SurfaceBufferImpl();
    SurfaceBufferImpl(int seqNum);
    virtual ~SurfaceBufferImpl();

    BufferHandle *GetBufferHandle() const override;
    int32_t GetWidth() const override;
    int32_t GetHeight() const override;
    int32_t GetFormat() const override;
    int64_t GetUsage() const override;
    uint64_t GetPhyAddr() const override;
    int32_t GetKey() const override;
    void *GetVirAddr() const override;
    int32_t GetFileDescriptor() const override;
    uint32_t GetSize() const override;

    SurfaceError SetInt32(uint32_t key, int32_t val) override;
    SurfaceError GetInt32(uint32_t key, int32_t &val) override;
    SurfaceError SetInt64(uint32_t key, int64_t val) override;
    SurfaceError GetInt64(uint32_t key, int64_t &val) override;

    virtual SurfaceError ExtraGet(std::string key, int32_t &value) const override;
    virtual SurfaceError ExtraGet(std::string key, int64_t &value) const override;
    virtual SurfaceError ExtraGet(std::string key, double &value) const override;
    virtual SurfaceError ExtraGet(std::string key, std::string &value) const override;
    virtual SurfaceError ExtraSet(std::string key, int32_t value) override;
    virtual SurfaceError ExtraSet(std::string key, int64_t value) override;
    virtual SurfaceError ExtraSet(std::string key, double value) override;
    virtual SurfaceError ExtraSet(std::string key, std::string value) override;

    static int32_t GetSeqNum(const sptr<SurfaceBuffer> &buffer);
    static void SetExtraData(sptr<SurfaceBuffer> &buffer, const std::shared_ptr<BufferExtraData> &bedata);
    static void GetExtraData(const sptr<SurfaceBuffer> &buffer, std::shared_ptr<BufferExtraData> &bedata);
    static void SetBufferHandle(sptr<SurfaceBuffer> &buffer, BufferHandle *handle);
    static void WriteToMessageParcel(const sptr<SurfaceBuffer> &buffer, MessageParcel &parcel);
    static sptr<EglData> GetEglData(const sptr<SurfaceBuffer> &buffer);
    static void SetEglData(sptr<SurfaceBuffer> &buffer, const sptr<EglData>& data);

private:
    SurfaceError SetData(uint32_t key, ExtraData data);
    SurfaceError GetData(uint32_t key, ExtraData &data);
    std::map<uint32_t, ExtraData> extraDatas_;

    BufferHandle *handle_ = nullptr;
    int32_t sequenceNumber = -1;
    std::shared_ptr<BufferExtraData> bedata_ = nullptr;
    sptr<EglData> eglData_ = nullptr;
};
} // namespace OHOS

#endif // FRAMEWORKS_SURFACE_INCLUDE_SURFACE_BUFFER_IMPL_H
