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
 * This file contains the declaration of widget dao class.
 *
 * @file    widget_dao_read_only.cpp
 * @author  Yang Jie (jie2.yang@samsung.com)
 * @author  Przemyslaw Dobrowolski (p.dobrowolsk@samsung.com)
 * @author  Pawel Sikorski (p.sikorski@samsung.com)
 * @version 1.0
 * @brief   This file contains the declaration of widget dao
 */

#include <dpl/wrt-dao-ro/widget_dao_read_only.h>

#include <sstream>
#include <dpl/foreach.h>
#include <dpl/sstream.h>
#include <dpl/wrt-dao-ro/global_config.h>
#include <dpl/wrt-dao-ro/webruntime_database.h>
#include <dpl/wrt-dao-ro/WrtDatabase.h>
#include <dpl/wrt-dao-ro/widget_config.h>
#include <dpl/wrt-dao-ro/feature_dao_read_only.h>
#include <orm_generator_wrt.h>

namespace WrtDB {

//TODO in current solution in each getter there exists a check
//"IsWidgetInstalled". Maybe it should be verified, if it could be done
//differently  (check in WidgetDAOReadOnly constructor)

#define SQL_CONNECTION_EXCEPTION_HANDLER_BEGIN          Try

#define SQL_CONNECTION_EXCEPTION_HANDLER_END(message)           \
    Catch(DPL::DB::SqlConnection::Exception::Base) {                \
        LogError(message);                                      \
        ReThrowMsg(WidgetDAOReadOnly::Exception::DatabaseError, \
                   message);                                    \
    }

#define CHECK_WIDGET_EXISTENCE(macro_transaction, macro_handle)          \
    if (!WidgetDAOReadOnly::isWidgetInstalled(macro_handle))             \
    {                                                                    \
        macro_transaction.Commit();                                      \
        LogWarning("Cannot find widget. Handle: " << macro_handle);      \
        ThrowMsg(WidgetDAOReadOnly::Exception::WidgetNotExist,           \
                 "Cannot find widget. Handle: " << macro_handle);        \
    }


typedef DPL::DB::ORM::wrt::WidgetInfo::Row WidgetInfoRow;
typedef DPL::DB::ORM::wrt::WidgetFeature::widget_feature_id::ColumnType
        WidgetFeatureId;

namespace {

WidgetInfoRow getWidgetInfoRow(int widgetHandle)
{
    LogDebug("Getting WidgetInfo row. Handle: " << widgetHandle);
    SQL_CONNECTION_EXCEPTION_HANDLER_BEGIN
    {
        using namespace DPL::DB::ORM;
        using namespace DPL::DB::ORM::wrt;
        WRT_DB_SELECT(select, WidgetInfo, &WrtDatabase::interface())
        select->Where(Equals<WidgetInfo::app_id>(widgetHandle));

        WidgetInfo::Select::RowList rows = select->GetRowList();
        if (rows.empty()) {
            ThrowMsg(WidgetDAOReadOnly::Exception::WidgetNotExist,
                     "Cannot find widget. Handle: " << widgetHandle);
        }
        return rows.front();
    }
    SQL_CONNECTION_EXCEPTION_HANDLER_END("Failed in GetWidgetInfoRow")
}

} // namespace


IWacSecurity::~IWacSecurity()
{
}

WidgetDAOReadOnly::WidgetDAOReadOnly(DbWidgetHandle widgetHandle) :
    m_widgetHandle(widgetHandle)
{
}

WidgetDAOReadOnly::WidgetDAOReadOnly(DPL::OptionalString widgetGUID) :
    m_widgetHandle(WidgetDAOReadOnly::getHandle(widgetGUID))
{
}

WidgetDAOReadOnly::~WidgetDAOReadOnly()
{
}

DbWidgetHandle WidgetDAOReadOnly::getHandle() const
{
    return m_widgetHandle;
}

DbWidgetHandle WidgetDAOReadOnly::getHandle(const WidgetGUID GUID)
{
    LogDebug("Getting WidgetHandle by GUID [" << GUID << "]");

    SQL_CONNECTION_EXCEPTION_HANDLER_BEGIN
    {
        using namespace DPL::DB::ORM;
        using namespace DPL::DB::ORM::wrt;
        WRT_DB_SELECT(select, WidgetInfo, &WrtDatabase::interface())
        select->Where(Equals<WidgetInfo::widget_id>(GUID));
        WidgetInfo::Select::RowList rowList = select->GetRowList();

        if (rowList.empty()) {
            ThrowMsg(WidgetDAOReadOnly::Exception::WidgetNotExist,
                 "Failed to get widget by guid");
        }
        return rowList.front().Get_app_id();
    }
    SQL_CONNECTION_EXCEPTION_HANDLER_END("Failed in getHandle")

    ThrowMsg(WidgetDAOReadOnly::Exception::WidgetNotExist,
                     "Failed to get widget by guid");
}

DbWidgetHandle WidgetDAOReadOnly::getHandle(const DPL::String pkgName)
{
    LogDebug("Getting WidgetHandle by Package Name [" << pkgName << "]");

    SQL_CONNECTION_EXCEPTION_HANDLER_BEGIN
    {
        using namespace DPL::DB::ORM;
        using namespace DPL::DB::ORM::wrt;
        WRT_DB_SELECT(select, WidgetInfo, &WrtDatabase::interface())
        select->Where(Equals<WidgetInfo::pkgname>(pkgName));
        WidgetInfo::Select::RowList rowList = select->GetRowList();

        if (rowList.empty()) {
            ThrowMsg(WidgetDAOReadOnly::Exception::WidgetNotExist,
                 "Failed to get widget by package name");
        }
        return rowList.front().Get_app_id();
    }
    SQL_CONNECTION_EXCEPTION_HANDLER_END("Failed in getHandle")

    ThrowMsg(WidgetDAOReadOnly::Exception::WidgetNotExist,
                     "Failed to get widget by package name");
}

PropertyDAOReadOnly::WidgetPropertyKeyList
WidgetDAOReadOnly::getPropertyKeyList() const
{
    return PropertyDAOReadOnly::GetPropertyKeyList(m_widgetHandle);
}

PropertyDAOReadOnly::WidgetPreferenceList
WidgetDAOReadOnly::getPropertyList() const
{
    return PropertyDAOReadOnly::GetPropertyList(m_widgetHandle);
}

PropertyDAOReadOnly::WidgetPropertyValue WidgetDAOReadOnly::getPropertyValue(
        const PropertyDAOReadOnly::WidgetPropertyKey &key) const
{
    return PropertyDAOReadOnly::GetPropertyValue(m_widgetHandle, key);
}

DPL::OptionalInt WidgetDAOReadOnly::checkPropertyReadFlag(
        const PropertyDAOReadOnly::WidgetPropertyKey &key) const
{
    return PropertyDAOReadOnly::CheckPropertyReadFlag(m_widgetHandle, key);
}

DPL::String WidgetDAOReadOnly::getPath() const
{
    DPL::String path = DPL::FromUTF8String(
            GlobalConfig::GetUserInstalledWidgetPath());
    DPL::String srcPath = DPL::FromUTF8String(GlobalConfig::GetWidgetSrcPath());

    bool isFactoryWidget = isFactory();

    if (isFactoryWidget) {
        WidgetGUID widgetGUID = getGUID();
        if (!widgetGUID) {
            Throw(WidgetDAOReadOnly::Exception::GUIDisNull);
        }
        path += L"/" + *widgetGUID + L"/";
    } else {
        // if the widget is a "downloaded widget",
        // use unique package name.
        DPL::OStringStream strAppId;
        strAppId << m_widgetHandle;
        DPL::OptionalString pkgname = getPkgname();
        path += L"/" + *pkgname + L"/";
        path += srcPath + L"/";
    }

    return path;
}

DPL::String WidgetDAOReadOnly::getFullPath() const
{
    return L"file://" + getPath();
}

WidgetLocalizedInfo
        WidgetDAOReadOnly::getLocalizedInfo(const DPL::String& languageTag)
        const
{
    LogDebug("Getting Localized Info. Handle: " << m_widgetHandle);
    SQL_CONNECTION_EXCEPTION_HANDLER_BEGIN
    {
        DPL::DB::ORM::wrt::ScopedTransaction transaction(&WrtDatabase::interface());
        CHECK_WIDGET_EXISTENCE(transaction, m_widgetHandle)

        using namespace DPL::DB::ORM;
        using namespace DPL::DB::ORM::wrt;
        WRT_DB_SELECT(select, LocalizedWidgetInfo, &WrtDatabase::interface())
        select->Where(
            And(Equals<LocalizedWidgetInfo::app_id>(m_widgetHandle),
                Equals<LocalizedWidgetInfo::widget_locale>(languageTag)));
        LocalizedWidgetInfo::Row info = select->GetSingleRow();
        WidgetLocalizedInfo result;

        result.name = info.Get_widget_name();
        result.shortName = info.Get_widget_shortname();
        result.description = info.Get_widget_description();
        result.license = info.Get_widget_license();
        result.licenseHref = info.Get_widget_license_href();

        transaction.Commit();
        return result;
    }
    SQL_CONNECTION_EXCEPTION_HANDLER_END("Failed to get localized info")
}

DbWidgetFeatureSet WidgetDAOReadOnly::getFeaturesList() const
{
    LogDebug("Getting FeaturesList. Handle: " << m_widgetHandle);
    SQL_CONNECTION_EXCEPTION_HANDLER_BEGIN
    {
        DPL::DB::ORM::wrt::ScopedTransaction transaction(&WrtDatabase::interface());
        CHECK_WIDGET_EXISTENCE(transaction, m_widgetHandle)

        using namespace DPL::DB::ORM;
        using namespace DPL::DB::ORM::wrt;
        WRT_DB_SELECT(select, DPL::DB::ORM::wrt::WidgetFeature, &WrtDatabase::interface())
        select->Where(Equals<DPL::DB::ORM::wrt::WidgetFeature::app_id>(m_widgetHandle));

        DbWidgetFeatureSet resultSet;
        typedef std::list<DPL::DB::ORM::wrt::WidgetFeature::Row> RowList;
        RowList list = select->GetRowList();

        for (RowList::iterator i = list.begin(); i != list.end(); ++i) {
            DbWidgetFeature feature;
            feature.name = i->Get_name();
            feature.required = i->Get_required();
            feature.rejected = i->Get_rejected();
            feature.params = getFeatureParams(i->Get_widget_feature_id());
            FeatureDAOReadOnly featureDao(DPL::ToUTF8String(i->Get_name()));
            feature.pluginId = featureDao.GetPluginHandle();
            resultSet.insert(feature);
        }
        transaction.Commit();
        return resultSet;
    }
    SQL_CONNECTION_EXCEPTION_HANDLER_END("Failed to get features list")
}

WidgetParamMap WidgetDAOReadOnly::getFeatureParams(int id)
{
    WidgetFeatureId widgetFeatureId(id);
    LogDebug("Getting feature Params. featureId: " << widgetFeatureId);
    SQL_CONNECTION_EXCEPTION_HANDLER_BEGIN
    {
        using namespace DPL::DB::ORM;
        using namespace DPL::DB::ORM::wrt;
        WRT_DB_SELECT(select, DPL::DB::ORM::wrt::FeatureParam, &WrtDatabase::interface())
        select->Where(Equals<DPL::DB::ORM::wrt::FeatureParam::widget_feature_id>(
                          widgetFeatureId));

        WidgetParamMap resultMap;
        typedef std::list<DPL::DB::ORM::wrt::FeatureParam::Row> RowList;
        RowList list = select->GetRowList();

        FOREACH(i, list)
            resultMap.insert(std::make_pair(i->Get_name(), i->Get_value()));

        return resultMap;
    }
    SQL_CONNECTION_EXCEPTION_HANDLER_END("Failed to get feature params")
}

bool WidgetDAOReadOnly::hasFeature(const std::string& featureName) const
{
    LogDebug(
        "Checking if widget has feature: " << featureName << ". Handle: " <<
        m_widgetHandle);
    SQL_CONNECTION_EXCEPTION_HANDLER_BEGIN
    {
        DPL::DB::ORM::wrt::ScopedTransaction transaction(&WrtDatabase::interface());
        CHECK_WIDGET_EXISTENCE(transaction, m_widgetHandle)

        using namespace DPL::DB::ORM;
        using namespace DPL::DB::ORM::wrt;
        WRT_DB_SELECT(select, wrt::WidgetFeature, &WrtDatabase::interface())
        select->Where(And(Equals<wrt::WidgetFeature::app_id>(m_widgetHandle),
                          Equals<wrt::WidgetFeature::name>(
                              DPL::FromUTF8String(featureName))));

        wrt::WidgetFeature::Select::RowList rows = select->GetRowList();
        transaction.Commit();
        return !rows.empty();
    }
    SQL_CONNECTION_EXCEPTION_HANDLER_END("Failed to check for feature")
}

HostList WidgetDAOReadOnly::getAccessHostList() const
{
    LogDebug("Getting AccessHostList. Handle: " << m_widgetHandle);
    SQL_CONNECTION_EXCEPTION_HANDLER_BEGIN
    {
        DPL::DB::ORM::wrt::ScopedTransaction transaction(&WrtDatabase::interface());
        CHECK_WIDGET_EXISTENCE(transaction, m_widgetHandle)

        using namespace DPL::DB::ORM;
        using namespace DPL::DB::ORM::wrt;
        WRT_DB_SELECT(select, WidgetAccessHost, &WrtDatabase::interface())
        select->Where(Equals<WidgetAccessHost::app_id>(m_widgetHandle));
        std::list<WidgetAccessHost::host::ColumnType> values =
            select->GetValueList<WidgetAccessHost::host>();
        HostList ret;
        FOREACH(it, values)
        ret.push_back(DPL::ToUTF8String(*it));

        transaction.Commit();
        return ret;
    }
    SQL_CONNECTION_EXCEPTION_HANDLER_END("Failed to get access host list")
}

bool WidgetDAOReadOnly::getAccessNetworkMode() const
{
    //TODO there is no column access_network
    //it was removed in "Widget localization overhaul
    return true;
}

DbWidgetHandleList WidgetDAOReadOnly::getHandleList()
{
    LogDebug("Getting DbWidgetHandle List");
    SQL_CONNECTION_EXCEPTION_HANDLER_BEGIN
    {
        using namespace DPL::DB::ORM;
        using namespace DPL::DB::ORM::wrt;
        WRT_DB_SELECT(select, WidgetInfo, &WrtDatabase::interface())
        return select->GetValueList<WidgetInfo::app_id>();
    }
    SQL_CONNECTION_EXCEPTION_HANDLER_END("Failed to get handle list")
}

bool WidgetDAOReadOnly::isWidgetInstalled(DbWidgetHandle handle)
{
    LogDebug("Checking if widget exist. Handle: " << handle);
    SQL_CONNECTION_EXCEPTION_HANDLER_BEGIN
    {
        using namespace DPL::DB::ORM;
        using namespace DPL::DB::ORM::wrt;
        WRT_DB_SELECT(select, WidgetInfo, &WrtDatabase::interface())
        select->Where(Equals<WidgetInfo::app_id>(handle));

        WidgetInfo::Select::RowList rows = select->GetRowList();

        return !rows.empty();
    }
    SQL_CONNECTION_EXCEPTION_HANDLER_END("Failed to check if widget exist")
}

bool WidgetDAOReadOnly::isWidgetInstalled(DPL::String pkgName)
{
    LogDebug("Checking if widget exist. package name " << pkgName);
    SQL_CONNECTION_EXCEPTION_HANDLER_BEGIN
    {
        using namespace DPL::DB::ORM;
        using namespace DPL::DB::ORM::wrt;
        WRT_DB_SELECT(select, WidgetInfo, &WrtDatabase::interface())
        select->Where(Equals<WidgetInfo::pkgname>(pkgName));

        WidgetInfo::Select::RowList rows = select->GetRowList();

        return !rows.empty();
    }
    SQL_CONNECTION_EXCEPTION_HANDLER_END("Failed to check if widget exist")
}

CertificateChainList WidgetDAOReadOnly::getWidgetCertificate() const
{
    using namespace DPL::DB::ORM;
    using namespace DPL::DB::ORM::wrt;
    WRT_DB_SELECT(select, WidgetCertificate, &WrtDatabase::interface())
    select->Where(Equals<WidgetCertificate::app_id>(m_widgetHandle));

    std::list<DPL::DB::ORM::wrt::WidgetCertificate::Row> chainList = select->GetRowList();

    CertificateChainList result;

    FOREACH(iter, chainList)
        result.push_back(DPL::ToUTF8String(iter->Get_encoded_chain()));
    return result;
}

DbWidgetSize WidgetDAOReadOnly::getPreferredSize() const
{
    WidgetInfoRow row = getWidgetInfoRow(m_widgetHandle);

    DbWidgetSize size;
    size.width = row.Get_widget_width();
    size.height = row.Get_widget_height();

    LogDebug("Return size wxh = " <<
             (!!size.width ? *size.width : -1) << " x " <<
             (!!size.height ? *size.height : -1));

    return size;
}

WidgetType WidgetDAOReadOnly::getWidgetType() const
{
    WidgetInfoRow row = getWidgetInfoRow(m_widgetHandle);
    DPL::OptionalInt result = row.Get_widget_type();
    return WidgetType(static_cast<AppType>(*result));
}

WidgetGUID WidgetDAOReadOnly::getGUID() const
{
    WidgetInfoRow row = getWidgetInfoRow(m_widgetHandle);
    return row.Get_widget_id();
}

DPL::OptionalString WidgetDAOReadOnly::getPkgname() const
{
    WidgetInfoRow row = getWidgetInfoRow(m_widgetHandle);
    return row.Get_pkgname();
}

DPL::OptionalString WidgetDAOReadOnly::getDefaultlocale() const
{
    WidgetInfoRow row = getWidgetInfoRow(m_widgetHandle);
    return row.Get_defaultlocale();
}

DPL::Optional<DPL::String> WidgetDAOReadOnly::getVersion() const
{
    WidgetInfoRow row = getWidgetInfoRow(m_widgetHandle);
    return row.Get_widget_version();
}

DPL::Optional<DPL::String> WidgetDAOReadOnly::getAuthorName() const
{
    WidgetInfoRow row = getWidgetInfoRow(m_widgetHandle);
    return row.Get_author_name();
}

DPL::Optional<DPL::String> WidgetDAOReadOnly::getAuthorEmail() const
{
    WidgetInfoRow row = getWidgetInfoRow(m_widgetHandle);
    return row.Get_author_email();
}

DPL::Optional<DPL::String> WidgetDAOReadOnly::getAuthorHref() const
{
    WidgetInfoRow row = getWidgetInfoRow(m_widgetHandle);
    return row.Get_author_href();
}

DPL::Optional<DPL::String> WidgetDAOReadOnly::getMinimumWacVersion() const
{
    WidgetInfoRow row = getWidgetInfoRow(m_widgetHandle);
    return row.Get_min_version();
}

std::string WidgetDAOReadOnly::getShareHref() const
{
    SQL_CONNECTION_EXCEPTION_HANDLER_BEGIN
    {
        using namespace DPL::DB::ORM;
        using namespace DPL::DB::ORM::wrt;
        WRT_DB_SELECT(select, WidgetExtendedInfo, &WrtDatabase::interface())
        select->Where(Equals<WidgetExtendedInfo::app_id>(m_widgetHandle));
        WidgetExtendedInfo::Select::RowList rows = select->GetRowList();

        if (rows.empty()) {
            ThrowMsg(WidgetDAOReadOnly::Exception::WidgetNotExist,
                     "Cannot find widget. Handle: " << m_widgetHandle);
        }

        DPL::Optional<DPL::String> value = rows.front().Get_share_href();
        std::string ret = "";
        if (!value.IsNull()) {
            ret = DPL::ToUTF8String(*value);
        }
        return ret;
    }
    SQL_CONNECTION_EXCEPTION_HANDLER_END("Failed to get share HREF")
}

bool WidgetDAOReadOnly::getBackSupported() const
{
    WidgetInfoRow row = getWidgetInfoRow(m_widgetHandle);
    return row.Get_back_supported();
}

bool WidgetDAOReadOnly::isRecognized() const
{
    WidgetInfoRow row = getWidgetInfoRow(m_widgetHandle);
    DPL::OptionalInt result = row.Get_recognized();
    if (result.IsNull()) {
        return false;
    }
    return static_cast<bool>(*result);
}

bool WidgetDAOReadOnly::isWacSigned() const
{
    WidgetInfoRow row = getWidgetInfoRow(m_widgetHandle);
    DPL::OptionalInt result = row.Get_wac_signed();
    if (result.IsNull()) {
        return false;
    }
    return static_cast<bool>(*result);
}

bool WidgetDAOReadOnly::isDistributorSigned() const
{
    WidgetInfoRow row = getWidgetInfoRow(m_widgetHandle);
    DPL::OptionalInt result = row.Get_distributor_signed();
    if (result.IsNull()) {
        return false;
    }
    return static_cast<bool>(*result);
}

bool WidgetDAOReadOnly::isTrusted() const
{
    // SP-2100
    // widget with verified distributor signature is trusted
    return isDistributorSigned();
}

bool WidgetDAOReadOnly::isTestWidget() const
{
    Try {
        using namespace DPL::DB::ORM;
        using namespace DPL::DB::ORM::wrt;
        WRT_DB_SELECT(select, WidgetExtendedInfo, &WrtDatabase::interface())
        select->Where(Equals<WidgetExtendedInfo::app_id>(m_widgetHandle));

        WidgetExtendedInfo::Select::RowList rows = select->GetRowList();
        if (rows.empty()) {
            ThrowMsg(WidgetDAOReadOnly::Exception::WidgetNotExist,
                     "Cannot find widget. Handle: " << m_widgetHandle);
        }

        return static_cast<bool>(rows.front().Get_test_widget());
    }
    Catch(DPL::DB::SqlConnection::Exception::Base){
        ReThrowMsg(WidgetDAOReadOnly::Exception::DatabaseError,
                   "Failed to check IsTestWidget");
    }
}

bool WidgetDAOReadOnly::getWebkitPluginsRequired() const
{
    WidgetInfoRow row = getWidgetInfoRow(m_widgetHandle);
    DPL::OptionalInt ret = row.Get_webkit_plugins_required();

    if (ret.IsNull() || *ret == 0) { return false; } else { return true; }
}

bool WidgetDAOReadOnly::isFactory() const
{
    SQL_CONNECTION_EXCEPTION_HANDLER_BEGIN
    {
        using namespace DPL::DB::ORM;
        using namespace DPL::DB::ORM::wrt;
        WRT_DB_SELECT(select, WidgetExtendedInfo, &WrtDatabase::interface())
        select->Where(Equals<WidgetExtendedInfo::app_id>(m_widgetHandle));

        WidgetExtendedInfo::Select::RowList rows = select->GetRowList();
        if (rows.empty()) {
            ThrowMsg(WidgetDAOReadOnly::Exception::WidgetNotExist,
                     "Cannot find widget. Handle: " << m_widgetHandle);
        }

        DPL::OptionalInt ret = rows.front().Get_factory_widget();
        if (ret.IsNull()) {
            return false;
        }
        return static_cast<bool>(*ret);
    }
    SQL_CONNECTION_EXCEPTION_HANDLER_END("Failed to get isFactory")
}

time_t WidgetDAOReadOnly::getInstallTime() const
{
    SQL_CONNECTION_EXCEPTION_HANDLER_BEGIN
    {
        using namespace DPL::DB::ORM;
        using namespace DPL::DB::ORM::wrt;
        WRT_DB_SELECT(select, WidgetExtendedInfo, &WrtDatabase::interface())
        select->Where(Equals<WidgetExtendedInfo::app_id>(m_widgetHandle));

        WidgetExtendedInfo::Select::RowList rows = select->GetRowList();
        if (rows.empty()) {
            ThrowMsg(WidgetDAOReadOnly::Exception::WidgetNotExist,
                     "Cannot find widget. Handle: " << m_widgetHandle);
        }

        return static_cast<time_t>(*rows.front().Get_install_time());
    }
    SQL_CONNECTION_EXCEPTION_HANDLER_END("Failed to get widdget install time")
}

DPL::OptionalString WidgetDAOReadOnly::getSplashImgSrc() const
{
    SQL_CONNECTION_EXCEPTION_HANDLER_BEGIN
    {
        using namespace DPL::DB::ORM;
        using namespace DPL::DB::ORM::wrt;
        WRT_DB_SELECT(select, WidgetExtendedInfo, &WrtDatabase::interface())
        select->Where(Equals<WidgetExtendedInfo::app_id>(m_widgetHandle));

        WidgetExtendedInfo::Select::RowList rows = select->GetRowList();
        if (rows.empty()) {
            ThrowMsg(WidgetDAOReadOnly::Exception::WidgetNotExist,
                     "Cannot find widget. Handle: " << m_widgetHandle);
        }

        DPL::OptionalString value = rows.front().Get_splash_img_src();
        if (value.IsNull()) {
            return DPL::OptionalString::Null;
        }

        return DPL::OptionalString(getPath() + *value);
    }
    SQL_CONNECTION_EXCEPTION_HANDLER_END("Failed to get splash image path")
}

WidgetDAOReadOnly::WidgetLocalizedIconList WidgetDAOReadOnly::getLocalizedIconList() const
{
    //TODO check widget existance??
    SQL_CONNECTION_EXCEPTION_HANDLER_BEGIN
    {
        using namespace DPL::DB::ORM;
        using namespace DPL::DB::ORM::wrt;
        WRT_DB_SELECT(select, WidgetLocalizedIcon, &WrtDatabase::interface())
        select->Where(Equals<WidgetLocalizedIcon::app_id>(m_widgetHandle));

        std::list<DPL::DB::ORM::wrt::WidgetLocalizedIcon::Row> list =
                select->GetRowList();
        WidgetLocalizedIconList ret;
        FOREACH(it,list)
        {
            WidgetLocalizedIconRow icon = {it->Get_app_id(),
                                           it->Get_icon_id(),
                                           it->Get_widget_locale()};
            ret.push_back(icon);
        }
        return ret;
    }
    SQL_CONNECTION_EXCEPTION_HANDLER_END("Failed to get icon data")
}

WidgetDAOReadOnly::WidgetIconList WidgetDAOReadOnly::getIconList() const
{
    //TODO check widget existance
    SQL_CONNECTION_EXCEPTION_HANDLER_BEGIN
    {
        using namespace DPL::DB::ORM;
        using namespace DPL::DB::ORM::wrt;
        WRT_DB_SELECT(select, wrt::WidgetIcon, &WrtDatabase::interface())
        select->Where(Equals<wrt::WidgetIcon::app_id>(m_widgetHandle));

        std::list<DPL::DB::ORM::wrt::WidgetIcon::Row> list =
                select->GetRowList();
        WidgetIconList ret;
        FOREACH(it,list)
        {
            WidgetIconRow icon = {it->Get_icon_id(),
                                  it->Get_app_id(),
                                  it->Get_icon_src(),
                                  it->Get_icon_width(),
                                  it->Get_icon_height()};
            ret.push_back(icon);
        }
        return ret;
    }
    SQL_CONNECTION_EXCEPTION_HANDLER_END("Failed to get icon data")
}

WidgetDAOReadOnly::LocalizedStartFileList WidgetDAOReadOnly::getLocalizedStartFileList() const
{
    //TODO check widget existance
    SQL_CONNECTION_EXCEPTION_HANDLER_BEGIN
    {
        using namespace DPL::DB::ORM;
        using namespace DPL::DB::ORM::wrt;
        WRT_DB_SELECT(select, WidgetLocalizedStartFile, &WrtDatabase::interface())
        select->Where(Equals<WidgetLocalizedStartFile::app_id>(
                          m_widgetHandle));
        select->OrderBy("start_file_id ASC");

        std::list<DPL::DB::ORM::wrt::WidgetLocalizedStartFile::Row> list =
                select->GetRowList();
        LocalizedStartFileList ret;
        FOREACH(it,list)
        {
            WidgetLocalizedStartFileRow file = {it->Get_start_file_id(),
                                                it->Get_app_id(),
                                                it->Get_widget_locale(),
                                                it->Get_type(),
                                                it->Get_encoding()};
            ret.push_back(file);
        }
        return ret;
    }
    SQL_CONNECTION_EXCEPTION_HANDLER_END("Failed to get start file list data")
}

WidgetDAOReadOnly::WidgetStartFileList WidgetDAOReadOnly::getStartFileList() const
{
    //TODO check widget existance
    SQL_CONNECTION_EXCEPTION_HANDLER_BEGIN
    {
        using namespace DPL::DB::ORM;
        using namespace DPL::DB::ORM::wrt;
        WRT_DB_SELECT(select, WidgetStartFile, &WrtDatabase::interface())
        select->Where(Equals<WidgetStartFile::app_id>(m_widgetHandle));
        select->OrderBy("start_file_id ASC");

        std::list<DPL::DB::ORM::wrt::WidgetStartFile::Row> list =
                select->GetRowList();
        WidgetStartFileList ret;
        FOREACH(it,list)
        {
            WidgetStartFileRow file = {it->Get_start_file_id(),
                                       it->Get_app_id(),
                                       it->Get_src()};
            ret.push_back(file);
        }
        return ret;
    }
    SQL_CONNECTION_EXCEPTION_HANDLER_END("Failed to get start file list data")
}

WindowModeList WidgetDAOReadOnly::getWindowModes() const
{
    //TODO check widget existance
    SQL_CONNECTION_EXCEPTION_HANDLER_BEGIN
    {
        using namespace DPL::DB::ORM;
        using namespace DPL::DB::ORM::wrt;
        WRT_DB_SELECT(select, WidgetWindowModes, &WrtDatabase::interface())
        select->Where(Equals<WidgetWindowModes::app_id>(m_widgetHandle));

        return select->GetValueList<WidgetWindowModes::window_mode>();
    }
    SQL_CONNECTION_EXCEPTION_HANDLER_END("Failed to get window modes")
}

std::string WidgetDAOReadOnly::getBaseFolder() const
{
    WidgetInfoRow row = getWidgetInfoRow(m_widgetHandle);
    DPL::Optional<DPL::String> ret = row.Get_base_folder();
    std::string baseFolder;
    if (!ret.IsNull()) {
        baseFolder = DPL::ToUTF8String(*ret);
    }

    if (!baseFolder.empty()) {
        baseFolder += "/";
    }

    return baseFolder;
}

bool WidgetDAOReadOnly::isDeletable() const
{
    return !WidgetDAOReadOnly::isFactory();
}

WidgetCertificateDataList WidgetDAOReadOnly::getCertificateDataList() const
{
    //TODO check widget existance
    SQL_CONNECTION_EXCEPTION_HANDLER_BEGIN
    {
        using namespace DPL::DB::ORM;
        using namespace DPL::DB::ORM::wrt;
        WRT_DB_SELECT(select, WidgetCertificateFingerprint, &WrtDatabase::interface())
        select->Where(Equals<WidgetCertificateFingerprint::app_id>(
                          m_widgetHandle));
        select->OrderBy("chainid");
        WidgetCertificateFingerprint::Select::RowList rows =
            select->GetRowList();

        WidgetCertificateDataList outlCertificateData;
        FOREACH(it, rows)
        {
            WidgetCertificateData data;

            data.owner =
                static_cast <WidgetCertificateData::Owner>(it->Get_owner());
            data.type =
                static_cast <WidgetCertificateData::Type>(it->Get_type());
            data.chainId = it->Get_chainid();
            DPL::Optional<DPL::String> md5 = it->Get_md5_fingerprint();
            data.strMD5Fingerprint =
                md5.IsNull() ? "" : DPL::ToUTF8String(*md5);
            DPL::Optional<DPL::String> sha1 = it->Get_sha1_fingerprint();
            data.strSHA1Fingerprint =
                sha1.IsNull() ? "" : DPL::ToUTF8String(*sha1);
            DPL::Optional<DPL::String> cname = it->Get_common_name();
            data.strCommonName =
                cname.IsNull() ? DPL::FromUTF8String("") : *cname;

            outlCertificateData.push_back(data);
        }
        return outlCertificateData;
    }
    SQL_CONNECTION_EXCEPTION_HANDLER_END("Failed to get fingerprint list")
}

FingerPrintList WidgetDAOReadOnly::getKeyFingerprints(
        WidgetCertificateData::Owner owner,
        WidgetCertificateData::Type type) const
{
    //TODO check widget existance
    SQL_CONNECTION_EXCEPTION_HANDLER_BEGIN
    {
        using namespace DPL::DB::ORM;
        using namespace DPL::DB::ORM::wrt;
        WRT_DB_SELECT(select, WidgetCertificateFingerprint, &WrtDatabase::interface())
        select->Where(And(And(
            Equals<WidgetCertificateFingerprint::app_id>(m_widgetHandle),
            Equals<WidgetCertificateFingerprint::owner>(owner)),
            Equals<WidgetCertificateFingerprint::type>(type)));

        WidgetCertificateFingerprint::Select::RowList rows =
            select->GetRowList();

        FingerPrintList keys;
        FOREACH(it, rows)
        {
            DPL::Optional<DPL::String> sha1 = it->Get_sha1_fingerprint();
            if (!sha1.IsNull()) {
                keys.push_back(DPL::ToUTF8String(*sha1));
            }
            DPL::Optional<DPL::String> md5 = it->Get_md5_fingerprint();
            if (!md5.IsNull()) {
                keys.push_back(DPL::ToUTF8String(*md5));
            }
        }
        return keys;
    }
    SQL_CONNECTION_EXCEPTION_HANDLER_END("Failed to get fingerprint list")
}

WidgetCertificateCNList WidgetDAOReadOnly::getKeyCommonNameList(
        WidgetCertificateData::Owner owner,
        WidgetCertificateData::Type type) const
{
    //TODO check widget existance
    SQL_CONNECTION_EXCEPTION_HANDLER_BEGIN
    {
        using namespace DPL::DB::ORM;
        using namespace DPL::DB::ORM::wrt;
        WRT_DB_SELECT(select, WidgetCertificateFingerprint, &WrtDatabase::interface())
        select->Where(And(And(
            Equals<WidgetCertificateFingerprint::app_id>(m_widgetHandle),
            Equals<WidgetCertificateFingerprint::owner>(owner)),
            Equals<WidgetCertificateFingerprint::type>(type)));

        WidgetCertificateFingerprint::Select::RowList rows =
            select->GetRowList();

        WidgetCertificateCNList out;
        FOREACH(it, rows)
        {
            DPL::Optional<DPL::String> cname = it->Get_common_name();
            out.push_back(cname.IsNull() ? "" : DPL::ToUTF8String(*cname));
        }
        return out;
    }
    SQL_CONNECTION_EXCEPTION_HANDLER_END("Failed to get key common name")
}

ResourceAttributeList WidgetDAOReadOnly::getResourceAttribute(
        const std::string &resourceId) const
{
    using namespace DPL::DB::ORM;
    using namespace DPL::DB::ORM::wrt;
    WRT_DB_SELECT(select, DPL::DB::ORM::wrt::WidgetFeature, &WrtDatabase::interface())
    select->Where(And(Equals<DPL::DB::ORM::wrt::WidgetFeature::app_id>(m_widgetHandle),
                      Equals<DPL::DB::ORM::wrt::WidgetFeature::name>(
                          DPL::FromUTF8String(resourceId))));

    std::list<DPL::DB::ORM::wrt::WidgetFeature::Row> list = select->GetRowList();
    ResourceAttributeList result;
    if (!list.empty()) {
        int widgetFeatureId = list.begin()->Get_widget_feature_id();
        WidgetParamMap map = getFeatureParams(widgetFeatureId);

        FOREACH(i, map)
            result.push_back(DPL::ToUTF8String(i->first));
    }
    return result;
}

void WidgetDAOReadOnly::getWidgetAccessInfo(
        WidgetAccessInfoList& outAccessInfoList) const
{
    //TODO check widget existance
    SQL_CONNECTION_EXCEPTION_HANDLER_BEGIN
    {
        using namespace DPL::DB::ORM;
        using namespace DPL::DB::ORM::wrt;
        WRT_DB_SELECT(select, WidgetWARPInfo, &WrtDatabase::interface())
        select->Where(Equals<WidgetWARPInfo::app_id>(m_widgetHandle));

        WidgetWARPInfo::Select::RowList rows = select->GetRowList();

        FOREACH(it, rows)
        {
            WidgetAccessInfo info;

            info.strIRI = it->Get_iri();
            DPL::OptionalInt access = it->Get_subdomain_access();
            if (access.IsNull() || 0 == *access) {
                info.bSubDomains = false;
            } else if (1 == *access) {
                info.bSubDomains = true;
            }

            outAccessInfoList.push_back(info);
        }
    }
    SQL_CONNECTION_EXCEPTION_HANDLER_END("Failed to get accessinfo list")
}

LanguageTagList WidgetDAOReadOnly::getLanguageTags() const
{
    //TODO check widget existance
    SQL_CONNECTION_EXCEPTION_HANDLER_BEGIN
    {
        using namespace DPL::DB::ORM;
        using namespace DPL::DB::ORM::wrt;
        WRT_DB_SELECT(select, LocalizedWidgetInfo, &WrtDatabase::interface())
        select->Where(Equals<LocalizedWidgetInfo::app_id>(m_widgetHandle));
        return select->GetValueList<LocalizedWidgetInfo::widget_locale>();
    }
    SQL_CONNECTION_EXCEPTION_HANDLER_END("Failed to get language tags")
}

LanguageTagList WidgetDAOReadOnly::getIconLanguageTags() const
{
    //TODO check widget existance
    using namespace DPL::DB::ORM;
    using namespace DPL::DB::ORM::wrt;
    WRT_DB_SELECT(select, WidgetLocalizedIcon, &WrtDatabase::interface())
    select->Where(Equals<WidgetLocalizedIcon::app_id>(m_widgetHandle));
    select->Distinct();
    return select->GetValueList<WidgetLocalizedIcon::widget_locale>();
}

std::string WidgetDAOReadOnly::getCookieDatabasePath() const
{
    using namespace WrtDB::WidgetConfig;
    std::ostringstream path;

    DPL::OptionalString pkgname = getPkgname();

    path << GetWidgetPersistentStoragePath(*pkgname);
    path << "/";
    path << GlobalConfig::GetCookieDatabaseFile();

    return path.str();
}

std::string WidgetDAOReadOnly::getPrivateLocalStoragePath() const
{
    std::ostringstream path;
    DPL::OptionalString pkgname = getPkgname();
    path << WidgetConfig::GetWidgetWebLocalStoragePath(*pkgname);
    path << "/";

    if (isFactory()) {
        WidgetGUID widgetGUID = getGUID();
        if (!widgetGUID) {
            Throw(WidgetDAOReadOnly::Exception::GUIDisNull);
        }
        path << DPL::ToUTF8String(*widgetGUID);
    }

    return path.str();
}

void WidgetDAOReadOnly::getWidgetSettings(
        WidgetSettings& outWidgetSettings) const
{
    //TODO check widget existance
    SQL_CONNECTION_EXCEPTION_HANDLER_BEGIN
    {
        using namespace DPL::DB::ORM;
        using namespace DPL::DB::ORM::wrt;
        WRT_DB_SELECT(select, SettginsList, &WrtDatabase::interface())
        select->Where(Equals<SettginsList::appId>(m_widgetHandle));

        SettginsList::Select::RowList rows = select->GetRowList();

        FOREACH(it, rows)
        {
            WidgetSetting info;

            info.settingName = it->Get_settingName();
            info.settingValue = it->Get_settingValue();
            outWidgetSettings.push_back(info);
        }
    }
    SQL_CONNECTION_EXCEPTION_HANDLER_END("Failed to get settings list")
}

void WidgetDAOReadOnly::getAppServiceList(
        WidgetApplicationServiceList& outAppServiceList) const
{
    LogDebug("Getting getAppServiceList. Handle: " << m_widgetHandle);
    SQL_CONNECTION_EXCEPTION_HANDLER_BEGIN
    {
        DPL::DB::ORM::wrt::ScopedTransaction transaction(&WrtDatabase::interface());
        CHECK_WIDGET_EXISTENCE(transaction, m_widgetHandle)

        using namespace DPL::DB::ORM;
        using namespace DPL::DB::ORM::wrt;
        WRT_DB_SELECT(select, ApplicationServiceInfo, &WrtDatabase::interface())
        select->Where(Equals<ApplicationServiceInfo::app_id>(m_widgetHandle));

        ApplicationServiceInfo::Select::RowList rows = select->GetRowList();

        if (rows.empty()) {
            LogDebug("Application Service list is empty. Handle: " <<
                     m_widgetHandle);
        }

        FOREACH(it, rows) {
            WidgetApplicationService ret;
            ret.src = it->Get_src();
            ret.operation = it->Get_operation();
            ret.scheme = it->Get_scheme();
            ret.mime = it->Get_mime();
            outAppServiceList.push_back(ret);
        }

        transaction.Commit();
    }
    SQL_CONNECTION_EXCEPTION_HANDLER_END("Failed to get access host list")
}

PkgType WidgetDAOReadOnly::getPkgType() const
{
    WidgetInfoRow row = getWidgetInfoRow(m_widgetHandle);
    DPL::OptionalInt result = row.Get_pkg_type();
    return PkgType(static_cast<PackagingType>(*result));
}

void WidgetDAOReadOnly::getEncryptedFileList(EncryptedFileList& filesList) const
{
    //TODO check widget existance
    using namespace DPL::DB::ORM;
    using namespace DPL::DB::ORM::wrt;
    WRT_DB_SELECT(select, EncryptedResourceList, &WrtDatabase::interface())
    select->Where(Equals<EncryptedResourceList::app_id>(m_widgetHandle));

    typedef std::list<DPL::DB::ORM::wrt::EncryptedResourceList::Row> RowList;
    RowList list = select->GetRowList();

    FOREACH(it, list) {
        EncryptedFileInfo info;
        info.fileName = it->Get_resource();
        info.fileSize = it->Get_size();
        filesList.insert(info);
    }
}

#undef SQL_CONNECTION_EXCEPTION_HANDLER_BEGIN
#undef SQL_CONNECTION_EXCEPTION_HANDLER_END
#undef CHECK_WIDGET_EXISTENCE

} // namespace WrtDB
