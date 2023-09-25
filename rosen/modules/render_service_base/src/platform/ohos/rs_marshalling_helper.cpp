/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#include "transaction/rs_marshalling_helper.h"

#include <cstdint>
#include <memory>
#include <message_parcel.h>
#include <sys/mman.h>
#include <unistd.h>

#include "ashmem.h"
#ifndef USE_ROSEN_DRAWING
#include "include/core/SkDrawable.h"
#include "include/core/SkImage.h"
#include "include/core/SkMatrix.h"
#include "include/core/SkPaint.h"
#include "include/core/SkPicture.h"
#include "include/core/SkSerialProcs.h"
#include "include/core/SkStream.h"
#include "include/core/SkTextBlob.h"
#include "include/core/SkTypeface.h"
#include "include/core/SkVertices.h"
#ifdef NEW_SKIA
#include "include/core/SkSamplingOptions.h"
#include "src/core/SkVerticesPriv.h"
#endif
#endif
#include "memory/rs_memory_track.h"
#include "pixel_map.h"
#include "securec.h"
#ifndef USE_ROSEN_DRAWING
#include "src/core/SkAutoMalloc.h"
#include "src/core/SkPaintPriv.h"
#include "src/core/SkReadBuffer.h"
#include "src/core/SkWriteBuffer.h"
#include "src/image/SkImage_Base.h"
#else
#include "recording/recording_path.h"
#include "recording/recording_shader_effect.h"
#endif

#include "animation/rs_render_curve_animation.h"
#include "animation/rs_render_interpolating_spring_animation.h"
#include "animation/rs_render_keyframe_animation.h"
#include "animation/rs_render_particle.h"
#include "animation/rs_render_particle_animation.h"
#include "animation/rs_render_path_animation.h"
#include "animation/rs_render_spring_animation.h"
#include "animation/rs_render_transition.h"
#include "common/rs_color.h"
#include "common/rs_common_def.h"
#include "common/rs_matrix3.h"
#include "common/rs_vector4.h"
#include "modifier/rs_render_modifier.h"
#ifndef USE_ROSEN_DRAWING
#include "pipeline/rs_draw_cmd.h"
#include "pipeline/rs_draw_cmd_list.h"
#endif
#include "platform/common/rs_log.h"
#include "render/rs_blur_filter.h"
#include "render/rs_filter.h"
#include "render/rs_gradient_blur_para.h"
#include "render/rs_image.h"
#include "render/rs_image_base.h"
#include "render/rs_light_up_effect_filter.h"
#include "render/rs_material_filter.h"
#include "render/rs_path.h"
#include "render/rs_shader.h"
#include "transaction/rs_ashmem_helper.h"
#if defined (ENABLE_DDGR_OPTIMIZE)
#include <sys/mman.h>
#include "securec.h"
#include "platform/common/rs_system_properties.h"
#include "ipc_file_descriptor.h"
#include "src/core/SkTextBlobPriv.h"
#include "ddgr_renderer.h"
#endif

namespace OHOS {
namespace Rosen {

namespace {
    bool g_useSharedMem = true;
    std::thread::id g_tid = std::thread::id();
}

 
#define MARSHALLING_AND_UNMARSHALLING(TYPE, TYPENAME)                      \
    bool RSMarshallingHelper::Marshalling(Parcel& parcel, const TYPE& val) \
    {                                                                      \
        return parcel.Write##TYPENAME(val);                                \
    }                                                                      \
    bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, TYPE& val)     \
    {                                                                      \
        return parcel.Read##TYPENAME(val);                                 \
    }

// basic types
MARSHALLING_AND_UNMARSHALLING(bool, Bool)
MARSHALLING_AND_UNMARSHALLING(int8_t, Int8)
MARSHALLING_AND_UNMARSHALLING(uint8_t, Uint8)
MARSHALLING_AND_UNMARSHALLING(int16_t, Int16)
MARSHALLING_AND_UNMARSHALLING(uint16_t, Uint16)
MARSHALLING_AND_UNMARSHALLING(int32_t, Int32)
MARSHALLING_AND_UNMARSHALLING(uint32_t, Uint32)
MARSHALLING_AND_UNMARSHALLING(int64_t, Int64)
MARSHALLING_AND_UNMARSHALLING(uint64_t, Uint64)
MARSHALLING_AND_UNMARSHALLING(float, Float)
MARSHALLING_AND_UNMARSHALLING(double, Double)

#undef MARSHALLING_AND_UNMARSHALLING

namespace {
template<typename T, typename P>
static inline sk_sp<T> sk_reinterpret_cast(sk_sp<P> ptr)
{
    return sk_sp<T>(static_cast<T*>(SkSafeRef(ptr.get())));
}
} // namespace

#ifndef USE_ROSEN_DRAWING
// SkData
bool RSMarshallingHelper::Marshalling(Parcel& parcel, sk_sp<SkData> val)
{
    if (!val) {
        return parcel.WriteInt32(-1);
    }

    bool ret = parcel.WriteInt32(val->size());
    if (val->size() == 0) {
        ROSEN_LOGW("unirender: RSMarshallingHelper::Marshalling SkData size is 0");
        return ret;
    }

    ret = ret && RSMarshallingHelper::WriteToParcel(parcel, val->data(), val->size());
    if (!ret) {
        ROSEN_LOGE("unirender: failed RSMarshallingHelper::Marshalling SkData");
    }
    return ret;
}
bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, sk_sp<SkData>& val)
{
    int32_t size = parcel.ReadInt32();
    if (size == -1) {
        val = nullptr;
        return true;
    }
    if (size == 0) {
        ROSEN_LOGW("unirender: RSMarshallingHelper::Unmarshalling SkData size is 0");
        val = SkData::MakeEmpty();
        return true;
    }

    const void* data = RSMarshallingHelper::ReadFromParcel(parcel, size);
    if (data == nullptr) {
        ROSEN_LOGE("unirender: failed RSMarshallingHelper::Unmarshalling SkData");
        return false;
    }
    if (static_cast<uint32_t>(size) < MIN_DATA_SIZE || (!g_useSharedMem && g_tid == std::this_thread::get_id())) {
        val = SkData::MakeWithoutCopy(data, size);
    } else {
        val = SkData::MakeFromMalloc(data, size);
    }
    return val != nullptr;
}
bool RSMarshallingHelper::SkipSkData(Parcel& parcel)
{
    int32_t size = parcel.ReadInt32();
    if (size <= 0) {
        return true;
    }
    return SkipFromParcel(parcel, size);
}

bool RSMarshallingHelper::UnmarshallingWithCopy(Parcel& parcel, sk_sp<SkData>& val)
{
    bool success = Unmarshalling(parcel, val);
    if (success) {
        if (val && (val->size() < MIN_DATA_SIZE || (!g_useSharedMem && g_tid == std::this_thread::get_id()))) {
            val = SkData::MakeWithCopy(val->data(), val->size());
        }
    }
    return success;
}
#else
// Drawing::Data
bool RSMarshallingHelper::Marshalling(Parcel& parcel, std::shared_ptr<Drawing::Data> val)
{
    if (!val) {
        return parcel.WriteInt32(-1);
    }

    bool ret = parcel.WriteInt32(val->GetSize());
    if (val->GetSize() == 0) {
        ROSEN_LOGW("unirender: RSMarshallingHelper::Marshalling Data size is 0");
        return ret;
    }

    ret = ret && RSMarshallingHelper::WriteToParcel(parcel, val->GetData(), val->GetSize());
    if (!ret) {
        ROSEN_LOGE("unirender: failed RSMarshallingHelper::Marshalling Data");
    }
    return ret;
}

bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, std::shared_ptr<Drawing::Data>& val)
{
    int32_t size = parcel.ReadInt32();
    if (size == -1) {
        val = nullptr;
        return true;
    }
    val = std::make_shared<Drawing::Data>();
    if (size == 0) {
        ROSEN_LOGW("unirender: RSMarshallingHelper::Unmarshalling Data size is 0");
        val->BuildUninitialized(0);
        return true;
    }

    const void* data = RSMarshallingHelper::ReadFromParcel(parcel, size);
    if (data == nullptr) {
        ROSEN_LOGE("unirender: failed RSMarshallingHelper::Unmarshalling Data");
        return false;
    }

    if (static_cast<uint32_t>(size) < MIN_DATA_SIZE) {
        val->BuildWithoutCopy(data, size);
    } else {
        val->BuildFromMalloc(data, size);
    }
    return true;
}

bool RSMarshallingHelper::SkipData(Parcel& parcel)
{
    int32_t size = parcel.ReadInt32();
    if (size <= 0) {
        return true;
    }
    return SkipFromParcel(parcel, size);
}

bool RSMarshallingHelper::UnmarshallingWithCopy(Parcel& parcel, std::shared_ptr<Drawing::Data>& val)
{
    bool success = Unmarshalling(parcel, val);
    if (success) {
        if (val && val->GetSize() < MIN_DATA_SIZE) {
            val->BuildWithCopy(val->GetData(), val->GetSize());
        }
    }
    return success;
}
#endif

#ifndef USE_ROSEN_DRAWING
// SkTypeface serial proc
sk_sp<SkData> RSMarshallingHelper::SerializeTypeface(SkTypeface* tf, void* ctx)
{
    if (tf == nullptr) {
        ROSEN_LOGD("unirender: RSMarshallingHelper::SerializeTypeface SkTypeface is nullptr");
        return nullptr;
    }
    return tf->serialize();
}

// SkTypeface deserial proc
sk_sp<SkTypeface> RSMarshallingHelper::DeserializeTypeface(const void* data, size_t length, void* ctx)
{
    SkMemoryStream stream(data, length);
    return SkTypeface::MakeDeserialize(&stream);
}

#if defined (ENABLE_DDGR_OPTIMIZE)
int RSMarshallingHelper::IntegrateReadDescriptor(Parcel& parcel)
{
    sptr<IPCFileDescriptor> descriptor = parcel.ReadObject<IPCFileDescriptor>();
    if (descriptor == nullptr) {
        return -1;
    }
    return descriptor->GetFd();
}

bool RSMarshallingHelper::IntegrateWriteDescriptor(Parcel& parcel, int fId)
{
    sptr<IPCFileDescriptor> descriptor = new (std::nothrow) IPCFileDescriptor(fId);
    if (descriptor == nullptr) {
        return false;
    }
    return parcel.WriteObject<IPCFileDescriptor>(descriptor);
}

bool RSMarshallingHelper::SerializeInternal(Parcel& parcel, const sk_sp<SkTextBlob>& val, const SkSerialProcs& procs)
{
    SkBinaryWriteBuffer buffer;
    buffer.setSerialProcs(procs);
    val->TextBlobFlatten(buffer);
    size_t total = buffer.bytesWritten();
    int fId = -1;
    sk_sp<SkData> data;

    void* dataPtr = DDGRRenderer::GetInstance().IntegrateMemAlloc(fId, total);
    if (dataPtr != nullptr) {
        data = SkData::MakeUninitialized(1);
        buffer.writeToMemory(dataPtr);
        int fId2 = ::dup(fId);
        val->TextBlobSetShareParas(fId2, total, dataPtr);
    } else {
        data = SkData::MakeUninitialized(total);
        buffer.writeToMemory(data->writable_data());
    }
    bool ret = Marshalling(parcel, data);
    IntegrateWriteDescriptor(parcel, fId);
    return ret;
}

