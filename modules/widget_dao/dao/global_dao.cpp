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
/**
 * @file   global_dao.cpp
 * @author  Pawel Sikorski (p.sikorski@samsung.com)
 * @version 1.0
 * @brief   This file contains the definition of global dao class.
 */
#include <stddef.h>
#include <dpl/wrt-dao-rw/global_dao.h>
#include <dpl/log/log.h>
#include <dpl/string.h>
#include <dpl/db/orm.h>
#include <orm_generator_wrt.h>
#include <dpl/wrt-dao-ro/webruntime_database.h>
#include <dpl/wrt-dao-ro/WrtDatabase.h>

namespace WrtDB {
void GlobalDAO::SetRoamingDataUsage(GlobalDAO::NetworkAccessMode newMode)
{
    LogDebug("updating roaming network data usage to:" << newMode);
    Try {
        using namespace DPL::DB::ORM;
        using namespace DPL::DB::ORM::wrt;
        GlobalProperties::Row row;
        row.Set_roaming_data_usage(static_cast<int>(newMode));

        WRT_DB_UPDATE(update, GlobalProperties, &WrtDatabase::interface())
        update->Values(row);
        update->Execute();
    }
    Catch(DPL::DB::SqlConnection::Exception::Base){
        ReThrowMsg(GlobalDAO::Exception::DatabaseError,
                   "Failed to update roaming network data usage");
    }
}

} // namespace WrtDB
