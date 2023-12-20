/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef DRAW_CMD_H
#define DRAW_CMD_H

#include <unordered_map>
#include <functional>
#include <stack>

#include "draw/canvas.h"
#include "draw/paint.h"
#include "recording/cmd_list.h"
#include "recording/recording_canvas.h"

#ifdef ROSEN_OHOS
#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
#include "external_window.h"
#include "window.h"
#endif
#ifdef RS_ENABLE_GL
#include <GLES/gl.h>
#include "EGL/egl.h"
#include "EGL/eglext.h"
#include "GLES2/gl2.h"
#include "GLES2/gl2ext.h"
#include "include/gpu/gl/GrGLTypes.h"
#include "include/gpu/GrBackendSurface.h"
#endif
#endif

namespace OHOS {
namespace Rosen {
namespace Drawing {
class DrawOpItem : public OpItem {
public:
    explicit DrawOpItem(uint32_t type) : OpItem(type) {}
    ~DrawOpItem() override = default;

    // If update this item, notify feature of opinc {DDGR_ENABLE_FEATURE_OPINC}
    enum Type : uint32_t {
        OPITEM_HEAD,

        // NO-REAL-DRAW opItem
        ATTACH_PEN_OPITEM,
        ATTACH_BRUSH_OPITEM,
        DETACH_PEN_OPITEM,
        DETACH_BRUSH_OPITEM,
        CLEAR_OPITEM,
        DISCARD_OPITEM,
        SHEAR_OPITEM,
        FLUSH_OPITEM,
        SAVE_OPITEM,
        SAVE_LAYER_OPITEM,
        RESTORE_OPITEM,

        // NO-REAL-DRAW opItem, POS-CHANGE opItem
        OPINC_COUNT_OPITEM_START,

        SYMBOL_OPITEM,
        CLIP_RECT_OPITEM,
        CLIP_IRECT_OPITEM,
        CLIP_ROUND_RECT_OPITEM,
        CLIP_PATH_OPITEM,
        CLIP_REGION_OPITEM,
        CLIP_ADAPTIVE_ROUND_RECT_OPITEM,
        SET_MATRIX_OPITEM,
        RESET_MATRIX_OPITEM,
        CONCAT_MATRIX_OPITEM,
        TRANSLATE_OPITEM,
        SCALE_OPITEM,
        ROTATE_OPITEM,

        REAL_DRAW_OPITEM_START, // used by skipnode feature

        POINT_OPITEM,
        POINTS_OPITEM,
        LINE_OPITEM,
        RECT_OPITEM,
        ROUND_RECT_OPITEM,
        NESTED_ROUND_RECT_OPITEM,
        ARC_OPITEM,
        PIE_OPITEM,
        OVAL_OPITEM,
        CIRCLE_OPITEM,
        COLOR_OPITEM,
        IMAGE_NINE_OPITEM,
        IMAGE_ANNOTATION_OPITEM,
        IMAGE_LATTICE_OPITEM,
        PATH_OPITEM,
        BACKGROUND_OPITEM,
        SHADOW_OPITEM,
        BITMAP_OPITEM,
        IMAGE_OPITEM,
        IMAGE_RECT_OPITEM,
        PICTURE_OPITEM,
        TEXT_BLOB_OPITEM,
        ADAPTIVE_IMAGE_OPITEM,
        ADAPTIVE_PIXELMAP_OPITEM,
        IMAGE_WITH_PARM_OPITEM,
        PIXELMAP_WITH_PARM_OPITEM,
        PIXELMAP_RECT_OPITEM,
        REGION_OPITEM,
        PATCH_OPITEM,
        EDGEAAQUAD_OPITEM,
        VERTICES_OPITEM,
        IMAGE_SNAPSHOT_OPITEM,

        OPINC_COUNT_OPITEM_END, // used by opinc feature

        CMD_LIST_OPITEM,
        SURFACEBUFFER_OPITEM,

        REAL_DRAW_OPITEM_END, // used by skipnode feature
    };

    virtual void Playback(Canvas* canvas, const Rect* rect) = 0;
};

class UnmarshallingPlayer {
public:
    UnmarshallingPlayer(const CmdList& cmdList);
    ~UnmarshallingPlayer() = default;

    std::shared_ptr<DrawOpItem> Unmarshalling(uint32_t type, void* handle);

    const CmdList& cmdList_;

    using UnmarshallingFunc = std::shared_ptr<DrawOpItem>(*)(const CmdList& cmdList, void* handle);
private:
    static std::unordered_map<uint32_t, UnmarshallingFunc> opUnmarshallingFuncLUT_;
};

class GenerateCachedOpItemPlayer {
public:
    GenerateCachedOpItemPlayer(CmdList &cmdList, Canvas* canvas = nullptr, const Rect* rect = nullptr);
    ~GenerateCachedOpItemPlayer() = default;

    bool GenerateCachedOpItem(uint32_t type, void* handle);
    
    Canvas* canvas_ = nullptr;
    const Rect* rect_;
    CmdList& cmdList_;
};

class DrawWithPaintOpItem : public DrawOpItem {
public:
    DrawWithPaintOpItem(const CmdList& cmdList, const PaintHandle& paintHandle, uint32_t type);
    ~DrawWithPaintOpItem() override = default;
    void Playback(Canvas* canvas, const Rect* rect) override {}
protected:
    Paint paint_;
};

class ImageSnapshotOpItem : public DrawOpItem {
public:
    ImageSnapshotOpItem(std::shared_ptr<Image> image, const Rect& src, const Rect& dst);
    ~ImageSnapshotOpItem() override = default;
    void Playback(Canvas* canvas, const Rect* rect) override;
private:
    std::shared_ptr<Image> image_;
    Rect src_;
    Rect dst_;
    SamplingOptions sampling_;
    Paint paint_;
};

class DrawCmdListOpItem : public DrawOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(const CmdListHandle& handle) : OpItem(DrawOpItem::CMD_LIST_OPITEM), handle(handle) {}
        ~ConstructorHandle() override = default;
        CmdListHandle handle;
    };
    DrawCmdListOpItem(const CmdList& cmdList, ConstructorHandle* handle);
    ~DrawCmdListOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const CmdList& cmdList, void* handle);
    void Playback(Canvas* canvas, const Rect* rect) override;
private:
    std::vector<std::shared_ptr<DrawOpItem>> opItems_;
};

