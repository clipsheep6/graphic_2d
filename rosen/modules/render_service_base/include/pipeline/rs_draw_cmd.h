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

#ifndef RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_DRAW_CMD_H
#define RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_DRAW_CMD_H

#ifdef ROSEN_OHOS
#include <GLES/gl.h>

#include "EGL/egl.h"
#include "EGL/eglext.h"
#include "GLES2/gl2.h"
#include "GLES2/gl2ext.h"
#endif
#ifndef USE_ROSEN_DRAWING
#include "include/core/SkCanvas.h"
#include "include/core/SkDrawable.h"
#include "include/core/SkImage.h"
#include "include/core/SkImageFilter.h"
#include "include/core/SkPaint.h"
#include "include/core/SkPath.h"
#include "include/core/SkPicture.h"
#include "include/core/SkRRect.h"
#include "include/core/SkRect.h"
#include "include/core/SkRegion.h"
#include "include/core/SkTextBlob.h"
#ifdef NEW_SKIA
#include "include/core/SkVertices.h"
#endif
#include "pixel_map.h"
#include "src/core/SkDrawShadowInfo.h"
#ifdef ROSEN_OHOS
#include "surface_buffer.h"
#include "window.h"
#endif

#include "common/rs_common_def.h"
#include "memory/rs_dfx_string.h"
#include "pipeline/rs_draw_cmd_list.h"
#include "pipeline/rs_recording_canvas.h"
#include "property/rs_properties_def.h"
#include "render/rs_image.h"
#include "transaction/rs_marshalling_helper.h"
#include <optional>

#ifdef RS_ENABLE_VK
#include "include/gpu/GrBackendSurface.h"
#include "platform/ohos/backend/native_buffer_utils.h"
#endif

namespace OHOS {
namespace Rosen {
class RSPaintFilterCanvas;
class RSRenderContent;
enum class RSPropertyDrawableSlot : uint8_t;

enum RSOpType : uint16_t {
    OPITEM,
    OPITEM_WITH_PAINT,
    RECT_OPITEM,
    ROUND_RECT_OPITEM,
    IMAGE_WITH_PARM_OPITEM,
    DRRECT_OPITEM,
    OVAL_OPITEM,
    REGION_OPITEM,
    ARC_OPITEM,
    SAVE_OPITEM,
    RESTORE_OPITEM,
    FLUSH_OPITEM,
    MATRIX_OPITEM,
    CLIP_RECT_OPITEM,
    CLIP_RRECT_OPITEM,
    CLIP_REGION_OPITEM,
    TRANSLATE_OPITEM,
    TEXTBLOB_OPITEM,
    BITMAP_OPITEM,
    COLOR_FILTER_BITMAP_OPITEM,
    BITMAP_RECT_OPITEM,
    BITMAP_NINE_OPITEM,
    PIXELMAP_NINE_OPITEM,
    PIXELMAP_OPITEM,
    PIXELMAP_RECT_OPITEM,
    ADAPTIVE_RRECT_OPITEM,
    ADAPTIVE_RRECT_SCALE_OPITEM,
    CLIP_ADAPTIVE_RRECT_OPITEM,
    CLIP_OUTSET_RECT_OPITEM,
    PATH_OPITEM,
    CLIP_PATH_OPITEM,
    PAINT_OPITEM,
    CONCAT_OPITEM,
    SAVE_LAYER_OPITEM,
    DRAWABLE_OPITEM,
    PICTURE_OPITEM,
    POINTS_OPITEM,
    VERTICES_OPITEM,
    SHADOW_REC_OPITEM,
    MULTIPLY_ALPHA_OPITEM,
    SAVE_ALPHA_OPITEM,
    RESTORE_ALPHA_OPITEM,
    SURFACEBUFFER_OPITEM,
    SCALE_OPITEM,
    PROPERTY_DRAWABLE_OPITEM,
    PROPERTY_DRAWABLE_RANGE_OPITEM,
};
namespace {
    std::string GetOpTypeString(RSOpType type)
    {
#define GETOPTYPESTRING(t) case (t): return #t
        switch (type) {
            GETOPTYPESTRING(OPITEM);
            GETOPTYPESTRING(OPITEM_WITH_PAINT);
            GETOPTYPESTRING(RECT_OPITEM);
            GETOPTYPESTRING(ROUND_RECT_OPITEM);
            GETOPTYPESTRING(IMAGE_WITH_PARM_OPITEM);
            GETOPTYPESTRING(DRRECT_OPITEM);
            GETOPTYPESTRING(OVAL_OPITEM);
            GETOPTYPESTRING(REGION_OPITEM);
            GETOPTYPESTRING(ARC_OPITEM);
            GETOPTYPESTRING(SAVE_OPITEM);
            GETOPTYPESTRING(RESTORE_OPITEM);
            GETOPTYPESTRING(FLUSH_OPITEM);
            GETOPTYPESTRING(MATRIX_OPITEM);
            GETOPTYPESTRING(CLIP_RECT_OPITEM);
            GETOPTYPESTRING(CLIP_RRECT_OPITEM);
            GETOPTYPESTRING(CLIP_REGION_OPITEM);
            GETOPTYPESTRING(TRANSLATE_OPITEM);
            GETOPTYPESTRING(TEXTBLOB_OPITEM);
            GETOPTYPESTRING(BITMAP_OPITEM);
            GETOPTYPESTRING(COLOR_FILTER_BITMAP_OPITEM);
            GETOPTYPESTRING(BITMAP_RECT_OPITEM);
            GETOPTYPESTRING(BITMAP_NINE_OPITEM);
            GETOPTYPESTRING(PIXELMAP_NINE_OPITEM);
            GETOPTYPESTRING(PIXELMAP_OPITEM);
            GETOPTYPESTRING(PIXELMAP_RECT_OPITEM);
            GETOPTYPESTRING(ADAPTIVE_RRECT_OPITEM);
            GETOPTYPESTRING(ADAPTIVE_RRECT_SCALE_OPITEM);
            GETOPTYPESTRING(CLIP_ADAPTIVE_RRECT_OPITEM);
            GETOPTYPESTRING(CLIP_OUTSET_RECT_OPITEM);
            GETOPTYPESTRING(PATH_OPITEM);
            GETOPTYPESTRING(CLIP_PATH_OPITEM);
            GETOPTYPESTRING(PAINT_OPITEM);
            GETOPTYPESTRING(CONCAT_OPITEM);
            GETOPTYPESTRING(SAVE_LAYER_OPITEM);
            GETOPTYPESTRING(DRAWABLE_OPITEM);
            GETOPTYPESTRING(PICTURE_OPITEM);
            GETOPTYPESTRING(POINTS_OPITEM);
            GETOPTYPESTRING(VERTICES_OPITEM);
            GETOPTYPESTRING(SHADOW_REC_OPITEM);
            GETOPTYPESTRING(MULTIPLY_ALPHA_OPITEM);
            GETOPTYPESTRING(SAVE_ALPHA_OPITEM);
            GETOPTYPESTRING(RESTORE_ALPHA_OPITEM);
            GETOPTYPESTRING(SURFACEBUFFER_OPITEM);
            GETOPTYPESTRING(SCALE_OPITEM);
            GETOPTYPESTRING(PROPERTY_DRAWABLE_OPITEM);
            GETOPTYPESTRING(PROPERTY_DRAWABLE_RANGE_OPITEM);
            default:
                return "";
        }
    }
#undef GETOPTYPESTRING
}

class RSB_EXPORT OpItemTasks {
public:
    static OpItemTasks& Instance();
    void AddTask(std::function<void()> task);
    void ProcessTask();
private:
    OpItemTasks() = default;
    ~OpItemTasks() = default;
    OpItemTasks(const OpItemTasks&);
    OpItemTasks(const OpItemTasks&&);
    OpItemTasks& operator=(const OpItemTasks&);
    OpItemTasks& operator=(const OpItemTasks&&);

