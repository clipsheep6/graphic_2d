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

#include <map>
#include <buffer_handle_parcel.h>
#include <buffer_handle_utils.h>
#include <surface_buffer.h>
#include "egl_data.h"

namespace OHOS {
class SurfaceBufferImpl : public SurfaceBuffer {
public:
    SurfaceBufferImpl();
    SurfaceBufferImpl(int seqNum);
    DELETE_COPY_AND_ASSIGN(SurfaceBufferImpl);
    virtual ~SurfaceBufferImpl();

    static SurfaceBufferImpl *FromBase(const sptr<SurfaceBuffer>& buffer);

    BufferHandle *GetBufferHandle() const override;
    int32_t GetWidth() const override;
    int32_t GetHeight() const override;
    int32_t GetStride() const override;
    int32_t GetFormat() const override;
    int64_t GetUsage() const override;
    uint64_t GetPhyAddr() const override;
    int32_t GetKey() const override;
    void *GetVirAddr() const override;
    int32_t GetFileDescriptor() const override;
    uint32_t GetSize() const override;

    int32_t GetSurfaceBufferWidth() const override;
    int32_t GetSurfaceBufferHeight() const override;
    ColorGamut GetSurfaceBufferColorGamut() const override;
    TransformType GetSurfaceBufferTransform() const override;
    GSError SetSurfaceBufferWidth(int32_t width) override;
    GSError SetSurfaceBufferHeight(int32_t height) override;
    GSError SetSurfaceBufferColorGamut(ColorGamut colorGamut) override;
    GSError SetSurfaceBufferTransform(TransformType transform) override;

    int32_t GetSeqNum() const override;

    sptr<EglData> GetEglData() const override;
    void SetEglData(const sptr<EglData>& data) override;

    void SetExtraData(const sptr<BufferExtraData> &bedata) override;
    void GetExtraData(sptr<BufferExtraData> &bedata) const override;

    void SetBufferHandle(BufferHandle *handle) override;
    void WriteToMessageParcel(MessageParcel &parcel) override;

    GSError Alloc(const BufferRequestConfig &config) override;
    GSError Map() override;
    GSError Unmap() override;
    GSError FlushCache() override;
    GSError InvalidateCache() override;

private:
    BufferHandle *handle_ = nullptr;
    int32_t sequenceNumber_ = -1;
    sptr<BufferExtraData> bedata_ = nullptr;
    sptr<EglData> eglData_ = nullptr;
    int32_t surfaceBufferWidth_ = 0;
    int32_t surfaceBufferHeight_ = 0;
    ColorGamut surfaceBufferColorGamut_ = ColorGamut::COLOR_GAMUT_SRGB;
    TransformType transform_ = TransformType::ROTATE_NONE;
};
} // namespace OHOS

#endif // FRAMEWORKS_SURFACE_INCLUDE_SURFACE_BUFFER_IMPL_H