bool RSMarshallingHelper::DserializeInternal(Parcel& parcel, sk_sp<SkTextBlob>& val,
    const SkDeserialProcs& procs, sk_sp<SkData>& data)
{
    int sizePtr = 0;
    int fId = IntegrateReadDescriptor(parcel);
    fId = ::dup(fId);
    void* dataPtr = DDGRRenderer::GetInstance().IntegrateGetHandle(fId, sizePtr);
    if (dataPtr != nullptr && sizePtr > 0) {
        val = SkTextBlob::Deserialize(dataPtr, sizePtr, procs);
        val->TextBlobSetShareParas(fId, sizePtr, dataPtr);
        return val != nullptr;
    }
    val = SkTextBlob::Deserialize(data->data(), data->size(), procs);
    return val != nullptr;
}
#endif

// SkTextBlob
bool RSMarshallingHelper::Marshalling(Parcel& parcel, const sk_sp<SkTextBlob>& val)
{
    sk_sp<SkData> data;
    if (!val) {
        ROSEN_LOGD("unirender: RSMarshallingHelper::Marshalling SkTextBlob is nullptr");
        return Marshalling(parcel, data);
    }
    SkSerialProcs serialProcs;
    serialProcs.fTypefaceProc = &RSMarshallingHelper::SerializeTypeface;
#if defined (ENABLE_DDGR_OPTIMIZE)
    if (RSSystemProperties::GetDDGRIntegrateEnable()) {
        ROSEN_LOGD("Marshalling text Integrate");
        return SerializeInternal(parcel, val, serialProcs);
    }
#endif
    data = val->serialize(serialProcs);
    return Marshalling(parcel, data);
}
bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, sk_sp<SkTextBlob>& val)
{
    sk_sp<SkData> data;
    if (!Unmarshalling(parcel, data)) {
        ROSEN_LOGE("unirender: failed RSMarshallingHelper::Unmarshalling SkTextBlob");
        return false;
    }
    if (data == nullptr) {
        val = nullptr;
        return true;
    }
    SkDeserialProcs deserialProcs;
    deserialProcs.fTypefaceProc = &RSMarshallingHelper::DeserializeTypeface;
#if defined (ENABLE_DDGR_OPTIMIZE)
    if (RSSystemProperties::GetDDGRIntegrateEnable()) {
        ROSEN_LOGD("Unmarshalling text Integrate");
        return DserializeInternal(parcel, val, deserialProcs, data);
    }
#endif
    val = SkTextBlob::Deserialize(data->data(), data->size(), deserialProcs);
    return val != nullptr;
}

// SkPaint
bool RSMarshallingHelper::Marshalling(Parcel& parcel, const SkPaint& val)
{
    SkBinaryWriteBuffer writer;
    writer.writePaint(val);
    size_t length = writer.bytesWritten();
    sk_sp<SkData> data = SkData::MakeUninitialized(length);
    writer.writeToMemory(data->writable_data());
    return Marshalling(parcel, data);
}
bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, SkPaint& val)
{
    sk_sp<SkData> data;
    if (!Unmarshalling(parcel, data) || !data) {
        ROSEN_LOGE("unirender: failed RSMarshallingHelper::Unmarshalling SkPaint");
        return false;
    }
    SkReadBuffer reader(data->data(), data->size());
#ifdef NEW_SKIA
    val = reader.readPaint();
#else
    reader.readPaint(&val, nullptr);
#endif
    return true;
}
#endif

#ifndef USE_ROSEN_DRAWING
// SkImage
bool RSMarshallingHelper::Marshalling(Parcel& parcel, const sk_sp<SkImage>& val)
{
    if (!val) {
        return parcel.WriteInt32(-1);
    }
    int32_t type = val->isLazyGenerated();
    parcel.WriteInt32(type);
    if (type == 1) {
        ROSEN_LOGD("RSMarshallingHelper::Marshalling SkImage isLazyGenerated");
        SkBinaryWriteBuffer writer;
        writer.writeImage(val.get());
        size_t length = writer.bytesWritten();
        sk_sp<SkData> data = SkData::MakeUninitialized(length);
        writer.writeToMemory(data->writable_data());
        return Marshalling(parcel, data);
    } else {
        SkBitmap bitmap;
#ifdef NEW_SKIA
        auto context = as_IB(val.get())->directContext();
        if (!as_IB(val.get())->getROPixels(context, &bitmap)) {
#else
        if (!as_IB(val.get())->getROPixels(&bitmap)) {
#endif
            ROSEN_LOGE("RSMarshallingHelper::Marshalling SkImage getROPixels failed");
            return false;
        }
        SkPixmap pixmap;
        if (!bitmap.peekPixels(&pixmap)) {
            ROSEN_LOGE("RSMarshallingHelper::Marshalling SkImage peekPixels failed");
            return false;
        }
        size_t rb = pixmap.rowBytes();
        int width = pixmap.width();
        int height = pixmap.height();
        const void* addr = pixmap.addr();
        size_t size = rb * static_cast<size_t>(height);

        parcel.WriteUint32(size);
        if (!WriteToParcel(parcel, addr, size)) {
            ROSEN_LOGE("RSMarshallingHelper::Marshalling SkImage WriteToParcel failed");
            return false;
        }

        parcel.WriteUint32(rb);
        parcel.WriteInt32(width);
        parcel.WriteInt32(height);

        parcel.WriteUint32(pixmap.colorType());
        parcel.WriteUint32(pixmap.alphaType());

        if (pixmap.colorSpace() == nullptr) {
            parcel.WriteUint32(0);
            return true;
        } else {
            auto data = pixmap.colorSpace()->serialize();
            parcel.WriteUint32(data->size());
            if (!WriteToParcel(parcel, data->data(), data->size())) {
                ROSEN_LOGE("RSMarshallingHelper::Marshalling SkImage WriteToParcel colorSpace failed");
                return false;
            }
        }
        return true;
    }
}

static void sk_free_releaseproc(const void* ptr, void*)
{
    MemoryTrack::Instance().RemovePictureRecord(ptr);
    free(const_cast<void*>(ptr));
}

bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, sk_sp<SkImage>& val)
{
    void* addr = nullptr;
    return Unmarshalling(parcel, val, addr);
}

bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, sk_sp<SkImage>& val, void*& imagepixelAddr)
{
    int32_t type = parcel.ReadInt32();
    if (type == -1) {
        val = nullptr;
        return true;
    }
    if (type == 1) {
        sk_sp<SkData> data;
        ROSEN_LOGD("RSMarshallingHelper::Unmarshalling lazy");
        if (!Unmarshalling(parcel, data)) {
            ROSEN_LOGE("failed RSMarshallingHelper::Unmarshalling SkImage");
            return false;
        }
        SkReadBuffer reader(data->data(), data->size());
        val = reader.readImage();
        return val != nullptr;
    } else {
        size_t pixmapSize = parcel.ReadUint32();
        const void* addr = RSMarshallingHelper::ReadFromParcel(parcel, pixmapSize);
        if (addr == nullptr) {
            ROSEN_LOGE("failed RSMarshallingHelper::Unmarshalling SkData addr");
            return false;
        }

        size_t rb = parcel.ReadUint32();
        int width = parcel.ReadInt32();
        int height = parcel.ReadInt32();

        SkColorType colorType = static_cast<SkColorType>(parcel.ReadUint32());
        SkAlphaType alphaType = static_cast<SkAlphaType>(parcel.ReadUint32());
        sk_sp<SkColorSpace> colorSpace;

        size_t size = parcel.ReadUint32();
        if (size == 0) {
            colorSpace = nullptr;
        } else {
            const void* data = RSMarshallingHelper::ReadFromParcel(parcel, size);
            if (data == nullptr) {
                ROSEN_LOGE("failed RSMarshallingHelper::Unmarshalling SkData data");
                return false;
            }
            colorSpace = SkColorSpace::Deserialize(data, size);

            if (size >= MIN_DATA_SIZE && g_useSharedMem) {
                free(const_cast<void*>(data));
            }
        }

        // if pixelmapSize >= MIN_DATA_SIZE(copyFromAshMem). record this memory size
        // use this proc to follow release step
        SkImageInfo imageInfo = SkImageInfo::Make(width, height, colorType, alphaType, colorSpace);
        auto skData =
            (pixmapSize < MIN_DATA_SIZE || (!g_useSharedMem && g_tid == std::this_thread::get_id()))
                ? SkData::MakeWithCopy(addr, pixmapSize)
                : SkData::MakeWithProc(addr, pixmapSize, sk_free_releaseproc, nullptr);
        val = SkImage::MakeRasterData(imageInfo, skData, rb);
        // add to MemoryTrack for memoryManager
        if (pixmapSize >= MIN_DATA_SIZE && g_useSharedMem) {
            MemoryInfo info = { pixmapSize, 0, 0, MEMORY_TYPE::MEM_SKIMAGE };
            MemoryTrack::Instance().AddPictureRecord(addr, info);
            imagepixelAddr = const_cast<void*>(addr);
        }
        return val != nullptr;
    }
}
#else
bool RSMarshallingHelper::Marshalling(Parcel& parcel, const std::shared_ptr<Drawing::Image>& val)
{
    if (!val) {
        return parcel.WriteInt32(-1);
    }
    parcel.WriteInt32(1);
    auto data = val->Serialize();
    return Marshalling(parcel, data);
}

bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, std::shared_ptr<Drawing::Image>& val, void*& imagepixelAddr)
{
    (void)imagepixelAddr;
    int32_t type = parcel.ReadInt32();
    if (type == -1) {
        val = nullptr;
        return true;
    }
    std::shared_ptr<Drawing::Data> data;
    if (!Unmarshalling(parcel, data) || !data) {
        ROSEN_LOGE("failed RSMarshallingHelper::Unmarshalling Drawing::Image");
        return false;
    }
    val = std::make_shared<Drawing::Image>();
    if (!val->Deserialize(data)) {
        ROSEN_LOGE("failed RSMarshallingHelper::Unmarshalling Drawing::Image deserialize");
        val = nullptr;
        return false;
    }
    return true;
}
#endif

#ifndef USE_ROSEN_DRAWING
bool RSMarshallingHelper::SkipSkImage(Parcel& parcel)
{
    int32_t type = parcel.ReadInt32();
    if (type == -1) {
        return true;
    }
    sk_sp<SkData> data;
    if (type == 1) {
        ROSEN_LOGD("RSMarshallingHelper::SkipSkImage lazy");
        return SkipSkData(parcel);
    } else {
        size_t pixmapSize = parcel.ReadUint32();
        if (!SkipFromParcel(parcel, pixmapSize)) {
            ROSEN_LOGE("failed RSMarshallingHelper::SkipSkImage SkData addr");
            return false;
        }

        parcel.ReadUint32();
        parcel.ReadInt32();
        parcel.ReadInt32();
        parcel.ReadUint32();
        parcel.ReadUint32();
        size_t size = parcel.ReadUint32();
        return size == 0 ? true : SkipFromParcel(parcel, size);
    }
}
#else
bool RSMarshallingHelper::SkipImage(Parcel& parcel)
{
    int32_t type = parcel.ReadInt32();
    if (type == -1) {
        return true;
    }
    std::shared_ptr<Drawing::Data> data;
    if (type == 1) {
        ROSEN_LOGD("RSMarshallingHelper::SkipImage lazy");
        return SkipData(parcel);
    } else {
        size_t pixmapSize = parcel.ReadUint32();
        if (!SkipFromParcel(parcel, pixmapSize)) {
            ROSEN_LOGE("failed RSMarshallingHelper::SkipImage Data addr");
            return false;
        }

        parcel.ReadUint32();
        parcel.ReadInt32();
        parcel.ReadInt32();
        parcel.ReadUint32();
        parcel.ReadUint32();
        size_t size = parcel.ReadUint32();
        return size == 0 ? true : SkipFromParcel(parcel, size);
    }
}
#endif

