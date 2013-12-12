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
#include <dpl/availability.h>
#include <dpl/atomic.h>
#include <dpl/test/test_results_collector.h>
#include <sstream>
#include <string>
#include <vector>
#include <chrono>
#include <list>
#include <set>
#include <map>

namespace DPL {
namespace Test {
class TestRunner
{
    typedef std::map<std::string, TestResultsCollectorBasePtr>
    TestResultsCollectors;
    TestResultsCollectors m_collectors;

    std::string m_startTestId;
    bool m_runIgnored;

  public:
    TestRunner() :
        m_currentTestCase(NULL)
      , m_terminate(false)
      , m_allowChildLogs(false)
    {}

    void beginPerformanceTestTime(std::chrono::system_clock::duration maxTimeInMicroseconds);
    void endPerformanceTestTime();
    void getCurrentTestCasePerformanceResult(bool& isPerformanceTest,
                                             std::chrono::system_clock::duration& result,
                                             std::chrono::system_clock::duration& resultMax);
    void setCurrentTestCasePerformanceResult(bool isPerformanceTest,
                                             std::chrono::system_clock::duration result,
                                             std::chrono::system_clock::duration resultMax);

    typedef void (*TestCase)();

  private:
    struct TestCaseStruct
    {
        std::string name;
        TestCase proc;

        bool m_isPerformanceTest;
        std::chrono::system_clock::time_point m_performanceTestStartTime;
        std::chrono::system_clock::duration m_performanceTestDurationTime;
        std::chrono::system_clock::duration m_performanceMaxTime;

        bool operator <(const TestCaseStruct &other) const
        {
            return name < other.name;
        }

        bool operator ==(const TestCaseStruct &other) const
        {
            return name == other.name;
        }

        TestCaseStruct(const std::string &n, TestCase p) :
            name(n),
            proc(p),
            m_isPerformanceTest(false)
        {}
    };

    typedef std::list<TestCaseStruct> TestCaseStructList;
    typedef std::map<std::string, TestCaseStructList> TestCaseGroupMap;
    TestCaseGroupMap m_testGroups;

    TestCaseStruct * m_currentTestCase;

    typedef std::set<std::string> SelectedTestNameSet;
    SelectedTestNameSet m_selectedTestNamesSet;
    typedef std::set<std::string> SelectedTestGroupSet;
    SelectedTestGroupSet m_selectedTestGroupSet;
    std::string m_currentGroup;

    DPL::Atomic m_totalAssertions;

    // Terminate without any logs.
    // Some test requires to call fork function.
    // Child process must not produce any logs and should die quietly.
    bool m_terminate;
    bool m_allowChildLogs;

    void Banner();
    void InvalidArgs(const std::string& message = "Invalid arguments!");
    void Usage();

    bool filterGroupsByXmls(const std::vector<std::string> & files);
    bool filterByXML(std::map<std::string, bool> & casesMap);
    void normalizeXMLTag(std::string& str, const std::string& testcase);

    enum Status { FAILED, IGNORED, PASS };

    Status RunTestCase(const TestCaseStruct& testCase);

    void setCurrentTestCase(TestCaseStruct* testCase);
    TestCaseStruct *getCurrentTestCase();

    void RunTests();

    void CollectResult(const std::string& id,
                       const std::string& description,
                       const TestResultsCollectorBase::FailStatus::Type status
                           = TestResultsCollectorBase::FailStatus::NONE,
                       const std::string& reason = std::string(),
                       const bool& isPerformanceTest = false,
                       const std::chrono::system_clock::duration& performanceTime = std::chrono::microseconds{0},
                       const std::chrono::system_clock::duration& performanceMaxTime = std::chrono::microseconds{0});

  public:
    class TestFailed
    {
      private:
        std::string m_message;

      public:
        TestFailed()
        {}

        //! \brief Failed test message creator
        //!
        //! \param[in] aTest string for tested expression
        //! \param[in] aFile source file name
        //! \param[in] aLine source file line
        //! \param[in] aMessage error message
        TestFailed(const char* aTest,
                   const char* aFile,
                   int aLine,
                   const std::string &aMessage);

        TestFailed(const std::string &message);

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
        {}

        Ignored(const std::string &message) :
            m_message(message)
        {}

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
    // The runner will terminate as soon as possible (after current test).
    void Terminate();
    bool GetAllowChildLogs();
};

typedef DPL::Singleton<TestRunner> TestRunnerSingleton;
}
} // namespace DPL

#define RUNNER_TEST_GROUP_INIT(GroupName)                                \
    static int Static##GroupName##Init()                                 \
    {                                                                    \
        DPL::Test::TestRunnerSingleton::Instance().InitGroup(#GroupName); \
        return 0;                                                        \
    }                                                                    \
    const int DPL_UNUSED Static##GroupName##InitVar =                   \
        Static##GroupName##Init();

#define RUNNER_TEST(Proc)                                                \
    void Proc();                                                         \
    static int Static##Proc##Init()                                      \
    {                                                                    \
        DPL::Test::TestRunnerSingleton::Instance().RegisterTest(#Proc, &Proc); \
        return 0;                                                        \
    }                                                                    \
    const int DPL_UNUSED Static##Proc##InitVar = Static##Proc##Init();  \
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
            throw DPL::Test::TestRunner::TestFailed(#test, \
                                                    __FILE__, \
                                                    __LINE__, \
                                                    assertMsg.str()); \
        }                                                                                  \
    } while (0)

#define RUNNER_ASSERT(test) RUNNER_ASSERT_MSG(test, "")

#define RUNNER_FAIL RUNNER_ASSERT(false)

#define RUNNER_IGNORED_MSG(message) do { std::ostringstream assertMsg; \
                                         assertMsg << message; \
                                         throw DPL::Test::TestRunner::Ignored( \
                                                   assertMsg.str()); \
} while (0)

/*
 * Use these macros to do the time measurement. The first macro will start time measurement,
 * the second will gather the result. These macros can be used only once per test-case.
 * The result of time measurement will be displayed only if the test will pass.
 * Notice that these macros will work only if will be used in parent process. If these
 * macros will be used in child process then there will be no time measure results printed.
 * This macro has now effect in multiprocess tests.
 * The precision of measurement is 1 microsecond - the smallest time value that can be
 * measured is 0.000001s.
 * The time measure results will be printed only specific output format:
 *     - text
 *     - html
 *     - xml
 *     - csv
 */
#define RUNNER_PERF_TEST_BEGIN(maxTime)                                        \
    do {                                                                       \
        DPL::Test::TestRunnerSingleton::Instance().beginPerformanceTestTime(   \
                std::chrono::microseconds{static_cast<long long int>(maxTime*1000000.0)}); \
    } while (0)

#define RUNNER_PERF_TEST_END()                                                \
    do {                                                                      \
        DPL::Test::TestRunnerSingleton::Instance().endPerformanceTestTime();  \
    } while (0)

#endif // DPL_TEST_RUNNER_H
