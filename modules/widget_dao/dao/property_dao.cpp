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
 * @file   property_dao.h
 * @author  Pawel Sikorski (p.sikorski@samsung.com)
 * @version 1.0
 * @brief   This file contains the definition of property dao class.
 */
#include <stddef.h>
#include <dpl/wrt-dao-rw/property_dao.h>
#include <dpl/wrt-dao-ro/widget_dao_read_only.h>
#include <dpl/log/log.h>
#include <dpl/foreach.h>
#include <dpl/wrt-dao-ro/webruntime_database.h>
#include <dpl/wrt-dao-ro/WrtDatabase.h>
#include <orm_generator_wrt.h>

namespace WrtDB {
namespace PropertyDAO {

void RemoveProperty(TizenAppId tzAppid,
                    const PropertyDAOReadOnly::WidgetPropertyKey &key)
{
    //TODO below there are two queries.
    // First query asks if given property can be removed,
    // Second removes it. Maybe it should be combined two one.

    LogDebug("Removing Property. appid: " << tzAppid << ", key: " << key);
    Try {
        DPL::DB::ORM::wrt::ScopedTransaction transaction(&WrtDatabase::interface());

        DPL::OptionalInt readonly = PropertyDAOReadOnly::CheckPropertyReadFlag(
                tzAppid,
                key);
        if (!readonly.IsNull() && *readonly == 1) {
            LogError("'" << key <<
                     "' key is readonly. Cannot remove property !");
            ThrowMsg(PropertyDAOReadOnly::Exception::ReadOnlyProperty,
                     "Property is readonly");
        }

        // Key is not readonly, or has no flag defined

        using namespace DPL::DB::ORM;
        using namespace DPL::DB::ORM::wrt;
        WRT_DB_DELETE(del, WidgetPreference, &WrtDatabase::interface())
        del->Where(And(
                       Equals<WidgetPreference::tizen_appid>(tzAppid),
                       Equals<WidgetPreference::key_name>(key)));
        del->Execute();

        transaction.Commit();
    }
    Catch(DPL::DB::SqlConnection::Exception::Base){
        ReThrowMsg(PropertyDAOReadOnly::Exception::DatabaseError,
                   "Failure during removing property");
    }
}

//deprecated
void SetProperty(DbWidgetHandle widgetHandle,
                 const PropertyDAOReadOnly::WidgetPropertyKey &key,
                 const PropertyDAOReadOnly::WidgetPropertyValue &value,
                 bool readOnly)
{
    SetProperty(WidgetDAOReadOnly::getTzAppId(widgetHandle),key,value,readOnly);
}

void SetProperty(TizenAppId tzAppid,
                 const PropertyDAOReadOnly::WidgetPropertyKey &key,
                 const PropertyDAOReadOnly::WidgetPropertyValue &value,
                 bool readOnly)
{
    LogDebug("Setting/updating Property. appid: " << tzAppid <<
             ", key: " << key);
    Try {
        using namespace DPL::DB::ORM;
        using namespace DPL::DB::ORM::wrt;
        DPL::DB::ORM::wrt::ScopedTransaction transaction(&WrtDatabase::interface());

        DPL::OptionalInt readonly = PropertyDAOReadOnly::CheckPropertyReadFlag(
                tzAppid,
                key);
        if (!readonly.IsNull() && *readonly == 1) {
            LogError("'" << key <<
                     "' key is readonly. Cannot remove property !");
            ThrowMsg(PropertyDAOReadOnly::Exception::ReadOnlyProperty,
                     "Property is readonly");
        }

        if (readonly.IsNull()) {
            WidgetPreference::Row row;
            row.Set_tizen_appid(tzAppid);
            row.Set_key_name(key);
            row.Set_key_value(value);
            row.Set_readonly(readOnly ? 1 : 0);

            WRT_DB_INSERT(insert, WidgetPreference, &WrtDatabase::interface())
            insert->Values(row);
            insert->Execute();
        } else {
            WidgetPreference::Row row;
            row.Set_key_value(value);

            WRT_DB_UPDATE(update, WidgetPreference, &WrtDatabase::interface())
            update->Where(And(
                              Equals<WidgetPreference::tizen_appid>(tzAppid),
                              Equals<WidgetPreference::key_name>(key)));

            update->Values(row);
            update->Execute();
        }
        transaction.Commit();
    }
    Catch(DPL::DB::SqlConnection::Exception::Base){
        ReThrowMsg(PropertyDAOReadOnly::Exception::DatabaseError,
                   "Failure during setting/updating property");
    }
}

void RegisterProperties(TizenAppId tzAppid,
                        const WidgetRegisterInfo &regInfo)
{
    LogDebug("Registering proferences for widget. appid: " << tzAppid);

    // Try-Catch in WidgetDAO

    using namespace DPL::DB::ORM;
    using namespace DPL::DB::ORM::wrt;
    const ConfigParserData& widgetConfigurationInfo = regInfo.configInfo;

    FOREACH(it, widgetConfigurationInfo.preferencesList)
    {
        { // Insert into table Widget Preferences
            WidgetPreference::Row row;
            row.Set_tizen_appid(tzAppid);
            row.Set_key_name(it->name);
            row.Set_key_value(it->value);
            int readonly = true == it->readonly ? 1 : 0;
            row.Set_readonly(readonly);

            WRT_DB_INSERT(insert, WidgetPreference, &WrtDatabase::interface())
            insert->Values(row);
            insert->Execute();
        }
    }
}

} // namespace PropertyDAO
} // namespace WrtDB