#ifndef USE_ROSEN_DRAWING
// SkPicture
bool RSMarshallingHelper::Marshalling(Parcel& parcel, const sk_sp<SkPicture>& val)
{
    sk_sp<SkData> data;
    if (!val) {
        ROSEN_LOGD("unirender: RSMarshallingHelper::Marshalling SkPicture is nullptr");
        return Marshalling(parcel, data);
    }
    data = val->serialize();
    return Marshalling(parcel, data);
}
bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, sk_sp<SkPicture>& val)
{
    sk_sp<SkData> data;
    if (!Unmarshalling(parcel, data)) {
        ROSEN_LOGE("unirender: failed RSMarshallingHelper::Unmarshalling SkPicture");
        return false;
    }
    if (data == nullptr) {
        val = nullptr;
        return true;
    }
    val = SkPicture::MakeFromData(data->data(), data->size());
    return val != nullptr;
}

// SkVertices
bool RSMarshallingHelper::Marshalling(Parcel& parcel, const sk_sp<SkVertices>& val)
{
    sk_sp<SkData> data;
    if (!val) {
        ROSEN_LOGD("unirender: RSMarshallingHelper::Marshalling SkVertices is nullptr");
        return Marshalling(parcel, data);
    }
#ifdef NEW_SKIA
    SkBinaryWriteBuffer writer;
    SkVerticesPriv info(val->priv());
    info.encode(writer);
    size_t length = writer.bytesWritten();
    data = SkData::MakeUninitialized(length);
    writer.writeToMemory(data->writable_data());
#else
    data = val->encode();
#endif
    return Marshalling(parcel, data);
}
bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, sk_sp<SkVertices>& val)
{
    sk_sp<SkData> data;
    if (!Unmarshalling(parcel, data)) {
        ROSEN_LOGE("unirender: failed RSMarshallingHelper::Unmarshalling SkVertices");
        return false;
    }
    if (data == nullptr) {
        val = nullptr;
        return true;
    }
#ifdef NEW_SKIA
    SkReadBuffer reader(data->data(), data->size());
    val = SkVerticesPriv::Decode(reader);
#else
    val = SkVertices::Decode(data->data(), data->size());
#endif
    return val != nullptr;
}

// SkRect
bool RSMarshallingHelper::Marshalling(Parcel& parcel, const SkRect& rect)
{
    SkBinaryWriteBuffer writer;
    writer.writeRect(rect);
    size_t length = writer.bytesWritten();
    sk_sp<SkData> data = SkData::MakeUninitialized(length);
    writer.writeToMemory(data->writable_data());
    return Marshalling(parcel, data);
}
bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, SkRect& rect)
{
    sk_sp<SkData> data;
    if (!Unmarshalling(parcel, data) || !data) {
        ROSEN_LOGE("unirender: failed RSMarshallingHelper::Unmarshalling SkRect");
        return false;
    }
    SkReadBuffer reader(data->data(), data->size());
    reader.readRect(&rect);
    return true;
}

// SkRegion
bool RSMarshallingHelper::Marshalling(Parcel& parcel, const SkRegion& region)
{
    SkBinaryWriteBuffer writer;
    writer.writeRegion(region);
    size_t length = writer.bytesWritten();
    sk_sp<SkData> data = SkData::MakeUninitialized(length);
    writer.writeToMemory(data->writable_data());
    return Marshalling(parcel, data);
}
bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, SkRegion& region)
{
    sk_sp<SkData> data;
    if (!Unmarshalling(parcel, data) || !data) {
        ROSEN_LOGE("unirender: failed RSMarshallingHelper::Unmarshalling SkRegion");
        return false;
    }
    SkReadBuffer reader(data->data(), data->size());
    reader.readRegion(&region);
    return true;
}

// SkBitmap
bool RSMarshallingHelper::Marshalling(Parcel& parcel, const SkBitmap& val)
{
    size_t rb = val.rowBytes();
    int width = val.width();
    int height = val.height();
    const void* addr = val.pixmap().addr();
    size_t pixmapSize = rb * static_cast<size_t>(height);

    parcel.WriteUint32(pixmapSize);
    if (!WriteToParcel(parcel, addr, pixmapSize)) {
        ROSEN_LOGE("RSMarshallingHelper::Marshalling write SkBitmap addr failed");
        return false;
    }

    parcel.WriteUint32(rb);
    parcel.WriteInt32(width);
    parcel.WriteInt32(height);

    parcel.WriteUint32(val.colorType());
    parcel.WriteUint32(val.alphaType());

    if (val.colorSpace() == nullptr) {
        parcel.WriteUint32(0);
        return true;
    } else {
        auto data = val.colorSpace()->serialize();
        parcel.WriteUint32(data->size());
        if (!WriteToParcel(parcel, data->data(), data->size())) {
            ROSEN_LOGE("RSMarshallingHelper::Marshalling write SkBitmap colorSpace failed");
            return false;
        }
    }
    return true;
}

bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, SkBitmap& val)
{
    size_t pixmapSize = parcel.ReadUint32();
    const void* addr = RSMarshallingHelper::ReadFromParcel(parcel, pixmapSize);
    if (addr == nullptr) {
        ROSEN_LOGE("RSMarshallingHelper::Unmarshalling read SkBitmap addr failed");
        return false;
    }

    size_t rb = parcel.ReadUint32();
    int width = parcel.ReadInt32();
    int height = parcel.ReadInt32();

    SkColorType colorType = static_cast<SkColorType>(parcel.ReadUint32());
    SkAlphaType alphaType = static_cast<SkAlphaType>(parcel.ReadUint32());
    sk_sp<SkColorSpace> colorSpace;

    size_t size = parcel.ReadUint32();
    if (size == 0) {
        colorSpace = nullptr;
    } else {
        const void* data = RSMarshallingHelper::ReadFromParcel(parcel, size);
        if (data == nullptr) {
            ROSEN_LOGE("failed RSMarshallingHelper::Unmarshalling read SkBitmap data failed");
            return false;
        }
        colorSpace = SkColorSpace::Deserialize(data, size);

        if (size >= MIN_DATA_SIZE && g_useSharedMem) {
            free(const_cast<void*>(data));
        }
    }

    SkImageInfo imageInfo = SkImageInfo::Make(width, height, colorType, alphaType, colorSpace);
    auto releaseProc = [](void* addr, void* context) -> void {
        free(const_cast<void*>(addr));
        addr = nullptr;
    };
    val.installPixels(imageInfo, const_cast<void*>(addr), rb, releaseProc, nullptr);
    return true;
}

// SKPath
bool RSMarshallingHelper::Marshalling(Parcel& parcel, const SkPath& val)
{
    SkBinaryWriteBuffer writer;
    writer.writePath(val);
    size_t length = writer.bytesWritten();
    sk_sp<SkData> data = SkData::MakeUninitialized(length);
    writer.writeToMemory(data->writable_data());
    return Marshalling(parcel, data);
}
bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, SkPath& val)
{
    sk_sp<SkData> data;
    if (!Unmarshalling(parcel, data) || !data) {
        ROSEN_LOGE("unirender: failed RSMarshallingHelper::Unmarshalling SKPath");
        return false;
    }
    SkReadBuffer reader(data->data(), data->size());
    reader.readPath(&val);
    return true;
}

// SkFlattenable
bool RSMarshallingHelper::Marshalling(Parcel& parcel, const sk_sp<SkFlattenable>& val)
{
    if (!val) {
        ROSEN_LOGD("unirender: RSMarshallingHelper::Marshalling SkFlattenable is nullptr");
        return parcel.WriteInt32(-1);
    }
    sk_sp<SkData> data = val->serialize();
    return parcel.WriteInt32(val->getFlattenableType()) && Marshalling(parcel, data);
}
bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, sk_sp<SkFlattenable>& val)
{
    int32_t type = parcel.ReadInt32();
    if (type == -1) {
        val = nullptr;
        return true;
    }
    sk_sp<SkData> data;
    if (!Unmarshalling(parcel, data) || !data) {
        ROSEN_LOGE("unirender: failed RSMarshallingHelper::Unmarshalling SkFlattenable");
        return false;
    }
    val = SkFlattenable::Deserialize(static_cast<SkFlattenable::Type>(type), data->data(), data->size());
    return val != nullptr;
}

// SkDrawable
bool RSMarshallingHelper::Marshalling(Parcel& parcel, const sk_sp<SkDrawable>& val)
{
    if (!val) {
        ROSEN_LOGD("unirender: RSMarshallingHelper::Marshalling SkDrawable is nullptr");
    }
    return Marshalling(parcel, sk_sp<SkFlattenable>(val));
}
bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, sk_sp<SkDrawable>& val)
{
    sk_sp<SkFlattenable> flattenablePtr;
    if (!Unmarshalling(parcel, flattenablePtr)) {
        ROSEN_LOGE("unirender: failed RSMarshallingHelper::Unmarshalling SkDrawable");
        return false;
    }
    val = sk_reinterpret_cast<SkDrawable>(flattenablePtr);
    return true;
}

// SkImageFilter
bool RSMarshallingHelper::Marshalling(Parcel& parcel, const sk_sp<SkImageFilter>& val)
{
    if (!val) {
        ROSEN_LOGD("unirender: RSMarshallingHelper::Marshalling SkImageFilter is nullptr");
    }
    return Marshalling(parcel, sk_sp<SkFlattenable>(val));
}
bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, sk_sp<SkImageFilter>& val)
{
    sk_sp<SkFlattenable> flattenablePtr;
    if (!Unmarshalling(parcel, flattenablePtr)) {
        ROSEN_LOGE("unirender: failed RSMarshallingHelper::Unmarshalling SkImageFilter");
        return false;
    }
    val = sk_reinterpret_cast<SkImageFilter>(flattenablePtr);
    return true;
}
#endif

