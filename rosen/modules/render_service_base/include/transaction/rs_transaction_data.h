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

#ifndef ROSEN_RENDER_SERVICE_BASE_RS_TRANSACTION_DATA_H
#define ROSEN_RENDER_SERVICE_BASE_RS_TRANSACTION_DATA_H

#include <memory>
#include <vector>

#include "command/rs_command.h"
#include "pipeline/rs_context.h"

#include <parcel.h>

namespace OHOS {
namespace Rosen {
class RS_EXPORT RSTransactionData : public Parcelable {
public:
    RSTransactionData() = default;
    RSTransactionData(RSTransactionData&& other) : commands_(std::move(other.commands_)) {}
    ~RSTransactionData() noexcept;

    static RSTransactionData* Unmarshalling(Parcel& parcel);
    bool Marshalling(Parcel& parcel) const override;

    int GetCommandCount() const
    {
        return commands_.size();
    }

    bool IsEmpty() const
    {
        return commands_.empty();
    }

    void Process(RSContext& context);

    void Clear();

private:
    void AddCommand(std::unique_ptr<RSCommand>& command);
    void AddCommand(std::unique_ptr<RSCommand>&& command);

    bool UnmarshallingCommand(Parcel& parcel);

    std::vector<std::unique_ptr<RSCommand>> commands_;

    friend class RSTransactionProxy;
    friend class RSMessageProcessor;
};

} // namespace Rosen
} // namespace OHOS

#endif // ROSEN_RENDER_SERVICE_BASE_RS_TRANSACTION_DATA_H
