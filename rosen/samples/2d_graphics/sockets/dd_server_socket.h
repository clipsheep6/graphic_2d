/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef DD_SERVERSOCKET_H
#define DD_SERVERSOCKET_H
#include <iostream>
#include <netinet/in.h>
namespace OHOS {
namespace Rosen {
class DdServerSocket {
public:
    DdServerSocket();
    ~DdServerSocket();

    int Init();
    // connect to client
    int Listen();
    int Accept();
    // IO
    ssize_t Recv();
    ssize_t Send(const std::string &sendStr);
    void Close() const;
    void CloseClient() const;
    std::string GetRecvBuf() const;

private:
    int serverSockFd_ = -1;
    int clientSockFd_ = -1;
    struct sockaddr_in serverAddr_;
    struct sockaddr_in clientAddr_;
    const int sockPort_ = 0; // sockPort_ are allocated by the kernel.
    const static int buffSizeRecv_ = 1024;
    const static int maxConnectionNum_ = 8;
    char rbuf_[buffSizeRecv_] = "";
};
}
}

#endif // !SPSERVERSOCKET