    std::mutex mutex_;
    std::vector<std::function<void()>> tasks_ = {};
};

class OpItem : public MemObject, public Parcelable {
public:
    explicit OpItem(size_t size) : MemObject(size) {}
    virtual ~OpItem() {}

    virtual void Draw(RSPaintFilterCanvas& canvas, const SkRect* rect) const {};
    virtual RSOpType GetType() const = 0;
    virtual std::string GetTypeWithDesc() const = 0;

    virtual std::unique_ptr<OpItem> GenerateCachedOpItem(
        const RSPaintFilterCanvas* canvas = nullptr, const SkRect* rect = nullptr) const
    {
        return nullptr;
    }
    virtual std::optional<SkRect> GetCacheBounds() const
    {
        // not cacheable by default
        return std::nullopt;
    }

    virtual void DumpPicture(DfxString& info) const
    {
        return;
    }

    bool Marshalling(Parcel& parcel) const override
    {
        return true;
    }
    virtual bool IsImageOp() const
    {
        return false;
    }
    virtual void SetNodeId(NodeId id) {}
};

class OpItemWithPaint : public OpItem {
public:
    explicit OpItemWithPaint(size_t size) : OpItem(size) {}
    ~OpItemWithPaint() override = default;

    std::unique_ptr<OpItem> GenerateCachedOpItem(const RSPaintFilterCanvas* canvas, const SkRect* rect) const override;

protected:
    SkPaint paint_;
};

class OpItemWithRSImage : public OpItemWithPaint {
public:
#ifdef NEW_SKIA
    OpItemWithRSImage(std::shared_ptr<RSImageBase> rsImage, const SkSamplingOptions& samplingOptions,
        const SkPaint& paint, size_t size)
        : OpItemWithPaint(size), rsImage_(rsImage), samplingOptions_(samplingOptions)
    {
        paint_ = paint;
    }
    explicit OpItemWithRSImage(const SkSamplingOptions& samplingOptions, size_t size)
        : OpItemWithPaint(size), samplingOptions_(samplingOptions)
    {}
#else
    OpItemWithRSImage(std::shared_ptr<RSImageBase> rsImage, const SkPaint& paint, size_t size)
        : OpItemWithPaint(size), rsImage_(rsImage)
    {
        paint_ = paint;
    }
    explicit OpItemWithRSImage(size_t size) : OpItemWithPaint(size) {}
#endif
    ~OpItemWithRSImage() override = default;
    void Draw(RSPaintFilterCanvas& canvas, const SkRect*) const override;
    void SetNodeId(NodeId id) override;
    bool IsImageOp() const override
    {
        return true;
    }
protected:
    std::shared_ptr<RSImageBase> rsImage_;
#ifdef NEW_SKIA
private:
    SkSamplingOptions samplingOptions_;
#endif
};

class RectOpItem : public OpItemWithPaint {
public:
    RectOpItem(SkRect rect, const SkPaint& paint);
    ~RectOpItem() override = default;
    void Draw(RSPaintFilterCanvas& canvas, const SkRect*) const override;

    std::string GetTypeWithDesc() const override
    {
        std::string desc = "{OpType: " + GetOpTypeString(GetType()) +", Description:{";
        int depth = 1;
        rect_.dump(desc, depth);
        desc += "}, \n";
        return desc;
    }

    RSOpType GetType() const override
    {
        return RSOpType::RECT_OPITEM;
    }

    bool Marshalling(Parcel& parcel) const override;
    [[nodiscard]] static OpItem* Unmarshalling(Parcel& parcel);

private:
    SkRect rect_;
};

class RoundRectOpItem : public OpItemWithPaint {
public:
    RoundRectOpItem(const SkRRect& rrect, const SkPaint& paint);
    ~RoundRectOpItem() override = default;
    void Draw(RSPaintFilterCanvas& canvas, const SkRect*) const override;

    std::string GetTypeWithDesc() const override
    {
        std::string desc = "{OpType: " + GetOpTypeString(GetType()) +", Description:{";
        int depth = 1;
        rrect_.dump(desc, depth);
        desc += "}, \n";
        return desc;
    }

    RSOpType GetType() const override
    {
        return RSOpType::ROUND_RECT_OPITEM;
    }

    bool Marshalling(Parcel& parcel) const override;
    [[nodiscard]] static OpItem* Unmarshalling(Parcel& parcel);

private:
    SkRRect rrect_;
};

class ImageWithParmOpItem : public OpItemWithPaint {
public:
#ifdef NEW_SKIA
    ImageWithParmOpItem(
        const sk_sp<SkImage> img, const sk_sp<SkData> data, const RsImageInfo& rsimageInfo,
        const SkSamplingOptions& samplingOptions, const SkPaint& paint);
    ImageWithParmOpItem(
        const std::shared_ptr<Media::PixelMap>& pixelmap, const RsImageInfo& rsimageInfo,
        const SkSamplingOptions& samplingOptions, const SkPaint& paint);
    ImageWithParmOpItem(const std::shared_ptr<RSImage>& rsImage,
        const SkSamplingOptions& samplingOptions, const SkPaint& paint);
#else
    ImageWithParmOpItem(
        const sk_sp<SkImage> img, const sk_sp<SkData> data, const RsImageInfo& rsimageInfo, const SkPaint& paint);
    ImageWithParmOpItem(
        const std::shared_ptr<Media::PixelMap>& pixelmap, const RsImageInfo& rsimageInfo, const SkPaint& paint);
    ImageWithParmOpItem(const std::shared_ptr<RSImage>& rsImage, const SkPaint& paint);
#endif
    ~ImageWithParmOpItem() override;
    void Draw(RSPaintFilterCanvas& canvas, const SkRect*) const override;

    std::string GetTypeWithDesc() const override
    {
        std::string desc = "{OpType: " + GetOpTypeString(GetType()) +", Description:{";
        if (rsImage_ == nullptr) {
            desc += "\trsImage_ == nullptr";
        } else {
            int depth = 1;
            rsImage_->dump(desc, depth);
        }
        desc += "}, \n";
        return desc;
    }

    RSOpType GetType() const override
    {
        return RSOpType::IMAGE_WITH_PARM_OPITEM;
    }
    bool IsImageOp() const override
    {
        return true;
    }

    void DumpPicture(DfxString& info) const override
    {
        if (!rsImage_) {
            return;
        }
        rsImage_->DumpPicture(info);
    }

    void SetNodeId(NodeId id) override;

    bool Marshalling(Parcel& parcel) const override;
    [[nodiscard]] static OpItem* Unmarshalling(Parcel& parcel);
#if defined(ROSEN_OHOS) && (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
#ifndef USE_ROSEN_DRAWING
    sk_sp<SkImage> GetSkImageFromSurfaceBuffer(SkCanvas& canvas, SurfaceBuffer* surfaceBuffer) const;
#endif
#endif
private:
    std::shared_ptr<RSImage> rsImage_;
#if defined(ROSEN_OHOS) && (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
#ifndef USE_ROSEN_DRAWING
#ifdef RS_ENABLE_GL
    mutable EGLImageKHR eglImage_ = EGL_NO_IMAGE_KHR;
    mutable GLuint texId_ = 0;
#endif
#ifdef RS_ENABLE_VK
    mutable GrBackendTexture backendTexture_ = {};
    mutable NativeBufferUtils::VulkanCleanupHelper* cleanupHelper_ = nullptr;
#endif
    mutable OHNativeWindowBuffer* nativeWindowBuffer_ = nullptr;
    mutable pid_t tid_ = 0;
#endif
#endif
#ifdef NEW_SKIA
    SkSamplingOptions samplingOptions_;
#endif
};

class DRRectOpItem : public OpItemWithPaint {
public:
    DRRectOpItem(const SkRRect& outer, const SkRRect& inner, const SkPaint& paint);
    ~DRRectOpItem() override = default;
    void Draw(RSPaintFilterCanvas& canvas, const SkRect*) const override;

