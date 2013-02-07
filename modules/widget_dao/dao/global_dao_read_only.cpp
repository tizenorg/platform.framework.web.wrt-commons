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
 * @file    global_dao_read_only.cpp
 * @author  Pawel Sikorski (p.sikorski@samsung.com)
 * @version 1.0
 * @brief   This file contains the definition of global dao class.
 */
#include <stddef.h>
#include <dpl/wrt-dao-ro/global_dao_read_only.h>

#include <dpl/foreach.h>
#include <dpl/log/log.h>
#include <dpl/string.h>
#include <dpl/db/orm.h>
#include <orm_generator_wrt.h>
#include <dpl/wrt-dao-ro/webruntime_database.h>
#include <dpl/wrt-dao-ro/WrtDatabase.h>
#include <dpl/wrt-dao-ro/common_dao_types.h>

namespace WrtDB {
bool GlobalDAOReadOnly::GetDeveloperMode()
{
    LogDebug("Getting Developer mode");
    Try {
        using namespace DPL::DB::ORM;
        using namespace DPL::DB::ORM::wrt;
        WRT_DB_SELECT(select, GlobalProperties, &WrtDatabase::interface())
        return select->GetSingleValue<GlobalProperties::developer_mode>();
    }
    Catch(DPL::DB::SqlConnection::Exception::Base){
        ReThrowMsg(GlobalDAOReadOnly::Exception::DatabaseError,
                   "Failed to get developer mode");
    }
}

bool GlobalDAOReadOnly::GetSecureByDefault()
{
    using namespace DPL::DB::ORM;
    using namespace DPL::DB::ORM::wrt;
    WRT_DB_SELECT(select, GlobalProperties, &WrtDatabase::interface())
    return select->GetSingleValue<GlobalProperties::secure_by_default>();
}

bool GlobalDAOReadOnly::getComplianceMode()
{
    LogDebug("Getting compliance mode");
    Try {
        using namespace DPL::DB::ORM;
        using namespace DPL::DB::ORM::wrt;
        WRT_DB_SELECT(select, GlobalProperties, &WrtDatabase::interface())
        return select->GetSingleValue<GlobalProperties::compliance_mode>();
    }
    Catch(DPL::DB::SqlConnection::Exception::Base){
        ReThrowMsg(GlobalDAOReadOnly::Exception::DatabaseError,
                   "Failed to get compliance mode");
    }
}

std::string GlobalDAOReadOnly::getComplianceFakeImei()
{
    LogDebug("Getting compliance fake IMEI");
    Try {
        using namespace DPL::DB::ORM;
        using namespace DPL::DB::ORM::wrt;
        WRT_DB_SELECT(select, GlobalProperties, &WrtDatabase::interface())
        DPL::Optional<DPL::String> result =
            select->GetSingleValue<GlobalProperties::compliance_fake_imei>();
        if (!result) {
            return std::string();
        }
        return DPL::ToUTF8String(*result);
    }
    Catch(DPL::DB::SqlConnection::Exception::Base){
        ReThrowMsg(GlobalDAOReadOnly::Exception::DatabaseError,
                   "Failed to get compliance fake IMEI");
    }
}

std::string GlobalDAOReadOnly::getComplianceFakeMeid()
{
    LogDebug("Getting compliance fake MEID");
    Try {
        using namespace DPL::DB::ORM;
        using namespace DPL::DB::ORM::wrt;
        WRT_DB_SELECT(select, GlobalProperties, &WrtDatabase::interface())
        DPL::Optional<DPL::String> result =
            select->GetSingleValue<GlobalProperties::compliance_fake_meid>();
        if (!result) {
            return std::string();
        }
        return DPL::ToUTF8String(*result);
    }
    Catch(DPL::DB::SqlConnection::Exception::Base){
        ReThrowMsg(GlobalDAOReadOnly::Exception::DatabaseError,
                   "Failed to get compliance fake MEID");
    }
}

bool GlobalDAOReadOnly::IsValidSubTag(const DPL::String& tag, int type)
{
    using namespace DPL::DB::ORM;
    using namespace DPL::DB::ORM::wrt;
    WRT_DB_SELECT(select, iana_records, &WrtDatabase::interface())
    select->Where(Equals<iana_records::SUBTAG>(tag));
    auto row = select->GetRowList();
    if (row.empty() || row.front().Get_TYPE() != type) {
        return false;
    } else {
        return true;
    }
}

GlobalDAOReadOnly::NetworkAccessMode
GlobalDAOReadOnly::GetHomeNetworkDataUsage()
{
    LogDebug("Getting home network data usage");
    Try {
        using namespace DPL::DB::ORM;
        using namespace DPL::DB::ORM::wrt;
        WRT_DB_SELECT(select, GlobalProperties, &WrtDatabase::interface())
        return static_cast<GlobalDAOReadOnly::NetworkAccessMode>(
                   select->GetSingleValue<
                       GlobalProperties::home_network_data_usage>());
    }
    Catch(DPL::DB::SqlConnection::Exception::Base){
        ReThrowMsg(GlobalDAOReadOnly::Exception::DatabaseError,
                   "Failed to get home network data usage");
    }
}

GlobalDAOReadOnly::NetworkAccessMode GlobalDAOReadOnly::GetRoamingDataUsage()
{
    LogDebug("Getting roaming network data usage");
    Try {
        using namespace DPL::DB::ORM;
        using namespace DPL::DB::ORM::wrt;
        WRT_DB_SELECT(select, GlobalProperties, &WrtDatabase::interface())
        return static_cast<GlobalDAOReadOnly::NetworkAccessMode>(
                   select->GetSingleValue<GlobalProperties::roaming_data_usage>());
    }
    Catch(DPL::DB::SqlConnection::Exception::Base){
        ReThrowMsg(GlobalDAOReadOnly::Exception::DatabaseError,
                   "Failed to get roaming network data usage");
    }
}

//user agent strings are stored in db...
//and it is configurable for test in development.
DPL::String GlobalDAOReadOnly::GetUserAgentValue(const DPL::String &key)
{
    LogDebug("Get User Agent Value : " << key);
    Try {
        using namespace DPL::DB::ORM;
        using namespace DPL::DB::ORM::wrt;
        WRT_DB_SELECT(select, UserAgents, &WrtDatabase::interface())
        select->Where(Equals<UserAgents::key_name>(key));
        DPL::Optional<DPL::String> agent =
            select->GetSingleValue<UserAgents::key_value>();
        if (agent.IsNull()) {
            return DPL::FromUTF8String("");
        } else {
            return *agent;
        }
    }
    Catch(DPL::DB::SqlConnection::Exception::Base){
        ReThrowMsg(GlobalDAOReadOnly::Exception::DatabaseError,
                   "Failed to get user agent string");
    }
}

DeviceCapabilitySet GlobalDAOReadOnly::GetDeviceCapability(
    const DPL::String &apifeature)
{
    // This could be done with one simply sql query but support for join is
    // needed in orm.
    Try {
        using namespace DPL::DB::ORM;
        using namespace DPL::DB::ORM::wrt;

        int featureUUID;
        FeatureDeviceCapProxy::Select::RowList rows;
        DeviceCapabilitySet result;

        {
            WRT_DB_SELECT(select, FeaturesList, &WrtDatabase::interface())
            select->Where(Equals<FeaturesList::FeatureName>(apifeature));
            featureUUID = select->GetSingleValue<FeaturesList::FeatureUUID>();
        }

        {
            WRT_DB_SELECT(select,
                          FeatureDeviceCapProxy,
                          &WrtDatabase::interface())
            select->Where(Equals<FeatureDeviceCapProxy::FeatureUUID>(
                              featureUUID));
            rows = select->GetRowList();
        }

        FOREACH(it, rows){
            WRT_DB_SELECT(select, DeviceCapabilities, &WrtDatabase::interface())
            select->Where(Equals<DeviceCapabilities::DeviceCapID>(
                              it->Get_DeviceCapID()));
            result.insert(select->
                              GetSingleValue<DeviceCapabilities::DeviceCapName>());
        }

        return result;
    } Catch(DPL::DB::SqlConnection::Exception::Base){
        ReThrowMsg(GlobalDAOReadOnly::Exception::DatabaseError,
                   "Failed to update roaming network data usage");
    }
}

WidgetAccessInfoList GlobalDAOReadOnly::GetWhiteURIList()
{
    LogDebug("Getting WhiteURIList.");
    Try {
        using namespace DPL::DB::ORM;
        using namespace DPL::DB::ORM::wrt;
        WRT_DB_SELECT(select, WidgetWhiteURIList, &WrtDatabase::interface())

        WidgetAccessInfoList resultList;
        typedef std::list<WidgetWhiteURIList::Row> RowList;
        RowList list = select->GetRowList();

        for (RowList::iterator i = list.begin(); i != list.end(); ++i) {
            WidgetAccessInfo whiteURI;
            whiteURI.strIRI = i->Get_uri();
            whiteURI.bSubDomains = i->Get_subdomain_access();
            resultList.push_back(whiteURI);
            LogInfo("[uri] : " << whiteURI.strIRI <<
                    ", [subdomain] : " << whiteURI.bSubDomains);
        }
        return resultList;
    } Catch(DPL::DB::SqlConnection::Exception::Base) {
        ReThrowMsg(GlobalDAOReadOnly::Exception::DatabaseError,
                   "Failed to get whiteURI list");
    }
}

bool GlobalDAOReadOnly::GetCookieSharingMode()
{
    LogDebug("Getting Cookie Sharing mode");
    Try {
        using namespace DPL::DB::ORM;
        using namespace DPL::DB::ORM::wrt;
        WRT_DB_SELECT(select, GlobalProperties, &WrtDatabase::interface())
        return select->GetSingleValue<GlobalProperties::cookie_sharing_mode>();
    }
    Catch(DPL::DB::SqlConnection::Exception::Base){
        ReThrowMsg(GlobalDAOReadOnly::Exception::DatabaseError,
                   "Failed to get Cookie Sharing mode");
    }
}
} // namespace WrtDB
