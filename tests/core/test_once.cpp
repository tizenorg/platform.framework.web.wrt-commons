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
 * @file        test_once.cpp
 * @author      Przemyslaw Dobrowolski (p.dobrowolsk@samsung.com)
 * @version     1.0
 * @brief       This file is the implementation file of once tests
 */
#include <dpl/test/test_runner.h>
#include <dpl/once.h>
#include <dpl/waitable_event.h>
#include <dpl/waitable_handle.h>
#include <dpl/thread.h>
#include <dpl/shared_ptr.h>
#include <dpl/atomic.h>

RUNNER_TEST_GROUP_INIT(DPL)

namespace // anonymous
{
gint g_counter;

void Delegate()
{
    ++g_counter;
}
} // namespace anonymous

RUNNER_TEST(Once_DoubleCall)
{
    g_counter = 0;

    DPL::Once once;

    once.Call(&Delegate);
    once.Call(&Delegate);

    RUNNER_ASSERT_MSG(g_counter == 1, "Counter value is: " << g_counter);
}

class MyThread
    : public DPL::Thread
{
protected:
    virtual int ThreadEntry()
    {
        DPL::WaitForSingleHandle(m_event->GetHandle());
        m_once->Call(DPL::Once::Delegate(this, &MyThread::Call));
        return 0;
    }

    void Call()
    {
        ++*m_atom;
    }

public:
    MyThread(DPL::WaitableEvent *event, DPL::Once *once, DPL::Atomic *atom)
        : m_event(event),  m_once(once), m_atom(atom)
    {
    }

private:
    DPL::WaitableEvent *m_event;
    DPL::Once *m_once;
    DPL::Atomic *m_atom;
};

RUNNER_TEST(Once_MultiThreadCall)
{
    const size_t NUM_THREADS = 20;
    typedef DPL::SharedPtr<MyThread> ThreadPtr;

    ThreadPtr threads[NUM_THREADS];
    DPL::WaitableEvent event;
    DPL::Once once;
    DPL::Atomic atom;

    for (size_t i = 0; i< NUM_THREADS; ++i)
    {
        (threads[i] = ThreadPtr(new MyThread(&event, &once, &atom)))->Run();
    }

    event.Signal();

    for (size_t i = 0; i< NUM_THREADS; ++i)
        threads[i]->Quit();

    RUNNER_ASSERT_MSG(atom == 1, "Atom value is: " << atom);
}
