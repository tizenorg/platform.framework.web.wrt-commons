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
 * @file    feature_dao_read_only.cpp
 * @author  Krzysztof Jackiewicz (k.jackiewicz@samsung.com)
 * @version 1.0
 * @brief   This file contains the implementation of feature dao read only
 */
#include <stddef.h>
#include <dpl/wrt-dao-ro/feature_dao_read_only.h>
#include <sstream>
#include <dpl/log/log.h>
#include <dpl/foreach.h>
#include <dpl/db/orm.h>
#include <orm_generator_wrt.h>
#include <dpl/wrt-dao-ro/webruntime_database.h>
#include <dpl/wrt-dao-ro/plugin_dao_read_only.h>

namespace WrtDB {
FeatureDAOReadOnly::FeatureDAOReadOnly(FeatureHandle featureHandle) :
    m_featureHandle(featureHandle)
{
    if (!isFeatureInstalled(m_featureHandle)) {
        std::ostringstream exc;
        exc << "Feature " << m_featureHandle << " not installed.";
        LogError(exc.str());
        ThrowMsg(FeatureDAOReadOnly::Exception::FeatureNotExist, exc.str());
    }
}

FeatureDAOReadOnly::FeatureDAOReadOnly(const std::string &featureName)
{
    LogDebug("FeatureDAOReadOnly ( " << featureName << " )");
    Try {
        using namespace DPL::DB::ORM;
        using namespace DPL::DB::ORM::wrt;
        WRT_DB_SELECT(select, FeaturesList, &WrtDatabase::interface())
        select->Where(Equals<FeaturesList::FeatureName>(
                          DPL::FromUTF8String(featureName)));

        m_featureHandle = select->GetSingleValue<FeaturesList::FeatureUUID>();
        LogDebug(" >> FeatureHandle retrieved: " << m_featureHandle);
    }
    Catch(DPL::DB::SqlConnection::Exception::Base){
        ReThrowMsg(FeatureDAOReadOnly::Exception::DatabaseError,
                   "Failure during creating FeatureDAOReadOnly");
    }
}

#define GET_PLUGIN_DATA(func)                                           \
    Try {                                                               \
        DPL::DB::ORM::wrt::ScopedTransaction transaction( \
            &WrtDatabase::interface()); \
        LogDebug(#func << ". FeatureHandle: " << m_featureHandle);     \
        std::string ret = PluginDAOReadOnly(GetPluginHandle()).func();  \
        transaction.Commit();                                           \
        return ret;                                                     \
    }                                                                   \
    Catch(DPL::DB::SqlConnection::Exception::Base) {                        \
        std::ostringstream failure("Failure during ");                  \
        failure << #func;                                              \
        ReThrowMsg(FeatureDAOReadOnly::Exception::DatabaseError,        \
                   failure.str());                                         \
    }

std::string FeatureDAOReadOnly::GetLibraryPath() const
{
    GET_PLUGIN_DATA(getLibraryPath)
}

std::string FeatureDAOReadOnly::GetLibraryName() const
{
    GET_PLUGIN_DATA(getLibraryName)
}

#undef GET_PLUGIN_DATA

FeatureHandle FeatureDAOReadOnly::GetFeatureHandle()  const
{
    return m_featureHandle;
}

std::string FeatureDAOReadOnly::GetName() const
{
    LogDebug("Getting Feature Name. Handle: " << m_featureHandle);
    Try {
        using namespace DPL::DB::ORM;
        using namespace DPL::DB::ORM::wrt;
        WRT_DB_SELECT(select, FeaturesList, &WrtDatabase::interface())
        select->Where(Equals<FeaturesList::FeatureUUID>(m_featureHandle));

        std::string ret = DPL::ToUTF8String(
                select->GetSingleValue< FeaturesList::FeatureName>());

        LogDebug(" >> Feature name: " << ret);
        return ret;
    }
    Catch(DPL::DB::SqlConnection::Exception::Base){
        ReThrowMsg(FeatureDAOReadOnly::Exception::DatabaseError,
                   "Failure during getting GetName");
    }
}

DbPluginHandle FeatureDAOReadOnly::GetPluginHandle() const
{
    LogDebug("Getting Plugin handle. FHandle: " << m_featureHandle);
    Try {
        using namespace DPL::DB::ORM;
        using namespace DPL::DB::ORM::wrt;
        WRT_DB_SELECT(select, FeaturesList, &WrtDatabase::interface())
        select->Where(Equals<FeaturesList::FeatureUUID>(m_featureHandle));

        DbPluginHandle pluginHandle =
            select->GetSingleValue< FeaturesList::PluginPropertiesId>();

        LogDebug(" >> Plugin Handle: " << pluginHandle);
        return pluginHandle;
    }
    Catch(DPL::DB::SqlConnection::Exception::Base){
        ReThrowMsg(FeatureDAOReadOnly::Exception::DatabaseError,
                   "Failure during getting Plugin Handle");
    }
}

FeatureHandleList FeatureDAOReadOnly::GetHandleList()
{
    LogDebug("Getting FeatureHandle list.");
    Try {
        using namespace DPL::DB::ORM;
        using namespace DPL::DB::ORM::wrt;
        WRT_DB_SELECT(select, FeaturesList, &WrtDatabase::interface())

        FeatureHandleList ret =
            select->GetValueList<FeaturesList::FeatureUUID>();

        std::ostringstream handles;
        FOREACH(it, ret)
        handles << *it << " ";
        LogDebug(" >> FeatureHandle list retrieved: (" << handles << ")");

        return ret;
    }
    Catch(DPL::DB::SqlConnection::Exception::Base){
        ReThrowMsg(FeatureDAOReadOnly::Exception::DatabaseError,
                   "Failure during getting FeatureHandle list");
    }
}

bool FeatureDAOReadOnly::isFeatureInstalled(const std::string &featureName)
{
    LogDebug("Check if Feature is installed. Name: " << featureName);
    Try {
        using namespace DPL::DB::ORM;
        using namespace DPL::DB::ORM::wrt;
        WRT_DB_SELECT(select, FeaturesList, &WrtDatabase::interface())
        select->Where(Equals<FeaturesList::FeatureName>(
                          DPL::FromUTF8String(featureName)));

        FeaturesList::Select::RowList rows = select->GetRowList();

        bool flag = !rows.empty();
        LogDebug(" >> Feature " << featureName <<
                 (flag ? " found." : " not found."));

        return flag;
    }
    Catch(DPL::DB::SqlConnection::Exception::Base) {
        ReThrowMsg(FeatureDAOReadOnly::Exception::DatabaseError,
                   "Failure during checking if Feature is installed");
    }
}

bool FeatureDAOReadOnly::isFeatureInstalled(FeatureHandle handle)
{
    LogDebug("Check if Feature is installed. Handle: " << handle);
    Try
    {
        using namespace DPL::DB::ORM;
        using namespace DPL::DB::ORM::wrt;
        WRT_DB_SELECT(select, FeaturesList, &WrtDatabase::interface())
        select->Where(Equals<FeaturesList::FeatureUUID>(handle));

        FeaturesList::Select::RowList rows = select->GetRowList();

        bool flag = !rows.empty();
        LogDebug(" >> Feature " << handle <<
                 (flag ? " found." : " not found."));

        return flag;
    }
    Catch(DPL::DB::SqlConnection::Exception::Base){
        ReThrowMsg(FeatureDAOReadOnly::Exception::DatabaseError,
                   "Failure during checking if Feature is installed");
    }
}

bool FeatureDAOReadOnly::isDeviceCapabilityInstalled(
    const std::string &deviceCapName)
{
    LogDebug("Check if DeviceCap is installed. Name: " << deviceCapName);
    Try {
        using namespace DPL::DB::ORM;
        using namespace DPL::DB::ORM::wrt;
        WRT_DB_SELECT(select, DeviceCapabilities, &WrtDatabase::interface())
        select->Where(Equals<DeviceCapabilities::DeviceCapName>(
                          DPL::FromUTF8String(deviceCapName)));

        DeviceCapabilities::Select::RowList rows = select->GetRowList();

        bool flag = !rows.empty();
        LogDebug(" >> Device Cap " << deviceCapName <<
                 (flag ? "found." : "not found."));

        return flag;
    }
    Catch(DPL::DB::SqlConnection::Exception::Base){
        ReThrowMsg(FeatureDAOReadOnly::Exception::DatabaseError,
                   "Failure during checking if DeviceCap is installed");
    }
}

FeatureDAOReadOnly::DeviceCapabilitiesList
FeatureDAOReadOnly::GetDeviceCapabilities() const
{
    Try {
        LogDebug("Get DeviceCap. FeatureHandle: " << m_featureHandle);
        DPL::DB::ORM::wrt::ScopedTransaction transaction(
            &WrtDatabase::interface());

        using namespace DPL::DB::ORM;
        using namespace DPL::DB::ORM::wrt;
        WRT_DB_SELECT(selectDevCP,
                      FeatureDeviceCapProxy,
                      &WrtDatabase::interface())
        selectDevCP->Where(Equals<FeatureDeviceCapProxy::FeatureUUID>(
                               m_featureHandle));

        DeviceCapabilitiesList devCap;

        std::list<int> deviceIDs =
            selectDevCP->GetValueList<FeatureDeviceCapProxy::DeviceCapID>();
        FOREACH(devCId, deviceIDs)
        {
            WRT_DB_SELECT(selectDevC,
                          DeviceCapabilities,
                          &WrtDatabase::interface())
            selectDevC->Where(Equals<DeviceCapabilities::DeviceCapID>(*devCId));

            DPL::String devNames =
                selectDevC->GetSingleValue<DeviceCapabilities::DeviceCapName>();

            devCap.insert(DPL::ToUTF8String(devNames));
        }

        transaction.Commit();
        return devCap;
    }
    Catch(DPL::DB::SqlConnection::Exception::Base){
        ReThrowMsg(FeatureDAOReadOnly::Exception::DatabaseError,
                   "Failure during getting DeviceCapabilities names");
    }
}

FeatureHandleListPtr FeatureDAOReadOnly::GetFeatureHandleListForPlugin(
    DbPluginHandle pluginHandle)
{
    LogDebug("Getting FeatureHandle list for pluginHandle: " << pluginHandle);
    Try {
        using namespace DPL::DB::ORM;
        using namespace DPL::DB::ORM::wrt;

        WRT_DB_SELECT(select, FeaturesList, &WrtDatabase::interface())
        select->Where(Equals<FeaturesList::PluginPropertiesId>(pluginHandle));

        FeatureHandleListPtr handles(new FeatureHandleList);
        FeatureHandleList ret =
            select->GetValueList<FeaturesList::FeatureUUID>();

        FOREACH(it, ret)
        {
            LogDebug("feature handle: " << *it);
            handles->push_back(*it);
        }

        return handles;
    }
    Catch(DPL::DB::SqlConnection::Exception::Base){
        ReThrowMsg(
            FeatureDAOReadOnly::Exception::DatabaseError,
            "Failure during getting FeatureHandle Set for plugin handle");
    }
}

FeatureDAOReadOnly::NameMap
FeatureDAOReadOnly::GetNames()
{
    Try {
        using namespace DPL::DB::ORM;
        using namespace DPL::DB::ORM::wrt;
        WRT_DB_SELECT(select, FeaturesList, &WrtDatabase::interface())

        NameMap nameMap;

        FeaturesList::Select::RowList rows = select->GetRowList();
        FOREACH(rowIt, rows)
        {
            nameMap.insert(std::pair<FeatureHandle,
                                     std::string>(rowIt->Get_FeatureUUID(),
                                                  DPL::ToUTF8String(rowIt->
                                                                        Get_FeatureName())));
        }

        return nameMap;
    }
    Catch(DPL::DB::SqlConnection::Exception::Base){
        ReThrowMsg(FeatureDAOReadOnly::Exception::DatabaseError,
                   "Failure during getting GetNames");
    }
}

FeatureDAOReadOnly::DeviceCapabilitiesMap
FeatureDAOReadOnly::GetDevCapWithFeatureHandle()
{
    Try {
        using namespace DPL::DB::ORM;
        using namespace DPL::DB::ORM::wrt;

        DECLARE_COLUMN_TYPE_LIST()
        SELECTED_COLUMN(FeatureDeviceCapProxy, FeatureUUID)
        SELECTED_COLUMN(DeviceCapabilities, DeviceCapName)
        DECLARE_COLUMN_TYPE_LIST_END(DevCapNameList)

        WRT_DB_SELECT(select, FeatureDeviceCapProxy, &WrtDatabase::interface())
        select->Join<DevCapNameList>(Equal<FeatureDeviceCapProxy::DeviceCapID,
                                           DeviceCapabilities::DeviceCapID>());

        DeviceCapabilitiesMap devCap;

        std::list< CustomRow<DevCapNameList> > rowList =
            select->GetCustomRowList< DevCapNameList, CustomRow<DevCapNameList> >();
        FOREACH(rowIt, rowList)
        {
            FeatureHandle featureHandle =
                (*rowIt).GetColumnData<FeatureDeviceCapProxy::FeatureUUID>();
            std::string devName =
                DPL::ToUTF8String((*rowIt).GetColumnData<DeviceCapabilities::
                                                             DeviceCapName>());
            devCap.insert(std::pair<FeatureHandle, std::string>(featureHandle,
                                                                devName));
        }

        return devCap;
    }
    Catch(DPL::DB::SqlConnection::Exception::Base){
        ReThrowMsg(FeatureDAOReadOnly::Exception::DatabaseError,
                   "Failure during getting DeviceCapabilities names");
    }
}

FeatureDAOReadOnly::FeatureMap
FeatureDAOReadOnly::GetFeatures(const std::list<std::string>& featureNames)
{
    Try {
        using namespace DPL::DB::ORM;
        using namespace DPL::DB::ORM::wrt;

        std::set<typename FeaturesList::FeatureName::ColumnType> nameList;
        FOREACH(nm, featureNames) {
            nameList.insert(DPL::FromUTF8String(*nm));
        }

        WRT_DB_SELECT(select, FeaturesList, &WrtDatabase::interface())
        select->Where(In<FeaturesList::FeatureName>(nameList));

        FeatureMap featureMap;
        FeatureData featureData;
        FeaturesList::Select::RowList rows = select->GetRowList();
        FOREACH(rowIt, rows) {
            featureData.featureName = DPL::ToUTF8String(
                    rowIt->Get_FeatureName());
            featureData.pluginHandle = rowIt->Get_PluginPropertiesId();
            featureMap.insert(std::pair<FeatureHandle, FeatureData>(
                                  rowIt->Get_FeatureUUID(), featureData));
        }

        return featureMap;
    }
    Catch(DPL::DB::SqlConnection::Exception::Base){
        ReThrowMsg(FeatureDAOReadOnly::Exception::DatabaseError,
                   "Failure during getting GetFeatures");
    }
}
} // namespace WrtDB
