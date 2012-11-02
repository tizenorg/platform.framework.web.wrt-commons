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
 * @author      Pawel Sikorski (p.sikorski@samsung.com)
 * @version     1.0
 * @brief       This file is the implementation file of RPC example
 */
#include <stddef.h>
#include <dpl/unix_socket_rpc_client.h>
#include <dpl/unix_socket_rpc_server.h>
#include <dpl/unix_socket_rpc_connection.h>
#include <dpl/fake_rpc_connection.h>
#include <dpl/scoped_ptr.h>
#include <dpl/application.h>
#include <dpl/controller.h>
#include <dpl/thread.h>
#include <dpl/log/log.h>
#include <string>

// FLOW:
// 1st connection - UnixSockets
// 2nd connection - Fake
// client sends data via fake IPC
// server sends back this data via unix IPC.
// client compare sent and received data

static const char *UNIX_RPC_NAME = "/tmp/unix_socket_rpc";
static const char *FAKE_RPC_NAME = "/tmp/fake_rpc";

typedef DPL::AbstractRPCConnectionEvents::AsyncCallEvent AsyncCallEvent;
typedef DPL::AbstractRPCConnectionEvents::ConnectionClosedEvent
    ConnectionClosedEvent;
typedef DPL::AbstractRPCConnectionEvents::ConnectionBrokenEvent
    ConnectionBrokenEvent;
typedef DPL::AbstractRPCConnectorEvents::ConnectionEstablishedEvent
    ConnectionEstablishedEvent;

