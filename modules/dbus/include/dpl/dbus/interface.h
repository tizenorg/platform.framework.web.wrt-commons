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
 * @file    interface.h
 * @author  Zbigniew Kostrzewa (z.kostrzewa@samsung.com)
 * @version 1.0
 * @brief
 */

#ifndef DPL_DBUS_INTERFACE_H
#define DPL_DBUS_INTERFACE_H

#include <memory>
#include <vector>
#include <gio/gio.h>
#include <dpl/noncopyable.h>
#include <dpl/dbus/dispatcher.h>

namespace DPL {
namespace DBus {

class Interface;
typedef std::shared_ptr<Interface> InterfacePtr;

class Interface : private DPL::Noncopyable
{
public:
    /**
     * Parses supplied XML string to produce DBus interface descriptions.
     *
     * @param xmlString XML string to parse.
     * @return Interfaces.
     * @throw DPL::DBus::Exception If error while parsing occurs.
     */
    static std::vector<InterfacePtr> fromXMLString(
            const std::string& xmlString);

public:
    ~Interface();

    /**
     * Gets pointers to functions called on method call or property get/set
     * request.
     *
     * @return Pointers to functions.
     */
    const GDBusInterfaceVTable* getVTable() const;

    /**
     * Gets interface description.
     *
     * @return Interface description.
     */
    GDBusInterfaceInfo* getInfo() const;

    /**
     * Sets method/property dispatcher for the interface.
     *
     * @param dispatcher Method call and property get/set dispatcher.
     */
    void setDispatcher(Dispatcher* dispatcher);

private:
    static void onMethodCallFunc(GDBusConnection *connection,
                                 const gchar *sender,
                                 const gchar *objectPath,
                                 const gchar *interfaceName,
                                 const gchar *methodName,
                                 GVariant *parameters,
                                 GDBusMethodInvocation *invocation,
                                 gpointer data);

    static GVariant* onPropertyGetFunc(GDBusConnection *connection,
                                       const gchar *sender,
                                       const gchar *objectPath,
                                       const gchar *interfaceName,
                                       const gchar *propertyName,
                                       GError **error,
                                       gpointer data);

    static gboolean onPropertySetFunc(GDBusConnection *connection,
                                      const gchar *sender,
                                      const gchar *objectPath,
                                      const gchar *interfaceName,
                                      const gchar *propertyName,
                                      GVariant *value,
                                      GError **error,
                                      gpointer data);

    explicit Interface(GDBusInterfaceInfo* info);

    static const GDBusInterfaceVTable m_vTable;

    GDBusInterfaceInfo* m_info;

    Dispatcher* m_dispatcher;
};

}
}

#endif // DPL_DBUS_INTERFACE_H
