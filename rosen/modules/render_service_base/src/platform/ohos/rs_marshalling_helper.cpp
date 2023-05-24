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
#include "recording/recording_shader_effect.h"
#include "recording/recording_path.h"
#endif

#include "animation/rs_render_curve_animation.h"
#include "animation/rs_render_interpolating_spring_animation.h"
#include "animation/rs_render_keyframe_animation.h"
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
#include "render/rs_image.h"
#include "render/rs_image_base.h"
#include "render/rs_light_up_effect_filter.h"
#include "render/rs_material_filter.h"
#include "render/rs_path.h"
#include "render/rs_shader.h"
#include "transaction/rs_ashmem_helper.h"
#ifdef RS_ENABLE_RECORDING
#include "benchmarks/rs_recording_thread.h"
#endif

namespace OHOS {
namespace Rosen {

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
#ifdef RS_ENABLE_RECORDING
    if (static_cast<uint32_t>(size) < MIN_DATA_SIZE ||
        parcel.GetMaxCapacity() == RSRecordingThread::RECORDING_PARCEL_MAX_CAPCITY) {
#else
    if (static_cast<uint32_t>(size) < MIN_DATA_SIZE) {
#endif
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
#ifdef RS_ENABLE_RECORDING
        if (val && (val->size() < MIN_DATA_SIZE ||
            parcel.GetMaxCapacity() == RSRecordingThread::RECORDING_PARCEL_MAX_CAPCITY)) {
#else
        if (val && val->size() < MIN_DATA_SIZE) {
#endif
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
    if (!Unmarshalling(parcel, data)) {
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
        if (!as_IB(val.get())->getROPixels(nullptr, &bitmap)) {
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

#ifdef RS_ENABLE_RECORDING
            if (size >= MIN_DATA_SIZE && parcel.GetMaxCapacity() != RSRecordingThread::RECORDING_PARCEL_MAX_CAPCITY) {
#else
            if (size >= MIN_DATA_SIZE) {
#endif
                free(const_cast<void*>(data));
            }
        }

        // if pixelmapSize >= MIN_DATA_SIZE(copyFromAshMem). record this memory size
        // use this proc to follow release step
        SkImageInfo imageInfo = SkImageInfo::Make(width, height, colorType, alphaType, colorSpace);
#ifdef RS_ENABLE_RECORDING
        auto skData = (pixmapSize < MIN_DATA_SIZE ||
            parcel.GetMaxCapacity() == RSRecordingThread::RECORDING_PARCEL_MAX_CAPCITY)
                ? SkData::MakeWithCopy(addr, pixmapSize)
#else
        auto skData = pixmapSize < MIN_DATA_SIZE ? SkData::MakeWithCopy(addr, pixmapSize)
#endif
                                                : SkData::MakeWithProc(addr, pixmapSize, sk_free_releaseproc, nullptr);
        val = SkImage::MakeRasterData(imageInfo, skData, rb);
        // add to MemoryTrack for memoryManager
#ifdef RS_ENABLE_RECORDING
        if (pixmapSize >= MIN_DATA_SIZE &&
            parcel.GetMaxCapacity() != RSRecordingThread::RECORDING_PARCEL_MAX_CAPCITY) {
#else
        if (pixmapSize >= MIN_DATA_SIZE) {
#endif
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
    auto val = std::make_shared<Drawing::Image>();
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
    if (!Unmarshalling(parcel, data)) {
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
    if (!Unmarshalling(parcel, data)) {
        ROSEN_LOGE("unirender: failed RSMarshallingHelper::Unmarshalling SkRegion");
        return false;
    }
    SkReadBuffer reader(data->data(), data->size());
    reader.readRegion(&region);
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
    if (!Unmarshalling(parcel, data)) {
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
    if (!Unmarshalling(parcel, data)) {
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
    if (!val) {
        ROSEN_LOGD("unirender: RSMarshallingHelper::Marshalling RSShader is nullptr");
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
    auto shaderEffectCmdList = Drawing::ShaderEffectCmdList::CreateFromData({data, size});
    if (shaderEffectCmdList == nullptr) {
        ROSEN_LOGE("unirender: failed RSMarshallingHelper::Unmarshalling RSShader shader effect cmdlist is nullptr");
        return false;
    }
    auto shaderEffect = shaderEffectCmdList->Playback();
    val = RSShader::CreateRSShader(shaderEffect);
    return val != nullptr;
}
#endif

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
    auto pathCmdList = Drawing::PathCmdList::CreateFromData({ data, size });
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
        case RSFilter::LIGHTUPEFFECT: {
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
        case RSFilter::LIGHTUPEFFECT: {
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
    MemoryInfo info = {val->GetByteCount(), 0, 0, MEMORY_TYPE::MEM_PIXELMAP}; // pid is set to 0 temporarily.
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

//RRect
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
    bool success = parcel.WriteBool(val.useCubic) &&
                   parcel.WriteFloat(val.cubic.B) &&
                   parcel.WriteFloat(val.cubic.C) &&
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
    bool success = parcel.ReadBool(useCubic) &&
                   parcel.ReadFloat(b) &&
                   parcel.ReadFloat(c) &&
                   parcel.ReadInt32(filter) &&
                   parcel.ReadInt32(mipmap);
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
    val = Drawing::DrawCmdList::CreateFromData({ data, size });
    if (val != nullptr) {
        val->SetWidth(width);
        val->SetHeight(height);
    }

    return val != nullptr;
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

#define MARSHALLING_AND_UNMARSHALLING(TEMPLATE)                                                    \
    template<typename T>                                                                           \
    bool RSMarshallingHelper::Marshalling(Parcel& parcel, const std::shared_ptr<TEMPLATE<T>>& val) \
    {                                                                                              \
        return parcel.WriteUint64(val->GetId()) && Marshalling(parcel, val->Get());                \
    }                                                                                              \
    template<typename T>                                                                           \
    bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, std::shared_ptr<TEMPLATE<T>>& val)     \
    {                                                                                              \
        PropertyId id = 0;                                                                         \
        if (!parcel.ReadUint64(id)) {                                                              \
            return false;                                                                          \
        }                                                                                          \
        T value;                                                                                   \
        if (!Unmarshalling(parcel, value)) {                                                       \
            return false;                                                                          \
        }                                                                                          \
        val.reset(new TEMPLATE<T>(value, id));                                                     \
        return val != nullptr;                                                                     \
    }

MARSHALLING_AND_UNMARSHALLING(RSRenderProperty)
MARSHALLING_AND_UNMARSHALLING(RSRenderAnimatableProperty)
#undef MARSHALLING_AND_UNMARSHALLING

#define EXPLICIT_INSTANTIATION(TEMPLATE, TYPE)                                                                  \
    template bool RSMarshallingHelper::Marshalling(Parcel& parcel, const std::shared_ptr<TEMPLATE<TYPE>>& val); \
    template bool RSMarshallingHelper::Unmarshalling(Parcel& parcel, std::shared_ptr<TEMPLATE<TYPE>>& val);

#ifndef USE_ROSEN_DRAWING
#ifdef NEW_SKIA
#define BATCH_EXPLICIT_INSTANTIATION(TEMPLATE)                     \
    EXPLICIT_INSTANTIATION(TEMPLATE, bool)                         \
    EXPLICIT_INSTANTIATION(TEMPLATE, float)                        \
    EXPLICIT_INSTANTIATION(TEMPLATE, int)                          \
    EXPLICIT_INSTANTIATION(TEMPLATE, Color)                        \
    EXPLICIT_INSTANTIATION(TEMPLATE, Gravity)                      \
    EXPLICIT_INSTANTIATION(TEMPLATE, ForegroundColorStrategyType)  \
    EXPLICIT_INSTANTIATION(TEMPLATE, Matrix3f)                     \
    EXPLICIT_INSTANTIATION(TEMPLATE, Quaternion)                   \
    EXPLICIT_INSTANTIATION(TEMPLATE, std::shared_ptr<RSFilter>)    \
    EXPLICIT_INSTANTIATION(TEMPLATE, std::shared_ptr<RSImage>)     \
    EXPLICIT_INSTANTIATION(TEMPLATE, std::shared_ptr<RSMask>)      \
    EXPLICIT_INSTANTIATION(TEMPLATE, std::shared_ptr<RSPath>)      \
    EXPLICIT_INSTANTIATION(TEMPLATE, std::shared_ptr<RSShader>)    \
    EXPLICIT_INSTANTIATION(TEMPLATE, Vector2f)                     \
    EXPLICIT_INSTANTIATION(TEMPLATE, Vector4<uint32_t>)            \
    EXPLICIT_INSTANTIATION(TEMPLATE, Vector4<Color>)               \
    EXPLICIT_INSTANTIATION(TEMPLATE, Vector4f)                     \
    EXPLICIT_INSTANTIATION(TEMPLATE, std::shared_ptr<DrawCmdList>) \
    EXPLICIT_INSTANTIATION(TEMPLATE, SkMatrix)                     \
    EXPLICIT_INSTANTIATION(TEMPLATE, SkM44)
#else
#define BATCH_EXPLICIT_INSTANTIATION(TEMPLATE)                     \
    EXPLICIT_INSTANTIATION(TEMPLATE, bool)                         \
    EXPLICIT_INSTANTIATION(TEMPLATE, float)                        \
    EXPLICIT_INSTANTIATION(TEMPLATE, int)                          \
    EXPLICIT_INSTANTIATION(TEMPLATE, Color)                        \
    EXPLICIT_INSTANTIATION(TEMPLATE, Gravity)                      \
    EXPLICIT_INSTANTIATION(TEMPLATE, ForegroundColorStrategyType)  \
    EXPLICIT_INSTANTIATION(TEMPLATE, Matrix3f)                     \
    EXPLICIT_INSTANTIATION(TEMPLATE, Quaternion)                   \
    EXPLICIT_INSTANTIATION(TEMPLATE, std::shared_ptr<RSFilter>)    \
    EXPLICIT_INSTANTIATION(TEMPLATE, std::shared_ptr<RSImage>)     \
    EXPLICIT_INSTANTIATION(TEMPLATE, std::shared_ptr<RSMask>)      \
    EXPLICIT_INSTANTIATION(TEMPLATE, std::shared_ptr<RSPath>)      \
    EXPLICIT_INSTANTIATION(TEMPLATE, std::shared_ptr<RSShader>)    \
    EXPLICIT_INSTANTIATION(TEMPLATE, Vector2f)                     \
    EXPLICIT_INSTANTIATION(TEMPLATE, Vector4<uint32_t>)            \
    EXPLICIT_INSTANTIATION(TEMPLATE, Vector4<Color>)               \
    EXPLICIT_INSTANTIATION(TEMPLATE, Vector4f)                     \
    EXPLICIT_INSTANTIATION(TEMPLATE, RRectT<float>)                \
    EXPLICIT_INSTANTIATION(TEMPLATE, std::shared_ptr<DrawCmdList>) \
    EXPLICIT_INSTANTIATION(TEMPLATE, SkMatrix)
#endif
#else
#define BATCH_EXPLICIT_INSTANTIATION(TEMPLATE)                     \
    EXPLICIT_INSTANTIATION(TEMPLATE, bool)                         \
    EXPLICIT_INSTANTIATION(TEMPLATE, float)                        \
    EXPLICIT_INSTANTIATION(TEMPLATE, int)                          \
    EXPLICIT_INSTANTIATION(TEMPLATE, Color)                        \
    EXPLICIT_INSTANTIATION(TEMPLATE, Gravity)                      \
    EXPLICIT_INSTANTIATION(TEMPLATE, ForegroundColorStrategyType)  \
    EXPLICIT_INSTANTIATION(TEMPLATE, Matrix3f)                     \
    EXPLICIT_INSTANTIATION(TEMPLATE, Quaternion)                   \
    EXPLICIT_INSTANTIATION(TEMPLATE, std::shared_ptr<RSFilter>)    \
    EXPLICIT_INSTANTIATION(TEMPLATE, std::shared_ptr<RSImage>)     \
    EXPLICIT_INSTANTIATION(TEMPLATE, std::shared_ptr<RSMask>)      \
    EXPLICIT_INSTANTIATION(TEMPLATE, std::shared_ptr<RSPath>)      \
    EXPLICIT_INSTANTIATION(TEMPLATE, std::shared_ptr<RSShader>)    \
    EXPLICIT_INSTANTIATION(TEMPLATE, Vector2f)                     \
    EXPLICIT_INSTANTIATION(TEMPLATE, Vector4<uint32_t>)            \
    EXPLICIT_INSTANTIATION(TEMPLATE, Vector4<Color>)               \
    EXPLICIT_INSTANTIATION(TEMPLATE, Vector4f)                     \
    EXPLICIT_INSTANTIATION(TEMPLATE, std::shared_ptr<Drawing::DrawCmdList>) \
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
        ROSEN_LOGD("RSMarshallingHelper::WriteToParcel data exceed MAX_DATA_SIZE, size:%zu", size);
    }

    if (!parcel.WriteUint32(size)) {
        return false;
    }
#ifdef RS_ENABLE_RECORDING
    if (size < MIN_DATA_SIZE || parcel.GetMaxCapacity() == RSRecordingThread::RECORDING_PARCEL_MAX_CAPCITY) {
#else
    if (size < MIN_DATA_SIZE) {
#endif
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
#ifdef RS_ENABLE_RECORDING
    if (static_cast<unsigned int>(bufferSize) < MIN_DATA_SIZE ||
        parcel.GetMaxCapacity() == RSRecordingThread::RECORDING_PARCEL_MAX_CAPCITY) {
#else
    if (static_cast<unsigned int>(bufferSize) < MIN_DATA_SIZE) {
#endif
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
#ifdef RS_ENABLE_RECORDING
    if (static_cast<unsigned int>(bufferSize) < MIN_DATA_SIZE ||
        parcel.GetMaxCapacity() == RSRecordingThread::RECORDING_PARCEL_MAX_CAPCITY) {
#else
    if (static_cast<unsigned int>(bufferSize) < MIN_DATA_SIZE) {
#endif
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
        ROSEN_LOGW("unirender: opItem Unmarshalling func not define, optype = %d", type);
        return false;
    }

    OpItem* item = (*func)(parcel);
    if (!item) {
        ROSEN_LOGE("unirender: failed opItem Unmarshalling, optype = %d", type);
        return false;
    }

    val.reset(item);
    return true;
}
#endif
} // namespace Rosen
} // namespace OHOS
