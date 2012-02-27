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
 * @file    object_proxy.h
 * @author  Zbigniew Kostrzewa (z.kostrzewa@samsung.com)
 * @version 1.0
 * @brief
 */

#ifndef DPL_DBUS_OBJECT_PROXY_H
#define DPL_DBUS_OBJECT_PROXY_H

#include <string>
#include <gio/gio.h>
#include <dpl/dbus/exception.h>
#include <dpl/dbus/method_proxy.h>

namespace DPL {
namespace DBus {

class Connection;

/**
 * Represents a remote object attached to a DBus service.
 */
class ObjectProxy
{
public:
    ~ObjectProxy();

    /**
     * Creates method proxy object.
     *
     * The object is used to call remote methods.
     *
     * @param interface Name of the DBus interface.
     * @param name Name of the method to call.
     * @return Proxy to remote method.
     * @throw DBus::ConnectionClosedException If connection is closed.
     */
    template<typename Result, typename ...Args>
    MethodProxyPtr<Result, Args...> createMethodProxy(
            const std::string& interface,
            const std::string& name)
    {
        if (g_dbus_connection_is_closed(m_connection))
        {
            ThrowMsg(DBus::ConnectionClosedException, "Connection closed.");
        }

        return MethodProxyPtr<Result, Args...>(
                new MethodProxy<Result, Args...>(m_connection,
                                                 m_serviceName,
                                                 m_objectPath,
                                                 interface,
                                                 name));
    }

private:
    friend class Connection;

    ObjectProxy(GDBusConnection* connection,
                const std::string& serviceName,
                const std::string& objectPath);

    GDBusConnection* m_connection;
    std::string m_serviceName;
    std::string m_objectPath;
};

}
}

#endif
