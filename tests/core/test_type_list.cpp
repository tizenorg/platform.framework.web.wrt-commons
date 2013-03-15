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
/**
 * @file       test_type_list.cpp
 * @author     Krzysztof Jackiewicz (k.jackiewicz@samsung.com)
 * @version    0.1
 * @brief
 */

#include <dpl/test/test_runner.h>
#include <dpl/type_list.h>

RUNNER_TEST_GROUP_INIT(DPL)

/*
Name: TypeList_TypeCount
Description: tests size of typelist idiom
Expected: size should match
*/
RUNNER_TEST(TypeList_TypeCount)
{
    typedef DPL::TypeListDecl<int, char, int[64]>::Type TestTypeList1;
    typedef DPL::TypeListDecl<int>::Type TestTypeList2;
    typedef DPL::TypeListDecl<>::Type TestTypeList3;
    typedef DPL::TypeList<int, TestTypeList1> TestTypeList4;

    RUNNER_ASSERT(TestTypeList1::Size == 3);
    RUNNER_ASSERT(TestTypeList2::Size == 1);
    RUNNER_ASSERT(TestTypeList3::Size == 0);
    RUNNER_ASSERT(TestTypeList4::Size == 4);

    RUNNER_ASSERT(TestTypeList4::Tail::Tail::Size == 2);
}
