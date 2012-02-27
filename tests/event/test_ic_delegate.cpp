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
 * @file        test_ic_delegate.cpp
 * @author      Pawel Sikorski (p.sikorski@samsung.com)
 * @author      Lukasz Wrzosek (l.wrzosek@samsung.com)
 * @version     1.0
 * @brief       This file is the implementation file of fast delegate tests.
 */
#include <dpl/test/test_runner.h>
#include <dpl/application.h>
#include <dpl/event/controller.h>
#include <dpl/log/log.h>
#include <dpl/fast_delegate.h>
#include <dpl/event/inter_context_delegate.h>
#include <dpl/thread.h>
#include <dpl/shared_ptr.h>
#include <dpl/waitable_event.h>
#include <dpl/assert.h>
#include <dpl/mutex.h>
#include <dpl/type_list.h>

RUNNER_TEST_GROUP_INIT(DPL)

const int IntVal = 123;
const std::string StringVal = "someString";

typedef DPL::Event::ICDelegate<> GetNothingDlpType;
typedef DPL::Event::ICDelegate<int> GetIntDlgType;
typedef DPL::Event::ICDelegate<int, std::string> GetIntAndStringDlgType;
DECLARE_GENERIC_EVENT_1(GetNothingEvent, GetNothingDlpType)
DECLARE_GENERIC_EVENT_1(GetIntEvent, GetIntDlgType)
DECLARE_GENERIC_EVENT_1(GetIntAndStringEvent, GetIntAndStringDlgType)

class ICTestController
: public DPL::Event::Controller<DPL::TypeListDecl<GetNothingEvent,
                                                  GetIntEvent,
                                                  GetIntAndStringEvent>::Type>
{
  public:
    ICTestController() { }

  protected:
    virtual void OnEventReceived(const GetNothingEvent& event)
    {
        event.GetArg0()(); //calling intercontext delegate
    }
    virtual void OnEventReceived(const GetIntEvent& event)
    {
        event.GetArg0()(IntVal); //calling intercontext delegate
    }

    virtual void OnEventReceived(const GetIntAndStringEvent& event)
    {
        event.GetArg0()(IntVal, StringVal); //calling intercontext delegate
    }
};

struct TestResult
{
    TestResult() :
        m_correctThread0(false),
        m_correctThread1(false),
        m_correctThread2(false),
        m_int(-1),
        m_int2(-1),
        m_string("")
    {
    }

    void TestEventsPassed()
    {
        RUNNER_ASSERT(m_correctThread0);
        RUNNER_ASSERT(m_correctThread1);
        RUNNER_ASSERT(m_int == IntVal);
        RUNNER_ASSERT(m_correctThread2);
        RUNNER_ASSERT(m_int2 == IntVal);
        RUNNER_ASSERT(m_string == StringVal);
    }

    void TestEventsDidNotPass()
    {
        RUNNER_ASSERT(!m_correctThread0);
        RUNNER_ASSERT(!m_correctThread1);
        RUNNER_ASSERT(m_int == -1);
        RUNNER_ASSERT(!m_correctThread2);
        RUNNER_ASSERT(m_int2 == -1);
        RUNNER_ASSERT(m_string == "");
    }

    bool m_correctThread0;
    bool m_correctThread1;
    bool m_correctThread2;
    int m_int;
    int m_int2;
    std::string m_string;
};

