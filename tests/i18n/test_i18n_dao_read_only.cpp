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
 * @file        test_i18n_dao_read_only.cpp
 * @author      Zbigniew Kostrzewa (z.kostrzewa@samsung.com)
 * @version     1.0
 * @brief       This file is the implementation file of i18n dao tests
 */
#include <dpl/test/test_runner.h>
#include <dpl/log/log.h>
#include <wrt-commons/i18n-dao-ro/i18n_database.h>
#include <wrt-commons/i18n-dao-ro/i18n_dao_read_only.h>

RUNNER_TEST_GROUP_INIT(I18N)

/*
Name: I18nDAOReadOnly_IsValidSubTag_True
Description: Test valid IANA subtag presence
Expected: Subtag found
*/
RUNNER_TEST(I18nDAOReadOnly_IsValidSubTag_True)
{
    I18n::DB::Interface::attachDatabaseRO();
    bool result = I18n::DB::I18nDAOReadOnly::IsValidSubTag(L"aa", 0);
    I18n::DB::Interface::detachDatabase();
    RUNNER_ASSERT_MSG(result, "Subtag not found");
}

/*
Name: I18nDAOReadOnly_IsValidSubTag_False
Description: Test invalid IANA subtag presence
Expected: Subtag not found
*/
RUNNER_TEST(I18nDAOReadOnly_IsValidSubTag_False)
{
    I18n::DB::Interface::attachDatabaseRO();
    bool result = I18n::DB::I18nDAOReadOnly::IsValidSubTag(L"xxx000xxx", -1);
    I18n::DB::Interface::detachDatabase();
    RUNNER_ASSERT_MSG(!result, "Subtag found");
}
