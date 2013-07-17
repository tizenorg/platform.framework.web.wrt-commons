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
#include <dpl/log/log.h>
#include <unistd.h>
#include <vector>
#include <sys/types.h>
#include <signal.h>


namespace {
enum class TestResult
{
    PASS,
    FAIL,
    IGNORED,
    TIMEOUT,
    UNKNOWN
};
}

#define RUNNER_CHILD_TEST_EXPECT(name, result, message)                        \
    static void testExpectFunction##name();                                    \
    RUNNER_TEST(name)                                                          \
    {                                                                          \
        TestResult eResult = result;                                           \
        TestResult rResult = TestResult::UNKNOWN;                              \
        std::string eMessage = message;                                        \
        Try                                                                    \
        {                                                                      \
            DPL::Test::RunChildProc(&testExpectFunction##name);                \
        }                                                                      \
        Catch(DPL::Test::TestRunner::TestFailed)                               \
        {                                                                      \
            std::string rMessage = _rethrown_exception.GetMessage();           \
            size_t pos = rMessage.find(")");                                   \
            if(pos != std::string::npos && pos+2 <= rMessage.length())         \
            {                                                                  \
                rMessage = rMessage.substr(pos+2);                             \
            }                                                                  \
            if(rMessage == "Timeout")                                          \
            {                                                                  \
                rResult = TestResult::TIMEOUT;                                 \
            }                                                                  \
            else if(rMessage == "Ignored")                                     \
            {                                                                  \
                rResult = TestResult::IGNORED;                                 \
            }                                                                  \
            else if(rMessage == eMessage)                                      \
            {                                                                  \
                rResult = TestResult::FAIL;                                    \
            }                                                                  \
            else                                                               \
            {                                                                  \
                RUNNER_ASSERT_MSG(false, "Fail message do not matches");       \
            }                                                                  \
        }                                                                      \
        if(rResult == TestResult::UNKNOWN)                                     \
        {                                                                      \
            rResult = TestResult::PASS;                                        \
        }                                                                      \
        RUNNER_ASSERT_MSG(eResult == rResult, "Expected other result");        \
    }                                                                          \
    void testExpectFunction##name()                                            \


RUNNER_TEST_GROUP_INIT(DPL_TESTS_TEST_CHILD)

RUNNER_CHILD_TEST_EXPECT(t00_pass, TestResult::PASS, "")
{
    RUNNER_ASSERT_MSG(1, "This test should pass");
}

RUNNER_CHILD_TEST_EXPECT(t01_pass, TestResult::PASS, "")
{
    RUNNER_ASSERT_MSG(1, "This test should pass");
}

RUNNER_CHILD_TEST_EXPECT(t02_fail, TestResult::FAIL, "This test should fail")
{
    RUNNER_ASSERT_MSG(0, "This test should fail");
}

RUNNER_CHILD_TEST_EXPECT(t03_fail_timeout, TestResult::TIMEOUT, "")
{
    sleep(20);
    RUNNER_ASSERT_MSG(1, "This test should fail");
}

RUNNER_CHILD_TEST_EXPECT(t04_fail, TestResult::FAIL, "This test should fail")
{
    RUNNER_ASSERT_MSG(1, "This test should fail");
    RUNNER_ASSERT_MSG(1, "This test should fail");
    RUNNER_ASSERT_MSG(1, "This test should fail");
    RUNNER_ASSERT_MSG(1, "This test should fail");
    RUNNER_ASSERT_MSG(0, "This test should fail");
}

RUNNER_CHILD_TEST_EXPECT(t05_fail_child_died, TestResult::FAIL, "Reading pipe error")
{
    kill(getpid(), SIGKILL);
    RUNNER_ASSERT_MSG(1, "This test should fail");
}

RUNNER_CHILD_TEST_EXPECT(t06_pass_8_second_test, TestResult::PASS, "")
{
    sleep(8);
    RUNNER_ASSERT_MSG(1, "This test should pass");
}

RUNNER_CHILD_TEST_EXPECT(t07_fail_unknown_exception, TestResult::FAIL, "unhandled exeception")
{
    throw("hello");
}

RUNNER_CHILD_TEST_EXPECT(t08_fail_unknown_exception, TestResult::FAIL, "unhandled exeception")
{
    throw(1);
}

RUNNER_CHILD_TEST_EXPECT(t09_fail_you_should_see_text_normal_assert, TestResult::FAIL, "Normal assert")
{
    RUNNER_ASSERT_MSG(0, "Normal assert");
}

RUNNER_CHILD_TEST_EXPECT(t10_pass, TestResult::PASS, "")
{
    RUNNER_ASSERT_MSG(1, "Normal assert");
}

RUNNER_CHILD_TEST_EXPECT(t11_ignore, TestResult::IGNORED, "Test ignored")
{
    RUNNER_IGNORED_MSG("Test ignored");
}


