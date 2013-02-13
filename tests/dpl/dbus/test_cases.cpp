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
 * @file    TestCases.cpp
 * @author  Zbigniew Kostrzewa (z.kostrzewa@samsung.com)
 * @version 1.0
 * @brief   Implementation file for test cases for DBus internal tests.
 */

#include <string>
#include <dpl/test/test_runner.h>
#include <dpl/event/event_listener.h>
#include <dpl/dbus/exception.h>
#include <dpl/dbus/connection.h>
#include <dpl/dbus/interface.h>
#include "dbus_test.h"

namespace {
const std::string dbusServiceName = "org.freedesktop.DBus";
const std::string dbusObjectPath = "/";
const std::string dbusInterfaceName = "org.freedesktop.DBus";
const std::string dbusMethodGetId = "GetId";

const std::string serviceName = "org.tizen.DBusTestService";
const std::string objectPath = "/org/tizen/DBusTestService";
const std::string interfaceName = "org.tizen.DBusTestService";
const std::string methodNameEcho = "echo";
const std::string methodNameQuit = "quit";
const std::string nodeInfo =
    "<?xml version='1.0'?>"
    "<node>"
    "  <interface name='" + interfaceName + "'>"
                                            "    <method name='" +
    methodNameEcho + "'>"
                     "      <arg type='s' name='challenge' direction='in'/>"
                     "      <arg type='s' name='response' direction='out'/>"
                     "    </method>"
                     "    <method name='"
    + methodNameQuit + "'>"
                       "    </method>"
                       "  </interface>"
                       "</node>";

const std::string challenge = "Hello world!";

const int DEFAULT_TIMEOUT = 2; // in seconds
}

RUNNER_TEST_GROUP_INIT(DPL)

/*
Name: AcquireSessionBus
Description: tests acquiring session bus
Expected: no exceptions
*/
RUNNER_TEST(AcquireSessionBus)
{
    try {
        DPL::DBus::Connection::sessionBus();
    } catch (const DPL::DBus::Exception& ex) {
        RUNNER_ASSERT_MSG(false, ex.DumpToString());
    }
}

/*
Name: AcquireSystemBus
Description: tests acquiring system bus
Expected: no exceptions
*/
RUNNER_TEST(AcquireSystemBus)
{
    try {
        DPL::DBus::Connection::systemBus();
    } catch (const DPL::DBus::Exception& ex) {
        RUNNER_ASSERT_MSG(false, ex.DumpToString());
    }
}

/*
Name: ParseNodeInfo
Description: creates dbus interface from xml string
Expected: interface should be created correctly
*/
RUNNER_TEST(ParseNodeInfo)
{
    try {
        auto ifaces = DPL::DBus::Interface::fromXMLString(nodeInfo);
        RUNNER_ASSERT(!ifaces.empty());

        auto iface = ifaces.at(0);
        RUNNER_ASSERT(NULL != iface->getVTable());
        RUNNER_ASSERT(NULL != iface->getInfo());
    } catch (const DPL::DBus::Exception& ex) {
        RUNNER_ASSERT_MSG(false, ex.DumpToString());
    }
}

/*
Name: InvokeRemoteMethod
Description: performs procedure call via dbus
Expected: call should return not empty id
*/
RUNNER_TEST(InvokeRemoteMethod)
{
    try {
        auto connection = DPL::DBus::Connection::systemBus();
        auto freedesktop = connection->createObjectProxy(dbusServiceName,
                                                         dbusObjectPath);
        auto getId = freedesktop->createMethodProxy<std::string>
                (dbusInterfaceName, dbusMethodGetId);
        RUNNER_ASSERT(!getId().empty());
    } catch (const DPL::DBus::Exception& ex) {
        RUNNER_ASSERT_MSG(false, ex.DumpToString());
    }
}

class RegisterServiceListener :
    public DPL::Event::EventListener<DPL::DBus::ConnectionEvents::
                                         ServiceNameAcquiredEvent>
{
  public:
    void OnEventReceived(
        const DPL::DBus::ConnectionEvents::ServiceNameAcquiredEvent& event)
    {
        DBusTest& test = DBusTestManager::getInstance().getCurrentTest();

        auto name = event.GetArg0();
        if (serviceName == name) {
            test.success();
        } else {
            test.fail("Acquired service name: " + name);
        }
        test.quit();
    }
};

/*
Name: RegisterService
Description: tests event listener for AcquiredEvent in context of dbus
Expected: event should be received
*/
DBUS_TEST(RegisterService)
{
    try {
        RegisterServiceListener listener;

        auto connection = DPL::DBus::Connection::sessionBus();
        connection->DPL::Event::EventSupport<DPL::DBus::ConnectionEvents::
                                                 ServiceNameAcquiredEvent>::
            AddListener(&listener);
        connection->registerService(serviceName);

        DBusTestManager::getInstance().getCurrentTest().run(DEFAULT_TIMEOUT);
    } catch (const DPL::DBus::Exception& ex) {
        RUNNER_ASSERT_MSG(false, ex.DumpToString());
    }
}

/**
 * This test checks:
 * - object registration (done on the wrt-dbus-test-service side)
 * - service registration (done on the wrt-dbus-test-service side)
 * - dispatching method calls (done on the wrt-dbus-test-service side)
 * - launching dbus service on demand
 * - invoking remote method(s)
 */
DBUS_TEST(InvokeTestService)
{
    try {
        auto connection = DPL::DBus::Connection::sessionBus();
        auto testService = connection->createObjectProxy(serviceName,
                                                         objectPath);
        auto echo = testService->createMethodProxy<std::string, std::string>
                (interfaceName, methodNameEcho);
        auto response = echo(challenge);

        testService->createMethodProxy<void>(interfaceName, methodNameQuit) ();

        RUNNER_ASSERT_MSG(response == challenge,
                          "[challenge = " << challenge <<
                          ", response = " << response << "]");
    } catch (const DPL::DBus::Exception& ex) {
        RUNNER_ASSERT_MSG(false, ex.DumpToString());
    }
}
