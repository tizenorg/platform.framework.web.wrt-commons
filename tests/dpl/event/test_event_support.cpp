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
 * @file        test_event_support.cpp
 * @author      Piotr Marcinkiewicz (p.marcinkiew@samsung.com)
 * @author      Pawel Sikorski (p.sikorski@samsung.com)
 * @version     1.0
 * @brief       This file contains test for event support
 */

#include <dpl/test/test_runner.h>
#include <dpl/generic_event.h>
#include <dpl/event/event_listener.h>
#include <dpl/event/event_support.h>
#include <dpl/application.h>
#include <dpl/event/controller.h>
#include <dpl/fast_delegate.h>
#include <dpl/log/log.h>

DECLARE_GENERIC_EVENT_0(TestEvent)

class TestListener : public DPL::Event::EventListener<TestEvent>
{
  public:
    explicit TestListener() : m_dummyVar(0) { }
    void OnEventReceived(const TestEvent &)
    {
        m_dummyVar = 1;
    }
    int GetDummyVar() const
    {
        return m_dummyVar;
    }
    void ZeroDummyVar()
    {
        m_dummyVar = 0;
    }

  private:
    int m_dummyVar;
};

class TestEventSupport :
    public DPL::Event::EventSupport<TestEvent>
{
  public:
    void TestEmitEvent()
    {
        EmitEvent(TestEvent());
    }
};

DECLARE_GENERIC_EVENT_0(QuitEvent)

class QuitController :
    public DPL::Event::Controller<DPL::TypeListDecl<QuitEvent>::Type>,
    public DPL::ApplicationExt
{
  public:
    QuitController() : DPL::ApplicationExt(1, NULL, "test-app")
    {
        Touch();
    }

  protected:
    virtual void OnEventReceived(const QuitEvent &)
    {
        Quit();
    }
};

RUNNER_TEST(EventSupport_DestroyBeforeProcessing)
{
    QuitController quitter;
    quitter.PostTimedEvent(QuitEvent(), 1.0);

    TestListener eventListener;
    {
        TestEventSupport eventSupport;
        eventSupport.AddListener(&eventListener);
        eventSupport.TestEmitEvent();
        eventSupport.RemoveListener(&eventListener);
    }
    eventListener.ZeroDummyVar();

    quitter.Exec();
    RUNNER_ASSERT(eventListener.GetDummyVar() == 0);
}

int g_delegateTest;

void OnDelegateTest(const int &k);

void OnDelegateTest(const int &k)
{
    LogInfo("Got delegate call");
    g_delegateTest = k;
}

class DelegateTestSupport :
    public DPL::Event::EventSupport<int>
{
  public:
    void Test()
    {
        EmitEvent(7);
    }
};

RUNNER_TEST(EventSupport_BindDelegate)
{
    g_delegateTest = 0;

    DelegateTestSupport support;
    support.AddListener(&OnDelegateTest);

    QuitController quitter;
    quitter.PostTimedEvent(QuitEvent(), 1.0);

    support.Test();

    quitter.Exec();

    support.RemoveListener(&OnDelegateTest);

    RUNNER_ASSERT(g_delegateTest == 7);
}