    std::string GetTypeWithDesc() const override
    {
        std::string desc = "{OpType: " + GetOpTypeString(GetType()) +", Description:{";
        int depth = 1;
        desc += "outer_";
        outer_.dump(desc, depth);
        desc += "\ninner_";
        inner_.dump(desc, depth);
        desc += "}, \n";
        return desc;
    }

    RSOpType GetType() const override
    {
        return RSOpType::DRRECT_OPITEM;
    }

    bool Marshalling(Parcel& parcel) const override;
    [[nodiscard]] static OpItem* Unmarshalling(Parcel& parcel);

private:
    SkRRect outer_;
    SkRRect inner_;
};

class OvalOpItem : public OpItemWithPaint {
public:
    OvalOpItem(SkRect rect, const SkPaint& paint);
    ~OvalOpItem() override = default;
    void Draw(RSPaintFilterCanvas& canvas, const SkRect*) const override;

    std::string GetTypeWithDesc() const override
    {
        std::string desc = "{OpType: " + GetOpTypeString(GetType()) +", Description:{";
        int depth = 1;
        rect_.dump(desc, depth);
        desc += "}, \n";
        return desc;
    }

    RSOpType GetType() const override
    {
        return RSOpType::OVAL_OPITEM;
    }

    bool Marshalling(Parcel& parcel) const override;
    [[nodiscard]] static OpItem* Unmarshalling(Parcel& parcel);

private:
    SkRect rect_;
};

class RegionOpItem : public OpItemWithPaint {
public:
    RegionOpItem(SkRegion region, const SkPaint& paint);
    ~RegionOpItem() override = default;
    void Draw(RSPaintFilterCanvas& canvas, const SkRect*) const override;

    std::string GetTypeWithDesc() const override
    {
        std::string desc = "{OpType: " + GetOpTypeString(GetType()) +", Description:{";
        int depth = 1;
        region_.dump(desc, depth);
        desc += "}, \n";
        return desc;
    }

    RSOpType GetType() const override
    {
        return RSOpType::REGION_OPITEM;
    }

    bool Marshalling(Parcel& parcel) const override;
    [[nodiscard]] static OpItem* Unmarshalling(Parcel& parcel);

private:
    SkRegion region_;
};

class ArcOpItem : public OpItemWithPaint {
public:
    ArcOpItem(const SkRect& rect, float startAngle, float sweepAngle, bool useCenter, const SkPaint& paint);
    ~ArcOpItem() override = default;
    void Draw(RSPaintFilterCanvas& canvas, const SkRect*) const override;

    std::string GetTypeWithDesc() const override
    {
        std::string desc = "{OpType: " + GetOpTypeString(GetType()) +", Description:{";
        int depth = 1;
        rect_.dump(desc, depth);
        desc += "\tstartAngle_: " + std::to_string(startAngle_) + "\n";
        desc += "\tsweepAngle_: " + std::to_string(sweepAngle_) + "\n";
        desc += "\tuseCenter_: " + std::to_string(useCenter_) + "\n";
        desc += "}, \n";
        return desc;
    }

    RSOpType GetType() const override
    {
        return RSOpType::ARC_OPITEM;
    }

    bool Marshalling(Parcel& parcel) const override;
    [[nodiscard]] static OpItem* Unmarshalling(Parcel& parcel);

private:
    SkRect rect_;
    float startAngle_;
    float sweepAngle_;
    bool useCenter_;
};

class SaveOpItem : public OpItem {
public:
    SaveOpItem();
    ~SaveOpItem() override = default;
    void Draw(RSPaintFilterCanvas& canvas, const SkRect*) const override;

    std::string GetTypeWithDesc() const override
    {
        std::string desc = "{OpType: " + GetOpTypeString(GetType()) +", Description:{";
        desc += "}, \n";
        return desc;
    }

    RSOpType GetType() const override
    {
        return RSOpType::SAVE_OPITEM;
    }

    [[nodiscard]] static OpItem* Unmarshalling(Parcel& parcel);
};

class RestoreOpItem : public OpItem {
public:
    RestoreOpItem();
    ~RestoreOpItem() override = default;
    void Draw(RSPaintFilterCanvas& canvas, const SkRect*) const override;

    std::string GetTypeWithDesc() const override
    {
        std::string desc = "{OpType: " + GetOpTypeString(GetType()) +", Description:{";
        desc += "}, \n";
        return desc;
    }

    RSOpType GetType() const override
    {
        return RSOpType::RESTORE_OPITEM;
    }

    [[nodiscard]] static OpItem* Unmarshalling(Parcel& parcel);
};

class FlushOpItem : public OpItem {
public:
    FlushOpItem();
    ~FlushOpItem() override = default;
    void Draw(RSPaintFilterCanvas& canvas, const SkRect*) const override;

    std::string GetTypeWithDesc() const override
    {
        std::string desc = "{OpType: " + GetOpTypeString(GetType()) +", Description:{";
        desc += "}, \n";
        return desc;
    }

    RSOpType GetType() const override
    {
        return RSOpType::FLUSH_OPITEM;
    }

    [[nodiscard]] static OpItem* Unmarshalling(Parcel& parcel);
};

class MatrixOpItem : public OpItem {
public:
#ifdef NEW_SKIA
    MatrixOpItem(const SkM44& matrix);
#else
    MatrixOpItem(const SkMatrix& matrix);
#endif
    ~MatrixOpItem() override = default;
    void Draw(RSPaintFilterCanvas& canvas, const SkRect*) const override;

    std::string GetTypeWithDesc() const override
    {
        std::string desc = "{OpType: " + GetOpTypeString(GetType()) +", Description:{";
#ifndef NEW_SKIA
        int depth = 1;
        matrix_.dump(desc, depth);
#endif
        desc += "}, \n";
        return desc;
    }

    RSOpType GetType() const override
    {
        return RSOpType::MATRIX_OPITEM;
    }

    bool Marshalling(Parcel& parcel) const override;
    [[nodiscard]] static OpItem* Unmarshalling(Parcel& parcel);

private:
#ifdef NEW_SKIA
    SkM44 matrix_;
#else
    SkMatrix matrix_;
#endif
};

class ClipRectOpItem : public OpItem {
public:
    ClipRectOpItem(const SkRect& rect, SkClipOp op, bool doAA);
    ~ClipRectOpItem() override = default;
    void Draw(RSPaintFilterCanvas& canvas, const SkRect*) const override;

    std::string GetTypeWithDesc() const override
    {
        std::string desc = "{OpType: " + GetOpTypeString(GetType()) +", Description:{";
        int depth = 1;
        rect_.dump(desc, depth);
        desc += "\tclipOp_: " + std::to_string(static_cast<int>(clipOp_)) + "\n";
        desc += "\tdoAA_: " + std::to_string(doAA_) + "\n";
        desc += "}, \n";
        return desc;
    }

    RSOpType GetType() const override
    {
        return RSOpType::CLIP_RECT_OPITEM;
    }

    bool Marshalling(Parcel& parcel) const override;
    [[nodiscard]] static OpItem* Unmarshalling(Parcel& parcel);

