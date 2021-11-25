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

#ifndef FRAMEWORKS_DUMPRE_INCLUDE_GRAPHIC_DUMPER_SERVICE_PROXY_H
#define FRAMEWORKS_DUMPRE_INCLUDE_GRAPHIC_DUMPER_SERVICE_PROXY_H

#include <iremote_object.h>
#include <iremote_proxy.h>

#include "ipc/igraphic_dumper_service.h"

namespace OHOS {
class GraphicDumperServiceProxy : public IRemoteProxy<IGraphicDumperService> {
public:
    GraphicDumperServiceProxy(const sptr<IRemoteObject>& impl);
    virtual ~GraphicDumperServiceProxy() = default;
    GSError AddClientListener(const std::string &tag, sptr<IGraphicDumperClientListener> &listener) override;
    GSError SendInfo(const std::string &tag, const std::string &info) override;

private:
    static inline BrokerDelegator<GraphicDumperServiceProxy> delegator_;
};
} // namespace OHOS

#endif // FRAMEWORKS_DUMPRE_INCLUDE_GRAPHIC_DUMPER_SERVICE_PROXY_H
