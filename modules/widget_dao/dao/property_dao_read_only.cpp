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
 * property_dao_read_only.cpp
 *
 *  Created on: Nov 16, 2011
 *      Author: Krzysztof Jackiewicz(k.jackiewicz@samsung.com)
 */
#include <stddef.h>
#include <dpl/wrt-dao-ro/property_dao_read_only.h>
#include <dpl/wrt-dao-ro/widget_dao_read_only.h>
#include <dpl/log/log.h>
#include <dpl/foreach.h>
#include <dpl/wrt-dao-ro/webruntime_database.h>
#include <dpl/wrt-dao-ro/WrtDatabase.h>
#include <orm_generator_wrt.h>

namespace WrtDB {
namespace PropertyDAOReadOnly {
namespace {
typedef DPL::DB::ORM::wrt::WidgetPreference::key_name::ColumnType
ORMWidgetPropertyKey;
typedef DPL::DB::ORM::wrt::WidgetPreference::key_value::ColumnType
ORMWidgetPropertyValue;
typedef std::list<DPL::DB::ORM::wrt::WidgetPreference::Row>
ORMWidgetPreferenceList;
typedef std::list<WidgetPropertyKey> ORMWidgetPropertyKeyList;

void convertPropertyKey(const ORMWidgetPropertyKey& ormKey,
                        WidgetPropertyKey& key)
{
    key = ormKey;
}

void convertPropertyValue(const ORMWidgetPropertyValue& ormPropertyVal,
                          WidgetPropertyValue& propertyVal)
{
    propertyVal = ormPropertyVal;
}

void convertWidgetPreferenceRow(const ORMWidgetPreferenceList& ormWidgetPrefRow,
                                WidgetPreferenceList& prefRow)
{
    FOREACH(it, ormWidgetPrefRow) {
        WidgetPreferenceRow row;

        row.appId = it->Get_app_id();
        row.tizen_appid = it->Get_tizen_appid();
        row.key_name = it->Get_key_name();
        row.key_value = it->Get_key_value();
        row.readonly = it->Get_readonly();

        prefRow.push_back(row);
    }
}

void convertWidgetPropertyKeyList(const ORMWidgetPropertyKeyList& propKeyList,
                                  WidgetPropertyKeyList& keyList)
{
    FOREACH(it, propKeyList) {
        keyList.push_back(*it);
    }
}
}

//deprecated
DPL::OptionalInt CheckPropertyReadFlag(DbWidgetHandle widgetHandle,
                                       const WidgetPropertyKey &key)
{
    return CheckPropertyReadFlag(WidgetDAOReadOnly::getTzAppId(widgetHandle),
                                 key);
}

DPL::OptionalInt CheckPropertyReadFlag(TizenAppId tzAppid,
                                       const WidgetPropertyKey &key)
{
    LogDebug("Checking Property flag. appid: " << tzAppid <<
             ", key: " << key);

    Try {
        using namespace DPL::DB::ORM;
        using namespace DPL::DB::ORM::wrt;

        DbWidgetHandle widgetHandle(WidgetDAOReadOnly::getHandle(tzAppid));

        WRT_DB_SELECT(select, WidgetPreference, &WrtDatabase::interface())
        select->Where(And(Equals<WidgetPreference::app_id>(widgetHandle),
                          Equals<WidgetPreference::key_name>(key)));

        return select->GetSingleValue<WidgetPreference::readonly>();
    }
    Catch(DPL::DB::SqlConnection::Exception::Base){
        ReThrowMsg(Exception::DatabaseError,
                   "Failure during checking readonly flag for property");
    }
}

WidgetPropertyKeyList GetPropertyKeyList(TizenAppId tzAppid)
{
    LogDebug("Get PropertyKey list. appid: " << tzAppid);
    Try {
        using namespace DPL::DB::ORM;
        using namespace DPL::DB::ORM::wrt;

        DbWidgetHandle widgetHandle(WidgetDAOReadOnly::getHandle(tzAppid));

        ORMWidgetPropertyKeyList keyList;
        WRT_DB_SELECT(select, WidgetPreference, &WrtDatabase::interface())
        select->Where(Equals<WidgetPreference::app_id>(widgetHandle));
        keyList = select->GetValueList<WidgetPreference::key_name>();

        WidgetPropertyKeyList retKeyList;

        convertWidgetPropertyKeyList(keyList, retKeyList);
        return retKeyList;
    }
    Catch(DPL::DB::SqlConnection::Exception::Base){
        ReThrowMsg(Exception::DatabaseError,
                   "Failure during getting propertykey list");
    }
}

//deprecated
WidgetPreferenceList GetPropertyList(DbWidgetHandle widgetHandle)
{
    Try {
        TizenAppId tzAppid = WidgetDAOReadOnly::getTzAppId(widgetHandle);
        return GetPropertyList(tzAppid);
    } Catch(WidgetDAOReadOnly::Exception::WidgetNotExist){
        WidgetPreferenceList empty;
        return empty;
    }
}

WidgetPreferenceList GetPropertyList(TizenAppId tzAppId)
{
    LogDebug("Get Property list. tizenAppId: " << tzAppId);
    Try {
        using namespace DPL::DB::ORM;
        using namespace DPL::DB::ORM::wrt;

        DbWidgetHandle widgetHandle(WidgetDAOReadOnly::getHandle(tzAppId));

        WRT_DB_SELECT(select, WidgetPreference, &WrtDatabase::interface())
        select->Where(Equals<WidgetPreference::app_id>(widgetHandle));

        ORMWidgetPreferenceList ormPrefList = select->GetRowList();
        WidgetPreferenceList prefList;
        convertWidgetPreferenceRow(ormPrefList, prefList);

        return prefList;
    }
    Catch(DPL::DB::SqlConnection::Exception::Base){
        ReThrowMsg(Exception::DatabaseError,
                   "Failure during getting property list");
    }
}

WidgetPropertyValue GetPropertyValue(TizenAppId tzAppid,
                                     const WidgetPropertyKey &key)
{
    LogDebug("Get Property value. appid: " << tzAppid <<
             ", key: " << key);
    Try {
        using namespace DPL::DB::ORM;
        using namespace DPL::DB::ORM::wrt;

        DbWidgetHandle widgetHandle(WidgetDAOReadOnly::getHandle(tzAppid));

        WRT_DB_SELECT(select, WidgetPreference, &WrtDatabase::interface())
        select->Where(And(Equals<WidgetPreference::app_id>(widgetHandle),
                          Equals<WidgetPreference::key_name>(key)));

        ORMWidgetPropertyValue ormPropValue =
            select->GetSingleValue<WidgetPreference::key_value>();

        WidgetPropertyValue propValue;

        convertPropertyValue(ormPropValue, propValue);

        return propValue;
    }
    Catch(DPL::DB::SqlConnection::Exception::Base){
        ReThrowMsg(Exception::DatabaseError,
                   "Failure during getting property");
    }
}
} // namespace PropertyDAOReadOnly
} // namespace WrtDB