    // functions that are dedicated to driven render [start]
    std::optional<SkRect> GetClipRect() const
    {
        return rect_;
    }
    // functions that are dedicated to driven render [end]

private:
    SkRect rect_;
    SkClipOp clipOp_;
    bool doAA_;
};

class ClipRRectOpItem : public OpItem {
public:
    ClipRRectOpItem(const SkRRect& rrect, SkClipOp op, bool doAA);
    ~ClipRRectOpItem() override = default;
    void Draw(RSPaintFilterCanvas& canvas, const SkRect*) const override;

    std::string GetTypeWithDesc() const override
    {
        std::string desc = "{OpType: " + GetOpTypeString(GetType()) +", Description:{";
        int depth = 1;
        rrect_.dump(desc, depth);
        desc += "\tclipOp_: " + std::to_string(static_cast<int>(clipOp_)) + "\n";
        desc += "\tdoAA_: " + std::to_string(doAA_) + "\n";
        desc += "}, \n";
        return desc;
    }

    RSOpType GetType() const override
    {
        return RSOpType::CLIP_RRECT_OPITEM;
    }

    bool Marshalling(Parcel& parcel) const override;
    [[nodiscard]] static OpItem* Unmarshalling(Parcel& parcel);

private:
    SkRRect rrect_;
    SkClipOp clipOp_;
    bool doAA_;
};

class ClipRegionOpItem : public OpItem {
public:
    ClipRegionOpItem(const SkRegion& region, SkClipOp op);
    ~ClipRegionOpItem() override = default;
    void Draw(RSPaintFilterCanvas& canvas, const SkRect*) const override;

    std::string GetTypeWithDesc() const override
    {
        std::string desc = "{OpType: " + GetOpTypeString(GetType()) +", Description:{";
        int depth = 1;
        region_.dump(desc, depth);
        desc += "\tclipOp_: " + std::to_string(static_cast<int>(clipOp_)) + "\n";
        desc += "}, \n";
        return desc;
    }

    RSOpType GetType() const override
    {
        return RSOpType::CLIP_REGION_OPITEM;
    }

    bool Marshalling(Parcel& parcel) const override;
    [[nodiscard]] static OpItem* Unmarshalling(Parcel& parcel);

private:
    SkRegion region_;
    SkClipOp clipOp_;
};

class TranslateOpItem : public OpItem {
public:
    TranslateOpItem(float distanceX, float distanceY);
    ~TranslateOpItem() override = default;
    void Draw(RSPaintFilterCanvas& canvas, const SkRect*) const override;

    std::string GetTypeWithDesc() const override
    {
        std::string desc = "{OpType: " + GetOpTypeString(GetType()) +", Description:{";
        desc += "\tdistanceX_: " + std::to_string(distanceX_) + "\n";
        desc += "\tdistanceY_: " + std::to_string(distanceY_) + "\n";
        desc += "}, \n";
        return desc;
    }

    RSOpType GetType() const override
    {
        return RSOpType::TRANSLATE_OPITEM;
    }

    bool Marshalling(Parcel& parcel) const override;
    [[nodiscard]] static OpItem* Unmarshalling(Parcel& parcel);

private:
    float distanceX_;
    float distanceY_;
};

class ScaleOpItem : public OpItem {
public:
    ScaleOpItem(float scaleX, float scaleY);
    ~ScaleOpItem() override = default;
    void Draw(RSPaintFilterCanvas& canvas, const SkRect*) const override;

    std::string GetTypeWithDesc() const override
    {
        std::string desc = "{OpType: " + GetOpTypeString(GetType()) +", Description:{";
        desc += "\tscaleX_: " + std::to_string(scaleX_) + "\n";
        desc += "\tscaleY_: " + std::to_string(scaleY_) + "\n";
        desc += "}, \n";
        return desc;
    }

    RSOpType GetType() const override
    {
        return RSOpType::SCALE_OPITEM;
    }

    bool Marshalling(Parcel& parcel) const override;
    [[nodiscard]] static OpItem* Unmarshalling(Parcel& parcel);

private:
    float scaleX_ = 1.0f;
    float scaleY_ = 1.0f;
};

class PropertyDrawableOpItem : public OpItem {
public:
    PropertyDrawableOpItem(const std::shared_ptr<const RSRenderContent>& content, RSPropertyDrawableSlot slot);
    ~PropertyDrawableOpItem() override = default;
    void Draw(RSPaintFilterCanvas& canvas, const SkRect*) const override;

    std::string GetTypeWithDesc() const override
    {
        std::string desc = "{OpType: " + GetOpTypeString(GetType()) +", Description:{";
        desc += "\tslot: " + std::to_string(static_cast<int>(slot_)) + "\n";
        desc += "}, \n";
        return desc;
    }

    RSOpType GetType() const override
    {
        return RSOpType::PROPERTY_DRAWABLE_OPITEM;
    }

private:
    const std::shared_ptr<const RSRenderContent> content_;
    const RSPropertyDrawableSlot slot_;
};

class PropertyDrawableRangeOpItem : public OpItem {
public:
    PropertyDrawableRangeOpItem(const std::shared_ptr<const RSRenderContent>& content, RSPropertyDrawableSlot begin,
        RSPropertyDrawableSlot end);
    ~PropertyDrawableRangeOpItem() override = default;
    void Draw(RSPaintFilterCanvas& canvas, const SkRect*) const override;

    std::string GetTypeWithDesc() const override
    {
        std::string desc = "{OpType: " + GetOpTypeString(GetType()) +", Description:{";
        desc += "\tslots: " + std::to_string(static_cast<int>(begin_)) + " to " +
                std::to_string(static_cast<int>(end_)) + "\n";
        desc += "}, \n";
        return desc;
    }

    RSOpType GetType() const override
    {
        return RSOpType::PROPERTY_DRAWABLE_RANGE_OPITEM;
    }

private:
    const std::shared_ptr<const RSRenderContent> content_;
    const RSPropertyDrawableSlot begin_;
    const RSPropertyDrawableSlot end_;
};

class TextBlobOpItem : public OpItemWithPaint {
public:
    TextBlobOpItem(const sk_sp<SkTextBlob> textBlob, float x, float y, const SkPaint& paint);
    ~TextBlobOpItem() override = default;
    void Draw(RSPaintFilterCanvas& canvas, const SkRect*) const override;
    std::optional<SkRect> GetCacheBounds() const override
    {
        // bounds of textBlob_, with additional offset [x_, y_]. textBlob_ should never be null but we should check.
        return textBlob_ ? std::make_optional<SkRect>(textBlob_->bounds().makeOffset(x_, y_)) : std::nullopt;
    }

    std::string GetTypeWithDesc() const override
    {
        std::string desc = "{OpType: " + GetOpTypeString(GetType()) +", Description:{";
        int depth = 1;
        if (textBlob_ == nullptr) {
            desc += "\ttextBlob_ = nullptr\n";
        } else {
            textBlob_->dump(desc, depth);
        }
        desc += "\tx_: " + std::to_string(x_) + "\n";
        desc += "\ty_: " + std::to_string(y_) + "\n";
        desc += "}, \n";
        return desc;
    }

    RSOpType GetType() const override
    {
        return RSOpType::TEXTBLOB_OPITEM;
    }
    bool Marshalling(Parcel& parcel) const override;
    [[nodiscard]] static OpItem* Unmarshalling(Parcel& parcel);

private:
    sk_sp<SkTextBlob> textBlob_;
    float x_;
    float y_;
};

class BitmapOpItem : public OpItemWithRSImage {
public:
#ifdef NEW_SKIA
    BitmapOpItem(const sk_sp<SkImage> bitmapInfo, float left, float top,
        const SkSamplingOptions& samplingOptions, const SkPaint* paint);
    BitmapOpItem(std::shared_ptr<RSImageBase> rsImage, const SkSamplingOptions& samplingOptions, const SkPaint& paint);
#else
    BitmapOpItem(const sk_sp<SkImage> bitmapInfo, float left, float top, const SkPaint* paint);
    BitmapOpItem(std::shared_ptr<RSImageBase> rsImage, const SkPaint& paint);
#endif
    ~BitmapOpItem() override = default;

