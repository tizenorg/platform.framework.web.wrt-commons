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
 * @file        test_scoped_array.cpp
 * @author      Przemyslaw Dobrowolski (p.dobrowolsk@samsung.com)
 * @version     1.0
 * @brief       This file is the implementation file of test scoped array
 */
#include <dpl/test/test_runner.h>
#include <dpl/scoped_array.h>

RUNNER_TEST_GROUP_INIT(DPL)

/*
Name: ScopedArray_Zero
Description: tests emptiness of empty scoped array
Expected: array should be empty
*/
RUNNER_TEST(ScopedArray_Zero)
{
    DPL::ScopedArray<char> array;

    RUNNER_ASSERT(!array);
    RUNNER_ASSERT(!!!array);
}

/*
Name: ScopedArray_NonZero
Description: tests emptiness of not empty scoped array
Expected: array should be not empty
*/
RUNNER_TEST(ScopedArray_NonZero)
{
    DPL::ScopedArray<char> array(new char[7]);

    RUNNER_ASSERT(array);
    RUNNER_ASSERT(!!array);
}

/*
Name: ScopedArray_Reset
Description: tests reseting content of array
Expected: array should be empty after reset
*/
RUNNER_TEST(ScopedArray_Reset)
{
    DPL::ScopedArray<char> array(new char[7]);
    array.Reset();

    RUNNER_ASSERT(!array);

    array.Reset(new char);
    RUNNER_ASSERT(array);
}

/*
Name: ScopedArray_ArrayOperator
Description: tests accessing elements of array
Expected: returned values should be equal to those which were set
*/
RUNNER_TEST(ScopedArray_ArrayOperator)
{
    DPL::ScopedArray<char> array(new char[7]);

    array[1] = array[2] = 3;

    RUNNER_ASSERT(array[1] == 3);
    RUNNER_ASSERT(array[2] == 3);
}
