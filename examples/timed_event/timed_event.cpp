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
 * @file        timed_event.cpp
 * @author      Przemyslaw Dobrowolski (p.dobrowolsk@samsung.com)
 * @version     1.0
 * @brief       This file is the implementation file of timed event example
 */
#include <stddef.h>
#include <dpl/generic_event.h>
#include <dpl/application.h>
#include <dpl/controller.h>
#include <dpl/type_list.h>
#include <dpl/thread.h>
#include <dpl/log/log.h>
#include <string>

DECLARE_GENERIC_EVENT_0(FirstEvent)
DECLARE_GENERIC_EVENT_0(SecondEvent)

class ControllerInThread
    : public DPL::Controller<DPL::TypeListDecl<FirstEvent,
                                               SecondEvent>::Type>
{
protected:
    virtual void OnEventReceived(const FirstEvent &event)
    {
        (void)event;
        LogDebug("First event occurred");
    }

    virtual void OnEventReceived(const SecondEvent &event)
    {
        (void)event;
        LogDebug("Second event occurred");
    }
};

DECLARE_GENERIC_EVENT_0(QuitEvent)

class MyApplication
    : public DPL::Application,
      private DPL::Controller<DPL::TypeListDecl<QuitEvent>::Type>
{
private:
    DPL::Thread m_thread;
    ControllerInThread m_controllerInThread;

    // Quit application event occurred
    virtual void OnEventReceived(const QuitEvent &event)
    {
        (void)event;
        Quit();
    }

public:
    MyApplication(int argc, char **argv)
        : Application(argc, argv, "timed_event", false)
    {
        // Touch
        Touch();
        m_controllerInThread.Touch();

        // Run thread
        m_thread.Run();
        m_controllerInThread.SwitchToThread(&m_thread);

        // Emit thread timed events
        m_controllerInThread.DPL::ControllerEventHandler<SecondEvent>::PostTimedEvent(SecondEvent(), 3);
        m_controllerInThread.DPL::ControllerEventHandler<FirstEvent>::PostTimedEvent(FirstEvent(), 2);

        // Emit framework timed quit event
        DPL::ControllerEventHandler<QuitEvent>::PostTimedEvent(QuitEvent(), 5);
    }

    virtual ~MyApplication()
    {
        m_controllerInThread.SwitchToThread(NULL);

        // Quit thread
        m_thread.Quit();
    }
};

int main(int argc, char *argv[])
{
    MyApplication app(argc, argv);
    return app.Exec();
}