class DrawPointOpItem : public DrawWithPaintOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(const Point& point, const PaintHandle& paintHandle)
            : OpItem(DrawOpItem::POINT_OPITEM), point(point), paintHandle(paintHandle) {}
        ~ConstructorHandle() override = default;
        Point point;
        PaintHandle paintHandle;
    };
    DrawPointOpItem(const CmdList& cmdList, ConstructorHandle* handle);
    ~DrawPointOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const CmdList& cmdList, void* handle);
    void Playback(Canvas* canvas, const Rect* rect) override;
private:
    Point point_;
};

class DrawPointsOpItem : public DrawWithPaintOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(PointMode mode, const std::pair<uint32_t, size_t>& pts, const PaintHandle& paintHandle)
            : OpItem(DrawOpItem::POINTS_OPITEM), mode(mode), pts(pts), paintHandle(paintHandle) {}
        ~ConstructorHandle() override = default;
        PointMode mode;
        std::pair<uint32_t, size_t> pts;
        PaintHandle paintHandle;
    };
    DrawPointsOpItem(const CmdList& cmdList, ConstructorHandle* handle);
    ~DrawPointsOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const CmdList& cmdList, void* handle);
    void Playback(Canvas* canvas, const Rect* rect) override;
private:
    PointMode mode_;
    std::vector<Point> pts_;
};

class DrawLineOpItem : public DrawWithPaintOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(const Point& startPt, const Point& endPt, const PaintHandle& paintHandle)
            : OpItem(DrawOpItem::LINE_OPITEM), startPt(startPt), endPt(endPt), paintHandle(paintHandle) {}
        ~ConstructorHandle() override = default;
        Point startPt;
        Point endPt;
        PaintHandle paintHandle;
    };
    DrawLineOpItem(const CmdList& cmdList, ConstructorHandle* handle);
    ~DrawLineOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const CmdList& cmdList, void* handle);
    void Playback(Canvas* canvas, const Rect* rect) override;
private:
    Point startPt_;
    Point endPt_;
};

class DrawRectOpItem : public DrawWithPaintOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(const Rect& rect, const PaintHandle& paintHandle)
            : OpItem(DrawOpItem::RECT_OPITEM), rect(rect), paintHandle(paintHandle) {}
        ~ConstructorHandle() override = default;
        Rect rect;
        PaintHandle paintHandle;
    };
    DrawRectOpItem(const CmdList& cmdList, ConstructorHandle* handle);
    ~DrawRectOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const CmdList& cmdList, void* handle);
    void Playback(Canvas* canvas, const Rect* rect) override;
private:
    Rect rect_;
};

class DrawRoundRectOpItem : public DrawWithPaintOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(const RoundRect& rrect, const PaintHandle& paintHandle)
            : OpItem(DrawOpItem::ROUND_RECT_OPITEM), rrect(rrect), paintHandle(paintHandle) {}
        ~ConstructorHandle() override = default;
        RoundRect rrect;
        PaintHandle paintHandle;
    };
    DrawRoundRectOpItem(const CmdList& cmdList, ConstructorHandle* handle);
    ~DrawRoundRectOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const CmdList& cmdList, void* handle);
    void Playback(Canvas* canvas, const Rect* rect) override;
private:
    RoundRect rrect_;
};

class DrawNestedRoundRectOpItem : public DrawWithPaintOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(const RoundRect& outerRRect, const RoundRect& innerRRect, const PaintHandle& paintHandle)
            : OpItem(DrawOpItem::NESTED_ROUND_RECT_OPITEM), outerRRect(outerRRect), innerRRect(innerRRect),
              paintHandle(paintHandle) {}
        ~ConstructorHandle() override = default;
        RoundRect outerRRect;
        RoundRect innerRRect;
        PaintHandle paintHandle;
    };
    DrawNestedRoundRectOpItem(const CmdList& cmdList, ConstructorHandle* handle);
    ~DrawNestedRoundRectOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const CmdList& cmdList, void* handle);
    void Playback(Canvas* canvas, const Rect* rect) override;
private:
    RoundRect outerRRect_;
    RoundRect innerRRect_;
};

class DrawArcOpItem : public DrawWithPaintOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(const Rect& rect, scalar startAngle, scalar sweepAngle, const PaintHandle& paintHandle)
            : OpItem(DrawOpItem::ARC_OPITEM), rect(rect), startAngle(startAngle), sweepAngle(sweepAngle),
              paintHandle(paintHandle) {}
        ~ConstructorHandle() override = default;
        Rect rect;
        scalar startAngle;
        scalar sweepAngle;
        PaintHandle paintHandle;
    };
    DrawArcOpItem(const CmdList& cmdList, ConstructorHandle* handle);
    ~DrawArcOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const CmdList& cmdList, void* handle);
    void Playback(Canvas* canvas, const Rect* rect) override;
private:
    Rect rect_;
    scalar startAngle_;
    scalar sweepAngle_;
};

class DrawPieOpItem : public DrawWithPaintOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(const Rect& rect, scalar startAngle, scalar sweepAngle, const PaintHandle& paintHandle)
            : OpItem(DrawOpItem::PIE_OPITEM), rect(rect), startAngle(startAngle), sweepAngle(sweepAngle),
              paintHandle(paintHandle) {}
        ~ConstructorHandle() override = default;
        Rect rect;
        scalar startAngle;
        scalar sweepAngle;
        PaintHandle paintHandle;
    };
    DrawPieOpItem(const CmdList& cmdList, ConstructorHandle* handle);
    ~DrawPieOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const CmdList& cmdList, void* handle);
    void Playback(Canvas* canvas, const Rect* rect) override;
