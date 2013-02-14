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
 * @file        tcpsock.cpp
 * @author      Przemyslaw Dobrowolski (p.dobrowolsk@samsung.com)
 * @version     1.0
 * @brief       This file is the implementation file of tcpsock example
 */
#include <stddef.h>
#include <dpl/tcp_socket.h>
#include <dpl/abstract_socket.h>
#include <dpl/application.h>
#include <dpl/generic_event.h>
#include <dpl/binary_queue.h>
#include <dpl/scoped_array.h>
#include <dpl/log/log.h>
#include <string>
#include <dpl/assert.h>

class MyApplication
    : public DPL::Application,
      private DPL::EventListener<DPL::AbstractSocketEvents::ConnectedEvent>,
      private DPL::EventListener<DPL::AbstractSocketEvents::ReadEvent>
{
private:
    DPL::TcpSocket m_socket;

    virtual void OnEventReceived(const DPL::AbstractSocketEvents::ConnectedEvent &event)
    {
        (void)event;
        LogInfo("Connected!");

        // Send request
        DPL::BinaryQueue data;
        const char *query = "GET /wiki/Main_Page HTTP/1.1\nHost: en.wikipedia.org\n\n";
        data.AppendCopy(query, strlen(query) + 1);
        m_socket.Write(data, data.Size());
    }

    virtual void OnEventReceived(const DPL::AbstractSocketEvents::ReadEvent &event)
    {
        (void)event;
        LogInfo("Read!");

        DPL::BinaryQueueAutoPtr data = m_socket.Read(100); // Bad: DLOG cannot log more than about 450 bytes...

        Assert(data.get() != NULL);

        if (data->Empty())
        {
            LogInfo("Connection closed!");
            m_socket.Close();

            // Done
            Quit();
            return;
        }

        // Show data
        DPL::ScopedArray<char> text(new char[data->Size()]);
        data->Flatten(text.Get(), data->Size());

        LogPedantic("READ: \n--------------------------------------------------------\n"
                            << std::string(text.Get(), text.Get() + data->Size()) <<
                          "\n--------------------------------------------------------");
    }

public:
    MyApplication(int argc, char **argv)
        : Application(argc, argv, "tcpsock")
    {
        LogInfo("CTOR!");

        // Add listeners
        m_socket.DPL::EventSupport<DPL::AbstractSocketEvents::ConnectedEvent>::AddListener(this);
        m_socket.DPL::EventSupport<DPL::AbstractSocketEvents::ReadEvent>::AddListener(this);

        // Connect
        m_socket.Open();
        LogInfo("Connecting...");
        m_socket.Connect(DPL::Address("en.wikipedia.org", 80));
    }

    virtual ~MyApplication()
    {
        LogInfo("DTOR!");

        // Remove listeners
        m_socket.DPL::EventSupport<DPL::AbstractSocketEvents::ConnectedEvent>::RemoveListener(this);
        m_socket.DPL::EventSupport<DPL::AbstractSocketEvents::ReadEvent>::RemoveListener(this);
    }
};

int main(int argc, char *argv[])
{
    MyApplication app(argc, argv);
    return app.Exec();
}
