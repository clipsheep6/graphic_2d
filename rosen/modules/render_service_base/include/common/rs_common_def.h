/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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
#ifndef RENDER_SERVICE_CLIENT_CORE_COMMON_RS_COMMON_DEF_H
#define RENDER_SERVICE_CLIENT_CORE_COMMON_RS_COMMON_DEF_H

#include <cmath>
#include <limits>
#include <map>
#include <memory>
#include <mutex>
#include <vector>

#include "common/rs_macros.h"

namespace OHOS {
namespace Rosen {
using AnimationId = uint64_t;
using NodeId = uint64_t;
using PropertyId = uint64_t;

// types in the same layer should be 0/1/2/4/8
// types for UINode
enum class RSUINodeType : uint32_t {
    BASE_NODE    = 0x00010u,
    RS_NODE      = 0x00110u,   // formerly RSPropertyNode
    DISPLAY_NODE = 0x01110u,
    SURFACE_NODE = 0x02110u,
    CANVAS_NODE  = 0x04110u,   // formerly RSNode
    ROOT_NODE    = 0x14110u,
};

enum class FollowType : uint8_t {
    NONE,
    FOLLOW_TO_PARENT,
    FOLLOW_TO_SELF,
};

static const std::map<RSUINodeType, std::string> RSUINodeTypeStrs = {
    {RSUINodeType::BASE_NODE,    "BaseNode"},
    {RSUINodeType::DISPLAY_NODE, "DisplayNode"},
    {RSUINodeType::RS_NODE,      "RsNode"},
    {RSUINodeType::SURFACE_NODE, "SurfaceNode"},
    {RSUINodeType::CANVAS_NODE,  "CanvasNode"},
    {RSUINodeType::ROOT_NODE,    "RootNode"},
};

// types for RenderNode
enum class RSRenderNodeType : uint32_t {
    BASE_NODE    = 0x00011u,
    RS_NODE      = 0x00111u,   // formerly RSPropertyRenderNode
    DISPLAY_NODE = 0x01111u,
    SURFACE_NODE = 0x02111u,
    CANVAS_NODE  = 0x04111u,   // formerly RSRenderNode
    ROOT_NODE    = 0x14111u,
};

struct RSSurfaceRenderNodeConfig {
    NodeId id = 0;
    std::string name = "SurfaceNode";
    bool isUni = false;
};

struct RSDisplayNodeConfig {
    uint64_t screenId = 0;
    bool isMirrored = false;
    NodeId mirrorNodeId = 0;
};

#if defined(M_PI)
const float PI = M_PI;
#else
const float PI = std::atanf(1.0) * 4;
#endif

template<typename T>
inline bool ROSEN_EQ(const T& x, const T& y)
{
    return x == y;
}

template<>
inline bool ROSEN_EQ(const float& x, const float& y)
{
    return (std::abs((x) - (y)) <= (std::numeric_limits<float>::epsilon()));
}

template<>
inline bool ROSEN_EQ(const double& x, const double& y)
{
    return (std::abs((x) - (y)) <= (std::numeric_limits<double>::epsilon()));
}

template<>
inline bool ROSEN_EQ(const long double& x, const long double& y)
{
    return (std::abs((x) - (y)) <= (std::numeric_limits<long double>::epsilon()));
}

template<typename T>
inline bool ROSEN_EQ(T x, T y, T epsilon)
{
    return (std::abs((x) - (y)) <= (epsilon));
}

template<typename T>
inline bool ROSEN_EQ(const std::weak_ptr<T>& x, const std::weak_ptr<T>& y)
{
    return !(x.owner_before(y) || y.owner_before(x));
}

class MemObject {
public:
    explicit MemObject(size_t size) : size_(size) {}
    virtual ~MemObject() {}

    void* operator new(size_t size);
    void operator delete(void* ptr);

    void* operator new(std::size_t size, const std::nothrow_t&) noexcept;
    void operator delete(void* ptr, const std::nothrow_t&) noexcept;

protected:
    size_t size_;
};
} // namespace Rosen
} // namespace OHOS
#endif // RENDER_SERVICE_CLIENT_CORE_COMMON_RS_COMMON_DEF_H