private:
    Rect rect_;
    scalar startAngle_;
    scalar sweepAngle_;
};

class DrawOvalOpItem : public DrawWithPaintOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(const Rect& rect, const PaintHandle& paintHandle)
            : OpItem(DrawOpItem::OVAL_OPITEM), rect(rect), paintHandle(paintHandle) {}
        ~ConstructorHandle() override = default;
        Rect rect;
        PaintHandle paintHandle;
    };
    DrawOvalOpItem(const CmdList& cmdList, ConstructorHandle* handle);
    ~DrawOvalOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const CmdList& cmdList, void* handle);
    void Playback(Canvas* canvas, const Rect* rect) override;
private:
    Rect rect_;
};

class DrawCircleOpItem : public DrawWithPaintOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(const Point& centerPt, scalar radius, const PaintHandle& paintHandle)
            : OpItem(DrawOpItem::CIRCLE_OPITEM), centerPt(centerPt), radius(radius), paintHandle(paintHandle) {}
        ~ConstructorHandle() override = default;
        Point centerPt;
        scalar radius;
        PaintHandle paintHandle;
    };
    DrawCircleOpItem(const CmdList& cmdList, ConstructorHandle* handle);
    ~DrawCircleOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const CmdList& cmdList, void* handle);
    void Playback(Canvas* canvas, const Rect* rect) override;
private:
    Point centerPt_;
    scalar radius_;
};

class DrawPathOpItem : public DrawWithPaintOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(const OpDataHandle& path, const PaintHandle& paintHandle)
            : OpItem(DrawOpItem::PATH_OPITEM), path(path), paintHandle(paintHandle) {}
        ~ConstructorHandle() override = default;
        OpDataHandle path;
        PaintHandle paintHandle;
    };
    DrawPathOpItem(const CmdList& cmdList, ConstructorHandle* handle);
    ~DrawPathOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const CmdList& cmdList, void* handle);
    void Playback(Canvas* canvas, const Rect* rect) override;
private:
    std::shared_ptr<Path> path_;
};

class DrawBackgroundOpItem : public DrawOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(const BrushHandle& brushHandle)
            : OpItem(DrawOpItem::BACKGROUND_OPITEM), brushHandle(brushHandle) {}
        ~ConstructorHandle() override = default;
        BrushHandle brushHandle;
    };
    DrawBackgroundOpItem(const CmdList& cmdList, ConstructorHandle* handle);
    ~DrawBackgroundOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const CmdList& cmdList, void* handle);
    void Playback(Canvas* canvas, const Rect* rect) override;
private:
    Brush brush_;
};

class DrawShadowOpItem : public DrawOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(const OpDataHandle& path, const Point3& planeParams, const Point3& devLightPos,
            scalar lightRadius, Color ambientColor, Color spotColor, ShadowFlags flag)
            : OpItem(DrawOpItem::SHADOW_OPITEM), path(path), planeParams(planeParams), devLightPos(devLightPos),
            lightRadius(lightRadius), ambientColor(ambientColor), spotColor(spotColor), flag(flag) {}
        ~ConstructorHandle() override = default;
        OpDataHandle path;
        Point3 planeParams;
        Point3 devLightPos;
        scalar lightRadius;
        Color ambientColor;
        Color spotColor;
        ShadowFlags flag;
    };
    DrawShadowOpItem(const CmdList& cmdList, ConstructorHandle* handle);
    ~DrawShadowOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const CmdList& cmdList, void* handle);
    void Playback(Canvas* canvas, const Rect* rect) override;
private:
    Point3 planeParams_;
    Point3 devLightPos_;
    scalar lightRadius_;
    Color ambientColor_;
    Color spotColor_;
    ShadowFlags flag_;
    std::shared_ptr<Path> path_;
};

class DrawRegionOpItem : public DrawWithPaintOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(const CmdListHandle& region, const PaintHandle& paintHandle)
            : OpItem(DrawOpItem::REGION_OPITEM), region(region), paintHandle(paintHandle) {}
        ~ConstructorHandle() override = default;
        CmdListHandle region;
        PaintHandle paintHandle;
    };
    DrawRegionOpItem(const CmdList& cmdList, ConstructorHandle* handle);
    ~DrawRegionOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const CmdList& cmdList, void* handle);
    void Playback(Canvas* canvas, const Rect* rect) override;
private:
    std::shared_ptr<Region> region_;
};

class DrawPatchOpItem : public DrawWithPaintOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(const std::pair<uint32_t, size_t> cubics, const std::pair<uint32_t, size_t> colors,
            const std::pair<uint32_t, size_t> texCoords, BlendMode mode, const PaintHandle& paintHandle)
            : OpItem(DrawOpItem::PATCH_OPITEM), cubics(cubics), colors(colors), texCoords(texCoords), mode(mode),
              paintHandle(paintHandle) {}
        ~ConstructorHandle() override = default;
        std::pair<uint32_t, size_t> cubics;
        std::pair<uint32_t, size_t> colors;
        std::pair<uint32_t, size_t> texCoords;
        BlendMode mode;
        PaintHandle paintHandle;
    };
    explicit DrawPatchOpItem(const CmdList& cmdList, ConstructorHandle* handle);
    ~DrawPatchOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const CmdList& cmdList, void* handle);
    void Playback(Canvas* canvas, const Rect* rect) override;
private:
    BlendMode mode_;
    std::vector<Point> cubics_;
    std::vector<ColorQuad> colors_;
    std::vector<Point> texCoords_;
};

