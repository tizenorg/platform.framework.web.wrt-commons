/*
 * Copyright (c) 2012 Samsung Electronics Co., Ltd All Rights Reserved
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
/**
 * @file    tests_miscunit.cpp
 * @author  Marcin Kaminski (marcin.ka@samsung.com)
 * @version 1.0
 * @brief   This is main file for miscellaneous unit tests (tests of different
 *          classes, namespaces and functions).
 */

#include <dpl/test/test_runner.h>
#include <dpl/log/log.h>

int main (int argc, char *argv[])
{
    LogInfo("Starting miscellaneous unit tests");
    int status = DPL::Test::TestRunnerSingleton::Instance().ExecTestRunner(argc, argv);

    return status;
}
