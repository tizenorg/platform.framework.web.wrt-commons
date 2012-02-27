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
 * @file        metronome_server.cpp
 * @author      Przemyslaw Dobrowolski (p.dobrowolsk@samsung.com)
 * @version     1.0
 * @brief       This file is the implementation file of metronome server example
 */
#include <dpl/tcp_socket_rpc_server.h>
#include <dpl/tcp_socket_rpc_connection.h>
#include <dpl/controller.h>
#include <dpl/application.h>
#include <dpl/type_list.h>
#include <dpl/log/log.h>
#include <algorithm>
#include <list>
#include <dpl/assert.h>

// Metronome signal event
DECLARE_GENERIC_EVENT_0(SignalEvent)
DECLARE_GENERIC_EVENT_1(DeleteConnectionEvent, DPL::AbstractRPCConnection *)

// Heart beat interval
const double HEART_BEAT_INTERVAL = 1.0; // seconds

class MetronomeServerApplication
    : public DPL::Application,
      private DPL::Controller<DPL::TypeListDecl<SignalEvent,
                                                DeleteConnectionEvent>::Type>,
      private DPL::EventListener<DPL::AbstractRPCConnectionEvents::ConnectionClosedEvent>,
      private DPL::EventListener<DPL::AbstractRPCConnectionEvents::ConnectionBrokenEvent>,
      private DPL::EventListener<DPL::AbstractRPCConnectorEvents::ConnectionEstablishedEvent>
{
private:
    DPL::TcpSocketRPCServer m_rpcServer;

    typedef std::list<DPL::AbstractRPCConnection *> ConnectionList;
    ConnectionList m_connections;

    // Matronome signal received
    virtual void OnEventReceived(const SignalEvent &event)
    {
        (void)event;

        // Signal all conection about heart beat
        DPL::RPCFunction proc;
        proc.AppendArg((int)0);

        for (ConnectionList::iterator it = m_connections.begin(); it != m_connections.end(); ++it)
            (*it)->AsyncCall(proc);

        // Continue to emot heart beats
        DPL::ControllerEventHandler<SignalEvent>::PostTimedEvent(SignalEvent(), HEART_BEAT_INTERVAL);
    }

    virtual void OnEventReceived(const DeleteConnectionEvent &event)
    {
        delete event.GetArg0();
    }

    void RemoveConnection(DPL::AbstractRPCConnection *connection)
    {
        // Find connection
        ConnectionList::iterator it = std::find(m_connections.begin(), m_connections.end(), connection);
        Assert(it != m_connections.end());

        // Erase connection
        m_connections.erase(it);

        // Detach RPC connection listeners
        connection->DPL::EventSupport<DPL::AbstractRPCConnectionEvents::ConnectionClosedEvent>::RemoveListener(this);
        connection->DPL::EventSupport<DPL::AbstractRPCConnectionEvents::ConnectionBrokenEvent>::RemoveListener(this);

        // Delete connection
        DPL::ControllerEventHandler<DeleteConnectionEvent>::PostEvent(DeleteConnectionEvent(connection));
    }

    void AddConnection(DPL::AbstractRPCConnection *connection)
    {
        // Add connection
        m_connections.push_back(connection);

        // Attach event listeners
        connection->DPL::EventSupport<DPL::AbstractRPCConnectionEvents::ConnectionClosedEvent>::AddListener(this);
        connection->DPL::EventSupport<DPL::AbstractRPCConnectionEvents::ConnectionBrokenEvent>::AddListener(this);
    }

    virtual void OnEventReceived(const DPL::AbstractRPCConnectionEvents::ConnectionClosedEvent &event)
    {
        (void)event;

        LogInfo("Connection closed");

        // Remove connection from list
        RemoveConnection(static_cast<DPL::AbstractRPCConnection *>(event.GetSender()));
    }

    virtual void OnEventReceived(const DPL::AbstractRPCConnectionEvents::ConnectionBrokenEvent &event)
    {
        (void)event;

        LogInfo("Connection broken");

        // Remove connection from list
        RemoveConnection(static_cast<DPL::AbstractRPCConnection *>(event.GetSender()));
    }

    virtual void OnEventReceived(const DPL::AbstractRPCConnectorEvents::ConnectionEstablishedEvent &event)
    {
        // Save connection pointer
        LogInfo("New connection");

        // Add nre connection to list
        AddConnection(event.GetArg1());
    }

public:
    MetronomeServerApplication(int argc, char **argv)
        : Application(argc, argv, "rpc")
    {
        // Attach RPC server listeners
        m_rpcServer.DPL::EventSupport<DPL::AbstractRPCConnectorEvents::ConnectionEstablishedEvent>::AddListener(this);

        // Inherit calling context
        Touch();

        // Open RPC server
        m_rpcServer.Open(12345);

        // Start heart beat
        DPL::ControllerEventHandler<SignalEvent>::PostTimedEvent(SignalEvent(), HEART_BEAT_INTERVAL);

        // Started
        LogInfo("Metronome server started");
     }

    virtual ~MetronomeServerApplication()
    {
        // Delete all RPC connections
        while (!m_connections.empty())
            RemoveConnection(m_connections.front());

        // Close RPC server
        m_rpcServer.CloseAll();

        // Detach RPC server listener
        m_rpcServer.DPL::EventSupport<DPL::AbstractRPCConnectorEvents::ConnectionEstablishedEvent>::RemoveListener(this);
    }
};

int main(int argc, char *argv[])
{
    return MetronomeServerApplication(argc, argv).Exec();
}
