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
 * @file        rpc.cpp
 * @author      Przemyslaw Dobrowolski (p.dobrowolsk@samsung.com)
 * @version     1.0
 * @brief       This file is the implementation file of RPC example
 */
#include <stddef.h>
#include <dpl/unix_socket_rpc_client.h>
#include <dpl/unix_socket_rpc_server.h>
#include <dpl/unix_socket_rpc_connection.h>
#include <memory>
#include <dpl/application.h>
#include <dpl/controller.h>
#include <dpl/thread.h>
#include <dpl/log/log.h>
#include <string>

static const char *RPC_NAME = "/tmp/unix_socket_rpc";

class MyThread
    : public DPL::Thread,
      private DPL::EventListener<DPL::AbstractRPCConnectionEvents::AsyncCallEvent>,
      private DPL::EventListener<DPL::AbstractRPCConnectionEvents::ConnectionClosedEvent>,
      private DPL::EventListener<DPL::AbstractRPCConnectionEvents::ConnectionBrokenEvent>,
      private DPL::EventListener<DPL::AbstractRPCConnectorEvents::ConnectionEstablishedEvent>
{
private:
    DPL::UnixSocketRPCClient m_rpcClient;
    std::unique_ptr<DPL::AbstractRPCConnection> m_rpcConnection;

    virtual void OnEventReceived(const DPL::AbstractRPCConnectionEvents::AsyncCallEvent &event)
    {
        (void)event;

        LogDebug("CLIENT: AsyncCallEvent received");

        int value;
        event.GetArg0().ConsumeArg(value);
        LogDebug("CLIENT: Result from server: " << value);
    }

    virtual void OnEventReceived(const DPL::AbstractRPCConnectionEvents::ConnectionClosedEvent &event)
    {
        (void)event;
        LogDebug("CLIENT: ConnectionClosedEvent received");
    }

    virtual void OnEventReceived(const DPL::AbstractRPCConnectionEvents::ConnectionBrokenEvent &event)
    {
        (void)event;
        LogDebug("CLIENT: ConnectionBrokenEvent received");
    }

    virtual void OnEventReceived(const DPL::AbstractRPCConnectorEvents::ConnectionEstablishedEvent &event)
    {
        // Save connection pointer
        LogDebug("CLIENT: Acquiring new connection");
        m_rpcConnection.reset(event.GetArg1());

        // Attach listener to new connection
        LogDebug("CLIENT: Attaching connection event listeners");
        m_rpcConnection->DPL::EventSupport<DPL::AbstractRPCConnectionEvents::AsyncCallEvent>::AddListener(this);
        m_rpcConnection->DPL::EventSupport<DPL::AbstractRPCConnectionEvents::ConnectionClosedEvent>::AddListener(this);
        m_rpcConnection->DPL::EventSupport<DPL::AbstractRPCConnectionEvents::ConnectionBrokenEvent>::AddListener(this);

        LogDebug("CLIENT: Connection established");

        // Emit RPC function call
        DPL::RPCFunction proc;
        proc.AppendArg((int)1111);
        LogDebug("CLIENT: Calling RPC function");
        m_rpcConnection->AsyncCall(proc);
    }

public:
    virtual ~MyThread()
    {
        // Always quit thread
       Quit();
    }

    virtual int ThreadEntry()
    {
        // Attach RPC listeners
        LogDebug("CLIENT: Attaching connection established event");
        m_rpcClient.DPL::EventSupport<DPL::AbstractRPCConnectorEvents::ConnectionEstablishedEvent>::AddListener(this);

        // Open connection to server
        LogDebug("CLIENT: Opening connection to RPC");
        m_rpcClient.Open(RPC_NAME);

        // Start message loop
        LogDebug("CLIENT: Starting thread event loop");
        int ret = Exec();

        // Detach RPC listeners
        if (m_rpcConnection.get())
        {
            LogDebug("CLIENT: Detaching RPC connection events");
            m_rpcConnection->DPL::EventSupport<DPL::AbstractRPCConnectionEvents::AsyncCallEvent>::RemoveListener(this);
            m_rpcConnection->DPL::EventSupport<DPL::AbstractRPCConnectionEvents::ConnectionClosedEvent>::RemoveListener(this);
            m_rpcConnection->DPL::EventSupport<DPL::AbstractRPCConnectionEvents::ConnectionBrokenEvent>::RemoveListener(this);

            LogDebug("CLIENT: Resetting connection");
            m_rpcConnection.reset();
        }

        // Detach RPC client listener
        LogDebug("CLIENT: Detaching connection established event");
        m_rpcClient.DPL::EventSupport<DPL::AbstractRPCConnectorEvents::ConnectionEstablishedEvent>::RemoveListener(this);

        // Close RPC
        LogDebug("CLIENT: Closing RPC client");
        m_rpcClient.CloseAll();

        // Done
        return ret;
    }
};

DECLARE_GENERIC_EVENT_0(QuitEvent)
DECLARE_GENERIC_EVENT_0(CloseThreadEvent)

