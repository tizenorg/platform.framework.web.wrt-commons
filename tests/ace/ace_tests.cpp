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
 * @file        main.cpp
 * @author      Bartlomiej Grzelewski <b.grzelewski@samsung.com>
 * @version     1.0
 * @brief       This file is the implementation file of main
 */
#include <string>

#include <dpl/test/test_runner.h>
#include <dpl/log/log.h>

#include <loop_control.h>

#include <dpl/ace-dao-rw/AceDAO.h>

#include "Interfaces.h"
#include "PEPSingleton.h"


using namespace LoopControl;

class UnitTestThread
{
public:
    UnitTestThread()
    {
        // Attach databases
        AceDB::AceDAO::attachToThread();
    }

    ~UnitTestThread()
    {
        // Detach databases
        AceDB::AceDAO::detachFromThread();
    }
};

int main (int argc, char *argv[])
{
    init_loop(argc, argv);

    LogInfo("Initializing PEPSingleton.");

    UnitTestThread attacher;

    PEPSingleton::Instance().initialize(
        new WebRuntimeImp(),
        new ResourceInformationImp(),
        new OperationSystemImp());

    LogInfo("Starting tests.");

    int status = DPL::Test::TestRunnerSingleton::Instance().ExecTestRunner(argc, argv);

    PEPSingleton::Instance().terminate();

    quit_loop();
    return status;
}

