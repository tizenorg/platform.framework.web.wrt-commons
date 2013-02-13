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
 * @file    server.cpp
 * @author  Zbigniew Kostrzewa (z.kostrzewa@samsung.com)
 * @version 1.0
 * @brief
 */
#include <stddef.h>
#include <dpl/assert.h>
#include <dpl/log/log.h>
#include <dpl/dbus/server.h>

namespace DPL {
namespace DBus {
ServerPtr Server::create(const std::string& address)
{
    GError* error = NULL;

    int flags = G_DBUS_SERVER_FLAGS_NONE |
        G_DBUS_SERVER_FLAGS_AUTHENTICATION_ALLOW_ANONYMOUS;

    gchar* serverId = g_dbus_generate_guid();

    GDBusServer* server = g_dbus_server_new_sync(
            address.c_str(),
            static_cast<GDBusServerFlags>(flags),
            serverId,
            NULL,
            NULL,
            &error);
    g_free(serverId);

    if (NULL == server) {
        std::string message;
        if (NULL != error) {
            message = error->message;
            g_error_free(error);
        }

        ThrowMsg(DPL::Exception, "Error on server creation: " << message);
    }

    return ServerPtr(new Server(server));
}

Server::Server(GDBusServer* server) :
    m_server(server)
{}

Server::~Server()
{
    if (g_dbus_server_is_active(m_server)) {
        stop();
    }
    g_object_unref(m_server);
}

void Server::start()
{
    Assert(!g_dbus_server_is_active(m_server) && "Server already started.");

    g_dbus_server_start(m_server);

    g_signal_connect(m_server,
                     "new-connection",
                     G_CALLBACK(onNewConnection),
                     this);

    LogInfo("Server started at: "
            << g_dbus_server_get_client_address(m_server));
}

void Server::stop()
{
    Assert(g_dbus_server_is_active(m_server) && "Server not started.");

    g_dbus_server_stop(m_server);
}

gboolean Server::onNewConnection(GDBusServer* /*server*/,
                                 GDBusConnection* connection,
                                 gpointer data)
{
    Assert(NULL != data && "User data cannot be NULL.");

    Server* self = static_cast<Server*>(data);

    ServerEvents::NewConnectionEvent event(
        ConnectionPtr(new Connection(connection)));

    LogInfo("Emitting new connection event");
    // TODO Blocking to allow object registration before any DBus messages are
    //      processed.
    self->DPL::Event::EventSupport<ServerEvents::NewConnectionEvent>::
        EmitEvent(event, DPL::Event::EmitMode::Blocking);

    return TRUE;
}
}
}