    std::string GetTypeWithDesc() const override
    {
        std::string desc = "{OpType: " + GetOpTypeString(GetType()) +", Description:{";
        desc += "}, \n";
        return desc;
    }

    RSOpType GetType() const override
    {
        return RSOpType::BITMAP_OPITEM;
    }

    bool Marshalling(Parcel& parcel) const override;
    [[nodiscard]] static OpItem* Unmarshalling(Parcel& parcel);

#ifdef NEW_SKIA
private:
    SkSamplingOptions samplingOptions_;
#endif
};

class ColorFilterBitmapOpItem : public BitmapOpItem {
public:
#ifdef NEW_SKIA
    ColorFilterBitmapOpItem(const sk_sp<SkImage> bitmapInfo, float left, float top,
        const SkSamplingOptions& samplingOptions, const SkPaint* paint);
    ColorFilterBitmapOpItem(std::shared_ptr<RSImageBase> rsImage,
        const SkSamplingOptions& samplingOptions, const SkPaint& paint);
#else
    ColorFilterBitmapOpItem(const sk_sp<SkImage> bitmapInfo, float left, float top, const SkPaint* paint);
    ColorFilterBitmapOpItem(std::shared_ptr<RSImageBase> rsImage, const SkPaint& paint);
#endif
    ~ColorFilterBitmapOpItem() override = default;

    std::string GetTypeWithDesc() const override
    {
        std::string desc = "{OpType: " + GetOpTypeString(GetType()) +", Description:{";
        desc += "}, \n";
        return desc;
    }

    RSOpType GetType() const override
    {
        return RSOpType::COLOR_FILTER_BITMAP_OPITEM;
    }

    void Draw(RSPaintFilterCanvas& canvas, const SkRect*) const override;

    bool Marshalling(Parcel& parcel) const override;
    [[nodiscard]] static OpItem* Unmarshalling(Parcel& parcel);
};

class BitmapRectOpItem : public OpItemWithRSImage {
public:
#ifdef NEW_SKIA
    BitmapRectOpItem(const sk_sp<SkImage> bitmapInfo, const SkRect* rectSrc, const SkRect& rectDst,
        const SkSamplingOptions& samplingOptions, const SkPaint* paint, SkCanvas::SrcRectConstraint constraint);
    BitmapRectOpItem(std::shared_ptr<RSImageBase> rsImage, const SkSamplingOptions& samplingOptions,
        const SkPaint& paint, SkCanvas::SrcRectConstraint constraint);
#else
    BitmapRectOpItem(
        const sk_sp<SkImage> bitmapInfo, const SkRect* rectSrc, const SkRect& rectDst, const SkPaint* paint);
    BitmapRectOpItem(std::shared_ptr<RSImageBase> rsImage, const SkPaint& paint);
#endif
    ~BitmapRectOpItem() override = default;

    std::string GetTypeWithDesc() const override
    {
        std::string desc = "{OpType: " + GetOpTypeString(GetType()) +", Description:{";
        desc += "}, \n";
        return desc;
    }

    RSOpType GetType() const override
    {
        return RSOpType::BITMAP_RECT_OPITEM;
    }

    bool Marshalling(Parcel& parcel) const override;
    [[nodiscard]] static OpItem* Unmarshalling(Parcel& parcel);

#ifdef NEW_SKIA
private:
    SkSamplingOptions samplingOptions_;
    SkCanvas::SrcRectConstraint constraint_;
#endif
};

class PixelMapOpItem : public OpItemWithRSImage {
public:
#ifdef NEW_SKIA
    PixelMapOpItem(const std::shared_ptr<Media::PixelMap>& pixelmap, float left, float top,
        const SkSamplingOptions& samplingOptions, const SkPaint* paint);
    PixelMapOpItem(std::shared_ptr<RSImageBase> rsImage, const SkSamplingOptions& samplingOptions,
        const SkPaint& paint);
#else
    PixelMapOpItem(const std::shared_ptr<Media::PixelMap>& pixelmap, float left, float top, const SkPaint* paint);
    PixelMapOpItem(std::shared_ptr<RSImageBase> rsImage, const SkPaint& paint);
#endif
    ~PixelMapOpItem() override = default;

    std::string GetTypeWithDesc() const override
    {
        std::string desc = "{OpType: " + GetOpTypeString(GetType()) +", Description:{";
        desc += "}, \n";
        return desc;
    }

    RSOpType GetType() const override
    {
        return RSOpType::PIXELMAP_OPITEM;
    }

    bool Marshalling(Parcel& parcel) const override;
    [[nodiscard]] static OpItem* Unmarshalling(Parcel& parcel);

#ifdef NEW_SKIA
private:
    SkSamplingOptions samplingOptions_;
#endif
};

class PixelMapRectOpItem : public OpItemWithRSImage {
public:
#ifdef NEW_SKIA
    PixelMapRectOpItem(
        const std::shared_ptr<Media::PixelMap>& pixelmap, const SkRect& src, const SkRect& dst,
        const SkSamplingOptions& samplingOptions, const SkPaint* paint, SkCanvas::SrcRectConstraint constraint);
    PixelMapRectOpItem(std::shared_ptr<RSImageBase> rsImage,  const SkSamplingOptions& samplingOptions,
    const SkPaint& paint, SkCanvas::SrcRectConstraint constraint);
#else
    PixelMapRectOpItem(
        const std::shared_ptr<Media::PixelMap>& pixelmap, const SkRect& src, const SkRect& dst, const SkPaint* paint);
    PixelMapRectOpItem(std::shared_ptr<RSImageBase> rsImage, const SkPaint& paint);
#endif
    ~PixelMapRectOpItem() override = default;

    std::string GetTypeWithDesc() const override
    {
        std::string desc = "{OpType: " + GetOpTypeString(GetType()) +", Description:{";
        desc += "}, \n";
        return desc;
    }

    RSOpType GetType() const override
    {
        return RSOpType::PIXELMAP_RECT_OPITEM;
    }

    bool Marshalling(Parcel& parcel) const override;
    [[nodiscard]] static OpItem* Unmarshalling(Parcel& parcel);

#ifdef NEW_SKIA
private:
    SkSamplingOptions samplingOptions_;
    SkCanvas::SrcRectConstraint constraint_;
#endif
};

class BitmapNineOpItem : public OpItemWithPaint {
public:
#ifdef NEW_SKIA
    BitmapNineOpItem(const sk_sp<SkImage> bitmapInfo, const SkIRect& center, const SkRect& rectDst,
        SkFilterMode filter, const SkPaint* paint);
#else
    BitmapNineOpItem(
        const sk_sp<SkImage> bitmapInfo, const SkIRect& center, const SkRect& rectDst, const SkPaint* paint);
#endif
    ~BitmapNineOpItem() override = default;
    void Draw(RSPaintFilterCanvas& canvas, const SkRect*) const override;

    std::string GetTypeWithDesc() const override
    {
        std::string desc = "{OpType: " + GetOpTypeString(GetType()) +", Description:{";
        int depth = 1;
        center_.dump(desc, depth);
        rectDst_.dump(desc, depth);
        if (bitmapInfo_ == nullptr) {
            desc += "\tbitmapInfo_ = nullptr\n";
        } else {
            int depth = 1;
            bitmapInfo_->dump(desc, depth);
        }
        desc += "}, \n";
        return desc;
    }

    RSOpType GetType() const override
    {
        return RSOpType::BITMAP_NINE_OPITEM;
    }

