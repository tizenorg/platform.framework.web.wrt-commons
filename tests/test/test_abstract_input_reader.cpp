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
 * @file        test_abstract_input_reader.h
 * @author      Tomasz Iwanek (t.iwanek@samsung.com)
 * @brief       tests for AbstractInputReader
 */

#include <cstring>
#include <string>

#include <dpl/test/abstract_input_reader.h>
#include <dpl/abstract_input.h>
#include <dpl/test/test_runner.h>
#include <dpl/binary_queue.h>
#include <dpl/optional.h>

using namespace DPL;

namespace {

// TOKENIZER

class DigitTokenizer : public AbstractInputTokenizer<int>
{
public:
    DigitTokenizer() : m_valid(true) {}

    std::unique_ptr<int> GetNextToken()
    {
        typedef AbstractInputTokenizer<int>::Exception::TokenizerError TokenizerError;

        std::unique_ptr<int> token;

        char buffer;
        BinaryQueueAutoPtr baptr = m_input->Read(1); //not effective but it's test...
        if(baptr.get() == NULL)
        {
            ThrowMsg(TokenizerError, "Input reading failed");
        }
        if(baptr->Empty()) //end of source
        {
            return token;
        }
        baptr->FlattenConsume(&buffer,1);
        if(!isdigit(buffer))
        {
            ThrowMsg(TokenizerError, "Input source contains no digit characters/bytes");
        }
        token.reset(new int(static_cast<int>(buffer)));
        return token;
    }

    void Reset(std::shared_ptr<AbstractInput> ia)
    {
        AbstractInputTokenizer<int>::Reset(ia);
        m_valid = true;
    }

    bool IsStateValid()
    {
        return true;
    }

private:
    bool m_valid;
};

// PARSER

class SumatorParser : public AbstractInputParser<int, int>
{
public:
    SumatorParser() : m_sum(0) {}

    void ConsumeToken(std::unique_ptr<int> && token)
    {
        m_sum += (*token - '0');
    }

    bool IsStateValid()
    {
        return true;
    }

    int GetResult() const
    {
        return m_sum;
    }

private:
    int m_sum;
};

// READER

class Sumator : public AbstractInputReader<int, int>
{
public:
    Sumator(std::shared_ptr<AbstractInput> ia)
        : AbstractInputReader<int, int>(ia,
            std::unique_ptr<ParserBase>(new SumatorParser()),
            std::unique_ptr<TokenizerBase>(new DigitTokenizer()))
    {}
};

}

RUNNER_TEST_GROUP_INIT(AbstractInputReader)

RUNNER_TEST(AbstractInputReader_ByteSumatorInstance_Sum)
{
    const std::string data("1234567890");
    std::shared_ptr<AbstractInput> mem(new BinaryQueue());
    dynamic_cast<BinaryQueue*>(mem.get())->AppendCopy(data.c_str(), data.size());
    Sumator sum(mem);
    int result = sum.ReadInput();
    RUNNER_ASSERT_MSG(result == 45, "Sum is invalid");
}

RUNNER_TEST(AbstractInputReader_ByteSumatorInstance_Exception)
{
    const std::string data("12345string90");
    std::shared_ptr<AbstractInput> mem(new BinaryQueue());
    dynamic_cast<BinaryQueue*>(mem.get())->AppendCopy(data.c_str(), data.size());
    Sumator sum(mem);
    Try
    {
        sum.ReadInput();
    }
    Catch(Sumator::Exception::TokenizerError)
    {
        return;
    }
    RUNNER_ASSERT_MSG(false, "Tokenizer exception should be thrown");
}