class MyApplication
    : public DPL::Application,
      private DPL::Controller<DPL::TypeListDecl<QuitEvent,
                                                CloseThreadEvent>::Type>,
      private DPL::EventListener<DPL::AbstractRPCConnectionEvents::AsyncCallEvent>,
      private DPL::EventListener<DPL::AbstractRPCConnectionEvents::ConnectionClosedEvent>,
      private DPL::EventListener<DPL::AbstractRPCConnectionEvents::ConnectionBrokenEvent>,
      private DPL::EventListener<DPL::AbstractRPCConnectorEvents::ConnectionEstablishedEvent>
{
private:
    DPL::UnixSocketRPCServer m_rpcServer;
    std::unique_ptr<DPL::AbstractRPCConnection> m_rpcConnection;

    MyThread m_thread;

    // Quit application event occurred
    virtual void OnEventReceived(const QuitEvent &event)
    {
        (void)event;
        Quit();
    }

    virtual void OnEventReceived(const CloseThreadEvent &event)
    {
        (void)event;
        m_thread.Quit();
    }

    virtual void OnEventReceived(const DPL::AbstractRPCConnectionEvents::AsyncCallEvent &event)
    {
        (void)event;

        LogDebug("SERVER: AsyncCallEvent received");

        int value;
        event.GetArg0().ConsumeArg(value);
        LogDebug("SERVER: Result from client: " << value);
    }

    virtual void OnEventReceived(const DPL::AbstractRPCConnectionEvents::ConnectionClosedEvent &event)
    {
        (void)event;

        LogDebug("SERVER: ConnectionClosedEvent received");

        // Close RPC now
        LogDebug("SERVER: Closing RPC connection on event...");

        // Detach RPC connection listeners
        if (m_rpcConnection.get())
        {
            LogDebug("SERVER: Detaching connection events");
            m_rpcConnection->DPL::EventSupport<DPL::AbstractRPCConnectionEvents::AsyncCallEvent>::RemoveListener(this);
            m_rpcConnection->DPL::EventSupport<DPL::AbstractRPCConnectionEvents::ConnectionClosedEvent>::RemoveListener(this);
            m_rpcConnection->DPL::EventSupport<DPL::AbstractRPCConnectionEvents::ConnectionBrokenEvent>::RemoveListener(this);
        }
        LogDebug("SERVER: RPC connection closed");

        LogDebug("SERVER: Closing RPC on event...");
        m_rpcServer.CloseAll();
        LogDebug("SERVER: RPC closed");
    }

    virtual void OnEventReceived(const DPL::AbstractRPCConnectionEvents::ConnectionBrokenEvent &event)
    {
        (void)event;
        LogDebug("SERVER: ConnectionBrokenEvent received");
    }

    virtual void OnEventReceived(const DPL::AbstractRPCConnectorEvents::ConnectionEstablishedEvent &event)
    {
        // Save connection pointer
        LogDebug("SERVER: Acquiring RPC connection");
        m_rpcConnection.reset(event.GetArg1());

        // Attach event listeners
        LogDebug("SERVER: Attaching connection listeners");
        m_rpcConnection->DPL::EventSupport<DPL::AbstractRPCConnectionEvents::AsyncCallEvent>::AddListener(this);
        m_rpcConnection->DPL::EventSupport<DPL::AbstractRPCConnectionEvents::ConnectionClosedEvent>::AddListener(this);
        m_rpcConnection->DPL::EventSupport<DPL::AbstractRPCConnectionEvents::ConnectionBrokenEvent>::AddListener(this);

        LogDebug("SERVER: Connection established");

        // Emit RPC function call
        DPL::RPCFunction proc;
        proc.AppendArg((int)2222);
        LogDebug("SERVER: Calling RPC function");
        m_rpcConnection->AsyncCall(proc);
    }

public:
    MyApplication(int argc, char **argv)
        : Application(argc, argv, "rpc")
    {
        // Attach RPC server listeners
        LogDebug("SERVER: Attaching connection established event");
        m_rpcServer.DPL::EventSupport<DPL::AbstractRPCConnectorEvents::ConnectionEstablishedEvent>::AddListener(this);

        // Self touch
        LogDebug("SERVER: Touching controller");
        Touch();

        // Open RPC server
        LogDebug("SERVER: Opening server RPC");
        m_rpcServer.Open(RPC_NAME);

        // Run RPC client in thread
        LogDebug("SERVER: Starting RPC client thread");
        m_thread.Run();

        // Quit application automatically in few seconds
        LogDebug("SERVER: Sending control timed events");
        DPL::ControllerEventHandler<CloseThreadEvent>::PostTimedEvent(CloseThreadEvent(), 2);
        DPL::ControllerEventHandler<QuitEvent>::PostTimedEvent(QuitEvent(), 3);
    }

    virtual ~MyApplication()
    {
        // Quit thread
        LogDebug("SERVER: Quitting thread");
        m_thread.Quit();

        // Close RPC server
        LogDebug("SERVER: Closing RPC server");
        m_rpcServer.CloseAll();

        // Detach RPC server listener
        m_rpcServer.DPL::EventSupport<DPL::AbstractRPCConnectorEvents::ConnectionEstablishedEvent>::RemoveListener(this);
    }
};

int main(int argc, char *argv[])
{
    LogDebug("Starting");
    MyApplication app(argc, argv);
    return app.Exec();
}