// RSShader
#ifndef USE_ROSEN_DRAWING
bool RSMarshallingHelper::Marshalling(Parcel& parcel, const std::shared_ptr<RSShader>& val)
{
    if (!val) {
        ROSEN_LOGD("unirender: RSMarshallingHelper::Marshalling RSShader is nullptr");
        return parcel.WriteInt32(-1);
    }
    return parcel.WriteInt32(1) && Marshalling(parcel, sk_sp<SkFlattenable>(val->GetSkShader()));
}
bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, std::shared_ptr<RSShader>& val)
{
    if (parcel.ReadInt32() == -1) {
        val = nullptr;
        return true;
    }
    sk_sp<SkFlattenable> flattenablePtr;
    if (!Unmarshalling(parcel, flattenablePtr)) {
        ROSEN_LOGE("unirender: failed RSMarshallingHelper::Unmarshalling RSShader");
        return false;
    }
    auto shaderPtr = sk_reinterpret_cast<SkShader>(flattenablePtr);
    val = RSShader::CreateRSShader(shaderPtr);
    return val != nullptr;
}
#else
bool RSMarshallingHelper::Marshalling(Parcel& parcel, const std::shared_ptr<RSShader>& val)
{
    if (!val || !val->GetDrawingShader()) {
        ROSEN_LOGD("unirender: RSMarshallingHelper::Marshalling RSShader is nullptr");
        return parcel.WriteInt32(-1);
    }
    if (val->GetDrawingShader()->GetDrawingType() != Drawing::DrawingType::RECORDING) {
        ROSEN_LOGD("unirender: RSMarshallingHelper::Marshalling Drawing::ShaderEffect is invalid");
        return parcel.WriteInt32(-1);
    }
    auto recordingShaderEffect = static_cast<Drawing::RecordingShaderEffect*>(val->GetDrawingShader().get());
    auto cmdListData = recordingShaderEffect->GetCmdList()->GetData();
    bool ret = parcel.WriteInt32(cmdListData.second);
    if (cmdListData.second == 0) {
        ROSEN_LOGW("unirender: RSMarshallingHelper::Marshalling RecordingShaderEffectCmdList size is 0");
        return ret;
    }

    ret &= RSMarshallingHelper::WriteToParcel(parcel, cmdListData.first, cmdListData.second);
    if (!ret) {
        ROSEN_LOGE("unirender: failed RSMarshallingHelper::Marshalling Drawing::RecordingShaderEffect");
    }

    return ret;
}

bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, std::shared_ptr<RSShader>& val)
{
    int32_t size = parcel.ReadInt32();
    if (size == -1) {
        val = nullptr;
        return true;
    }
    if (size == 0) {
        ROSEN_LOGW("unirender: RSMarshallingHelper::Unmarshalling Drawing::ShaderEffectCmdList size is 0");
        val = RSShader::CreateRSShader();
        return true;
    }

    const void* data = RSMarshallingHelper::ReadFromParcel(parcel, size);
    if (data == nullptr) {
        ROSEN_LOGE("unirender: failed RSMarshallingHelper::Unmarshalling RSShader");
        return false;
    }
    auto shaderEffectCmdList = Drawing::ShaderEffectCmdList::CreateFromData({ data, size }, true);
    if (shaderEffectCmdList == nullptr) {
        ROSEN_LOGE("unirender: failed RSMarshallingHelper::Unmarshalling RSShader shader effect cmdlist is nullptr");
        return false;
    }
    auto shaderEffect = shaderEffectCmdList->Playback();
    val = RSShader::CreateRSShader(shaderEffect);
    return val != nullptr;
}

// Drawing::Matrix
bool RSMarshallingHelper::Marshalling(Parcel& parcel, const Drawing::Matrix& val)
{
    Drawing::Matrix::Buffer buffer;
    val.GetAll(buffer);
    int32_t size = buffer.size() * sizeof(Drawing::scalar);
    bool ret = parcel.WriteInt32(size);
    ret &= RSMarshallingHelper::WriteToParcel(parcel, buffer.data(), size);
    if (!ret) {
        ROSEN_LOGE("unirender: failed RSMarshallingHelper::Marshalling Drawing::Matrix");
    }
    return ret;
}

bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, Drawing::Matrix& val)
{
    int32_t size = parcel.ReadInt32();
    auto data = static_cast<const Drawing::scalar*>(RSMarshallingHelper::ReadFromParcel(parcel, size));
    if (data == nullptr) {
        ROSEN_LOGE("unirender: failed RSMarshallingHelper::Unmarshalling Drawing::Matrix");
        return false;
    }

    val.SetMatrix(data[Drawing::Matrix::SCALE_X], data[Drawing::Matrix::SKEW_X], data[Drawing::Matrix::TRANS_X],
        data[Drawing::Matrix::SKEW_Y], data[Drawing::Matrix::SCALE_Y], data[Drawing::Matrix::TRANS_Y],
        data[Drawing::Matrix::PERSP_0], data[Drawing::Matrix::PERSP_1], data[Drawing::Matrix::PERSP_2]);
    return true;
}
#endif

bool RSMarshallingHelper::Marshalling(Parcel& parcel, const std::shared_ptr<RSLinearGradientBlurPara>& val)
{
    bool success = Marshalling(parcel, val->blurRadius_);
    success = success && parcel.WriteUint32(static_cast<uint32_t>(val->fractionStops_.size()));
    for (size_t i = 0; i < val->fractionStops_.size(); i++) {
        success = success && Marshalling(parcel, val->fractionStops_[i].first);
        success = success && Marshalling(parcel, val->fractionStops_[i].second);
    }
    success = success && Marshalling(parcel, val->direction_);
    return success;
}

bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, std::shared_ptr<RSLinearGradientBlurPara>& val)
{
    float blurRadius;
    std::vector<std::pair<float, float>> fractionStops;
    GradientDirection direction = GradientDirection::NONE;
    bool success = Unmarshalling(parcel, blurRadius);
    uint32_t fractionStopsSize = parcel.ReadUint32();
    for (size_t i = 0; i < fractionStopsSize; i++) {
        std::pair<float, float> fractionStop;
        float first = 0.0;
        float second = 0.0;
        success = success && Unmarshalling(parcel, first);
        if (!success) {
            return false;
        }
        fractionStop.first = first;
        success = success && Unmarshalling(parcel, second);
        if (!success) {
            return false;
        }
        fractionStop.second = second;
        fractionStops.push_back(fractionStop);
    }
    success = success && Unmarshalling(parcel, direction);
    if (success) {
        val = std::make_shared<RSLinearGradientBlurPara>(blurRadius, fractionStops, direction);
    }
    return success;
}

bool RSMarshallingHelper::Marshalling(Parcel& parcel, const EmitterConfig& val)
{
    bool success = Marshalling(parcel, val.emitRate_);
    success = success && Marshalling(parcel, val.emitShape_);
    success = success && Marshalling(parcel, val.position_.x_);
    success = success && Marshalling(parcel, val.position_.y_);
    success = success && Marshalling(parcel, val.emitSize_.x_);
    success = success && Marshalling(parcel, val.emitSize_.y_);
    success = success && Marshalling(parcel, val.particleCount_);
    success = success && Marshalling(parcel, val.lifeTime_);
    success = success && Marshalling(parcel, val.type_);
    success = success && Marshalling(parcel, val.radius_);
    success = success && Marshalling(parcel, val.image_);
    success = success && Marshalling(parcel, val.imageSize_.x_);
    success = success && Marshalling(parcel, val.imageSize_.y_);

    return success;
}

bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, EmitterConfig& val)
{
    int emitRate = 0;
    ShapeType emitShape = ShapeType::RECT;
    float positionX = 0.f;
    float positionY = 0.f;
    float emitSizeWidth = 0.f;
    float emitSizeHeight = 0.f;
    int particleCount = 0;
    int64_t lifeTime = 0;
    ParticleType particleType = ParticleType::POINTS;
    float radius = 0.f;
    std::shared_ptr<RSImage> image = nullptr;
    float imageWidth = 0.f;
    float imageHeight = 0.f;

    bool success = Unmarshalling(parcel, emitRate);
    success = success && Unmarshalling(parcel, emitShape);
    success = success && Unmarshalling(parcel, positionX);
    success = success && Unmarshalling(parcel, positionY);
    Vector2f position(positionX, positionY);
    success = success && Unmarshalling(parcel, emitSizeWidth);
    success = success && Unmarshalling(parcel, emitSizeHeight);
    Vector2f emitSize(emitSizeWidth, emitSizeHeight);
    success = success && Unmarshalling(parcel, particleCount);
    success = success && Unmarshalling(parcel, lifeTime);
    success = success && Unmarshalling(parcel, particleType);
    success = success && Unmarshalling(parcel, radius);
    Unmarshalling(parcel, image);
    success = success && Unmarshalling(parcel, imageWidth);
    success = success && Unmarshalling(parcel, imageHeight);
    Vector2f imageSize(imageWidth, imageHeight);
    if (success) {
        val = EmitterConfig(
            emitRate, emitShape, position, emitSize, particleCount, lifeTime, particleType, radius, image, imageSize);
    }
    return success;
}

bool RSMarshallingHelper::Marshalling(Parcel& parcel, const ParticleVelocity& val)
{
    return Marshalling(parcel, val.velocityValue_.start_) && Marshalling(parcel, val.velocityValue_.end_) &&
           Marshalling(parcel, val.velocityAngle_.start_) && Marshalling(parcel, val.velocityAngle_.end_);
}

bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, ParticleVelocity& val)
{
    float valueStart = 0.f;
    float valueEnd = 0.f;
    float angleStart = 0.f;
    float angleEnd = 0.f;
    bool success = Unmarshalling(parcel, valueStart) && Unmarshalling(parcel, valueEnd) &&
        Unmarshalling(parcel, angleStart) && Unmarshalling(parcel, angleEnd);
    if (success) {
        Range<float> velocityValue(valueStart, valueEnd);
        Range<float> velocityAngle(angleStart, angleEnd);
        val = ParticleVelocity(velocityValue, velocityAngle);
    }
    return success;
}

bool RSMarshallingHelper::Marshalling(Parcel& parcel, const RenderParticleParaType<float>& val)
{
    bool success = Marshalling(parcel, val.val_.start_) && Marshalling(parcel, val.val_.end_) &&
                   Marshalling(parcel, val.updator_);
    if (val.updator_ == ParticleUpdator::RANDOM) {
        success = success && Marshalling(parcel, val.random_.start_) && Marshalling(parcel, val.random_.end_);
    } else if (val.updator_ == ParticleUpdator::CURVE) {
        success = success && parcel.WriteUint32(static_cast<uint32_t>(val.valChangeOverLife_.size()));
        for (size_t i = 0; i < val.valChangeOverLife_.size(); i++) {
            success = success && Marshalling(parcel, val.valChangeOverLife_[i]->fromValue_);
            success = success && Marshalling(parcel, val.valChangeOverLife_[i]->toValue_);
            success = success && Marshalling(parcel, val.valChangeOverLife_[i]->startMillis_);
            success = success && Marshalling(parcel, val.valChangeOverLife_[i]->endMillis_);
            success = success && val.valChangeOverLife_[i]->interpolator_->Marshalling(parcel);
        }
    }
    return success;
}

bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, RenderParticleParaType<float>& val)
{
    float valueStart = 0.f;
    float valueEnd = 0.f;
    ParticleUpdator updator = ParticleUpdator::NONE;
    float randomStart = 0.f;
    float randomEnd = 0.f;
    std::vector<std::shared_ptr<ChangeInOverLife<float>>> valChangeOverLife;
    bool success =
        Unmarshalling(parcel, valueStart) && Unmarshalling(parcel, valueEnd) && Unmarshalling(parcel, updator);
    if (updator == ParticleUpdator::RANDOM) {
        success = success && Unmarshalling(parcel, randomStart) && Unmarshalling(parcel, randomEnd);
    } else if (updator == ParticleUpdator::CURVE) {
        uint32_t valChangeOverLifeSize = parcel.ReadUint32();
        for (size_t i = 0; i < valChangeOverLifeSize; i++) {
            float fromValue = 0.f;
            float toValue = 0.f;
            int startMillis = 0;
            int endMillis = 0;
            success = success && Unmarshalling(parcel, fromValue);
            success = success && Unmarshalling(parcel, toValue);
            success = success && Unmarshalling(parcel, startMillis);
            success = success && Unmarshalling(parcel, endMillis);
            std::shared_ptr<RSInterpolator> interpolator(RSInterpolator::Unmarshalling(parcel));
            auto change = std::make_shared<ChangeInOverLife<float>>(
                fromValue, toValue, startMillis, endMillis, interpolator);
            valChangeOverLife.push_back(change);
        }
    }
    if (success) {
        Range<float> value(valueStart, valueEnd);
        Range<float> random(randomStart, randomEnd);
        val = RenderParticleParaType<float>(value, updator, random, valChangeOverLife);
    }
    return success;
}

bool RSMarshallingHelper::Marshalling(Parcel& parcel, const RenderParticleColorParaType& val)
{
    bool success = Marshalling(parcel, val.colorVal_.start_) && Marshalling(parcel, val.colorVal_.end_) &&
                   Marshalling(parcel, val.updator_);
    if (val.updator_ == ParticleUpdator::RANDOM) {
        success = success && Marshalling(parcel, val.redRandom_.start_) && Marshalling(parcel, val.redRandom_.end_);
        success =
            success && Marshalling(parcel, val.greenRandom_.start_) && Marshalling(parcel, val.greenRandom_.end_);
        success =
            success && Marshalling(parcel, val.blueRandom_.start_) && Marshalling(parcel, val.blueRandom_.end_);
        success =
            success && Marshalling(parcel, val.alphaRandom_.start_) && Marshalling(parcel, val.alphaRandom_.end_);
    } else if (val.updator_ == ParticleUpdator::CURVE) {
        success = success && parcel.WriteUint32(static_cast<uint32_t>(val.valChangeOverLife_.size()));
        for (size_t i = 0; i < val.valChangeOverLife_.size(); i++) {
            success = success && Marshalling(parcel, val.valChangeOverLife_[i]->fromValue_);
            success = success && Marshalling(parcel, val.valChangeOverLife_[i]->toValue_);
            success = success && Marshalling(parcel, val.valChangeOverLife_[i]->startMillis_);
            success = success && Marshalling(parcel, val.valChangeOverLife_[i]->endMillis_);
            success = success && val.valChangeOverLife_[i]->interpolator_->Marshalling(parcel);
        }
    }
    return success;
}

bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, RenderParticleColorParaType& val)
{
    Color colorValStart = RSColor(0, 0, 0);
    Color colorValEnd = RSColor(0, 0, 0);
    ParticleUpdator updator = ParticleUpdator::NONE;
    float redRandomStart = 0.f;
    float redRandomEnd = 0.f;
    float greenRandomStart = 0.f;
    float greenRandomEnd = 0.f;
    float blueRandomStart = 0.f;
    float blueRandomEnd = 0.f;
    float alphaRandomStart = 0.f;
    float alphaRandomEnd = 0.f;
    std::vector<std::shared_ptr<ChangeInOverLife<Color>>> valChangeOverLife;
    bool success = Unmarshalling(parcel, colorValStart) && Unmarshalling(parcel, colorValEnd) &&
                   Unmarshalling(parcel, updator);
    if (updator == ParticleUpdator::RANDOM) {
        success = success && Unmarshalling(parcel, redRandomStart) && Unmarshalling(parcel, redRandomEnd) &&
                  Unmarshalling(parcel, greenRandomStart) && Unmarshalling(parcel, greenRandomEnd) &&
                  Unmarshalling(parcel, blueRandomStart) && Unmarshalling(parcel, blueRandomEnd) &&
                  Unmarshalling(parcel, alphaRandomStart) && Unmarshalling(parcel, alphaRandomEnd);
    } else if (updator == ParticleUpdator::CURVE) {
        uint32_t valChangeOverLifeSize = parcel.ReadUint32();
        for (size_t i = 0; i < valChangeOverLifeSize; i++) {
            Color fromValue = RSColor(0, 0, 0);
            Color toValue = RSColor(0, 0, 0);
            int startMillis = 0;
            int endMillis = 0;
            success = success && Unmarshalling(parcel, fromValue);
            success = success && Unmarshalling(parcel, toValue);
            success = success && Unmarshalling(parcel, startMillis);
            success = success && Unmarshalling(parcel, endMillis);
            std::shared_ptr<RSInterpolator> interpolator(RSInterpolator::Unmarshalling(parcel));
            auto change =
                std::make_shared<ChangeInOverLife<Color>>(fromValue, toValue, startMillis, endMillis, interpolator);
            valChangeOverLife.push_back(change);
        }
    }
    if (success) {
        Range<Color> colorVal(colorValStart, colorValEnd);
        Range<float> redRandom(redRandomStart, redRandomEnd);
        Range<float> greenRandom(greenRandomStart, greenRandomEnd);
        Range<float> blueRandom(blueRandomStart, blueRandomEnd);
        Range<float> alphaRandom(alphaRandomStart, alphaRandomEnd);
        val = RenderParticleColorParaType(
            colorVal, updator, redRandom, greenRandom, blueRandom, alphaRandom, valChangeOverLife);
    }
    return success;
}

bool RSMarshallingHelper::Marshalling(Parcel& parcel, const std::shared_ptr<ParticleRenderParams>& val)
{
    bool success = Marshalling(parcel, val->emitterConfig_);
    success = success && Marshalling(parcel, val->velocity_);
    success = success && Marshalling(parcel, val->acceleration_.accelerationValue_);
    success = success && Marshalling(parcel, val->acceleration_.accelerationAngle_);
    success = success && Marshalling(parcel, val->color_);
    success = success && Marshalling(parcel, val->opacity_);
    success = success && Marshalling(parcel, val->scale_);
    success = success && Marshalling(parcel, val->spin_);
    return success;
}

bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, std::shared_ptr<ParticleRenderParams>& val)
{
    EmitterConfig emitterConfig;
    ParticleVelocity velocity;
    RenderParticleParaType<float> accelerationValue;
    RenderParticleParaType<float> accelerationAngle;
    RenderParticleColorParaType color;
    RenderParticleParaType<float> opacity;
    RenderParticleParaType<float> scale;
    RenderParticleParaType<float> spin;
    bool success = Unmarshalling(parcel, emitterConfig);
    success = success && Unmarshalling(parcel, velocity);
    success = success && Unmarshalling(parcel, accelerationValue);
    success = success && Unmarshalling(parcel, accelerationAngle);
    RenderParticleAcceleration acceleration = RenderParticleAcceleration(accelerationValue, accelerationAngle);
    success = success && Unmarshalling(parcel, color);
    success = success && Unmarshalling(parcel, opacity);
    success = success && Unmarshalling(parcel, scale);
    success = success && Unmarshalling(parcel, spin);
    if (success) {
        val =
            std::make_shared<ParticleRenderParams>(emitterConfig, velocity, acceleration, color, opacity, scale, spin);
    }
    return success;
}

bool RSMarshallingHelper::Marshalling(Parcel& parcel, const std::vector<std::shared_ptr<ParticleRenderParams>>& val)
{
    bool success = parcel.WriteUint32(static_cast<uint32_t>(val.size()));
    for (size_t i = 0; i < val.size(); i++) {
        success = success && Marshalling(parcel, val[i]);
    }
    return success;
}

bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, std::vector<std::shared_ptr<ParticleRenderParams>>& val)
{
    uint32_t size = parcel.ReadUint32();
    bool success = true;
    std::vector<std::shared_ptr<ParticleRenderParams>> particlesRenderParams;
    for (size_t i = 0; i < size; i++) {
        std::shared_ptr<ParticleRenderParams> particleRenderParams;
        success = success && Unmarshalling(parcel, particleRenderParams);
        particlesRenderParams.push_back(particleRenderParams);
    }
    if (success) {
        val = particlesRenderParams;
    }
    return success;
}

// RSPath
#ifndef USE_ROSEN_DRAWING
bool RSMarshallingHelper::Marshalling(Parcel& parcel, const std::shared_ptr<RSPath>& val)
{
    if (!val) {
        ROSEN_LOGD("unirender: RSMarshallingHelper::Marshalling RSPath is nullptr");
        return parcel.WriteInt32(-1);
    }
    return parcel.WriteInt32(1) && Marshalling(parcel, val->GetSkiaPath());
}

bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, std::shared_ptr<RSPath>& val)
{
    if (parcel.ReadInt32() == -1) {
        val = nullptr;
        return true;
    }
    SkPath path;
    if (!Unmarshalling(parcel, path)) {
        ROSEN_LOGE("unirender: failed RSMarshallingHelper::Unmarshalling RSPath");
        return false;
    }
    val = RSPath::CreateRSPath(path);
    return val != nullptr;
}
#else
bool RSMarshallingHelper::Marshalling(Parcel& parcel, const std::shared_ptr<RSPath>& val)
{
    if (!val) {
        ROSEN_LOGD("unirender: RSMarshallingHelper::Marshalling RSPath is nullptr");
        return parcel.WriteInt32(-1);
    }
    if (val->GetDrawingPath().GetDrawingType() != Drawing::DrawingType::RECORDING) {
        ROSEN_LOGD("unirender: RSMarshallingHelper::Marshalling Drawing::Path is invalid");
        return parcel.WriteInt32(-1);
    }
    auto recordingPath = static_cast<const Drawing::RecordingPath&>(val->GetDrawingPath());
    auto cmdListData = recordingPath.GetCmdList()->GetData();
    bool ret = parcel.WriteInt32(cmdListData.second);
    if (cmdListData.second == 0) {
        ROSEN_LOGW("unirender: RSMarshallingHelper::Marshalling RecordingPathCmdList size is 0");
        return ret;
    }

    ret &= RSMarshallingHelper::WriteToParcel(parcel, cmdListData.first, cmdListData.second);
    if (!ret) {
        ROSEN_LOGE("unirender: failed RSMarshallingHelper::Marshalling Drawing::RecordingPathCmdList");
    }

    return ret;
}

bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, std::shared_ptr<RSPath>& val)
{
    int32_t size = parcel.ReadInt32();
    if (size == -1) {
        val = nullptr;
        return true;
    }
    if (size == 0) {
        ROSEN_LOGW("unirender: RSMarshallingHelper::Unmarshalling Drawing::PathCmdList size is 0");
        val = RSPath::CreateRSPath();
        return true;
    }
    const void* data = RSMarshallingHelper::ReadFromParcel(parcel, size);
    if (data == nullptr) {
        ROSEN_LOGE("unirender: failed RSMarshallingHelper::Unmarshalling RSPath");
        return false;
    }
    auto pathCmdList = Drawing::PathCmdList::CreateFromData({ data, size }, true);
    if (pathCmdList == nullptr) {
        ROSEN_LOGE("unirender: failed RSMarshallingHelper::Unmarshalling RSPath path cmdlist is nullptr");
        return false;
    }
    auto path = pathCmdList->Playback();
    val = RSPath::CreateRSPath(*path);
    return val != nullptr;
}
#endif

