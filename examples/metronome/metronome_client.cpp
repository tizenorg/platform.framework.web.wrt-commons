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
 * @file        metronome_client.cpp
 * @author      Przemyslaw Dobrowolski (p.dobrowolsk@samsung.com)
 * @version     1.0
 * @brief       This file is the implementation file of metronome client example
 */
#include <dpl/tcp_socket_rpc_client.h>
#include <dpl/tcp_socket_rpc_connection.h>
#include <dpl/application.h>
#include <dpl/log/log.h>

class MetronomeClientApplication
    : public DPL::Application,
      private DPL::EventListener<DPL::AbstractRPCConnectionEvents::AsyncCallEvent>,
      private DPL::EventListener<DPL::AbstractRPCConnectionEvents::ConnectionClosedEvent>,
      private DPL::EventListener<DPL::AbstractRPCConnectionEvents::ConnectionBrokenEvent>,
      private DPL::EventListener<DPL::AbstractRPCConnectorEvents::ConnectionEstablishedEvent>
{
private:
    DPL::TcpSocketRPCClient m_rpcClient;
    DPL::ScopedPtr<DPL::AbstractRPCConnection> m_rpcConnection;

    virtual void OnEventReceived(const DPL::AbstractRPCConnectionEvents::AsyncCallEvent &event)
    {
        (void)event;

        // Heart beat
        LogInfo("* Got metronome signal *");
    }

    virtual void OnEventReceived(const DPL::AbstractRPCConnectionEvents::ConnectionClosedEvent &event)
    {
        (void)event;

        LogInfo("Connection closed");

        // Must quit
        Quit();
    }

    virtual void OnEventReceived(const DPL::AbstractRPCConnectionEvents::ConnectionBrokenEvent &event)
    {
        (void)event;

        LogInfo("Connection broken");

        // Must quit
        Quit();
    }

    virtual void OnEventReceived(const DPL::AbstractRPCConnectorEvents::ConnectionEstablishedEvent &event)
    {
        // Save connection pointer
        LogInfo("Connected to metronome server");
        m_rpcConnection.Reset(event.GetArg1());

        // Attach event listeners
        m_rpcConnection->DPL::EventSupport<DPL::AbstractRPCConnectionEvents::AsyncCallEvent>::AddListener(this);
        m_rpcConnection->DPL::EventSupport<DPL::AbstractRPCConnectionEvents::ConnectionClosedEvent>::AddListener(this);
        m_rpcConnection->DPL::EventSupport<DPL::AbstractRPCConnectionEvents::ConnectionBrokenEvent>::AddListener(this);
    }

public:
    MetronomeClientApplication(int argc, char **argv)
        : Application(argc, argv, "rpc")
    {
        // Attach RPC server listeners
        m_rpcClient.DPL::EventSupport<DPL::AbstractRPCConnectorEvents::ConnectionEstablishedEvent>::AddListener(this);

        // Open RPC server
        m_rpcClient.Open("127.0.0.1", 12345);

        // Started
        LogInfo("Metronome client started");
     }

    virtual ~MetronomeClientApplication()
    {
        // Delete all RPC connections
        if (m_rpcConnection.Get())
        {
            m_rpcConnection->DPL::EventSupport<DPL::AbstractRPCConnectionEvents::AsyncCallEvent>::RemoveListener(this);
            m_rpcConnection->DPL::EventSupport<DPL::AbstractRPCConnectionEvents::ConnectionClosedEvent>::RemoveListener(this);
            m_rpcConnection->DPL::EventSupport<DPL::AbstractRPCConnectionEvents::ConnectionBrokenEvent>::RemoveListener(this);
            m_rpcConnection.Reset();
        }

        // Close RPC server
        m_rpcClient.CloseAll();

        // Detach RPC server listener
        m_rpcClient.DPL::EventSupport<DPL::AbstractRPCConnectorEvents::ConnectionEstablishedEvent>::RemoveListener(this);
    }
};

int main(int argc, char *argv[])
{
    return MetronomeClientApplication(argc, argv).Exec();
}
