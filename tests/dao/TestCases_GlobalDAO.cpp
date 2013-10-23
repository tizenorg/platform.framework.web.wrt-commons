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
 * @file   TestCases_GlobalDAO.cpp
 * @author  Pawel Sikorski (p.sikorski@samsung.com)
 * @version 1.0
 * @brief   This file contains tests for global dao class.
 */

#include <list>
#include <vector>
#include <set>
#include <dpl/test/test_runner.h>
#include <dpl/foreach.h>
#include <dpl/exception.h>
#include <dpl/string.h>
#include <dpl/wrt-dao-rw/global_dao.h>
#include <dpl/wrt-dao-ro/wrt_db_types.h>

using namespace WrtDB;

namespace {
const DPL::String widgetPackage5(L"widgetpackage5");
} // namespace

RUNNER_TEST_GROUP_INIT(DAO)

/*
 * Name: roaming_data_usage
 * Description: tests if RoamingDataUsage is correctly set and get
 * Expected: received developer shoudl match the one was set
 */
RUNNER_TEST(roaming_data_usage)
{
    GlobalDAO::NetworkAccessMode original =
        GlobalDAO::GetRoamingDataUsage();

    GlobalDAO::SetRoamingDataUsage(GlobalDAO::CONNECT_AUTOMATICALLY);
    RUNNER_ASSERT_MSG(GlobalDAO::CONNECT_AUTOMATICALLY ==
                      GlobalDAO::GetRoamingDataUsage(), "Value not updated");

    GlobalDAO::SetRoamingDataUsage(GlobalDAO::ALWAYS_ASK);
    RUNNER_ASSERT_MSG(GlobalDAO::ALWAYS_ASK ==
                      GlobalDAO::GetRoamingDataUsage(), "Value not updated");

    GlobalDAO::SetRoamingDataUsage(GlobalDAO::NEVER_CONNECT);
    RUNNER_ASSERT_MSG(GlobalDAO::NEVER_CONNECT ==
                      GlobalDAO::GetRoamingDataUsage(), "Value not updated");

    GlobalDAO::SetRoamingDataUsage(original);
    RUNNER_ASSERT_MSG(original == GlobalDAO::GetRoamingDataUsage(),
                      "Value not updated");
}
