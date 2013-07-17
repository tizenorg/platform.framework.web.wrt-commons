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
 * @file        dbus_interface_dispatcher.h
 * @author      Tomasz Swierczek (t.swierczek@samsung.com)
 * @version     1.0
 * @brief       This file contains definitions of DBus::InterfaceDispatcher
 *              class.
 */

#ifndef DPL_DBUS_DBUS_INTERFACE_DISPATCHER_H_
#define DPL_DBUS_DBUS_INTERFACE_DISPATCHER_H_

#include <string>
#include <dpl/log/log.h>
#include <dpl/dbus/dispatcher.h>

namespace DPL {
namespace DBus {
class InterfaceDispatcher : public DBus::Dispatcher
{
  public:
    explicit InterfaceDispatcher(const std::string& interfaceName) :
        m_interfaceName(interfaceName)
    {}

    virtual ~InterfaceDispatcher()
    {}

    // Implement it in specific interface with method handling
    virtual void onMethodCall(const gchar* /*methodName*/,
                              GVariant* /*parameters*/,
                              GDBusMethodInvocation* /*invocation*/) = 0;

    virtual std::string getName() const
    {
        return m_interfaceName;
    }

    virtual std::string getXmlSignature()  const
    {
        return m_xml;
    }
    virtual void setXmlSignature(const std::string& newSignature)
    {
        m_xml = newSignature;
    }

    virtual void onMethodCall(GDBusConnection* /*connection*/,
                              const gchar* /*sender*/,
                              const gchar* /*objectPath*/,
                              const gchar* interfaceName,
                              const gchar* methodName,
                              GVariant* parameters,
                              GDBusMethodInvocation* invocation)
    {
        if (g_strcmp0(interfaceName, m_interfaceName.c_str()) == 0) {
            onMethodCall(methodName, parameters, invocation);
        } else {
            LogPedantic("Called invalid interface: " << interfaceName <<
                        " instead of: " << m_interfaceName);
        }
    }

    virtual GVariant* onPropertyGet(GDBusConnection* /*connection*/,
                                    const gchar* /*sender*/,
                                    const gchar* /*objectPath*/,
                                    const gchar* /*interfaceName*/,
                                    const gchar* propertyName)
    {
        LogDebug("InterfaceDispatcher onPropertyGet: " << propertyName);
        return NULL;
    }

    virtual gboolean onPropertySet(GDBusConnection* /*connection*/,
                                   const gchar* /*sender*/,
                                   const gchar* /*objectPath*/,
                                   const gchar* /*interfaceName*/,
                                   const gchar* propertyName,
                                   GVariant* /*value*/)
    {
        LogDebug("InterfaceDispatcher onPropertySet: " << propertyName);
        return false;
    }

  private:
    std::string m_interfaceName, m_xml;
};
} // namespace DBus
} // namespace DPL

#endif // DPL_DBUS_DBUS_INTERFACE_DISPATCHER_H_