class DrawEdgeAAQuadOpItem : public DrawOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(const Rect& rect, const std::pair<uint32_t, size_t> clipQuad, QuadAAFlags aaFlags,
            ColorQuad color, BlendMode mode) : OpItem(DrawOpItem::EDGEAAQUAD_OPITEM), rect(rect), clipQuad(clipQuad),
            aaFlags(aaFlags), color(color), mode(mode) {}
        ~ConstructorHandle() override = default;
        Rect rect;
        std::pair<uint32_t, size_t> clipQuad;
        QuadAAFlags aaFlags;
        ColorQuad color;
        BlendMode mode;
    };
    explicit DrawEdgeAAQuadOpItem(const CmdList& cmdList, ConstructorHandle* handle);
    ~DrawEdgeAAQuadOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const CmdList& cmdList, void* handle);
    void Playback(Canvas* canvas, const Rect* rect) override;
private:
    Rect rect_;
    QuadAAFlags aaFlags_;
    ColorQuad color_;
    BlendMode mode_;
    std::vector<Point> clipQuad_;
};

class DrawVerticesOpItem : public DrawWithPaintOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(const OpDataHandle& vertices, BlendMode mode, const PaintHandle& paintHandle)
            : OpItem(DrawOpItem::VERTICES_OPITEM), vertices(vertices), mode(mode), paintHandle(paintHandle) {}
        ~ConstructorHandle() override = default;
        OpDataHandle vertices;
        BlendMode mode;
        PaintHandle paintHandle;
    };
    DrawVerticesOpItem(const CmdList& cmdList, ConstructorHandle* handle);
    ~DrawVerticesOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const CmdList& cmdList, void* handle);
    void Playback(Canvas* canvas, const Rect* rect) override;
private:
    BlendMode mode_;
    std::shared_ptr<Vertices> vertices_;
};

class DrawColorOpItem : public DrawOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(ColorQuad color, BlendMode mode)
            : OpItem(DrawOpItem::COLOR_OPITEM), color(color), mode(mode) {}
        ~ConstructorHandle() override = default;
        ColorQuad color;
        BlendMode mode;
    };
    DrawColorOpItem(ConstructorHandle* handle);
    ~DrawColorOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const CmdList& cmdList, void* handle);
    void Playback(Canvas* canvas, const Rect* rect) override;
private:
    ColorQuad color_;
    BlendMode mode_;
};

class DrawImageNineOpItem : public DrawOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(const OpDataHandle& image, const RectI& center, const Rect& dst, FilterMode filterMode,
            const BrushHandle& brushHandle, bool hasBrush) : OpItem(DrawOpItem::IMAGE_NINE_OPITEM), image(image),
            center(center), dst(dst), filter(filterMode), brushHandle(brushHandle), hasBrush(hasBrush) {}
        ~ConstructorHandle() override = default;
        OpDataHandle image;
        RectI center;
        Rect dst;
        FilterMode filter;
        BrushHandle brushHandle;
        bool hasBrush;
    };
    explicit DrawImageNineOpItem(const CmdList& cmdList, ConstructorHandle* handle);
    ~DrawImageNineOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const CmdList& cmdList, void* handle);
    void Playback(Canvas* canvas, const Rect* rect) override;
private:
    RectI center_;
    Rect dst_;
    FilterMode filter_;
    bool hasBrush_;
    Brush brush_;
    std::shared_ptr<Image> image_;
};

class DrawAnnotationOpItem : public DrawOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(const Rect& rect, const char* key, const OpDataHandle& data)
            : OpItem(DrawOpItem::IMAGE_ANNOTATION_OPITEM), rect(rect), key(key), data(data) {}
        ~ConstructorHandle() override = default;
        Rect rect;
        const char* key;
        const OpDataHandle data;
    };
    DrawAnnotationOpItem(const CmdList& cmdList, ConstructorHandle* handle);
    ~DrawAnnotationOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const CmdList& cmdList, void* handle);
    void Playback(Canvas* canvas, const Rect* rect) override;
private:
    Rect rect_;
    const char* key_;
    std::shared_ptr<Data> data_;
};

class DrawImageLatticeOpItem : public DrawOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(const OpDataHandle& image, const Lattice& lattice, const Rect& dst, FilterMode filterMode,
            const BrushHandle& brushHandle, bool hasBrush) : OpItem(DrawOpItem::IMAGE_LATTICE_OPITEM), image(image),
            lattice(lattice), dst(dst), filter(filterMode), brushHandle(brushHandle), hasBrush(hasBrush) {}
        ~ConstructorHandle() override = default;
        OpDataHandle image;
        Lattice lattice;
        Rect dst;
        FilterMode filter;
        BrushHandle brushHandle;
        bool hasBrush;
    };
    explicit DrawImageLatticeOpItem(const CmdList& cmdList, ConstructorHandle* handle);
    ~DrawImageLatticeOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const CmdList& cmdList, void* handle);
    void Playback(Canvas* canvas, const Rect* rect) override;
private:
    Lattice lattice_;
    Rect dst_;
    FilterMode filter_;
    bool hasBrush_;
    Brush brush_;
    std::shared_ptr<Image> image_;
};

class DrawBitmapOpItem : public DrawWithPaintOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(const ImageHandle& bitmap, scalar px, scalar py, const PaintHandle& paintHandle)
            : OpItem(DrawOpItem::BITMAP_OPITEM), bitmap(bitmap), px(px), py(py), paintHandle(paintHandle) {}
        ~ConstructorHandle() override = default;
        ImageHandle bitmap;
        scalar px;
        scalar py;
        PaintHandle paintHandle;
    };
    DrawBitmapOpItem(const CmdList& cmdList, ConstructorHandle* handle);
    ~DrawBitmapOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const CmdList& cmdList, void* handle);
    void Playback(Canvas* canvas, const Rect* rect) override;