    bool Marshalling(Parcel& parcel) const override;
    [[nodiscard]] static OpItem* Unmarshalling(Parcel& parcel);

private:
    SkIRect center_;
    SkRect rectDst_;
    sk_sp<SkImage> bitmapInfo_;
#ifdef NEW_SKIA
    SkFilterMode filter_;
#endif
};

class PixelmapNineOpItem : public OpItemWithPaint {
public:
    PixelmapNineOpItem(const std::shared_ptr<Media::PixelMap>& pixelmap, const SkIRect& center,
        const SkRect& rectDst, const SkFilterMode filter, const SkPaint* paint);
    PixelmapNineOpItem(const std::shared_ptr<RSImageBase> rsImage, const SkIRect& center, const SkRect& rectDst,
        const SkFilterMode filter, const SkPaint* paint);
    ~PixelmapNineOpItem() override = default;
    void Draw(RSPaintFilterCanvas& canvas, const SkRect*) const override;
    
    std::string GetTypeWithDesc() const override
    {
        std::string desc = "{OpType: " + GetOpTypeString(GetType()) +", Description:{";
        desc += "}, \n";
        return desc;
    }

    RSOpType GetType() const override
    {
        return RSOpType::PIXELMAP_NINE_OPITEM;
    }

    bool Marshalling(Parcel& parcel) const override;
    [[nodiscard]] static OpItem* Unmarshalling(Parcel& parcel);

private:
    SkIRect center_;
    SkRect rectDst_;
    SkFilterMode filter_;
    std::shared_ptr<RSImageBase> rsImage_;
};

class AdaptiveRRectOpItem : public OpItemWithPaint {
public:
    AdaptiveRRectOpItem(float radius, const SkPaint& paint);
    ~AdaptiveRRectOpItem() override = default;
    void Draw(RSPaintFilterCanvas& canvas, const SkRect*) const override;

    std::string GetTypeWithDesc() const override
    {
        std::string desc = "{OpType: " + GetOpTypeString(GetType()) +", Description:{";
        desc += "\tradius_: " + std::to_string(radius_);
        desc += "\tpaint_: Omit";
        desc += "}, \n";
        return desc;
    }

    RSOpType GetType() const override
    {
        return RSOpType::ADAPTIVE_RRECT_OPITEM;
    }

    bool Marshalling(Parcel& parcel) const override;
    [[nodiscard]] static OpItem* Unmarshalling(Parcel& parcel);

private:
    float radius_;
    SkPaint paint_;
};

class AdaptiveRRectScaleOpItem : public OpItemWithPaint {
public:
    AdaptiveRRectScaleOpItem(float radiusRatio, const SkPaint& paint);
    ~AdaptiveRRectScaleOpItem() override = default;
    void Draw(RSPaintFilterCanvas& canvas, const SkRect*) const override;

    std::string GetTypeWithDesc() const override
    {
        std::string desc = "{OpType: " + GetOpTypeString(GetType()) +", Description:{";
        desc += "\tradiusRatio_: " + std::to_string(radiusRatio_) + "\n";
        desc += "\tpaint_: Omit\n";
        desc += "}, \n";
        return desc;
    }

    RSOpType GetType() const override
    {
        return RSOpType::ADAPTIVE_RRECT_SCALE_OPITEM;
    }

    bool Marshalling(Parcel& parcel) const override;
    [[nodiscard]] static OpItem* Unmarshalling(Parcel& parcel);

private:
    float radiusRatio_;
    SkPaint paint_;
};

class ClipAdaptiveRRectOpItem : public OpItem {
public:
    ClipAdaptiveRRectOpItem(const SkVector radius[]);
    ~ClipAdaptiveRRectOpItem() override = default;
    void Draw(RSPaintFilterCanvas& canvas, const SkRect*) const override;

    std::string GetTypeWithDesc() const override
    {
        std::string desc = "{OpType: " + GetOpTypeString(GetType()) +", Description:{";
        desc += "\tradius_:{";
        int radiusSize = 4;
        for (int i = 0; i < radiusSize; i++) {
            int depth = 2;
            radius_[i].dump(desc, depth);
        }
        desc += "}}, \n";
        return desc;
    }

    RSOpType GetType() const override
    {
        return RSOpType::CLIP_ADAPTIVE_RRECT_OPITEM;
    }

    bool Marshalling(Parcel& parcel) const override;
    [[nodiscard]] static OpItem* Unmarshalling(Parcel& parcel);

private:
    SkVector radius_[4];
};

class ClipOutsetRectOpItem : public OpItem {
public:
    ClipOutsetRectOpItem(float dx, float dy);
    ~ClipOutsetRectOpItem() override = default;
    void Draw(RSPaintFilterCanvas& canvas, const SkRect*) const override;

    std::string GetTypeWithDesc() const override
    {
        std::string desc = "{OpType: " + GetOpTypeString(GetType()) +", Description:{";
        desc += "\tdx_: " + std::to_string(dx_) + "\n";
        desc += "\tdy_: " + std::to_string(dy_) + "\n";
        desc += "}, \n";
        return desc;
    }

    RSOpType GetType() const override
    {
        return RSOpType::CLIP_OUTSET_RECT_OPITEM;
    }

    bool Marshalling(Parcel& parcel) const override;
    [[nodiscard]] static OpItem* Unmarshalling(Parcel& parcel);

private:
    float dx_;
    float dy_;
};

class PathOpItem : public OpItemWithPaint {
public:
    PathOpItem(const SkPath& path, const SkPaint& paint);
    ~PathOpItem() override = default;
    void Draw(RSPaintFilterCanvas& canvas, const SkRect*) const override;

    std::string GetTypeWithDesc() const override
    {
        std::string desc = "{OpType: " + GetOpTypeString(GetType()) +", Description:{";
        int depth = 1;
        path_.dump(desc, depth);
        desc += "}, \n";
        return desc;
    }

    RSOpType GetType() const override
    {
        return RSOpType::PATH_OPITEM;
    }

    bool Marshalling(Parcel& parcel) const override;
    [[nodiscard]] static OpItem* Unmarshalling(Parcel& parcel);

private:
    SkPath path_;
};

class ClipPathOpItem : public OpItem {
public:
    ClipPathOpItem(const SkPath& path, SkClipOp clipOp, bool doAA);
    ~ClipPathOpItem() override = default;
    void Draw(RSPaintFilterCanvas& canvas, const SkRect*) const override;

    std::string GetTypeWithDesc() const override
    {
        std::string desc = "{OpType: " + GetOpTypeString(GetType()) +", Description:{";
        int depth = 1;
        path_.dump(desc, depth);
        desc += "\tclipOp_: " + std::to_string(static_cast<int>(clipOp_)) + "\n";
        desc += "\tdoAA_: " + std::to_string(doAA_) + "\n";
        desc += "}, \n";
        return desc;
    }

    RSOpType GetType() const override
    {
        return RSOpType::CLIP_PATH_OPITEM;
    }

    bool Marshalling(Parcel& parcel) const override;
    [[nodiscard]] static OpItem* Unmarshalling(Parcel& parcel);

private:
    SkPath path_;
    SkClipOp clipOp_;
    bool doAA_;
};

class PaintOpItem : public OpItemWithPaint {
public:
    PaintOpItem(const SkPaint& paint);
    ~PaintOpItem() override = default;
    void Draw(RSPaintFilterCanvas& canvas, const SkRect*) const override;

    std::string GetTypeWithDesc() const override
    {
        std::string desc = "{OpType: " + GetOpTypeString(GetType()) +", Description:{";
        desc += "}, \n";
        return desc;
    }

    RSOpType GetType() const override
    {
        return RSOpType::PAINT_OPITEM;
    }

