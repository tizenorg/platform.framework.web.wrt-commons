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
 * @file        test_process_pipe.cpp
 * @author      Tomasz Iwanek (t.iwanek@samsung.com)
 * @version     1.0
 * @brief       This file is the implementation file of ProcessPipe tests
 */

#include <dpl/test/test_runner.h>
#include <dpl/test/process_pipe.h>
#include <dpl/binary_queue.h>
#include <dpl/log/log.h>

#include <cstring>

using namespace DPL;

RUNNER_TEST_GROUP_INIT(DPL)

namespace {
void readAll(ProcessPipe & npp, BinaryQueue & result)
{
    do
    {
        BinaryQueueAutoPtr dataptr = npp.Read(4096);

        RUNNER_ASSERT_MSG(dataptr.get() != NULL, "Cannot read from pipe subprocess");

        LogDebug("Size: " << dataptr->Size());

        if(dataptr->Empty()) break;
        result.AppendMoveFrom(*dataptr);
    }
    while(true);
}
}

RUNNER_TEST(ProcessPipe_echo)
{
    ProcessPipe npp;
    npp.Open("echo -e \"Test echo text\\nAnd new line\"");
    BinaryQueue result;
    readAll(npp, result);
    npp.Close();

    char buffer[100] = "";
    result.FlattenConsume(buffer, 99);

    RUNNER_ASSERT_MSG(strcmp(buffer, "Test echo text\nAnd new line\n") == 0, "Echoed text in not equal");
}

RUNNER_TEST(ProcessPipe_double_open)
{
    ProcessPipe npp;
    npp.Open("echo  \"Test \"");
    Try
    {
        npp.Open("echo \"Test\"");
    }
    Catch(DPL::ProcessPipe::Exception::DoubleOpen)
    {
        npp.Close();
        return;
    }
    npp.Close();
    RUNNER_ASSERT_MSG(false, "DoubleOpen not thrown");
}

RUNNER_TEST(ProcessPipe_double_close)
{
    ProcessPipe npp;
    npp.Open("echo  \"Test invalid\"");
    npp.Close();
    Try
    {
        npp.Close();
    }
    Catch(DPL::Exception)
    {
        RUNNER_ASSERT_MSG(false, "Second Close throws exception");
    }
}

RUNNER_TEST(ProcessPipe_pipeerror_off)
{
    ProcessPipe npp(ProcessPipe::PipeErrorPolicy::OFF);
    npp.Open("ls /nonexistingdirectory");
    BinaryQueue result;
    readAll(npp, result); //TODO: fix this test
    npp.Close();
}

RUNNER_TEST(ProcessPipe_pipeerror_pipe)
{
    //ls output dependent...
    ProcessPipe npp(ProcessPipe::PipeErrorPolicy::PIPE);
    npp.Open("ls /nonexistingdirectory");
    BinaryQueue result;
    readAll(npp, result);
    npp.Close();
    char buffer[100] = "";
    result.FlattenConsume(buffer, 99);

    RUNNER_ASSERT_MSG(strcmp(buffer, "ls: cannot access /nonexistingdirectory: No such file or directory\n") == 0, "Ls error text in not equal");
}
