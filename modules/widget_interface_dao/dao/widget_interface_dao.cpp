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
 * @author      Lukasz Marek (l.marek@samsung.com)
 * @version     0.1
 * @brief
 */

#include <wrt-commons/widget-interface-dao/widget_interface_dao.h>
#include <string>
#include <sstream>
#include <fstream>
#include <sys/stat.h>
#include <unistd.h>
#include <dpl/db/sql_connection.h>
#include <dpl/log/log.h>
#include <dpl/foreach.h>
#include <dpl/string.h>
#include <dpl/wrt-dao-ro/property_dao_read_only.h>
#include <dpl/wrt-dao-ro/widget_dao_read_only.h>
#include <dpl/wrt-dao-ro/WrtDatabase.h>
#include <dpl/wrt-dao-ro/widget_config.h>
#include <tzplatform_config.h>
#include "orm_generator_widget_interface.h"

namespace WidgetInterfaceDB {
using namespace DPL::DB::ORM;
using namespace DPL::DB::ORM::widget_interface;

#define SQL_CONNECTION_EXCEPTION_HANDLER_BEGIN          Try
#define SQL_CONNECTION_EXCEPTION_HANDLER_END(message)            \
    Catch(DPL::DB::SqlConnection::Exception::Base) {             \
        LogError(message);                                       \
        ReThrowMsg(WidgetInterfaceDAO::Exception::DatabaseError, \
                   message);                                     \
    }

namespace {
DPL::DB::SqlConnection::Flag::Type DATABASE_FLAG =
    DPL::DB::SqlConnection::Flag::UseLucene;
DPL::DB::SqlConnection::Flag::Option DATABASE_OPTION =
    DPL::DB::SqlConnection::Flag::RW;
const char *KEY_WIDGET_ARG = "widget_arg";

const char* const DATABASE_NAME = ".widget_interface.db";
const char* const DATABASE_FILE_PATH =
    "/usr/share/wrt-engine/widget_interface_db.sql";
const char* const DATABASE_JOURNAL_FILENAME = "-journal";

const int APP_UID = tzplatform_getuid(TZ_USER_NAME);
const int APP_GUID = tzplatform_getgid(TZ_SYS_USER_GROUP);
} // anonymous namespace

WidgetInterfaceDAO::WidgetInterfaceDAO(int widgetHandle) :
    m_widgetHandle(widgetHandle),
    m_databaseInterface(databaseFileName(widgetHandle), DATABASE_FLAG)
{
    checkDatabase();
    m_databaseInterface.AttachToThread(DATABASE_OPTION);
}

WidgetInterfaceDAO::~WidgetInterfaceDAO()
{
    m_databaseInterface.DetachFromThread();
}

void WidgetInterfaceDAO::checkDatabase()
{
    std::string databaseFile = databaseFileName(m_widgetHandle);
    struct stat buffer;
    if (stat(databaseFile.c_str(), &buffer) != 0) {
        //Create fresh database
        LogDebug("Creating database " << databaseFile);

        std::fstream file;
        file.open(DATABASE_FILE_PATH, std::ios_base::in);
        if (!file) {
            ThrowMsg(WidgetInterfaceDAO::Exception::DatabaseError,
                     "Cannot create database. SQL file is missing.");
        }

        std::stringstream stream;
        stream << file.rdbuf();

        file.close();

        SQL_CONNECTION_EXCEPTION_HANDLER_BEGIN
        {
            DPL::DB::SqlConnection con(databaseFile,
                                       DATABASE_FLAG,
                                       DATABASE_OPTION);
            con.ExecCommand(stream.str().c_str());
            copyPropertiesFromWrtDatabase();
        }
        SQL_CONNECTION_EXCEPTION_HANDLER_END("Cannot create database")

        if(chown(databaseFile.c_str(), APP_UID, APP_GUID) != 0) {
            ThrowMsg(WidgetInterfaceDAO::Exception::DatabaseError,
                     "Fail to change uid/guid");
        }
        std::string databaseJournal =
            databaseFile + DATABASE_JOURNAL_FILENAME;
        if(chown(databaseJournal.c_str(), APP_UID, APP_GUID) != 0) {
            ThrowMsg(WidgetInterfaceDAO::Exception::DatabaseError,
                     "Fail to change uid/guid");
        }
    }
}

void WidgetInterfaceDAO::copyPropertiesFromWrtDatabase()
{
    WrtDB::WrtDatabase::attachToThreadRO();
    m_databaseInterface.AttachToThread(DPL::DB::SqlConnection::Flag::RW);

    SQL_CONNECTION_EXCEPTION_HANDLER_BEGIN
    {
        WrtDB::PropertyDAOReadOnly::WidgetPreferenceList existing =
            WrtDB::PropertyDAOReadOnly::GetPropertyList(
                    WrtDB::WidgetDAOReadOnly::getTzAppId(m_widgetHandle)
        );

        //save all properties read from config.xml
        FOREACH(prop, existing) {
            std::string key = DPL::ToUTF8String(prop->key_name);
            if (key != KEY_WIDGET_ARG) {
                std::string value;
                if (!prop->key_value.IsNull()) {
                    value = DPL::ToUTF8String(*(prop->key_value));
                }
                bool readonly = !prop->readonly.IsNull() && (*prop->readonly);
                setItem(key, value, readonly, true);
            }
        }
    }
    SQL_CONNECTION_EXCEPTION_HANDLER_END(
        "Cannot copy properties read from config.xml");

    WrtDB::WrtDatabase::detachFromThread();
    m_databaseInterface.DetachFromThread();
}

void WidgetInterfaceDAO::setItem(const std::string& key,
                                 const std::string& value,
                                 bool readOnly)
{
    setItem(key, value, readOnly, false);
}

void WidgetInterfaceDAO::setItem(const std::string& key,
                                 const std::string& value,
                                 bool readOnly,
                                 bool fromConfigXml)
{
    SQL_CONNECTION_EXCEPTION_HANDLER_BEGIN
    {
        ScopedTransaction tran(&m_databaseInterface);
        //check if key exists
        Properties::Select select(&m_databaseInterface);
        select.Where(Equals<Properties::key>(DPL::FromUTF8String(key)));
        std::list<Properties::Row> rows = select.GetRowList();

        if (rows.empty()) {
            Properties::Insert insert(&m_databaseInterface);
            Properties::Row row;
            row.Set_key(DPL::FromUTF8String(key));
            row.Set_value(DPL::FromUTF8String(value));
            row.Set_readonly(readOnly ? 1 : 0);
            row.Set_configxml(fromConfigXml ? 1 : 0);
            insert.Values(row);
            insert.Execute();
        } else {
            Assert(rows.size() == 1);
            Properties::Row row = rows.front();
            if (row.Get_readonly() != 0) {
                Throw(Exception::LocalStorageValueNoModifableException);
            }
            row.Set_value(DPL::FromUTF8String(value));
            row.Set_readonly(readOnly ? 1 : 0);
            Properties::Update update(&m_databaseInterface);
            update.Where(Equals<Properties::key>(DPL::FromUTF8String(key)));
            update.Values(row);
            update.Execute();
        }
        tran.Commit();
    }
    SQL_CONNECTION_EXCEPTION_HANDLER_END("Cannot set item");
}

void WidgetInterfaceDAO::removeItem(const std::string& key)
{
    SQL_CONNECTION_EXCEPTION_HANDLER_BEGIN
    {
        Properties::Select select(&m_databaseInterface);
        select.Where(Equals<Properties::key>(DPL::FromUTF8String(key)));
        bool readonly = select.GetSingleValue<Properties::readonly>();
        if (readonly) {
            ThrowMsg(Exception::LocalStorageValueNoModifableException,
                     "Cannot delete item. Item is readonly");
        }
        Properties::Delete deleteItem(&m_databaseInterface);
        deleteItem.Where(Equals<Properties::key>(DPL::FromUTF8String(key)));
        deleteItem.Execute();
    }
    SQL_CONNECTION_EXCEPTION_HANDLER_END("Cannot delete item");
}

DPL::Optional<std::string> WidgetInterfaceDAO::getValue(
    const std::string& key) const
{
    SQL_CONNECTION_EXCEPTION_HANDLER_BEGIN
    {
        Properties::Select select(&m_databaseInterface);
        select.Where(Equals<Properties::key>(DPL::FromUTF8String(key)));
        std::list<DPL::String> value =
            select.GetValueList<Properties::value>();
        if (value.empty()) {
            return DPL::Optional<std::string>();
        }
        return DPL::Optional<std::string>(DPL::ToUTF8String(value.front()));
    }
    SQL_CONNECTION_EXCEPTION_HANDLER_END("Not found item");
}

void WidgetInterfaceDAO::clear(bool removeReadOnly)
{
    SQL_CONNECTION_EXCEPTION_HANDLER_BEGIN
    {
        Properties::Delete deleteItem(&m_databaseInterface);
        if (!removeReadOnly) {
            deleteItem.Where(Equals<Properties::readonly>(0));
        }
        deleteItem.Execute();
    }
    SQL_CONNECTION_EXCEPTION_HANDLER_END("Cannot delete all items");
}

size_t WidgetInterfaceDAO::getStorageSize() const
{
    SQL_CONNECTION_EXCEPTION_HANDLER_BEGIN
    {
        Properties::Select select(&m_databaseInterface);
        std::list<DPL::String> list =
            select.GetValueList<Properties::key>();
        return list.size();
    }
    SQL_CONNECTION_EXCEPTION_HANDLER_END("Cannot get item count");
}

std::string WidgetInterfaceDAO::getKeyByIndex(size_t index) const
{
    SQL_CONNECTION_EXCEPTION_HANDLER_BEGIN
    {
        Properties::Select select(&m_databaseInterface);
        select.OrderBy("key");
        std::list<DPL::String> list = select.GetValueList<Properties::key>();
        if (index >= list.size()) {
            Throw(Exception::InvalidArgumentException);
        }
        for (size_t i = 0; i < index; ++i) {
            list.pop_front();
        }
        return DPL::ToUTF8String(list.front());
    }
    SQL_CONNECTION_EXCEPTION_HANDLER_END("Cannot get item count");
}

std::string WidgetInterfaceDAO::databaseFileName(int widgetHandle)
{
    using namespace DPL::DB::ORM;
    using namespace WrtDB::WidgetConfig;

    WrtDB::WrtDatabase::attachToThreadRO();
    std::stringstream filename;
    SQL_CONNECTION_EXCEPTION_HANDLER_BEGIN
    {
        WrtDB::WidgetDAOReadOnly widgetDAO(widgetHandle);
        WrtDB::TizenPkgId pkgid = widgetDAO.getTizenPkgId();

        filename << GetWidgetPersistentStoragePath(pkgid)
                 << "/"
                 << DATABASE_NAME;
    }
    SQL_CONNECTION_EXCEPTION_HANDLER_END("Cannot get item count");
    WrtDB::WrtDatabase::detachFromThread();
    return filename.str();
}
} // namespace WidgetInterfaceDB
