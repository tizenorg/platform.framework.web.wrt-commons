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
 * @file        test_sql_connection.cpp
 * @author      Przemyslaw Dobrowolski (p.dobrowolsk@samsung.com)
 * @version     1.0
 * @brief       This file is the implementation file of sql connection tests
 */

/*
 *
 * This test has been saved from original test_sql_connection.cpp in wrt-commons
 * project.
 *
 */

#include <dpl/naive_synchronization_object.h>

RUNNER_TEST(SqlConnection_MassiveReadWrite_SemaphoreSynchronization)
{
    std::ostringstream dbSemaporeFileNameStream;
    unsigned int seed = time(NULL);

    dbSemaporeFileNameStream << "dpl_tests_dbso_sem_";
    dbSemaporeFileNameStream << rand_r(&seed) << ".sem";

    std::string dbSemaphoreFileName = dbSemaporeFileNameStream.str();

    SemaphoreSynchronizationObjectGenerator m_generator(dbSemaphoreFileName);
    MassiveReadWriteTest(&m_generator);
}
