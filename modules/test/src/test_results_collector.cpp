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
 * @file        test_results_collector.h
 * @author      Lukasz Wrzosek (l.wrzosek@samsung.com)
 * @version     1.0
 * @brief       Implementation file some concrete TestResulstsCollector
 */
#include <stddef.h>
#include <dpl/test/test_results_collector.h>
#include <dpl/colors.h>
#include <dpl/assert.h>
#include <dpl/foreach.h>
#include <dpl/scoped_fclose.h>

#include <string>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <cstdlib>

#define GREEN_RESULT_OK "[%s%s%s]\n", BOLD_GREEN_BEGIN, "   OK   ", BOLD_GREEN_END

namespace DPL
{
namespace Test
{

namespace
{
const char *DEFAULT_HTML_FILE_NAME = "index.html";
const char *DEFAULT_TAP_FILE_NAME = "results.tap";
const char *DEFAULT_XML_FILE_NAME = "results.xml";

bool ParseCollectorFileArg(const std::string &arg, std::string &filename)
{
    const std::string argname = "--file=";
    if (0 == arg.find(argname)) {
        filename = arg.substr(argname.size());
        return true;
    }
    return false;
}

class Statistic
{
  public:
    Statistic() :
        m_failed(0),
        m_ignored(0),
        m_passed(0),
        m_count(0)
    {
    }

    void AddTest(TestResultsCollectorBase::FailStatus::Type type)
    {
        ++m_count;
        switch (type) {
            case TestResultsCollectorBase::FailStatus::INTERNAL:
            case TestResultsCollectorBase::FailStatus::FAILED:   ++m_failed; break;
            case TestResultsCollectorBase::FailStatus::IGNORED:  ++m_ignored; break;
            case TestResultsCollectorBase::FailStatus::NONE:     ++m_passed; break;
            default:
                Assert(false && "Bad FailStatus");
        }
    }

    size_t GetTotal() const { return m_count; }
    size_t GetPassed() const { return m_passed; }
    size_t GetSuccesed() const { return m_passed; }
    size_t GetFailed() const { return m_failed; }
    size_t GetIgnored() const { return m_ignored; }
    float GetPassedOrIgnoredPercend() const
    {
        float passIgnoredPercent =
            100.0f * (static_cast<float>(m_passed)
                      + static_cast<float>(m_ignored))
            / static_cast<float>(m_count);
        return passIgnoredPercent;
    }

