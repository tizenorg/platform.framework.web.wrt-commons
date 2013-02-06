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
 * @file        test_thread.cpp
 * @author      Przemyslaw Dobrowolski (p.dobrowolsk@samsung.com)
 * @version     1.0
 * @brief       This file is the implementation file of thread tests
 */
#include <dpl/test/test_runner.h>
#include <dpl/thread.h>
#include <dpl/log/log.h>

RUNNER_TEST_GROUP_INIT(DPL)

bool g_wasFooDeleted;

class Foo
{
  public:
    int id;
    Foo(int i = 0) : id(i)
    {
        LogInfo("Foo: ctor: " << id);
    }

    ~Foo()
    {
        LogInfo("Foo: dtor: " << id);
        g_wasFooDeleted = true;
    }

    void Bar()
    {
        LogInfo("Foo: bar");
    }
};

typedef DPL::ThreadLocalVariable<Foo> TlsFoo;
TlsFoo g_foo;

class FooThread :
    public DPL::Thread
{
  protected:
    virtual int ThreadEntry()
    {
        LogInfo("In thread");

        RUNNER_ASSERT(!g_foo);
        RUNNER_ASSERT(g_foo.IsNull());

        g_foo = Foo();
        g_foo->Bar();

        return 0;
    }
};

RUNNER_TEST(Thread_ThreadLocalVariable_FooDeletion)
{
    static TlsFoo staticFooForMain;
    staticFooForMain = Foo(1);

    TlsFoo fooForMain;
    fooForMain = Foo(2);

    RUNNER_ASSERT(!g_foo);
    RUNNER_ASSERT(g_foo.IsNull());

    g_wasFooDeleted = false;

    FooThread thread1;
    thread1.Run();
    thread1.Quit();

    RUNNER_ASSERT(!g_foo);
    RUNNER_ASSERT(g_foo.IsNull());

    RUNNER_ASSERT(g_wasFooDeleted == true);

    FooThread thread2;
    thread2.Run();
    thread2.Quit();

    RUNNER_ASSERT(!g_foo);
    RUNNER_ASSERT(g_foo.IsNull());
}
