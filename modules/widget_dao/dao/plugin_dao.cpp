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
 * @file   plugin_dao.cpp
 * @author  Pawel Sikorski (p.sikorski@samsung.com)
 * @author  Grzegorz Krawczyk (g.krawczyk@samsung.com)
 * @version 1.0
 * @brief   This file contains the definition of plugin dao class.
 */
#include <stddef.h>
#include <dpl/wrt-dao-rw/plugin_dao.h>
#include <dpl/log/log.h>
#include <dpl/foreach.h>
#include <orm_generator_wrt.h>
#include <dpl/wrt-dao-ro/webruntime_database.h>
#include <dpl/wrt-dao-ro/WrtDatabase.h>

namespace WrtDB {

PluginDAO::PluginDAO(DbPluginHandle pluginHandle) :
    PluginDAOReadOnly(pluginHandle)
{
}

PluginDAO::PluginDAO(const std::string &libraryName) :
    PluginDAOReadOnly(libraryName)
{
}

DbPluginHandle PluginDAO::registerPlugin(const PluginMetafileData& metafile,
                                       const std::string& pluginPath)
{
    LogDebug("Registering plugin. Path: " << pluginPath);

    Try {
        DPL::DB::ORM::wrt::ScopedTransaction transaction(&WrtDatabase::interface());
        DbPluginHandle handle;

        if (isPluginInstalled(metafile.m_libraryName)) {
            handle = PluginDAO(metafile.m_libraryName).getPluginHandle();
            LogInfo(" >> Library " << metafile.m_libraryName <<
                    " is already registered. Handle: " << handle);
        } else {
            LogDebug("Register Plugin: " << metafile.m_libraryName);

            using namespace DPL::DB::ORM;
            using namespace DPL::DB::ORM::wrt;

            typedef PluginProperties::Row PluginPropertiesRow;

            PluginPropertiesRow row;
            row.Set_PluginLibraryName(
                DPL::FromUTF8String(metafile.m_libraryName));
            row.Set_InstallationState(INSTALLATION_IN_PROGRESS);
            row.Set_PluginLibraryPath(
                DPL::FromUTF8String(pluginPath));

            WRT_DB_INSERT(insert, PluginProperties, &WrtDatabase::interface())
            insert->Values(row);
            handle = insert->Execute();
            LogDebug(" >> Plugin Registered. Handle: " << handle);
        }
        transaction.Commit();
        return handle;
    }
    Catch(DPL::DB::SqlConnection::Exception::Base)
    {
        ReThrowMsg(PluginDAO::Exception::DatabaseError,
                   "Failed in RegisterPlugin");
    }
}

void PluginDAO::registerPluginImplementedObject(const std::string& objectName,
        DbPluginHandle pluginHandle)
{
    LogDebug("Registering plugin object: " << objectName);

    Try {
        DPL::DB::ORM::wrt::ScopedTransaction transaction(&WrtDatabase::interface());

        LogDebug("Register Object: " << objectName);

        using namespace DPL::DB::ORM;
        using namespace DPL::DB::ORM::wrt;

        typedef PluginImplementedObjects::Row PluginObjectsRow;

        PluginObjectsRow row;
        row.Set_PluginObject(DPL::FromUTF8String(objectName));
        row.Set_PluginPropertiesId(pluginHandle);

        WRT_DB_INSERT(insert, PluginImplementedObjects, &WrtDatabase::interface())
        insert->Values(row);
        insert->Execute();
        transaction.Commit();
    }
    Catch(DPL::DB::SqlConnection::Exception::Base)
    {
        ReThrowMsg(PluginDAO::Exception::DatabaseError,
                   "Failed in RegisterPluginObject");
    }
}

void PluginDAO::registerPluginRequiredObject(const std::string& objectName,
        DbPluginHandle pluginHandle)
{
    LogDebug("Registering plugin object: " << objectName);

    Try {
        DPL::DB::ORM::wrt::ScopedTransaction transaction(&WrtDatabase::interface());

        LogDebug("Register Object: " << objectName);

        using namespace DPL::DB::ORM;
        using namespace DPL::DB::ORM::wrt;

        typedef PluginRequiredObjects::Row PluginObjectsRow;

        PluginObjectsRow row;
        row.Set_PluginPropertiesId(pluginHandle);
        row.Set_PluginObject(DPL::FromUTF8String(objectName));

        WRT_DB_INSERT(insert, PluginRequiredObjects, &WrtDatabase::interface())
        insert->Values(row);
        insert->Execute();
        transaction.Commit();
    }
    Catch(DPL::DB::SqlConnection::Exception::Base)
    {
        ReThrowMsg(PluginDAO::Exception::DatabaseError,
                   "Failed in RegisterPluginObject");
    }
}

void PluginDAO::registerPluginLibrariesDependencies(
        DbPluginHandle pluginHandle,
        const PluginHandleSetPtr& dependencies)
{
    LogDebug("Registering plugin library dependencies: " << pluginHandle);

    Try {
        DPL::DB::ORM::wrt::ScopedTransaction transaction(&WrtDatabase::interface());

        using namespace DPL::DB::ORM;
        using namespace DPL::DB::ORM::wrt;

        typedef PluginDependencies::Row PluginDependeciesRow;
        PluginDependeciesRow row;

        FOREACH(it, *dependencies)
        {
            row.Set_PluginPropertiesId(pluginHandle);
            row.Set_RequiredPluginPropertiesId(*it);

            WRT_DB_INSERT(insert, PluginDependencies, &WrtDatabase::interface())
            insert->Values(row);
            insert->Execute();
            transaction.Commit();
        }
    }
    Catch(DPL::DB::SqlConnection::Exception::Base)
    {
        ReThrowMsg(PluginDAO::Exception::DatabaseError,
                   "Failed in RegisterPluginObject");
    }
}

void PluginDAO::setPluginInstallationStatus(DbPluginHandle pluginHandle,
                                            PluginInstallationState state)
{
    Try {
        LogDebug(
            "Set installation state: " << state << " handle " << pluginHandle);

        using namespace DPL::DB::ORM;
        using namespace DPL::DB::ORM::wrt;
        ScopedTransaction transaction(&WrtDatabase::interface());

        typedef wrt::PluginProperties::Row PluginPropertiesRow;

        PluginPropertiesRow row;
        row.Set_InstallationState(state);

        WRT_DB_UPDATE(update, PluginProperties, &WrtDatabase::interface())
        update->Where(
            Equals<PluginProperties::PluginPropertiesId>(pluginHandle));

        update->Values(row);
        update->Execute();
        transaction.Commit();
    }
    Catch(DPL::DB::SqlConnection::Exception::Base)
    {
        ReThrowMsg(PluginDAO::Exception::DatabaseError,
                   "Failed in RegisterLibraryDependencies");
    }
}

void PluginDAO::unregisterPlugin(DbPluginHandle pluginHandle)
{
    LogDebug("unregisterPlugin plugin. Handle: " << pluginHandle);

    Try {
        DPL::DB::ORM::wrt::ScopedTransaction transaction(
                &WrtDatabase::interface());
        DbPluginHandle handle;

        if (!isPluginInstalled(pluginHandle)) {
            LogInfo("PluginHandle is invalid. Handle: " << handle);
            return;

        } else {
            using namespace DPL::DB::ORM;
            using namespace DPL::DB::ORM::wrt;

            WRT_DB_DELETE(del, PluginProperties, &WrtDatabase::interface())
            del->Where(
                Equals<PluginProperties::PluginPropertiesId>(pluginHandle));
            del->Execute();

            transaction.Commit();
            LogDebug(" >> Plugin Unregistered. Handle: " << handle);
        }
    }
    Catch(DPL::DB::SqlConnection::Exception::Base)
    {
        ReThrowMsg(PluginDAO::Exception::DatabaseError,
                   "Failed in UnregisterPlugin");
    }
}

} // namespace WrtDB
