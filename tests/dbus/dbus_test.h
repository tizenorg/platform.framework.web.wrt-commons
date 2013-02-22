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
 * @file    dbus_test.h
 * @author  Zbigniew Kostrzewa (z.kostrzewa@samsung.com)
 * @brief   Header file for DBusTest and DBusTestManager.
 */

#ifndef WRT_TESTS_DBUS_TESTS_DBUS_TEST_H
#define WRT_TESTS_DBUS_TESTS_DBUS_TEST_H

#include <string>
#include <dpl/event/controller.h>
#include <dpl/generic_event.h>

DECLARE_GENERIC_EVENT_0(QuitEvent)
DECLARE_GENERIC_EVENT_0(TimeoutEvent)

class DBusTest :
    private DPL::Event::Controller<DPL::TypeListDecl<QuitEvent,
                                                     TimeoutEvent>::Type>
{
  public:
    enum class Status
    {
        NONE,
        SUCCESS,
        FAILED
    };

    explicit DBusTest(const std::string& name);

    void run(unsigned int timeout);
    void quit();

    void setStatus(Status status);
    void setMessage(const std::string& message);

    void success();
    void fail(const std::string& message = std::string());

  private:
    void OnEventReceived(const TimeoutEvent& event);
    void OnEventReceived(const QuitEvent& event);

    std::string m_name;
    Status m_status;
    std::string m_message;
};

class DBusTestManager : private DPL::Noncopyable
{
  public:
    static DBusTestManager& getInstance();

    DBusTest& getCurrentTest() const;
    void setCurrentTest(DBusTest& test);

    void clear();

  private:
    DBusTestManager();

    DBusTest* m_test;
};

#define DBUS_TEST(TestProc)                                                    \
    void DBus##TestProc();                                                     \
    RUNNER_TEST(TestProc)                                                      \
    {                                                                          \
        DBusTest test(#TestProc);                                              \
        DBusTestManager::getInstance().setCurrentTest(test);                   \
        DBus##TestProc();                                                      \
        DBusTestManager::getInstance().clear();                                \
    }                                                                          \
    void DBus##TestProc()

#endif
