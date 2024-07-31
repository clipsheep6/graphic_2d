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

#include "drawcmd_fuzzer.h"

#include <cstddef>
#include <cstdint>

#include "get_object.h"
#include "recording/draw_cmd.h"
#include "recording/draw_cmd_list.h"

namespace OHOS {
namespace Rosen {
namespace {
constexpr size_t BLENDMODE_SIZE = 29;
constexpr size_t MATH_TWO = 2;
constexpr size_t MAX_SIZE = 5000;
constexpr size_t PIONTMODE_SIZE = 3;
constexpr size_t SHADOWFLAG_SIZE = 4;
constexpr size_t VERTEXMODE_SIZE = 3;
} // namespace
namespace Drawing {
bool DrawCmdFuzzTest001(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    BrushHandle brushHandle;
    brushHandle.colorFilterHandle.size = GetObject<size_t>();
    brushHandle.colorSpaceHandle.size = GetObject<size_t>();
    brushHandle.shaderEffectHandle.size = GetObject<size_t>();
    brushHandle.imageFilterHandle.size = GetObject<size_t>();
    brushHandle.maskFilterHandle.size = GetObject<size_t>();
    size_t length = GetObject<size_t>() % MAX_SIZE + 1;
    char* dataText = new char[length];
    for (size_t i = 0; i < length; i++) {
        dataText[i] = GetObject<char>();
    }
    dataText[length - 1] = '\0';
    std::pair<const void*, size_t> cmdListData;
    cmdListData.first = static_cast<const void*>(dataText);
    cmdListData.second = length;
    auto drawCmdList = DrawCmdList::CreateFromData(cmdListData, false);
    Brush brush;
    DrawOpItem::BrushHandleToBrush(brushHandle, *drawCmdList, brush);
    if (dataText != nullptr) {
        delete [] dataText;
        dataText = nullptr;
    }
    return true;
}

bool DrawCmdFuzzTest002(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    BrushHandle brushHandle;
    brushHandle.colorFilterHandle.size = GetObject<size_t>();
    brushHandle.colorSpaceHandle.size = GetObject<size_t>();
    brushHandle.shaderEffectHandle.size = GetObject<size_t>();
    brushHandle.imageFilterHandle.size = GetObject<size_t>();
    brushHandle.maskFilterHandle.size = GetObject<size_t>();
    size_t length = GetObject<size_t>() % MAX_SIZE + 1;
    char* dataText = new char[length];
    for (size_t i = 0; i < length; i++) {
        dataText[i] = GetObject<char>();
    }
    dataText[length - 1] = '\0';
    std::pair<const void*, size_t> cmdListData;
    cmdListData.first = static_cast<const void*>(dataText);
    cmdListData.second = length;
    auto drawCmdList = DrawCmdList::CreateFromData(cmdListData, false);
    Brush brush;
    DrawOpItem::BrushToBrushHandle(brush, *drawCmdList, brushHandle);
    if (dataText != nullptr) {
        delete [] dataText;
        dataText = nullptr;
    }
    return true;
}
bool DrawCmdFuzzTest003(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    PaintHandle paintHandle;
    paintHandle.isAntiAlias = GetObject<bool>();
    paintHandle.width = GetObject<scalar>();
    size_t length = GetObject<size_t>() % MAX_SIZE + 1;
    char* dataText = new char[length];
    for (size_t i = 0; i < length; i++) {
        dataText[i] = GetObject<char>();
    }
    dataText[length - 1] = '\0';
    std::pair<const void*, size_t> cmdListData;
    cmdListData.first = static_cast<const void*>(dataText);
    cmdListData.second = length;
    auto drawCmdList = DrawCmdList::CreateFromData(cmdListData, false);
    uint32_t alpha = GetObject<uint32_t>();
    uint32_t red = GetObject<uint32_t>();
    uint32_t blue = GetObject<uint32_t>();
    uint32_t green = GetObject<uint32_t>();
    Color color = Color(red, green, blue, alpha);
    Paint paint = Paint(color);
    DrawOpItem::GeneratePaintFromHandle(paintHandle, *drawCmdList, paint);
    if (dataText != nullptr) {
        delete [] dataText;
        dataText = nullptr;
    }
    return true;
}

bool DrawCmdFuzzTest004(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    PaintHandle paintHandle;
    paintHandle.isAntiAlias = GetObject<bool>();
    paintHandle.width = GetObject<scalar>();
    size_t length = GetObject<size_t>() % MAX_SIZE + 1;
    char* dataText = new char[length];
    for (size_t i = 0; i < length; i++) {
        dataText[i] = GetObject<char>();
    }
    dataText[length - 1] = '\0';
    std::pair<const void*, size_t> cmdListData;
    cmdListData.first = static_cast<const void*>(dataText);
    cmdListData.second = length;
    auto drawCmdList = DrawCmdList::CreateFromData(cmdListData, false);
    uint32_t alpha = GetObject<uint32_t>();
    uint32_t red = GetObject<uint32_t>();
    uint32_t blue = GetObject<uint32_t>();
    uint32_t green = GetObject<uint32_t>();
    Color color = Color(red, green, blue, alpha);
    Paint paint = Paint(color);
    DrawOpItem::GenerateHandleFromPaint(*drawCmdList, paint, paintHandle);
    if (dataText != nullptr) {
        delete [] dataText;
        dataText = nullptr;
    }
    return true;
}

bool DrawCmdFuzzTest005(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    size_t length = GetObject<size_t>() % MAX_SIZE + 1;
    char* dataText = new char[length];
    for (size_t i = 0; i < length; i++) {
        dataText[i] = GetObject<char>();
    }
    dataText[length - 1] = '\0';
    std::pair<const void*, size_t> cmdListData;
    cmdListData.first = static_cast<const void*>(dataText);
    cmdListData.second = length;
    auto drawCmdList = DrawCmdList::CreateFromData(cmdListData, false);
    uint32_t type = GetObject<uint32_t>();
    uint32_t alpha = GetObject<uint32_t>();
    uint32_t red = GetObject<uint32_t>();
    uint32_t blue = GetObject<uint32_t>();
    uint32_t green = GetObject<uint32_t>();
    Color color = Color(red, green, blue, alpha);
    Paint paint = Paint(color);
    DrawWithPaintOpItem drawWithPaintOpItem = DrawWithPaintOpItem(paint, type);
    drawWithPaintOpItem.Marshalling(*drawCmdList);
    if (dataText != nullptr) {
        delete [] dataText;
        dataText = nullptr;
    }
    return true;
}

bool DrawCmdFuzzTest006(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    size_t length = GetObject<size_t>() % MAX_SIZE + 1;
    char* dataText = new char[length];
    for (size_t i = 0; i < length; i++) {
        dataText[i] = GetObject<char>();
    }
    dataText[length - 1] = '\0';
    std::pair<const void*, size_t> cmdListData;
    cmdListData.first = static_cast<const void*>(dataText);
    cmdListData.second = length;
    auto drawCmdList = DrawCmdList::CreateFromData(cmdListData, false);
    uint32_t type = GetObject<uint32_t>();
    uint32_t alpha = GetObject<uint32_t>();
    uint32_t red = GetObject<uint32_t>();
    uint32_t blue = GetObject<uint32_t>();
    uint32_t green = GetObject<uint32_t>();
    Color color = Color(red, green, blue, alpha);
    Paint paint = Paint(color);
    PaintHandle paintHandle;
    paintHandle.isAntiAlias = GetObject<bool>();
    paintHandle.width = GetObject<scalar>();
    DrawWithPaintOpItem drawWithPaintOpItem = DrawWithPaintOpItem(*drawCmdList, paintHandle, type);
    int32_t width = GetObject<int32_t>();
    int32_t height = GetObject<int32_t>();
    Canvas canvas = Canvas(width, height);
    Rect rect { GetObject<float>(), GetObject<float>(), GetObject<float>(), GetObject<float>() };
    drawWithPaintOpItem.Playback(&canvas, &rect);
    if (dataText != nullptr) {
        delete [] dataText;
        dataText = nullptr;
    }
    return true;
}

bool DrawCmdFuzzTest007(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    size_t length = GetObject<size_t>() % MAX_SIZE + 1;
    char* dataText = new char[length];
    for (size_t i = 0; i < length; i++) {
        dataText[i] = GetObject<char>();
    }
    dataText[length - 1] = '\0';
    std::pair<const void*, size_t> cmdListData;
    cmdListData.first = static_cast<const void*>(dataText);
    cmdListData.second = length;
    auto drawCmdList = DrawCmdList::CreateFromData(cmdListData, false);
    PaintHandle paintHandle;
    paintHandle.isAntiAlias = GetObject<bool>();
    paintHandle.width = GetObject<scalar>();
    int32_t x = GetObject<int32_t>();
    int32_t y = GetObject<int32_t>();
    Point point { x, y };
    DrawPointOpItem::ConstructorHandle constructorHandle = DrawPointOpItem::ConstructorHandle(point, paintHandle);
    DrawPointOpItem drawPointOpItem = DrawPointOpItem(*drawCmdList, &constructorHandle);
    drawPointOpItem.Marshalling(*drawCmdList);
    DrawPointOpItem::Unmarshalling(*drawCmdList, &constructorHandle);
    if (dataText != nullptr) {
        delete [] dataText;
        dataText = nullptr;
    }
    return true;
}

bool DrawCmdFuzzTest008(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    int32_t x = GetObject<int32_t>();
    int32_t y = GetObject<int32_t>();
    Point point { x, y };
    uint32_t alpha = GetObject<uint32_t>();
    uint32_t red = GetObject<uint32_t>();
    uint32_t blue = GetObject<uint32_t>();
    uint32_t green = GetObject<uint32_t>();
    Color color = Color(red, green, blue, alpha);
    Paint paint = Paint(color);
    DrawPointOpItem drawPointOpItem = DrawPointOpItem(point, paint);
    int32_t width = GetObject<int32_t>();
    int32_t height = GetObject<int32_t>();
    Canvas canvas = Canvas(width, height);
    Rect rect { GetObject<float>(), GetObject<float>(), GetObject<float>(), GetObject<float>() };
    drawPointOpItem.Playback(&canvas, &rect);
    size_t length = GetObject<size_t>() % MAX_SIZE + 1;
    char* dataText = new char[length];
    for (size_t i = 0; i < length; i++) {
        dataText[i] = GetObject<char>();
    }
    dataText[length - 1] = '\0';
    std::pair<const void*, size_t> cmdListData;
    cmdListData.first = static_cast<const void*>(dataText);
    cmdListData.second = length;
    auto drawCmdList = DrawCmdList::CreateFromData(cmdListData, false);
    return true;
}

bool DrawCmdFuzzTest009(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    std::vector<Point> points;
    scalar ptOneX = GetObject<scalar>();
    scalar ptOneY = GetObject<scalar>();
    scalar ptTwoX = GetObject<scalar>();
    scalar ptTwoY = GetObject<scalar>();
    Point ptOne { ptOneX, ptOneY };
    Point ptTwo { ptTwoX, ptTwoY };
    points.push_back(ptOne);
    points.push_back(ptTwo);
    uint32_t pointMode = GetObject<uint32_t>();
    Color color = Color(GetObject<uint32_t>(), GetObject<uint32_t>(), GetObject<uint32_t>(), GetObject<uint32_t>());
    Paint paint = Paint(color);
    DrawPointsOpItem drawPointsOpItem = DrawPointsOpItem(static_cast<PointMode>(pointMode % PIONTMODE_SIZE), points,
        paint);
    int32_t width = GetObject<int32_t>();
    int32_t height = GetObject<int32_t>();
    Canvas canvas = Canvas(width, height);
    Rect rect { GetObject<float>(), GetObject<float>(), GetObject<float>(), GetObject<float>() };
    drawPointsOpItem.Playback(&canvas, &rect);
    size_t length = GetObject<size_t>() % MAX_SIZE + 1;
    char* dataText = new char[length];
    for (size_t i = 0; i < length; i++) {
        dataText[i] = GetObject<char>();
    }
    dataText[length - 1] = '\0';
    std::pair<const void*, size_t> cmdListData;
    cmdListData.first = static_cast<const void*>(dataText);
    cmdListData.second = length;
    auto drawCmdList = DrawCmdList::CreateFromData(cmdListData, false);
    drawPointsOpItem.Marshalling(*drawCmdList);
    std::pair<uint32_t, size_t> pts = { GetObject<uint32_t>(), GetObject<size_t>() };
    PaintHandle paintHandle;
    paintHandle.isAntiAlias = GetObject<bool>();
    paintHandle.width = GetObject<scalar>();
    DrawPointsOpItem::ConstructorHandle constructorHandle = DrawPointsOpItem::ConstructorHandle(
        static_cast<PointMode>(pointMode % PIONTMODE_SIZE), pts, paintHandle);
    DrawPointsOpItem::Unmarshalling(*drawCmdList, &constructorHandle);
    if (dataText != nullptr) {
        delete [] dataText;
        dataText = nullptr;
    }
    return true;
}

bool DrawCmdFuzzTest010(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    Path path;
    uint32_t strCount = GetObject<uint32_t>() % MAX_SIZE + 1;
    char str[strCount];
    for (size_t i = 0; i < strCount; i++) {
        str[i] =  GetObject<char>();
    }
    str[strCount - 1] = '\0';
    path.BuildFromSVGString(str);
    Color color = Color(GetObject<uint32_t>(), GetObject<uint32_t>(), GetObject<uint32_t>(), GetObject<uint32_t>());
    Paint paint = Paint(color);
    DrawPathOpItem drawPathOpItem = DrawPathOpItem(path, paint);
    int32_t width = GetObject<int32_t>();
    int32_t height = GetObject<int32_t>();
    Canvas canvas = Canvas(width, height);
    Rect rectT { GetObject<float>(), GetObject<float>(), GetObject<float>(), GetObject<float>() };
    drawPathOpItem.Playback(&canvas, &rectT);
    size_t length = GetObject<size_t>() % MAX_SIZE + 1;
    char* dataText = new char[length];
    for (size_t i = 0; i < length; i++) {
        dataText[i] = GetObject<char>();
    }
    dataText[length - 1] = '\0';
    std::pair<const void*, size_t> cmdListData;
    cmdListData.first = static_cast<const void*>(dataText);
    cmdListData.second = length;
    auto drawCmdList = DrawCmdList::CreateFromData(cmdListData, false);
    OpDataHandle pathT { GetObject<uint32_t>(), GetObject<size_t>() };
    PaintHandle paintHandle;
    paintHandle.isAntiAlias = GetObject<bool>();
    paintHandle.width = GetObject<scalar>();
    DrawPathOpItem::ConstructorHandle constructorHandle = DrawPathOpItem::ConstructorHandle(pathT, paintHandle);
    drawPathOpItem.Marshalling(*drawCmdList);
    DrawPathOpItem::Unmarshalling(*drawCmdList, &constructorHandle);
    DrawPathOpItem drawPathOpItemTwo = DrawPathOpItem(*drawCmdList, &constructorHandle);
    if (dataText != nullptr) {
        delete [] dataText;
        dataText = nullptr;
    }
    return true;
}

bool DrawCmdFuzzTest011(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    size_t length = GetObject<size_t>() % MAX_SIZE + 1;
    char* dataText = new char[length];
    for (size_t i = 0; i < length; i++) {
        dataText[i] = GetObject<char>();
    }
    dataText[length - 1] = '\0';
    std::pair<const void*, size_t> cmdListData;
    cmdListData.first = static_cast<const void*>(dataText);
    cmdListData.second = length;
    auto drawCmdList = DrawCmdList::CreateFromData(cmdListData, false);
    BrushHandle brushHandle;
    brushHandle.colorFilterHandle.size = GetObject<size_t>();
    brushHandle.colorSpaceHandle.size = GetObject<size_t>();
    brushHandle.shaderEffectHandle.size = GetObject<size_t>();
    brushHandle.imageFilterHandle.size = GetObject<size_t>();
    brushHandle.maskFilterHandle.size = GetObject<size_t>();
    DrawBackgroundOpItem::ConstructorHandle constructorHandle = DrawBackgroundOpItem::ConstructorHandle(brushHandle);
    DrawBackgroundOpItem drawBackgroundOpItem = DrawBackgroundOpItem(*drawCmdList, &constructorHandle);
    drawBackgroundOpItem.Marshalling(*drawCmdList);
    DrawBackgroundOpItem::Unmarshalling(*drawCmdList, &constructorHandle);
    if (dataText != nullptr) {
        delete [] dataText;
        dataText = nullptr;
    }
    return true;
}

bool DrawCmdFuzzTest012(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    Region region;
    RectI rect { GetObject<float>(), GetObject<float>(), GetObject<float>(), GetObject<float>() };
    region.SetRect(rect);
    uint32_t op = GetObject<uint32_t>();
    ClipRegionOpItem clipRegionOpItem = ClipRegionOpItem(region, static_cast<ClipOp>(op % MATH_TWO));
    int32_t width = GetObject<int32_t>();
    int32_t height = GetObject<int32_t>();
    Canvas canvas = Canvas(width, height);
    Rect rectT { GetObject<float>(), GetObject<float>(), GetObject<float>(), GetObject<float>() };
    clipRegionOpItem.Playback(&canvas, &rectT);
    size_t length = GetObject<size_t>() % MAX_SIZE + 1;
    char* dataText = new char[length];
    for (size_t i = 0; i < length; i++) {
        dataText[i] = GetObject<char>();
    }
    dataText[length - 1] = '\0';
    std::pair<const void*, size_t> cmdListData;
    cmdListData.first = static_cast<const void*>(dataText);
    cmdListData.second = length;
    auto drawCmdList = DrawCmdList::CreateFromData(cmdListData, false);
    OpDataHandle regionT { GetObject<uint32_t>(), GetObject<size_t>() };
    ClipRegionOpItem::ConstructorHandle constructorHandle = ClipRegionOpItem::ConstructorHandle(
        regionT, static_cast<ClipOp>(op % MATH_TWO));
    clipRegionOpItem.Marshalling(*drawCmdList);
    ClipRegionOpItem::Unmarshalling(*drawCmdList, &constructorHandle);
    ClipRegionOpItem clipRegionOpItemT = ClipRegionOpItem(*drawCmdList, &constructorHandle);
    if (dataText != nullptr) {
        delete [] dataText;
        dataText = nullptr;
    }
    return true;
}

bool DrawCmdFuzzTest013(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    Path path;
    uint32_t strCount = GetObject<uint32_t>() % MAX_SIZE + 1;
    char str[strCount];
    for (size_t i = 0; i < strCount; i++) {
        str[i] =  GetObject<char>();
    }
    str[strCount - 1] = '\0';
    path.BuildFromSVGString(str);
    Point3 planeParams = Point3(GetObject<scalar>(), GetObject<scalar>(), GetObject<scalar>());
    Point3 devLightPos = Point3(GetObject<scalar>(), GetObject<scalar>(), GetObject<scalar>());
    scalar lightRadius = GetObject<scalar>();
    Color ambient = Color(GetObject<uint32_t>(), GetObject<uint32_t>(), GetObject<uint32_t>(), GetObject<uint32_t>());
    Color spotColor = Color(GetObject<uint32_t>(), GetObject<uint32_t>(), GetObject<uint32_t>(), GetObject<uint32_t>());
    uint32_t flag = GetObject<uint32_t>();
    DrawShadowOpItem drawShadowOpItem = DrawShadowOpItem(path, planeParams, devLightPos, lightRadius, ambient,
        spotColor, static_cast<ShadowFlags>(flag % SHADOWFLAG_SIZE));
    int32_t width = GetObject<int32_t>();
    int32_t height = GetObject<int32_t>();
    Canvas canvas = Canvas(width, height);
    Rect rect { GetObject<float>(), GetObject<float>(), GetObject<float>(), GetObject<float>() };
    drawShadowOpItem.Playback(&canvas, &rect);
    size_t length = GetObject<size_t>() % MAX_SIZE + 1;
    char* dataText = new char[length];
    for (size_t i = 0; i < length; i++) {
        dataText[i] = GetObject<char>();
    }
    dataText[length - 1] = '\0';
    std::pair<const void*, size_t> cmdListData;
    cmdListData.first = static_cast<const void*>(dataText);
    cmdListData.second = length;
    auto drawCmdList = DrawCmdList::CreateFromData(cmdListData, false);
    OpDataHandle pathT { GetObject<uint32_t>(), GetObject<size_t>() };
    DrawShadowOpItem::ConstructorHandle constructorHandle = DrawShadowOpItem::ConstructorHandle(pathT, planeParams,
        devLightPos, lightRadius, ambient, spotColor, static_cast<ShadowFlags>(flag % SHADOWFLAG_SIZE));
    drawShadowOpItem.Marshalling(*drawCmdList);
    DrawShadowOpItem::Unmarshalling(*drawCmdList, &constructorHandle);
    DrawShadowOpItem drawShadowOpItemT = DrawShadowOpItem(*drawCmdList, &constructorHandle);
    if (dataText != nullptr) {
        delete [] dataText;
        dataText = nullptr;
    }
    return true;
}

bool DrawCmdFuzzTest014(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    uint32_t alpha = GetObject<uint32_t>();
    uint32_t red = GetObject<uint32_t>();
    uint32_t blue = GetObject<uint32_t>();
    uint32_t green = GetObject<uint32_t>();
    Color color = Color(red, green, blue, alpha);
    Brush brush = Brush(color);
    DrawBackgroundOpItem drawBackgroundOpItem = DrawBackgroundOpItem(brush);
    int32_t width = GetObject<int32_t>();
    int32_t height = GetObject<int32_t>();
    Canvas canvas = Canvas(width, height);
    Rect rect { GetObject<float>(), GetObject<float>(), GetObject<float>(), GetObject<float>() };
    drawBackgroundOpItem.Playback(&canvas, &rect);
    return true;
}

bool DrawCmdFuzzTest015(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    size_t length = GetObject<size_t>() % MAX_SIZE + 1;
    char* dataText = new char[length];
    for (size_t i = 0; i < length; i++) {
        dataText[i] = GetObject<char>();
    }
    dataText[length - 1] = '\0';
    std::pair<const void*, size_t> cmdListData;
    cmdListData.first = static_cast<const void*>(dataText);
    cmdListData.second = length;
    auto drawCmdList = DrawCmdList::CreateFromData(cmdListData, false);
    scalar startPtX = GetObject<scalar>();
    scalar startPtY = GetObject<scalar>();
    scalar endPtX = GetObject<scalar>();
    scalar endPtY = GetObject<scalar>();
    Point startPt { startPtX, startPtY };
    Point endPt { endPtX, endPtY };
    PaintHandle paintHandle;
    paintHandle.isAntiAlias = GetObject<bool>();
    paintHandle.width = GetObject<scalar>();
    DrawLineOpItem::ConstructorHandle constructorHandle = DrawLineOpItem::ConstructorHandle(
        startPt, endPt, paintHandle);
    DrawLineOpItem drawLineOpItem = DrawLineOpItem(*drawCmdList, &constructorHandle);
    drawLineOpItem.Marshalling(*drawCmdList);
    DrawLineOpItem::Unmarshalling(*drawCmdList, &constructorHandle);
    if (dataText != nullptr) {
        delete [] dataText;
        dataText = nullptr;
    }
    return true;
}

bool DrawCmdFuzzTest016(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    scalar startPtX = GetObject<scalar>();
    scalar startPtY = GetObject<scalar>();
    scalar endPtX = GetObject<scalar>();
    scalar endPtY = GetObject<scalar>();
    Point startPt { startPtX, startPtY };
    Point endPt { endPtX, endPtY };
    uint32_t alpha = GetObject<uint32_t>();
    uint32_t red = GetObject<uint32_t>();
    uint32_t blue = GetObject<uint32_t>();
    uint32_t green = GetObject<uint32_t>();
    Color color = Color(red, green, blue, alpha);
    Paint paint = Paint(color);
    DrawLineOpItem drawLineOpItem = DrawLineOpItem(startPt, endPt, paint);
    int32_t width = GetObject<int32_t>();
    int32_t height = GetObject<int32_t>();
    Canvas canvas = Canvas(width, height);
    Rect rect { GetObject<float>(), GetObject<float>(), GetObject<float>(), GetObject<float>() };
    drawLineOpItem.Playback(&canvas, &rect);
    return true;
}

bool DrawCmdFuzzTest017(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    size_t length = GetObject<size_t>() % MAX_SIZE + 1;
    char* dataText = new char[length];
    for (size_t i = 0; i < length; i++) {
        dataText[i] = GetObject<char>();
    }
    dataText[length - 1] = '\0';
    std::pair<const void*, size_t> cmdListData;
    cmdListData.first = static_cast<const void*>(dataText);
    cmdListData.second = length;
    auto drawCmdList = DrawCmdList::CreateFromData(cmdListData, false);
    Rect rect { GetObject<float>(), GetObject<float>(), GetObject<float>(), GetObject<float>() };
    PaintHandle paintHandle;
    paintHandle.isAntiAlias = GetObject<bool>();
    paintHandle.width = GetObject<scalar>();
    DrawRectOpItem::ConstructorHandle constructorHandle = DrawRectOpItem::ConstructorHandle(rect, paintHandle);
    DrawRectOpItem drawRectOpItem = DrawRectOpItem(*drawCmdList, &constructorHandle);
    drawRectOpItem.Marshalling(*drawCmdList);
    DrawRectOpItem::Unmarshalling(*drawCmdList, &constructorHandle);
    if (dataText != nullptr) {
        delete [] dataText;
        dataText = nullptr;
    }
    return true;
}

bool DrawCmdFuzzTest018(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    Rect rect { GetObject<float>(), GetObject<float>(), GetObject<float>(), GetObject<float>() };
    uint32_t alpha = GetObject<uint32_t>();
    uint32_t red = GetObject<uint32_t>();
    uint32_t blue = GetObject<uint32_t>();
    uint32_t green = GetObject<uint32_t>();
    Color color = Color(red, green, blue, alpha);
    Paint paint = Paint(color);
    DrawRectOpItem drawRectOpItem = DrawRectOpItem(rect, paint);
    int32_t width = GetObject<int32_t>();
    int32_t height = GetObject<int32_t>();
    Canvas canvas = Canvas(width, height);
    Rect rectT { GetObject<float>(), GetObject<float>(), GetObject<float>(), GetObject<float>() };
    drawRectOpItem.Playback(&canvas, &rectT);
    return true;
}

bool DrawCmdFuzzTest019(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    size_t length = GetObject<size_t>() % MAX_SIZE + 1;
    char* dataText = new char[length];
    for (size_t i = 0; i < length; i++) {
        dataText[i] = GetObject<char>();
    }
    dataText[length - 1] = '\0';
    std::pair<const void*, size_t> cmdListData;
    cmdListData.first = static_cast<const void*>(dataText);
    cmdListData.second = length;
    auto drawCmdList = DrawCmdList::CreateFromData(cmdListData, false);
    Rect rect { GetObject<float>(), GetObject<float>(), GetObject<float>(), GetObject<float>() };
    scalar xRad = GetObject<scalar>();
    scalar yRad = GetObject<scalar>();
    RoundRect roundRect = RoundRect(rect, xRad, yRad);
    PaintHandle paintHandle;
    paintHandle.isAntiAlias = GetObject<bool>();
    paintHandle.width = GetObject<scalar>();
    DrawRoundRectOpItem::ConstructorHandle constructorHandle = DrawRoundRectOpItem::ConstructorHandle(
        roundRect, paintHandle);
    DrawRoundRectOpItem drawRoundRectOpItem = DrawRoundRectOpItem(*drawCmdList, &constructorHandle);
    drawRoundRectOpItem.Marshalling(*drawCmdList);
    DrawRoundRectOpItem::Unmarshalling(*drawCmdList, &constructorHandle);
    if (dataText != nullptr) {
        delete [] dataText;
        dataText = nullptr;
    }
    return true;
}

bool DrawCmdFuzzTest020(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    Rect rect { GetObject<float>(), GetObject<float>(), GetObject<float>(), GetObject<float>() };
    scalar xRad = GetObject<scalar>();
    scalar yRad = GetObject<scalar>();
    RoundRect roundRect = RoundRect(rect, xRad, yRad);
    uint32_t alpha = GetObject<uint32_t>();
    uint32_t red = GetObject<uint32_t>();
    uint32_t blue = GetObject<uint32_t>();
    uint32_t green = GetObject<uint32_t>();
    Color color = Color(red, green, blue, alpha);
    Paint paint = Paint(color);
    DrawRoundRectOpItem drawRoundRectOpItem = DrawRoundRectOpItem(roundRect, paint);
    int32_t width = GetObject<int32_t>();
    int32_t height = GetObject<int32_t>();
    Canvas canvas = Canvas(width, height);
    Rect rectT { GetObject<float>(), GetObject<float>(), GetObject<float>(), GetObject<float>() };
    drawRoundRectOpItem.Playback(&canvas, &rectT);
    return true;
}

bool DrawCmdFuzzTest021(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    size_t length = GetObject<size_t>() % MAX_SIZE + 1;
    char* dataText = new char[length];
    for (size_t i = 0; i < length; i++) {
        dataText[i] = GetObject<char>();
    }
    dataText[length - 1] = '\0';
    std::pair<const void*, size_t> cmdListData;
    cmdListData.first = static_cast<const void*>(dataText);
    cmdListData.second = length;
    auto drawCmdList = DrawCmdList::CreateFromData(cmdListData, false);
    Rect rect { GetObject<float>(), GetObject<float>(), GetObject<float>(), GetObject<float>() };
    Rect otherRect { GetObject<float>(), GetObject<float>(), GetObject<float>(), GetObject<float>() };
    scalar xRad = GetObject<scalar>();
    scalar yRad = GetObject<scalar>();
    RoundRect innerRRect = RoundRect(rect, xRad, yRad);
    scalar xRadT = GetObject<scalar>();
    scalar yRadT = GetObject<scalar>();
    RoundRect outerRRect = RoundRect(otherRect, xRadT, yRadT);
    PaintHandle paintHandle;
    paintHandle.isAntiAlias = GetObject<bool>();
    paintHandle.width = GetObject<scalar>();
    DrawNestedRoundRectOpItem::ConstructorHandle constructorHandle = DrawNestedRoundRectOpItem::ConstructorHandle(
        outerRRect, innerRRect, paintHandle);
    DrawNestedRoundRectOpItem drawNestedRoundRectOpItem = DrawNestedRoundRectOpItem(*drawCmdList, &constructorHandle);
    drawNestedRoundRectOpItem.Marshalling(*drawCmdList);
    DrawNestedRoundRectOpItem::Unmarshalling(*drawCmdList, &constructorHandle);
    if (dataText != nullptr) {
        delete [] dataText;
        dataText = nullptr;
    }
    return true;
}

bool DrawCmdFuzzTest022(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    Rect rect { GetObject<float>(), GetObject<float>(), GetObject<float>(), GetObject<float>() };
    Rect otherRect { GetObject<float>(), GetObject<float>(), GetObject<float>(), GetObject<float>() };
    scalar xRad = GetObject<scalar>();
    scalar yRad = GetObject<scalar>();
    RoundRect outer = RoundRect(rect, xRad, yRad);
    scalar xRadT = GetObject<scalar>();
    scalar yRadT = GetObject<scalar>();
    RoundRect inner = RoundRect(otherRect, xRadT, yRadT);
    uint32_t alpha = GetObject<uint32_t>();
    uint32_t red = GetObject<uint32_t>();
    uint32_t blue = GetObject<uint32_t>();
    uint32_t green = GetObject<uint32_t>();
    Color color = Color(red, green, blue, alpha);
    Paint paint = Paint(color);
    DrawNestedRoundRectOpItem drawNestedRoundRectOpItem = DrawNestedRoundRectOpItem(outer, inner, paint);
    int32_t width = GetObject<int32_t>();
    int32_t height = GetObject<int32_t>();
    Canvas canvas = Canvas(width, height);
    Rect rectT { GetObject<float>(), GetObject<float>(), GetObject<float>(), GetObject<float>() };
    drawNestedRoundRectOpItem.Playback(&canvas, &rectT);
    return true;
}

bool DrawCmdFuzzTest023(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    size_t length = GetObject<size_t>() % MAX_SIZE + 1;
    char* dataText = new char[length];
    for (size_t i = 0; i < length; i++) {
        dataText[i] = GetObject<char>();
    }
    dataText[length - 1] = '\0';
    std::pair<const void*, size_t> cmdListData;
    cmdListData.first = static_cast<const void*>(dataText);
    cmdListData.second = length;
    auto drawCmdList = DrawCmdList::CreateFromData(cmdListData, false);
    Rect rect { GetObject<float>(), GetObject<float>(), GetObject<float>(), GetObject<float>() };
    scalar startAngle = GetObject<scalar>();
    scalar sweepAngle = GetObject<scalar>();
    PaintHandle paintHandle;
    paintHandle.isAntiAlias = GetObject<bool>();
    paintHandle.width = GetObject<scalar>();
    DrawArcOpItem::ConstructorHandle constructorHandle = DrawArcOpItem::ConstructorHandle(
        rect, startAngle, sweepAngle, paintHandle);
    DrawArcOpItem drawArcOpItem = DrawArcOpItem(*drawCmdList, &constructorHandle);
    drawArcOpItem.Marshalling(*drawCmdList);
    DrawArcOpItem::Unmarshalling(*drawCmdList, &constructorHandle);
    if (dataText != nullptr) {
        delete [] dataText;
        dataText = nullptr;
    }
    return true;
}

bool DrawCmdFuzzTest024(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    Rect rect { GetObject<float>(), GetObject<float>(), GetObject<float>(), GetObject<float>() };
    scalar startAngle = GetObject<scalar>();
    scalar sweepAngle = GetObject<scalar>();
    uint32_t alpha = GetObject<uint32_t>();
    uint32_t red = GetObject<uint32_t>();
    uint32_t blue = GetObject<uint32_t>();
    uint32_t green = GetObject<uint32_t>();
    Color color = Color(red, green, blue, alpha);
    Paint paint = Paint(color);
    DrawArcOpItem drawArcOpItem = DrawArcOpItem(rect, startAngle, sweepAngle, paint);
    int32_t width = GetObject<int32_t>();
    int32_t height = GetObject<int32_t>();
    Canvas canvas = Canvas(width, height);
    Rect rectT { GetObject<float>(), GetObject<float>(), GetObject<float>(), GetObject<float>() };
    drawArcOpItem.Playback(&canvas, &rectT);
    return true;
}

bool DrawCmdFuzzTest025(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    size_t length = GetObject<size_t>() % MAX_SIZE + 1;
    char* dataText = new char[length];
    for (size_t i = 0; i < length; i++) {
        dataText[i] = GetObject<char>();
    }
    dataText[length - 1] = '\0';
    std::pair<const void*, size_t> cmdListData;
    cmdListData.first = static_cast<const void*>(dataText);
    cmdListData.second = length;
    auto drawCmdList = DrawCmdList::CreateFromData(cmdListData, false);
    Rect rect { GetObject<float>(), GetObject<float>(), GetObject<float>(), GetObject<float>() };
    scalar startAngle = GetObject<scalar>();
    scalar sweepAngle = GetObject<scalar>();
    PaintHandle paintHandle;
    paintHandle.isAntiAlias = GetObject<bool>();
    paintHandle.width = GetObject<scalar>();
    DrawPieOpItem::ConstructorHandle constructorHandle = DrawPieOpItem::ConstructorHandle(
        rect, startAngle, sweepAngle, paintHandle);
    DrawPieOpItem drawPieOpItem = DrawPieOpItem(*drawCmdList, &constructorHandle);
    drawPieOpItem.Marshalling(*drawCmdList);
    DrawPieOpItem::Unmarshalling(*drawCmdList, &constructorHandle);
    if (dataText != nullptr) {
        delete [] dataText;
        dataText = nullptr;
    }
    return true;
}

bool DrawCmdFuzzTest026(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    Rect rect { GetObject<float>(), GetObject<float>(), GetObject<float>(), GetObject<float>() };
    scalar startAngle = GetObject<scalar>();
    scalar sweepAngle = GetObject<scalar>();
    uint32_t alpha = GetObject<uint32_t>();
    uint32_t red = GetObject<uint32_t>();
    uint32_t blue = GetObject<uint32_t>();
    uint32_t green = GetObject<uint32_t>();
    Color color = Color(red, green, blue, alpha);
    Paint paint = Paint(color);
    DrawPieOpItem drawPieOpItem = DrawPieOpItem(rect, startAngle, sweepAngle, paint);
    int32_t width = GetObject<int32_t>();
    int32_t height = GetObject<int32_t>();
    Canvas canvas = Canvas(width, height);
    Rect rectT { GetObject<float>(), GetObject<float>(), GetObject<float>(), GetObject<float>() };
    drawPieOpItem.Playback(&canvas, &rectT);
    return true;
}

bool DrawCmdFuzzTest027(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    size_t length = GetObject<size_t>() % MAX_SIZE + 1;
    char* dataText = new char[length];
    for (size_t i = 0; i < length; i++) {
        dataText[i] = GetObject<char>();
    }
    dataText[length - 1] = '\0';
    std::pair<const void*, size_t> cmdListData;
    cmdListData.first = static_cast<const void*>(dataText);
    cmdListData.second = length;
    auto drawCmdList = DrawCmdList::CreateFromData(cmdListData, false);
    Rect rect { GetObject<float>(), GetObject<float>(), GetObject<float>(), GetObject<float>() };
    PaintHandle paintHandle;
    paintHandle.isAntiAlias = GetObject<bool>();
    paintHandle.width = GetObject<scalar>();
    DrawOvalOpItem::ConstructorHandle constructorHandle = DrawOvalOpItem::ConstructorHandle(
        rect, paintHandle);
    DrawOvalOpItem drawOvalOpItem = DrawOvalOpItem(*drawCmdList, &constructorHandle);
    drawOvalOpItem.Marshalling(*drawCmdList);
    DrawOvalOpItem::Unmarshalling(*drawCmdList, &constructorHandle);
    if (dataText != nullptr) {
        delete [] dataText;
        dataText = nullptr;
    }
    return true;
}

bool DrawCmdFuzzTest028(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    Rect rect { GetObject<float>(), GetObject<float>(), GetObject<float>(), GetObject<float>() };
    uint32_t alpha = GetObject<uint32_t>();
    uint32_t red = GetObject<uint32_t>();
    uint32_t blue = GetObject<uint32_t>();
    uint32_t green = GetObject<uint32_t>();
    Color color = Color(red, green, blue, alpha);
    Paint paint = Paint(color);
    DrawOvalOpItem drawOvalOpItem = DrawOvalOpItem(rect, paint);
    int32_t width = GetObject<int32_t>();
    int32_t height = GetObject<int32_t>();
    Canvas canvas = Canvas(width, height);
    Rect rectT { GetObject<float>(), GetObject<float>(), GetObject<float>(), GetObject<float>() };
    drawOvalOpItem.Playback(&canvas, &rectT);
    return true;
}

bool DrawCmdFuzzTest029(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    size_t length = GetObject<size_t>() % MAX_SIZE + 1;
    char* dataText = new char[length];
    for (size_t i = 0; i < length; i++) {
        dataText[i] = GetObject<char>();
    }
    dataText[length - 1] = '\0';
    std::pair<const void*, size_t> cmdListData;
    cmdListData.first = static_cast<const void*>(dataText);
    cmdListData.second = length;
    auto drawCmdList = DrawCmdList::CreateFromData(cmdListData, false);
    int32_t x = GetObject<int32_t>();
    int32_t y = GetObject<int32_t>();
    Point point { x, y };
    scalar radius = GetObject<scalar>();
    PaintHandle paintHandle;
    paintHandle.isAntiAlias = GetObject<bool>();
    paintHandle.width = GetObject<scalar>();
    DrawCircleOpItem::ConstructorHandle constructorHandle = DrawCircleOpItem::ConstructorHandle(
        point, radius, paintHandle);
    DrawCircleOpItem drawCircleOpItem = DrawCircleOpItem(*drawCmdList, &constructorHandle);
    drawCircleOpItem.Marshalling(*drawCmdList);
    DrawCircleOpItem::Unmarshalling(*drawCmdList, &constructorHandle);
    if (dataText != nullptr) {
        delete [] dataText;
        dataText = nullptr;
    }
    return true;
}

bool DrawCmdFuzzTest030(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    int32_t x = GetObject<int32_t>();
    int32_t y = GetObject<int32_t>();
    Point point { x, y };
    scalar radius = GetObject<scalar>();
    uint32_t alpha = GetObject<uint32_t>();
    uint32_t red = GetObject<uint32_t>();
    uint32_t blue = GetObject<uint32_t>();
    uint32_t green = GetObject<uint32_t>();
    Color color = Color(red, green, blue, alpha);
    Paint paint = Paint(color);
    DrawCircleOpItem drawCircleOpItem = DrawCircleOpItem(point, radius, paint);
    int32_t width = GetObject<int32_t>();
    int32_t height = GetObject<int32_t>();
    Canvas canvas = Canvas(width, height);
    Rect rectT { GetObject<float>(), GetObject<float>(), GetObject<float>(), GetObject<float>() };
    drawCircleOpItem.Playback(&canvas, &rectT);
    return true;
}

bool DrawCmdFuzzTest031(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    size_t length = GetObject<size_t>() % MAX_SIZE + 1;
    char* dataText = new char[length];
    for (size_t i = 0; i < length; i++) {
        dataText[i] = GetObject<char>();
    }
    dataText[length - 1] = '\0';
    std::pair<const void*, size_t> cmdListData;
    cmdListData.first = static_cast<const void*>(dataText);
    cmdListData.second = length;
    auto drawCmdList = DrawCmdList::CreateFromData(cmdListData, false);
    scalar deg = GetObject<scalar>();
    scalar sx = GetObject<scalar>();
    scalar sy = GetObject<scalar>();
    RotateOpItem::ConstructorHandle constructorHandle = RotateOpItem::ConstructorHandle(deg, sx, sy);
    RotateOpItem rotateOpItem = RotateOpItem(&constructorHandle);
    rotateOpItem.Marshalling(*drawCmdList);
    RotateOpItem::Unmarshalling(*drawCmdList, &constructorHandle);
    if (dataText != nullptr) {
        delete [] dataText;
        dataText = nullptr;
    }
    return true;
}

bool DrawCmdFuzzTest032(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    scalar deg = GetObject<scalar>();
    scalar sx = GetObject<scalar>();
    scalar sy = GetObject<scalar>();
    RotateOpItem rotateOpItem = RotateOpItem(deg, sx, sy);
    int32_t width = GetObject<int32_t>();
    int32_t height = GetObject<int32_t>();
    Canvas canvas = Canvas(width, height);
    Rect rectT { GetObject<float>(), GetObject<float>(), GetObject<float>(), GetObject<float>() };
    rotateOpItem.Playback(&canvas, &rectT);
    return true;
}

bool DrawCmdFuzzTest033(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    size_t length = GetObject<size_t>() % MAX_SIZE + 1;
    char* dataText = new char[length];
    for (size_t i = 0; i < length; i++) {
        dataText[i] = GetObject<char>();
    }
    dataText[length - 1] = '\0';
    std::pair<const void*, size_t> cmdListData;
    cmdListData.first = static_cast<const void*>(dataText);
    cmdListData.second = length;
    auto drawCmdList = DrawCmdList::CreateFromData(cmdListData, false);
    scalar sx = GetObject<scalar>();
    scalar sy = GetObject<scalar>();
    ShearOpItem::ConstructorHandle constructorHandle = ShearOpItem::ConstructorHandle(sx, sy);
    ShearOpItem shearOpItem = ShearOpItem(&constructorHandle);
    shearOpItem.Marshalling(*drawCmdList);
    ShearOpItem::Unmarshalling(*drawCmdList, &constructorHandle);
    if (dataText != nullptr) {
        delete [] dataText;
        dataText = nullptr;
    }
    return true;
}

bool DrawCmdFuzzTest034(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    scalar sx = GetObject<scalar>();
    scalar sy = GetObject<scalar>();
    ShearOpItem shearOpItem = ShearOpItem(sx, sy);
    int32_t width = GetObject<int32_t>();
    int32_t height = GetObject<int32_t>();
    Canvas canvas = Canvas(width, height);
    Rect rectT { GetObject<float>(), GetObject<float>(), GetObject<float>(), GetObject<float>() };
    shearOpItem.Playback(&canvas, &rectT);
    return true;
}

bool DrawCmdFuzzTest035(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    size_t length = GetObject<size_t>() % MAX_SIZE + 1;
    char* dataText = new char[length];
    for (size_t i = 0; i < length; i++) {
        dataText[i] = GetObject<char>();
    }
    dataText[length - 1] = '\0';
    std::pair<const void*, size_t> cmdListData;
    cmdListData.first = static_cast<const void*>(dataText);
    cmdListData.second = length;
    auto drawCmdList = DrawCmdList::CreateFromData(cmdListData, false);
    FlushOpItem::ConstructorHandle constructorHandle = FlushOpItem::ConstructorHandle();
    FlushOpItem flushOpItem = FlushOpItem();
    flushOpItem.Marshalling(*drawCmdList);
    FlushOpItem::Unmarshalling(*drawCmdList, &constructorHandle);
    if (dataText != nullptr) {
        delete [] dataText;
        dataText = nullptr;
    }
    return true;
}

bool DrawCmdFuzzTest036(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    FlushOpItem flushOpItem = FlushOpItem();
    int32_t width = GetObject<int32_t>();
    int32_t height = GetObject<int32_t>();
    Canvas canvas = Canvas(width, height);
    Rect rectT { GetObject<float>(), GetObject<float>(), GetObject<float>(), GetObject<float>() };
    flushOpItem.Playback(&canvas, &rectT);
    return true;
}

bool DrawCmdFuzzTest037(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    size_t length = GetObject<size_t>() % MAX_SIZE + 1;
    char* dataText = new char[length];
    for (size_t i = 0; i < length; i++) {
        dataText[i] = GetObject<char>();
    }
    dataText[length - 1] = '\0';
    std::pair<const void*, size_t> cmdListData;
    cmdListData.first = static_cast<const void*>(dataText);
    cmdListData.second = length;
    auto drawCmdList = DrawCmdList::CreateFromData(cmdListData, false);
    ColorQuad colorQuad = GetObject<ColorQuad>();
    ClearOpItem::ConstructorHandle constructorHandle = ClearOpItem::ConstructorHandle(colorQuad);
    ClearOpItem clearOpItem = ClearOpItem(&constructorHandle);
    clearOpItem.Marshalling(*drawCmdList);
    ClearOpItem::Unmarshalling(*drawCmdList, &constructorHandle);
    if (dataText != nullptr) {
        delete [] dataText;
        dataText = nullptr;
    }
    return true;
}

bool DrawCmdFuzzTest038(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    ColorQuad colorQuad = GetObject<ColorQuad>();
    ClearOpItem clearOpItem = ClearOpItem(colorQuad);
    int32_t width = GetObject<int32_t>();
    int32_t height = GetObject<int32_t>();
    Canvas canvas = Canvas(width, height);
    Rect rectT { GetObject<float>(), GetObject<float>(), GetObject<float>(), GetObject<float>() };
    clearOpItem.Playback(&canvas, &rectT);
    return true;
}

bool DrawCmdFuzzTest039(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    Region region;
    RectI rect { GetObject<float>(), GetObject<float>(), GetObject<float>(), GetObject<float>() };
    region.SetRect(rect);
    uint32_t alpha = GetObject<uint32_t>();
    uint32_t red = GetObject<uint32_t>();
    uint32_t blue = GetObject<uint32_t>();
    uint32_t green = GetObject<uint32_t>();
    Color color = Color(red, green, blue, alpha);
    Paint paint = Paint(color);
    DrawRegionOpItem drawRegionOpItem = DrawRegionOpItem(region, paint);
    int32_t width = GetObject<int32_t>();
    int32_t height = GetObject<int32_t>();
    Canvas canvas = Canvas(width, height);
    Rect rectT { GetObject<float>(), GetObject<float>(), GetObject<float>(), GetObject<float>() };
    drawRegionOpItem.Playback(&canvas, &rectT);
    size_t length = GetObject<size_t>() % MAX_SIZE + 1;
    char* dataText = new char[length];
    for (size_t i = 0; i < length; i++) {
        dataText[i] = GetObject<char>();
    }
    dataText[length - 1] = '\0';
    std::pair<const void*, size_t> cmdListData;
    cmdListData.first = static_cast<const void*>(dataText);
    cmdListData.second = length;
    auto drawCmdList = DrawCmdList::CreateFromData(cmdListData, false);
    OpDataHandle regionT { GetObject<uint32_t>(), GetObject<size_t>() };
    PaintHandle paintHandle;
    paintHandle.isAntiAlias = GetObject<bool>();
    paintHandle.width = GetObject<scalar>();
    DrawRegionOpItem::ConstructorHandle constructorHandle = DrawRegionOpItem::ConstructorHandle(regionT, paintHandle);
    drawRegionOpItem.Marshalling(*drawCmdList);
    DrawRegionOpItem::Unmarshalling(*drawCmdList, &constructorHandle);
    DrawRegionOpItem drawRegionOpItemTwo = DrawRegionOpItem(*drawCmdList, &constructorHandle);
    if (dataText != nullptr) {
        delete [] dataText;
        dataText = nullptr;
    }
    return true;
}

bool DrawCmdFuzzTest040(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    Vertices vertices;
    uint32_t vertexMode = GetObject<uint32_t>();
    uint32_t arr_size = GetObject<uint32_t>() % MAX_SIZE;
    scalar xRad = GetObject<scalar>();
    scalar yRad = GetObject<scalar>();
    Point positions[arr_size];
    Point texs[arr_size];
    ColorQuad colors[arr_size];
    uint16_t indices[arr_size];
    for (size_t i = 0; i < arr_size; i++) {
        positions[i].Set(xRad, yRad);
        texs[i].Set(xRad, yRad);
        colors[i] = GetObject<ColorQuad>();
        indices[i] = GetObject<uint16_t>();
    }
    vertices.MakeCopy(static_cast<VertexMode>(vertexMode % VERTEXMODE_SIZE), MATH_TWO,
        positions, texs, colors, arr_size, indices);
    uint32_t mode = GetObject<uint32_t>();
    Color color = Color(GetObject<uint32_t>(), GetObject<uint32_t>(), GetObject<uint32_t>(), GetObject<uint32_t>());
    Paint paint = Paint(color);
    DrawVerticesOpItem drawVerticesOpItem = DrawVerticesOpItem(vertices,
        static_cast<BlendMode>(mode % BLENDMODE_SIZE), paint);
    int32_t width = GetObject<int32_t>();
    int32_t height = GetObject<int32_t>();
    Canvas canvas = Canvas(width, height);
    Rect rect { GetObject<float>(), GetObject<float>(), GetObject<float>(), GetObject<float>() };
    drawVerticesOpItem.Playback(&canvas, &rect);
    size_t length = GetObject<size_t>() % MAX_SIZE + 1;
    char* dataText = new char[length];
    for (size_t i = 0; i < length; i++) {
        dataText[i] = GetObject<char>();
    }
    dataText[length - 1] = '\0';
    std::pair<const void*, size_t> cmdListData;
    cmdListData.first = static_cast<const void*>(dataText);
    cmdListData.second = length;
    auto drawCmdList = DrawCmdList::CreateFromData(cmdListData, false);
    OpDataHandle verticesT { GetObject<uint32_t>(), GetObject<size_t>() };
    PaintHandle paintHandle;
    paintHandle.isAntiAlias = GetObject<bool>();
    paintHandle.width = GetObject<scalar>();
    DrawVerticesOpItem::ConstructorHandle constructorHandle = DrawVerticesOpItem::ConstructorHandle(verticesT,
        static_cast<BlendMode>(mode % BLENDMODE_SIZE), paintHandle);
    drawVerticesOpItem.Marshalling(*drawCmdList);
    DrawVerticesOpItem::Unmarshalling(*drawCmdList, &constructorHandle);
    DrawVerticesOpItem drawVerticesOpItemTwo = DrawVerticesOpItem(*drawCmdList, &constructorHandle);
    if (dataText != nullptr) {
        delete [] dataText;
        dataText = nullptr;
    }
    return true;
}


bool DrawCmdFuzzTest041(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    size_t length = GetObject<size_t>() % MAX_SIZE + 1;
    char* dataText = new char[length];
    for (size_t i = 0; i < length; i++) {
        dataText[i] = GetObject<char>();
    }
    dataText[length - 1] = '\0';
    std::pair<const void*, size_t> cmdListData;
    cmdListData.first = static_cast<const void*>(dataText);
    cmdListData.second = length;
    auto drawCmdList = DrawCmdList::CreateFromData(cmdListData, false);
    ColorQuad color = GetObject<ColorQuad>();
    BlendMode mode = GetObject<BlendMode>();
    DrawColorOpItem::ConstructorHandle constructorHandle = DrawColorOpItem::ConstructorHandle(color, mode);
    DrawColorOpItem drawColorOpItem = DrawColorOpItem(&constructorHandle);
    drawColorOpItem.Marshalling(*drawCmdList);
    DrawColorOpItem::Unmarshalling(*drawCmdList, &constructorHandle);
    if (dataText != nullptr) {
        delete [] dataText;
        dataText = nullptr;
    }
    return true;
}

bool DrawCmdFuzzTest042(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    ColorQuad color = GetObject<ColorQuad>();
    BlendMode mode = GetObject<BlendMode>();
    DrawColorOpItem drawColorOpItem = DrawColorOpItem(color, mode);
    int32_t width = GetObject<int32_t>();
    int32_t height = GetObject<int32_t>();
    Canvas canvas = Canvas(width, height);
    Rect rect { GetObject<float>(), GetObject<float>(), GetObject<float>(), GetObject<float>() };
    drawColorOpItem.Playback(&canvas, &rect);
    return true;
}

bool DrawCmdFuzzTest043(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    Bitmap bitmap;
    int bitmapWidth = GetObject<int>() % MAX_SIZE;
    int bitmapHeight = GetObject<int>() % MAX_SIZE;
    BitmapFormat bitmapFormat = { COLORTYPE_ARGB_4444, ALPHATYPE_OPAQUE };
    bool buildBitmap = bitmap.Build(bitmapWidth, bitmapHeight, bitmapFormat);
    if (!buildBitmap) {
        return false;
    }
    Image image;
    image.BuildFromBitmap(bitmap);
    RectI center { GetObject<float>(), GetObject<float>(), GetObject<float>(), GetObject<float>() };
    Rect dst { GetObject<float>(), GetObject<float>(), GetObject<float>(), GetObject<float>() };
    uint32_t filterMode = GetObject<uint32_t>();
    Color color = Color(GetObject<uint32_t>(), GetObject<uint32_t>(), GetObject<uint32_t>(), GetObject<uint32_t>());
    Brush brush = Brush(color);
    DrawImageNineOpItem drawImageNineOpItem = DrawImageNineOpItem(&image, center,
        dst, static_cast<FilterMode>(filterMode % MATH_TWO), &brush);
    int32_t width = GetObject<int32_t>();
    int32_t height = GetObject<int32_t>();
    Canvas canvas = Canvas(width, height);
    Rect rect { GetObject<float>(), GetObject<float>(), GetObject<float>(), GetObject<float>() };
    drawImageNineOpItem.Playback(&canvas, &rect);
    size_t length = GetObject<size_t>() % MAX_SIZE + 1;
    char* dataText = new char[length];
    for (size_t i = 0; i < length; i++) {
        dataText[i] = GetObject<char>();
    }
    dataText[length - 1] = '\0';
    std::pair<const void*, size_t> cmdListData;
    cmdListData.first = static_cast<const void*>(dataText);
    cmdListData.second = length;
    auto drawCmdList = DrawCmdList::CreateFromData(cmdListData, false);
    OpDataHandle imageT { GetObject<uint32_t>(), GetObject<size_t>() };
    BrushHandle brushHandle;
    brushHandle.colorFilterHandle.size = GetObject<size_t>();
    brushHandle.colorSpaceHandle.size = GetObject<size_t>();
    brushHandle.shaderEffectHandle.size = GetObject<size_t>();
    brushHandle.imageFilterHandle.size = GetObject<size_t>();
    brushHandle.maskFilterHandle.size = GetObject<size_t>();
    bool hasBrush = GetObject<bool>();
    DrawImageNineOpItem::ConstructorHandle constructorHandle = DrawImageNineOpItem::ConstructorHandle(
        imageT, center, dst, static_cast<FilterMode>(filterMode % MATH_TWO), brushHandle, hasBrush);
    drawImageNineOpItem.Marshalling(*drawCmdList);
    DrawImageNineOpItem::Unmarshalling(*drawCmdList, &constructorHandle);
    DrawImageNineOpItem drawImageNineOpItemT = DrawImageNineOpItem(*drawCmdList, &constructorHandle);
    if (dataText != nullptr) {
        delete [] dataText;
        dataText = nullptr;
    }
    return true;
}

bool DrawCmdFuzzTest044(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    Bitmap bitmap;
    int bitmapWidth = GetObject<int>() % MAX_SIZE;
    int bitmapHeight = GetObject<int>() % MAX_SIZE;
    BitmapFormat bitmapFormat = { COLORTYPE_ARGB_4444, ALPHATYPE_OPAQUE };
    bool buildBitmap = bitmap.Build(bitmapWidth, bitmapHeight, bitmapFormat);
    if (!buildBitmap) {
        return false;
    }
    Image image;
    image.BuildFromBitmap(bitmap);
    Lattice lattice;
    lattice.fXCount = GetObject<int>();
    lattice.fYCount = GetObject<int>();
    Rect dst { GetObject<float>(), GetObject<float>(), GetObject<float>(), GetObject<float>() };
    uint32_t filterMode = GetObject<uint32_t>();
    Color color = Color(GetObject<uint32_t>(), GetObject<uint32_t>(), GetObject<uint32_t>(), GetObject<uint32_t>());
    Paint paint = Paint(color);
    DrawImageLatticeOpItem drawImageLatticeOpItem = DrawImageLatticeOpItem(&image, lattice,
        dst, static_cast<FilterMode>(filterMode % MATH_TWO), paint);
    int32_t width = GetObject<int32_t>();
    int32_t height = GetObject<int32_t>();
    Canvas canvas = Canvas(width, height);
    Rect rect { GetObject<float>(), GetObject<float>(), GetObject<float>(), GetObject<float>() };
    drawImageLatticeOpItem.Playback(&canvas, &rect);
    size_t length = GetObject<size_t>() % MAX_SIZE + 1;
    char* dataText = new char[length];
    for (size_t i = 0; i < length; i++) {
        dataText[i] = GetObject<char>();
    }
    dataText[length - 1] = '\0';
    std::pair<const void*, size_t> cmdListData;
    cmdListData.first = static_cast<const void*>(dataText);
    cmdListData.second = length;
    auto drawCmdList = DrawCmdList::CreateFromData(cmdListData, false);
    OpDataHandle imageT { GetObject<uint32_t>(), GetObject<size_t>() };
    PaintHandle paintHandle;
    paintHandle.isAntiAlias = GetObject<bool>();
    paintHandle.width = GetObject<scalar>();
    DrawImageLatticeOpItem::ConstructorHandle constructorHandle = DrawImageLatticeOpItem::ConstructorHandle(
        imageT, lattice, dst, static_cast<FilterMode>(filterMode % MATH_TWO), paintHandle);
    drawImageLatticeOpItem.Marshalling(*drawCmdList);
    DrawImageLatticeOpItem::Unmarshalling(*drawCmdList, &constructorHandle);
    DrawImageLatticeOpItem drawImageLatticeOpItemT = DrawImageLatticeOpItem(*drawCmdList, &constructorHandle);
    if (dataText != nullptr) {
        delete [] dataText;
        dataText = nullptr;
    }
    return true;
}

bool DrawCmdFuzzTest045(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    Bitmap bitmap;
    int bitmapWidth = GetObject<int>() % MAX_SIZE;
    int bitmapHeight = GetObject<int>() % MAX_SIZE;
    BitmapFormat bitmapFormat = { COLORTYPE_ARGB_4444, ALPHATYPE_OPAQUE };
    bool buildBitmap = bitmap.Build(bitmapWidth, bitmapHeight, bitmapFormat);
    if (!buildBitmap) {
        return false;
    }
    Image image;
    image.BuildFromBitmap(bitmap);
    RSXform rsxform = RSXform::Make(GetObject<scalar>(), GetObject<scalar>(), GetObject<scalar>(), GetObject<scalar>());
    std::vector<RSXform> xform { rsxform };
    Rect rect { GetObject<float>(), GetObject<float>(), GetObject<float>(), GetObject<float>() };
    std::vector<Rect> tex { rect };
    ColorQuad colorQuad = GetObject<ColorQuad>();
    std::vector<ColorQuad> colors { colorQuad };
    uint32_t mode = GetObject<uint32_t>();
    uint32_t filterMode = GetObject<uint32_t>();
    SamplingOptions samplingOptions = SamplingOptions(static_cast<FilterMode>(filterMode % MATH_TWO));
    bool hasCullRect = GetObject<bool>();
    Rect cullRect { GetObject<float>(), GetObject<float>(), GetObject<float>(), GetObject<float>() };
    Color color = Color(GetObject<uint32_t>(), GetObject<uint32_t>(), GetObject<uint32_t>(), GetObject<uint32_t>());
    Paint paint = Paint(color);
    DrawAtlasOpItem drawAtlasOpItem = DrawAtlasOpItem(&image, xform, tex, colors,
        static_cast<BlendMode>(mode % BLENDMODE_SIZE), samplingOptions, hasCullRect, cullRect, paint);
    size_t length = GetObject<size_t>() % MAX_SIZE + 1;
    char* dataText = new char[length];
    for (size_t i = 0; i < length; i++) {
        dataText[i] = GetObject<char>();
    }
    dataText[length - 1] = '\0';
    std::pair<const void*, size_t> cmdListData;
    cmdListData.first = static_cast<const void*>(dataText);
    cmdListData.second = length;
    auto drawCmdList = DrawCmdList::CreateFromData(cmdListData, false);
    OpDataHandle atlas { GetObject<uint32_t>(), GetObject<size_t>() };
    std::pair<uint32_t, size_t> xformT = { GetObject<uint32_t>(), GetObject<size_t>() };
    std::pair<uint32_t, size_t> texT = { GetObject<uint32_t>(), GetObject<size_t>() };
    std::pair<uint32_t, size_t> colorsT = { GetObject<uint32_t>(), GetObject<size_t>() };
    PaintHandle paintHandle;
    paintHandle.isAntiAlias = GetObject<bool>();
    paintHandle.width = GetObject<scalar>();
    DrawAtlasOpItem::ConstructorHandle constructorHandle = DrawAtlasOpItem::ConstructorHandle(atlas, xformT, texT,
        colorsT, static_cast<BlendMode>(mode % BLENDMODE_SIZE), samplingOptions, hasCullRect, cullRect, paintHandle);
    drawAtlasOpItem.Marshalling(*drawCmdList);
    DrawAtlasOpItem::Unmarshalling(*drawCmdList, &constructorHandle);
    if (dataText != nullptr) {
        delete [] dataText;
        dataText = nullptr;
    }
    return true;
}

bool DrawCmdFuzzTest046(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    size_t length = GetObject<size_t>() % MAX_SIZE + 1;
    char* dataText = new char[length];
    for (size_t i = 0; i < length; i++) {
        dataText[i] = GetObject<char>();
    }
    dataText[length - 1] = '\0';
    std::pair<const void*, size_t> cmdListData;
    cmdListData.first = static_cast<const void*>(dataText);
    cmdListData.second = length;
    auto drawCmdList = DrawCmdList::CreateFromData(cmdListData, false);
    OpDataHandle atlas { GetObject<uint32_t>(), GetObject<size_t>() };
    std::pair<uint32_t, size_t> xformT = { GetObject<uint32_t>(), GetObject<size_t>() };
    std::pair<uint32_t, size_t> texT = { GetObject<uint32_t>(), GetObject<size_t>() };
    std::pair<uint32_t, size_t> colorsT = { GetObject<uint32_t>(), GetObject<size_t>() };
    PaintHandle paintHandle;
    paintHandle.isAntiAlias = GetObject<bool>();
    paintHandle.width = GetObject<scalar>();
    uint32_t mode = GetObject<uint32_t>();
    uint32_t filterMode = GetObject<uint32_t>();
    SamplingOptions samplingOptions = SamplingOptions(static_cast<FilterMode>(filterMode % MATH_TWO));
    bool hasCullRect = GetObject<bool>();
    Rect cullRect { GetObject<float>(), GetObject<float>(), GetObject<float>(), GetObject<float>() };
    DrawAtlasOpItem::ConstructorHandle constructorHandle = DrawAtlasOpItem::ConstructorHandle(atlas, xformT, texT,
        colorsT, static_cast<BlendMode>(mode % BLENDMODE_SIZE), samplingOptions, hasCullRect, cullRect, paintHandle);
    DrawAtlasOpItem drawAtlasOpItem = DrawAtlasOpItem(*drawCmdList, &constructorHandle);
    int32_t width = GetObject<int32_t>();
    int32_t height = GetObject<int32_t>();
    Canvas canvas = Canvas(width, height);
    Rect rectT { GetObject<float>(), GetObject<float>(), GetObject<float>(), GetObject<float>() };
    drawAtlasOpItem.Playback(&canvas, &rectT);
    if (dataText != nullptr) {
        delete [] dataText;
        dataText = nullptr;
    }
    return true;
}

bool DrawCmdFuzzTest047(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    Bitmap bitmap;
    int bitmapWidth = GetObject<int>() % MAX_SIZE;
    int bitmapHeight = GetObject<int>() % MAX_SIZE;
    BitmapFormat bitmapFormat = { COLORTYPE_ARGB_4444, ALPHATYPE_OPAQUE };
    bool buildBitmap = bitmap.Build(bitmapWidth, bitmapHeight, bitmapFormat);
    if (!buildBitmap) {
        return false;
    }
    scalar px = GetObject<scalar>();
    scalar py = GetObject<scalar>();
    Color color = Color(GetObject<uint32_t>(), GetObject<uint32_t>(), GetObject<uint32_t>(), GetObject<uint32_t>());
    Paint paint = Paint(color);
    DrawBitmapOpItem drawBitmapOpItem = DrawBitmapOpItem(bitmap, px, py, paint);
    int32_t width = GetObject<int32_t>();
    int32_t height = GetObject<int32_t>();
    Canvas canvas = Canvas(width, height);
    Rect rect { GetObject<float>(), GetObject<float>(), GetObject<float>(), GetObject<float>() };
    drawBitmapOpItem.Playback(&canvas, &rect);
    size_t length = GetObject<size_t>() % MAX_SIZE + 1;
    char* dataText = new char[length];
    for (size_t i = 0; i < length; i++) {
        dataText[i] = GetObject<char>();
    }
    dataText[length - 1] = '\0';
    std::pair<const void*, size_t> cmdListData;
    cmdListData.first = static_cast<const void*>(dataText);
    cmdListData.second = length;
    auto drawCmdList = DrawCmdList::CreateFromData(cmdListData, false);
    ImageHandle bitmapT { GetObject<uint32_t>(), GetObject<size_t>(), GetObject<int32_t>(), GetObject<int32_t>() };
    PaintHandle paintHandle;
    paintHandle.isAntiAlias = GetObject<bool>();
    paintHandle.width = GetObject<scalar>();
    DrawBitmapOpItem::ConstructorHandle constructorHandle = DrawBitmapOpItem::ConstructorHandle(
        bitmapT, px, py, paintHandle);
    drawBitmapOpItem.Marshalling(*drawCmdList);
    DrawBitmapOpItem::Unmarshalling(*drawCmdList, &constructorHandle);
    DrawBitmapOpItem drawBitmapOpItemT = DrawBitmapOpItem(*drawCmdList, &constructorHandle);
    if (dataText != nullptr) {
        delete [] dataText;
        dataText = nullptr;
    }
    return true;
}

bool DrawCmdFuzzTest048(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    Bitmap bitmap;
    int bitmapWidth = GetObject<int>() % MAX_SIZE;
    int bitmapHeight = GetObject<int>() % MAX_SIZE;
    BitmapFormat bitmapFormat = { COLORTYPE_ARGB_4444, ALPHATYPE_OPAQUE };
    bool buildBitmap = bitmap.Build(bitmapWidth, bitmapHeight, bitmapFormat);
    if (!buildBitmap) {
        return false;
    }
    Image image;
    image.BuildFromBitmap(bitmap);
    scalar px = GetObject<scalar>();
    scalar py = GetObject<scalar>();
    uint32_t filterMode = GetObject<uint32_t>();
    SamplingOptions samplingOptions = SamplingOptions(static_cast<FilterMode>(filterMode % MATH_TWO));
    Color color = Color(GetObject<uint32_t>(), GetObject<uint32_t>(), GetObject<uint32_t>(), GetObject<uint32_t>());
    Paint paint = Paint(color);
    DrawImageOpItem drawImageOpItem = DrawImageOpItem(image, px, py, samplingOptions, paint);
    int32_t width = GetObject<int32_t>();
    int32_t height = GetObject<int32_t>();
    Canvas canvas = Canvas(width, height);
    Rect rect { GetObject<float>(), GetObject<float>(), GetObject<float>(), GetObject<float>() };
    drawImageOpItem.Playback(&canvas, &rect);
    size_t length = GetObject<size_t>() % MAX_SIZE + 1;
    char* dataText = new char[length];
    for (size_t i = 0; i < length; i++) {
        dataText[i] = GetObject<char>();
    }
    dataText[length - 1] = '\0';
    std::pair<const void*, size_t> cmdListData;
    cmdListData.first = static_cast<const void*>(dataText);
    cmdListData.second = length;
    auto drawCmdList = DrawCmdList::CreateFromData(cmdListData, false);
    OpDataHandle imageT { GetObject<uint32_t>(), GetObject<size_t>() };
    PaintHandle paintHandle;
    paintHandle.isAntiAlias = GetObject<bool>();
    paintHandle.width = GetObject<scalar>();
    DrawImageOpItem::ConstructorHandle constructorHandle = DrawImageOpItem::ConstructorHandle(
        imageT, px, py, samplingOptions, paintHandle);
    drawImageOpItem.Marshalling(*drawCmdList);
    DrawImageOpItem::Unmarshalling(*drawCmdList, &constructorHandle);
    DrawImageOpItem drawImageOpItemT = DrawImageOpItem(*drawCmdList, &constructorHandle);
    if (dataText != nullptr) {
        delete [] dataText;
        dataText = nullptr;
    }
    return true;
}

bool DrawCmdFuzzTest049(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }
    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    Bitmap bitmap;
    int bitmapWidth = GetObject<int>() % MAX_SIZE;
    int bitmapHeight = GetObject<int>() % MAX_SIZE;
    BitmapFormat bitmapFormat = { COLORTYPE_ARGB_4444, ALPHATYPE_OPAQUE };
    bool buildBitmap = bitmap.Build(bitmapWidth, bitmapHeight, bitmapFormat);
    if (!buildBitmap) {
        return false;
    }
    Image image;
    image.BuildFromBitmap(bitmap);
    Rect src { GetObject<float>(), GetObject<float>(), GetObject<float>(), GetObject<float>() };
    Rect dst { GetObject<float>(), GetObject<float>(), GetObject<float>(), GetObject<float>() };
    uint32_t filterMode = GetObject<uint32_t>();
    SamplingOptions samplingOptions = SamplingOptions(static_cast<FilterMode>(filterMode % MATH_TWO));
    uint32_t constraint = GetObject<uint32_t>();
    bool isForeground = GetObject<bool>();
    Color color = Color(GetObject<uint32_t>(), GetObject<uint32_t>(), GetObject<uint32_t>(), GetObject<uint32_t>());
    Paint paint = Paint(color);
    DrawImageRectOpItem drawImageRectOpItem = DrawImageRectOpItem(image, src, dst,
        samplingOptions, static_cast<SrcRectConstraint>(constraint % MATH_TWO), paint, isForeground);
    int32_t width = GetObject<int32_t>();
    int32_t height = GetObject<int32_t>();
    Canvas canvas = Canvas(width, height);
    Rect rect { GetObject<float>(), GetObject<float>(), GetObject<float>(), GetObject<float>() };
    drawImageRectOpItem.Playback(&canvas, &rect);
        size_t length = GetObject<size_t>() % MAX_SIZE + 1;
    char* dataText = new char[length];
    for (size_t i = 0; i < length; i++) {
        dataText[i] = GetObject<char>();
    }
    dataText[length - 1] = '\0';
    std::pair<const void*, size_t> cmdListData;
    cmdListData.first = static_cast<const void*>(dataText);
    cmdListData.second = length;
    auto drawCmdList = DrawCmdList::CreateFromData(cmdListData, false);
    OpDataHandle imageT { GetObject<uint32_t>(), GetObject<size_t>() };
    PaintHandle paintHandle;
    paintHandle.isAntiAlias = GetObject<bool>();
    paintHandle.width = GetObject<scalar>();
    DrawImageRectOpItem::ConstructorHandle constructorHandle = DrawImageRectOpItem::ConstructorHandle(imageT, src, dst,
        samplingOptions, static_cast<SrcRectConstraint>(constraint % MATH_TWO), paintHandle, isForeground);
    drawImageRectOpItem.Marshalling(*drawCmdList);
    DrawImageRectOpItem::Unmarshalling(*drawCmdList, &constructorHandle);
    DrawImageRectOpItem drawImageRectOpItemT = DrawImageRectOpItem(*drawCmdList, &constructorHandle);
    if (dataText != nullptr) {
        delete [] dataText;
        dataText = nullptr;
    }
    return true;
}

bool DrawCmdFuzzTest050(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    Picture picture;
    auto dataVal = std::make_shared<Data>();
    size_t length = GetObject<size_t>() % MAX_SIZE + 1;
    char* dataText = new char[length];
    for (size_t i = 0; i < length; i++) {
        dataText[i] = GetObject<char>();
    }
    dataText[length - 1] = '\0';
    dataVal->BuildWithoutCopy(dataText, length);
    picture.Deserialize(dataVal);
    picture.Serialize();
    DrawPictureOpItem drawPictureOpItem = DrawPictureOpItem(picture);
    int32_t width = GetObject<int32_t>();
    int32_t height = GetObject<int32_t>();
    Canvas canvas = Canvas(width, height);
    Rect rectT { GetObject<float>(), GetObject<float>(), GetObject<float>(), GetObject<float>() };
    drawPictureOpItem.Playback(&canvas, &rectT);
    std::pair<const void*, size_t> cmdListData;
    cmdListData.first = static_cast<const void*>(dataText);
    cmdListData.second = length;
    auto drawCmdList = DrawCmdList::CreateFromData(cmdListData, false);
    OpDataHandle pictureT { GetObject<uint32_t>(), GetObject<size_t>() };
    DrawPictureOpItem::ConstructorHandle constructorHandle = DrawPictureOpItem::ConstructorHandle(pictureT);
    drawPictureOpItem.Marshalling(*drawCmdList);
    DrawPictureOpItem::Unmarshalling(*drawCmdList, &constructorHandle);
    DrawPictureOpItem drawPictureOpItemT = DrawPictureOpItem(*drawCmdList, &constructorHandle);
    if (dataText != nullptr) {
        delete [] dataText;
        dataText = nullptr;
    }
    return true;
}

bool DrawCmdFuzzTest051(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    size_t length = GetObject<size_t>() % MAX_SIZE + 1;
    char* dataText = new char[length];
    for (size_t i = 0; i < length; i++) {
        dataText[i] = GetObject<char>();
    }
    dataText[length - 1] = '\0';
    std::pair<const void*, size_t> cmdListData;
    cmdListData.first = static_cast<const void*>(dataText);
    cmdListData.second = length;
    auto drawCmdList = DrawCmdList::CreateFromData(cmdListData, false);
    OpDataHandle textBlob { GetObject<uint32_t>(), GetObject<size_t>() };
    uint64_t globalUniqueId = GetObject<uint64_t>();
    scalar x = GetObject<scalar>();
    scalar y = GetObject<scalar>();
    PaintHandle paintHandle;
    paintHandle.isAntiAlias = GetObject<bool>();
    paintHandle.width = GetObject<scalar>();
    DrawTextBlobOpItem::ConstructorHandle constructorHandle = DrawTextBlobOpItem::ConstructorHandle(
        textBlob, globalUniqueId, x, y, paintHandle);
    DrawTextBlobOpItem drawTextBlobOpItem = DrawTextBlobOpItem(*drawCmdList, &constructorHandle);
    drawTextBlobOpItem.Marshalling(*drawCmdList);
    DrawTextBlobOpItem::Unmarshalling(*drawCmdList, &constructorHandle);
    if (dataText != nullptr) {
        delete [] dataText;
        dataText = nullptr;
    }
    return true;
}

bool DrawCmdFuzzTest052(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    Font font;
    scalar fSize = GetObject<scalar>();
    font.SetSize(fSize);
    const char* str = "as";
    auto textBlob = TextBlob::MakeFromText(str, strlen(str), font, TextEncoding::UTF8);
    scalar x = GetObject<scalar>();
    scalar y = GetObject<scalar>();
    uint32_t alpha = GetObject<uint32_t>();
    uint32_t red = GetObject<uint32_t>();
    uint32_t blue = GetObject<uint32_t>();
    uint32_t green = GetObject<uint32_t>();
    Color color = Color(red, green, blue, alpha);
    Paint paint = Paint(color);
    DrawTextBlobOpItem drawTextBlobOpItem = DrawTextBlobOpItem(textBlob.get(), x, y, paint);
    int32_t width = GetObject<int32_t>();
    int32_t height = GetObject<int32_t>();
    Canvas canvas = Canvas(width, height);
    Rect rectT { GetObject<float>(), GetObject<float>(), GetObject<float>(), GetObject<float>() };
    drawTextBlobOpItem.Playback(&canvas, &rectT);
    return true;
}

bool DrawCmdFuzzTest053(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    size_t length = GetObject<size_t>() % MAX_SIZE + 1;
    char* dataText = new char[length];
    for (size_t i = 0; i < length; i++) {
        dataText[i] = GetObject<char>();
    }
    dataText[length - 1] = '\0';
    std::pair<const void*, size_t> cmdListData;
    cmdListData.first = static_cast<const void*>(dataText);
    cmdListData.second = length;
    auto drawCmdList = DrawCmdList::CreateFromData(cmdListData, false);
    SymbolLayersHandle symbolLayerHandle { GetObject<uint32_t>() };
    OpDataHandle pathHandle { GetObject<uint32_t>(), GetObject<size_t>() };
    uint64_t symbolId = GetObject<uint64_t>();
    SymbolOpHandle symbolHandle { symbolLayerHandle, pathHandle, symbolId };
    int32_t x = GetObject<int32_t>();
    int32_t y = GetObject<int32_t>();
    Point locate { x, y };
    PaintHandle paintHandle;
    paintHandle.isAntiAlias = GetObject<bool>();
    paintHandle.width = GetObject<scalar>();
    DrawSymbolOpItem::ConstructorHandle constructorHandle = DrawSymbolOpItem::ConstructorHandle(
        symbolHandle, locate, paintHandle);
    DrawSymbolOpItem drawSymbolOpItem = DrawSymbolOpItem(*drawCmdList, &constructorHandle);
    drawSymbolOpItem.Marshalling(*drawCmdList);
    DrawSymbolOpItem::Unmarshalling(*drawCmdList, &constructorHandle);
    if (dataText != nullptr) {
        delete [] dataText;
        dataText = nullptr;
    }
    return true;
}

bool DrawCmdFuzzTest054(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    Rect rect { GetObject<float>(), GetObject<float>(), GetObject<float>(), GetObject<float>() };
    scalar xRad = GetObject<scalar>();
    scalar yRad = GetObject<scalar>();
    RoundRect roundRect = RoundRect(rect, xRad, yRad);
    uint32_t alpha = GetObject<uint32_t>();
    uint32_t red = GetObject<uint32_t>();
    uint32_t blue = GetObject<uint32_t>();
    uint32_t green = GetObject<uint32_t>();
    Color color = Color(red, green, blue, alpha);
    Paint paint = Paint(color);
    DrawingSymbolLayers symbolInfo { GetObject<uint32_t>() };
    Path path;
    uint32_t strCount = GetObject<uint32_t>() % MAX_SIZE + 1;
    char str[strCount];
    for (size_t i = 0; i < strCount; i++) {
        str[i] =  GetObject<char>();
    }
    str[strCount - 1] = '\0';
    path.BuildFromSVGString(str);
    uint64_t symbolId = GetObject<uint64_t>();
    DrawingHMSymbolData symbol { symbolInfo, path, symbolId };
    int32_t x = GetObject<int32_t>();
    int32_t y = GetObject<int32_t>();
    Point locate { x, y };
    DrawSymbolOpItem drawSymbolOpItem = DrawSymbolOpItem(symbol, locate, paint);
    int32_t width = GetObject<int32_t>();
    int32_t height = GetObject<int32_t>();
    Canvas canvas = Canvas(width, height);
    Rect rectT { GetObject<float>(), GetObject<float>(), GetObject<float>(), GetObject<float>() };
    drawSymbolOpItem.Playback(&canvas, &rectT);
    return true;
}

bool DrawCmdFuzzTest055(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    size_t length = GetObject<size_t>() % MAX_SIZE + 1;
    char* dataText = new char[length];
    for (size_t i = 0; i < length; i++) {
        dataText[i] = GetObject<char>();
    }
    dataText[length - 1] = '\0';
    std::pair<const void*, size_t> cmdListData;
    cmdListData.first = static_cast<const void*>(dataText);
    cmdListData.second = length;
    auto drawCmdList = DrawCmdList::CreateFromData(cmdListData, false);
    SaveOpItem::ConstructorHandle constructorHandle = SaveOpItem::ConstructorHandle();
    SaveOpItem saveOpItem = SaveOpItem();
    saveOpItem.Marshalling(*drawCmdList);
    SaveOpItem::Unmarshalling(*drawCmdList, &constructorHandle);
    if (dataText != nullptr) {
        delete [] dataText;
        dataText = nullptr;
    }
    return true;
}

bool DrawCmdFuzzTest056(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    SaveOpItem saveOpItem = SaveOpItem();
    int32_t width = GetObject<int32_t>();
    int32_t height = GetObject<int32_t>();
    Canvas canvas = Canvas(width, height);
    Rect rectT { GetObject<float>(), GetObject<float>(), GetObject<float>(), GetObject<float>() };
    saveOpItem.Playback(&canvas, &rectT);
    return true;
}

bool DrawCmdFuzzTest057(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    size_t length = GetObject<size_t>() % MAX_SIZE + 1;
    char* dataText = new char[length];
    for (size_t i = 0; i < length; i++) {
        dataText[i] = GetObject<char>();
    }
    dataText[length - 1] = '\0';
    std::pair<const void*, size_t> cmdListData;
    cmdListData.first = static_cast<const void*>(dataText);
    cmdListData.second = length;
    auto drawCmdList = DrawCmdList::CreateFromData(cmdListData, false);
    scalar dx = GetObject<scalar>();
    scalar dy = GetObject<scalar>();
    TranslateOpItem::ConstructorHandle constructorHandle = TranslateOpItem::ConstructorHandle(dx, dy);
    TranslateOpItem translateOpItem = TranslateOpItem(&constructorHandle);
    translateOpItem.Marshalling(*drawCmdList);
    TranslateOpItem::Unmarshalling(*drawCmdList, &constructorHandle);
    if (dataText != nullptr) {
        delete [] dataText;
        dataText = nullptr;
    }
    return true;
}

bool DrawCmdFuzzTest058(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    scalar dx = GetObject<scalar>();
    scalar dy = GetObject<scalar>();
    TranslateOpItem translateOpItem = TranslateOpItem(dx, dy);
    int32_t width = GetObject<int32_t>();
    int32_t height = GetObject<int32_t>();
    Canvas canvas = Canvas(width, height);
    Rect rectT { GetObject<float>(), GetObject<float>(), GetObject<float>(), GetObject<float>() };
    translateOpItem.Playback(&canvas, &rectT);
    return true;
}

bool DrawCmdFuzzTest059(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    size_t length = GetObject<size_t>() % MAX_SIZE + 1;
    char* dataText = new char[length];
    for (size_t i = 0; i < length; i++) {
        dataText[i] = GetObject<char>();
    }
    dataText[length - 1] = '\0';
    std::pair<const void*, size_t> cmdListData;
    cmdListData.first = static_cast<const void*>(dataText);
    cmdListData.second = length;
    auto drawCmdList = DrawCmdList::CreateFromData(cmdListData, false);
    scalar sx = GetObject<scalar>();
    scalar sy = GetObject<scalar>();
    ScaleOpItem::ConstructorHandle constructorHandle = ScaleOpItem::ConstructorHandle(sx, sy);
    ScaleOpItem scaleOpItem = ScaleOpItem(&constructorHandle);
    scaleOpItem.Marshalling(*drawCmdList);
    ScaleOpItem::Unmarshalling(*drawCmdList, &constructorHandle);
    if (dataText != nullptr) {
        delete [] dataText;
        dataText = nullptr;
    }
    return true;
}

bool DrawCmdFuzzTest060(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    scalar sx = GetObject<scalar>();
    scalar sy = GetObject<scalar>();
    ScaleOpItem scaleOpItem = ScaleOpItem(sx, sy);
    int32_t width = GetObject<int32_t>();
    int32_t height = GetObject<int32_t>();
    Canvas canvas = Canvas(width, height);
    Rect rectT { GetObject<float>(), GetObject<float>(), GetObject<float>(), GetObject<float>() };
    scaleOpItem.Playback(&canvas, &rectT);
    return true;
}

bool DrawCmdFuzzTest061(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    size_t length = GetObject<size_t>() % MAX_SIZE + 1;
    char* dataText = new char[length];
    for (size_t i = 0; i < length; i++) {
        dataText[i] = GetObject<char>();
    }
    dataText[length - 1] = '\0';
    std::pair<const void*, size_t> cmdListData;
    cmdListData.first = static_cast<const void*>(dataText);
    cmdListData.second = length;
    auto drawCmdList = DrawCmdList::CreateFromData(cmdListData, false);
    Rect rect { GetObject<float>(), GetObject<float>(), GetObject<float>(), GetObject<float>() };
    uint32_t op = GetObject<uint32_t>();
    bool doAntiAlias = GetObject<bool>();
    ClipRectOpItem::ConstructorHandle constructorHandle = ClipRectOpItem::ConstructorHandle(
        rect, static_cast<ClipOp>(op % MATH_TWO), doAntiAlias);
    ClipRectOpItem clipRectOpItem = ClipRectOpItem(&constructorHandle);
    clipRectOpItem.Marshalling(*drawCmdList);
    ClipRectOpItem::Unmarshalling(*drawCmdList, &constructorHandle);
    if (dataText != nullptr) {
        delete [] dataText;
        dataText = nullptr;
    }
    return true;
}

bool DrawCmdFuzzTest062(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    Rect rect { GetObject<float>(), GetObject<float>(), GetObject<float>(), GetObject<float>() };
    uint32_t op = GetObject<uint32_t>();
    bool doAntiAlias = GetObject<bool>();
    ClipRectOpItem clipRectOpItem = ClipRectOpItem(rect, static_cast<ClipOp>(op % MATH_TWO), doAntiAlias);
    int32_t width = GetObject<int32_t>();
    int32_t height = GetObject<int32_t>();
    Canvas canvas = Canvas(width, height);
    Rect rectT { GetObject<float>(), GetObject<float>(), GetObject<float>(), GetObject<float>() };
    clipRectOpItem.Playback(&canvas, &rectT);
    return true;
}
bool DrawCmdFuzzTest063(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    size_t length = GetObject<size_t>() % MAX_SIZE + 1;
    char* dataText = new char[length];
    for (size_t i = 0; i < length; i++) {
        dataText[i] = GetObject<char>();
    }
    dataText[length - 1] = '\0';
    std::pair<const void*, size_t> cmdListData;
    cmdListData.first = static_cast<const void*>(dataText);
    cmdListData.second = length;
    auto drawCmdList = DrawCmdList::CreateFromData(cmdListData, false);
    RectI rect { GetObject<float>(), GetObject<float>(), GetObject<float>(), GetObject<float>() };
    uint32_t op = GetObject<uint32_t>();
    ClipIRectOpItem::ConstructorHandle constructorHandle = ClipIRectOpItem::ConstructorHandle(
        rect, static_cast<ClipOp>(op % MATH_TWO));
    ClipIRectOpItem clipIRectOpItem = ClipIRectOpItem(&constructorHandle);
    clipIRectOpItem.Marshalling(*drawCmdList);
    ClipIRectOpItem::Unmarshalling(*drawCmdList, &constructorHandle);
    if (dataText != nullptr) {
        delete [] dataText;
        dataText = nullptr;
    }
    return true;
}

bool DrawCmdFuzzTest064(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    RectI rect { GetObject<float>(), GetObject<float>(), GetObject<float>(), GetObject<float>() };
    uint32_t op = GetObject<uint32_t>();
    ClipIRectOpItem clipIRectOpItem = ClipIRectOpItem(rect, static_cast<ClipOp>(op % MATH_TWO));
    int32_t width = GetObject<int32_t>();
    int32_t height = GetObject<int32_t>();
    Canvas canvas = Canvas(width, height);
    Rect rectT { GetObject<float>(), GetObject<float>(), GetObject<float>(), GetObject<float>() };
    clipIRectOpItem.Playback(&canvas, &rectT);
    return true;
}

bool DrawCmdFuzzTest065(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    size_t length = GetObject<size_t>() % MAX_SIZE + 1;
    char* dataText = new char[length];
    for (size_t i = 0; i < length; i++) {
        dataText[i] = GetObject<char>();
    }
    dataText[length - 1] = '\0';
    std::pair<const void*, size_t> cmdListData;
    cmdListData.first = static_cast<const void*>(dataText);
    cmdListData.second = length;
    auto drawCmdList = DrawCmdList::CreateFromData(cmdListData, false);
    Rect rect { GetObject<float>(), GetObject<float>(), GetObject<float>(), GetObject<float>() };
    scalar xRad = GetObject<scalar>();
    scalar yRad = GetObject<scalar>();
    RoundRect roundRect = RoundRect(rect, xRad, yRad);
    uint32_t op = GetObject<uint32_t>();
    bool doAntiAlias = GetObject<bool>();
    ClipRoundRectOpItem::ConstructorHandle constructorHandle = ClipRoundRectOpItem::ConstructorHandle(
        roundRect, static_cast<ClipOp>(op % MATH_TWO), doAntiAlias);
    ClipRoundRectOpItem clipRoundRectOpItem = ClipRoundRectOpItem(&constructorHandle);
    clipRoundRectOpItem.Marshalling(*drawCmdList);
    ClipRoundRectOpItem::Unmarshalling(*drawCmdList, &constructorHandle);
    if (dataText != nullptr) {
        delete [] dataText;
        dataText = nullptr;
    }
    return true;
}

bool DrawCmdFuzzTest066(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    Rect rect { GetObject<float>(), GetObject<float>(), GetObject<float>(), GetObject<float>() };
    scalar xRad = GetObject<scalar>();
    scalar yRad = GetObject<scalar>();
    RoundRect roundRect = RoundRect(rect, xRad, yRad);
    uint32_t op = GetObject<uint32_t>();
    bool doAntiAlias = GetObject<bool>();
    ClipRoundRectOpItem clipRoundRectOpItem = ClipRoundRectOpItem(
        roundRect, static_cast<ClipOp>(op % MATH_TWO), doAntiAlias);
    int32_t width = GetObject<int32_t>();
    int32_t height = GetObject<int32_t>();
    Canvas canvas = Canvas(width, height);
    Rect rectT { GetObject<float>(), GetObject<float>(), GetObject<float>(), GetObject<float>() };
    clipRoundRectOpItem.Playback(&canvas, &rectT);
    return true;
}

bool DrawCmdFuzzTest067(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    Path path;
    uint32_t strCount = GetObject<uint32_t>() % MAX_SIZE + 1;
    char str[strCount];
    for (size_t i = 0; i < strCount; i++) {
        str[i] =  GetObject<char>();
    }
    str[strCount - 1] = '\0';
    path.BuildFromSVGString(str);
    uint32_t op = GetObject<uint32_t>();
    bool doAntiAlias = GetObject<bool>();
    ClipPathOpItem clipPathOpItem = ClipPathOpItem(
        path, static_cast<ClipOp>(op % MATH_TWO), doAntiAlias);
    int32_t width = GetObject<int32_t>();
    int32_t height = GetObject<int32_t>();
    Canvas canvas = Canvas(width, height);
    Rect rectT { GetObject<float>(), GetObject<float>(), GetObject<float>(), GetObject<float>() };
    clipPathOpItem.Playback(&canvas, &rectT);
    size_t length = GetObject<size_t>() % MAX_SIZE + 1;
    char* dataText = new char[length];
    for (size_t i = 0; i < length; i++) {
        dataText[i] = GetObject<char>();
    }
    dataText[length - 1] = '\0';
    std::pair<const void*, size_t> cmdListData;
    cmdListData.first = static_cast<const void*>(dataText);
    cmdListData.second = length;
    auto drawCmdList = DrawCmdList::CreateFromData(cmdListData, false);
    OpDataHandle pathT { GetObject<uint32_t>(), GetObject<size_t>() };
    ClipPathOpItem::ConstructorHandle constructorHandle = ClipPathOpItem::ConstructorHandle(
        pathT, static_cast<ClipOp>(op % MATH_TWO), doAntiAlias);
    clipPathOpItem.Marshalling(*drawCmdList);
    ClipPathOpItem::Unmarshalling(*drawCmdList, &constructorHandle);
    ClipPathOpItem clipPathOpItemT = ClipPathOpItem(*drawCmdList, &constructorHandle);
    if (dataText != nullptr) {
        delete [] dataText;
        dataText = nullptr;
    }
    return true;
}

bool DrawCmdFuzzTest068(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    size_t length = GetObject<size_t>() % MAX_SIZE + 1;
    char* dataText = new char[length];
    for (size_t i = 0; i < length; i++) {
        dataText[i] = GetObject<char>();
    }
    dataText[length - 1] = '\0';
    std::pair<const void*, size_t> cmdListData;
    cmdListData.first = static_cast<const void*>(dataText);
    cmdListData.second = length;
    auto drawCmdList = DrawCmdList::CreateFromData(cmdListData, false);
    Matrix::Buffer matrixBuffer;
    for (size_t i = 0; i < Matrix::MATRIX_SIZE; i++) {
        matrixBuffer[i] = GetObject<scalar>();
    }
    SetMatrixOpItem::ConstructorHandle constructorHandle = SetMatrixOpItem::ConstructorHandle(
        matrixBuffer);
    SetMatrixOpItem setMatrixOpItem = SetMatrixOpItem(&constructorHandle);
    setMatrixOpItem.Marshalling(*drawCmdList);
    SetMatrixOpItem::Unmarshalling(*drawCmdList, &constructorHandle);
    if (dataText != nullptr) {
        delete [] dataText;
        dataText = nullptr;
    }
    return true;
}

bool DrawCmdFuzzTest069(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    scalar sx = GetObject<scalar>();
    scalar sy = GetObject<scalar>();
    scalar sz = GetObject<scalar>();
    scalar dx = GetObject<scalar>();
    scalar dy = GetObject<scalar>();
    scalar dz = GetObject<scalar>();
    scalar tx = GetObject<scalar>();
    scalar ty = GetObject<scalar>();
    scalar tz = GetObject<scalar>();
    Matrix matrix;
    matrix.SetMatrix(tx, ty, tz, sx, sy, sz, dx, dy, dz);
    SetMatrixOpItem setMatrixOpItem = SetMatrixOpItem(matrix);
    int32_t width = GetObject<int32_t>();
    int32_t height = GetObject<int32_t>();
    Canvas canvas = Canvas(width, height);
    Rect rectT { GetObject<float>(), GetObject<float>(), GetObject<float>(), GetObject<float>() };
    setMatrixOpItem.Playback(&canvas, &rectT);
    return true;
}

bool DrawCmdFuzzTest070(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    size_t length = GetObject<size_t>() % MAX_SIZE + 1;
    char* dataText = new char[length];
    for (size_t i = 0; i < length; i++) {
        dataText[i] = GetObject<char>();
    }
    dataText[length - 1] = '\0';
    std::pair<const void*, size_t> cmdListData;
    cmdListData.first = static_cast<const void*>(dataText);
    cmdListData.second = length;
    auto drawCmdList = DrawCmdList::CreateFromData(cmdListData, false);
    ResetMatrixOpItem::ConstructorHandle constructorHandle = ResetMatrixOpItem::ConstructorHandle();
    ResetMatrixOpItem resetMatrixOpItem = ResetMatrixOpItem();
    resetMatrixOpItem.Marshalling(*drawCmdList);
    ResetMatrixOpItem::Unmarshalling(*drawCmdList, &constructorHandle);
    if (dataText != nullptr) {
        delete [] dataText;
        dataText = nullptr;
    }
    return true;
}

bool DrawCmdFuzzTest071(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    ResetMatrixOpItem resetMatrixOpItem = ResetMatrixOpItem();
    int32_t width = GetObject<int32_t>();
    int32_t height = GetObject<int32_t>();
    Canvas canvas = Canvas(width, height);
    Rect rectT { GetObject<float>(), GetObject<float>(), GetObject<float>(), GetObject<float>() };
    resetMatrixOpItem.Playback(&canvas, &rectT);
    return true;
}

bool DrawCmdFuzzTest072(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    size_t length = GetObject<size_t>() % MAX_SIZE + 1;
    char* dataText = new char[length];
    for (size_t i = 0; i < length; i++) {
        dataText[i] = GetObject<char>();
    }
    dataText[length - 1] = '\0';
    std::pair<const void*, size_t> cmdListData;
    cmdListData.first = static_cast<const void*>(dataText);
    cmdListData.second = length;
    auto drawCmdList = DrawCmdList::CreateFromData(cmdListData, false);
    Matrix::Buffer matrixBuffer;
    for (size_t i = 0; i < Matrix::MATRIX_SIZE; i++) {
        matrixBuffer[i] = GetObject<scalar>();
    }
    ConcatMatrixOpItem::ConstructorHandle constructorHandle = ConcatMatrixOpItem::ConstructorHandle(
        matrixBuffer);
    ConcatMatrixOpItem concatMatrixOpItem = ConcatMatrixOpItem(&constructorHandle);
    concatMatrixOpItem.Marshalling(*drawCmdList);
    ConcatMatrixOpItem::Unmarshalling(*drawCmdList, &constructorHandle);
    if (dataText != nullptr) {
        delete [] dataText;
        dataText = nullptr;
    }
    return true;
}

bool DrawCmdFuzzTest073(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    scalar sx = GetObject<scalar>();
    scalar sy = GetObject<scalar>();
    scalar sz = GetObject<scalar>();
    scalar dx = GetObject<scalar>();
    scalar dy = GetObject<scalar>();
    scalar dz = GetObject<scalar>();
    scalar tx = GetObject<scalar>();
    scalar ty = GetObject<scalar>();
    scalar tz = GetObject<scalar>();
    Matrix matrix;
    matrix.SetMatrix(tx, ty, tz, sx, sy, sz, dx, dy, dz);
    ConcatMatrixOpItem concatMatrixOpItem = ConcatMatrixOpItem(matrix);
    int32_t width = GetObject<int32_t>();
    int32_t height = GetObject<int32_t>();
    Canvas canvas = Canvas(width, height);
    Rect rectT { GetObject<float>(), GetObject<float>(), GetObject<float>(), GetObject<float>() };
    concatMatrixOpItem.Playback(&canvas, &rectT);
    return true;
}

bool DrawCmdFuzzTest074(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;

    DrawOpItem::SetBaseCallback([](std::shared_ptr<Drawing::Image> image) {
        if (image) {
            image->GetWidth();
        }
    });
    auto image = std::make_shared<Drawing::Image>();
    DrawOpItem::holdDrawingImagefunc_(image);

    DrawOpItem::SetTypefaceQueryCallBack([](uint64_t typefaceId) -> std::shared_ptr<Drawing::Typeface> {
        std::shared_ptr<Typeface> typeface;
        if ((typefaceId & 1) == 0) {
            typeface = Typeface::MakeDefault();
        }
        return typeface;
    });
    uint64_t randomTypefaceId = GetObject<uint64_t>();
    DrawOpItem::customTypefaceQueryfunc_(randomTypefaceId);
    return true;
}

bool DrawCmdFuzzTest075(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    size_t length = GetObject<size_t>() % MAX_SIZE + 1;
    char* dataText = new char[length];
    for (size_t i = 0; i < length; i++) {
        dataText[i] = GetObject<char>();
    }
    dataText[length - 1] = '\0';
    std::pair<const void*, size_t> cmdListData;
    cmdListData.first = static_cast<const void*>(dataText);
    cmdListData.second = length;
    auto drawCmdList = DrawCmdList::CreateFromData(cmdListData, false);
    UnmarshallingPlayer unmarshallingPlayer = UnmarshallingPlayer(*drawCmdList);
    UnmarshallingPlayer::UnmarshallingFunc func = nullptr;
    uint32_t type = GetObject<uint32_t>();
    unmarshallingPlayer.RegisterUnmarshallingFunc(type, func);
    uint32_t count = GetObject<uint32_t>() % MAX_SIZE + 1;
    char* handle = new char[count];
    for (size_t i = 0; i < count; i++) {
        handle[i] = GetObject<char>();
    }
    handle[count - 1] = '\0';
    unmarshallingPlayer.Unmarshalling(type, handle);
    if (dataText != nullptr) {
        delete [] dataText;
        dataText = nullptr;
    }
    if (handle != nullptr) {
        delete [] handle;
        handle = nullptr;
    }
    return true;
}

bool DrawCmdFuzzTest076(const uint8_t* data, size_t size)
{
    if (data == nullptr) {
        return false;
    }

    // initialize
    g_data = data;
    g_size = size;
    g_pos = 0;
    size_t length = GetObject<size_t>() % MAX_SIZE + 1;
    char* dataText = new char[length];
    for (size_t i = 0; i < length; i++) {
        dataText[i] = GetObject<char>();
    }
    dataText[length - 1] = '\0';
    std::pair<const void*, size_t> cmdListData;
    cmdListData.first = static_cast<const void*>(dataText);
    cmdListData.second = length;
    auto drawCmdList = DrawCmdList::CreateFromData(cmdListData, false);
    int32_t width = GetObject<int32_t>();
    int32_t height = GetObject<int32_t>();
    Canvas canvas = Canvas(width, height);
    Rect rect { GetObject<float>(), GetObject<float>(), GetObject<float>(), GetObject<float>() };

    GenerateCachedOpItemPlayer generateCachedOpItemPlayer = GenerateCachedOpItemPlayer(*drawCmdList, &canvas, &rect);
    uint32_t type = GetObject<uint32_t>();
    uint32_t count = GetObject<uint32_t>() % MAX_SIZE + 1;
    char* handle = new char[count];
    for (size_t i = 0; i < count; i++) {
        handle[i] = GetObject<char>();
    }
    handle[count - 1] = '\0';
    generateCachedOpItemPlayer.GenerateCachedOpItem(type, handle);
    if (dataText != nullptr) {
        delete [] dataText;
        dataText = nullptr;
    }
    if (handle != nullptr) {
        delete [] handle;
        handle = nullptr;
    }
    return true;
}

} // namespace Drawing
} // namespace Rosen
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::Rosen::Drawing::DrawCmdFuzzTest001(data, size);
    OHOS::Rosen::Drawing::DrawCmdFuzzTest002(data, size);
    OHOS::Rosen::Drawing::DrawCmdFuzzTest003(data, size);
    OHOS::Rosen::Drawing::DrawCmdFuzzTest004(data, size);
    OHOS::Rosen::Drawing::DrawCmdFuzzTest005(data, size);
    OHOS::Rosen::Drawing::DrawCmdFuzzTest006(data, size);
    OHOS::Rosen::Drawing::DrawCmdFuzzTest007(data, size);
    OHOS::Rosen::Drawing::DrawCmdFuzzTest008(data, size);
    OHOS::Rosen::Drawing::DrawCmdFuzzTest009(data, size);
    OHOS::Rosen::Drawing::DrawCmdFuzzTest010(data, size);
    OHOS::Rosen::Drawing::DrawCmdFuzzTest011(data, size);
    OHOS::Rosen::Drawing::DrawCmdFuzzTest012(data, size);
    OHOS::Rosen::Drawing::DrawCmdFuzzTest013(data, size);
    OHOS::Rosen::Drawing::DrawCmdFuzzTest014(data, size);
    OHOS::Rosen::Drawing::DrawCmdFuzzTest015(data, size);
    OHOS::Rosen::Drawing::DrawCmdFuzzTest016(data, size);
    OHOS::Rosen::Drawing::DrawCmdFuzzTest017(data, size);
    OHOS::Rosen::Drawing::DrawCmdFuzzTest018(data, size);
    OHOS::Rosen::Drawing::DrawCmdFuzzTest019(data, size);
    OHOS::Rosen::Drawing::DrawCmdFuzzTest020(data, size);
    OHOS::Rosen::Drawing::DrawCmdFuzzTest021(data, size);
    OHOS::Rosen::Drawing::DrawCmdFuzzTest022(data, size);
    OHOS::Rosen::Drawing::DrawCmdFuzzTest023(data, size);
    OHOS::Rosen::Drawing::DrawCmdFuzzTest024(data, size);
    OHOS::Rosen::Drawing::DrawCmdFuzzTest025(data, size);
    OHOS::Rosen::Drawing::DrawCmdFuzzTest026(data, size);
    OHOS::Rosen::Drawing::DrawCmdFuzzTest027(data, size);
    OHOS::Rosen::Drawing::DrawCmdFuzzTest028(data, size);
    OHOS::Rosen::Drawing::DrawCmdFuzzTest029(data, size);
    OHOS::Rosen::Drawing::DrawCmdFuzzTest030(data, size);
    OHOS::Rosen::Drawing::DrawCmdFuzzTest031(data, size);
    OHOS::Rosen::Drawing::DrawCmdFuzzTest032(data, size);
    OHOS::Rosen::Drawing::DrawCmdFuzzTest033(data, size);
    OHOS::Rosen::Drawing::DrawCmdFuzzTest034(data, size);
    OHOS::Rosen::Drawing::DrawCmdFuzzTest035(data, size);
    OHOS::Rosen::Drawing::DrawCmdFuzzTest036(data, size);
    OHOS::Rosen::Drawing::DrawCmdFuzzTest037(data, size);
    OHOS::Rosen::Drawing::DrawCmdFuzzTest038(data, size);
    OHOS::Rosen::Drawing::DrawCmdFuzzTest039(data, size);
    OHOS::Rosen::Drawing::DrawCmdFuzzTest040(data, size);
    OHOS::Rosen::Drawing::DrawCmdFuzzTest041(data, size);
    OHOS::Rosen::Drawing::DrawCmdFuzzTest042(data, size);
    OHOS::Rosen::Drawing::DrawCmdFuzzTest043(data, size);
    OHOS::Rosen::Drawing::DrawCmdFuzzTest044(data, size);
    OHOS::Rosen::Drawing::DrawCmdFuzzTest045(data, size);
    OHOS::Rosen::Drawing::DrawCmdFuzzTest046(data, size);
    OHOS::Rosen::Drawing::DrawCmdFuzzTest047(data, size);
    OHOS::Rosen::Drawing::DrawCmdFuzzTest048(data, size);
    OHOS::Rosen::Drawing::DrawCmdFuzzTest049(data, size);
    OHOS::Rosen::Drawing::DrawCmdFuzzTest050(data, size);
    OHOS::Rosen::Drawing::DrawCmdFuzzTest051(data, size);
    OHOS::Rosen::Drawing::DrawCmdFuzzTest052(data, size);
    OHOS::Rosen::Drawing::DrawCmdFuzzTest053(data, size);
    OHOS::Rosen::Drawing::DrawCmdFuzzTest054(data, size);
    OHOS::Rosen::Drawing::DrawCmdFuzzTest055(data, size);
    OHOS::Rosen::Drawing::DrawCmdFuzzTest056(data, size);
    OHOS::Rosen::Drawing::DrawCmdFuzzTest057(data, size);
    OHOS::Rosen::Drawing::DrawCmdFuzzTest058(data, size);
    OHOS::Rosen::Drawing::DrawCmdFuzzTest059(data, size);
    OHOS::Rosen::Drawing::DrawCmdFuzzTest060(data, size);
    OHOS::Rosen::Drawing::DrawCmdFuzzTest061(data, size);
    OHOS::Rosen::Drawing::DrawCmdFuzzTest062(data, size);
    OHOS::Rosen::Drawing::DrawCmdFuzzTest063(data, size);
    OHOS::Rosen::Drawing::DrawCmdFuzzTest064(data, size);
    OHOS::Rosen::Drawing::DrawCmdFuzzTest065(data, size);
    OHOS::Rosen::Drawing::DrawCmdFuzzTest066(data, size);
    OHOS::Rosen::Drawing::DrawCmdFuzzTest067(data, size);
    OHOS::Rosen::Drawing::DrawCmdFuzzTest068(data, size);
    OHOS::Rosen::Drawing::DrawCmdFuzzTest069(data, size);
    OHOS::Rosen::Drawing::DrawCmdFuzzTest070(data, size);
    OHOS::Rosen::Drawing::DrawCmdFuzzTest071(data, size);
    OHOS::Rosen::Drawing::DrawCmdFuzzTest072(data, size);
    OHOS::Rosen::Drawing::DrawCmdFuzzTest073(data, size);
    OHOS::Rosen::Drawing::DrawCmdFuzzTest074(data, size);
    OHOS::Rosen::Drawing::DrawCmdFuzzTest075(data, size);
    OHOS::Rosen::Drawing::DrawCmdFuzzTest076(data, size);
    return 0;
}
