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
 * @file        unix_socket_rpc_client.cpp
 * @author      Przemyslaw Dobrowolski (p.dobrowolsk@samsung.com)
 * @version     1.0
 * @brief       This file is the implementation file of unix socket RPC client
 */
#include <dpl/rpc/unix_socket_rpc_client.h>
#include <dpl/rpc/unix_socket_rpc_connection.h>

namespace DPL
{
namespace RPC
{
AbstractRPCConnection *UnixSocketRPCClient::OpenSpecificConnection(DPL::Socket::UnixSocket *socket)
{
    // Allocate new UNIX/RPC connection object
    UnixSocketRPCConnection *connection = new UnixSocketRPCConnection(socket);

    // Return new connection
    return connection;
}

AbstractRPCConnectionID UnixSocketRPCClient::Open(const std::string &fileName)
{
    return GenericSocketRPCClient<DPL::Socket::UnixSocket>::Open(Address(fileName));
}
}
} // namespace DPL