class TestContextFreeClass :
    protected DPL::Thread,
    public DPL::Event::ICDelegateSupport<TestContextFreeClass>
{
  public:
    TestContextFreeClass(ICTestController* controller, TestResult* result) :
        Thread(),
        m_testResult(result),
        m_controller(controller)
    {
        LogDebug("Context thread id = " << this);
    }

    void Run()
    {
        LogDebug("Running Context Free thread");
        Thread::Run();
    }

    void Quit()
    {
        LogDebug("Exiting Context Free thread");
        Thread::Quit();
    }


    void Wait()
    {
        LogDebug("Waiting for thread");
        DPL::WaitForSingleHandle(m_waitable.GetHandle());
    }

  protected:
    void OnNothing()
    {
        LogDebug("Received nothing in thread = " << GetCurrentThread());
        m_testResult->m_correctThread0 = (GetCurrentThread() == this);
    }

    void OnIntReceive(int val)
    {
        LogDebug("Received int in thread = " << GetCurrentThread());
        m_testResult->m_correctThread1 = (GetCurrentThread() == this);
        m_testResult->m_int = val;
    }

    void OnIntAndStringReceive(int val, std::string stringval)
    {
        LogDebug("Received int and string in thread = " << GetCurrentThread());
        m_testResult->m_correctThread2 = (GetCurrentThread() == this);
        m_testResult->m_int2 = val;
        m_testResult->m_string = stringval;
        m_waitable.Signal();
    }

    virtual int ThreadEntry()
    {
        GetNothingEvent getNothingEvent(
            makeICDelegate(
                &TestContextFreeClass::OnNothing));
        m_controller->DPL::Event::ControllerEventHandler<GetNothingEvent>::PostEvent(
            getNothingEvent);

        GetIntEvent getIntEvent(
            makeICDelegate(
                &TestContextFreeClass::OnIntReceive));
        m_controller->DPL::Event::ControllerEventHandler<GetIntEvent>::PostEvent(
            getIntEvent);

        GetIntAndStringEvent getIntAndStringEvent(
            makeICDelegate(
                &TestContextFreeClass::OnIntAndStringReceive));
        m_controller->DPL::Event::ControllerEventHandler<GetIntAndStringEvent>::PostEvent(
            getIntAndStringEvent);

        return Thread::ThreadEntry();
    }

  private:
    TestResult* m_testResult;
    DPL::WaitableEvent m_waitable;
    ICTestController* m_controller;
};

RUNNER_TEST(ICDelegate_0)
{
    DPL::Thread thread;
    thread.Run();
    LogDebug("Controller thread id = " << &thread);

    ICTestController testController;
    testController.Touch();
    testController.SwitchToThread(&thread);

    TestResult result;
    TestContextFreeClass* contextFree =
        new TestContextFreeClass(&testController, &result);
    result.TestEventsDidNotPass();

    thread.Run();
    contextFree->Run();
    contextFree->Wait();
    contextFree->Quit();
    thread.Quit();

    delete contextFree;

    result.TestEventsPassed();
}

RUNNER_TEST(ICDelegate_1)
{
    DPL::Thread thread;
    LogDebug("Controller thread id = " << &thread);

    ICTestController testController;
    testController.Touch();
    testController.SwitchToThread(&thread);

    TestResult result;
    TestContextFreeClass* contextFree =
        new TestContextFreeClass(&testController, &result);
    result.TestEventsDidNotPass();

    contextFree->Run();
    contextFree->Quit();
    delete contextFree; //deleting Delegates before actual Events are worked out
    thread.Run();
    thread.Quit();

    result.TestEventsDidNotPass();
}

class TestContextFree;
class TestRunnerInThread;

namespace
{
const int ControllersPerThread = 40;
const int ContextFreePerThread = 180;
const int TestsPerController = 110;
const int TestThreads = 23;
const int TestsPerThread = 100;
const int NumberOfEvents = 230;

typedef DPL::SharedPtr<ICTestController> ICTestControllerPtr;
typedef DPL::SharedPtr<TestContextFree> TestContextFreePtr;
typedef DPL::SharedPtr<TestRunnerInThread> TestRunnerInThreadPtr;
typedef DPL::SharedPtr<DPL::Thread> ThreadPtr;

DPL::Mutex mutex;
std::list<TestContextFreePtr> frees;
std::list<ICTestControllerPtr> ctrls;
std::list<TestRunnerInThreadPtr> frees_threads;
std::list<ThreadPtr> ctrls_threads;

}

class TestContextFree : public DPL::Event::ICDelegateSupport<TestContextFree>
{
  public:
    TestContextFree(ICTestController* controller,
                    int eventsCount) :
        m_controller(controller),
        m_eventsCount(eventsCount)
    {
    }

