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
#include <dpl/scoped_ptr.h>
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
    DPL::ScopedPtr<DPL::AbstractRPCConnection> m_rpcConnection;

    virtual void OnEventReceived(const DPL::AbstractRPCConnectionEvents::AsyncCallEvent &event)
    {
        (void)event;

        LogInfo("CLIENT: AsyncCallEvent received");

        int value;
        event.GetArg0().ConsumeArg(value);
        LogInfo("CLIENT: Result from server: " << value);
    }

    virtual void OnEventReceived(const DPL::AbstractRPCConnectionEvents::ConnectionClosedEvent &event)
    {
        (void)event;
        LogInfo("CLIENT: ConnectionClosedEvent received");
    }

    virtual void OnEventReceived(const DPL::AbstractRPCConnectionEvents::ConnectionBrokenEvent &event)
    {
        (void)event;
        LogInfo("CLIENT: ConnectionBrokenEvent received");
    }

    virtual void OnEventReceived(const DPL::AbstractRPCConnectorEvents::ConnectionEstablishedEvent &event)
    {
        // Save connection pointer
        LogInfo("CLIENT: Acquiring new connection");
        m_rpcConnection.Reset(event.GetArg1());

        // Attach listener to new connection
        LogInfo("CLIENT: Attaching connection event listeners");
        m_rpcConnection->DPL::EventSupport<DPL::AbstractRPCConnectionEvents::AsyncCallEvent>::AddListener(this);
        m_rpcConnection->DPL::EventSupport<DPL::AbstractRPCConnectionEvents::ConnectionClosedEvent>::AddListener(this);
        m_rpcConnection->DPL::EventSupport<DPL::AbstractRPCConnectionEvents::ConnectionBrokenEvent>::AddListener(this);

        LogInfo("CLIENT: Connection established");

        // Emit RPC function call
        DPL::RPCFunction proc;
        proc.AppendArg((int)1111);
        LogInfo("CLIENT: Calling RPC function");
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
        LogInfo("CLIENT: Attaching connection established event");
        m_rpcClient.DPL::EventSupport<DPL::AbstractRPCConnectorEvents::ConnectionEstablishedEvent>::AddListener(this);

        // Open connection to server
        LogInfo("CLIENT: Opening connection to RPC");
        m_rpcClient.Open(RPC_NAME);

        // Start message loop
        LogInfo("CLIENT: Starting thread event loop");
        int ret = Exec();

        // Detach RPC listeners
        if (m_rpcConnection.Get())
        {
            LogInfo("CLIENT: Detaching RPC connection events");
            m_rpcConnection->DPL::EventSupport<DPL::AbstractRPCConnectionEvents::AsyncCallEvent>::RemoveListener(this);
            m_rpcConnection->DPL::EventSupport<DPL::AbstractRPCConnectionEvents::ConnectionClosedEvent>::RemoveListener(this);
            m_rpcConnection->DPL::EventSupport<DPL::AbstractRPCConnectionEvents::ConnectionBrokenEvent>::RemoveListener(this);

            LogInfo("CLIENT: Resetting connection");
            m_rpcConnection.Reset();
        }

        // Detach RPC client listener
        LogInfo("CLIENT: Detaching connection established event");
        m_rpcClient.DPL::EventSupport<DPL::AbstractRPCConnectorEvents::ConnectionEstablishedEvent>::RemoveListener(this);

        // Close RPC
        LogInfo("CLIENT: Closing RPC client");
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
    DPL::ScopedPtr<DPL::AbstractRPCConnection> m_rpcConnection;

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

        LogInfo("SERVER: AsyncCallEvent received");

        int value;
        event.GetArg0().ConsumeArg(value);
        LogInfo("SERVER: Result from client: " << value);
    }

    virtual void OnEventReceived(const DPL::AbstractRPCConnectionEvents::ConnectionClosedEvent &event)
    {
        (void)event;

        LogInfo("SERVER: ConnectionClosedEvent received");

        // Close RPC now
        LogInfo("SERVER: Closing RPC connection on event...");

        // Detach RPC connection listeners
        if (m_rpcConnection.Get())
        {
            LogInfo("SERVER: Detaching connection events");
            m_rpcConnection->DPL::EventSupport<DPL::AbstractRPCConnectionEvents::AsyncCallEvent>::RemoveListener(this);
            m_rpcConnection->DPL::EventSupport<DPL::AbstractRPCConnectionEvents::ConnectionClosedEvent>::RemoveListener(this);
            m_rpcConnection->DPL::EventSupport<DPL::AbstractRPCConnectionEvents::ConnectionBrokenEvent>::RemoveListener(this);
        }
        LogInfo("SERVER: RPC connection closed");

        LogInfo("SERVER: Closing RPC on event...");
        m_rpcServer.CloseAll();
        LogInfo("SERVER: RPC closed");
    }

    virtual void OnEventReceived(const DPL::AbstractRPCConnectionEvents::ConnectionBrokenEvent &event)
    {
        (void)event;
        LogInfo("SERVER: ConnectionBrokenEvent received");
    }

    virtual void OnEventReceived(const DPL::AbstractRPCConnectorEvents::ConnectionEstablishedEvent &event)
    {
        // Save connection pointer
        LogInfo("SERVER: Acquiring RPC connection");
        m_rpcConnection.Reset(event.GetArg1());

        // Attach event listeners
        LogInfo("SERVER: Attaching connection listeners");
        m_rpcConnection->DPL::EventSupport<DPL::AbstractRPCConnectionEvents::AsyncCallEvent>::AddListener(this);
        m_rpcConnection->DPL::EventSupport<DPL::AbstractRPCConnectionEvents::ConnectionClosedEvent>::AddListener(this);
        m_rpcConnection->DPL::EventSupport<DPL::AbstractRPCConnectionEvents::ConnectionBrokenEvent>::AddListener(this);

        LogInfo("SERVER: Connection established");

        // Emit RPC function call
        DPL::RPCFunction proc;
        proc.AppendArg((int)2222);
        LogInfo("SERVER: Calling RPC function");
        m_rpcConnection->AsyncCall(proc);
    }