// RSMask
bool RSMarshallingHelper::Marshalling(Parcel& parcel, const std::shared_ptr<RSMask>& val)
{
    if (!val) {
        ROSEN_LOGD("unirender: RSMarshallingHelper::Marshalling RSMask is nullptr");
        return parcel.WriteInt32(-1);
    }
    return parcel.WriteInt32(1) && val->Marshalling(parcel);
}
bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, std::shared_ptr<RSMask>& val)
{
    if (parcel.ReadInt32() == -1) {
        val = nullptr;
        return true;
    }
    val.reset(RSMask::Unmarshalling(parcel));
    return val != nullptr;
}

// RSFilter
bool RSMarshallingHelper::Marshalling(Parcel& parcel, const std::shared_ptr<RSFilter>& val)
{
    if (!val) {
        return parcel.WriteInt32(RSFilter::NONE);
    }
    bool success = parcel.WriteInt32(static_cast<int>(val->GetFilterType()));
    switch (val->GetFilterType()) {
        case RSFilter::BLUR: {
            auto blur = std::static_pointer_cast<RSBlurFilter>(val);
            success = success && parcel.WriteFloat(blur->GetBlurRadiusX()) && parcel.WriteFloat(blur->GetBlurRadiusY());
            break;
        }
        case RSFilter::MATERIAL: {
            auto material = std::static_pointer_cast<RSMaterialFilter>(val);
            success = success && parcel.WriteFloat(material->radius_) && parcel.WriteFloat(material->saturation_) &&
                      parcel.WriteFloat(material->brightness_) &&
                      RSMarshallingHelper::Marshalling(parcel, material->maskColor_) &&
                      parcel.WriteInt32(material->colorMode_);
            break;
        }
        case RSFilter::LIGHT_UP_EFFECT: {
            auto lightUp = std::static_pointer_cast<RSLightUpEffectFilter>(val);
            success = success && parcel.WriteFloat(lightUp->lightUpDegree_);
            break;
        }
        default:
            break;
    }
    return success;
}
bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, std::shared_ptr<RSFilter>& val)
{
    int type = 0;
    bool success = parcel.ReadInt32(type);
    switch (static_cast<RSFilter::FilterType>(type)) {
        case RSFilter::BLUR: {
            float blurRadiusX;
            float blurRadiusY;
            success = success && parcel.ReadFloat(blurRadiusX) && parcel.ReadFloat(blurRadiusY);
            if (success) {
                val = RSFilter::CreateBlurFilter(blurRadiusX, blurRadiusY);
            }
            break;
        }
        case RSFilter::MATERIAL: {
            MaterialParam materialParam;
            int colorMode;
            success = success && parcel.ReadFloat(materialParam.radius) && parcel.ReadFloat(materialParam.saturation) &&
                      parcel.ReadFloat(materialParam.brightness) &&
                      RSMarshallingHelper::Unmarshalling(parcel, materialParam.maskColor) &&
                      parcel.ReadInt32(colorMode);
            if (success) {
                val = std::make_shared<RSMaterialFilter>(materialParam, static_cast<BLUR_COLOR_MODE>(colorMode));
            }
            break;
        }
        case RSFilter::LIGHT_UP_EFFECT: {
            float lightUpDegree;
            success = success && parcel.ReadFloat(lightUpDegree);
            if (success) {
                val = RSFilter::CreateLightUpEffectFilter(lightUpDegree);
            }
            break;
        }
        default: {
            val = nullptr;
            break;
        }
    }
    return success;
}

// RSImageBase
bool RSMarshallingHelper::Marshalling(Parcel& parcel, const std::shared_ptr<RSImageBase>& val)
{
    if (!val) {
        ROSEN_LOGD("RSMarshallingHelper::Marshalling RSImageBase is nullptr");
        return parcel.WriteInt32(-1);
    }
    return parcel.WriteInt32(1) && val->Marshalling(parcel);
}
bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, std::shared_ptr<RSImageBase>& val)
{
    if (parcel.ReadInt32() == -1) {
        val = nullptr;
        return true;
    }
    val.reset(RSImageBase::Unmarshalling(parcel));
    return val != nullptr;
}

// RSImage
bool RSMarshallingHelper::Marshalling(Parcel& parcel, const std::shared_ptr<RSImage>& val)
{
    if (!val) {
        ROSEN_LOGD("unirender: RSMarshallingHelper::Marshalling RSImage is nullptr");
        return parcel.WriteInt32(-1);
    }
    return parcel.WriteInt32(1) && val->Marshalling(parcel);
}
bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, std::shared_ptr<RSImage>& val)
{
    if (parcel.ReadInt32() == -1) {
        val = nullptr;
        return true;
    }
    val.reset(RSImage::Unmarshalling(parcel));
    return val != nullptr;
}

// Media::PixelMap
bool RSMarshallingHelper::Marshalling(Parcel& parcel, const std::shared_ptr<Media::PixelMap>& val)
{
    if (!val) {
        return parcel.WriteInt32(-1);
    }
    auto position = parcel.GetWritePosition();
    if (!(parcel.WriteInt32(1) && val->Marshalling(parcel))) {
        ROSEN_LOGE("failed RSMarshallingHelper::Marshalling Media::PixelMap");
        return false;
    }
    // correct pixelmap size recorded in Parcel
    *reinterpret_cast<int32_t*>(parcel.GetData() + position) =
        static_cast<int32_t>(parcel.GetWritePosition() - position - sizeof(int32_t));
    return true;
}

static void CustomFreePixelMap(void* addr, void* context, uint32_t size)
{
    MemoryTrack::Instance().RemovePictureRecord(addr);
}

bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, std::shared_ptr<Media::PixelMap>& val)
{
    if (parcel.ReadInt32() == -1) {
        val = nullptr;
        return true;
    }
    val.reset(Media::PixelMap::Unmarshalling(parcel));
    if (val == nullptr) {
        ROSEN_LOGE("failed RSMarshallingHelper::Unmarshalling Media::PixelMap");
        return false;
    }
    MemoryInfo info = { val->GetByteCount(), 0, 0, MEMORY_TYPE::MEM_PIXELMAP }; // pid is set to 0 temporarily.
    MemoryTrack::Instance().AddPictureRecord(val->GetPixels(), info);
    val->SetFreePixelMapProc(CustomFreePixelMap);
    return true;
}
bool RSMarshallingHelper::SkipPixelMap(Parcel& parcel)
{
    auto size = parcel.ReadInt32();
    if (size != -1) {
        parcel.SkipBytes(size);
    }
    return true;
}

// RectF
bool RSMarshallingHelper::Marshalling(Parcel& parcel, const std::shared_ptr<RectT<float>>& val)
{
    if (!val) {
        ROSEN_LOGD("unirender: RSMarshallingHelper::Marshalling RectF is nullptr");
        return parcel.WriteInt32(-1);
    }
    return parcel.WriteInt32(1) && val->Marshalling(parcel);
}
bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, std::shared_ptr<RectT<float>>& val)
{
    if (parcel.ReadInt32() == -1) {
        val = nullptr;
        return true;
    }
    val.reset(RectT<float>::Unmarshalling(parcel));
    return val != nullptr;
}

// RRect
bool RSMarshallingHelper::Marshalling(Parcel& parcel, const RRectT<float>& val)
{
    return Marshalling(parcel, val.rect_) && Marshalling(parcel, val.radius_[0]) &&
           Marshalling(parcel, val.radius_[1]) && Marshalling(parcel, val.radius_[2]) &&
           Marshalling(parcel, val.radius_[3]);
}
bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, RRectT<float>& val)
{
    return Unmarshalling(parcel, val.rect_) && Unmarshalling(parcel, val.radius_[0]) &&
           Unmarshalling(parcel, val.radius_[1]) && Unmarshalling(parcel, val.radius_[2]) &&
           Unmarshalling(parcel, val.radius_[3]);
}

#ifndef USE_ROSEN_DRAWING
#ifdef NEW_SKIA
// SkPaint
bool RSMarshallingHelper::Marshalling(Parcel& parcel, const SkSamplingOptions& val)
{
    bool success = parcel.WriteBool(val.useCubic) && parcel.WriteFloat(val.cubic.B) && parcel.WriteFloat(val.cubic.C) &&
                   parcel.WriteInt32(static_cast<int32_t>(val.filter)) &&
                   parcel.WriteInt32(static_cast<int32_t>(val.mipmap));
    return success;
}
bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, SkSamplingOptions& val)
{
    bool useCubic = false;
    float b = 0;
    float c = 0;
    int32_t filter = 0;
    int32_t mipmap = 0;
    bool success = parcel.ReadBool(useCubic) && parcel.ReadFloat(b) && parcel.ReadFloat(c) &&
                   parcel.ReadInt32(filter) && parcel.ReadInt32(mipmap);
    if (!success) {
        ROSEN_LOGE("failed RSMarshallingHelper::Unmarshalling SkSamplingOptions");
        return false;
    }
    if (useCubic) {
        SkCubicResampler cubicResampler;
        cubicResampler.B = b;
        cubicResampler.C = c;
        SkSamplingOptions options(cubicResampler);
        val = options;
    } else {
        SkSamplingOptions options(static_cast<SkFilterMode>(filter), static_cast<SkMipmapMode>(mipmap));
        val = options;
    }
    return true;
}
#endif
#endif

#ifdef USE_ROSEN_DRAWING
// Drawing::DrawCmdList
bool RSMarshallingHelper::Marshalling(Parcel& parcel, const std::shared_ptr<Drawing::DrawCmdList>& val)
{
    if (!val) {
        return parcel.WriteInt32(-1);
    }
    auto cmdListData = val->GetData();
    bool ret = parcel.WriteInt32(cmdListData.second);
    parcel.WriteInt32(val->GetWidth());
    parcel.WriteInt32(val->GetHeight());
    if (cmdListData.second == 0) {
        ROSEN_LOGW("unirender: RSMarshallingHelper::Marshalling Drawing::DrawCmdList, size is 0");
        return ret;
    }
    ret &= RSMarshallingHelper::WriteToParcel(parcel, cmdListData.first, cmdListData.second);
    if (!ret) {
        ROSEN_LOGE("unirender: failed RSMarshallingHelper::Marshalling Drawing::DrawCmdList");
        return ret;
    }

    auto imageData = val->GetAllImageData();
    ret &= parcel.WriteInt32(imageData.second);
    if (!ret) {
        ROSEN_LOGE("unirender: failed RSMarshallingHelper::Marshalling Drawing::DrawCmdList image size");
        return ret;
    }
    if (imageData.second > 0) {
        ret &= RSMarshallingHelper::WriteToParcel(parcel, imageData.first, imageData.second);
        if (!ret) {
            ROSEN_LOGE("unirender: failed RSMarshallingHelper::Marshalling Drawing::DrawCmdList image");
            return ret;
        }
    }

    std::vector<std::shared_ptr<Media::PixelMap>> pixelMapVec;
    uint32_t pixelMapSize = val->GetAllPixelMap(pixelMapVec);
    ret &= parcel.WriteUint32(pixelMapSize);
    if (pixelMapSize == 0) {
        return ret;
    }
    for (const auto& pixelMap : pixelMapVec) {
        ret &= RSMarshallingHelper::Marshalling(parcel, pixelMap);
        if (!ret) {
            ROSEN_LOGE("unirender: failed RSMarshallingHelper::Marshalling Drawing::DrawCmdList pixelMap");
            return ret;
        }
    }
    return ret;
}

bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, std::shared_ptr<Drawing::DrawCmdList>& val)
{
    int32_t size = parcel.ReadInt32();
    if (size == -1) {
        val = nullptr;
        return true;
    }
    int32_t width = parcel.ReadInt32();
    int32_t height = parcel.ReadInt32();
    if (size == 0) {
        ROSEN_LOGW("unirender: RSMarshallingHelper::Unmarshalling Drawing::DrawCmdList size is 0");
        val = std::make_shared<Drawing::DrawCmdList>(width, height);
        return true;
    }

    const void* data = RSMarshallingHelper::ReadFromParcel(parcel, size);
    if (data == nullptr) {
        ROSEN_LOGE("unirender: failed RSMarshallingHelper::Unmarshalling Drawing::DrawCmdList");
        return false;
    }

    val = Drawing::DrawCmdList::CreateFromData({ data, size }, true);
    if (val == nullptr) {
        ROSEN_LOGE("unirender: failed RSMarshallingHelper::Unmarshalling Drawing::DrawCmdList is nullptr");
        return false;
    }
    val->SetWidth(width);
    val->SetHeight(height);
    int32_t imageSize = parcel.ReadInt32();
    if (imageSize > 0) {
        const void* imageData = RSMarshallingHelper::ReadFromParcel(parcel, imageSize);
        if (imageData == nullptr) {
            ROSEN_LOGE("unirender: failed RSMarshallingHelper::Unmarshalling Drawing::DrawCmdList image is nullptr");
            return false;
        }
        val->SetUpImageData(imageData, imageSize);
    }

    uint32_t pixelMapSize = parcel.ReadUint32();
    if (pixelMapSize == 0) {
        return true;
    }
    bool ret = true;
    std::vector<std::shared_ptr<Media::PixelMap>> pixelMapVec;
    for (uint32_t i = 0; i < pixelMapSize; i++) {
        std::shared_ptr<Media::PixelMap> pixelMap = std::make_shared<Media::PixelMap>();
        ret &= RSMarshallingHelper::Unmarshalling(parcel, pixelMap);
        if (!ret) {
            ROSEN_LOGE("unirender: failed RSMarshallingHelper::Unmarshalling DrawCmdList pixelMap: %{public}d", i);
            return ret;
        }
        pixelMapVec.emplace_back(pixelMap);
    }
    val->SetupPixelMap(pixelMapVec);
    return ret;
}
#endif

#define MARSHALLING_AND_UNMARSHALLING(TYPE)                                                 \
    bool RSMarshallingHelper::Marshalling(Parcel& parcel, const std::shared_ptr<TYPE>& val) \
    {                                                                                       \
        return parcel.WriteParcelable(val.get());                                           \
    }                                                                                       \
    bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, std::shared_ptr<TYPE>& val)     \
    {                                                                                       \
        val.reset(parcel.ReadParcelable<TYPE>());                                           \
        return val != nullptr;                                                              \
    }
MARSHALLING_AND_UNMARSHALLING(RSRenderTransition)
MARSHALLING_AND_UNMARSHALLING(RSRenderTransitionEffect)
#ifndef USE_ROSEN_DRAWING
MARSHALLING_AND_UNMARSHALLING(DrawCmdList)
#endif
#undef MARSHALLING_AND_UNMARSHALLING

#define MARSHALLING_AND_UNMARSHALLING(TEMPLATE)                                                 \
    bool RSMarshallingHelper::Marshalling(Parcel& parcel, const std::shared_ptr<TEMPLATE>& val) \
    {                                                                                           \
        return parcel.WriteParcelable(val.get());                                               \
    }                                                                                           \
    bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, std::shared_ptr<TEMPLATE>& val)     \
    {                                                                                           \
        val.reset(parcel.ReadParcelable<TEMPLATE>());                                           \
        return val != nullptr;                                                                  \
    }

MARSHALLING_AND_UNMARSHALLING(RSRenderCurveAnimation)
MARSHALLING_AND_UNMARSHALLING(RSRenderParticleAnimation)
MARSHALLING_AND_UNMARSHALLING(RSRenderInterpolatingSpringAnimation)
MARSHALLING_AND_UNMARSHALLING(RSRenderKeyframeAnimation)
MARSHALLING_AND_UNMARSHALLING(RSRenderSpringAnimation)
MARSHALLING_AND_UNMARSHALLING(RSRenderPathAnimation)
#undef MARSHALLING_AND_UNMARSHALLING

bool RSMarshallingHelper::Marshalling(Parcel& parcel, const std::shared_ptr<RSRenderModifier>& val)
{
    return val != nullptr && val->Marshalling(parcel);
}
bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, std::shared_ptr<RSRenderModifier>& val)
{
    val.reset(RSRenderModifier::Unmarshalling(parcel));
    return val != nullptr;
}

#define MARSHALLING_AND_UNMARSHALLING(TEMPLATE)                                                                       \
    template<typename T>                                                                                              \
    bool RSMarshallingHelper::Marshalling(Parcel& parcel, const std::shared_ptr<TEMPLATE<T>>& val)                    \
    {                                                                                                                 \
        return parcel.WriteInt16(static_cast<int16_t>(val->GetPropertyType())) && parcel.WriteUint64(val->GetId()) && \
               Marshalling(parcel, val->Get());                                                                       \
    }                                                                                                                 \
    template<typename T>                                                                                              \
    bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, std::shared_ptr<TEMPLATE<T>>& val)                        \
    {                                                                                                                 \
        PropertyId id = 0;                                                                                            \
        int16_t typeId = 0;                                                                                           \
        if (!parcel.ReadInt16(typeId)) {                                                                              \
            return false;                                                                                             \
        }                                                                                                             \
        RSRenderPropertyType type = static_cast<RSRenderPropertyType>(typeId);                                        \
        if (!parcel.ReadUint64(id)) {                                                                                 \
            return false;                                                                                             \
        }                                                                                                             \
        T value;                                                                                                      \
        if (!Unmarshalling(parcel, value)) {                                                                          \
            return false;                                                                                             \
        }                                                                                                             \
        val.reset(new TEMPLATE<T>(value, id, type));                                                                  \
        return val != nullptr;                                                                                        \
    }

MARSHALLING_AND_UNMARSHALLING(RSRenderProperty)
MARSHALLING_AND_UNMARSHALLING(RSRenderAnimatableProperty)
#undef MARSHALLING_AND_UNMARSHALLING

#define EXPLICIT_INSTANTIATION(TEMPLATE, TYPE)                                                                  \
    template bool RSMarshallingHelper::Marshalling(Parcel& parcel, const std::shared_ptr<TEMPLATE<TYPE>>& val); \
    template bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, std::shared_ptr<TEMPLATE<TYPE>>& val);

#ifndef USE_ROSEN_DRAWING
#ifdef NEW_SKIA
#define BATCH_EXPLICIT_INSTANTIATION(TEMPLATE)                                       \
    EXPLICIT_INSTANTIATION(TEMPLATE, bool)                                           \
    EXPLICIT_INSTANTIATION(TEMPLATE, float)                                          \
    EXPLICIT_INSTANTIATION(TEMPLATE, int)                                            \
    EXPLICIT_INSTANTIATION(TEMPLATE, Color)                                          \
    EXPLICIT_INSTANTIATION(TEMPLATE, Gravity)                                        \
    EXPLICIT_INSTANTIATION(TEMPLATE, GradientDirection)                              \
    EXPLICIT_INSTANTIATION(TEMPLATE, ForegroundColorStrategyType)                    \
    EXPLICIT_INSTANTIATION(TEMPLATE, Matrix3f)                                       \
    EXPLICIT_INSTANTIATION(TEMPLATE, Quaternion)                                     \
    EXPLICIT_INSTANTIATION(TEMPLATE, std::shared_ptr<RSFilter>)                      \
    EXPLICIT_INSTANTIATION(TEMPLATE, std::shared_ptr<RSImage>)                       \
    EXPLICIT_INSTANTIATION(TEMPLATE, std::shared_ptr<RSMask>)                        \
    EXPLICIT_INSTANTIATION(TEMPLATE, std::shared_ptr<RSPath>)                        \
    EXPLICIT_INSTANTIATION(TEMPLATE, std::shared_ptr<RSShader>)                      \
    EXPLICIT_INSTANTIATION(TEMPLATE, std::shared_ptr<RSLinearGradientBlurPara>)      \
    EXPLICIT_INSTANTIATION(TEMPLATE, std::vector<std::shared_ptr<ParticleRenderParams>>)              \
    EXPLICIT_INSTANTIATION(TEMPLATE, std::shared_ptr<ParticleRenderParams>)          \
    EXPLICIT_INSTANTIATION(TEMPLATE, RenderParticleColorParaType)                    \
    EXPLICIT_INSTANTIATION(TEMPLATE, RenderParticleParaType<float>)                  \
    EXPLICIT_INSTANTIATION(TEMPLATE, ParticleVelocity)                               \
    EXPLICIT_INSTANTIATION(TEMPLATE, EmitterConfig)                                  \
    EXPLICIT_INSTANTIATION(TEMPLATE, Vector2f)                                       \
    EXPLICIT_INSTANTIATION(TEMPLATE, Vector4<uint32_t>)                              \
    EXPLICIT_INSTANTIATION(TEMPLATE, Vector4<Color>)                                 \
    EXPLICIT_INSTANTIATION(TEMPLATE, Vector4f)                                       \
    EXPLICIT_INSTANTIATION(TEMPLATE, std::shared_ptr<DrawCmdList>)                   \
    EXPLICIT_INSTANTIATION(TEMPLATE, SkMatrix)                                       \
    EXPLICIT_INSTANTIATION(TEMPLATE, SkM44)