    void Wait()
    {
        LogDebug("Waiting for thread");
        DPL::WaitForSingleHandle(m_waitable.GetHandle());
    }


    void OnNothing()
    {
        LogDebug("Got");
        m_eventsCount--;
        if (m_eventsCount > 0) {
            LogDebug("posting next event");
            GetIntAndStringEvent getIntAndStringEvent(
                makeICDelegate(
                    &TestContextFree::OnIntAndStringReceive));
            LogDebug("posting next event ...");
            m_controller->DPL::Event::ControllerEventHandler<GetIntAndStringEvent>::PostEvent(
                getIntAndStringEvent);
            LogDebug("posting next event done");
        } else {
            LogDebug("test finished");
            m_waitable.Signal();
        }
    }

    void OnIntReceive(int)
    {
        LogDebug("Got");
        m_eventsCount--;
        if (m_eventsCount > 0) {
            LogDebug("posting next event");
            GetNothingEvent getNothingEvent(
                makeICDelegate(
                    &TestContextFree::OnNothing));
            LogDebug("posting next event ...");
            m_controller->DPL::Event::ControllerEventHandler<GetNothingEvent>::PostEvent(
                getNothingEvent);
            LogDebug("posting next event done");
        } else {
            LogDebug("test finished");
            m_waitable.Signal();
        }
    }

    void OnIntAndStringReceive(int, std::string)
    {
        LogDebug("Got");
        m_eventsCount--;
        if (m_eventsCount > 0) {
            LogDebug("posting next event");

            GetIntEvent getIntEvent(
                makeICDelegate(
                    &TestContextFree::OnIntReceive));
            LogDebug("posting next event ...");
            m_controller->DPL::Event::ControllerEventHandler<GetIntEvent>::PostEvent(
                getIntEvent);
            LogDebug("posting next event done");
        } else {
            LogDebug("test finished");
            m_waitable.Signal();
        }
    }

    void StartTestOnNothing()
    {
        GetNothingEvent getNothingEvent(
            makeICDelegate(
                &TestContextFree::OnNothing));
        m_controller->DPL::Event::ControllerEventHandler<GetNothingEvent>::PostEvent(
            getNothingEvent);
    }

    void StartTestOnInt()
    {
        GetIntEvent getIntEvent(
            makeICDelegate(
                &TestContextFree::OnIntReceive));
        m_controller->DPL::Event::ControllerEventHandler<GetIntEvent>::PostEvent(
            getIntEvent);
    }

    void StartTestOnIntAndString()
    {
        GetIntAndStringEvent getIntAndStringEvent(
            makeICDelegate(
                &TestContextFree::OnIntAndStringReceive));
        m_controller->DPL::Event::ControllerEventHandler<GetIntAndStringEvent>::PostEvent(
            getIntAndStringEvent);
    }

    bool CheckTest()
    {
        LogDebug("Checking test result");
        return m_eventsCount == 0;
    }

  private:
    ICTestController* m_controller;
    int m_eventsCount;
    DPL::WaitableEvent m_waitable;
};

class TestRunnerInThread : public DPL::Thread
{
  public:
    TestRunnerInThread(int events, int tests) :
        m_eventsCount(events),
        m_tests(tests) {}

    void WaitForInit()
    {
        LogDebug("Waiting for thread");
        DPL::WaitForSingleHandle(m_init.GetHandle());
    }

  protected:
    virtual int ThreadEntry()
    {
        LogDebug("Thread starts");
        {
            DPL::Mutex::ScopedLock lock(&mutex);
            for (int i = 0; i < m_tests; ++i)
            {
                if (i % TestsPerController == 0) {
                    if (ctrls.size() % ControllersPerThread == 0) {
                        ThreadPtr thread(new DPL::Thread());
                        thread->Run();
                        ctrls_threads.push_back(thread);
                    }
                    ICTestControllerPtr ptr(new ICTestController());
                    ptr->Touch();
                    ptr->SwitchToThread(ctrls_threads.back().Get());
                    ctrls.push_back(ptr);

                    TestContextFreePtr t(new TestContextFree(ctrls.back().Get(),
                                                             m_eventsCount));
                    t->StartTestOnNothing();
                    LogDebug("");
                    frees.push_back(t);
                }
            }
        }
        m_init.Signal();
        LogDebug("Thread starts loop");
        return DPL::Thread::ThreadEntry();
    }

