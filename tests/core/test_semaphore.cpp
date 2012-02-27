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
 * @file        test_semaphore.cpp
 * @author      Przemyslaw Dobrowolski (p.dobrowolsk@samsung.com)
 * @version     1.0
 * @brief       This file is the implementation file of semaphore tests
 */
#include <dpl/test/test_runner.h>
#include <dpl/lexical_cast.h>
#include <dpl/semaphore.h>
#include <dpl/thread.h>
#include <dpl/log/log.h>
#include <string>
#include <ctime>

RUNNER_TEST_GROUP_INIT(DPL)

class SemaphoreThread
    : public DPL::Thread
{
    int m_delta;
    int m_times;
    int *m_value;
    std::string m_semaphoreName;

public:
    SemaphoreThread(int delta,
                    int times,
                    int *value,
                    const std::string &semaphoreName)
        : m_delta(delta),
          m_times(times),
          m_value(value),
          m_semaphoreName(semaphoreName)
    {
    }

protected:
    virtual int ThreadEntry()
    {
        DPL::Semaphore semaphore(m_semaphoreName);

        for (int i = 0; i < m_times; ++i)
        {
            // Take scoped semaphore lock
            DPL::Semaphore::ScopedLock lock(&semaphore);
            *m_value += m_delta;
        }

        return 0;
    }
};

RUNNER_TEST(Semaphore_NamedIncrementDecrement)
{
    std::string semaphoreName =
        "dpl_test_semaphore_" +
        DPL::lexical_cast<std::string>(std::time(NULL));

    int value = 0;
    SemaphoreThread threadA(-1, 10000, &value, semaphoreName);
    SemaphoreThread threadB(+1, 10000, &value, semaphoreName);

    threadA.Run();
    threadB.Run();

    threadA.Quit();
    threadB.Quit();

    RUNNER_ASSERT_MSG(value == 0, "Final value is: " << value);
}
