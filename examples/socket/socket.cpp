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
 * @file        socket.cpp
 * @author      Przemyslaw Dobrowolski (p.dobrowolsk@samsung.com)
 * @version     1.0
 * @brief       This file is the implementation file of socket example
 */
#include <stddef.h>
#include <dpl/unix_socket.h>
#include <dpl/abstract_socket.h>
#include <dpl/application.h>
#include <dpl/controller.h>
#include <dpl/generic_event.h>
#include <dpl/thread.h>
#include <dpl/log/log.h>
#include <string>
#include <dpl/assert.h>

namespace // anonymous
{
static const char *SOCKET_NAME = "/tmp/unix_sock";
} // namespace anonymous

class MyThread
    : public DPL::Thread,
      private DPL::EventListener<DPL::AbstractSocketEvents::AcceptEvent>
{
private:
    DPL::UnixSocket m_socket;

    // Socket accept event
    virtual void OnEventReceived(const DPL::AbstractSocketEvents::AcceptEvent &event)
    {
        (void)event;
        LogDebug("Accept event occurred");

        DPL::UnixSocket *client = static_cast<DPL::UnixSocket *>(m_socket.Accept());

        LogDebug("Accepted client remote address: " << client->GetRemoteAddress().ToString());
        LogDebug("Accepted client local address: " << client->GetLocalAddress().ToString());
     
        delete client;
    }

public:
    virtual ~MyThread()
    {
        // Quit thread
        Quit();
    }

    virtual int ThreadEntry()
    {
        // Add listeners
        m_socket.DPL::EventSupport<DPL::AbstractSocketEvents::AcceptEvent>::AddListener(this);

        // Create server
        LogDebug("Starting server...");

        m_socket.Bind(DPL::Address(SOCKET_NAME));
        m_socket.Listen(5);

        LogDebug("Server started");

        LogDebug("Server local address: " << m_socket.GetLocalAddress().ToString());

        int result = Exec();

        // Remove listeners
        m_socket.DPL::EventSupport<DPL::AbstractSocketEvents::AcceptEvent>::RemoveListener(this);

        // Must close socket in same context
        m_socket.Close();

        return result;
    }
};

DECLARE_GENERIC_EVENT_0(QuitEvent)

class MyApplication
    : public DPL::Application,
      public DPL::Controller<DPL::TypeListDecl<QuitEvent>::Type>,
      private DPL::EventListener<DPL::AbstractSocketEvents::ConnectedEvent>
{
private:
    MyThread thread;
    DPL::UnixSocket sock;

    // Quit application event occurred
    virtual void OnEventReceived(const QuitEvent &event)
    {
        (void)event;
        Quit();
    }

    // Socket connected event
    virtual void OnEventReceived(const DPL::AbstractSocketEvents::ConnectedEvent &event)
    {
        (void)event;
        LogDebug("Connected event occurred");
    }

public:
    MyApplication(int argc, char **argv)
        : Application(argc, argv, "example_socket_application")
    {
        // Add listeners
        sock.DPL::EventSupport<DPL::AbstractSocketEvents::ConnectedEvent>::AddListener(this);

        // Touch self controller
        Touch();

        // Start threaded server
        LogDebug("Running threaded server");

        // Run server in thread
        thread.Run();

        LogDebug("Waiting for server to start...");
        sleep(1);

        // Connect to server
        sock.Connect(DPL::Address(SOCKET_NAME));

        // Quit application automatically in few seconds
        DPL::ControllerEventHandler<QuitEvent>::PostTimedEvent(QuitEvent(), 2);
    }

    virtual ~MyApplication()
    {
        // Remove listeners
        sock.DPL::EventSupport<DPL::AbstractSocketEvents::ConnectedEvent>::RemoveListener(this);
    }
};

int main(int argc, char *argv[])
{
    MyApplication app(argc, argv);
    return app.Exec();
}