  private:
    size_t m_failed;
    size_t m_ignored;
    size_t m_passed;
    size_t m_count;
};

class ConsoleCollector
    : public TestResultsCollectorBase
{
  public:
    static TestResultsCollectorBase* Constructor();

  private:
    ConsoleCollector() {}

    virtual void CollectCurrentTestGroupName(const std::string& name)
    {
        printf("Starting group %s\n", name.c_str());
        m_currentGroup = name;
    }

    virtual void Finish()
    {
        using namespace DPL::Colors::Text;

        // Show result
        FOREACH(group, m_groupsStats) {
            PrintStats(group->first, group->second);
        }
        PrintStats("All tests together", m_stats);
    }

    virtual void CollectResult(const std::string& id,
                               const std::string& /*description*/,
                               const FailStatus::Type status = FailStatus::NONE,
                               const std::string& reason = "")
    {
        using namespace DPL::Colors::Text;
        std::string tmp = "'" + id + "' ...";

        printf("Running test case %-60s", tmp.c_str());
        switch(status) {
            case TestResultsCollectorBase::FailStatus::NONE:
                printf(GREEN_RESULT_OK); break;
            case TestResultsCollectorBase::FailStatus::FAILED:
                PrintfErrorMessage(  " FAILED ", reason, true); break;
            case TestResultsCollectorBase::FailStatus::IGNORED:
                PrintfIgnoredMessage("Ignored ", reason, true); break;
            case TestResultsCollectorBase::FailStatus::INTERNAL:
                PrintfErrorMessage(  "INTERNAL", reason, true); break;
            default:
                Assert(false && "Bad status");
        }
        m_stats.AddTest(status);
        m_groupsStats[m_currentGroup].AddTest(status);
    }

    void PrintfErrorMessage(const char* type,
                            const std::string& message,
                            bool verbosity)
    {
        using namespace DPL::Colors::Text;
        if (verbosity) {
            printf("[%s%s%s] %s%s%s\n",
                   BOLD_RED_BEGIN,
                   type,
                   BOLD_RED_END,
                   BOLD_YELLOW_BEGIN,
                   message.c_str(),
                   BOLD_YELLOW_END);
        } else {
            printf("[%s%s%s]\n",
                    BOLD_RED_BEGIN,
                    type,
                    BOLD_RED_END);
        }
    }

    void PrintfIgnoredMessage(const char* type,
                              const std::string& message,
                              bool verbosity)
    {
        using namespace DPL::Colors::Text;
        if (verbosity) {
            printf("[%s%s%s] %s%s%s\n",
                   CYAN_BEGIN,
                   type,
                   CYAN_END,
                   BOLD_GOLD_BEGIN,
                   message.c_str(),
                   BOLD_GOLD_END);
        } else {
            printf("[%s%s%s]\n",
                   CYAN_BEGIN ,
                   type,
                   CYAN_END);
        }
    }

    void PrintStats(const std::string& title, const Statistic& stats)
    {
        using namespace DPL::Colors::Text;
        printf("\n%sResults [%s]: %s\n", BOLD_GREEN_BEGIN, title.c_str(), BOLD_GREEN_END);
        printf("%s%s%3d%s\n", CYAN_BEGIN,     "Total tests:            ", stats.GetTotal(), CYAN_END);
        printf("  %s%s%3d%s\n", CYAN_BEGIN,   "Succeeded:            ", stats.GetPassed(), CYAN_END);
        printf("  %s%s%3d%s\n", CYAN_BEGIN,   "Failed:               ", stats.GetFailed(), CYAN_END);
        printf("  %s%s%3d%s\n", CYAN_BEGIN,   "Ignored:              ", stats.GetIgnored(), CYAN_END);
    }

    Statistic m_stats;
    std::map<std::string, Statistic> m_groupsStats;
    std::string m_currentGroup;
};


TestResultsCollectorBase* ConsoleCollector::Constructor()
{
    return new ConsoleCollector();
}


class HtmlCollector
    : public TestResultsCollectorBase
{
  public:
    static TestResultsCollectorBase* Constructor();

  private:
    HtmlCollector() : m_filename(DEFAULT_HTML_FILE_NAME) {}

    virtual void CollectCurrentTestGroupName(const std::string& name)
    {
        fprintf(m_fp.Get(),"<b>Starting group %s", name.c_str());
        m_currentGroup = name;
    }

    virtual bool Configure()
    {
        m_fp.Reset(fopen (m_filename.c_str(), "w"));
        if (!m_fp) {
            LogPedantic("Could not open file " << m_filename << " for writing");
            return false;
        }
        return true;
    }
    virtual std::string CollectorSpecificHelp() const
    {
        return "--file=<filename> - name of file for output\n"
               "                    default - index.html\n";
    }

    virtual void Start()
    {
        Assert(!!m_fp && "File handle must not be null");
        fprintf(m_fp.Get(),
                "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0"
                "Transitional//EN\" "
                "\"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\""
                ">\n");
        fprintf(m_fp.Get(),
                "<html xmlns=\"http://www.w3.org/1999/xhtml\" "
                "lang=\"en\" dir=\"ltr\">\n");
        fprintf(m_fp.Get(), "<body style=\"background-color: black;\">\n");
        fprintf(m_fp.Get(), "<pre>\n");
        fprintf(m_fp.Get(), "<font color=\"white\">\n");
    }

    virtual void Finish()
    {
        using namespace DPL::Colors::Html;
        // Show result
        FOREACH(group, m_groupsStats) {
            PrintStats(group->first, group->second);
        }
        PrintStats("All tests together", m_stats);
        fprintf(m_fp.Get(), "</font>\n");
        fprintf(m_fp.Get(), "</pre>\n");
        fprintf(m_fp.Get(), "</body>\n");
        fprintf(m_fp.Get(), "</html>\n");
    }

    virtual bool ParseCollectorSpecificArg(const std::string& arg)
    {
        return ParseCollectorFileArg(arg, m_filename);
    }

    virtual void CollectResult(const std::string& id,
                               const std::string& /*description*/,
                               const FailStatus::Type status = FailStatus::NONE,
                               const std::string& reason = "")
    {
        using namespace DPL::Colors::Html;
        std::string tmp = "'" + id + "' ...";

        fprintf(m_fp.Get(), "Running test case %-100s", tmp.c_str());
        switch(status) {
            case TestResultsCollectorBase::FailStatus::NONE:
                fprintf(m_fp.Get(), GREEN_RESULT_OK); break;
            case TestResultsCollectorBase::FailStatus::FAILED:
                PrintfErrorMessage(  " FAILED ", reason, true); break;
            case TestResultsCollectorBase::FailStatus::IGNORED:
                PrintfIgnoredMessage("Ignored ", reason, true); break;
            case TestResultsCollectorBase::FailStatus::INTERNAL:
                PrintfErrorMessage(  "INTERNAL", reason, true); break;
            default:
                Assert(false && "Bad status");
        }
        m_groupsStats[m_currentGroup].AddTest(status);
        m_stats.AddTest(status);
    }

    void PrintfErrorMessage(const char* type,
                            const std::string& message,
                            bool verbosity)
    {
        using namespace DPL::Colors::Html;
        if (verbosity) {
            fprintf(m_fp.Get(),
                    "[%s%s%s] %s%s%s\n",
                    BOLD_RED_BEGIN,
                    type,
                    BOLD_RED_END,
                    BOLD_YELLOW_BEGIN,
                    message.c_str(),
                    BOLD_YELLOW_END);
        } else {
            fprintf(m_fp.Get(),
                    "[%s%s%s]\n",
                    BOLD_RED_BEGIN,
                    type,
                    BOLD_RED_END);
        }
    }

    void PrintfIgnoredMessage(const char* type,
                              const std::string& message,
                              bool verbosity)
    {
        using namespace DPL::Colors::Html;

        if (verbosity) {
            fprintf(m_fp.Get(),
                    "[%s%s%s] %s%s%s\n",
                    CYAN_BEGIN,
                    type,
                    CYAN_END,
                    BOLD_GOLD_BEGIN,
                    message.c_str(),
                    BOLD_GOLD_END);
        } else {
            fprintf(m_fp.Get(),
                    "[%s%s%s]\n",
                    CYAN_BEGIN ,
                    type,
                    CYAN_END);
        }
    }

    void PrintStats(const std::string& name, const Statistic& stats)
    {
        using namespace DPL::Colors::Html;
        fprintf(m_fp.Get(), "\n%sResults [%s]:%s\n", BOLD_GREEN_BEGIN, name.c_str(), BOLD_GREEN_END);
        fprintf(m_fp.Get(), "%s%s%3d%s\n", CYAN_BEGIN,     "Total tests:            ", stats.GetTotal(), CYAN_END);
        fprintf(m_fp.Get(), "  %s%s%3d%s\n", CYAN_BEGIN,   "Succeeded:            ", stats.GetPassed(), CYAN_END);
        fprintf(m_fp.Get(), "  %s%s%3d%s\n", CYAN_BEGIN,   "Failed:               ", stats.GetFailed(), CYAN_END);
        fprintf(m_fp.Get(), "  %s%s%3d%s\n", CYAN_BEGIN,   "Ignored:              ", stats.GetIgnored(), CYAN_END);
    }

    std::string m_filename;
    ScopedFClose m_fp;
    Statistic m_stats;
    std::string m_currentGroup;
    std::map<std::string, Statistic> m_groupsStats;
};

TestResultsCollectorBase* HtmlCollector::Constructor()
{
    return new HtmlCollector();
}


class XmlCollector
    : public TestResultsCollectorBase
{
  public:
    static TestResultsCollectorBase* Constructor();

  private:
    XmlCollector() : m_filename(DEFAULT_XML_FILE_NAME) {}

    virtual void CollectCurrentTestGroupName(const std::string& name)
    {
        std::string groupHeader;
        groupHeader.append("\n\t<testsuite name=\"");
        groupHeader.append(EscapeSpecialCharacters(name));
        groupHeader.append("\">\n\t\t<testcase name=\"unknown\" status=\"FAILED\">\n");
        groupHeader.append("\t\t\t<failure type=\"FAILED\" message=\"segmentation fault\"/>\n");
        groupHeader.append("\t\t</testcase>\n\t</testsuite>");
        size_t pos = m_outputBuffer.find("</testsuites>");
        m_outputBuffer.insert(pos - 1, groupHeader);
        m_currentGroup = name;
        fseek(m_fp.Get(), 0L, SEEK_SET);
        fprintf(m_fp.Get(), "%s", m_outputBuffer.c_str());
        fflush(m_fp.Get());

    }

    virtual bool Configure()
    {
        m_fp.Reset(fopen (m_filename.c_str(), "w"));
        if (!m_fp) {
            LogPedantic("Could not open file " << m_filename << " for writing");
            return false;
        }
        return true;
    }

    virtual std::string CollectorSpecificHelp() const
    {
        return "--file=<filename> - name of file for output\n"
               "                    default - results.xml\n";
    }

    virtual void Start()
    {
        Assert(!!m_fp && "File handle must not be null");
        m_outputBuffer.append("<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n");
        m_outputBuffer.append("<testsuites>\n</testsuites>");
        fseek(m_fp.Get(), 0L, SEEK_SET);
        fprintf(m_fp.Get(), "%s", m_outputBuffer.c_str());
        fflush(m_fp.Get());

    }

    virtual void Finish()
    {
        // Show result
        FOREACH(group, m_groupsStats) {
            PrintStats(group->first, group->second);
            size_t posBegin = m_outputBuffer.find("<testcase name=\"unknown\"");
            size_t posEnd = m_outputBuffer.find("</testcase>", posBegin);
            m_outputBuffer.erase(posBegin - 3, posEnd - posBegin + sizeof("</testcase>") + 2);
        }


        remove(m_filename.c_str());
        m_fp.Reset(fopen (m_filename.c_str(), "w"));
        Assert(!!m_fp && "File handle must not be null");
        fseek(m_fp.Get(), 0L, SEEK_SET);
        fprintf(m_fp.Get(),"%s", m_outputBuffer.c_str());
        fflush(m_fp.Get());
    }

    virtual bool ParseCollectorSpecificArg(const std::string& arg)
    {
        return ParseCollectorFileArg(arg, m_filename);
    }

    virtual void CollectResult(const std::string& id,
                               const std::string& /*description*/,
                               const FailStatus::Type status = FailStatus::NONE,
                               const std::string& reason = "")
    {
        m_resultBuffer.erase();
        m_resultBuffer.append("\t\t<testcase name=\"");
        m_resultBuffer.append(EscapeSpecialCharacters(id));
        m_resultBuffer.append("\"");
        switch(status) {
            case TestResultsCollectorBase::FailStatus::NONE:
                m_resultBuffer.append(" status=\"OK\"/>\n");
                break;
            case TestResultsCollectorBase::FailStatus::FAILED:
                m_resultBuffer.append(" status=\"FAILED\">\n\t\t\t<failure");
                PrintfErrorMessage("FAILED", EscapeSpecialCharacters(reason), true);
                m_resultBuffer.append("\t\t</testcase>\n");
                break;
            case TestResultsCollectorBase::FailStatus::IGNORED:
                m_resultBuffer.append(" status=\"Ignored\">\n");
                PrintfIgnoredMessage("Ignored", EscapeSpecialCharacters(reason), true);
                m_resultBuffer.append("\t\t</testcase>\n");
                break;
            case TestResultsCollectorBase::FailStatus::INTERNAL:
                m_resultBuffer.append(" status=\"INTERNAL\">\n\t\t\t<failure");
                PrintfErrorMessage("INTERNAL", EscapeSpecialCharacters(reason), true);
                m_resultBuffer.append("\t\t</testcase>");
                break;
            default:
                Assert(false && "Bad status");
        }
        size_t group_pos = m_outputBuffer.find(m_currentGroup);
        size_t last_case_pos = m_outputBuffer.find("<testcase name=\"unknown\"", group_pos);
        m_outputBuffer.insert(last_case_pos - 2, m_resultBuffer);
        fseek(m_fp.Get(), 0L, SEEK_SET);
        fprintf(m_fp.Get(), "%s", m_outputBuffer.c_str());
        fflush(m_fp.Get());
        m_groupsStats[m_currentGroup].AddTest(status);
        m_stats.AddTest(status);

    }

    void PrintfErrorMessage(const char* type,
                            const std::string& message,
                            bool verbosity)
    {
        if (verbosity) {
            m_resultBuffer.append(" type=\"");
            m_resultBuffer.append(EscapeSpecialCharacters(type));
            m_resultBuffer.append("\" message=\"");
            m_resultBuffer.append(EscapeSpecialCharacters(message));
            m_resultBuffer.append("\"/>\n");

        } else {

            m_resultBuffer.append(" type=\"");
            m_resultBuffer.append(EscapeSpecialCharacters(type));
            m_resultBuffer.append("\"/>\n");
        }
    }

    void PrintfIgnoredMessage(const char* type,
                              const std::string& message,
                              bool verbosity)
    {
        if (verbosity) {
            m_resultBuffer.append("\t\t\t<skipped type=\"");
            m_resultBuffer.append(EscapeSpecialCharacters(type));
            m_resultBuffer.append("\" message=\"");
            m_resultBuffer.append(EscapeSpecialCharacters(message));
            m_resultBuffer.append("\"/>\n");
        } else {

            m_resultBuffer.append("\t\t\t<skipped type=\"");
            m_resultBuffer.append(EscapeSpecialCharacters(type));
            m_resultBuffer.append("\"/>\n");
        }
    }

    void PrintStats(const std::string& name, const Statistic& stats)
    {
        std::stringstream totalStats;
        totalStats << " tests=\"";
        totalStats << stats.GetTotal();
        totalStats << "\" failures=\"";
        totalStats << stats.GetFailed();
        totalStats << "\" skipped=\"";
        totalStats << stats.GetIgnored();
        totalStats << "\"";
        std::string suiteHeader;
        suiteHeader.append("<testsuite name=\"");
        suiteHeader.append(EscapeSpecialCharacters(name));
        size_t pos = m_outputBuffer.find(suiteHeader);
        m_outputBuffer.insert(pos+suiteHeader.size()+1,totalStats.str());
    }

    std::string EscapeSpecialCharacters(std::string s)
    {
        for(unsigned int i = 0; i < s.size();){
            switch(s[i]){
            case '"':
                s.erase(i,1);
                s.insert(i, "&quot;");
                i+=6;
                break;

            case '&':
                s.erase(i,1);
                s.insert(i, "&amp;");
                i+=5;
                break;

            case '<':
                s.erase(i,1);
                s.insert(i, "&lt;");
                i+=4;
                break;

            case '>':
                s.erase(i,1);
                s.insert(i, "&gt;");
                i+=4;
                break;

            case '\'':
                s.erase(i,1);
                s.insert(i, "&#39;");
                i+=5;
                break;
            default:
                ++i;
                break;
            }
        }
        return s;
    }

    std::string m_filename;
    ScopedFClose m_fp;
    Statistic m_stats;
    std::string m_currentGroup;
    std::map<std::string, Statistic> m_groupsStats;
    std::string m_outputBuffer;
    std::string m_resultBuffer;
};

TestResultsCollectorBase* XmlCollector::Constructor()
{
    return new XmlCollector();
}



class CSVCollector
    : public TestResultsCollectorBase
{
  public:
    static TestResultsCollectorBase* Constructor();

  private:
    CSVCollector() {}

    virtual void Start() {
        printf("GROUP;ID;RESULT;REASON\n");
    }

    virtual void CollectCurrentTestGroupName(const std::string& name)
    {
        m_currentGroup = name;
    }

    virtual void CollectResult(const std::string& id,
                               const std::string& /*description*/,
                               const FailStatus::Type status = FailStatus::NONE,
                               const std::string& reason = "")
    {
        std::string statusMsg = "";
        switch(status) {
            case TestResultsCollectorBase::FailStatus::NONE: statusMsg = "OK"; break;
            case TestResultsCollectorBase::FailStatus::FAILED: statusMsg = "FAILED"; break;
            case TestResultsCollectorBase::FailStatus::IGNORED: statusMsg = "IGNORED"; break;
            case TestResultsCollectorBase::FailStatus::INTERNAL: statusMsg = "FAILED"; break;
            default:
                Assert(false && "Bad status");
        }
        printf("%s;%s;%s;%s\n",
               m_currentGroup.c_str(),
               id.c_str(),
               statusMsg.c_str(),
               reason.c_str());
    }

    std::string m_currentGroup;
};


TestResultsCollectorBase* CSVCollector::Constructor()
{
    return new CSVCollector();
}

}

class TAPCollector
    : public TestResultsCollectorBase
{
  public:
    static TestResultsCollectorBase* Constructor();

  private:
    TAPCollector() : m_filename(DEFAULT_TAP_FILE_NAME)  {}

    virtual bool Configure()
    {
        m_output.open(m_filename.c_str(), std::ios_base::trunc);
        if (m_output.fail()) {
            LogError("Can't open output file: " << m_filename);
            return false;
        }
        return true;
    }
    virtual std::string CollectorSpecificHelp() const
    {
        std::string retVal = "--file=<filename> - name of file for output\n"
                             "                    default - ";
        retVal += DEFAULT_TAP_FILE_NAME;
        retVal += "\n";
        return retVal;
    }

    virtual void Start()
    {
        Assert(m_output.good() && "Output file must be opened.");
        m_output << "TAP version 13" << std::endl;
        m_testIndex = 0;
    }

    virtual void Finish()
    {
        m_output << "1.." << m_testIndex << std::endl;
        m_output << m_collectedData.rdbuf();
        m_output.close();
    }

    virtual bool ParseCollectorSpecificArg(const std::string& arg)
    {
        return ParseCollectorFileArg(arg, m_filename);
    }

    virtual void CollectResult(const std::string& id,
                               const std::string& description,
                               const FailStatus::Type status = FailStatus::NONE,
                               const std::string& reason = "")
    {
        m_testIndex++;
        switch(status) {
            case TestResultsCollectorBase::FailStatus::NONE:
                LogBasicTAP(true, id, description);
                endTAPLine();
                break;
            case TestResultsCollectorBase::FailStatus::FAILED:
                LogBasicTAP(false, id, description);
                endTAPLine();
                break;
            case TestResultsCollectorBase::FailStatus::IGNORED:
                LogBasicTAP(true, id, description);
                m_collectedData << " # skip " << reason;
                endTAPLine();
                break;
            case TestResultsCollectorBase::FailStatus::INTERNAL:
                LogBasicTAP(true, id, description);
                endTAPLine();
                m_collectedData << "    ---" << std::endl;
                m_collectedData << "    message: " << reason << std::endl;
                m_collectedData << "    severity: Internal" << std::endl;
                m_collectedData << "    ..." << std::endl;
                break;
            default:
                Assert(false && "Bad status");
        }
    }

    void LogBasicTAP(bool isOK, const std::string& id,
            const std::string& description)
    {
        if (!isOK) {
            m_collectedData << "not ";
        }
        m_collectedData << "ok " << m_testIndex << " [" <<
                            id << "] " << description;
    }

    void endTAPLine()
    {
        m_collectedData << std::endl;
    }


    std::string m_filename;
    std::stringstream m_collectedData;
    std::ofstream m_output;
    int m_testIndex;
};


TestResultsCollectorBase* TAPCollector::Constructor()
{
    return new TAPCollector();
}


void TestResultsCollectorBase::RegisterCollectorConstructor(
    const std::string& name,
    TestResultsCollectorBase::CollectorConstructorFunc func)
{
    Assert(m_constructorsMap.find(name) == m_constructorsMap.end());
    m_constructorsMap[name] = func;
}

TestResultsCollectorBase* TestResultsCollectorBase::Create(
    const std::string& name)
{
    ConstructorsMap::iterator found = m_constructorsMap.find(name);
    if (found != m_constructorsMap.end())
        return found->second();
    else
        return NULL;
}

std::vector<std::string> TestResultsCollectorBase::GetCollectorsNames()
{
    std::vector<std::string> list;
    FOREACH(it, m_constructorsMap)
    {
        list.push_back(it->first);
    }
    return list;
}

TestResultsCollectorBase::ConstructorsMap TestResultsCollectorBase::m_constructorsMap;

namespace
{
static int RegisterCollectorConstructors();
static const int RegisterHelperVariable = RegisterCollectorConstructors();
int RegisterCollectorConstructors()
{
    (void)RegisterHelperVariable;

    TestResultsCollectorBase::RegisterCollectorConstructor(
        "text",
        &ConsoleCollector::Constructor);
    TestResultsCollectorBase::RegisterCollectorConstructor(
        "html",
        &HtmlCollector::Constructor);
    TestResultsCollectorBase::RegisterCollectorConstructor(
        "csv",
        &CSVCollector::Constructor);
    TestResultsCollectorBase::RegisterCollectorConstructor(
        "tap",
        &TAPCollector::Constructor);
    TestResultsCollectorBase::RegisterCollectorConstructor(
        "xml",
        &XmlCollector::Constructor);

    return 0;
}

}

}
}
#undef GREEN_RESULT_OK
