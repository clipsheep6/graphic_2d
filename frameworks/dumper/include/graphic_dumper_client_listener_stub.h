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

#ifndef FRAMEWORKS_DUMPRE_INCLUDE_GRAPHIC_DUMPER_CLIENT_LISTENER_STUB_H
#define FRAMEWORKS_DUMPRE_INCLUDE_GRAPHIC_DUMPER_CLIENT_LISTENER_STUB_H

#include <iremote_stub.h>
#include <message_parcel.h>
#include <message_option.h>

#include "igraphic_dumper_client_listener.h"

namespace OHOS {
class GraphicDumperClientListenerStub : public IRemoteStub<IGraphicDumperClientListener> {
public:
    virtual int32_t OnRemoteRequest(uint32_t code, MessageParcel& data,
                            MessageParcel& reply, MessageOption& option) override;
};
} // namespace OHOS

#endif // FRAMEWORKS_DUMPRE_INCLUDE_GRAPHIC_DUMPER_CLIENT_LISTENER_STUB_H
