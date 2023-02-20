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

#ifndef RS_NODE_COST_MANAGER_H
#define RS_NODE_COST_MANAGER_H

#include "pipeline/rs_render_node.h"

namespace OHOS::Rosen {
class RS_EXPORT RSNodeCostManager {
public:
    RSNodeCostManager();
    ~RSNodeCostManager() = default;
    void AddNodeCost(int32_t cost);
    void Reset();
    int32_t GetDirtyNodeCost();
private:
    int32_t dirtyNodeCost_ = 0;
};
}
#endif // RS_NODE_COST_MANAGER_H