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
 * @file        test_scoped_dir.cpp
 * @author      Iwanek Tomasz (t.iwanek@smasung.com)
 * @version     1.0
 * @brief       Scoped directory test
 */
#include <dpl/test/test_runner.h>
#include <dpl/scoped_dir.h>

#include <cstdio>
#include <sstream>
#include <cstdlib>

#include <unistd.h>

RUNNER_TEST_GROUP_INIT(DPL)

/*
Name: ScopedDir_Basic
Description: tests if scoped directory is working
Expected: directory created and removed
*/
RUNNER_TEST(ScopedDir_Basic)
{
    const char * path = "/tmp/wrttest123456";
    if(access(path, F_OK) == 0)
    {
        RUNNER_ASSERT_MSG(!remove(path), "Cannot remove test directory");
    }

    {
        DPL::ScopedDir dir(path, S_IRUSR | S_IWUSR);
        std::ostringstream command;
        command << "touch " << path << "/" << "file.txt";
        (void)system(command.str().c_str());
        RUNNER_ASSERT_MSG(access(path, R_OK) == 0, "Directory should be accessible");
        RUNNER_ASSERT_MSG(access(path, W_OK) == 0, "Directory should be writable");
    }
    RUNNER_ASSERT_MSG(access(path, F_OK) != 0, "Directory should not exists");
}
