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
 * @file        test_shared_ptr.cpp
 * @author      Przemyslaw Dobrowolski (p.dobrowolsk@samsung.com)
 * @version     1.0
 * @brief       This file is the implementation file of test shared ptr
 */
#include <dpl/test/test_runner.h>
#include <dpl/shared_ptr.h>

RUNNER_TEST_GROUP_INIT(DPL)

RUNNER_TEST(SharedPtr_Zero)
{
    DPL::SharedPtr<char> ptr;

    RUNNER_ASSERT(!ptr);
    RUNNER_ASSERT(!!!ptr);
    RUNNER_ASSERT(ptr == DPL::SharedPtr<char>());
}

RUNNER_TEST(SharedPtr_NonZero)
{
    DPL::SharedPtr<char> ptr(new char(7));

    RUNNER_ASSERT(ptr);
    RUNNER_ASSERT(!!ptr);
    RUNNER_ASSERT(ptr != DPL::SharedPtr<char>());
}

RUNNER_TEST(SharedPtr_Copy)
{
    DPL::SharedPtr<char> ptr1(new char(7));
    DPL::SharedPtr<char> ptr2(new char(7));

    RUNNER_ASSERT(ptr1 != ptr2);

    ptr2 = ptr1;

    RUNNER_ASSERT(ptr1 == ptr2);
}

RUNNER_TEST(SharedPtr_Reset)
{
    DPL::SharedPtr<char> ptr(new char(7));
    ptr.Reset();

    RUNNER_ASSERT(!ptr);

    ptr.Reset(new char);
    RUNNER_ASSERT(ptr);
}

RUNNER_TEST(SharedPtr_RefCounting)
{
    DPL::SharedPtr<char> ptr1(new char(7));
    DPL::SharedPtr<char> ptr2;

    ptr2 = ptr1;

    RUNNER_ASSERT(ptr1 == ptr2);
    RUNNER_ASSERT(ptr1.GetUseCount() == ptr2.GetUseCount());
    RUNNER_ASSERT(ptr1.GetUseCount() == 2);
}

RUNNER_TEST(SharedPtr_Operators)
{
    DPL::SharedPtr<char> ptr(new char(7));

    RUNNER_ASSERT(*ptr == *ptr.Get());
}
