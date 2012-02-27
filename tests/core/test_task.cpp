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
 * @file        test_task.cpp
 * @author      Przemyslaw Dobrowolski (p.dobrowolsk@samsung.com)
 * @version     1.0
 * @brief       This file is the implementation file of task tests
 */
#include <dpl/test/test_runner.h>
#include <dpl/task.h>
#include <dpl/log/log.h>

RUNNER_TEST_GROUP_INIT(DPL)

class MySingleTask
    : public DPL::TaskDecl<MySingleTask>
{
protected:
    void StepOne()
    {
    }

public:
    MySingleTask()
        : DPL::TaskDecl<MySingleTask>(this)
    {
        AddStep(&MySingleTask::StepOne);
    }
};

class MyMultiTask
    : public DPL::MultiTaskDecl<MyMultiTask>
{
protected:
    typedef DPL::MultiTaskDecl<MyMultiTask> BaseType;

    void StepOne()
    {
        LogInfo("Step one");
    }

    void StepTwo()
    {
        LogInfo("Step two");
    }

    void StepThree()
    {
        LogInfo("Step three");
    }

public:
    MyMultiTask()
        : BaseType(this, 2)
    {
        BaseType::StepList depListStepThree;
        depListStepThree.push_back(&MyMultiTask::StepOne);
        depListStepThree.push_back(&MyMultiTask::StepTwo);
        AddStep(&MyMultiTask::StepThree, depListStepThree);

        BaseType::StepList depListStepTwo;
        depListStepTwo.push_back(&MyMultiTask::StepOne);
        AddStep(&MyMultiTask::StepTwo, depListStepTwo);

        BaseType::StepList depListStepOne;
        AddStep(&MyMultiTask::StepOne, depListStepOne);
    }
};

RUNNER_TEST(Task_SingleTask)
{
    MySingleTask task;
    while (task.NextStep());
}

RUNNER_TEST(Task_MultiTask)
{
    MyMultiTask task;
    while (task.NextStep());
}
