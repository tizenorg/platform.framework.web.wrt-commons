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
 * @file        test_address.cpp
 * @author      Tomasz Swierczek (t.swierczek@samsung.com)
 * @version     1.0
 * @brief       This file is the implementation file of caller tests
 */

#include <dpl/test_runner.h>
#include <dpl/serialization.h>
#include <dpl/caller.h>

RUNNER_TEST_GROUP_INIT(DPL)

// test stream class
class BinaryStream : public DPL::IStream {
  public:
    virtual void Read(size_t num, void * bytes)
    {
        for (unsigned i = 0; i < num; ++i) {
            ((unsigned char*)bytes)[i] = data[i + readPosition];
        }
        readPosition += num;
    }
    virtual void Write(size_t num, const void * bytes)
    {
        for (unsigned i = 0; i < num; ++i) {
            data.push_back(((unsigned char*)bytes)[i]);
        }
    }
    BinaryStream()
    {
        readPosition = 0;
    }
    virtual ~BinaryStream(){};

  private:
    std::vector<unsigned char> data;
    unsigned readPosition;
};

static int return_func(int a, bool b)
{
    if (b) {
        return a;
    } else {
        return 0;
    }
}

static int called = 0;

static void void_func(int a)
{
    called = a;
}

static struct VoidDelegate
{
        void operator()(int a)
        {
            called = a;
        }
} voidDelegate;

static struct ReturnDelegate
{
        int operator()(int a)
        {
            return a;
        }
} returnDelegate;

RUNNER_TEST(Caller_function_void)
{
    int a = 23;
    BinaryStream stream;
    DPL::Serialization::Serialize(stream,a);
    called = 0;
    DPL::Caller::Call(stream,void_func);
    RUNNER_ASSERT(called == a);
}

RUNNER_TEST(Caller_function_return)
{
    int a = 23;
    bool b = true;
    BinaryStream stream;
    DPL::Serialization::Serialize(stream,a);
    DPL::Serialization::Serialize(stream,b);
    int result = DPL::Caller::Call(stream,return_func);
    RUNNER_ASSERT(result == a);
}

RUNNER_TEST(Caller_delegate_void)
{
    int a = 23;
    BinaryStream stream;
    called = 0;
    DPL::Serialization::Serialize(stream,a);
    DPL::Caller::CallDelegate(stream,voidDelegate);
    RUNNER_ASSERT(called == a);
}

RUNNER_TEST(Caller_delegate_return)
{
    int a = 23;
    BinaryStream stream;
    called = 0;
    DPL::Serialization::Serialize(stream,a);
    int result = 0;
    DPL::Caller::CallDelegate(stream,returnDelegate,result);
    RUNNER_ASSERT(result == a);
}
