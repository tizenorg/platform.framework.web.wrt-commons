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
void GlobalDAO::SetSecureByDefault(bool secure)
{
    //If secure == true -> widget does not need to be signed
    Try {
        using namespace DPL::DB::ORM;
        using namespace DPL::DB::ORM::wrt;
        GlobalProperties::Row row;
        row.Set_secure_by_default(secure);
        WRT_DB_UPDATE(update, GlobalProperties, &WrtDatabase::interface())
        update->Values(row);
        update->Execute();
    }
    Catch(DPL::DB::SqlConnection::Exception::Base) {
        ReThrowMsg(GlobalDAO::Exception::DatabaseError,
                   "Failed to update secureByDefault");
    }
}

void GlobalDAO::setComplianceMode(bool mode)
{
    LogDebug("Updating compliance mode to:" << mode);
    Try {
        using namespace DPL::DB::ORM;
        using namespace DPL::DB::ORM::wrt;
        GlobalProperties::Row row;
        row.Set_compliance_mode(mode);

        WRT_DB_UPDATE(update, GlobalProperties, &WrtDatabase::interface())
        update->Values(row);
        update->Execute();
    }
    Catch(DPL::DB::SqlConnection::Exception::Base){
        ReThrowMsg(GlobalDAO::Exception::DatabaseError,
                   "Failed to update compliance mode");
    }
}

void GlobalDAO::setComplianceFakeImei(const std::string &imei)
{
    LogDebug("Setting compliance fake IMEI: " << imei);
    Try {
        using namespace DPL::DB::ORM;
        using namespace DPL::DB::ORM::wrt;
        GlobalProperties::Row row;
        row.Set_compliance_fake_imei(DPL::FromASCIIString(imei));

        WRT_DB_UPDATE(update, GlobalProperties, &WrtDatabase::interface())
        update->Values(row);
        update->Execute();
    }
    Catch(DPL::DB::SqlConnection::Exception::Base){
        ReThrowMsg(GlobalDAO::Exception::DatabaseError,
                   "Failed to update compliance fake IMEI");
    }
}

void GlobalDAO::setComplianceFakeMeid(const std::string &meid)
{
    LogDebug("Setting compliance fake MEID: " << meid);
    Try {
        using namespace DPL::DB::ORM;
        using namespace DPL::DB::ORM::wrt;
        GlobalProperties::Row row;
        row.Set_compliance_fake_meid(DPL::FromASCIIString(meid));

        WRT_DB_UPDATE(update, GlobalProperties, &WrtDatabase::interface())
        update->Values(row);
        update->Execute();
    }
    Catch(DPL::DB::SqlConnection::Exception::Base){
        ReThrowMsg(GlobalDAO::Exception::DatabaseError,
                   "Failed to update compliance fake MEID");
    }
}

void GlobalDAO::SetHomeNetworkDataUsage(GlobalDAO::NetworkAccessMode newMode)
{
    LogDebug("updating home network data usage to:" << newMode);
    Try {
        using namespace DPL::DB::ORM;
        using namespace DPL::DB::ORM::wrt;
        GlobalProperties::Row row;
        row.Set_home_network_data_usage(static_cast<int>(newMode));

        WRT_DB_UPDATE(update, GlobalProperties, &WrtDatabase::interface())
        update->Values(row);
        update->Execute();
    }
    Catch(DPL::DB::SqlConnection::Exception::Base){
        ReThrowMsg(GlobalDAO::Exception::DatabaseError,
                   "Failed to update home network data usage");
    }
}

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

void GlobalDAO::AddWhiteURI(const std::string &uri, bool subDomain)
{
    LogDebug("Add White URI : " << uri);
    Try {
        using namespace DPL::DB::ORM;
        using namespace DPL::DB::ORM::wrt;
        WidgetWhiteURIList::Row row;
        row.Set_uri(DPL::FromASCIIString(uri));
        row.Set_subdomain_access(static_cast<int>(subDomain));
        wrt::ScopedTransaction transaction(&WrtDatabase::interface());

        WRT_DB_INSERT(insert, WidgetWhiteURIList, &WrtDatabase::interface())

        insert->Values(row);
        insert->Execute();
        transaction.Commit();
    }
    Catch(DPL::DB::SqlConnection::Exception::Base){
        ReThrowMsg(GlobalDAO::Exception::DatabaseError,
                   "Failed to add white URI");
    }
}

void GlobalDAO::RemoveWhiteURI(const std::string &uri)
{
    LogDebug("Remove White URI : " << uri);
    Try {
        using namespace DPL::DB::ORM;
        using namespace DPL::DB::ORM::wrt;

        WRT_DB_DELETE(deleteFrom, WidgetWhiteURIList, &WrtDatabase::interface())
        deleteFrom->Where(Equals<WidgetWhiteURIList::uri>(DPL::FromASCIIString(
                                                              uri)));
        deleteFrom->Execute();
    } Catch(DPL::DB::SqlConnection::Exception::Base) {
        ReThrowMsg(GlobalDAO::Exception::DatabaseError,
                   "Failed to removed white URI from AdultBlackList");
    }
}

void GlobalDAO::SetCookieSharingMode(bool mode)
{
    LogDebug("updating Cookie Sharing mode to:" << mode);
    Try {
        using namespace DPL::DB::ORM;
        using namespace DPL::DB::ORM::wrt;
        GlobalProperties::Row row;
        row.Set_cookie_sharing_mode(mode);

        WRT_DB_UPDATE(update, GlobalProperties, &WrtDatabase::interface())
        update->Values(row);
        update->Execute();
    }
    Catch(DPL::DB::SqlConnection::Exception::Base){
        ReThrowMsg(GlobalDAO::Exception::DatabaseError,
                   "Failed to update Cookie Sharing Mode");
    }
}
} // namespace WrtDB
