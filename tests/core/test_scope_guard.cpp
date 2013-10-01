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
 * @file        test_scope_guard.cpp
 * @author      Zbigniew Kostrzewa (z.kostrzewa@samsung.com)
 * @version     1.0
 * @brief       This file is the implementation file of test scope guard
 */
#include <dpl/test/test_runner.h>
#include <dpl/scope_guard.h>

namespace {
bool g_guardCalled = false;

void regularFunction()
{
    g_guardCalled = true;
}

struct Functor
{
    explicit Functor(bool& guardCalled)
        : m_guardCalled(guardCalled)
    {}

    void operator()()
    {
        m_guardCalled = true;
    }

    bool& m_guardCalled;
};
}

RUNNER_TEST_GROUP_INIT(DPL)

/*
Name: ScopeGuard_MakeScopeGuard_RegularFunction
Description: tests scope guard, created explicitly, with regular function
Expected: guard should be called
*/
RUNNER_TEST(ScopeGuard_MakeScopeGuard_RegularFunction)
{
    g_guardCalled = false;
    {
        auto guard = DPL::MakeScopeGuard(regularFunction);
    }
    RUNNER_ASSERT(g_guardCalled);
}

/*
Name: ScopeGuard_MakeScopeGuard_Functor
Description: tests scope guard, created explicitly, with a functor
Expected: guard should be called
*/
RUNNER_TEST(ScopeGuard_MakeScopeGuard_Functor)
{
    g_guardCalled = false;
    {
        auto guard = DPL::MakeScopeGuard(Functor(g_guardCalled));
    }
    RUNNER_ASSERT(g_guardCalled);
}

/*
Name: ScopeGuard_MakeScopeGuard_Lambda
Description: tests scope guard, created explicitly, with a lambda
Expected: guard should be called
*/
RUNNER_TEST(ScopeGuard_MakeScopeGuard_Lambda)
{
    g_guardCalled = false;
    {
        auto guard = DPL::MakeScopeGuard(
          [&g_guardCalled](){ g_guardCalled = true; });
    }
    RUNNER_ASSERT(g_guardCalled);
}

/*
Name: ScopeGuard_Macro
Description: tests scope guard, created implicitly, with a lambda
Expected: guard should be called
*/
RUNNER_TEST(ScopeGuard_Macro)
{
    g_guardCalled = false;
    {
        DPL_SCOPE_EXIT(&g_guardCalled)
        {
            g_guardCalled = true;
        };
    }
    RUNNER_ASSERT(g_guardCalled);
}

/*
Name: ScopeGuard_Release
Description: tests scope guard releasing API
Expected: guard should not be called
*/
RUNNER_TEST(ScopeGuard_Release)
{
    g_guardCalled = false;
    {
        auto guard = DPL::MakeScopeGuard(
                [&g_guardCalled](){ g_guardCalled = true; });
        guard.Release();
    }
    RUNNER_ASSERT(!g_guardCalled);
}
