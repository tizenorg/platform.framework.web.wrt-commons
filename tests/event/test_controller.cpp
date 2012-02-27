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
 * @file        test_controller.cpp
 * @author      Przemyslaw Dobrowolski (p.dobrowolsk@samsung.com)
 * @version     1.0
 * @brief       This file is the implementation file of test controller
 */
#include <dpl/test/test_runner.h>
#include <dpl/event/controller.h>
#include <dpl/thread.h>
#include <dpl/generic_event.h>
#include <dpl/waitable_handle.h>
#include <dpl/waitable_event.h>
#include <dpl/type_list.h>
#include <dpl/application.h>
#include <dpl/atomic.h>
#include <dpl/scoped_ptr.h>
#include <dpl/shared_ptr.h>
#include <list>
#include <vector>
RUNNER_TEST_GROUP_INIT(DPL)

class IntController
    : public DPL::Event::Controller<DPL::TypeListDecl<int>::Type>
{
private:
    int m_value;

protected:
    virtual void OnEventReceived(const int &event)
    {
        m_value = event;
    }

public:
    IntController()
        : m_value(-1)
    {
    }

    int Value() const
    {
        return m_value;
    }
};

DECLARE_GENERIC_EVENT_1(DoneSignalEvent, DPL::WaitableEvent *)

class ThreadController
    : public DPL::Event::Controller<DPL::TypeListDecl<DoneSignalEvent>::Type>
{
private:
    DPL::Thread *m_value;

protected:
    virtual void OnEventReceived(const DoneSignalEvent &event)
    {
        m_value = DPL::Thread::GetCurrentThread();
        event.GetArg0()->Signal();
    }

public:
    ThreadController()
        : m_value(NULL)
    {
    }

    DPL::Thread *Value() const
    {
        return m_value;
    }
};

struct StrangeStruct
{
    int a;
    float b;
    double c;
};

class StrangeController
    : public DPL::Event::Controller<DPL::TypeListDecl<char, short, int, long,
                                               unsigned char, unsigned short, unsigned int, unsigned long,
                                               float, double, StrangeStruct>::Type>
{
protected:
    virtual void OnEventReceived(const char &event) { (void)event; }
    virtual void OnEventReceived(const short &event) { (void)event; }
    virtual void OnEventReceived(const int &event) { (void)event; }
    virtual void OnEventReceived(const long &event) { (void)event; }
    virtual void OnEventReceived(const unsigned char &event) { (void)event; }
    virtual void OnEventReceived(const unsigned short &event) { (void)event; }
    virtual void OnEventReceived(const unsigned int &event) { (void)event; }
    virtual void OnEventReceived(const unsigned long &event) { (void)event; }
    virtual void OnEventReceived(const float &event) { (void)event; }
    virtual void OnEventReceived(const double &event) { (void)event; }
    virtual void OnEventReceived(const StrangeStruct &event) { (void)event; }
};

RUNNER_TEST(Controller_InitSimple)
{
    IntController controller;
    controller.Touch();
    RUNNER_ASSERT(controller.Value() == -1);
}

RUNNER_TEST(Controller_InitStrange)
{
    StrangeController controller;
    controller.Touch();
}

RUNNER_TEST(Controller_PostEventToThread)
{
    ThreadController controller;
    controller.Touch();

    DPL::Thread thread;
    thread.Run();

    controller.SwitchToThread(&thread);

    DPL::WaitableEvent waitHandle;

    controller.PostEvent(DoneSignalEvent(&waitHandle));

    DPL::WaitForSingleHandle(waitHandle.GetHandle());

    controller.SwitchToThread(NULL);

    RUNNER_ASSERT(controller.Value() == &thread);
}

RUNNER_TEST(Controller_PostTimedEventToThread)
{
    ThreadController controller;
    controller.Touch();

    DPL::Thread thread;
    thread.Run();

    controller.SwitchToThread(&thread);

    DPL::WaitableEvent waitHandle;

    controller.PostTimedEvent(DoneSignalEvent(&waitHandle), 0.5);

    DPL::WaitForSingleHandle(waitHandle.GetHandle());

    controller.SwitchToThread(NULL);

    RUNNER_ASSERT(controller.Value() == &thread);
}

DECLARE_GENERIC_EVENT_2(TouchInThread, DPL::WaitableEvent *, DPL::Thread **)
DECLARE_GENERIC_EVENT_2(TouchedControllerSignal, DPL::WaitableEvent *, DPL::Thread **)

class TouchInThreadController
    : public DPL::Event::Controller<DPL::TypeListDecl<TouchInThread>::Type>,
      private DPL::Event::Controller<DPL::TypeListDecl<TouchedControllerSignal>::Type>
{
public:
    typedef DPL::Event::Controller<DPL::TypeListDecl<TouchInThread>::Type> PublicController;
    typedef DPL::Event::Controller<DPL::TypeListDecl<TouchedControllerSignal>::Type> PrivateController;

    virtual void OnEventReceived(const TouchInThread &event)
    {
        // Touch controller in thread
        PrivateController::Touch();

        // Post signal
        PrivateController::PostEvent(TouchedControllerSignal(event.GetArg0(), event.GetArg1()));
    }

    virtual void OnEventReceived(const TouchedControllerSignal &event)
    {
        // Return touched thread
        *event.GetArg1() = DPL::Thread::GetCurrentThread();

        // Signal waitable event
        event.GetArg0()->Signal();
    }
};

