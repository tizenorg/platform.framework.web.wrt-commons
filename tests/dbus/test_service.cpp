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
 * @file    test_service.cpp
 * @author  Zbigniew Kostrzewa (z.kostrzewa@samsung.com)
 * @brief   Implementation file for wrt-dbus-test-service.
 */

#include <string>
#include <dpl/dbus/connection.h>
#include <dpl/dbus/object.h>
#include <dpl/dbus/interface.h>
#include <dpl/dbus/dispatcher.h>
#include "loop_control.h"

namespace {
const std::string serviceName = "org.tizen.DBusTestService";
const std::string objectPath = "/org/tizen/DBusTestService";
const std::string interfaceName = "org.tizen.DBusTestService";
const std::string methodNameEcho = "echo";
const std::string methodNameQuit = "quit";
const std::string nodeInfo =
        "<?xml version='1.0'?>"
        "<node>"
        "  <interface name='" + interfaceName + "'>"
        "    <method name='" + methodNameEcho + "'>"
        "      <arg type='s' name='challenge' direction='in'/>"
        "      <arg type='s' name='response' direction='out'/>"
        "    </method>"
        "    <method name='" + methodNameQuit + "'>"
        "    </method>"
        "  </interface>"
        "</node>";
}

class TestServiceDispatcher : public DPL::DBus::Dispatcher
{
private:
    void onMethodCall(GDBusConnection* /*connection*/,
                      const gchar* /*sender*/,
                      const gchar* /*objectPath*/,
                      const gchar* /*interfaceName*/,
                      const gchar* methodName,
                      GVariant* parameters,
                      GDBusMethodInvocation* invocation)
    {
        if (methodNameEcho == methodName)
        {
            LogDebug("Echo");
            g_dbus_method_invocation_return_value(invocation,
                                                  parameters);
        }
        else if (methodNameQuit == methodName)
        {
            LogDebug("Quit");
            g_dbus_method_invocation_return_value(invocation, NULL);
            LoopControl::wrt_end_loop();
        }
    }
};

int main(int argc, char* argv[])
{
    LoopControl::init_loop(argc, argv);

    TestServiceDispatcher dispatcher;

    auto iface = DPL::DBus::Interface::fromXMLString(nodeInfo).at(0);
    iface->setDispatcher(&dispatcher);

    auto object = DPL::DBus::Object::create(objectPath, iface);

    auto connection = DPL::DBus::Connection::sessionBus();
    connection->registerObject(object);
    connection->registerService(serviceName);

    LoopControl::wrt_start_loop();

    return 0;
}