#else
#define BATCH_EXPLICIT_INSTANTIATION(TEMPLATE)                                       \
    EXPLICIT_INSTANTIATION(TEMPLATE, bool)                                           \
    EXPLICIT_INSTANTIATION(TEMPLATE, float)                                          \
    EXPLICIT_INSTANTIATION(TEMPLATE, int)                                            \
    EXPLICIT_INSTANTIATION(TEMPLATE, Color)                                          \
    EXPLICIT_INSTANTIATION(TEMPLATE, Gravity)                                        \
    EXPLICIT_INSTANTIATION(TEMPLATE, GradientDirection)                              \
    EXPLICIT_INSTANTIATION(TEMPLATE, ForegroundColorStrategyType)                    \
    EXPLICIT_INSTANTIATION(TEMPLATE, Matrix3f)                                       \
    EXPLICIT_INSTANTIATION(TEMPLATE, Quaternion)                                     \
    EXPLICIT_INSTANTIATION(TEMPLATE, std::shared_ptr<RSFilter>)                      \
    EXPLICIT_INSTANTIATION(TEMPLATE, std::shared_ptr<RSImage>)                       \
    EXPLICIT_INSTANTIATION(TEMPLATE, std::shared_ptr<RSMask>)                        \
    EXPLICIT_INSTANTIATION(TEMPLATE, std::shared_ptr<RSPath>)                        \
    EXPLICIT_INSTANTIATION(TEMPLATE, std::shared_ptr<RSShader>)                      \
    EXPLICIT_INSTANTIATION(TEMPLATE, std::shared_ptr<RSLinearGradientBlurPara>)      \
    EXPLICIT_INSTANTIATION(TEMPLATE, std::vector<std::shared_ptr<ParticleRenderParams>>)              \
    EXPLICIT_INSTANTIATION(TEMPLATE, std::shared_ptr<ParticleRenderParams>)          \
    EXPLICIT_INSTANTIATION(TEMPLATE, RenderParticleColorParaType)                    \
    EXPLICIT_INSTANTIATION(TEMPLATE, RenderParticleParaType<float>)                  \
    EXPLICIT_INSTANTIATION(TEMPLATE, ParticleVelocity)                               \
    EXPLICIT_INSTANTIATION(TEMPLATE, EmitterConfig)                                  \
    EXPLICIT_INSTANTIATION(TEMPLATE, Vector2f)                                       \
    EXPLICIT_INSTANTIATION(TEMPLATE, Vector4<uint32_t>)                              \
    EXPLICIT_INSTANTIATION(TEMPLATE, Vector4<Color>)                                 \
    EXPLICIT_INSTANTIATION(TEMPLATE, Vector4f)                                       \
    EXPLICIT_INSTANTIATION(TEMPLATE, RRectT<float>)                                  \
    EXPLICIT_INSTANTIATION(TEMPLATE, std::shared_ptr<DrawCmdList>)                   \
    EXPLICIT_INSTANTIATION(TEMPLATE, SkMatrix)
#endif
#else
#define BATCH_EXPLICIT_INSTANTIATION(TEMPLATE)                                     \
    EXPLICIT_INSTANTIATION(TEMPLATE, bool)                                         \
    EXPLICIT_INSTANTIATION(TEMPLATE, float)                                        \
    EXPLICIT_INSTANTIATION(TEMPLATE, int)                                          \
    EXPLICIT_INSTANTIATION(TEMPLATE, Color)                                        \
    EXPLICIT_INSTANTIATION(TEMPLATE, Gravity)                                      \
    EXPLICIT_INSTANTIATION(TEMPLATE, GradientDirection)                            \
    EXPLICIT_INSTANTIATION(TEMPLATE, ForegroundColorStrategyType)                  \
    EXPLICIT_INSTANTIATION(TEMPLATE, Matrix3f)                                     \
    EXPLICIT_INSTANTIATION(TEMPLATE, Quaternion)                                   \
    EXPLICIT_INSTANTIATION(TEMPLATE, std::shared_ptr<RSFilter>)                    \
    EXPLICIT_INSTANTIATION(TEMPLATE, std::shared_ptr<RSImage>)                     \
    EXPLICIT_INSTANTIATION(TEMPLATE, std::shared_ptr<RSMask>)                      \
    EXPLICIT_INSTANTIATION(TEMPLATE, std::shared_ptr<RSPath>)                      \
    EXPLICIT_INSTANTIATION(TEMPLATE, std::shared_ptr<RSShader>)                    \
    EXPLICIT_INSTANTIATION(TEMPLATE, std::shared_ptr<RSLinearGradientBlurPara>)    \
    EXPLICIT_INSTANTIATION(TEMPLATE, std::vector<std::shared_ptr<ParticleRenderParams>>)              \
    EXPLICIT_INSTANTIATION(TEMPLATE, std::shared_ptr<ParticleRenderParams>)        \
    EXPLICIT_INSTANTIATION(TEMPLATE, RenderParticleColorParaType)                  \
    EXPLICIT_INSTANTIATION(TEMPLATE, RenderParticleParaType<float>)                \
    EXPLICIT_INSTANTIATION(TEMPLATE, ParticleVelocity)                             \
    EXPLICIT_INSTANTIATION(TEMPLATE, EmitterConfig)                                \
    EXPLICIT_INSTANTIATION(TEMPLATE, Vector2f)                                     \
    EXPLICIT_INSTANTIATION(TEMPLATE, Vector4<uint32_t>)                            \
    EXPLICIT_INSTANTIATION(TEMPLATE, Vector4<Color>)                               \
    EXPLICIT_INSTANTIATION(TEMPLATE, Vector4f)                                     \
    EXPLICIT_INSTANTIATION(TEMPLATE, std::shared_ptr<Drawing::DrawCmdList>)        \
    EXPLICIT_INSTANTIATION(TEMPLATE, Drawing::Matrix)
#endif

BATCH_EXPLICIT_INSTANTIATION(RSRenderProperty)

#undef EXPLICIT_INSTANTIATION
#undef BATCH_EXPLICIT_INSTANTIATION

#define EXPLICIT_INSTANTIATION(TEMPLATE, TYPE)                                                                  \
    template bool RSMarshallingHelper::Marshalling(Parcel& parcel, const std::shared_ptr<TEMPLATE<TYPE>>& val); \
    template bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, std::shared_ptr<TEMPLATE<TYPE>>& val);

#define BATCH_EXPLICIT_INSTANTIATION(TEMPLATE)                  \
    EXPLICIT_INSTANTIATION(TEMPLATE, float)                     \
    EXPLICIT_INSTANTIATION(TEMPLATE, int)                       \
    EXPLICIT_INSTANTIATION(TEMPLATE, Color)                     \
    EXPLICIT_INSTANTIATION(TEMPLATE, Matrix3f)                  \
    EXPLICIT_INSTANTIATION(TEMPLATE, Quaternion)                \
    EXPLICIT_INSTANTIATION(TEMPLATE, std::shared_ptr<RSFilter>) \
    EXPLICIT_INSTANTIATION(TEMPLATE, Vector2f)                  \
    EXPLICIT_INSTANTIATION(TEMPLATE, Vector4<Color>)            \
    EXPLICIT_INSTANTIATION(TEMPLATE, Vector4f)                  \
    EXPLICIT_INSTANTIATION(TEMPLATE, RRectT<float>)

BATCH_EXPLICIT_INSTANTIATION(RSRenderAnimatableProperty)

#undef EXPLICIT_INSTANTIATION
#undef BATCH_EXPLICIT_INSTANTIATION

bool RSMarshallingHelper::WriteToParcel(Parcel& parcel, const void* data, size_t size)
{
    if (data == nullptr) {
        ROSEN_LOGE("RSMarshallingHelper::WriteToParcel data is nullptr");
        return false;
    }
    if (size > MAX_DATA_SIZE) {
        ROSEN_LOGD("RSMarshallingHelper::WriteToParcel data exceed MAX_DATA_SIZE, size:%{public}zu", size);
    }

    if (!parcel.WriteUint32(size)) {
        return false;
    }
    if (size < MIN_DATA_SIZE || (!g_useSharedMem && g_tid == std::this_thread::get_id())) {
        return parcel.WriteUnpadBuffer(data, size);
    }

    // write to ashmem
    auto ashmemAllocator = AshmemAllocator::CreateAshmemAllocator(size, PROT_READ | PROT_WRITE);
    if (!ashmemAllocator) {
        ROSEN_LOGE("RSMarshallingHelper::WriteToParcel CreateAshmemAllocator fail");
        return false;
    }
    int fd = ashmemAllocator->GetFd();
    if (!(static_cast<MessageParcel*>(&parcel)->WriteFileDescriptor(fd))) {
        ROSEN_LOGE("RSMarshallingHelper::WriteToParcel WriteFileDescriptor error");
        return false;
    }
    if (!ashmemAllocator->WriteToAshmem(data, size)) {
        ROSEN_LOGE("RSMarshallingHelper::WriteToParcel memcpy_s failed");
        return false;
    }
    return true;
}

const void* RSMarshallingHelper::ReadFromParcel(Parcel& parcel, size_t size)
{
    uint32_t bufferSize = parcel.ReadUint32();
    if (static_cast<unsigned int>(bufferSize) != size) {
        ROSEN_LOGE("RSMarshallingHelper::ReadFromParcel size mismatch");
        return nullptr;
    }
    if (static_cast<unsigned int>(bufferSize) < MIN_DATA_SIZE  ||
        (!g_useSharedMem && g_tid == std::this_thread::get_id())) {
        return parcel.ReadUnpadBuffer(size);
    }
    // read from ashmem
    int fd = static_cast<MessageParcel*>(&parcel)->ReadFileDescriptor();
    auto ashmemAllocator = AshmemAllocator::CreateAshmemAllocatorWithFd(fd, size, PROT_READ);
    if (!ashmemAllocator) {
        ROSEN_LOGE("RSMarshallingHelper::ReadFromParcel CreateAshmemAllocator fail");
        return nullptr;
    }
    return ashmemAllocator->CopyFromAshmem(size);
}

bool RSMarshallingHelper::SkipFromParcel(Parcel& parcel, size_t size)
{
    int32_t bufferSize = parcel.ReadInt32();
    if (static_cast<unsigned int>(bufferSize) != size) {
        ROSEN_LOGE("RSMarshallingHelper::SkipFromParcel size mismatch");
        return false;
    }
    if (static_cast<unsigned int>(bufferSize) < MIN_DATA_SIZE ||
        (!g_useSharedMem && g_tid == std::this_thread::get_id())) {
        parcel.SkipBytes(size);
        return true;
    }
    // read from ashmem
    int fd = static_cast<MessageParcel*>(&parcel)->ReadFileDescriptor();
    auto ashmemAllocator = AshmemAllocator::CreateAshmemAllocatorWithFd(fd, size, PROT_READ);
    return ashmemAllocator != nullptr;
}

#ifndef USE_ROSEN_DRAWING
bool RSMarshallingHelper::Marshalling(Parcel& parcel, const std::unique_ptr<OpItem>& val)
{
    return RSMarshallingHelper::Marshalling(parcel, val->GetType()) && val->Marshalling(parcel);
}
bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, std::unique_ptr<OpItem>& val)
{
    val = nullptr;
    RSOpType type;
    if (!RSMarshallingHelper::Unmarshalling(parcel, type)) {
        ROSEN_LOGE("DrawCmdList::Unmarshalling failed");
        return false;
    }
    auto func = DrawCmdList::GetOpUnmarshallingFunc(type);
    if (!func) {
        ROSEN_LOGW("unirender: opItem Unmarshalling func not define, optype = %{public}d", type);
        return false;
    }

    OpItem* item = (*func)(parcel);
    if (!item) {
        ROSEN_LOGE("unirender: failed opItem Unmarshalling, optype = %{public}d", type);
        return false;
    }

    val.reset(item);
    return true;
}
#endif

void RSMarshallingHelper::BeginNoSharedMem(std::thread::id tid)
{
    g_useSharedMem = false;
    g_tid = tid;
}
void RSMarshallingHelper::EndNoSharedMem()
{
    g_useSharedMem = true;
    g_tid.__reset();
}

bool RSMarshallingHelper::GetUseSharedMem()
{
    return g_useSharedMem;
}
} // namespace Rosen
} // namespace OHOS
