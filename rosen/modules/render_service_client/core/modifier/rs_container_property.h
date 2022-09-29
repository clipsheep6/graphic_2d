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

#ifndef RENDER_SERVICE_CLIENT_CORE_MODIFIER_RS_CONTAINER_PROPERTY_H
#define RENDER_SERVICE_CLIENT_CORE_MODIFIER_RS_CONTAINER_PROPERTY_H

#include <unordered_map>

#include "common/rs_macros.h"
#include "modifier/rs_modifier_type.h"
#include "modifier/rs_property.h"

namespace OHOS {
namespace Rosen {
class RS_EXPORT RSContainerProperty : public RSPropertyBase {
public:
    RSContainerProperty() = default;
    ~RSContainerProperty() = default;

    void AddProperty(int32_t propertyKey, std::shared_ptr<RSPropertyBase>& property)
    {
        auto iter = properties_.find(propertyKey);
        if (iter != properties_.end()) {
            ROSEN_LOGE("RSContainerProperty::AddSubProperty, property with key %d is already existed", propertyKey);
            return;
        }
        properties_.insert({ propertyKey, property });
    }

    std::shared_ptr<RSPropertyBase> GetProperty(int32_t propertyKey)
    {
        auto iter = properties_.find(propertyKey);
        if (iter == properties_.end()) {
            ROSEN_LOGE("RSContainerProperty::GetSubProperty, property with key %d is not existed", propertyKey);
            return nullptr;
        }
        return properties_[propertyKey];
    }

protected:
    void SetIsCustom(bool isCustom) override
    {
        for (auto& [propertyKey, property] : properties_) {
            property->SetIsCustom(isCustom);
        }
    }

    void SetValue(const std::shared_ptr<RSPropertyBase>& value) override
    {
        for (auto& [propertyKey, property] : properties_) {
            property->SetValue(value);
        }
    }

    RSRenderPropertyType GetPropertyType() const override
    {
        return RSRenderPropertyType::PROPERTY_CONTAINER;
    }

private:
    std::unordered_map<int32_t, std::shared_ptr<RSPropertyBase>> properties_;
};
} // namespace Rosen
} // namespace OHOS

#endif // RENDER_SERVICE_CLIENT_CORE_MODIFIER_RS_CONTAINER_PROPERTY_H
