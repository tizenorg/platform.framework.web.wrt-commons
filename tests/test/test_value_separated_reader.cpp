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
 * @file        test_value_separated_reader.cpp
 * @author      Tomasz Iwanek (t.iwanek@samsung.com)
 * @brief       tests for VSReader
 */

#include<dpl/test/value_separated_reader.h>
#include<dpl/abstract_input.h>
#include<dpl/test/test_runner.h>

#include<string>

using namespace  DPL;

RUNNER_TEST_GROUP_INIT(ValueSeparatedReader)

RUNNER_TEST(ValueSeparatedReader_readValidCSV)
{
    std::string data;
    data += "1-1,1-2,1-3,1-4\n";
    data += "2-1,2-2,2-3,2-4\n";
    data += "3-1,3-2,3-3,3-4\n";
    data += "4-1,4-2,4-3,4-4\n";
    std::shared_ptr<AbstractInput> mem(new BinaryQueue());
    dynamic_cast<BinaryQueue*>(mem.get())->AppendCopy(data.data(), data.size());
    CSVReader csv(mem);
    VSResultPtr result = csv.ReadInput();
    RUNNER_ASSERT_MSG((*result)[0][0] == "1-1", "Wrong value");
    RUNNER_ASSERT_MSG((*result)[0][1] == "1-2", "Wrong value");
    RUNNER_ASSERT_MSG((*result)[0][2] == "1-3", "Wrong value");
    RUNNER_ASSERT_MSG((*result)[0][3] == "1-4", "Wrong value");

    RUNNER_ASSERT_MSG((*result)[1][0] == "2-1", "Wrong value");
    RUNNER_ASSERT_MSG((*result)[1][1] == "2-2", "Wrong value");
    RUNNER_ASSERT_MSG((*result)[1][2] == "2-3", "Wrong value");
    RUNNER_ASSERT_MSG((*result)[1][3] == "2-4", "Wrong value");

    RUNNER_ASSERT_MSG((*result)[2][0] == "3-1", "Wrong value");
    RUNNER_ASSERT_MSG((*result)[2][1] == "3-2", "Wrong value");
    RUNNER_ASSERT_MSG((*result)[2][2] == "3-3", "Wrong value");
    RUNNER_ASSERT_MSG((*result)[2][3] == "3-4", "Wrong value");

    RUNNER_ASSERT_MSG((*result)[3][0] == "4-1", "Wrong value");
    RUNNER_ASSERT_MSG((*result)[3][1] == "4-2", "Wrong value");
    RUNNER_ASSERT_MSG((*result)[3][2] == "4-3", "Wrong value");
    RUNNER_ASSERT_MSG((*result)[3][3] == "4-4", "Wrong value");
}

RUNNER_TEST(ValueSeparatedReader_readInvalidCSV)
{
    Try
    {
        std::string data;
        data += "1-1,1-2,1-3,1-4\n";
        data += "2-1,2-2,2-3,2-4\n";
        data += "3-1,3-2,3-3\n";
        data += "4-1,4-2,4-3,4-4\n";
        std::shared_ptr<AbstractInput> mem(new BinaryQueue());
        dynamic_cast<BinaryQueue*>(mem.get())->AppendCopy(data.data(), data.size());
        CSVReader csv(mem);
        VSResultPtr result = csv.ReadInput();
    }
    Catch(CSVReader::Exception::ParserError)
    {
        return;
    }
    RUNNER_ASSERT_MSG(false, "Should throw parser error");
}
