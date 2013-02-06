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
 * @file    plugin_dao_read_only.cpp
 * @author  Krzysztof Jackiewicz (k.jackiewicz@samsung.com)
 * @version 1.0
 * @brief   This file contains the implementation of plugin dao read only
 */
#include <stddef.h>
#include <dpl/wrt-dao-ro/plugin_dao_read_only.h>

#include <sstream>
#include <dpl/log/log.h>
#include <dpl/foreach.h>
#include <dpl/wrt-dao-ro/webruntime_database.h>
#include <dpl/db/orm.h>
#include <orm_generator_wrt.h>
#include <dpl/wrt-dao-ro/WrtDatabase.h>

namespace WrtDB {
namespace {
typedef DPL::DB::ORM::wrt::PluginProperties::Row PluginRow;

PluginRow getPluginRow(DbPluginHandle pluginHandle)
{
    LogDebug("Getting plugin row. Handle: " << pluginHandle);
    Try {
        using namespace DPL::DB::ORM;
        using namespace DPL::DB::ORM::wrt;
        WRT_DB_SELECT(select, PluginProperties, &WrtDatabase::interface())
        select->Where(Equals<PluginProperties::PluginPropertiesId>(
                          pluginHandle));

        PluginProperties::Select::RowList rows = select->GetRowList();
        if (rows.empty()) {
            ThrowMsg(PluginDAOReadOnly::Exception::PluginNotExist,
                     "Cannot find plugin. Handle: " + pluginHandle);
        }
        return rows.front();
    }
    Catch(DPL::DB::SqlConnection::Exception::Base) {
        ReThrowMsg(PluginDAOReadOnly::Exception::DatabaseError,
                   "Failed in GetPluginRow");
    }
}
}

PluginDAOReadOnly::PluginDAOReadOnly(DbPluginHandle pluginHandle) :
    m_pluginHandle(pluginHandle)
{
    if (!isPluginInstalled(m_pluginHandle)) {
        LogError("Plugin " << m_pluginHandle << " not installed.");
        Throw(PluginDAOReadOnly::Exception::PluginNotExist);
    }

    checkInstallationCompleted();
}

PluginDAOReadOnly::PluginDAOReadOnly(const std::string &libraryName)
{
    LogDebug("PluginDAOReadOnly ( " << libraryName << " )");
    Try {
        using namespace DPL::DB::ORM;
        using namespace DPL::DB::ORM::wrt;
        WRT_DB_SELECT(select, PluginProperties, &WrtDatabase::interface())
        select->Where(Equals<PluginProperties::PluginLibraryName>(
                          DPL::FromUTF8String(libraryName)));

        PluginProperties::Select::RowList rows = select->GetRowList();
        if (!rows.empty()) {
            m_pluginHandle = rows.front().Get_PluginPropertiesId();
        } else {
            ThrowMsg(PluginDAOReadOnly::Exception::PluginNotExist,
                     "Cannot find plugin: [" + libraryName + "]");
        }
        LogDebug(" >> Handle for this plugin: " << m_pluginHandle);

        checkInstallationCompleted();
    }
    Catch(DPL::DB::SqlConnection::Exception::Base) {
        ReThrowMsg(PluginDAOReadOnly::Exception::DatabaseError,
                   "Failed to connect to database");
    }
}

void PluginDAOReadOnly::checkInstallationCompleted()
{
    if (getInstallationStateForHandle(m_pluginHandle)
        != PluginDAOReadOnly::INSTALLATION_COMPLETED)
    {
        LogError("Plugin " << m_pluginHandle << " installation not completed");
        Throw(PluginDAOReadOnly::Exception::PluginInstallationNotCompleted);
    }
}

bool PluginDAOReadOnly::isPluginInstalled(const std::string &libraryName)
{
    LogDebug("Check if Library is installed. LibraryName: " << libraryName);
    Try {
        using namespace DPL::DB::ORM;
        using namespace DPL::DB::ORM::wrt;
        WRT_DB_SELECT(select, PluginProperties, &WrtDatabase::interface())
        select->Where(Equals<PluginProperties::PluginLibraryName>(
                          DPL::FromUTF8String(libraryName)));

        PluginProperties::Select::RowList rows = select->GetRowList();

        bool flag = !rows.empty();
        LogDebug(" >> Plugin " << libraryName <<
                 (flag ? " found." : " not found."));

        return flag;
    }
    Catch(DPL::DB::SqlConnection::Exception::Base) {
        ReThrowMsg(PluginDAOReadOnly::Exception::DatabaseError,
                   "Failed in isPluginInstalled");
    }
}

PluginHandleList PluginDAOReadOnly::getPluginHandleList()
{
    LogDebug("Getting plugin handle list.");
    Try {
        using namespace DPL::DB::ORM;
        using namespace DPL::DB::ORM::wrt;
        WRT_DB_SELECT(select, PluginProperties, &WrtDatabase::interface())

        PluginHandleList ret =
            select->GetValueList<PluginProperties::PluginPropertiesId>();

        std::ostringstream handles;
        FOREACH(it, ret)
        handles << *it << " ";
        LogDebug(" >> PluginHandle list retrieved: (" << handles << ")");

        return ret;
    }
    Catch(DPL::DB::SqlConnection::Exception::Base) {
        ReThrowMsg(PluginDAOReadOnly::Exception::DatabaseError,
                   "Failed in GetPluginHandleList");
    }
}

DbPluginHandle PluginDAOReadOnly::getPluginHandle() const
{
    return m_pluginHandle;
}

//"value" cannot be null, as in registerPlugin it is always set
#define RETURN_STD_STRING(in, what)                 \
    std::string ret = "";                           \
    if (!in.IsNull()) {                             \
        ret = DPL::ToUTF8String(*in); }             \
    LogDebug(" >> Plugin " << what << ": " << ret); \
    return ret;

std::string PluginDAOReadOnly::getLibraryPath() const
{
    LogDebug("Getting plugin library path. Handle: " << m_pluginHandle);
    PluginRow row = getPluginRow(m_pluginHandle);
    RETURN_STD_STRING(row.Get_PluginLibraryPath(), "library path")
}

std::string PluginDAOReadOnly::getLibraryName() const
{
    LogDebug("Getting plugin library name. Handle: " << m_pluginHandle);
    PluginRow row = getPluginRow(m_pluginHandle);
    std::string ret = DPL::ToUTF8String(row.Get_PluginLibraryName());
    LogDebug(" >> Plugin library name: " << ret);
    return ret;
}

#undef RETURN_STD_STRING

PluginHandleSetPtr PluginDAOReadOnly::getLibraryDependencies() const
{
    Try
    {
        using namespace DPL::DB::ORM;
        using namespace DPL::DB::ORM::wrt;
        PluginHandleSetPtr dependencies(new PluginHandleSet);

        WRT_DB_SELECT(select, PluginDependencies, &WrtDatabase::interface())
        select->Where(
            Equals<PluginDependencies::PluginPropertiesId>(m_pluginHandle));

        PluginDependencies::Select::RowList rows = select->GetRowList();

        if (!rows.empty()) {
            FOREACH(it, rows)
            {
                dependencies->insert(it->Get_RequiredPluginPropertiesId());
            }
        }

        return dependencies;
    }
    Catch(DPL::DB::SqlConnection::Exception::Base) {
        ReThrowMsg(PluginDAOReadOnly::Exception::DatabaseError,
                   "Failed in GetLibraryDependencies");
    }
}

DbPluginHandle PluginDAOReadOnly::getPluginHandleForImplementedObject(
    const std::string& objectName)
{
    LogDebug("GetPluginHandle for object: " << objectName);

    Try
    {
        DbPluginHandle pluginHandle = INVALID_PLUGIN_HANDLE;

        using namespace DPL::DB::ORM;
        using namespace DPL::DB::ORM::wrt;

        WRT_DB_SELECT(select, PluginImplementedObjects, &WrtDatabase::interface())
        select->Where(
            Equals<PluginImplementedObjects::PluginObject>(
                DPL::FromUTF8String(objectName)));

        PluginImplementedObjects::Select::RowList rows = select->GetRowList();

        if (!rows.empty()) {
            pluginHandle = rows.front().Get_PluginPropertiesId();
        } else {
            LogWarning("PluginHandle for object not found");
        }
        return pluginHandle;
    }
    Catch(DPL::DB::SqlConnection::Exception::Base) {
        ReThrowMsg(PluginDAOReadOnly::Exception::DatabaseError,
                   "Failed in GetPluginHandleForImplementedObject");
    }
}

ImplementedObjectsList PluginDAOReadOnly::getImplementedObjectsForPluginHandle(
    DbPluginHandle handle)
{
    LogDebug("getImplementedObjects for pluginHandle: " << handle);

    Try
    {
        ImplementedObjectsList objectList;
        using namespace DPL::DB::ORM;
        using namespace DPL::DB::ORM::wrt;

        WRT_DB_SELECT(select, PluginImplementedObjects, &WrtDatabase::interface())
        select->Where(
            Equals<PluginImplementedObjects::PluginPropertiesId>(handle));

        PluginImplementedObjects::Select::RowList rows = select->GetRowList();

        if (!rows.empty()) {
            FOREACH(it, rows)
            {
                objectList.push_back(DPL::ToUTF8String(it->Get_PluginObject()));
            }
        } else {
            LogWarning("PluginHandle for object not found");
        }
        return objectList;
    }
    Catch(DPL::DB::SqlConnection::Exception::Base) {
        ReThrowMsg(PluginDAOReadOnly::Exception::DatabaseError,
                   "Failed in GetPluginHandleForImplementedObject");
    }
}

PluginObjectsDAO::ObjectsPtr PluginDAOReadOnly::
    getRequiredObjectsForPluginHandle(
    DbPluginHandle handle)
{
    Try
    {
        using namespace DPL::DB::ORM;
        using namespace DPL::DB::ORM::wrt;

        PluginObjectsDAO::ObjectsPtr objects =
            PluginObjectsDAO::ObjectsPtr(new PluginObjectsDAO::Objects);

        WRT_DB_SELECT(select, PluginRequiredObjects, &WrtDatabase::interface())
        select->Where(
            Equals<PluginRequiredObjects::PluginPropertiesId>(handle));

        PluginRequiredObjects::Select::RowList rows = select->GetRowList();

        if (!rows.empty()) {
            FOREACH(it, rows)
            {
                objects->insert(DPL::ToUTF8String(it->Get_PluginObject()));
            }
        }

        return objects;
    }
    Catch(DPL::DB::SqlConnection::Exception::Base) {
        ReThrowMsg(PluginDAOReadOnly::Exception::DatabaseError,
                   "Failed in GetRequiredObjectsForPluginHandle");
    }
}

PluginHandleSetPtr PluginDAOReadOnly::getPluginHandleByStatus(
    PluginInstallationState state)
{
    Try
    {
        using namespace DPL::DB::ORM;
        using namespace DPL::DB::ORM::wrt;

        PluginHandleSetPtr handleSet(new PluginHandleSet);

        WRT_DB_SELECT(select, PluginProperties, &WrtDatabase::interface())
        select->Where(
            Equals<PluginProperties::InstallationState>(ToInt(state)));

        PluginProperties::Select::RowList rows = select->GetRowList();

        if (!rows.empty()) {
            FOREACH(it, rows)
            {
                handleSet->insert(it->Get_PluginPropertiesId());
            }
        }

        return handleSet;
    }
    Catch(DPL::DB::SqlConnection::Exception::Base) {
        ReThrowMsg(PluginDAOReadOnly::Exception::DatabaseError,
                   "Failed in GetPluginHandleByStatus");
    }
}

PluginDAOReadOnly::PluginInstallationState PluginDAOReadOnly::
    getInstallationStatus() const
{
    PluginRow row = getPluginRow(m_pluginHandle);
    return ToState(row.Get_InstallationState());
}

PluginDAOReadOnly::PluginInstallationState PluginDAOReadOnly::
    getInstallationStateForHandle(
    DbPluginHandle handle)
{
    Try
    {
        using namespace DPL::DB::ORM;
        using namespace DPL::DB::ORM::wrt;

        WRT_DB_SELECT(select, PluginProperties, &WrtDatabase::interface())
        select->Where(
            Equals<PluginProperties::PluginPropertiesId>(handle));

        PluginProperties::Select::RowList rows = select->GetRowList();

        if (!rows.empty()) {
            return ToState(rows.front().Get_InstallationState());
        }
        LogError("Data in DB are invalid. Missing field");
        return UNKNOWN_ERROR;
    }
    Catch(DPL::DB::SqlConnection::Exception::Base) {
        ReThrowMsg(PluginDAOReadOnly::Exception::DatabaseError,
                   "Failed in GetStatusForHandle");
    }
}

bool PluginDAOReadOnly::isPluginInstalled(DbPluginHandle pluginHandle)
{
    Try {
        using namespace DPL::DB::ORM;
        using namespace DPL::DB::ORM::wrt;
        WRT_DB_SELECT(select, PluginProperties, &WrtDatabase::interface())
        select->Where(
            Equals<PluginProperties::PluginPropertiesId>(pluginHandle));

        PluginProperties::Select::RowList rows = select->GetRowList();

        bool flag = !rows.empty();

        return flag;
    }
    Catch(DPL::DB::SqlConnection::Exception::Base) {
        ReThrowMsg(PluginDAOReadOnly::Exception::DatabaseError,
                   "Failed in isPluginInstalled");
    }
}
} // namespace WrtDB