class MyThread
    : public DPL::Thread,
      private DPL::EventListener<AsyncCallEvent>,
      private DPL::EventListener<ConnectionEstablishedEvent>
{
private:
    DPL::UnixSocketRPCClient m_rpcUnixClient;
    DPL::FakeRpcClient m_rpcFakeClient;
    int m_connections;
    int m_sentData;
    int m_receivedData;

    DPL::ScopedPtr<DPL::AbstractRPCConnection> m_rpcUnixConnection;
    DPL::ScopedPtr<DPL::AbstractRPCConnection> m_rpcFakeConnection;

    virtual void OnEventReceived(const AsyncCallEvent &event)
    {
        LogInfo("CLIENT: AsyncCallEvent received");

        event.GetArg0().ConsumeArg(m_receivedData);
        LogInfo("CLIENT: Result from server: " << m_receivedData);

        if (m_receivedData != m_sentData)
            LogError("Wrong data Received!");
    }

    virtual void OnEventReceived(const ConnectionEstablishedEvent &event)
    {
        if (dynamic_cast<DPL::FakeRpcConnection *>(event.GetArg1())){
            ++m_connections;
            LogInfo("CLIENT: Acquiring new fake connection");
            m_rpcFakeConnection.Reset(event.GetArg1());
            //this is not used on this side
//            m_rpcFakeConnection->DPL::EventSupport<AsyncCallEvent>::AddListener(this);
        }
        else{
            ++m_connections;
            LogInfo("CLIENT: Acquiring new unix connection");
            m_rpcUnixConnection.Reset(event.GetArg1());
            m_rpcUnixConnection->DPL::EventSupport<AsyncCallEvent>::AddListener(this);
        }
        if(m_connections == 2){
            m_sentData = 1213;
            // Emit RPC function call
            DPL::RPCFunction proc;
            proc.AppendArg(m_sentData);
            LogInfo("CLIENT: Calling RPC function");
            m_rpcFakeConnection->AsyncCall(proc);
        }
    }

public:
    virtual ~MyThread()
    {
        // Always quit thread
       Quit();
    }

    virtual int ThreadEntry()
    {
        m_connections = 0;
        // Attach RPC listeners
        LogInfo("CLIENT: Attaching connection established event");
        m_rpcUnixClient.DPL::EventSupport<ConnectionEstablishedEvent>::AddListener(this);
        m_rpcFakeClient.DPL::EventSupport<ConnectionEstablishedEvent>::AddListener(this);

        // Open connection to server
        LogInfo("CLIENT: Opening connection to RPC");
        m_rpcUnixClient.Open(UNIX_RPC_NAME);
        m_rpcFakeClient.Open(FAKE_RPC_NAME);

        // Start message loop
        LogInfo("CLIENT: Starting thread event loop");
        int ret = Exec();

        if (m_rpcUnixConnection.Get()){
            LogInfo("CLIENT: Removing Unix connection");
            m_rpcUnixConnection->DPL::EventSupport<AsyncCallEvent>::RemoveListener(this);
            m_rpcUnixConnection.Reset();
        }

        LogInfo("CLIENT: Closing");

        if (m_rpcFakeConnection.Get()){
            LogInfo("CLIENT: Removing Fake connection");
            //this is not used on this side
//            m_rpcFakeConnection->DPL::EventSupport<AsyncCallEvent>::RemoveListener(this);
            m_rpcFakeConnection.Reset();
        }

        // Detach RPC client listener
        LogInfo("CLIENT: Detaching connection established event");
        m_rpcUnixClient.DPL::EventSupport<ConnectionEstablishedEvent>::RemoveListener(this);
        m_rpcFakeClient.DPL::EventSupport<ConnectionEstablishedEvent>::RemoveListener(this);

        // Close RPC
        LogInfo("CLIENT: Closing RPC client");
        m_rpcUnixClient.CloseAll();
        m_rpcFakeClient.Close();//not needed

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
      private DPL::EventListener<AsyncCallEvent>,
      private DPL::EventListener<ConnectionEstablishedEvent>
{
private:
    DPL::UnixSocketRPCServer m_rpcUnixServer;
    DPL::FakeRpcServer m_rpcFakeServer;

    DPL::ScopedPtr<DPL::AbstractRPCConnection> m_rpcUnixConnection;
    DPL::ScopedPtr<DPL::AbstractRPCConnection> m_rpcFakeConnection;

    MyThread m_thread;

    // Quit application event occurred
    virtual void OnEventReceived(const QuitEvent &/*event*/){
        // Close RPC now
        LogInfo("SERVER: Closing RPC connection...");

        // Detach RPC connection listeners
        if (m_rpcUnixConnection.Get()) {
            //this is not used on this side
//            m_rpcUnixConnection->DPL::EventSupport<AsyncCallEvent>::RemoveListener(this);
            m_rpcUnixConnection.Reset();
        }

        if (m_rpcFakeConnection.Get()) {
            m_rpcFakeConnection->DPL::EventSupport<AsyncCallEvent>::RemoveListener(this);
            m_rpcFakeConnection.Reset();
        }

        LogInfo("SERVER: Closing Server");
        m_rpcUnixServer.CloseAll();
        m_rpcFakeServer.CloseAll();//not needed
        m_rpcUnixServer.DPL::EventSupport<ConnectionEstablishedEvent>::RemoveListener(this);
        m_rpcFakeServer.DPL::EventSupport<ConnectionEstablishedEvent>::RemoveListener(this);

        LogInfo("SERVER: Server closed");

        Quit();
    }

    virtual void OnEventReceived(const CloseThreadEvent &/*event*/){
        m_thread.Quit();
    }

    virtual void OnEventReceived(const AsyncCallEvent &event)
    {
        LogInfo("SERVER: AsyncCallEvent received");

        int value;
        event.GetArg0().ConsumeArg(value);
        LogInfo("SERVER: Result from client: " << value);

        // send back data to client (via fake)
        // Emit RPC function call
        DPL::RPCFunction proc;
        proc.AppendArg(value);
        LogInfo("SERVER: Calling RPC function");
        m_rpcUnixConnection->AsyncCall(proc);
    }

    virtual void OnEventReceived(const ConnectionEstablishedEvent &event)
    {
        // Save connection pointer
        if (dynamic_cast<DPL::FakeRpcConnection *>(event.GetArg1())){
            LogInfo("SERVER: Acquiring Fake RPC connection");
            m_rpcFakeConnection.Reset(event.GetArg1());
            m_rpcFakeConnection->DPL::EventSupport<AsyncCallEvent>::AddListener(this);
        }
        else{
            LogInfo("SERVER: Acquiring Unix RPC connection");
            m_rpcUnixConnection.Reset(event.GetArg1());
            //this is not used on this side
//            m_rpcUnixConnection->DPL::EventSupport<AsyncCallEvent>::AddListener(this);
        }
    }

public:
    MyApplication(int argc, char **argv)
        : Application(argc, argv, "rpc")
    {
        // Attach RPC server listeners
        LogInfo("SERVER: Attaching connection established event");
        m_rpcUnixServer.DPL::EventSupport<ConnectionEstablishedEvent>::AddListener(this);
        m_rpcFakeServer.DPL::EventSupport<ConnectionEstablishedEvent>::AddListener(this);

        // Self touch
        LogInfo("SERVER: Touching controller");
        Touch();

        // Open RPC server
        LogInfo("SERVER: Opening server RPC");
        m_rpcUnixServer.Open(UNIX_RPC_NAME);
        m_rpcFakeServer.Open(FAKE_RPC_NAME);

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
    }
};

int main(int argc, char *argv[])
{
    LogInfo("Starting");
    MyApplication app(argc, argv);
    return app.Exec();
}
