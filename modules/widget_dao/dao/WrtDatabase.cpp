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

#include <dpl/wrt-dao-ro/WrtDatabase.h>

#include <dpl/db/thread_database_support.h>
#include <dpl/db/sql_connection.h>
#include <dpl/mutex.h>
#include <dpl/thread.h>
#include <dpl/wrt-dao-ro/global_config.h>

namespace WrtDB {

const char* WrtDatabase::Address()
{
    using namespace WrtDB;
    return GlobalConfig::GetWrtDatabaseFilePath();
}

DPL::DB::SqlConnection::Flag::Type WrtDatabase::Flags()
{
    return DPL::DB::SqlConnection::Flag::UseLucene;
}

DPL::DB::ThreadDatabaseSupport WrtDatabase::m_interface(
        WrtDatabase::Address(),
        WrtDatabase::Flags());

void WrtDatabase::attachToThread()
{
    m_interface.AttachToThread();
}

void WrtDatabase::detachFromThread()
{
    m_interface.DetachFromThread();
}

DPL::DB::ThreadDatabaseSupport& WrtDatabase::interface()
{
    return m_interface;
}

bool WrtDatabase::CheckTableExist(const char *name)
{
    return m_interface.CheckTableExist(name);
}

}
