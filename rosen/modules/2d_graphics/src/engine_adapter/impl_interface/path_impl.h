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

#ifndef PATH_IMPL_H
#define PATH_IMPL_H

#include "draw/path.h"
#include "utils/matrix.h"
#include "utils/point.h"
#include "utils/rect.h"
#include "utils/scalar.h"

namespace OHOS {
namespace Rosen {
struct PathData;
class PathImpl {
public:
    PathImpl() noexcept {}
    virtual ~PathImpl() {}

    virtual void MoveTo(scalar x, scalar y) = 0;
    virtual void LineTo(scalar x, scalar y) = 0;
    virtual void ArcTo(scalar pt1X, scalar pt1Y, scalar pt2X, scalar pt2Y, scalar startAngle, scalar sweepAngle) = 0;
    virtual void CubicTo(scalar ctrlPt1X, scalar ctrlPt1Y, scalar ctrlPt2X, scalar ctrlPt2Y,
                         scalar endPtX, scalar endPtY) = 0;
    virtual void QuadTo(scalar ctrlPtX, scalar ctrlPtY, scalar endPtX, scalar endPtY) = 0;

    virtual void AddRect(scalar left, scalar top, scalar right, scalar bottom,
                         PathDirection dir = PathDirection::CW_DIRECTION) = 0;
    virtual void AddOval(scalar left, scalar top, scalar right, scalar bottom,
                         PathDirection dir = PathDirection::CW_DIRECTION) = 0;
    virtual void AddArc(scalar left, scalar top, scalar right, scalar bottom,
                        scalar startAngle, scalar sweepAngle) = 0;
    virtual void AddCircle(scalar x, scalar y, scalar radius, PathDirection dir = PathDirection::CW_DIRECTION) = 0;
    virtual void AddRoundRect(scalar left, scalar top, scalar right, scalar bottom,
                              scalar xRadius, scalar yRadius, PathDirection dir = PathDirection::CW_DIRECTION) = 0;

    virtual void AddPath(const PathData* d, scalar dx, scalar dy) = 0;
    virtual void AddPath(const PathData* d) = 0;
    virtual void AddPathWithMatrix(const PathData* d, const Matrix &matrix) = 0;

    virtual void SetFillStyle(PathFillType fillstyle) = 0;

    virtual bool OpWith(const PathData* d1, const PathData* d2, PathOp op) = 0;

    virtual void Reset() = 0;

    virtual void Close() = 0;

    const PathData * GetPathData() const { return pathData_.get(); }
protected:
    std::shared_ptr<PathData> pathData_;
};
}
}
#endif
