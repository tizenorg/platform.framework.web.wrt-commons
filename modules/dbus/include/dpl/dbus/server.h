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
/**
 * @file    server.h
 * @author  Zbigniew Kostrzewa (z.kostrzewa@samsung.com)
 * @version 1.0
 * @brief
 */

#ifndef DPL_DBUS_SERVER_H
#define DPL_DBUS_SERVER_H

#include <memory>
#include <string>
#include <gio/gio.h>
#include <dpl/generic_event.h>
#include <dpl/event/event_support.h>
#include <dpl/dbus/connection.h>

namespace DPL
{
namespace DBus
{

namespace ServerEvents
{
/**
 * Emitted when new connection is accepted.
 *
 * Arg0 Accepted connection.
 *
 * @remarks If this event is processed on separate thread than that thread
 *          should run GLib main loop if one wants to e.g. register DBus
 *          objects during this event processing.
 */
DECLARE_GENERIC_EVENT_1(NewConnectionEvent, ConnectionPtr)
}

class Server;
typedef std::shared_ptr<Server> ServerPtr;

/**
 * Class acting as a server for peer to peer connections over DBus.
 */
class Server : public DPL::Event::EventSupport<ServerEvents::NewConnectionEvent>
{
public:
    /**
     * Creates server.
     *
     * @param address Address the server should listen on.
     * @return Server.
     */
    static ServerPtr create(const std::string& address);

    ~Server();

    /**
     * Starts the server.
     */
    void start();

    /**
     * Stops the server.
     */
    void stop();

protected:
    explicit Server(GDBusServer* server);

private:
    static gboolean onNewConnection(GDBusServer* server,
                                    GDBusConnection* connection,
                                    gpointer data);

    GDBusServer* m_server;
};

}
}

#endif // DPL_DBUS_SERVER_H
