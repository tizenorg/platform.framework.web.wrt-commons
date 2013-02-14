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
 * @file    interface.cpp
 * @author  Zbigniew Kostrzewa (z.kostrzewa@samsung.com)
 * @version 1.0
 * @brief
 */
#include <stddef.h>
#include <dpl/log/log.h>
#include <dpl/assert.h>
#include <dpl/exception.h>
#include <dpl/dbus/exception.h>
#include <dpl/dbus/interface.h>

namespace DPL {
namespace DBus {
const GDBusInterfaceVTable Interface::m_vTable = {
    Interface::onMethodCallFunc,
    Interface::onPropertyGetFunc,
    Interface::onPropertySetFunc,
    { 0, 0, 0, 0, 0, 0, 0, 0 }
};

std::vector<InterfacePtr> Interface::fromXMLString(const std::string& xmlString)
{
    GError* error = NULL;

    GDBusNodeInfo* nodeInfo = g_dbus_node_info_new_for_xml(xmlString.c_str(),
                                                           &error);
    if (NULL == nodeInfo) {
        std::string message;
        if (NULL != error) {
            message = error->message;
            g_error_free(error);
        }
        ThrowMsg(DPL::DBus::Exception,
                 "Error parsing node info <" << message << ">");
    }

    std::vector<InterfacePtr> result;

    GDBusInterfaceInfo** interface = nodeInfo->interfaces;
    while (NULL != *interface) {
        result.push_back(InterfacePtr(new Interface(*interface)));
        ++interface;
    }

    g_dbus_node_info_unref(nodeInfo);

    return result;
}

Interface::Interface(GDBusInterfaceInfo* info) :
    m_info(info)
{
    g_dbus_interface_info_ref(m_info);
}

Interface::~Interface()
{
    g_dbus_interface_info_unref(m_info);
}

const GDBusInterfaceVTable* Interface::getVTable() const
{
    return &m_vTable;
}

GDBusInterfaceInfo* Interface::getInfo() const
{
    return m_info;
}

void Interface::setDispatcher(Dispatcher* dispatcher)
{
    m_dispatcher = dispatcher;
}

void Interface::onMethodCallFunc(GDBusConnection *connection,
                                 const gchar *sender,
                                 const gchar *objectPath,
                                 const gchar *interfaceName,
                                 const gchar *methodName,
                                 GVariant *parameters,
                                 GDBusMethodInvocation *invocation,
                                 gpointer data)
{
    Assert(NULL != data && "Interface cannot be NULL.");
    Interface* self = static_cast<Interface*>(data);

    // TODO Verify interface name.

    if (NULL != self->m_dispatcher) {
        try {
            self->m_dispatcher->onMethodCall(connection,
                                             sender,
                                             objectPath,
                                             interfaceName,
                                             methodName,
                                             parameters,
                                             invocation);
        } catch (const DPL::Exception& /*ex*/) {
            // TODO Support for errors.
        }
    }
}

GVariant* Interface::onPropertyGetFunc(GDBusConnection *connection,
                                       const gchar *sender,
                                       const gchar *objectPath,
                                       const gchar *interfaceName,
                                       const gchar *propertyName,
                                       GError **error,
                                       gpointer data)
{
    Assert(NULL != data && "Interface cannot be NULL.");
    Interface* self = static_cast<Interface*>(data);

    // TODO Verify interface name.

    if (NULL != self->m_dispatcher) {
        try {
            // TODO Check if NULL is returned, if so set error variable.
            return self->m_dispatcher->onPropertyGet(connection,
                                                     sender,
                                                     objectPath,
                                                     interfaceName,
                                                     propertyName,
                                                     error);
        } catch (const DPL::Exception& /*ex*/) {
            // TODO Support for errors.
        }
    }

    // TODO Set error.

    return NULL;
}

gboolean Interface::onPropertySetFunc(GDBusConnection *connection,
                                      const gchar *sender,
                                      const gchar *objectPath,
                                      const gchar *interfaceName,
                                      const gchar *propertyName,
                                      GVariant *value,
                                      GError **error,
                                      gpointer data)
{
    Assert(NULL != data && "Interface cannot be NULL.");
    Interface* self = static_cast<Interface*>(data);

    // TODO Verify interface name.

    if (NULL != self->m_dispatcher) {
        try {
            return self->m_dispatcher->onPropertySet(connection,
                                                     sender,
                                                     objectPath,
                                                     interfaceName,
                                                     propertyName,
                                                     value,
                                                     error);
        } catch (const DPL::Exception& /*ex*/) {
            // TODO Support for errors.
        }
    }

    // TODO Set error.

    return false;
}
}
}
