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
/**
 * @file       test_static_block.cpp
 * @author     Tomasz Iwanek (t.iwanek@samsung.com)
 * @version    0.1
 * @brief
 */

#include <dpl/test/test_runner.h>
#include <dpl/static_block.h>

RUNNER_TEST_GROUP_INIT(DPL)

namespace {
bool ok_namespace = false;
bool ok_class = false;

STATIC_BLOCK
{
    ok_namespace = true;
}

struct A
{
    static void init()
    {
        ok_class = true;
    }
};
STATIC_BLOCK_CLASS( A, init );
}

/*
Name: StaticBlockInitCheck
Description: checks if static blocks were run
Expected: variables should be set
*/
RUNNER_TEST(StaticBlockInitCheck)
{
    RUNNER_ASSERT(ok_namespace);
    RUNNER_ASSERT(ok_class);
}
