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
 * @file    connection.h
 * @author  Zbigniew Kostrzewa (z.kostrzewa@samsung.com)
 * @version 1.0
 * @brief
 */

#ifndef DPL_DBUS_CONNECTION_H
#define DPL_DBUS_CONNECTION_H

#include <memory>
#include <vector>
#include <string>
#include <gio/gio.h>
#include <dpl/generic_event.h>
#include <dpl/event/event_support.h>
#include <dpl/dbus/object.h>
#include <dpl/dbus/object_proxy.h>

namespace DPL {
namespace DBus {

namespace ConnectionEvents
{
/**
 * Emitted when service name is acquired.
 *
 * Arg0 Acquired name.
 */
DECLARE_GENERIC_EVENT_1(ServiceNameAcquiredEvent, std::string)

/**
 * Emitted when service name is lost.
 *
 * Arg0 Lost name.
 */
DECLARE_GENERIC_EVENT_1(ServiceNameLostEvent, std::string)

/**
 * Emitted when remote host closes connection.
 *
 * Arg0 Low-level error message.
 */
DECLARE_GENERIC_EVENT_1(ConnectionBrokenEvent, std::string)

/**
 * Emitted when invalid or malformed data appear on connection.
 *
 * Arg0 Low-level error message.
 */
DECLARE_GENERIC_EVENT_1(ConnectionInvalidEvent, std::string)
}

class Server;

class Connection;
typedef std::shared_ptr<Connection> ConnectionPtr;

typedef std::shared_ptr<ObjectProxy> ObjectProxyPtr;

class Connection :
        public DPL::Event::EventSupport<ConnectionEvents::ServiceNameAcquiredEvent>,
        public DPL::Event::EventSupport<ConnectionEvents::ServiceNameLostEvent>,
        public DPL::Event::EventSupport<ConnectionEvents::ConnectionBrokenEvent>,
        public DPL::Event::EventSupport<ConnectionEvents::ConnectionInvalidEvent>
{
public:
    /**
     * Acquires connection to session bus.
     *
     * @return Session bus connection.
     * @throw DBus::Exception If unable to connect to session bus.
     */
    static ConnectionPtr sessionBus();

    /**
     * Acquires connection to system bus.
     *
     * @return System bus connection.
     * @throw DBus::Exception If unable to connect to system bus.
     */
    static ConnectionPtr systemBus();

    /**
     * Acquires connection to specified bus.
     *
     * @return Bus connection.
     * @throw DBus::Exception If unable to connect to a bus.
     */
    static ConnectionPtr connectTo(GBusType busType);

    /**
     * Acquires connection to for specified address.
     *
     * @return Connection.
     * @throw DBus::Exception If unable to connect.
     *
     * @remarks Address should be in DBus format (@see DBus documentation).
     */
    static ConnectionPtr connectTo(const std::string& address);

    ~Connection();

    /**
     * Sets up a service on the connection.
     *
     * @param serviceName Service to register.
     * @throw DBus::Exception If registration failed.
     *
     * @remarks Add objects before services to prevent notifications about new
     *          interfaces being added.
     */
    void registerService(const std::string& serviceName);

    /**
     * Unregisters a service from the connection.
     *
     * @param serviceName Service to unregister.
     * @throw DBus::Exception If service not registered.
     */
    void unregisterService(const std::string& serviceName);

    /**
     * Adds object to the connection.
     *
     * @param object Object to register.
     * @throw DBus::Exception If registration failed.
     *
     * @remarks Add objects before services to prevent notifications about new
     *          interfaces being added.
     */
    void registerObject(const ObjectPtr& object);

    /**
     * Removed object from the connection.
     *
     * @param objectPath Path of the object to unregister.
     * @throw DBus::Exception If object not registered.
     */
    void unregisterObject(const std::string& objectPath);

    /**
     * Creates proxy to remote objects.
     *
     * @param serviceName Name of the DBus service.
     * @param objectPath DBus path to the object.
     * @return Object proxy.
     * @throw DBus::ConnectionClosedException If connection is closed.
     */
    ObjectProxyPtr createObjectProxy(const std::string& serviceName,
                                     const std::string& objectPath);

private:
    friend class Server;

    typedef std::map<std::string, guint> RegisteredServices;

    struct ObjectRegistration
    {
        ObjectRegistration(guint registrationId, const ObjectPtr& object)
            : registrationId(registrationId),
              object(object)
        {
        }

        guint registrationId;
        ObjectPtr object;
    };
    typedef std::map<std::string, ObjectRegistration> RegisteredObjects;

    static void onServiceNameAcquired(GDBusConnection* connection,
                                      const gchar* serviceName,
                                      gpointer data);

    static void onServiceNameLost(GDBusConnection* connection,
                                  const gchar* serviceName,
                                  gpointer data);

    static void onConnectionClosed(GDBusConnection* connection,
                                   gboolean peerVanished,
                                   GError* error,
                                   gpointer data);

    explicit Connection(GDBusConnection* connection);

    GDBusConnection* m_connection;

    RegisteredServices m_registeredServices;

    RegisteredObjects m_registeredObjects;
};

}
}

#endif // DPL_DBUS_CONNECTION_H
