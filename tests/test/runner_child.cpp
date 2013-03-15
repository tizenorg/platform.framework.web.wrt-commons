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
 * @file    widget_version.cpp
 * @author  Bartlomiej Grzelewski (b.grzelewski@samsung.com)
 * @version 1.0
 * @brief   Implementation file for test cases for engine internal tests
 */
#include <dpl/test/test_runner_child.h>
#include <unistd.h>
#include <vector>
#include <sys/types.h>
#include <signal.h>

RUNNER_TEST_GROUP_INIT(DPL_TESTS_TEST_CHILD)

RUNNER_TEST(t00_pass)
{
    RUNNER_ASSERT_MSG(1, "This test should pass");
}

RUNNER_CHILD_TEST(t01_pass)
{
    RUNNER_ASSERT_MSG(1, "This test should pass");
}

RUNNER_CHILD_TEST(t02_fail)
{
    RUNNER_ASSERT_MSG(0, "This test should fail");
}

RUNNER_CHILD_TEST(t03_fail_timeout)
{
    sleep(20);
    RUNNER_ASSERT_MSG(1, "This test should fail");
}

RUNNER_CHILD_TEST(t04_fail)
{
    RUNNER_ASSERT_MSG(1, "This test should fail");
    RUNNER_ASSERT_MSG(1, "This test should fail");
    RUNNER_ASSERT_MSG(1, "This test should fail");
    RUNNER_ASSERT_MSG(1, "This test should fail");
    RUNNER_ASSERT_MSG(0, "This test should fail");
}

RUNNER_CHILD_TEST(t05_fail_child_died)
{
    kill(getpid(), SIGKILL);
    RUNNER_ASSERT_MSG(1, "This test should fail");
}

RUNNER_CHILD_TEST(t06_pass_8_second_test)
{
    sleep(8);
    RUNNER_ASSERT_MSG(1, "This test should pass");
}

RUNNER_CHILD_TEST(t07_fail_unknown_exception)
{
    throw("hello");
}

RUNNER_CHILD_TEST(t08_fail_unknown_exception)
{
    throw(1);
}

RUNNER_CHILD_TEST(t09_fail_you_should_see_text_normal_assert)
{
    RUNNER_ASSERT_MSG(0, "Normal assert");
}

RUNNER_CHILD_TEST(t10_pass)
{
    RUNNER_ASSERT_MSG(1, "Normal assert");
}

