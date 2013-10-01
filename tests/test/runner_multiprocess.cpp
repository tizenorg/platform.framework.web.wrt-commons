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
 * @file    runner_multiprocess.cpp
 * @author  Marcin Niesluchowski (m.niesluchow@samsung.com)
 * @version 1.0
 * @brief   Implementation file for test cases for engine internal tests
 */

#include <dpl/test/test_runner_multiprocess.h>
#include <dpl/test/test_runner.h>
#include <dpl/log/log.h>
#include <list>

namespace {
std::list<std::string> split_string(std::string str, std::string delimiter)
{
    size_t pos = 0;
    std::string token;
    std::list<std::string> stringList;
    while ((pos = str.find(delimiter)) != std::string::npos) {
        token = str.substr(0, pos);
        stringList.push_back(token);
        str.erase(0, pos + delimiter.length());
    }
    if(str.length() != 0){
        stringList.push_back(token);
    }
    return stringList;
}
}

#define RUNNER_MULTIPROCESS_TEST_EXPECT(name, messages)                        \
    static void testExpectFunction##name();                                    \
    RUNNER_TEST(name)                                                          \
    {                                                                          \
        Try                                                                    \
        {                                                                      \
            DPL::Test::RunMultiProc(&testExpectFunction##name);                \
        }                                                                      \
        Catch(DPL::Test::TestRunner::TestFailed)                               \
        {                                                                      \
            std::string eMsg = messages;                                       \
            std::list<std::string> eMessages = split_string(eMsg, "|");        \
            std::string rMessage = _rethrown_exception.GetMessage();           \
            if(eMsg.length() == 0 && rMessage.length() != 0) {                 \
                RUNNER_ASSERT_MSG(false, rMessage);                            \
            }                                                                  \
            bool failedFound = false;                                          \
            for(std::list<std::string>::iterator it = eMessages.begin();       \
                it != eMessages.end();                                         \
                ++it)                                                          \
            {                                                                  \
                if (!(*it).compare("TEST_FAILED")) {                           \
                    failedFound = true;                                        \
                    continue;                                                  \
                }                                                              \
                RUNNER_ASSERT_MSG(rMessage.find(*it)!=std::string::npos,       \
                    "Key word " << *it << " not found in " << rMessage);       \
            }                                                                  \
            RUNNER_ASSERT_MSG(                                                 \
                rMessage.find("Reading pipe error")==std::string::npos,        \
                "Reading pipe error");                                         \
            RUNNER_ASSERT_MSG(                                                 \
                rMessage.find("Timeout error")==std::string::npos,             \
                "Timeout error");                                              \
            RUNNER_ASSERT_MSG(failedFound, "No TEST_FAILED found");            \
        }                                                                      \
        Catch(DPL::Test::TestRunner::Ignored)                                  \
        {                                                                      \
            std::string eMsg = messages;                                       \
            std::list<std::string> eMessages = split_string(eMsg, "|");        \
            std::string rMessage = _rethrown_exception.GetMessage();           \
            if(eMsg.length() == 0 && rMessage.length() != 0) {                 \
                RUNNER_ASSERT_MSG(false, rMessage);                            \
            }                                                                  \
            bool ignoredFound = false;                                         \
            for(std::list<std::string>::iterator it = eMessages.begin();       \
                it != eMessages.end();                                         \
                ++it)                                                          \
            {                                                                  \
                if (!(*it).compare("TEST_IGNORED")) {                           \
                    ignoredFound = true;                                       \
                    continue;                                                  \
                }                                                              \
                RUNNER_ASSERT_MSG(rMessage.find(*it)!=std::string::npos,       \
                    "Key word " << *it << " not found in " << rMessage);       \
            }                                                                  \
            RUNNER_ASSERT_MSG(                                                 \
                rMessage.find("Reading pipe error")==std::string::npos,        \
                "Reading pipe error");                                         \
            RUNNER_ASSERT_MSG(                                                 \
                rMessage.find("Timeout error")==std::string::npos,             \
                "Timeout error");                                              \
            RUNNER_ASSERT_MSG(ignoredFound, "No TEST_IGNORED found");          \
        }                                                                      \
    }                                                                          \
    void testExpectFunction##name()                                            \

RUNNER_TEST_GROUP_INIT(DPL_TESTS_TEST_MULTIPROCESS)

RUNNER_MULTIPROCESS_TEST_EXPECT(tm00_pass, "")
{
    RUNNER_ASSERT_MSG(1, "This test should pass");
}

RUNNER_MULTIPROCESS_TEST_EXPECT(tm01_pass, "")
{
    pid_t pid = fork();
    if(pid){
        sleep(2);
        RUNNER_ASSERT_MSG(1, "This test should pass");
    } else {
        RUNNER_ASSERT_MSG(1, "This test should pass");
    }
    RUNNER_ASSERT_MSG(1, "This test should pass");
}

RUNNER_MULTIPROCESS_TEST_EXPECT(tm02_pass, "")
{
    pid_t pid = fork();
    if(pid){
        RUNNER_ASSERT_MSG(1, "This test should pass");
    } else {
        sleep(2);
        RUNNER_ASSERT_MSG(1, "This test should pass");
    }
    RUNNER_ASSERT_MSG(1, "This test should pass");
}

RUNNER_MULTIPROCESS_TEST_EXPECT(tm03_pass, "")
{
    pid_t pid = fork();
    if(pid){
        pid = fork();
        if(pid){
            sleep(1);
        } else {
            sleep(2);
        }
    } else {
        if(pid){
            sleep(2);
        } else {
            sleep(1);
        }
    }
}

RUNNER_MULTIPROCESS_TEST_EXPECT(tm04_fail, "TEST_FAILED|"
                                           "This test should fail")
{
    RUNNER_ASSERT_MSG(0, "This test should fail");
}

RUNNER_MULTIPROCESS_TEST_EXPECT(tm05_fail,"TEST_FAILED|"
                                          "Test failed 1|"
                                          "Test failed 2|"
                                          "Test failed 3|"
                                          "Test failed 4")
{
    pid_t pid = fork();
    if(pid){
        pid = fork();
        if(pid){
            RUNNER_ASSERT_MSG(0, "Test failed 1");
        } else {
            RUNNER_ASSERT_MSG(0, "Test failed 2");
        }
    } else {
        pid = fork();
        if(pid){
            RUNNER_ASSERT_MSG(0, "Test failed 3");
        } else {
            RUNNER_ASSERT_MSG(0, "Test failed 4");
        }
    }
}

RUNNER_MULTIPROCESS_TEST_EXPECT(tm06_fail, "TEST_FAILED|"
                                          "Test failed 1|"
                                          "Test failed 2|"
                                          "Test failed 3|"
                                          "Test failed 4")
{
    pid_t pid = fork();
    if(pid){
        pid = fork();
        if(pid){
            sleep(2);
            RUNNER_ASSERT_MSG(0, "Test failed 1");
        } else {
            RUNNER_ASSERT_MSG(0, "Test failed 2");
        }
    } else {
        pid = fork();
        if(pid){
            RUNNER_ASSERT_MSG(0, "Test failed 3");
        } else {
            RUNNER_ASSERT_MSG(0, "Test failed 4");
        }
    }
}

RUNNER_MULTIPROCESS_TEST_EXPECT(tm07_fail, "TEST_FAILED|"
                                          "Test failed 1|"
                                          "Test failed 2|"
                                          "Test failed 3|"
                                          "Test failed 4")
{
    pid_t pid = fork();
    if(pid){
        pid = fork();
        if(pid){
            RUNNER_ASSERT_MSG(0, "Test failed 1");
        } else {
            RUNNER_ASSERT_MSG(0, "Test failed 2");
        }
    } else {
        pid = fork();
        if(pid){
            sleep(2);
            RUNNER_ASSERT_MSG(0, "Test failed 3");
        } else {
            RUNNER_ASSERT_MSG(0, "Test failed 4");
        }
    }
}

RUNNER_MULTIPROCESS_TEST_EXPECT(tm08_fail_unknown_exception, "TEST_FAILED|"
                                                             "unknown exception")
{
    throw("hello");
}

RUNNER_MULTIPROCESS_TEST_EXPECT(tm09_fail_unknown_exception, "TEST_FAILED|"
                                                            "unknown exception")
{
    throw(1);
}

RUNNER_MULTIPROCESS_TEST_EXPECT(tm10_ignore, "TEST_IGNORED|"
                                             "Test ignored")
{
    RUNNER_IGNORED_MSG("Test ignored");
}

RUNNER_MULTIPROCESS_TEST_EXPECT(tm11_ignore, "TEST_IGNORED|"
                                            "Test ignored 1|"
                                            "Test ignored 2|"
                                            "Test ignored 3|"
                                            "Test ignored 4")
{
    pid_t pid = fork();
    if(pid){
        pid = fork();
        if(pid){
            RUNNER_IGNORED_MSG("Test ignored 1");
        } else {
            RUNNER_IGNORED_MSG("Test ignored 2");
        }
    } else {
        pid = fork();
        if(pid){
            sleep(2);
            RUNNER_IGNORED_MSG("Test ignored 3");
        } else {
            RUNNER_IGNORED_MSG("Test ignored 4");
        }
    }
}

RUNNER_MULTIPROCESS_TEST_EXPECT(tm12_fail, "TEST_FAILED|"
                                          "Test failed 1|"
                                          "Test ignored 2|"
                                          "Test ignored 3|"
                                          "Test ignored 4")
{
    pid_t pid = fork();
    if(pid){
        pid = fork();
        if(pid){
            RUNNER_ASSERT_MSG(0, "Test failed 1");
        } else {
            RUNNER_IGNORED_MSG("Test ignored 2");
        }
    } else {
        pid = fork();
        if(pid){
            sleep(2);
            RUNNER_IGNORED_MSG("Test ignored 3");
        } else {
            RUNNER_IGNORED_MSG("Test ignored 4");
        }
    }
}