private:
    scalar px_;
    scalar py_;
    std::shared_ptr<Bitmap> bitmap_;
};

class DrawImageOpItem : public DrawWithPaintOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(const OpDataHandle& image, scalar px, scalar py, const SamplingOptions& samplingOptions,
            const PaintHandle& paintHandle)
            : OpItem(DrawOpItem::IMAGE_OPITEM), image(image), px(px), py(py), samplingOptions(samplingOptions),
              paintHandle(paintHandle) {}
        ~ConstructorHandle() override = default;
        OpDataHandle image;
        scalar px;
        scalar py;
        SamplingOptions samplingOptions;
        PaintHandle paintHandle;
    };
    DrawImageOpItem(const CmdList& cmdList, ConstructorHandle* handle);
    ~DrawImageOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const CmdList& cmdList, void* handle);
    void Playback(Canvas* canvas, const Rect* rect) override;

private:
    scalar px_;
    scalar py_;
    SamplingOptions samplingOptions_;
    std::shared_ptr<Image> image_;
};

class DrawImageRectOpItem : public DrawWithPaintOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(const OpDataHandle& image, const Rect& src, const Rect& dst, const SamplingOptions& sampling,
            SrcRectConstraint constraint, const PaintHandle& paintHandle, bool isForeground = false)
            : OpItem(DrawOpItem::IMAGE_RECT_OPITEM), image(image), src(src), dst(dst),
              sampling(sampling), constraint(constraint), paintHandle(paintHandle), isForeground(isForeground) {}
        ~ConstructorHandle() override = default;
        OpDataHandle image;
        Rect src;
        Rect dst;
        SamplingOptions sampling;
        SrcRectConstraint constraint;
        PaintHandle paintHandle;
        bool isForeground;
    };
    DrawImageRectOpItem(const CmdList& cmdList, ConstructorHandle* handle);
    ~DrawImageRectOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const CmdList& cmdList, void* handle);
    void Playback(Canvas* canvas, const Rect* rect) override;
private:
    Rect src_;
    Rect dst_;
    SamplingOptions sampling_;
    SrcRectConstraint constraint_;
    std::shared_ptr<Image> image_;
    bool isForeground_ = false;
};

class DrawPictureOpItem : public DrawOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(const OpDataHandle& picture) : OpItem(DrawOpItem::PICTURE_OPITEM), picture(picture) {}
        ~ConstructorHandle() override = default;
        OpDataHandle picture;
    };
    DrawPictureOpItem(const CmdList& cmdList, ConstructorHandle* handle);
    ~DrawPictureOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const CmdList& cmdList, void* handle);
    void Playback(Canvas* canvas, const Rect* rect) override;
private:
    std::shared_ptr<Picture> picture_;
};

class DrawTextBlobOpItem : public DrawWithPaintOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(const OpDataHandle& textBlob, scalar x, scalar y, const PaintHandle& paintHandle)
            : OpItem(DrawOpItem::TEXT_BLOB_OPITEM), textBlob(textBlob), x(x), y(y), paintHandle(paintHandle) {}
        ~ConstructorHandle() override = default;
        static bool GenerateCachedOpItem(CmdList& cmdList, const TextBlob* textBlob, scalar x, scalar y, Paint& p);
        bool GenerateCachedOpItem(CmdList& cmdList, Canvas* canvas);
        OpDataHandle textBlob;
        scalar x;
        scalar y;
        PaintHandle paintHandle;
    };
    DrawTextBlobOpItem(const CmdList& cmdList, ConstructorHandle* handle);
    ~DrawTextBlobOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const CmdList& cmdList, void* handle);
    void Playback(Canvas* canvas, const Rect* rect) override;

    std::shared_ptr<ImageSnapshotOpItem> GenerateCachedOpItem(Canvas* canvas);
private:
    scalar x_;
    scalar y_;
    std::shared_ptr<TextBlob> textBlob_;
};

class DrawSymbolOpItem : public DrawWithPaintOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(const SymbolOpHandle& symbolHandle, Point locate, const PaintHandle& paintHandle)
            : OpItem(DrawOpItem::SYMBOL_OPITEM), symbolHandle(symbolHandle), locate(locate), paintHandle(paintHandle) {}
        ~ConstructorHandle() override = default;
        SymbolOpHandle symbolHandle;
        Point locate;
        PaintHandle paintHandle;
    };
    DrawSymbolOpItem(const CmdList& cmdList, ConstructorHandle* handle);
    ~DrawSymbolOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const CmdList& cmdList, void* handle);
    void Playback(Canvas* canvas, const Rect* rect) override;
private:
    static void MergeDrawingPath(
        Drawing::Path& multPath, Drawing::DrawingRenderGroup& group, std::vector<Drawing::Path>& pathLayers);
    DrawingHMSymbolData symbol_;
    Point locate_;
};

class ClipRectOpItem : public DrawOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(const Rect& rect, ClipOp op, bool doAntiAlias)
            : OpItem(DrawOpItem::CLIP_RECT_OPITEM), rect(rect), clipOp(op), doAntiAlias(doAntiAlias) {}
        ~ConstructorHandle() override = default;
        Rect rect;
        ClipOp clipOp;
        bool doAntiAlias;
    };
    ClipRectOpItem(ConstructorHandle* handle);
    ~ClipRectOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const CmdList& cmdList, void* handle);
    void Playback(Canvas* canvas, const Rect* rect) override;
private:
    Rect rect_;
    ClipOp clipOp_;
    bool doAntiAlias_;
};

class ClipIRectOpItem : public DrawOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(const RectI& rect, ClipOp op = ClipOp::INTERSECT)
            : OpItem(DrawOpItem::CLIP_IRECT_OPITEM), rect(rect), clipOp(op) {}
        ~ConstructorHandle() override = default;
        RectI rect;
        ClipOp clipOp;
    };
    ClipIRectOpItem(ConstructorHandle* handle);
    ~ClipIRectOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const CmdList& cmdList, void* handle);
    void Playback(Canvas* canvas, const Rect* rect) override;
