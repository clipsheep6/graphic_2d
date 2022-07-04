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

#include "hardware_buffer.h"

#include <cinttypes>
#include <surface_type.h>
#include "buffer_log.h"
#include "external_window.h"
#include "surface_buffer_impl.h"

using namespace OHOS;

OH_HardwareBuffer* HardwareBufferFromSurfaceBuffer(SurfaceBuffer* buffer)
{
    return buffer->SurfaceBufferToHardwareBuffer();
}

const SurfaceBuffer* OHHardwareBufferToSurfaceBuffer(const OH_HardwareBuffer *buffer)
{
    return SurfaceBuffer::HardwareBufferToSurfaceBuffer(buffer);
}

SurfaceBuffer* OHHardwareBufferToSurfaceBuffer(OH_HardwareBuffer *buffer)
{
    return SurfaceBuffer::HardwareBufferToSurfaceBuffer(buffer);
}

OH_HardwareBuffer* OH_HardwareBuffer_HardwareBufferAlloc(const OH_HardwareBuffer_Config* config)
{
    if (config == nullptr) {
        BLOGE("parameter error, please check input parameter");
        return nullptr;
    }
    BufferRequestConfig bfConfig = {};
    bfConfig.width = config->width;
    bfConfig.height = config->height;
    bfConfig.strideAlignment = 0x8; // set 0x8 as default value to alloc SurfaceBufferImpl
    bfConfig.format = config->format; // PixelFormat
    bfConfig.usage = config->usage;
    bfConfig.timeout = 0;
    bfConfig.colorGamut = ColorGamut::COLOR_GAMUT_SRGB;
    bfConfig.transform = TransformType::ROTATE_NONE;
    sptr<SurfaceBuffer> bufferImpl = new SurfaceBufferImpl();
    GSError ret = bufferImpl->Alloc(bfConfig);
    if (ret != GSERROR_OK) {
        BLOGE("Surface Buffer Alloc failed, %{public}s", GSErrorStr(ret).c_str());
        return nullptr;
    }

    OH_HardwareBuffer* buffer = HardwareBufferFromSurfaceBuffer(bufferImpl);
    int32_t err = OH_HardwareBuffer_HardwareBufferReference(buffer);
    if (err != OHOS::GSERROR_OK) {
        BLOGE("HardwareBufferReference failed");
        return nullptr;
    }
    return buffer;
}

int32_t OH_HardwareBuffer_HardwareBufferReference(OH_HardwareBuffer *buffer)
{
    if (buffer == nullptr) {
        BLOGE("parameter error, please check input parameter");
        return OHOS::GSERROR_INVALID_ARGUMENTS;
    }
    OHOS::RefBase *ref = reinterpret_cast<OHOS::RefBase *>(buffer);
    ref->IncStrongRef(ref);
    return OHOS::GSERROR_OK;
}

int32_t OH_HardwareBuffer_HardwareBufferUnreference(OH_HardwareBuffer *buffer)
{
    if (buffer == nullptr) {
        BLOGE("parameter error, please check input parameter");
        return OHOS::GSERROR_INVALID_ARGUMENTS;
    }
    OHOS::RefBase *ref = reinterpret_cast<OHOS::RefBase *>(buffer);
    ref->DecStrongRef(ref);
    return OHOS::GSERROR_OK;
}

void OH_HardwareBuffer_GetHardwareBufferConfig(OH_HardwareBuffer *buffer, OH_HardwareBuffer_Config* config)
{
    if (buffer == nullptr || config == nullptr) {
        BLOGE("parameter error, please check input parameter");
        config = nullptr;
        return;
    }
    const SurfaceBuffer* sbuffer = OHHardwareBufferToSurfaceBuffer(buffer);
    config->width = sbuffer->GetWidth();
    config->height = sbuffer->GetHeight();
    config->format = sbuffer->GetFormat();
    config->usage = sbuffer->GetUsage();
}

int32_t OH_HardwareBuffer_HardwareBufferMap(OH_HardwareBuffer *buffer, void **virAddr)
{
    if (buffer == nullptr) {
        BLOGE("parameter error, please check input parameter");
        return OHOS::GSERROR_INVALID_ARGUMENTS;
    }
    SurfaceBuffer* sbuffer = OHHardwareBufferToSurfaceBuffer(buffer);
    int32_t ret = sbuffer->Map();
    if (ret == OHOS::GSERROR_OK) {
        *virAddr = sbuffer->GetVirAddr();
    }
    return ret;
}

int32_t OH_HardwareBuffer_HardwareBufferUnmap(OH_HardwareBuffer *buffer)
{
    if (buffer == nullptr) {
        BLOGE("parameter error, please check input parameter");
        return OHOS::GSERROR_INVALID_ARGUMENTS;
    }
    SurfaceBuffer* sbuffer = OHHardwareBufferToSurfaceBuffer(buffer);
    return sbuffer->Unmap();
}

uint32_t OH_HardwareBuffer_HardwareBufferGetSeqNum(OH_HardwareBuffer *buffer)
{
    if (buffer == nullptr) {
        BLOGE("parameter error, please check input parameter");
        return OHOS::GSERROR_INVALID_ARGUMENTS;
    }
    const SurfaceBuffer* sbuffer = OHHardwareBufferToSurfaceBuffer(buffer);
    return sbuffer->GetSeqNum();
}