    bool Marshalling(Parcel& parcel) const override;
    [[nodiscard]] static OpItem* Unmarshalling(Parcel& parcel);
};

class ConcatOpItem : public OpItem {
public:
#ifdef NEW_SKIA
    ConcatOpItem(const SkM44& matrix);
#else
    ConcatOpItem(const SkMatrix& matrix);
#endif
    ~ConcatOpItem() override = default;
    void Draw(RSPaintFilterCanvas& canvas, const SkRect*) const override;

    std::string GetTypeWithDesc() const override
    {
        std::string desc = "{OpType: " + GetOpTypeString(GetType()) +", Description:{";
#ifndef NEW_SKIA
        int depth = 1;
        matrix_.dump(desc, depth);
#endif
        desc += "}, \n";
        return desc;
    }

    RSOpType GetType() const override
    {
        return RSOpType::CONCAT_OPITEM;
    }

    bool Marshalling(Parcel& parcel) const override;
    [[nodiscard]] static OpItem* Unmarshalling(Parcel& parcel);

private:
#ifdef NEW_SKIA
    SkM44 matrix_;
#else
    SkMatrix matrix_;
#endif
};

class SaveLayerOpItem : public OpItemWithPaint {
public:
    SaveLayerOpItem(const SkCanvas::SaveLayerRec& rec);
    ~SaveLayerOpItem() override = default;
    void Draw(RSPaintFilterCanvas& canvas, const SkRect*) const override;

    std::string GetTypeWithDesc() const override
    {
        std::string desc = "{OpType: " + GetOpTypeString(GetType()) +", Description:{";
        int depth = 1;
        if (rectPtr_ == nullptr) {
            desc += "\trectPtr_ = nullptr\n";
        } else {
            rectPtr_->dump(desc, depth);
        }
        rect_.dump(desc, depth);
        desc += "\tbackdrop_:Omit\n";
#ifndef NEW_SKIA
        if (mask_ == nullptr) {
            desc += "\tmask_ = nullptr\n";
        } else {
            mask_->dump(desc, depth);
        }
        matrix_.dump(desc, depth);
#endif
        desc += "\tflags_:" + std::to_string(static_cast<int>(flags_)) + "\n";
        desc += "}, \n";
        return desc;
    }

    RSOpType GetType() const override
    {
        return RSOpType::SAVE_LAYER_OPITEM;
    }

    bool Marshalling(Parcel& parcel) const override;
    [[nodiscard]] static OpItem* Unmarshalling(Parcel& parcel);

private:
    SkRect* rectPtr_ = nullptr;
    SkRect rect_ = SkRect::MakeEmpty();
    sk_sp<SkImageFilter> backdrop_;
    SkCanvas::SaveLayerFlags flags_;
#ifndef NEW_SKIA
    sk_sp<SkImage> mask_;
    SkMatrix matrix_;
#endif
};

class DrawableOpItem : public OpItem {
public:
    DrawableOpItem(SkDrawable* drawable, const SkMatrix* matrix);
    ~DrawableOpItem() override = default;
    void Draw(RSPaintFilterCanvas& canvas, const SkRect*) const override;

    std::string GetTypeWithDesc() const override
    {
        std::string desc = "{OpType: " + GetOpTypeString(GetType()) +", Description:{";
        int depth = 1;
        if (drawable_ == nullptr) {
            desc += "\tdrawable_ == nullptr\n";
        } else {
            desc += "\t drawable_:" + std::to_string(drawable_->getGenerationID()) + "\n";
        }
        matrix_.dump(desc, depth);
        desc += "}, \n";
        return desc;
    }

    RSOpType GetType() const override
    {
        return RSOpType::DRAWABLE_OPITEM;
    }

    bool Marshalling(Parcel& parcel) const override;
    [[nodiscard]] static OpItem* Unmarshalling(Parcel& parcel);

private:
    sk_sp<SkDrawable> drawable_;
    SkMatrix matrix_ = SkMatrix::I();
};

class PictureOpItem : public OpItemWithPaint {
public:
    PictureOpItem(const sk_sp<SkPicture> picture, const SkMatrix* matrix, const SkPaint* paint);
    ~PictureOpItem() override = default;
    void Draw(RSPaintFilterCanvas& canvas, const SkRect*) const override;

    std::string GetTypeWithDesc() const override
    {
        std::string desc = "{OpType: " + GetOpTypeString(GetType()) +", Description:{";
        int depth = 1;
        desc += "\tpicture_:Omit\n";
        matrix_.dump(desc, depth);
        desc += "}, \n";
        return desc;
    }

    RSOpType GetType() const override
    {
        return RSOpType::PICTURE_OPITEM;
    }

    bool Marshalling(Parcel& parcel) const override;
    [[nodiscard]] static OpItem* Unmarshalling(Parcel& parcel);

private:
    sk_sp<SkPicture> picture_ { nullptr };
    SkMatrix matrix_;
};

class PointsOpItem : public OpItemWithPaint {
public:
    PointsOpItem(SkCanvas::PointMode mode, int count, const SkPoint processedPoints[], const SkPaint& paint);
    ~PointsOpItem() override
    {
        delete[] processedPoints_;
    }
    void Draw(RSPaintFilterCanvas& canvas, const SkRect*) const override;

    std::string GetTypeWithDesc() const override
    {
        std::string desc = "{OpType: " + GetOpTypeString(GetType()) +", Description:{";
        int depth = 1;
        desc += "\tmode_:" + std::to_string(static_cast<int>(mode_)) + "\n";
        desc += "\tcount_:" + std::to_string(count_) + "\n";
        if (processedPoints_ == nullptr) {
            desc += "\tprocessedPoints_ == nullptr";
        } else {
            processedPoints_->dump(desc, depth);
        }
        desc += "}, \n";
        return desc;
    }

    RSOpType GetType() const override
    {
        return RSOpType::POINTS_OPITEM;
    }

    bool Marshalling(Parcel& parcel) const override;
    [[nodiscard]] static OpItem* Unmarshalling(Parcel& parcel);

private:
    SkCanvas::PointMode mode_;
    int count_;
    SkPoint* processedPoints_;
};

class VerticesOpItem : public OpItemWithPaint {
public:
#ifdef NEW_SKIA
    VerticesOpItem(const SkVertices* vertices, SkBlendMode mode, const SkPaint& paint);
#else
    VerticesOpItem(const SkVertices* vertices, const SkVertices::Bone bones[],
        int boneCount, SkBlendMode mode, const SkPaint& paint);
#endif
    ~VerticesOpItem() override;
    void Draw(RSPaintFilterCanvas& canvas, const SkRect*) const override;

    std::string GetTypeWithDesc() const override
    {
        std::string desc = "{OpType: " + GetOpTypeString(GetType()) +", Description:{";
        int depth = 1;
        if (vertices_ == nullptr) {
            desc += "\tvertices_ = nullptr\n";
        } else {
            vertices_->dump(desc, depth);
        }
#ifndef NEW_SKIA
        if (bones_ == nullptr) {
            desc += "\tbones_ = nullptr\n";
        } else {
            bones_->dump(desc, depth);
        }
        desc += "\tboneCount_:" + std::to_string(boneCount_) + "\n";
#endif
        desc += "\tmode_:" + std::to_string(static_cast<int>(mode_)) + "\n";
        desc += "}, \n";
        return desc;
    }

    RSOpType GetType() const override
    {
        return RSOpType::VERTICES_OPITEM;
    }

