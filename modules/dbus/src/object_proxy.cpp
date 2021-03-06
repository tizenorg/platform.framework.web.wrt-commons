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
 * @file    object_proxy.cpp
 * @author  Zbigniew Kostrzewa (z.kostrzewa@samsung.com)
 * @version 1.0
 * @brief
 */

#include <dpl/dbus/object_proxy.h>

namespace DPL {
namespace DBus {

ObjectProxy::ObjectProxy(GDBusConnection* connection,
                         const std::string& serviceName,
                         const std::string& objectPath)
    : m_connection(connection),
      m_serviceName(serviceName),
      m_objectPath(objectPath)
{
    g_object_ref(m_connection);
}

ObjectProxy::~ObjectProxy()
{
    g_object_unref(m_connection);
}

}
}