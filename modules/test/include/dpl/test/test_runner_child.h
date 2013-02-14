/*
 * Copyright (c) 2013 Samsung Electronics Co., Ltd All Rights Reserved
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
 * @file        test_runner_child.h
 * @author      Bartlomiej Grzelewski (b.grzelewski@samsung.com)
 * @version     1.0
 * @brief       This file is the header file of test runner
 */
#ifndef DPL_TEST_RUNNER_CHILD_H
#define DPL_TEST_RUNNER_CHILD_H

#include <dpl/test/test_runner.h>

namespace DPL {
namespace Test {
void RunChildProc(TestRunner::TestCase procChild);
} // namespace Test
} // namespace DPL

#define RUNNER_CHILD_TEST(Proc)                                                      \
    void Proc();                                                                     \
    void Proc##Child();                                                              \
    static int Static##Proc##Init()                                                  \
    {                                                                                \
        DPL::Test::TestRunnerSingleton::Instance().RegisterTest(#Proc, &Proc);       \
        return 0;                                                                    \
    }                                                                                \
    const int DPL_UNUSED Static##Proc##InitVar = Static##Proc##Init();               \
    void Proc(){                                                                     \
        DPL::Test::RunChildProc(&Proc##Child);                                       \
    }                                                                                \
    void Proc##Child()

#endif // DPL_TEST_RUNNER_CHILD_H
