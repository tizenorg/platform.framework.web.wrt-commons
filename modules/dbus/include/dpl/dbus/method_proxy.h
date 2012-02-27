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
 * @file    method_proxy.h
 * @author  Zbigniew Kostrzewa (z.kostrzewa@samsung.com)
 * @version 1.0
 * @brief
 */

#ifndef DPL_DBUS_METHOD_PROXY_H
#define DPL_DBUS_METHOD_PROXY_H

#include <type_traits>
#include <utility>
#include <memory>
#include <string>
#include <gio/gio.h>
#include <dpl/log/log.h>
#include <dpl/assert.h>
#include <dpl/dbus/exception.h>
#include <dpl/dbus/dbus_server_serialization.h>
#include <dpl/dbus/dbus_server_deserialization.h>

namespace DPL {
namespace DBus {

class ObjectProxy;

/**
 * Represents a remote method.
 */
template<typename Result, typename ...Args>
class MethodProxy
{
public:
    ~MethodProxy()
    {
        g_object_unref(m_connection);
    }

    /**
     * Invokes remote method.
     *
     * @param args Input arguments for remote method.
     * @return Value returned by remote method.
     * @throw DBus::InvalidArgumentException If invalid argument(s) supplied.
     * @throw DBus::ConnectionClosedException If connection is closed.
     * @throw DBus::Exception If some other error occurs.
     */
    Result operator()(const Args&... args)
    {
        return invoke(args...);
    }

private:
    friend class ObjectProxy;

    MethodProxy(GDBusConnection* connection,
                const std::string& serviceName,
                const std::string& objectPath,
                const std::string& interfaceName,
                const std::string& methodName)
        : m_connection(connection),
          m_serviceName(serviceName),
          m_objectPath(objectPath),
          m_interfaceName(interfaceName),
          m_methodName(methodName)
    {
        Assert(m_connection && "Connection is not set.");

        g_object_ref(m_connection);
    }

    /**
     * @remarks Making it a template with parameter set by default to class
     *          template parameter to overload on return type by utilizing
     *          the SFINAE concept.
     */
    template<typename R = Result>
    typename std::enable_if<!std::is_void<R>::value, R>::type
    invoke(const Args&... args)
    {
        GVariant* parameters = serialize(args...);

        GVariant* invokeResult = invokeSync(parameters);

        R result;

        ServerDeserialization::deserialize(invokeResult, &result);

        g_variant_unref(invokeResult);

        return result;
    }

    /**
     * @remarks Void return type overload.
     */
    template<typename R = Result>
    typename std::enable_if<std::is_void<R>::value>::type
    invoke(const Args&... args)
    {
        GVariant* parameters = serialize(args...);

        GVariant* invokeResult = invokeSync(parameters);

        g_variant_unref(invokeResult);
    }

    /**
     * @remarks ArgsM... are the same as Args...; it has been made a template
     *          to make overloading/specialization possible.
     */
    template<typename ...ArgsM>
    GVariant* serialize(ArgsM&&... args)
    {
        return ServerSerialization::serialize(std::forward<ArgsM>(args)...);
    }

    /**
     * @remarks Specialization for zero-argument functions.
     */
    GVariant* serialize()
    {
        return NULL;
    }

    /**
     * Calls remote method over DBus.
     *
     * @param parameters Input parameters for the remote method.
     * @return Result returned by the remote method.
     * @throw DBus::InvalidArgumentException If invalid argument(s) supplied.
     * @throw DBus::ConnectionClosedException If connection is closed.
     * @throw DBus::Exception If some other error occurs.
     */
    GVariant* invokeSync(GVariant* parameters)
    {
        GError* error = NULL;

        LogPedantic("Invoking method: " << m_interfaceName << "." << m_methodName);
        GVariant* result = g_dbus_connection_call_sync(m_connection,
                                                       m_serviceName.c_str(),
                                                       m_objectPath.c_str(),
                                                       m_interfaceName.c_str(),
                                                       m_methodName.c_str(),
                                                       parameters,
                                                       G_VARIANT_TYPE_TUPLE,
                                                       G_DBUS_CALL_FLAGS_NONE,
                                                       DBUS_SYNC_CALL_TIMEOUT,
                                                       NULL,
                                                       &error);
        if (NULL == result)
        {
            std::ostringstream oss;
            oss << "Error while invoking: "
                << m_interfaceName << "." << m_methodName
                << " <" << error->message << ">";
            std::string message = oss.str();

            gint code = error->code;

            g_error_free(error);

            switch (code)
            {
            case G_IO_ERROR_INVALID_ARGUMENT:
                ThrowMsg(DBus::InvalidArgumentException, message);
            case G_IO_ERROR_CLOSED:
                ThrowMsg(DBus::ConnectionClosedException, message);
            default:
                ThrowMsg(DBus::Exception, message);
            }
        }

        return result;
    }

    /**
     * Default timeout for synchronous method call.
     *
     * @see GIO::GDBusConnection::g_dbus_connection_call_sync() for details.
     */
    static const gint DBUS_SYNC_CALL_TIMEOUT = -1;

    GDBusConnection* m_connection;
    std::string m_serviceName;
    std::string m_objectPath;
    std::string m_interfaceName;
    std::string m_methodName;
};

/**
 * Smart pointer for MethodProxy objects.
 */
template<typename Result, typename ...Args>
class MethodProxyPtr
{
public:
    explicit MethodProxyPtr(MethodProxy<Result, Args...>* method = NULL)
        : m_method(method)
    {
    }

    Result operator()(const Args&... args) const
    {
        Assert(NULL != m_method.get() && "Method not set.");

        return (*m_method)(args...);
    }

private:
    std::shared_ptr<MethodProxy<Result, Args...> > m_method;
};

}
}

#endif
