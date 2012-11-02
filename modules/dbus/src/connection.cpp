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
 * @file    connection.cpp
 * @author  Zbigniew Kostrzewa (z.kostrzewa@samsung.com)
 * @version 1.0
 * @brief
 */
#include <stddef.h>
#include <dpl/log/log.h>
#include <dpl/dbus/connection.h>
#include <dpl/dbus/exception.h>
#include <dpl/dbus/object_proxy.h>

namespace DPL
{
namespace DBus
{

ConnectionPtr Connection::sessionBus()
{
    return connectTo(G_BUS_TYPE_SESSION);
}

ConnectionPtr Connection::systemBus()
{
    return connectTo(G_BUS_TYPE_SYSTEM);
}

ConnectionPtr Connection::connectTo(GBusType busType)
{
    GError* error = NULL;

    GDBusConnection* connection = g_bus_get_sync(busType,
                                                 NULL,
                                                 &error);
    if (NULL == connection)
    {
        std::string message;
        if (NULL != error)
        {
            message = error->message;
            g_error_free(error);
        }
        ThrowMsg(DBus::Exception,
                 "Couldn't connect to bus: " << message);
    }

    g_dbus_connection_set_exit_on_close(connection, FALSE);

    return ConnectionPtr(new Connection(connection));
}

ConnectionPtr Connection::connectTo(const std::string& address)
{
    GError* error = NULL;

    GDBusConnection* connection = g_dbus_connection_new_for_address_sync(
                                          address.c_str(),
                                          G_DBUS_CONNECTION_FLAGS_NONE,
                                          NULL,
                                          NULL,
                                          &error);
    if (NULL == connection)
    {
        std::string message;
        if (NULL != error)
        {
            message = error->message;
            g_error_free(error);
        }
        ThrowMsg(DBus::Exception,
                 "Couldn't connect to " << address << ": " << message);
    }

    return ConnectionPtr(new Connection(connection));
}

Connection::Connection(GDBusConnection* connection)
    : m_connection(connection)
{
    g_signal_connect(m_connection,
                     "closed",
                     G_CALLBACK(onConnectionClosed),
                     this);
}

Connection::~Connection()
{
    std::for_each(m_registeredServices.begin(),
                  m_registeredServices.end(),
                  [] (const RegisteredServices::value_type& value)
                  {
                      g_bus_unown_name(value.second);
                  });

    std::for_each(m_registeredObjects.begin(),
                  m_registeredObjects.end(),
                  [m_connection] (const RegisteredObjects::value_type& value)
                  {
                      g_dbus_connection_unregister_object(
                              m_connection,
                              value.second.registrationId);
                  });

    if (!g_dbus_connection_is_closed(m_connection))
    {
        GError* error = NULL;

        if (FALSE == g_dbus_connection_flush_sync(m_connection, NULL, &error))
        {
            LogPedantic("Could not flush the connection"
                     << " <" << error->message << ">");
            g_error_free(error);
        }
    }

    g_object_unref(m_connection);
}

void Connection::registerService(const std::string& serviceName)
{
    guint regId = g_bus_own_name_on_connection(m_connection,
                                               serviceName.c_str(),
                                               G_BUS_NAME_OWNER_FLAGS_NONE,
                                               onServiceNameAcquired,
                                               onServiceNameLost,
                                               this,
                                               NULL);
    if (0 >= regId)
    {
        ThrowMsg(DBus::Exception, "Error while registering service.");
    }

    m_registeredServices.insert(RegisteredServices::value_type(serviceName,
                                                               regId));
}

void Connection::unregisterService(const std::string& serviceName)
{
    auto it = m_registeredServices.find(serviceName);
    if (m_registeredServices.end() == it)
    {
        ThrowMsg(DBus::Exception, "Service not registered.");
    }

    g_bus_unown_name(it->second);

    m_registeredServices.erase(it);
}

void Connection::registerObject(const ObjectPtr& object)
{
    GError* error = NULL;

    guint regId = g_dbus_connection_register_object(
                          m_connection,
                          object->getPath().c_str(),
                          object->getInterface()->getInfo(),
                          object->getInterface()->getVTable(),
                          // TODO This is ugly, fix this!
                          object->getInterface().get(),
                          NULL,
                          &error);
    if (0 == regId)
    {
        std::string message;
        if (NULL != error)
        {
            message = error->message;
            LogPedantic(error->message << " " << error->code);
            g_error_free(error);
        }
        ThrowMsg(DBus::Exception, "Error while registering an object: "
                                  << message);
    }

    m_registeredObjects.insert(RegisteredObjects::value_type(
                                       object->getPath(),
                                       ObjectRegistration(regId, object)));
}

void Connection::unregisterObject(const std::string& objectPath)
{
    auto it = m_registeredObjects.find(objectPath);
    if (m_registeredObjects.end() == it)
    {
        ThrowMsg(DBus::Exception, "Object not registered.");
    }

    gboolean result = g_dbus_connection_unregister_object(
                              m_connection,
                              it->second.registrationId);
    if (FALSE == result)
    {
        ThrowMsg(DBus::Exception, "Unregistering object failed.");
    }
    m_registeredObjects.erase(it);
}

ObjectProxyPtr Connection::createObjectProxy(const std::string& serviceName,
                                             const std::string& objectPath)
{
    if (g_dbus_connection_is_closed(m_connection))
    {
        ThrowMsg(DBus::ConnectionClosedException, "Connection closed.");
    }

    return ObjectProxyPtr(
            new ObjectProxy(m_connection, serviceName, objectPath));
}

void Connection::onServiceNameAcquired(GDBusConnection* /*connection*/,
                                       const gchar* serviceName,
                                       gpointer data)
{
    Assert(data && "Connection should not be NULL");

    Connection* self = static_cast<Connection*>(data);

    LogPedantic("Emitting service name acquired event: " << serviceName);

    ConnectionEvents::ServiceNameAcquiredEvent event(serviceName);
    self->DPL::Event::EventSupport<ConnectionEvents::ServiceNameAcquiredEvent>::
            EmitEvent(event, DPL::Event::EmitMode::Queued);
}

void Connection::onServiceNameLost(GDBusConnection* /*connection*/,
                                   const gchar* serviceName,
                                   gpointer data)
{
    Assert(data && "Connection should not be NULL");

    Connection* self = static_cast<Connection*>(data);

    LogPedantic("Emitting service name lost event: " << serviceName);

    ConnectionEvents::ServiceNameLostEvent event(serviceName);
    self->DPL::Event::EventSupport<ConnectionEvents::ServiceNameLostEvent>::
            EmitEvent(event, DPL::Event::EmitMode::Queued);
}

void Connection::onConnectionClosed(GDBusConnection* /*connection*/,
                                    gboolean peerVanished,
                                    GError* error,
                                    gpointer data)
{
    Assert(NULL != data && "Connection cannot be NULL");

    Connection* self = static_cast<Connection*>(data);

    if ((NULL == error) && (FALSE == peerVanished))
    {
        // Connection closed by this.
    }
    else if (NULL != error)
    {
        std::string message = error->message;

        g_error_free(error);

        if (TRUE == peerVanished)
        {
            // Connection closed by remote host.
            ConnectionEvents::ConnectionBrokenEvent event(message);
            self->DPL::Event::EventSupport<ConnectionEvents::ConnectionBrokenEvent>::
                    EmitEvent(event, DPL::Event::EmitMode::Queued);
        }
        else
        {
            // Invalid or malformed data on connection.
            ConnectionEvents::ConnectionInvalidEvent event(message);
            self->DPL::Event::EventSupport<ConnectionEvents::ConnectionInvalidEvent>::
                    EmitEvent(event, DPL::Event::EmitMode::Queued);
        }
    }
}

}
}
