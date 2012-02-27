/*
 * Copyright (c) 2011 Samsung Electronics Co., Ltd All Rights Reserved
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */
/*
 * @file        unix_socket.cpp
 * @author      Przemyslaw Dobrowolski (p.dobrowolsk@samsung.com)
 * @version     1.0
 * @brief       This file is the implementation file of unix socket
 */
#include <dpl/socket/unix_socket.h>
#include <dpl/log/log.h>
#include <dpl/exception.h>
#include <new>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

namespace DPL
{
namespace Socket
{

UnixSocket::UnixSocket()
{
}

int UnixSocket::AllocSpecificDescriptor() const
{
    LogPedantic("Creating UNIX socket...");

    // Create new descriptor
    int newSocket = socket(AF_UNIX, SOCK_STREAM, 0);

    if (newSocket == -1)
        Throw(Exception::CreateFailed);

    LogPedantic("UNIX socket created");

    // Return new descriptor
    return newSocket;
}

std::pair<sockaddr *, socklen_t> UnixSocket::TranslateAddressGenericToSpecific(const Address &address) const
{
    // Allocate new socket address structure
    sockaddr_un *sockAddress = static_cast<sockaddr_un *>(malloc(sizeof(sockaddr_un)));
    if (!sockAddress) throw std::bad_alloc();

    memset(sockAddress, 0, sizeof(sockaddr_un));

    // Copy address properties
    sockAddress->sun_family = AF_UNIX;
    strncpy(sockAddress->sun_path, address.GetAddress().c_str(), sizeof(sockAddress->sun_path) - 1);
    sockAddress->sun_path[sizeof(sockAddress->sun_path) - 1] = '\0'; // Prevent buffer overflows

    // Set proper address length
    socklen_t sockAddressLength = SUN_LEN(sockAddress);

    // Return new translated address
    return std::make_pair(reinterpret_cast<sockaddr *>(sockAddress), sockAddressLength);
}

Address UnixSocket::TranslateAddressSpecificToGeneric(sockaddr *address, socklen_t) const
{
    // FIXME: Constrain check ?
    sockaddr_un *unixAddress = reinterpret_cast<sockaddr_un *>(address);
    return Address(unixAddress->sun_path);
}

socklen_t UnixSocket::GetSpecificAddressSize() const
{
    return static_cast<socklen_t>(sizeof(sockaddr_un));
}

UnixSocket *UnixSocket::AllocAcceptedSpecificSocket() const
{
    return new UnixSocket();
}

void UnixSocket::Bind(const Address &address)
{
    // Always remove socket file if any
    unlink(address.GetAddress().c_str());

    // Call base implementation
    GenericSocket<UnixSocket>::Bind(address);

    // Always set proper permissions to the socket file
    chmod(address.GetAddress().c_str(), 0777);
}

}
} // namespace DPL
