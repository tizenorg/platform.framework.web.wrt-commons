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
 * @file        dbus_client.h
 * @author      Tomasz Swierczek (t.swierczek@samsung.com)
 * @version     1.0
 * @brief       Header file for DBus generic client support
 */

#ifndef DPL_DBUS_DBUS_CLIENT_H_
#define DPL_DBUS_DBUS_CLIENT_H_

#include <string>
#include <dbus/dbus.h>
#include <dpl/exception.h>
#include <dpl/log/log.h>
#include <dpl/dbus/dbus_serialization.h>
#include <dpl/dbus/dbus_deserialization.h>

namespace DPL {
namespace DBus {

/*
 * DBus::Client class is intended to act as simple DBus client. To call a method
 * on remote service "Service", on remote object "Object", interface
 * "Interface",use it like this:
 *
 *
 *  DBus::Client client("Object", "Service", "Interface");
 *  (...) // variables declarations
 *  client.call("Method name", arg1, arg2, arg2, ... argN,
 *                             &outArg1, &outArg2, &outArg3, ..., &outArgN);
 *
 *
 * As You can see, input parameters of the call are passed with reference,
 * output ones are passed as pointers - parameters MUST be passed this way.
 *
 * To call a void function (no out params), just pass in arguments to Call().
 *
 * Currently client supports serialization and deserialization of simple types
 * (int, char, float, unsigned), strings (std::string and char*) and
 * some STL containers (std::vector, std::list, std::map, std::set, std::pair).
 * Structures and classes are not (yet) supported.
 */

class Client
{

  public:
    class Exception
    {
      public:
        DECLARE_EXCEPTION_TYPE(DPL::Exception, Base)
        DECLARE_EXCEPTION_TYPE(Base, DBusClientException)
    };

    Client(std::string serverPath,
           std::string serviceName,
           std::string interfaceName) :
            m_serviceName(serviceName),
            m_serverPath(serverPath),
            m_interfaceName(interfaceName)
    {
        DBusError error;

        dbus_error_init(&error);
        m_connection = dbus_bus_get(DBUS_BUS_SYSTEM, &error);
        if (NULL == m_connection) {
            LogPedantic("Couldn't get DBUS connection. Error: " <<
                    error.message);
            dbus_error_free(&error);
            ThrowMsg(Exception::DBusClientException,
                    "Couldn't get DBUS connection." );
        }
    }

    template<typename ...Args>
    void call(const char* methodName, const Args&... args)
    {
        DBusMessage* message = dbus_message_new_method_call(
                m_serviceName.c_str(),
                m_serverPath.c_str(),
                m_interfaceName.c_str(),
                methodName);
        DBusMessageIter argsIterator;
        dbus_message_iter_init_append(message, &argsIterator);
        call(message, &argsIterator, args...);
        dbus_message_unref(message);
    }

    template<typename ...Args>
    void call(std::string methodName, const Args&... args)
    {
        call(methodName.c_str(), args...);
    }

    ~Client()
    {
        dbus_connection_unref(m_connection);
    }

  private:

    DBusMessage* makeCall(
            DBusMessage* message)
    {
        DBusError error;
        dbus_error_init(&error);
        DBusMessage* ret = dbus_connection_send_with_reply_and_block(
                m_connection,
                message,
                -1,
                &error);
        if (NULL == ret) {
            LogPedantic("Error sending DBUS message: " <<
                    error.message);
            dbus_error_free(&error);
            ThrowMsg(Exception::DBusClientException,
                    "Error sending DBUS message." );
        }
        return ret;
    }

    void call(DBusMessage* message, DBusMessageIter* /*argsIterator*/)
    {
        DBusMessage* ret = makeCall(message);
        if (ret != NULL) {
            dbus_message_unref(ret);
        } else {
            LogPedantic("Error getting DBUS response.");
            ThrowMsg(Exception::DBusClientException,
                    "Error getting DBUS response." );
        }
    }

    template<typename T, typename ... Args>
    void call(
            DBusMessage* message,
            DBusMessageIter* argsIterator,
            const T& invalue,
            const Args&... args)
    {
        if (!Serialization::serialize(argsIterator, invalue)){
            LogPedantic("Error in serialization.");
            ThrowMsg(Exception::DBusClientException,
                    "Error in serialization." );
        }
        call(message, argsIterator, args...);
    }

    template<typename T, typename ... Args>
    void call(
            DBusMessage* message,
            DBusMessageIter* argsIterator,
            const T* invalue,
            const Args&... args)
    {
        if (!Serialization::serialize(argsIterator, invalue)){
            LogPedantic("Error in serialization.");
            ThrowMsg(Exception::DBusClientException,
                    "Error in serialization." );
        }
        call(message, argsIterator, args...);
    }

    template<typename T, typename ... Args>
    void call(
            DBusMessage* message,
            DBusMessageIter* argsIterator,
            const T* invalue)
    {
        if (!Serialization::serialize(argsIterator, invalue)){
            LogPedantic("Error in serialization.");
            ThrowMsg(Exception::DBusClientException,
                    "Error in serialization." );
        }
        call(message, argsIterator);
    }

    template<typename T, typename ... Args>
    void call(
            DBusMessage* message,
            DBusMessageIter* /*argsIterator*/,
            T* out,
            const Args&... args)
    {
        DBusMessage* ret = makeCall(message);
        if (ret != NULL) {
            DBusMessageIter responseIterator;
            dbus_message_iter_init(ret, &responseIterator);
            returnFromCall(&responseIterator, out, args...);
            dbus_message_unref(ret);
        }
    }

    template<typename T, typename ... Args>
    void returnFromCall(
            DBusMessageIter* responseIterator,
            T* out,
            const Args&... args)
    {
        if (!Deserialization::deserialize(responseIterator, out)){
            LogPedantic("Error in deserialization.");
            ThrowMsg(Exception::DBusClientException,
                    "Error in deserialization." );
        }
        returnFromCall(responseIterator, args...);
    }

    template<typename T>
    void returnFromCall(DBusMessageIter* responseIterator, T* out)
    {
        if (!Deserialization::deserialize(responseIterator, out)){
            LogPedantic("Error in deserialization.");
            ThrowMsg(Exception::DBusClientException,
                    "Error in deserialization." );
        }
    }

    std::string m_serviceName, m_serverPath, m_interfaceName;
    DBusConnection* m_connection;
};

} // namespace DBus
} // namespace DPL

#endif // DPL_DBUS_DBUS_CLIENT_H_
