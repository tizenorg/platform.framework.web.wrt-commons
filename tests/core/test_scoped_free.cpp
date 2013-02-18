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
 * @file        test_scoped_free.cpp
 * @author      Przemyslaw Dobrowolski (p.dobrowolsk@samsung.com)
 * @version     1.0
 * @brief       This file is the implementation file of test scoped free
 */
#include <dpl/test/test_runner.h>
#include <dpl/scoped_free.h>
#include <malloc.h>

RUNNER_TEST_GROUP_INIT(DPL)

/*
Name: ScopedFree_Zero
Description: Checks emptiness of not set scoped free
Expected: resource should be freed
*/
RUNNER_TEST(ScopedFree_Zero)
{
    DPL::ScopedFree<void> free;

    RUNNER_ASSERT(!free);
    RUNNER_ASSERT(!!!free);
}

/*
Name: ScopedFree_NonZero
Description: Checks emptiness of set scoped free
Expected: resource should not be reported as freed
*/
RUNNER_TEST(ScopedFree_NonZero)
{
    DPL::ScopedFree<void> free(malloc(7));

    RUNNER_ASSERT(free);
    RUNNER_ASSERT(!!free);
}

/*
Name: ScopedFree_Reset
Description: Checks reseting scoped free
Expected: resource should be freed after reset
*/
RUNNER_TEST(ScopedFree_Reset)
{
    DPL::ScopedFree<void> free(malloc(7));
    free.Reset();

    RUNNER_ASSERT(!free);

    free.Reset(malloc(8));
    RUNNER_ASSERT(free);
}