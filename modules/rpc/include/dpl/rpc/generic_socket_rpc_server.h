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
 * @file        generic_socket_rpc_server.h
 * @author      Przemyslaw Dobrowolski (p.dobrowolsk@samsung.com)
 * @version     1.0
 * @brief       This file is the header file for generic socket RPC server
 */
#ifndef DPL_GENERIC_SOCKET_RPC_SERVER_H
#define DPL_GENERIC_SOCKET_RPC_SERVER_H

#include <dpl/rpc/abstract_rpc_connector.h>
#include <dpl/socket/abstract_socket.h>
#include <set>

namespace DPL
{
namespace RPC
{

template<typename SocketType>
class GenericSocketRPCServer
    : public AbstractRPCConnector,
      private DPL::Event::EventListener<DPL::Socket::AbstractSocketEvents::AcceptEvent>
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
    typedef std::set<SocketType *> InternalInterfaceSet;
    InternalInterfaceSet m_internalInterfacesSet;

    virtual void OnEventReceived(const DPL::Socket::AbstractSocketEvents::AcceptEvent &event)
    {
        // Retrieve socket sender
        SocketType *server = static_cast<SocketType *>(event.GetSender());

        // Is this interface still tracked ?
        // It might have disappeared on close
        typename InternalInterfaceSet::iterator iterator = m_internalInterfacesSet.find(server);

        if (iterator == m_internalInterfacesSet.end())
        {
            LogPedantic("RPC server interface socket disappeared");
            return;
        }

        // Accept incoming client
        SocketType *client = static_cast<SocketType *>(server->Accept());
        if(client == NULL)
        {
            LogPedantic("Spontaneous accept on socket occurred");
            return;
        }

        LogPedantic("Client connected to server: " << client->GetRemoteAddress().ToString());

        // Open specific connection implementation
        AbstractRPCConnection *connection = OpenSpecificConnection(client);

        // Retrieve ID once again
        AbstractRPCConnectionID connectionID = static_cast<AbstractRPCConnectionID>(server);

        // Inform listeners
        DPL::Event::EventSupport<AbstractRPCConnectorEvents::ConnectionEstablishedEvent>::
            EmitEvent(AbstractRPCConnectorEvents::ConnectionEstablishedEvent(
                connectionID, connection, EventSender(this)), DPL::Event::EmitMode::Queued);
    }

public:
    explicit GenericSocketRPCServer()
    {
    }

    virtual ~GenericSocketRPCServer()
    {
        // Always close connection
        CloseAll();
    }

    AbstractRPCConnectionID Open(const Address &socketAddress)
    {
        LogPedantic("Starting server: " << socketAddress.ToString());

        // Alloc new socket
        SocketType *socket = new SocketType();

        // Add socket listener
        socket->EventSupport<DPL::Socket::AbstractSocketEvents::AcceptEvent>::AddListener(this);

        Try
        {
            // Open socket
            socket->Open();

            // Bind socket address
            socket->Bind(socketAddress);

            // Start listening
            socket->Listen(8);
        }
        Catch (DPL::Socket::AbstractSocket::Exception::Base)
        {
            // Remove back socket listener
            socket->EventSupport<DPL::Socket::AbstractSocketEvents::AcceptEvent>::RemoveListener(this);

            // Log debug
            LogPedantic("Cannot start server: " << socketAddress.ToString());

            // Problem with server startup
            ReThrowMsg(typename Exception::OpenFailed, socketAddress.ToString());
        }

        // Register new internal connection
        m_internalInterfacesSet.insert(socket);

        // Debug info
        LogPedantic("Server started on interface: " << socket->GetLocalAddress().ToString());

        // Return unique identifier
        return static_cast<AbstractRPCConnectionID>(socket);
    }

    void Close(AbstractRPCConnectionID connectionID)
    {
        LogPedantic("Closing server interface...");

        // Get socket from ID
        SocketType *socket = static_cast<SocketType *>(connectionID);

        // Find corresponding internal connection
        typename InternalInterfaceSet::iterator iterator = m_internalInterfacesSet.find(socket);

        if (iterator == m_internalInterfacesSet.end())
            return;

        // Close socket
        socket->Close();

        // Remove socket listeners
        socket->EventSupport<DPL::Socket::AbstractSocketEvents::AcceptEvent>::RemoveListener(this);
        delete socket;

        m_internalInterfacesSet.erase(iterator);

        // Done
        LogPedantic("Closed");
    }

    void CloseAll()
    {
        while (!m_internalInterfacesSet.empty())
            Close(static_cast<AbstractRPCConnectionID>(*m_internalInterfacesSet.begin()));
    }
};

}
} // namespace DPL

#endif // DPL_GENERIC_SOCKET_RPC_SERVER_H