public:
    MyApplication(int argc, char **argv)
        : Application(argc, argv, "rpc")
    {
        // Attach RPC server listeners
        LogInfo("SERVER: Attaching connection established event");
        m_rpcServer.DPL::EventSupport<DPL::AbstractRPCConnectorEvents::ConnectionEstablishedEvent>::AddListener(this);

        // Self touch
        LogInfo("SERVER: Touching controller");
        Touch();

        // Open RPC server
        LogInfo("SERVER: Opening server RPC");
        m_rpcServer.Open(RPC_NAME);

        // Run RPC client in thread
        LogInfo("SERVER: Starting RPC client thread");
        m_thread.Run();

        // Quit application automatically in few seconds
        LogInfo("SERVER: Sending control timed events");
        DPL::ControllerEventHandler<CloseThreadEvent>::PostTimedEvent(CloseThreadEvent(), 2);
        DPL::ControllerEventHandler<QuitEvent>::PostTimedEvent(QuitEvent(), 3);
    }

    virtual ~MyApplication()
    {
        // Quit thread
        LogInfo("SERVER: Quitting thread");
        m_thread.Quit();

        // Close RPC server
        LogInfo("SERVER: Closing RPC server");
        m_rpcServer.CloseAll();

        // Detach RPC server listener
        m_rpcServer.DPL::EventSupport<DPL::AbstractRPCConnectorEvents::ConnectionEstablishedEvent>::RemoveListener(this);
    }
};

int main(int argc, char *argv[])
{
    LogInfo("Starting");
    MyApplication app(argc, argv);
    return app.Exec();
}
