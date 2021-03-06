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
 * @file        unix_socket_rpc_server.h
 * @author      Przemyslaw Dobrowolski (p.dobrowolsk@samsung.com)
 * @version     1.0
 * @brief       This file is the header file for unix socket RPC server
 */
#ifndef DPL_UNIX_SOCKET_RPC_SERVER_H
#define DPL_UNIX_SOCKET_RPC_SERVER_H

#include <dpl/rpc/generic_socket_rpc_server.h>
#include <dpl/socket/unix_socket.h>
#include <string>

namespace DPL
{
namespace RPC
{

class UnixSocketRPCServer
    : public GenericSocketRPCServer<DPL::Socket::UnixSocket>
{
protected:
    virtual AbstractRPCConnection *OpenSpecificConnection(DPL::Socket::UnixSocket *socket);

public:
    AbstractRPCConnectionID Open(const std::string &fileName);
};

}
} // namespace DPL

#endif // DPL_UNIX_SOCKET_RPC_SERVER_H
