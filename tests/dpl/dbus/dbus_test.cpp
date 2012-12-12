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
 * @file    dbus_test.cpp
 * @author  Zbigniew Kostrzewa (z.kostrzewa@samsung.com)
 * @brief   Implementation file for DBusTest and DBusTestManager.
 */

#include <dpl/test/test_runner.h>
#include "loop_control.h"
#include "dbus_test.h"

DBusTest::DBusTest(const std::string& name)
    : m_name(name),
      m_status(Status::NONE)
{
}

void DBusTest::run(unsigned int timeout)
{
    DPL::Event::ControllerEventHandler<TimeoutEvent>::Touch();
    DPL::Event::ControllerEventHandler<QuitEvent>::Touch();

    DPL::Event::ControllerEventHandler<TimeoutEvent>::PostTimedEvent(
        TimeoutEvent(), timeout);

    LoopControl::wrt_start_loop();

    switch (m_status)
    {
    case Status::FAILED:
        throw DPL::Test::TestRunner::TestFailed(m_name.c_str(),
                                                __FILE__,
                                                __LINE__,
                                                m_message);

    default:
        break;
    }
}

void DBusTest::quit()
{
    DPL::Event::ControllerEventHandler<QuitEvent>::PostEvent(QuitEvent());
}

void DBusTest::setStatus(Status status)
{
    m_status = status;
}

void DBusTest::setMessage(const std::string& message)
{
    m_message = message;
}

void DBusTest::success()
{
    m_status = Status::SUCCESS;
}

void DBusTest::fail(const std::string& message)
{
    m_status = Status::FAILED;
    m_message = message;
}

void DBusTest::OnEventReceived(const TimeoutEvent& /*event*/)
{
    fail("Test timed out.");

    // Saving one event dispatch since Quit and Timeout work on the same thread.
    LoopControl::wrt_end_loop();
}

void DBusTest::OnEventReceived(const QuitEvent& /*event*/)
{
    LoopControl::wrt_end_loop();
}

DBusTestManager& DBusTestManager::getInstance()
{
    static DBusTestManager instance;
    return instance;
}

DBusTestManager::DBusTestManager() : m_test(NULL) { }

DBusTest& DBusTestManager::getCurrentTest() const
{
    Assert(NULL != m_test && "Test not set.");

    return *m_test;
}

void DBusTestManager::setCurrentTest(DBusTest& test)
{
    m_test = &test;
}

void DBusTestManager::clear()
{
    m_test = NULL;
}
