/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#include "render/rs_attraction_effect_filter.h"

#include "common/rs_common_def.h"
#include "utils/point.h"
#include "common/rs_optional_trace.h"
#include "platform/common/rs_log.h"
#include "src/core/SkOpts.h"

namespace OHOS {
namespace Rosen {
RSAttractionEffectFilter::RSAttractionEffectFilter(float attractionFraction)
    : RSDrawingFilterOriginal(nullptr), attractionFraction_(attractionFraction)
{
    type_ = FilterType::ATTRACTION_EFFECT;

    hash_ = SkOpts::hash(&type_, sizeof(type_), 0);
    hash_ = SkOpts::hash(&attractionFraction_, sizeof(attractionFraction_), hash_);
}

RSAttractionEffectFilter::~RSAttractionEffectFilter() = default;

std::string RSAttractionEffectFilter::GetDescription()
{
    return "RSAttractionEffectFilter " +  std::to_string(attractionFraction_);
}

bool RSAttractionEffectFilter::IsValid() const
{
    constexpr float epsilon = 0.001f;
    return attractionFraction_ > epsilon;
}

float RSAttractionEffectFilter::GetAttractionFraction() const
{
    return attractionFraction_;
}

Drawing::Point RSAttractionEffectFilter::LerpPoint(const Drawing::Point &firstPoint, const Drawing::Point &secondPoint,
    float k1, float k2)
{
    Drawing::Point p = { 0.0f, 0.0f };
    p = k1 * firstPoint + k2 * secondPoint;
    return p;
}

Drawing::Point RSAttractionEffectFilter::CubicBezier(const Drawing::Point &p0, const Drawing::Point &p1,
    const Drawing::Point &p2, const Drawing::Point &p3, float t)
{
    float u = 1 - t;
    float tt = t * t;
    float uu = u * u;
    float uuu = uu * u;
    float ttt = tt * t;

    Drawing::Point p = { 0.0f, 0.0f };
    float besselCoefficient = 3.0f;
    p = p0 * uuu + p1 * besselCoefficient * uu * t + p2 * besselCoefficient * u * tt + p3 * ttt;
    return p;
}

float RSAttractionEffectFilter::CalculateCubic(float p1, float p2, float t)
{
    return 3.0f * p1 * (1.0f - t) * (1.0f - t) * t + 3.0f * p2 * (1.0f - t) * t * t + t * t * t;
}

bool RSAttractionEffectFilter::IsWithinThreshold(const float left, const float right, const float threshold)
{
    return (std::abs(left - right) <= threshold);
}

float RSAttractionEffectFilter::BinarySearch(float targetX, const Drawing::Point &p1, const Drawing::Point &p2)
{
    float start = 0.0f;
    float end = 1.0f;
    float midPoint = 0.0f;
    int maxIterCount = 100;
    int currentIter = 0;
    while (currentIter < maxIterCount) {
        midPoint = (start + end) / 2.0f;
        float estimate = CalculateCubic(p1.GetX(), p2.GetX(), midPoint);
        if (IsWithinThreshold(targetX, estimate, 0.001f)) {
            return CalculateCubic(p1.GetY(), p2.GetY(), midPoint);
        }
        if (estimate < targetX) {
            start = midPoint;
        } else {
            end = midPoint;
        }
        currentIter++;
    }
    return CalculateCubic(p1.GetY(), p2.GetY(), midPoint);
}

std::vector<Drawing::Point> RSAttractionEffectFilter::CalculateCubicsCtrlPointOffset(
    const std::vector<Drawing::Point> controlPointOfVertex)
{
    std::vector<Drawing::Point> pathList;
    Drawing::Point topLeft = controlPointOfVertex[0];
    Drawing::Point topRight = controlPointOfVertex[1];
    Drawing::Point bottomLeft = controlPointOfVertex[2];
    Drawing::Point bottomRight = controlPointOfVertex[3];

    pathList.push_back(topLeft);                                          // 0
    pathList.push_back(LerpPoint(topLeft, topRight, 0.67f, 0.33f));       // 1
    pathList.push_back(LerpPoint(topLeft, topRight, 0.33f, 0.67f));       // 2
    pathList.push_back(topRight);                                         // 3
    pathList.push_back(LerpPoint(topRight, bottomRight, 0.67f, 0.33f));   // 4
    pathList.push_back(LerpPoint(topRight, bottomRight, 0.33f, 0.67f));   // 5
    pathList.push_back(bottomRight);                                      // 6
    pathList.push_back(LerpPoint(bottomLeft, bottomRight, 0.33f, 0.67f)); // 7
    pathList.push_back(LerpPoint(bottomLeft, bottomRight, 0.67f, 0.33f)); // 8
    pathList.push_back(bottomLeft);                                       // 9
    pathList.push_back(LerpPoint(topLeft, bottomLeft, 0.33f, 0.67f));     // 10
    pathList.push_back(LerpPoint(topLeft, bottomLeft, 0.67f, 0.33f));     // 11
    return pathList;
}

std::vector<int> RSAttractionEffectFilter::CreateIndexSequence(bool isBelowTarget, float location)
{
    // Select the parameter index of the window control point according to the window position.
    // 0 to 11 indicate the index of the window control point, which is rotated clockwise.
    if (!isBelowTarget) {
        return location == 1.0f ? std::vector<int>{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 } :
                                  std::vector<int>{ 3, 2, 1, 0, 11, 10, 9, 8, 7, 6, 5, 4 };
    } else {
        return location == 1.0f ? std::vector<int>{ 9, 10, 11, 0, 1, 2, 3, 4, 5, 6, 7, 8 } :
                                  std::vector<int>{ 0, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1 };
    }
}

std::vector<Drawing::Point> RSAttractionEffectFilter::CalculateCubicsCtrlPoint(
    std::vector<Drawing::Point> controlPointOfVertex,
    const Drawing::Point points[],          // Array instead of vector
    float location,                         // Determine whether the flag on the left or right is used.
    bool isBelowTarget,                     // Determine whether the upper or lower flag is used.
    bool isFirstCtrl)
{
    int pointNum = 12;
    std::vector<Drawing::Point> pathList = CalculateCubicsCtrlPointOffset(controlPointOfVertex);

    std::vector<int> indice = CreateIndexSequence(isBelowTarget, location);
    std::vector<Drawing::Point> pathCtrlPointList(pointNum, Drawing::Point(0.0f, 0.0f));

    for (int i = 0; i < pointNum; i++) {
        int index = indice[i];
        if (!isBelowTarget) {
            pathCtrlPointList[i] = pathList[index] +  (isFirstCtrl ? points[i] : points[0]);
        } else {
            float px = isFirstCtrl ? points[i].GetX() : points[0].GetX();
            float py = isFirstCtrl ? points[i].GetY() : points[0].GetY();

            if (location == 1.0f) {
                px = -px;
                pathCtrlPointList[i] = { -(pathList[index].GetY() +  px), pathList[index].GetX() +  py };
            } else {
                py = -py;
                pathCtrlPointList[i] = { pathList[index].GetY() +  px, -(pathList[index].GetX() +  py) };
            }
        }
    }
    return pathCtrlPointList;
}

void RSAttractionEffectFilter::CalculateBezierVelList(const std::vector<Drawing::Point> &velocityList,
    std::vector<Drawing::Point> &velocityCtrl,
    float location, bool isBelowTarget)
{
    std::vector<Drawing::Point> curveVelList;
    Drawing::Point topVelocity = velocityList[0];
    Drawing::Point bottomVelocity = velocityList[1];
    curveVelList.push_back(LerpPoint(topVelocity, bottomVelocity, 0.5f, 0.5f));
    curveVelList.push_back(LerpPoint(bottomVelocity, topVelocity, 0.33f, 0.67f));
    curveVelList.push_back(LerpPoint(bottomVelocity, topVelocity, 0.17f, 0.83f));
    curveVelList.push_back(topVelocity);
    curveVelList.push_back(LerpPoint(bottomVelocity, topVelocity, 0.17f, 0.68f));
    curveVelList.push_back(LerpPoint(bottomVelocity, topVelocity, 0.33f, 0.57f));
    curveVelList.push_back(LerpPoint(topVelocity, bottomVelocity, 0.5f, 0.5f));
    curveVelList.push_back(LerpPoint(bottomVelocity, topVelocity, 0.67f, 0.33f));
    curveVelList.push_back(LerpPoint(bottomVelocity, topVelocity, 0.83f, 0.17f));
    curveVelList.push_back(bottomVelocity);
    curveVelList.push_back(LerpPoint(bottomVelocity, topVelocity, 0.83f, 0.17f));
    curveVelList.push_back(LerpPoint(bottomVelocity, topVelocity, 0.67f, 0.33f));
    std::vector<int> indice = CreateIndexSequence(isBelowTarget, location);
    int pointNum = 12;
    for (int i = 0; i < pointNum; i++) {
        int index = indice[i];
        velocityCtrl[i] = curveVelList[index];
    }
}

void RSAttractionEffectFilter::GetWindowInitCtrlPoints(Drawing::Point windowCtrlPoints[], float canvasWidth,
    float canvasHeight, int size)
{
    // 0 to 11 indicate the index of the window control point, which is rotated clockwise.
    const int TOP_LEFT              = 0;
    const int TOP_ONE_THIRD         = 1;
    const int TOP_TWO_THIRDS        = 2;
    const int TOP_RIGHT             = 3;
    const int RIGHT_ONE_THIRD       = 4;
    const int RIGHT_TWO_THIRDS      = 5;
    const int BOTTOM_RIGHT          = 6;
    const int BOTTOM_TWO_THIRDS     = 7;
    const int BOTTOM_ONE_THIRD      = 8;
    const int BOTTOM_LEFT           = 9;
    const int LEFT_TWO_THIRDS       = 10;
    const int LEFT_ONE_THIRD        = 11;

    float widthOneThird = canvasWidth / 3.0;
    float widthTwoThirds = canvasWidth / 3.0 * 2.0;
    float heightOneThird = canvasHeight / 3.0;
    float heightTwoThirds = canvasHeight / 3.0 * 2.0;

    int pointNum = 12;
    if (size == pointNum) {
        windowCtrlPoints[TOP_LEFT] = { 0.0f, 0.0f };
        windowCtrlPoints[TOP_ONE_THIRD] = { widthOneThird, 0.0f };
        windowCtrlPoints[TOP_TWO_THIRDS] = { widthTwoThirds, 0.0f };
        windowCtrlPoints[TOP_RIGHT] = { canvasWidth, 0.0f };
        windowCtrlPoints[RIGHT_ONE_THIRD] = { canvasWidth, heightOneThird };
        windowCtrlPoints[RIGHT_TWO_THIRDS] = { canvasWidth, heightTwoThirds };
        windowCtrlPoints[BOTTOM_RIGHT] = { canvasWidth, canvasHeight };
        windowCtrlPoints[BOTTOM_TWO_THIRDS] = { widthTwoThirds, canvasHeight };
        windowCtrlPoints[BOTTOM_ONE_THIRD] = { widthOneThird, canvasHeight };
        windowCtrlPoints[BOTTOM_LEFT] = { 0.0f, canvasHeight };
        windowCtrlPoints[LEFT_TWO_THIRDS] = { 0.0f, heightTwoThirds };
        windowCtrlPoints[LEFT_ONE_THIRD] = { 0.0f, heightOneThird };
    }
}

void RSAttractionEffectFilter::CalculateDeltaXAndDeltaY(const Drawing::Point windowCtrlPoints[],
    const Drawing::Point &pointDst, float &deltaX, float &deltaY, int location)
{
    int topLeftIndex = 0;
    int topRightIndex = 3;
    int bottomRightIndex = 6;
    int bottomLeftIndex = 9;
    Drawing::Point windowCenter = { 0.5 * canvasWidth_, canvasHeight_ };
    if (windowCenter.GetY() > pointDst.GetY()) {
        if (location == 1.0f) {
            deltaX =
                std::abs((windowCtrlPoints[topLeftIndex].GetY() +  windowCtrlPoints[bottomLeftIndex].GetY()) / 2.0f -
                pointDst.GetY());
            deltaY = std::abs(windowCtrlPoints[topLeftIndex].GetX() - pointDst.GetX());
        } else {
            deltaX =
                std::abs((windowCtrlPoints[topRightIndex].GetY() +  windowCtrlPoints[bottomRightIndex].GetY()) / 2.0f -
                pointDst.GetY());
            deltaY = std::abs(windowCtrlPoints[topRightIndex].GetX() - pointDst.GetX());
        }
    } else {
        deltaX = std::abs(windowCenter.GetX() - pointDst.GetX());
        deltaY = std::abs(windowCenter.GetY() - pointDst.GetY());
    }
}

std::vector<Drawing::Point> RSAttractionEffectFilter::CalculateUpperCtrlPointOfVertex(float deltaX, float deltaY,
    float width, float height, int location)
{
    // Coordinates of the upper control point of the curve:(k1 * width + k2 * deltaX, k3 * height + k4 * deltaY)
    Drawing::Point topLeft = { (0.016f * width - 0.08f * deltaX) * location, 0.464f * height + 0.40f * deltaY };
    Drawing::Point topRight = { (-1.147f * width - 0.016f * deltaX) * location, -0.187f * height + 0.30f * deltaY };
    Drawing::Point bottomLeft = { (-0.15f * width - 0.075f * deltaX) * location, 0.0f * height + 0.2f * deltaY };
    Drawing::Point bottomRight = { (-0.84f * width - 0.2f * deltaX) * location, -0.859f * height - 0.2f * deltaY };
    std::vector<Drawing::Point> upperControlPoint = { topLeft, topRight, bottomLeft, bottomRight };
    return upperControlPoint;
}

std::vector<Drawing::Point> RSAttractionEffectFilter::CalculateLowerCtrlPointOfVertex(float deltaX, float deltaY,
    float width, float height, int location)
{
    if (width == 0.0f) {
        width = 1.0f;
    }
    // Coordinates of the lower control point of the curve:(m1*(deltaX * height/width - width)), m2 * deltaY)
    Drawing::Point topLeft = { (0.3f * (deltaX * height / width - width)) * location, -0.20f * deltaY };
    Drawing::Point topRight = { (0.45f * (deltaX * height / width - width)) * location, -0.30f * deltaY };
    Drawing::Point bottomLeft = { (0.15f * (deltaX * height / width - width)) * location, -0.20f * deltaY };
    Drawing::Point bottomRight = { (0.30f * (deltaX * height / width - width)) * location, -0.112f * deltaY };
    std::vector<Drawing::Point> lowerControlPoint = { topLeft, topRight, bottomLeft, bottomRight };
    return lowerControlPoint;
}

std::vector<Drawing::Point> RSAttractionEffectFilter::CalculateVelocityCtrlPointUpper()
{
    Drawing::Point topVelFirst = { 0.50f, 0.0f };
    Drawing::Point bottomVelFirst = { 0.20f, 0.0f };
    std::vector<Drawing::Point> velocityCtrlPoint = {topVelFirst, bottomVelFirst};
    return velocityCtrlPoint;
}

std::vector<Drawing::Point> RSAttractionEffectFilter::CalculateVelocityCtrlPointLower()
{
    Drawing::Point topVelSecond = { 0.50f, 1.0f };
    Drawing::Point bottomVelSecond = { 0.20f, 1.0f };
    std::vector<Drawing::Point> velocityCtrlPoint = {topVelSecond, bottomVelSecond};
    return velocityCtrlPoint;
}

void RSAttractionEffectFilter::CalculateWindowStatus(float canvasWidth, float canvasHeight, Vector2f destinationPoint)
{
    canvasWidth_ = canvasWidth;
    canvasHeight_ = canvasHeight;
    const int pointNum = 12;

    Drawing::Point windowCtrlPoints[pointNum];
    GetWindowInitCtrlPoints(windowCtrlPoints, canvasWidth_, canvasHeight_, pointNum);

    Drawing::Point pointDst[1] = { { destinationPoint.x_, destinationPoint.y_ } };

    Drawing::Point windowBottomCenter = { 0.5 * canvasWidth_, canvasHeight_ };

    // 1.0 indicates that the window is to the right of the target point,
    // and - 1.0 indicates that the window is to the left.
    float location = (windowBottomCenter.GetX() > pointDst[0].GetX()) ? 1.0f : -1.0f;
    bool isBelowTarget = (windowBottomCenter.GetY() > pointDst[0].GetY()) ? true : false;

    float width = isBelowTarget ? canvasHeight_ : canvasWidth_;
    float height = isBelowTarget ? canvasWidth_ : canvasHeight_;
    float deltaX = 0.0f;
    float deltaY = 0.0f;
    CalculateDeltaXAndDeltaY(windowCtrlPoints, pointDst[0], deltaX, deltaY, location);

    std::vector<Drawing::Point> upperControlPointOfVertex =
        CalculateUpperCtrlPointOfVertex(deltaX, deltaY, width, height, location);
    std::vector<Drawing::Point> lowerControlPointOfVertex =
        CalculateLowerCtrlPointOfVertex(deltaX, deltaY, width, height, location);
    std::vector<Drawing::Point> velocityCtrlPointUpper = CalculateVelocityCtrlPointUpper();
    std::vector<Drawing::Point> velocityCtrlPointLower = CalculateVelocityCtrlPointLower();

    std::vector<Drawing::Point> controlPointListFirst =
        CalculateCubicsCtrlPoint(upperControlPointOfVertex, windowCtrlPoints, location, isBelowTarget, true);
    std::vector<Drawing::Point> controlPointListSecond =
        CalculateCubicsCtrlPoint(lowerControlPointOfVertex, pointDst, location, isBelowTarget, false);

    std::vector<Drawing::Point> speedListsFirst(pointNum, Drawing::Point(0.0f, 0.0f));
    std::vector<Drawing::Point> speedListsSecond(pointNum, Drawing::Point(0.0f, 0.0f));
    CalculateBezierVelList(velocityCtrlPointUpper, speedListsFirst, location, isBelowTarget);
    CalculateBezierVelList(velocityCtrlPointLower, speedListsSecond, location, isBelowTarget);

    for (int i = 0; i < pointNum; ++i) {
        float speed = BinarySearch(attractionFraction_, speedListsFirst[i], speedListsSecond[i]);
        windowStatusPoints_[i] = CubicBezier(windowCtrlPoints[i], controlPointListFirst[i], controlPointListSecond[i],
            pointDst[0], speed);
    }
}

Drawing::Brush RSAttractionEffectFilter::GetBrush(const std::shared_ptr<Drawing::Image>& image) const
{
    Drawing::Brush brush;
    brush.SetBlendMode(Drawing::BlendMode::SRC_OVER);
    Drawing::SamplingOptions samplingOptions;
    Drawing::Matrix scaleMat;
    brush.SetShaderEffect(Drawing::ShaderEffect::CreateImageShader(
        *image, Drawing::TileMode::CLAMP, Drawing::TileMode::CLAMP, samplingOptions, scaleMat));
    return brush;
}

void RSAttractionEffectFilter::DrawImageRect(Drawing::Canvas& canvas, const std::shared_ptr<Drawing::Image>& image,
    const Drawing::Rect& src, const Drawing::Rect& dst) const
{
    if (!attractionFraction_ || !image || image->GetWidth() == 0 || image->GetHeight() == 0) {
        ROSEN_LOGE("RSAttractionEffectFilter::shader error");
        return;
    }
    RS_OPTIONAL_TRACE_NAME_FMT("DrawAttraction:%f", attractionFraction_);
    int width = image->GetWidth();
    int height = image->GetHeight();

    auto brush = GetBrush(image);
    canvas.AttachBrush(brush);

    // 4 coordinates of image texture
    const Drawing::Point texCoords[4] = { { 0.0f, 0.0f }, { width, 0.0f }, { width, height }, { 0.0f, height } };

    canvas.DrawPatch(windowStatusPoints_, nullptr, texCoords, Drawing::BlendMode::SRC_OVER);
    canvas.DetachBrush();
}
} // namespace Rosen
} // namespace OHOS