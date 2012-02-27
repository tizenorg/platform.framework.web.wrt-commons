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
 * @file        generic_socket_rpc_client.h
 * @author      Przemyslaw Dobrowolski (p.dobrowolsk@samsung.com)
 * @version     1.0
 * @brief       This file is the header file for generic socket RPC client
 */
#ifndef DPL_GENERIC_SOCKET_RPC_CLIENT_H
#define DPL_GENERIC_SOCKET_RPC_CLIENT_H

#include <dpl/rpc/abstract_rpc_connector.h>
#include <dpl/socket/abstract_socket.h>
#include <set>

namespace DPL
{
namespace RPC
{

template<typename SocketType>
class GenericSocketRPCClient
    : public AbstractRPCConnector,
      private DPL::Event::EventListener<DPL::Socket::AbstractSocketEvents::ConnectedEvent>
{
public:
    class Exception
    {
    public:
        DECLARE_EXCEPTION_TYPE(DPL::Exception, Base)
        DECLARE_EXCEPTION_TYPE(Base, OpenFailed)
        DECLARE_EXCEPTION_TYPE(Base, CloseFailed)
    };

protected:
    // Derived class implementations for connection managment
    virtual AbstractRPCConnection *OpenSpecificConnection(SocketType *socket) = 0;

private:
    typedef std::set<SocketType *> InternalConnectionSet;
    InternalConnectionSet m_internalConnectionSet;

    virtual void OnEventReceived(const DPL::Socket::AbstractSocketEvents::ConnectedEvent &event)
    {
        // Retrieve socket sender
        SocketType *socket = static_cast<SocketType *>(event.GetSender());

        LogPedantic("Connection with RPC server established");

        // Is this connection still tracked ?
        // It might have disappeared on close
        typename InternalConnectionSet::iterator iterator = m_internalConnectionSet.find(socket);

        if (iterator == m_internalConnectionSet.end())
        {
            LogPedantic("RPC client connection socket disappeared");
            return;
        }

        // Open specific connection implementation
        AbstractRPCConnection *connection = OpenSpecificConnection(socket);

        // Remove internal connection
        socket->EventSupport<DPL::Socket::AbstractSocketEvents::ConnectedEvent>::RemoveListener(this);
        m_internalConnectionSet.erase(iterator);

        // Retrieve ID once again
        AbstractRPCConnectionID connectionID = static_cast<AbstractRPCConnectionID>(socket);

        // Inform listeners
        DPL::Event::EventSupport<AbstractRPCConnectorEvents::ConnectionEstablishedEvent>::
            EmitEvent(AbstractRPCConnectorEvents::ConnectionEstablishedEvent(
                connectionID, connection, EventSender(this)), DPL::Event::EmitMode::Queued);
    }

public:
    explicit GenericSocketRPCClient()
    {
    }

    virtual ~GenericSocketRPCClient()
    {
        // Always close all connections
        CloseAll();
    }

    AbstractRPCConnectionID Open(const Address &socketAddress)
    {
        LogPedantic("Starting client: " << socketAddress.ToString());

        // Alloc new socket
        SocketType *socket = new SocketType();

        // Add socket listeners
        socket->EventSupport<DPL::Socket::AbstractSocketEvents::ConnectedEvent>::AddListener(this);

        Try
        {
            // Open socket
            socket->Open();

            // Start connecting to server
            socket->Connect(Address(socketAddress));
        }
        Catch (DPL::Socket::AbstractSocket::Exception::Base)
        {
            // Remove back socket listener
            socket->EventSupport<DPL::Socket::AbstractSocketEvents::ConnectedEvent>::RemoveListener(this);

            // Log debug message
            LogPedantic("Cannot connect to: " << socketAddress.ToString());

            // Problem with client startup
            ReThrowMsg(typename Exception::OpenFailed, socketAddress.ToString());
        }

        // Register new internal connection
        m_internalConnectionSet.insert(socket);

        // Debug info
        LogPedantic("Client started on interface: " << socket->GetLocalAddress().ToString());

        // Return unique identifier
        return static_cast<AbstractRPCConnectionID>(socket);
    }

    void Close(AbstractRPCConnectionID connectionID)
    {
        LogPedantic("Closing client interface...");

        // Get socket from ID
        SocketType *socket = static_cast<SocketType *>(connectionID);

        // Find corresponding internal connection
        typename InternalConnectionSet::iterator iterator = m_internalConnectionSet.find(socket);

        if (iterator == m_internalConnectionSet.end())
            return;

        // Close socket
        socket->Close();

        // Remove internal socket
        socket->EventSupport<DPL::Socket::AbstractSocketEvents::ConnectedEvent>::RemoveListener(this);
        delete socket;

        m_internalConnectionSet.erase(iterator);

        // Done
        LogPedantic("Closed");
    }

    void CloseAll()
    {
        while (!m_internalConnectionSet.empty())
            Close(static_cast<AbstractRPCConnectionID>(*m_internalConnectionSet.begin()));
    }
};

}
} // namespace DPL

#endif // DPL_GENERIC_SOCKET_RPC_CLIENT_H
