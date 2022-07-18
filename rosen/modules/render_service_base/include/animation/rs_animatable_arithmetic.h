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

#ifndef RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_ANIMATABLE_ARITHMETRIC_H
#define RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_ANIMATABLE_ARITHMETRIC_H

#include "common/rs_macros.h"

namespace OHOS {
namespace Rosen {
template<typebame T>
class RS_EXPORT RSAnimatableArithmetic {
public:
    RSAnimatableArithmetic(const T& data);
    virtual ~RSAnimatableArithmetic() = default;

    RSAnimatableArithmetic operator+(const RSAnimatableArithmetic<T>& other) const;
    RSAnimatableArithmetic& operator+=(const RSAnimatableArithmetic<T>& other);
    RSAnimatableArithmetic operator-(const RSAnimatableArithmetic<T>& other) const;
    RSAnimatableArithmetic& operator-=(const RSAnimatableArithmetic<T>& other);
    RSAnimatableArithmetic operator*(const RSAnimatableArithmetic<T>& other) const;
    RSAnimatableArithmetic& operator*=(const RSAnimatableArithmetic<T>& other);
    RSAnimatableArithmetic operator/(const RSAnimatableArithmetic<T>& other) const;
    RSAnimatableArithmetic& operator/=(const RSAnimatableArithmetic<T>& other);
    RSAnimatableArithmetic& operator=(const RSAnimatableArithmetic<T>& other);
    bool operator==(const RSAnimatableArithmetic<T>& other) const;

    T data_;
}

template<typename T>
RSAnimatableArithmetic<T>::RSAnimatableArithmetic(const T& data)
{
    data_ = data;
}

template<typename T>
RSAnimatableArithmetic<T> RSAnimatableArithmetic<T>::operator+(
    const RSAnimatableArithmetic<T>& other) const
{
    RSAnimatableArithmetic<T> result(data_ + other.data_);
    return result;
}

template<typename T>
RSAnimatableArithmetic<T>& RSAnimatableArithmetic<T>::operator+=(
    const RSAnimatableArithmetic<T>& other)
{
    data_ += other.data_;
    return *this;
}

template<typename T>
RSAnimatableArithmetic<T> RSAnimatableArithmetic<T>::operator-(
    const RSAnimatableArithmetic<T>& other) const
{
    RSAnimatableArithmetic<T> result(data_ - other.data_);
    return result;
}

template<typename T>
RSAnimatableArithmetic<T>& RSAnimatableArithmetic<T>::operator-=(
    const RSAnimatableArithmetic<T>& other)
{
    data_ -= other.data_;
    return *this;
}

template<typename T>
RSAnimatableArithmetic<T> RSAnimatableArithmetic<T>::operator*(
    const RSAnimatableArithmetic<T>& other) const
{
    RSAnimatableArithmetic<T> result(data_ * other.data_);
    return result;
}

template<typename T>
RSAnimatableArithmetic<T>& RSAnimatableArithmetic<T>::operator*=(
    const RSAnimatableArithmetic<T>& other)
{
    data_ *= other.data_;
    return *this;
}

template<typename T>
RSAnimatableArithmetic<T> RSAnimatableArithmetic<T>::operator/(
    const RSAnimatableArithmetic<T>& other) const
{
    RSAnimatableArithmetic<T> result(data_ / other.data_);
    return result;
}

template<typename T>
RSAnimatableArithmetic<T>& RSAnimatableArithmetic<T>::operator/=(
    const RSAnimatableArithmetic<T>& other)
{
    data_ /= other.data_;
    return *this;
}

template<typename T>
RSAnimatableArithmetic<T>& RSAnimatableArithmetic<T>::operator=(
    const RSAnimatableArithmetic<T>& other)
{
    data_ = other.data_;
    return *this;
}

template<typename T>
bool RSAnimatableArithmetic<T>::operator==(const RSAnimatableArithmetic<T>& other) const
{
    return data_ == other.data_;
}
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_ANIMATION_RS_ANIMATABLE_ARITHMETRIC_H