  private:
    DPL::WaitableEvent m_init;
    int m_eventsCount;
    int m_tests;
};

RUNNER_TEST(ICDelegate_2)
{
    LogDebug("Creating test threads");
    for (int i = 0; i < TestThreads; ++i)
    {
        TestRunnerInThreadPtr ptr(
            new TestRunnerInThread(NumberOfEvents, TestsPerThread));
        frees_threads.push_back(ptr);
        frees_threads.back()->Run();
    }

    FOREACH(it, frees_threads) {
        (*it)->WaitForInit();
    }
    LogDebug("Creating test threads done");

    FOREACH(it, frees) {
        LogDebug("...");
        (*it)->Wait();
    }

    FOREACH(it, frees) {
        RUNNER_ASSERT((*it)->CheckTest());
    }

    frees.clear();

    FOREACH(it, frees_threads) {
        (*it)->Quit();
    }

    frees_threads.clear();

    FOREACH(it, ctrls) {
        (*it)->SwitchToThread(NULL);
    }

    FOREACH(it, ctrls_threads) {
        (*it)->Quit();
    }

    ctrls.clear();
    ctrls_threads.clear();
}

namespace ReuseCheck {
const int ReuseCount = 5;
typedef DPL::Event::ICDelegate<> GetNothingDlpType;
DECLARE_GENERIC_EVENT_1(ReuseCountEvent, GetNothingDlpType)

class ICReuseTestController
: public DPL::Event::Controller<DPL::TypeListDecl<ReuseCountEvent>::Type>
{
  public:
    ICReuseTestController() { m_reuseCount = 0; }

  protected:
    virtual void OnEventReceived(const ReuseCountEvent& event)
    {
        event.GetArg0()(); //calling intercontext delegate
        if(++m_reuseCount < ReuseCount){
            LogInfo("[Send] Reuse: " << m_reuseCount);
            DPL::Event::ControllerEventHandler<ReuseCountEvent>::PostEvent(event);
        }
    }

    int m_reuseCount;
};

class ReuseTestContextFreeClass :
    protected DPL::Thread,
    public DPL::Event::ICDelegateSupport<ReuseTestContextFreeClass>
{
  public:
    ReuseTestContextFreeClass(ICReuseTestController* controller) :
        Thread(),
        m_controller(controller),
        m_reuseCount(0)
    { }

    void Run() { Thread::Run(); }
    void Quit() { Thread::Quit(); }
    void Wait() { DPL::WaitForSingleHandle(m_waitable.GetHandle()); }

  protected:
    void OnReuseReceive()
    {
        LogDebug("[Received] : " << ++m_reuseCount);
        if(m_reuseCount == ReuseCount)
            m_waitable.Signal();
    }

    virtual int ThreadEntry()
    {
        ReuseCountEvent reuseEvent(
            makeICDelegate(
                &ReuseTestContextFreeClass::OnReuseReceive,
                DPL::Event::ICD::Reuse::Yes));
        m_controller->DPL::Event::ControllerEventHandler<ReuseCountEvent>::PostEvent(
            reuseEvent);

        return Thread::ThreadEntry();
    }

  private:
    DPL::WaitableEvent m_waitable;
    ICReuseTestController* m_controller;
    int m_reuseCount;
};

RUNNER_TEST(ICDelegate_3)
{
    DPL::Thread thread;
    thread.Run();
    LogDebug("Controller thread id = " << &thread);

    ICReuseTestController testController;
    testController.Touch();
    testController.SwitchToThread(&thread);

    ReuseTestContextFreeClass* contextFree =
        new ReuseTestContextFreeClass(&testController);

    thread.Run();
    contextFree->Run();
    contextFree->Wait();
    contextFree->Quit();
    thread.Quit();

    delete contextFree;

    RUNNER_ASSERT(true);
}
} //namespace ReuseCheck
