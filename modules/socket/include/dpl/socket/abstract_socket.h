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
 * @file        abstract_socket.h
 * @author      Przemyslaw Dobrowolski (p.dobrowolsk@samsung.com)
 * @version     1.0
 * @brief       This file is the header file of abstract socket
 */
#ifndef DPL_ABSTRACT_SOCKET_H
#define DPL_ABSTRACT_SOCKET_H

#include <dpl/abstract_waitable_input_output.h>
#include <dpl/event/event_support.h>
#include <dpl/generic_event.h>
#include <dpl/exception.h>
#include <dpl/address.h>

namespace DPL
{
namespace Socket
{
namespace AbstractSocketEvents
{
// Successfuly connected to server socket
DECLARE_GENERIC_EVENT_0(ConnectedEvent)

// New connection occurred and need to be accepted
DECLARE_GENERIC_EVENT_0(AcceptEvent)

// Connection has read data waiting
DECLARE_GENERIC_EVENT_0(ReadEvent)

// Connection write buffer is now empty again and ready to write more
DECLARE_GENERIC_EVENT_0(WriteEvent)
} // namespace AbstractSocketEvents

class AbstractSocket
    : public AbstractWaitableInputOutput,
      public DPL::Event::EventSupport<AbstractSocketEvents::ConnectedEvent>,
      public DPL::Event::EventSupport<AbstractSocketEvents::AcceptEvent>,
      public DPL::Event::EventSupport<AbstractSocketEvents::ReadEvent>,
      public DPL::Event::EventSupport<AbstractSocketEvents::WriteEvent>
{
public:
    class Exception
    {
    public:
        DECLARE_EXCEPTION_TYPE(DPL::Exception, Base)       ///< Base abstract socket exception

        DECLARE_EXCEPTION_TYPE(Base, OpenFailed)           ///< Fatal error occurred during open socket descriptor. Socket state is inconsistent and it should be not used anymore.

        DECLARE_EXCEPTION_TYPE(Base, ConnectFailed)        ///< Fatal error occurred during connect. Socket state is inconsistent and it should be not used anymore.
                                                           ///< Warning: This exception does not mean that socket did not succeed to connect, see CannotConnect
                                                           ///< for this specific scenario

        DECLARE_EXCEPTION_TYPE(Base, SetNonBlockingFailed) ///< Fatal error occurred during setting socket to non-blocking mode. Socket state is inconsistent and it should be not used anymore.

        DECLARE_EXCEPTION_TYPE(Base, BindFailed)           ///< Fatal error occurred during bind. Socket state is inconsistent and it should be not used anymore.

        DECLARE_EXCEPTION_TYPE(Base, AcceptFailed)         ///< Fatal error occurred during accept. Socket state is inconsistent and it should be not used anymore.

        DECLARE_EXCEPTION_TYPE(Base, ListenFailed)         ///< Fatal error occurred during listen. Socket state is inconsistent and it should be not used anymore.

        DECLARE_EXCEPTION_TYPE(Base, CloseFailed)          ///< Fatal error occurred during close. Socket state is inconsistent and it should be not used anymore.

        DECLARE_EXCEPTION_TYPE(Base, ReadFailed)           ///< Fatal error occurred during read. Socket state is inconsistent and it should be not used anymore.
                                                           ///< Warning: This exception does not mean that connection was broken, see ConnectionBroken
                                                           ///< for this specific scenario

        DECLARE_EXCEPTION_TYPE(Base, WriteFailed)          ///< Fatal error occurred during write. Socket state is inconsistent and it should be not used anymore.
                                                           ///< Warning: This exception does not mean that connection was broken, see ConnectionBroken
                                                           ///< for this specific scenario

        DECLARE_EXCEPTION_TYPE(Base, GetPeerNameFailed)    ///< Fatal error occurred during getpeername or getsockname. Socket state is inconsistent and it should be not used anymore.

        DECLARE_EXCEPTION_TYPE(Base, CannotConnect)        ///< Cannot connect to remote socket. This is not fatal error, socket state is still consistent and it can be reconnected.

        DECLARE_EXCEPTION_TYPE(Base, ConnectionBroken)     ///< Connection was broken. This is not fatal error, socket state is still consistent and it can be reconnected.
    };

public:
    virtual ~AbstractSocket() {}

    // Connect to remote host
    virtual void Connect(const Address &address) = 0;

    // Open empty, unconnected socket
    virtual void Open() = 0;

    // Close connection
    virtual void Close() = 0;

    // Bind server socket address
    virtual void Bind(const Address &address) = 0;

    // Begin listening for incoming connections
    virtual void Listen(int backlog) = 0;

    // Accept waiting connection and create derived class connection
    // One should cast resulting pointer to derived class
    virtual AbstractSocket *Accept() = 0;

    // Local socket address
    virtual Address GetLocalAddress() const = 0;

    // Remote socket address
    virtual Address GetRemoteAddress() const = 0;
};

}
} // namespace DPL

#endif // DPL_ABSTRACT_SOCKET_H