private:
    RectI rect_;
    ClipOp clipOp_;
};

class ClipRoundRectOpItem : public DrawOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(const RoundRect& rrect, ClipOp op, bool doAntiAlias)
            : OpItem(DrawOpItem::CLIP_ROUND_RECT_OPITEM), rrect(rrect), clipOp(op), doAntiAlias(doAntiAlias) {}
        ~ConstructorHandle() override = default;
        RoundRect rrect;
        ClipOp clipOp;
        bool doAntiAlias;
    };
    ClipRoundRectOpItem(ConstructorHandle* handle);
    ~ClipRoundRectOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const CmdList& cmdList, void* handle);
    void Playback(Canvas* canvas, const Rect* rect) override;
private:
    RoundRect rrect_;
    ClipOp clipOp_;
    bool doAntiAlias_;
};

class ClipPathOpItem : public DrawOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(const OpDataHandle& path, ClipOp clipOp, bool doAntiAlias)
            : OpItem(DrawOpItem::CLIP_PATH_OPITEM), path(path), clipOp(clipOp), doAntiAlias(doAntiAlias) {}
        ~ConstructorHandle() override = default;
        OpDataHandle path;
        ClipOp clipOp;
        bool doAntiAlias;
    };
    ClipPathOpItem(const CmdList& cmdList, ConstructorHandle* handle);
    ~ClipPathOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const CmdList& cmdList, void* handle);
    void Playback(Canvas* canvas, const Rect* rect) override;
private:
    ClipOp clipOp_;
    bool doAntiAlias_;
    std::shared_ptr<Path> path_;
};

class ClipRegionOpItem : public DrawOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(const CmdListHandle& region, ClipOp clipOp = ClipOp::INTERSECT)
            : OpItem(DrawOpItem::CLIP_REGION_OPITEM), region(region), clipOp(clipOp) {}
        ~ConstructorHandle() override = default;
        CmdListHandle region;
        ClipOp clipOp;
    };
    ClipRegionOpItem(const CmdList& cmdList, ConstructorHandle* handle);
    ~ClipRegionOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const CmdList& cmdList, void* handle);
    void Playback(Canvas* canvas, const Rect* rect) override;
private:
    ClipOp clipOp_;
    std::shared_ptr<Region> region_;
};

class SetMatrixOpItem : public DrawOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(const Matrix::Buffer& matrixBuffer)
            : OpItem(DrawOpItem::SET_MATRIX_OPITEM), matrixBuffer(matrixBuffer) {}
        ~ConstructorHandle() override = default;
        Matrix::Buffer matrixBuffer;
    };
    SetMatrixOpItem(ConstructorHandle* handle);
    ~SetMatrixOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const CmdList& cmdList, void* handle);
    void Playback(Canvas* canvas, const Rect* rect) override;
private:
    Matrix matrix_;
};

class ResetMatrixOpItem : public DrawOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle() : OpItem(DrawOpItem::RESET_MATRIX_OPITEM) {}
        ~ConstructorHandle() override = default;
    };
    ResetMatrixOpItem();
    ~ResetMatrixOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const CmdList& cmdList, void* handle);
    void Playback(Canvas* canvas, const Rect* rect) override;
};

class ConcatMatrixOpItem : public DrawOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(const Matrix::Buffer& matrixBuffer)
            : OpItem(DrawOpItem::CONCAT_MATRIX_OPITEM), matrixBuffer(matrixBuffer) {}
        ~ConstructorHandle() override = default;
        Matrix::Buffer matrixBuffer;
    };
    ConcatMatrixOpItem(ConstructorHandle* handle);
    ~ConcatMatrixOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const CmdList& cmdList, void* handle);
    void Playback(Canvas* canvas, const Rect* rect) override;
private:
    Matrix matrix_;
};

class TranslateOpItem : public DrawOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(scalar dx, scalar dy) : OpItem(DrawOpItem::TRANSLATE_OPITEM), dx(dx), dy(dy) {}
        ~ConstructorHandle() override = default;
        scalar dx;
        scalar dy;
    };
    TranslateOpItem(ConstructorHandle* handle);
    ~TranslateOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const CmdList& cmdList, void* handle);
    void Playback(Canvas* canvas, const Rect* rect) override;
private:
    scalar dx_;
    scalar dy_;
};

class ScaleOpItem : public DrawOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(scalar sx, scalar sy) : OpItem(DrawOpItem::SCALE_OPITEM), sx(sx), sy(sy) {}
        ~ConstructorHandle() override = default;
        scalar sx;
        scalar sy;
    };
    ScaleOpItem(ConstructorHandle* handle);
    ~ScaleOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const CmdList& cmdList, void* handle);
    void Playback(Canvas* canvas, const Rect* rect) override;
private:
    scalar sx_;
    scalar sy_;
};

class RotateOpItem : public DrawOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(scalar deg, scalar sx, scalar sy)
            : OpItem(DrawOpItem::ROTATE_OPITEM), deg(deg), sx(sx), sy(sy) {}
        ~ConstructorHandle() override = default;
        scalar deg;
        scalar sx;
        scalar sy;
    };
    RotateOpItem(ConstructorHandle* handle);
    ~RotateOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const CmdList& cmdList, void* handle);
    void Playback(Canvas* canvas, const Rect* rect) override;
private:
    scalar deg_;
    scalar sx_;
    scalar sy_;
};

class ShearOpItem : public DrawOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(scalar sx, scalar sy) : OpItem(DrawOpItem::SHEAR_OPITEM), sx(sx), sy(sy) {}
        ~ConstructorHandle() override = default;
        scalar sx;
        scalar sy;
    };
    ShearOpItem(ConstructorHandle* handle);
    ~ShearOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const CmdList& cmdList, void* handle);
    void Playback(Canvas* canvas, const Rect* rect) override;