    bool Marshalling(Parcel& parcel) const override;
    [[nodiscard]] static OpItem* Unmarshalling(Parcel& parcel);

private:
    sk_sp<SkVertices> vertices_;
#ifndef NEW_SKIA
    SkVertices::Bone* bones_;
    int boneCount_;
#endif
    SkBlendMode mode_;
};

class ShadowRecOpItem : public OpItem {
public:
    ShadowRecOpItem(const SkPath& path, const SkDrawShadowRec& rec);
    ~ShadowRecOpItem() override = default;
    void Draw(RSPaintFilterCanvas& canvas, const SkRect*) const override;

    std::string GetTypeWithDesc() const override
    {
        std::string desc = "{OpType: " + GetOpTypeString(GetType()) +", Description:{";
        int depth = 1;
        path_.dump(desc, depth);
        rec_.dump(desc, depth);
        desc += "}, \n";
        return desc;
    }

    RSOpType GetType() const override
    {
        return RSOpType::SHADOW_REC_OPITEM;
    }

    bool Marshalling(Parcel& parcel) const override;
    [[nodiscard]] static OpItem* Unmarshalling(Parcel& parcel);

private:
    SkPath path_;
    SkDrawShadowRec rec_;
};

class MultiplyAlphaOpItem : public OpItem {
public:
    MultiplyAlphaOpItem(float alpha);
    ~MultiplyAlphaOpItem() override = default;
    void Draw(RSPaintFilterCanvas& canvas, const SkRect*) const override;

    std::string GetTypeWithDesc() const override
    {
        std::string desc = "{OpType: " + GetOpTypeString(GetType()) +", Description:{";
        desc += "alpha_:" + std::to_string(alpha_);
        desc += "}, \n";
        return desc;
    }

    RSOpType GetType() const override
    {
        return RSOpType::MULTIPLY_ALPHA_OPITEM;
    }

    bool Marshalling(Parcel& parcel) const override;
    [[nodiscard]] static OpItem* Unmarshalling(Parcel& parcel);

private:
    float alpha_;
};

class SaveAlphaOpItem : public OpItem {
public:
    SaveAlphaOpItem();
    ~SaveAlphaOpItem() override = default;
    void Draw(RSPaintFilterCanvas& canvas, const SkRect*) const override;

    std::string GetTypeWithDesc() const override
    {
        std::string desc = "{OpType: " + GetOpTypeString(GetType()) +", Description:{";
        desc += "}, \n";
        return desc;
    }

    RSOpType GetType() const override
    {
        return RSOpType::SAVE_ALPHA_OPITEM;
    }

    [[nodiscard]] static OpItem* Unmarshalling(Parcel& parcel);
};

class RestoreAlphaOpItem : public OpItem {
public:
    RestoreAlphaOpItem();
    ~RestoreAlphaOpItem() override = default;
    void Draw(RSPaintFilterCanvas& canvas, const SkRect*) const override;

    std::string GetTypeWithDesc() const override
    {
        std::string desc = "{OpType: " + GetOpTypeString(GetType()) +", Description:{";
        desc += "}, \n";
        return desc;
    }

    RSOpType GetType() const override
    {
        return RSOpType::RESTORE_ALPHA_OPITEM;
    }

    [[nodiscard]] static OpItem* Unmarshalling(Parcel& parcel);
};

#ifdef ROSEN_OHOS
class SurfaceBufferOpItem : public OpItemWithPaint {
public:
    SurfaceBufferOpItem(const RSSurfaceBufferInfo& surfaceBufferInfo);
    ~SurfaceBufferOpItem() override;
    void Draw(RSPaintFilterCanvas& canvas, const SkRect*) const override;

    std::string GetTypeWithDesc() const override
    {
        std::string desc = "{OpType: " + GetOpTypeString(GetType()) +", Description:{";
        desc += "}, \n";
        return desc;
    }

    RSOpType GetType() const override
    {
        return RSOpType::SURFACEBUFFER_OPITEM;
    }

    bool Marshalling(Parcel& parcel) const override;
    static OpItem* Unmarshalling(Parcel& parcel);

private:
    void Clear() const noexcept;

    mutable RSSurfaceBufferInfo surfaceBufferInfo_;
#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    mutable OHNativeWindowBuffer* nativeWindowBuffer_ = nullptr;
#endif
#ifdef RS_ENABLE_VK
    mutable sk_sp<SkImage> skImage_ = nullptr;
#endif
#ifdef RS_ENABLE_GL
    mutable EGLImageKHR eglImage_ = EGL_NO_IMAGE_KHR;
    mutable GLuint texId_ = 0;
#endif
};
#endif
} // namespace Rosen
} // namespace OHOS

#else
#include "render/rs_image.h"
#include "recording/draw_cmd_list.h"
#include "recording/adaptive_image_helper.h"
#include "draw/canvas.h"
#include "parcel.h"
#if defined(ROSEN_OHOS) && (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
#include <native_window.h>
#include "surface_buffer.h"
#endif

namespace OHOS {
namespace Rosen {
class RSB_EXPORT RSExtendImageObject : public Drawing::ExtendImageObject {
public:
    RSExtendImageObject() = default;
    RSExtendImageObject(const std::shared_ptr<Drawing::Image>& image, const std::shared_ptr<Drawing::Data>& data,
        const Drawing::AdaptiveImageInfo& imageInfo);
    RSExtendImageObject(const std::shared_ptr<Media::PixelMap>& pixelMap, const Drawing::AdaptiveImageInfo& imageInfo);
    ~RSExtendImageObject() override;
    void Playback(Drawing::Canvas& canvas, const Drawing::Rect& rect,
        const Drawing::SamplingOptions& sampling, bool isBackground = false) override;
    bool Marshalling(Parcel &parcel) const;
    static RSExtendImageObject *Unmarshalling(Parcel &parcel);
#if defined(ROSEN_OHOS) && defined(RS_ENABLE_GL)
    std::shared_ptr<Drawing::Image> GetDrawingImageFromSurfaceBuffer(
        Drawing::Canvas& canvas, SurfaceBuffer* surfaceBuffer) const;
#endif
#if defined(ROSEN_OHOS) && defined(RS_ENABLE_VK)
    std::shared_ptr<Drawing::Image> MakeFromTextureForVK(Drawing::Canvas& canvas, SurfaceBuffer *surfaceBuffer);
#endif
protected:
    std::shared_ptr<RSImage> rsImage_;
private:
#if defined(ROSEN_OHOS) && (defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK))
#ifdef RS_ENABLE_GL
    mutable EGLImageKHR eglImage_ = EGL_NO_IMAGE_KHR;
    mutable GLuint texId_ = 0;
#endif
    mutable OHNativeWindowBuffer* nativeWindowBuffer_ = nullptr;
    mutable pid_t tid_ = 0;
#endif
#ifdef RS_ENABLE_VK
    mutable Drawing::BackendTexture backendTexture_ = {};
    mutable NativeBufferUtils::VulkanCleanupHelper* cleanUpHelper_ = nullptr;
#endif
};

class RSB_EXPORT RSExtendImageBaseOj : public Drawing::ExtendImageBaseOj {
public:
    RSExtendImageBaseOj() = default;
    RSExtendImageBaseOj(const std::shared_ptr<Media::PixelMap>& pixelMap, const Drawing::Rect& src,
        const Drawing::Rect& dst);
    ~RSExtendImageBaseOj() override = default;
    void Playback(Drawing::Canvas& canvas, const Drawing::Rect& rect,
        const Drawing::SamplingOptions& sampling) override;
    bool Marshalling(Parcel &parcel) const;
    static RSExtendImageBaseOj *Unmarshalling(Parcel &parcel);
protected:
    std::shared_ptr<RSImageBase> rsImage_;
};
} // namespace Rosen
} // namespace OHOS
#endif // USE_ROSEN_DRAWING
#endif // RENDER_SERVICE_CLIENT_CORE_PIPELINE_RS_DRAW_CMD_H