RUNNER_TEST(Controller_TouchInThread)
{
    TouchInThreadController controller;
    controller.PublicController::Touch();

    DPL::Thread thread;
    thread.Run();

    controller.PublicController::SwitchToThread(&thread);

    DPL::WaitableEvent waitHandle;
    DPL::Thread *touchedThread = NULL;

    controller.PublicController::PostEvent(TouchInThread(&waitHandle, &touchedThread));

    DPL::WaitForSingleHandle(waitHandle.GetHandle());

    controller.PublicController::SwitchToThread(NULL);

    RUNNER_ASSERT(touchedThread == &thread);
}

RUNNER_TEST(Controller_SynchronizedEvent)
{
    IntController controller;
    controller.Touch();

    DPL::Thread thread;
    thread.Run();

    controller.SwitchToThread(&thread);
    controller.PostSyncEvent(12345);
    controller.SwitchToThread(NULL);

    RUNNER_ASSERT(controller.Value() == 12345);
}

const int ControllersNumber = 5;
const int MaxEventsPerController = 1;
const int MaxEvents = ControllersNumber * MaxEventsPerController;
const int ControllersPerThread = 1;

class TestController; //Forward Declaration

typedef DPL::SharedPtr<TestController> ControllerPtr;
typedef DPL::SharedPtr<DPL::Thread> ThreadPtr;
typedef std::vector<ControllerPtr> ControllerList;
typedef std::list<ThreadPtr> ThreadList;

DECLARE_GENERIC_EVENT_0(QuitEvent)
class QuitController
    : public DPL::Event::Controller<DPL::TypeListDecl<QuitEvent>::Type>,
      public DPL::ApplicationExt
{
public:
    explicit QuitController( ) : DPL::ApplicationExt(1, NULL, "test-app") { Touch(); }
protected:
    virtual void OnEventReceived(const QuitEvent &) { Quit(); }
};

struct TestContext
{
    ControllerList controllers;
    ThreadList threads;
    QuitController quitter;
    DPL::Atomic g_ReceivedCounter;
    DPL::Atomic g_SentCounter;
};
typedef DPL::ScopedPtr<TestContext> TestContextPtr;
TestContextPtr testContextPtr;

DECLARE_GENERIC_EVENT_0(StartSendEvent)
DECLARE_GENERIC_EVENT_0(RandomEvent)
class TestController
    : public DPL::Event::Controller<DPL::TypeListDecl<RandomEvent, StartSendEvent>::Type>
{
public:
    explicit TestController() { Touch(); }
protected:
    virtual void OnEventReceived(const RandomEvent &)
    {
        ++testContextPtr->g_ReceivedCounter;
        if(testContextPtr->g_ReceivedCounter == MaxEvents)
        {
            testContextPtr->quitter.DPL::Event::ControllerEventHandler<QuitEvent>::PostEvent(QuitEvent());
            return;
        }
    }
    virtual void OnEventReceived(const StartSendEvent &)
    {
        for (int i=0 ; i<MaxEventsPerController ;++i)
        {
            if(testContextPtr->g_SentCounter > MaxEvents)
            {
                return;
            }
            ++testContextPtr->g_SentCounter;
            int id = rand() % static_cast<int>(testContextPtr->controllers.size());
            testContextPtr->controllers.at(id)->DPL::Event::ControllerEventHandler<RandomEvent>::PostEvent(RandomEvent());
        }
    }
};

RUNNER_TEST(Controllers_MultipleEvents)
{
    srand ( time(NULL) );

    testContextPtr.Reset(new TestContext());
    testContextPtr->controllers.reserve(ControllersNumber);

    for (int i = 0; i < ControllersNumber ; ++i)
    {
        if(testContextPtr->controllers.size() % ControllersPerThread ==0)
        {
            ThreadPtr thread = ThreadPtr(new DPL::Thread());
            testContextPtr->threads.push_back(thread);
            thread->Run();
        }

        ControllerPtr controller = ControllerPtr(new TestController());
        testContextPtr->controllers.push_back(controller);
        if(testContextPtr->controllers.size() % 2 == 0)
        {
            //This controller is being switched to thread (otherwise it is touched to main thread)
            ThreadPtr thread = testContextPtr->threads.back();
            controller->SwitchToThread(thread.Get());
        }
        controller->DPL::Event::ControllerEventHandler<StartSendEvent>::PostEvent(StartSendEvent());
    }
    testContextPtr->quitter.Exec();
    RUNNER_ASSERT(testContextPtr->g_SentCounter == testContextPtr->g_ReceivedCounter);
    testContextPtr.Reset();
}
