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

#include "dd_server_socket.h"
#include <sstream>
#include <cstring>
#include <sys/socket.h>

namespace OHOS {
namespace Rosen {
DdServerSocket::DdServerSocket() {}

DdServerSocket::~DdServerSocket()
{
    Close();
}

int DdServerSocket::Init()
{
    serverSockFd_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSockFd_ < 0) {
        std::cout << "Socket Create Failed:" << serverSockFd_ << std::endl;
        return -1;
    }
    serverAddr_.sin_family = AF_INET;
    serverAddr_.sin_port = htons(sockPort_);
    serverAddr_.sin_addr.s_addr = htonl(INADDR_ANY);
    if (::bind(serverSockFd_, reinterpret_cast<struct sockaddr *>(&serverAddr_), sizeof(serverAddr_)) < 0) {
        std::cout << "Socket Bind failed:" << serverSockFd_ << std::endl;
        return -1;
    }
    return 0;
}

int DdServerSocket::Listen()
{
    return listen(serverSockFd_, maxConnectionNum_);
}

int DdServerSocket::Accept()
{
    socklen_t clientAddrLen = sizeof(clientAddr_);
    clientSockFd_ = accept(serverSockFd_, reinterpret_cast<struct sockaddr *>(&clientAddr_), &clientAddrLen);
    return clientSockFd_;
}

ssize_t DdServerSocket::Recv()
{
    bzero(rbuf_, sizeof(rbuf_));
    ssize_t len = recv(clientSockFd_, rbuf_, buffSizeRecv_, 0);
    if (len > 0) {
        std::cout << "Client:" << rbuf_ << std::endl;
    }
    return len;
}

ssize_t DdServerSocket::Send(const std::string &sendStr)
{
    return send(clientSockFd_, sendStr.c_str(), sendStr.size(), 0);
}

void DdServerSocket::CloseClient() const
{
    if(clientSockFd_ != -1){
        shutdown(clientSockFd_, SHUT_RD);
        clientSockFd_ = -1;
    }
}

void DdServerSocket::Close() const
{
    CloseClient();
    if(serverSockFd_ != -1){
        shutdown(serverSockFd_, SHUT_RD);
        serverSockFd_ = -1;
    }
}

std::string DdServerSocket::GetRecvBuf() const
{
    std::string recvBuf = rbuf_;
    return recvBuf;
}
}
}
