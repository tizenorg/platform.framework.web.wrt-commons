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
 * @author      Przemyslaw Dobrowolski (p.dobrowolsk@samsung.com)
 * @version     1.0
 * @brief       This file is the implementation file of main
 */
#include <dpl/test/test_runner.h>
#include <dpl/log/log.h>
#include <dpl/wrt-dao-ro/WrtDatabase.h>

int main(int argc, char *argv[])
{

    int ret = system("/usr/bin/wrt_db_localization_prepare.sh start");
    if (ret != 0) {
        LogError("Preparation script has return error: " << ret
                                                         << ". Quitting");
        return -1;
    }

    WrtDB::WrtDatabase::attachToThreadRW();
    int status = DPL::Test::TestRunnerSingleton::Instance().ExecTestRunner(argc, argv);
    WrtDB::WrtDatabase::detachFromThread();

    ret = system("/usr/bin/wrt_db_localization_prepare.sh stop");
    if (ret != 0) {
        LogError("Preparation script has return error: " << ret
                                                         << ". Quitting");
        return -1;
    }
    return status;
}

