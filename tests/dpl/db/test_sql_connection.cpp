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
#include <dpl/test/test_runner.h>
#include <dpl/db/sql_connection.h>
#include <dpl/db/naive_synchronization_object.h>
#include <memory>
#include <dpl/log/log.h>
#include <sstream>
#include <string>
#include <cstdlib>
#include <ctime>

extern const char* PATH_DB;

RUNNER_TEST_GROUP_INIT(DPL)

class AbstractSynchronizationObjectGenerator
{
public:
    virtual ~AbstractSynchronizationObjectGenerator() {}

    virtual DPL::DB::SqlConnection::SynchronizationObject *Create() = 0;
};

class NaiveSynchronizationObjectGenerator
    : public AbstractSynchronizationObjectGenerator
{
public:
    virtual DPL::DB::SqlConnection::SynchronizationObject *Create()
    {
        return new DPL::DB::NaiveSynchronizationObject();
    }
};

void MassiveReadWriteTest(AbstractSynchronizationObjectGenerator *generator);

class StressGenerator
    : public DPL::Thread
{
private:
    size_t m_prefix;
    std::string m_dbFileName;
    AbstractSynchronizationObjectGenerator *m_generator;

protected:
    virtual int ThreadEntry()
    {
        DPL::DB::SqlConnection connection(
            m_dbFileName,
            DPL::DB::SqlConnection::Flag::None,
            DPL::DB::SqlConnection::Flag::RW,
            m_generator->Create());

        DPL::DB::SqlConnection::DataCommandAutoPtr countCommand =
            connection.PrepareDataCommand(
                "SELECT COUNT(*) FROM test WHERE value=?");

        for (size_t i = 0; i < 10; ++i)
        {
            std::ostringstream valueStream;

            valueStream << "value_";
            valueStream << static_cast<unsigned long>(m_prefix);
            valueStream << "_";
            valueStream << static_cast<unsigned long>(i);

            std::string value = valueStream.str();

            connection.ExecCommand(
                "INSERT INTO test VALUES ('%s');",
                value.c_str());

            countCommand->BindString(1, value.c_str());

            RUNNER_ASSERT(countCommand->Step());

            RUNNER_ASSERT(countCommand->GetColumnString(0) == "1");

            countCommand->Reset();
        }

        countCommand.reset();

        return 0;
    }

public:
    StressGenerator(size_t prefix,
                    const std::string &dbFileName,
                    AbstractSynchronizationObjectGenerator *generator)
        : m_prefix(prefix),
          m_dbFileName(dbFileName),
          m_generator(generator)
    {
    }
};

typedef std::shared_ptr<DPL::Thread> ThreadPtr;

void MassiveReadWriteTest(AbstractSynchronizationObjectGenerator *generator)
{
    DPL::DB::SqlConnection connection(PATH_DB,
                                      DPL::DB::SqlConnection::Flag::UseLucene,
                                      DPL::DB::SqlConnection::Flag::RW);

    connection.ExecCommand("CREATE TABLE test(value TEXT);");

    const size_t STRESS_GENERATOR_COUNT = 5;
    ThreadPtr stressGenerators[STRESS_GENERATOR_COUNT];

    for (size_t i = 0; i < STRESS_GENERATOR_COUNT; ++i)
    {
        stressGenerators[i].reset(
            new StressGenerator(i, PATH_DB, generator));

        stressGenerators[i]->Run();
    }

    for (size_t i = 0; i < STRESS_GENERATOR_COUNT; ++i)
        stressGenerators[i]->Quit();

    connection.ExecCommand("DROP TABLE test;");
}

RUNNER_TEST(SqlConnection_MassiveReadWrite_NaiveSynchronization)
{
    srand(time(NULL));

    NaiveSynchronizationObjectGenerator m_generator;
    MassiveReadWriteTest(&m_generator);
}






RUNNER_TEST(SqlConnection_Not_Connected_Lucene)
{
    Try {
        DPL::DB::SqlConnection connection("/notexisitingdirectiory/foo",
                                          DPL::DB::SqlConnection::Flag::UseLucene,
                                          DPL::DB::SqlConnection::Flag::RW);
        RUNNER_ASSERT_MSG(false,
                          "connection should throw on accessing "
                          "nonexistent file as a database");
    }
    Catch (DPL::DB::SqlConnection::Exception::ConnectionBroken)
    {
        RUNNER_ASSERT(true);
    }
    catch (DPL::Exception)
    {
        RUNNER_ASSERT_MSG(false, "Wrong exception found");
    }
}

RUNNER_TEST(SqlConnection_Not_Connected)
{
    Try {
        DPL::DB::SqlConnection connection("/notexisitingdirectiory/foo",
                                          DPL::DB::SqlConnection::Flag::None,
                                          DPL::DB::SqlConnection::Flag::RW);
        RUNNER_ASSERT_MSG(false,
                          "connection should throw on accessing "
                          "nonexistent file as a database");
    }
    Catch (DPL::DB::SqlConnection::Exception::ConnectionBroken)
    {
        RUNNER_ASSERT(true);
    }
    catch (DPL::Exception)
    {
        RUNNER_ASSERT_MSG(false, "Wrong exception found");
    }
}

RUNNER_TEST(SqlConnection_Null_Query)
{
    DPL::DB::SqlConnection connection(PATH_DB,
                                      DPL::DB::SqlConnection::Flag::UseLucene,
                                      DPL::DB::SqlConnection::Flag::RW);
    Try
    {
        connection.ExecCommand(NULL);
        RUNNER_ASSERT_MSG(false,
                          "Null pointer should not be accepted");
    }
    Catch (DPL::DB::SqlConnection::Exception::SyntaxError)
    {
        RUNNER_ASSERT(true);
    }
    catch (DPL::Exception)
    {
        RUNNER_ASSERT_MSG(false, "Wrong exception found");
    }

}

RUNNER_TEST(SqlConnection_Bad_Query)
{
    DPL::DB::SqlConnection connection(PATH_DB,
                                      DPL::DB::SqlConnection::Flag::UseLucene,
                                      DPL::DB::SqlConnection::Flag::RW);
    Try
    {
        connection.ExecCommand("Some stupid string");
        RUNNER_ASSERT_MSG(false, "This string should not be accepted");
    }
    Catch (DPL::DB::SqlConnection::Exception::SyntaxError)
    {
        RUNNER_ASSERT(true);
    }
    catch (DPL::Exception)
    {
        RUNNER_ASSERT_MSG(false, "Wrong exception found");
    }
}
