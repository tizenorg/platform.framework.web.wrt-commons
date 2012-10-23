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
 * @file        test_runner.h
 * @author      Przemyslaw Dobrowolski (p.dobrowolsk@samsung.com)
 * @author      Lukasz Wrzosek (l.wrzosek@samsung.com)
 * @version     1.0
 * @brief       This file is the header file of test runner
 */
#ifndef DPL_TEST_RUNNER_H
#define DPL_TEST_RUNNER_H

#include <dpl/singleton.h>
#include <dpl/unused.h>
#include <dpl/atomic.h>
#include <dpl/test/test_results_collector.h>
#include <sstream>
#include <string>
#include <vector>
#include <list>
#include <set>
#include <map>

namespace DPL
{
namespace Test
{
class TestRunner
{
    typedef std::map<std::string, TestResultsCollectorBasePtr>
            TestResultsCollectors;
    TestResultsCollectors m_collectors;

    std::string m_startTestId;
    bool m_runIgnored;

public:
    typedef void (*TestCase)();

private:
    struct TestCaseStruct
    {
        std::string name;
        TestCase proc;

        bool operator <(const TestCaseStruct &other) const
        {
            return name < other.name;
        }

        bool operator ==(const TestCaseStruct &other) const
        {
            return name == other.name;
        }

        TestCaseStruct(const std::string &n, TestCase p)
            : name(n),
              proc(p)
        {
        }
    };

    typedef std::list<TestCaseStruct> TestCaseStructList;
    typedef std::map<std::string, TestCaseStructList> TestCaseGroupMap;
    TestCaseGroupMap m_testGroups;

    typedef std::set<std::string> SelectedTestNameSet;
    SelectedTestNameSet m_selectedTestNamesSet;
    typedef std::set<std::string> SelectedTestGroupSet;
    SelectedTestGroupSet m_selectedTestGroupSet;
    std::string m_currentGroup;

    DPL::Atomic m_totalAssertions;

    void Banner();
    void InvalidArgs(const std::string& message = "Invalid arguments!");
    void Usage();

    enum Status { FAILED, IGNORED, PASS };

    Status RunTestCase(const TestCaseStruct& testCase);

    void RunTests();

    void CollectResult(const std::string& id,
                       const std::string& description,
                       const TestResultsCollectorBase::FailStatus::Type status
                           = TestResultsCollectorBase::FailStatus::NONE,
                       const std::string& reason = std::string());

public:
    class TestFailed
    {
    private:
        std::string m_message;

    public:
        TestFailed()
        {
        }

        //! \brief Failed test message creator
        //!
        //! \param[in] aTest string for tested expression
        //! \param[in] aFile source file name
        //! \param[in] aLine source file line
        //! \param[in] aMessage error message
        TestFailed(const char* aTest, const char* aFile, int aLine, const std::string &aMessage);

        std::string GetMessage() const
        {
            return m_message;
        }
    };

    class Ignored
    {
    private:
        std::string m_message;

    public:
        Ignored()
        {
        }

        Ignored(const std::string &message)
            : m_message(message)
        {
        }

        std::string GetMessage() const
        {
            return m_message;
        }
    };

    void MarkAssertion();

    void RegisterTest(const char *testName, TestCase proc);
    void InitGroup(const char* name);

    int ExecTestRunner(int argc, char *argv[]);
    typedef std::vector<std::string> ArgsList;
    int ExecTestRunner(const ArgsList& args);
    bool getRunIgnored() const;
};

typedef DPL::Singleton<TestRunner> TestRunnerSingleton;

}
} // namespace DPL

#define RUNNER_TEST_GROUP_INIT(GroupName)                                \
    static int Static##GroupName##Init()                                 \
    {                                                                    \
        DPL::Test::TestRunnerSingleton::Instance().InitGroup(#GroupName);\
        return 0;                                                        \
    }                                                                    \
    const int DPL_UNUSED  Static##GroupName##InitVar =                   \
        Static##GroupName##Init();

#define RUNNER_TEST(Proc)                                                \
    void Proc();                                                         \
    static int Static##Proc##Init()                                      \
    {                                                                    \
        DPL::Test::TestRunnerSingleton::Instance().RegisterTest(#Proc, &Proc); \
        return 0;                                                        \
    }                                                                    \
    const int DPL_UNUSED  Static##Proc##InitVar = Static##Proc##Init();  \
    void Proc()

//! \brief Returns base name for path

#define RUNNER_ASSERT_MSG(test, message)                                               \
do                                                                                     \
{                                                                                      \
    DPL::Test::TestRunnerSingleton::Instance().MarkAssertion();                        \
                                                                                       \
    if (!(test))                                                                       \
    {                                                                                  \
        std::ostringstream assertMsg;                                                  \
        assertMsg << message;                                                          \
        throw DPL::Test::TestRunner::TestFailed(#test, __FILE__, __LINE__, assertMsg.str()); \
    }                                                                                  \
} while (0)

#define RUNNER_ASSERT(test) RUNNER_ASSERT_MSG(test, "")

#define RUNNER_FAIL RUNNER_ASSERT(false)

#define RUNNER_IGNORED_MSG(message) do { std::ostringstream assertMsg; assertMsg << message; throw DPL::Test::TestRunner::Ignored(assertMsg.str()); } while (0)

#endif // DPL_TEST_RUNNER_H
