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

#include "skia_path.h"

#include "include/pathops/SkPathOps.h"

#include "skia_path_data.h"

namespace OHOS {
namespace Rosen {
namespace Drawing {
SkiaPath::SkiaPath() noexcept
{
    pathData_ = std::make_shared<SkiaPathData>();
    pathData_->type = PathData::SKIA_PATH; 
}

void SkiaPath::MoveTo(scalar x, scalar y)
{
    MutablePath().moveTo(x, y);
}

void SkiaPath::LineTo(scalar x, scalar y)
{
    MutablePath().lineTo(x, y);
}

void SkiaPath::ArcTo(scalar pt1X, scalar pt1Y, scalar pt2X, scalar pt2Y, scalar startAngle, scalar sweepAngle)
{
    MutablePath().arcTo(SkRect::MakeLTRB(pt1X, pt1Y, pt2X, pt2Y), startAngle, sweepAngle, false);
}

void SkiaPath::CubicTo(scalar ctrlPt1X, scalar ctrlPt1Y, scalar ctrlPt2X, scalar ctrlPt2Y,
                       scalar endPtX, scalar endPtY)
{
    MutablePath().cubicTo(ctrlPt1X, ctrlPt1Y, ctrlPt2X, ctrlPt2Y, endPtX, endPtY);
}

void SkiaPath::QuadTo(scalar ctrlPtX, scalar ctrlPtY, scalar endPtX, scalar endPtY)
{
    MutablePath().quadTo(ctrlPtX, ctrlPtY, endPtX, endPtY);
}

void SkiaPath::AddRect(scalar left, scalar top, scalar right, scalar bottom, PathDirection dir)
{
    SkPath::Direction pathDir = static_cast<SkPath::Direction>(dir);
    MutablePath().addRect(SkRect::MakeLTRB(left, top, right, bottom), pathDir);
}

void SkiaPath::AddOval(scalar left, scalar top, scalar right, scalar bottom, PathDirection dir)
{
    SkPath::Direction pathDir = static_cast<SkPath::Direction>(dir);
    MutablePath().addOval(SkRect::MakeLTRB(left, top, right, bottom), pathDir);
}

void SkiaPath::AddArc(scalar left, scalar top, scalar right, scalar bottom, scalar startAngle, scalar sweepAngle)
{
    MutablePath().addArc(SkRect::MakeLTRB(left, top, right, bottom), startAngle, sweepAngle);
}

void SkiaPath::AddCircle(scalar x, scalar y, scalar radius, PathDirection dir)
{
    SkPath::Direction pathDir = static_cast<SkPath::Direction>(dir);
    MutablePath().addCircle(x, y, radius, pathDir);
}

void SkiaPath::AddRoundRect(scalar left, scalar top, scalar right, scalar bottom,
                            scalar xRadius, scalar yRadius, PathDirection dir)
{
    SkPath::Direction pathDir = static_cast<SkPath::Direction>(dir);
    MutablePath().addRoundRect(SkRect::MakeLTRB(left, top, right, bottom), xRadius, yRadius, pathDir);
}

void SkiaPath::AddPath(const PathData* d, scalar dx, scalar dy)
{
    const SkPath* p = static_cast<const SkPath*>(d->GetPath());
    if (p != nullptr) {
        MutablePath().addPath(*p, dx, dy);
    }
}

void SkiaPath::AddPath(const PathData* d)
{
    SkPath* p = static_cast<SkPath*>(d->GetPath());
    if (p != nullptr) {
        MutablePath().addPath(*p);
    }
}

void SkiaPath::AddPathWithMatrix(const PathData* d, const Matrix &matrix)
{
}

void SkiaPath::SetFillStyle(PathFillType fillstyle)
{
    SkPath::FillType ft = static_cast<SkPath::FillType>(fillstyle);
    MutablePath().setFillType(ft);
}

bool SkiaPath::OpWith(const PathData* d1, const PathData* d2, PathOp op)
{
    SkPathOp pathOp = static_cast<SkPathOp>(op);
    bool isOpSuccess = false;

    if (d1 != nullptr && d2 != nullptr) {
        SkPath* p1 = static_cast<SkPath*>(d1->GetPath());
        SkPath* p2 = static_cast<SkPath*>(d2->GetPath());
        if (p1 != nullptr && p2 != nullptr) {
            isOpSuccess = Op(*p1, *p2, pathOp, static_cast<SkPath*>(pathData_->GetPath()));
        }
    }

    if (isOpSuccess) {
        return true;
    }
    return false;
}

void SkiaPath::Reset()
{
    MutablePath().reset();
}

void SkiaPath::Close()
{
    MutablePath().close();
}

SkPath& SkiaPath::MutablePath()
{
    SkPath* p = static_cast<SkPath*>(pathData_->GetPath());
    return *p;
};
}
}
}
