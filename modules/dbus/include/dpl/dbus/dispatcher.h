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
 * @file    dispatcher.h
 * @author  Zbigniew Kostrzewa (z.kostrzewa@samsung.com)
 * @version 1.0
 * @brief
 */

#ifndef DPL_DBUS_DISPATCHER_H
#define DPL_DBUS_DISPATCHER_H

#include <gio/gio.h>

namespace DPL {
namespace DBus {

class Dispatcher
{
public:
    virtual ~Dispatcher() =0;

    /**
     * Called on method invocation.
     *
     * @param connection
     * @param sender
     * @param objectPath
     * @param interfaceName
     * @param methodName
     * @param parameters
     * @param invocation
     *
     * @see GLib DBus documentation.
     */
    virtual void onMethodCall(GDBusConnection* connection,
                              const gchar* sender,
                              const gchar* objectPath,
                              const gchar* interfaceName,
                              const gchar* methodName,
                              GVariant* parameters,
                              GDBusMethodInvocation* invocation) =0;

    /**
     * Called on property get.
     *
     * @param connection
     * @param sender
     * @param objectPath
     * @param interfaceName
     * @param propertyName
     * @return Porperty value.
     *
     * @see GLib DBus documentation.
     */
    virtual GVariant* onPropertyGet(GDBusConnection* connection,
                                    const gchar* sender,
                                    const gchar* objectPath,
                                    const gchar* interfaceName,
                                    const gchar* propertyName,
                                    GError** error);

    /**
     * Called on property set.
     *
     * @param connection
     * @param sender
     * @param objectPath
     * @param interfaceName
     * @param propertyName
     * @param value
     * @return TRUE if successfully set, FALSE otherwise.
     *
     * @see GLib DBus documentation.
     */
    virtual gboolean onPropertySet(GDBusConnection* connection,
                                   const gchar* sender,
                                   const gchar* objectPath,
                                   const gchar* interfaceName,
                                   const gchar* propertyName,
                                   GVariant* value,
                                   GError** error);
};

}
}

#endif // DPL_DBUS_DISPATCHER_H
