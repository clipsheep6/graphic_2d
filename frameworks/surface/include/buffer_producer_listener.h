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

#ifndef INTERFACES_INNERKITS_SURFACE_BUFFER_PRODUCER_LISTENER_H
#define INTERFACES_INNERKITS_SURFACE_BUFFER_PRODUCER_LISTENER_H

#include <refbase.h>
#include "iremote_broker.h"
#include "buffer_log.h"
#include "surface_buffer.h"
#include "ibuffer_producer_listener.h"
#include "iremote_proxy.h"
#include "iremote_stub.h"
#include "message_option.h"

namespace OHOS {
class ProducerListenerProxy : public IRemoteProxy<IProducerListener> {
public:
    explicit ProducerListenerProxy(const sptr<IRemoteObject>& impl) : IRemoteProxy<IProducerListener>(impl) {};
    virtual ~ProducerListenerProxy() noexcept = default;
    GSError OnBufferReleased() override
    {
        MessageOption option;
        MessageParcel arguments;
        MessageParcel reply;
        if (!arguments.WriteInterfaceToken(IProducerListener::GetDescriptor())) {
            BLOGE("write interface token failed");
            return GSERROR_BINDER;
        }
        option.SetFlags(MessageOption::TF_ASYNC);
        int32_t ret = Remote()->SendRequest(IProducerListener::ON_BUFFER_RELEASED, arguments, reply, option);
        if (ret != ERR_NONE) {
            return GSERROR_BINDER;
        }
        return GSERROR_OK;   
    };
private:
    static inline BrokerDelegator<ProducerListenerProxy> delegator_;
};

class ProducerListenerStub : public IRemoteStub<IProducerListener> {
public:
    ProducerListenerStub() = default;
    ~ProducerListenerStub() = default;
    int32_t OnRemoteRequest(uint32_t code, MessageParcel &arguments, MessageParcel &reply, MessageOption &option) override
    {
        (void)(option);
        auto remoteDescriptor = arguments.ReadInterfaceToken();
        if (GetDescriptor() != remoteDescriptor) {
            return ERR_INVALID_STATE;
        }

        auto ret = ERR_NONE;
        switch (code) {
            case ON_BUFFER_RELEASED: {
                auto sret = OnBufferReleased();
                reply.WriteInt32(sret);
                break;
            }
            default: {
                ret = ERR_UNKNOWN_TRANSACTION;
                break;
            }
        }
        return ret;
    };
};

class BufferReleaseProducerListener : public ProducerListenerStub
{
public:
    explicit BufferReleaseProducerListener(OnReleaseFunc func): func_(func) {};
    ~BufferReleaseProducerListener() override {};
    GSError OnBufferReleased() override
    {
        if(func_ != nullptr) {
            sptr<OHOS::SurfaceBuffer> ttt = nullptr;
            return func_(ttt);
        }
        return GSERROR_INTERNAL;
    };
private:
    OnReleaseFunc func_;
};


} // namespace OHOS

#endif // INTERFACES_INNERKITS_SURFACE_BUFFER_PRODUCER_LISTENER_H
