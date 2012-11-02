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
typedef std::list<DPL::DB::ORM::wrt::WidgetPreference::Row> ORMWidgetPreferenceList;
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

        row.app_id = it->Get_app_id();
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

WidgetPropertyKeyList GetPropertyKeyList(DbWidgetHandle widgetHandle)
{
    LogDebug("Get PropertyKey list. Handle: " << widgetHandle);
    Try {
        using namespace DPL::DB::ORM;
        using namespace DPL::DB::ORM::wrt;
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

WidgetPreferenceList GetPropertyList(DbWidgetHandle widgetHandle)
{
    LogDebug("Get Property list. Handle: " << widgetHandle);
    Try {
        using namespace DPL::DB::ORM;
        using namespace DPL::DB::ORM::wrt;
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

WidgetPropertyValue GetPropertyValue(DbWidgetHandle widgetHandle,
                                     const WidgetPropertyKey &key)
{
    LogDebug("Get Property value. Handle: " << widgetHandle <<
             ", key: " << key);
    Try {
        using namespace DPL::DB::ORM;
        using namespace DPL::DB::ORM::wrt;
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

DPL::OptionalInt CheckPropertyReadFlag(DbWidgetHandle widgetHandle,
                                  const WidgetPropertyKey &key)
{
    LogDebug("Checking Property flag. Handle: " << widgetHandle <<
             ", key: " << key);
    Try {
        using namespace DPL::DB::ORM;
        using namespace DPL::DB::ORM::wrt;
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

} // namespace PropertyDAOReadOnly
} // namespace WrtDB
