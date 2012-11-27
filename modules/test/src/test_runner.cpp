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
 * @file        test_runner.cpp
 * @author      Przemyslaw Dobrowolski (p.dobrowolsk@samsung.com)
 * @author      Lukasz Wrzosek (l.wrzosek@samsung.com)
 * @version     1.0
 * @brief       This file is the implementation file of test runner
 */
#include <stddef.h>
#include <dpl/test/test_runner.h>
#include <dpl/test/test_results_collector.h>
#include <dpl/exception.h>
#include <dpl/scoped_free.h>
#include <dpl/foreach.h>
#include <dpl/log/log.h>
#include <dpl/colors.h>
#include <pcrecpp.h>
#include <algorithm>
#include <cstdio>
#include <memory.h>
#include <libgen.h>
#include <cstring>
#include <cstdlib>
#include <dpl/utils/wrt_global_settings.h>

#include <dpl/singleton_impl.h>
IMPLEMENT_SINGLETON(DPL::Test::TestRunner)

namespace DPL
{
namespace Test
{

namespace // anonymous
{

std::string BaseName(std::string aPath)
{
    ScopedFree<char> path(strdup(aPath.c_str()));
    if (NULL == path.Get())
    {
       throw std::bad_alloc();
    }
    char* baseName = basename(path.Get());
    std::string retValue = baseName;
    return retValue;
}

} // namespace anonymous

//! \brief Failed test message creator
//!
//! \param[in] aTest string for tested expression
//! \param[in] aFile source file name
//! \param[in] aLine source file line
//! \param[in] aMessage error message
TestRunner::TestFailed::TestFailed(const char* aTest,
                                   const char* aFile,
                                   int aLine,
                                   const std::string &aMessage)
{
    std::ostringstream assertMsg;
    assertMsg << "[" << BaseName(aFile) << ":" << aLine
              << "] Assertion failed ("
              << aTest << ") " << aMessage;
    m_message = assertMsg.str();
}

void TestRunner::RegisterTest(const char *testName, TestCase proc)
{
    m_testGroups[m_currentGroup].push_back(TestCaseStruct(testName, proc));
}

void TestRunner::InitGroup(const char* name)
{
    m_currentGroup = name;
}


TestRunner::Status TestRunner::RunTestCase(const TestCaseStruct& testCase)
{
    try
    {
        testCase.proc();
    }
    catch (const TestFailed &e)
    {
        // Simple test failure
        CollectResult(testCase.name,
                      "",
                      TestResultsCollectorBase::FailStatus::FAILED,
                      e.GetMessage());
        return FAILED;
    }
    catch (const Ignored &e)
    {
        if (m_runIgnored) {
            // Simple test have to be implemented
            CollectResult(testCase.name,
                          "",
                          TestResultsCollectorBase::FailStatus::IGNORED,
                          e.GetMessage());
        }

        return IGNORED;
    }
    catch (const DPL::Exception &e)
    {
        // DPL exception failure
        CollectResult(testCase.name,
                      "",
                      TestResultsCollectorBase::FailStatus::INTERNAL,
                      "DPL exception:" + e.GetMessage());

        return FAILED;
    }
    catch (const std::exception &)
    {
        // std exception failure
        CollectResult(testCase.name,
                      "",
                      TestResultsCollectorBase::FailStatus::INTERNAL,
                      "std exception");

        return FAILED;
    }
    catch (...)
    {
        // Unknown exception failure
        CollectResult(testCase.name,
                      "",
                      TestResultsCollectorBase::FailStatus::INTERNAL,
                      "unknown exception");

        return FAILED;
    }

    CollectResult(testCase.name,
                  "",
                  TestResultsCollectorBase::FailStatus::NONE);

    // Everything OK
    return PASS;
}

void TestRunner::RunTests()
{
    using namespace DPL::Colors::Text;

    Banner();
    std::for_each(m_collectors.begin(),
                  m_collectors.end(),
                  [](const TestResultsCollectors::value_type& collector)
                  {
                      collector.second->Start();
                  });

    fprintf(stderr, "%s%s%s\n", GREEN_BEGIN, "Running tests...", GREEN_END);
    FOREACH(group, m_testGroups) {
        TestCaseStructList list = group->second;
        if (!list.empty()) {
            std::for_each(
                    m_collectors.begin(),
                    m_collectors.end(),
                    [&group](const TestResultsCollectors::value_type& collector)
                    {
                        collector.second->
                                CollectCurrentTestGroupName(group->first);
                    });
            list.sort();

            for (TestCaseStructList::const_iterator iterator = list.begin();
                 iterator != list.end();
                 ++iterator)
            {
                TestCaseStruct test = *iterator;
                if (m_startTestId == test.name)
                    m_startTestId = "";

                if (m_startTestId.empty()) {
                    RunTestCase(test);
                }
            }
        }
    }

    std::for_each(m_collectors.begin(),
                  m_collectors.end(),
                  [](const TestResultsCollectors::value_type& collector)
                  {
                      collector.second->Finish();
                  });

    // Finished
    fprintf(stderr, "%s%s%s\n\n", GREEN_BEGIN, "Finished", GREEN_END);
}

void TestRunner::CollectResult(
        const std::string& id,
        const std::string& description,
        const TestResultsCollectorBase::FailStatus::Type status,
        const std::string& reason)
{
    std::for_each(m_collectors.begin(),
                  m_collectors.end(),
                  [&](const TestResultsCollectors::value_type& collector)
                  {
                      collector.second->CollectResult(id,
                                                      description,
                                                      status,
                                                      reason);
                  });
}

void TestRunner::Banner()
{
    using namespace DPL::Colors::Text;
    fprintf(stderr,
            "%s%s%s\n",
            BOLD_GREEN_BEGIN,
            "DPL tests runner",
            BOLD_GREEN_END);
    fprintf(stderr,
            "%s%s%s%s\n\n",
            GREEN_BEGIN,
            "Build: ",
            __TIMESTAMP__,
            GREEN_END);
}

void TestRunner::InvalidArgs(const std::string& message)
{
    using namespace DPL::Colors::Text;
    fprintf(stderr,
            "%s%s%s\n",
            BOLD_RED_BEGIN,
            message.c_str(),
            BOLD_RED_END);
}

void TestRunner::Usage()
{
    fprintf(stderr, "Usage: runner [options]\n\n");
    fprintf(stderr, "Output type:\n");
    fprintf(stderr, "  --output=<output type> --output=<output type> ...\n");
    fprintf(stderr, "\n  possible output types:\n");
    FOREACH (type, TestResultsCollectorBase::GetCollectorsNames()) {
        fprintf(stderr, "    --output=%s\n", type->c_str());
    }
    fprintf(stderr, "\n  example:\n");
    fprintf(stderr, "    test-binary --output=text --output=xml --file=output.xml\n\n");
    fprintf(stderr, "Other parameters:\n");
    fprintf(stderr,
            "  --regexp='regexp'\t Only selected tests"
            " which names match regexp run\n\n");
    fprintf(stderr, "  --start=<test id>\tStart from concrete test id");
    fprintf(stderr, "  --group=<group name>\t Run tests only from one group\n");
    fprintf(stderr, "  --runignored\t Run also ignored tests\n");
    fprintf(stderr, "  --list\t Show a list of Test IDs\n");
    fprintf(stderr, "  --listgroups\t Show a list of Test Group names \n");
    fprintf(stderr, "  --listingroup=<group name>\t Show a list of Test IDS in one group\n");
    fprintf(stderr, "  --help\t This help\n\n");
    std::for_each(m_collectors.begin(),
                  m_collectors.end(),
                  [](const TestResultsCollectors::value_type& collector)
                  {
                      fprintf(stderr,
                              "Output %s has specific args:\n",
                              collector.first.c_str());
                      fprintf(stderr,
                              "%s\n",
                              collector.second->
                                      CollectorSpecificHelp().c_str());
                  });
    fprintf(stderr, "For bug reporting, please write to:\n");
    fprintf(stderr, "<p.dobrowolsk@samsung.com>\n");
}

int TestRunner::ExecTestRunner(int argc, char *argv[])
{
    std::vector<std::string> args;
    for (int i = 0; i < argc; ++i)
    {
        args.push_back(argv[i]);
    }
    return ExecTestRunner(args);
}

void TestRunner::MarkAssertion()
{
    ++m_totalAssertions;
}

int TestRunner::ExecTestRunner(const ArgsList& value)
{
    m_runIgnored = false;
    ArgsList args = value;
    // Parse command line
    if (args.size() == 1)
    {
        InvalidArgs();
        Usage();
        return -1;
    }

    args.erase(args.begin());

    bool showHelp = false;

    TestResultsCollectorBasePtr currentCollector;

    // Parse each argument
    FOREACH(it, args)
    {
        std::string arg = *it;
        const std::string regexp = "--regexp=";
        const std::string output = "--output=";
        const std::string groupId = "--group=";
        const std::string runIgnored = "--runignored";
        const std::string listCmd = "--list";
        const std::string startCmd = "--start=";
        const std::string listGroupsCmd = "--listgroups";
        const std::string listInGroup = "--listingroup=";

        if (currentCollector)
        {
            if (currentCollector->ParseCollectorSpecificArg(arg))
            {
                continue;
            }
        }

        if (arg.find(startCmd) == 0)
        {
            arg.erase(0, startCmd.length());
            FOREACH(group, m_testGroups) {
                FOREACH(tc, group->second) {
                    if (tc->name == arg) {
                        m_startTestId = arg;
                        break;
                    }
                }
                if (!m_startTestId.empty()) break;
            }
            if (!m_startTestId.empty()) continue;
            InvalidArgs();
            fprintf(stderr, "Start test id has not been found\n");
            Usage();
            return 0;
        }
        else if (arg.find(groupId) == 0)
        {
            arg.erase(0, groupId.length());
            TestCaseGroupMap::iterator found = m_testGroups.find(arg);
            if (found != m_testGroups.end()) {
                std::string name = found->first;
                TestCaseStructList newList = found->second;
                m_testGroups.clear();
                m_testGroups[name] = newList;
            } else {
                fprintf(stderr, "Group %s not found\n", arg.c_str());
                InvalidArgs();
                Usage();
                return -1;
            }
        }
        else if (arg == runIgnored)
        {
            m_runIgnored = true;
        }
        else if (arg == listCmd)
        {
            FOREACH(group, m_testGroups) {
                FOREACH(test, group->second) {
                    printf("ID:%s:%s\n", group->first.c_str(), test->name.c_str());
                }
            }
            return 0;
        }
        else if (arg == listGroupsCmd)
        {
            FOREACH(group, m_testGroups) {
                printf("GR:%s\n", group->first.c_str());
            }
            return 0;
        }
        else if (arg.find(listInGroup) == 0)
        {
            arg.erase(0, listInGroup.length());
            FOREACH(test, m_testGroups[arg]) {
                printf("ID:%s\n", test->name.c_str());
            }
            return 0;
        }
        else if (arg == "--help")
            showHelp = true;
        else if (arg.find(output) == 0)
        {
            arg.erase(0, output.length());
            if (m_collectors.find(arg) != m_collectors.end())
            {
                InvalidArgs("Multiple outputs of the same type are not supported!");
                Usage();
                return -1;
            }
            currentCollector.reset(TestResultsCollectorBase::Create(arg));
            if (!currentCollector) {
                InvalidArgs("Unsupported output type!");
                Usage();
                return -1;
            }
            m_collectors[arg] = currentCollector;
        }
        else if (arg.find(regexp) == 0)
        {
            arg.erase(0, regexp.length());
            if (arg.length() == 0)
            {
                InvalidArgs();
                Usage();
                return -1;
            }

            if (arg[0] == '\'' && arg[arg.length() - 1] == '\'')
            {
                arg.erase(0);
                arg.erase(arg.length() - 1);
            }

            if (arg.length() == 0)
            {
                InvalidArgs();
                Usage();
                return -1;
            }

            pcrecpp::RE re(arg.c_str());
            FOREACH(group, m_testGroups) {
                TestCaseStructList newList;
                FOREACH(iterator, group->second)
                {
                    if (re.PartialMatch(iterator->name))
                    {
                        newList.push_back(*iterator);
                    }
                }
                group->second = newList;
            }
        }
        else
        {
            InvalidArgs();
            Usage();
            return -1;
        }
    }

    currentCollector.reset();

    // Show help
    if (showHelp)
    {
        Usage();
        return 0;
    }

    if (m_collectors.empty())
    {
        TestResultsCollectorBasePtr collector(
                TestResultsCollectorBase::Create("text"));
        m_collectors["text"] = collector;
    }

    for (auto it = m_collectors.begin(); it != m_collectors.end(); ++it)
    {
        if (!it->second->Configure())
        {
            fprintf(stderr, "Could not configure selected output");
            return 0;
        }
    }

    // Run tests
    RunTests();

    return 0;
}

bool TestRunner::getRunIgnored() const
{
    return m_runIgnored;
}

}
} // namespace DPL