private:
    scalar sx_;
    scalar sy_;
};

class FlushOpItem : public DrawOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle() : OpItem(DrawOpItem::FLUSH_OPITEM) {}
        ~ConstructorHandle() override = default;
    };
    FlushOpItem();
    ~FlushOpItem() override = default;
    static std::shared_ptr<DrawOpItem> Unmarshalling(const CmdList& cmdList, void* handle);
    void Playback(Canvas* canvas, const Rect* rect) override;
};

class ClearOpItem : public DrawOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(ColorQuad color) : OpItem(DrawOpItem::CLEAR_OPITEM), color(color) {}
        ~ConstructorHandle() override = default;
        ColorQuad color;
    };
    ClearOpItem(ConstructorHandle* handle);
    ~ClearOpItem() override = default;
    static std::shared_ptr<DrawOpItem> Unmarshalling(const CmdList& cmdList, void* handle);
    void Playback(Canvas* canvas, const Rect* rect) override;
private:
    ColorQuad color_;
};

class SaveOpItem : public DrawOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle() : OpItem(DrawOpItem::SAVE_OPITEM) {}
        ~ConstructorHandle() override = default;
    };
    SaveOpItem();
    ~SaveOpItem() override = default;
    static std::shared_ptr<DrawOpItem> Unmarshalling(const CmdList& cmdList, void* handle);
    void Playback(Canvas* canvas, const Rect* rect) override;
};

class SaveLayerOpItem : public DrawOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(const Rect& rect, bool hasBrush, const BrushHandle& brushHandle,
            const CmdListHandle& imageFilter, uint32_t saveLayerFlags) : OpItem(DrawOpItem::SAVE_LAYER_OPITEM),
            rect(rect), hasBrush(hasBrush), brushHandle(brushHandle), imageFilter(imageFilter),
            saveLayerFlags(saveLayerFlags) {}
        ~ConstructorHandle() override = default;
        Rect rect;
        bool hasBrush;
        BrushHandle brushHandle;
        CmdListHandle imageFilter;
        uint32_t saveLayerFlags;
    };
    SaveLayerOpItem(const CmdList& cmdList, ConstructorHandle* handle);
    ~SaveLayerOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const CmdList& cmdList, void* handle);
    void Playback(Canvas* canvas, const Rect* rect) override;
private:
    Rect rect_;
    uint32_t saveLayerFlags_;
    bool hasBrush_;
    Brush brush_;
    std::shared_ptr<ImageFilter> imageFilter_;
};

class RestoreOpItem : public DrawOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle() : OpItem(DrawOpItem::RESTORE_OPITEM) {}
        ~ConstructorHandle() override = default;
    };
    RestoreOpItem();
    ~RestoreOpItem() override = default;
    static std::shared_ptr<DrawOpItem> Unmarshalling(const CmdList& cmdList, void* handle);
    void Playback(Canvas* canvas, const Rect* rect) override;
};

class DiscardOpItem : public DrawOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle() : OpItem(DrawOpItem::DISCARD_OPITEM) {}
        ~ConstructorHandle() override = default;
    };
    DiscardOpItem();
    ~DiscardOpItem() override = default;
    static std::shared_ptr<DrawOpItem> Unmarshalling(const CmdList& cmdList, void* handle);
    void Playback(Canvas* canvas, const Rect* rect) override;
};

class ClipAdaptiveRoundRectOpItem : public DrawOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(const std::pair<uint32_t, size_t>& radiusData)
            : OpItem(DrawOpItem::CLIP_ADAPTIVE_ROUND_RECT_OPITEM), radiusData(radiusData) {}
        ~ConstructorHandle() override = default;
        std::pair<uint32_t, size_t> radiusData;
    };
    ClipAdaptiveRoundRectOpItem(const CmdList& cmdList, ConstructorHandle* handle);
    ~ClipAdaptiveRoundRectOpItem() override = default;
    static std::shared_ptr<DrawOpItem> Unmarshalling(const CmdList& cmdList, void* handle);
    void Playback(Canvas* canvas, const Rect* rect) override;
private:
    std::vector<Point> radiusData_;
};

class DrawAdaptiveImageOpItem : public DrawWithPaintOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(const OpDataHandle& image, const AdaptiveImageInfo& rsImageInfo,
            const SamplingOptions& sampling, const bool isImage, const PaintHandle& paintHandle)
            : OpItem(DrawOpItem::ADAPTIVE_IMAGE_OPITEM), image(image), rsImageInfo(rsImageInfo), sampling(sampling),
              isImage(isImage), paintHandle(paintHandle) {}
        ~ConstructorHandle() override = default;
        OpDataHandle image;
        AdaptiveImageInfo rsImageInfo;
        SamplingOptions sampling;
        bool isImage;
        PaintHandle paintHandle;
    };
    DrawAdaptiveImageOpItem(const CmdList& cmdList, ConstructorHandle* handle);
    ~DrawAdaptiveImageOpItem() override = default;
    static std::shared_ptr<DrawOpItem> Unmarshalling(const CmdList& cmdList, void* handle);
    void Playback(Canvas* canvas, const Rect* rect) override;
private:
    AdaptiveImageInfo rsImageInfo_;
    SamplingOptions sampling_;
    bool isImage_;
    std::shared_ptr<Image> image_;
    std::shared_ptr<Data> data_;
};

class DrawAdaptivePixelMapOpItem : public DrawWithPaintOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(const OpDataHandle& pixelMap, const AdaptiveImageInfo& imageInfo,
            const SamplingOptions& smapling, const PaintHandle& paintHandle)
            : OpItem(DrawOpItem::ADAPTIVE_PIXELMAP_OPITEM), pixelMap(pixelMap),
              imageInfo(imageInfo), smapling(smapling), paintHandle(paintHandle) {}
        ~ConstructorHandle() override = default;
        OpDataHandle pixelMap;
        AdaptiveImageInfo imageInfo;
        SamplingOptions smapling;
        PaintHandle paintHandle;
    };
    DrawAdaptivePixelMapOpItem(const CmdList& cmdList, ConstructorHandle* handle);
    ~DrawAdaptivePixelMapOpItem() override = default;
    static std::shared_ptr<DrawOpItem> Unmarshalling(const CmdList& cmdList, void* handle);
    void Playback(Canvas* canvas, const Rect* rect) override;
private:
    AdaptiveImageInfo imageInfo_;
    SamplingOptions smapling_;
    std::shared_ptr<Media::PixelMap> pixelMap_;
};

class DrawImageWithParmOpItem : public DrawWithPaintOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(const OpDataHandle& objectHandle, const SamplingOptions& sampling,
            const PaintHandle& paintHandle)
            : OpItem(DrawOpItem::IMAGE_WITH_PARM_OPITEM), objectHandle(objectHandle), sampling(sampling),
              paintHandle(paintHandle) {}
        ~ConstructorHandle() override = default;
        OpDataHandle objectHandle;
        SamplingOptions sampling;
        PaintHandle paintHandle;
    };
    DrawImageWithParmOpItem(const CmdList& cmdList, ConstructorHandle* handle);
    ~DrawImageWithParmOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const CmdList& cmdList, void* handle);
    void Playback(Canvas* canvas, const Rect* rect) override;
private:
    SamplingOptions sampling_;
    std::shared_ptr<ExtendImageObject> objectHandle_;
};

class DrawPixelMapWithParmOpItem : public DrawWithPaintOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(const OpDataHandle& objectHandle, const SamplingOptions& sampling,
            const PaintHandle& paintHandle)
            : OpItem(DrawOpItem::PIXELMAP_WITH_PARM_OPITEM), objectHandle(objectHandle), sampling(sampling),
              paintHandle(paintHandle) {}
        ~ConstructorHandle() override = default;
        OpDataHandle objectHandle;
        SamplingOptions sampling;
        PaintHandle paintHandle;
    };
    DrawPixelMapWithParmOpItem(const CmdList& cmdList, ConstructorHandle* handle);
    ~DrawPixelMapWithParmOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const CmdList& cmdList, void* handle);
    void Playback(Canvas* canvas, const Rect* rect) override;
private:
    SamplingOptions sampling_;
    std::shared_ptr<ExtendImageObject> objectHandle_;
};

class DrawPixelMapRectOpItem : public DrawWithPaintOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(const OpDataHandle& objectHandle, const SamplingOptions& sampling,
            const PaintHandle& paintHandle)
            : OpItem(DrawOpItem::PIXELMAP_RECT_OPITEM), objectHandle(objectHandle), sampling(sampling),
              paintHandle(paintHandle) {}
        ~ConstructorHandle() override = default;
        OpDataHandle objectHandle;
        SamplingOptions sampling;
        PaintHandle paintHandle;
    };
    DrawPixelMapRectOpItem(const CmdList& cmdList, ConstructorHandle* handle);
    ~DrawPixelMapRectOpItem() override = default;

    static std::shared_ptr<DrawOpItem> Unmarshalling(const CmdList& cmdList, void* handle);
    void Playback(Canvas* canvas, const Rect* rect) override;
private:
    SamplingOptions sampling_;
    std::shared_ptr<ExtendImageBaseOj> objectHandle_;
};
#ifdef ROSEN_OHOS
class DrawSurfaceBufferOpItem : public DrawWithPaintOpItem {
public:
    struct ConstructorHandle : public OpItem {
        ConstructorHandle(uint32_t surfaceBufferId, int offSetX, int offSetY, int width, int height,
            const PaintHandle& paintHandle)
            : OpItem(DrawOpItem::SURFACEBUFFER_OPITEM), surfaceBufferId(surfaceBufferId),
            surfaceBufferInfo(nullptr, offSetX, offSetY, width, height), paintHandle(paintHandle) {}
        ~ConstructorHandle() override = default;
        uint32_t surfaceBufferId;
        DrawingSurfaceBufferInfo surfaceBufferInfo;
        PaintHandle paintHandle;
    };

    DrawSurfaceBufferOpItem(const CmdList& cmdList, ConstructorHandle* handle);
    ~DrawSurfaceBufferOpItem();

    static std::shared_ptr<DrawOpItem> Unmarshalling(const CmdList& cmdList, void* handle);
    void Playback(Canvas* canvas, const Rect* rect) override;
#ifdef RS_ENABLE_VK
    static void SetBaseCallback(
        std::function<Drawing::BackendTexture(NativeWindowBuffer* buffer, int width, int height)> makeBackendTexture,
        std::function<void(void* context)> deleteImage,
        std::function<void*(VkImage image, VkDeviceMemory memory)> helper);
#endif

private:
    DrawingSurfaceBufferInfo surfaceBufferInfo_;
    void Clear();
    void Draw(Canvas* canvas);

#if defined(RS_ENABLE_GL) || defined(RS_ENABLE_VK)
    OHNativeWindowBuffer* nativeWindowBuffer_ = nullptr;
#endif
#ifdef RS_ENABLE_VK
    static std::function<Drawing::BackendTexture(NativeWindowBuffer* buffer, int width, int height)>
        makeBackendTextureFromNativeBuffer;
    static std::function<void(void* context)> deleteVkImage;
    static std::function<void*(VkImage image, VkDeviceMemory memory)> vulkanCleanupHelper;
#endif
#ifdef RS_ENABLE_GL
    EGLImageKHR eglImage_ = EGL_NO_IMAGE_KHR;
    GLuint texId_ = 0;
#endif
};
#endif

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS
#endif
