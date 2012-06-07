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
 * This file contains the definition of widget dao class.
 *
 * @file    widget_dao.cpp
 * @author  Przemyslaw Dobrowolski (p.dobrowolsk@samsung.com)
 * @author  Bartosz Janiak (b.janiak@samsung.com)
 * @author  Yang Jie (jie2.yang@samsung.com)
 * @author  Koeun Choi(koeun.choi@samsung.com)
 * @author  Pawel Sikorski(p.sikorski@samsung.com)
 * @version 1.0
 * @brief   This file contains the definition of Configuration.
 */

#include <dpl/wrt-dao-rw/widget_dao.h>

#include <sstream>
#include <dpl/log/log.h>
#include <dpl/foreach.h>
#include <dpl/wrt-dao-ro/webruntime_database.h>
#include <dpl/wrt-dao-rw/property_dao.h>
#include <orm_generator_wrt.h>
#include <dpl/wrt-dao-ro/WrtDatabase.h>

namespace WrtDB {

//TODO in current solution in each getter there exists a check
//"IsWidgetInstalled". Maybe it should be verified, if it could be done
//differently  (check in WidgetDAO constructor)

#define SQL_CONNECTION_EXCEPTION_HANDLER_BEGIN          Try

#define SQL_CONNECTION_EXCEPTION_HANDLER_END(message)   \
    Catch(DPL::DB::SqlConnection::Exception::Base) {       \
        LogError(message);                              \
        ReThrowMsg(WidgetDAO::Exception::DatabaseError, \
                   message);                            \
    }

#define CHECK_WIDGET_EXISTENCE(macro_transaction, macro_handle)          \
    if (!WidgetDAO::isWidgetInstalled(macro_handle))                      \
    {                                                                    \
        macro_transaction.Commit();                                      \
        LogWarning("Cannot find widget. Handle: " << macro_handle);      \
        ThrowMsg(WidgetDAO::Exception::WidgetNotExist,                   \
                 "Cannot find widget. Handle: " << macro_handle);        \
    }


WidgetDAO::WidgetDAO(DbWidgetHandle widgetHandle) :
    WidgetDAOReadOnly(widgetHandle)
{
}

WidgetDAO::WidgetDAO(DPL::OptionalString widgetGUID) :
    WidgetDAOReadOnly(WidgetDAOReadOnly::getHandle(widgetGUID))
{
}

WidgetDAO::~WidgetDAO()
{
}

void WidgetDAO::removeProperty(
        const PropertyDAOReadOnly::WidgetPropertyKey &key)
{
    Try {
        PropertyDAO::RemoveProperty(m_widgetHandle, key);
    }
    Catch(PropertyDAOReadOnly::Exception::ReadOnlyProperty){
        ReThrowMsg(WidgetDAO::Exception::DatabaseError,
                   "Failure during removing property");
    }
}

void WidgetDAO::setProperty(
        const PropertyDAOReadOnly::WidgetPropertyKey &key,
        const PropertyDAOReadOnly::WidgetPropertyValue &value,
        bool readOnly)
{
    Try {
        PropertyDAO::SetProperty(m_widgetHandle, key, value, readOnly);
    }
    Catch(PropertyDAOReadOnly::Exception::ReadOnlyProperty){
        ReThrowMsg(WidgetDAO::Exception::DatabaseError,
                   "Failure during setting/updating property");
    }
}

void WidgetDAO::setPkgName(const DPL::OptionalString& pkgName)
{
    SQL_CONNECTION_EXCEPTION_HANDLER_BEGIN
    {
        using namespace DPL::DB::ORM;
        wrt::ScopedTransaction transaction(&WrtDatabase::interface());

        isWidgetInstalled(getHandle());

        wrt::WidgetInfo::Row row;
        row.Set_pkgname(pkgName);

        WRT_DB_UPDATE(update, wrt::WidgetInfo, &WrtDatabase::interface())
        update->Where(
            Equals<wrt::WidgetInfo::app_id>(getHandle()));

        update->Values(row);
        update->Execute();
        transaction.Commit();
    }
    SQL_CONNECTION_EXCEPTION_HANDLER_END("Failed to register widget")
}

void WidgetDAO::registerWidget(
        const DbWidgetHandle& widgetHandle,
        const WidgetRegisterInfo &widgetRegInfo,
        const IWacSecurity &wacSecurity)
{
    LogDebug("Registering widget");
    SQL_CONNECTION_EXCEPTION_HANDLER_BEGIN
    {
        DPL::DB::ORM::wrt::ScopedTransaction transaction(&WrtDatabase::interface());

        //Register into WidgetInfo has to be first
        //as all other tables depend upon that
        registerWidgetInfo(widgetHandle, widgetRegInfo, wacSecurity);

        registerWidgetExtendedInfo(widgetHandle, widgetRegInfo);

        registerWidgetLocalizedInfo(widgetHandle, widgetRegInfo);

        registerWidgetIcons(widgetHandle, widgetRegInfo);

        registerWidgetStartFile(widgetHandle, widgetRegInfo);

        PropertyDAO::RegisterProperties(widgetHandle, widgetRegInfo);

        registerWidgetFeatures(widgetHandle, widgetRegInfo);

        registerWidgetWindowModes(widgetHandle, widgetRegInfo);

        registerWidgetWarpInfo(widgetHandle, widgetRegInfo);

        registerWidgetCertificates(widgetHandle, wacSecurity);

        CertificateChainList list;
        wacSecurity.getCertificateChainList(list);
        registerLaunchCertificates(widgetHandle,list);

        registerWidgetSettings(widgetHandle, widgetRegInfo);

        registerAppService(widgetHandle, widgetRegInfo);

        transaction.Commit();
    }
    SQL_CONNECTION_EXCEPTION_HANDLER_END("Failed to register widget")
}

#define DO_INSERT(row, table)                              \
    {                                                      \
        WRT_DB_INSERT(insert, table, &WrtDatabase::interface()) \
        insert->Values(row);                               \
        insert->Execute();                                 \
    }

void WidgetDAO::registerWidgetExtendedInfo(DbWidgetHandle widgetHandle,
        const WidgetRegisterInfo &regInfo)
{
    //Try and transaction not needed
    using namespace DPL::DB::ORM;
    using namespace DPL::DB::ORM::wrt;


    WidgetExtendedInfo::Row row;
    row.Set_app_id(widgetHandle);
    //    row.Set_share_href    (DPL::FromUTF8String(regInfo.shareHref));
    row.Set_signature_type(regInfo.signatureType);
    row.Set_factory_widget(regInfo.isFactoryWidget);
    row.Set_test_widget(regInfo.isTestWidget);
    row.Set_install_time(regInfo.installedTime);


    DO_INSERT(row, WidgetExtendedInfo)
}

void WidgetDAO::registerWidgetInfo(
        const DbWidgetHandle& widgetHandle,
        const WidgetRegisterInfo &regInfo,
        const IWacSecurity &wacSecurity)
{
    using namespace DPL::DB::ORM;
    using namespace DPL::DB::ORM::wrt;
    const ConfigParserData& widgetConfigurationInfo = regInfo.configInfo;

    // TODO in wrt_db all Columns in WidgetInfo have DEFAULT VALUE set.
    // Because of that, "Optional" is not used there

    WidgetInfo::Row row;
    row.Set_app_id(widgetHandle);
    row.Set_widget_type(regInfo.type.appType);
    row.Set_widget_id(widgetConfigurationInfo.widget_id);
    row.Set_defaultlocale(widgetConfigurationInfo.defaultlocale);
    row.Set_widget_version(widgetConfigurationInfo.version);
    row.Set_widget_width(widgetConfigurationInfo.width);
    row.Set_widget_height(widgetConfigurationInfo.height);
    row.Set_author_name(widgetConfigurationInfo.authorName);
    row.Set_author_email(widgetConfigurationInfo.authorEmail);
    row.Set_author_href(widgetConfigurationInfo.authorHref);
    row.Set_base_folder(DPL::FromUTF8String(regInfo.baseFolder));
    row.Set_webkit_plugins_required(widgetConfigurationInfo.flashNeeded);
    row.Set_recognized(wacSecurity.isRecognized());
    row.Set_wac_signed(wacSecurity.isWacSigned());
    row.Set_distributor_signed(wacSecurity.isDistributorSigned());
    {
        std::stringstream tmp;
        tmp << widgetConfigurationInfo.minVersionRequired;
        row.Set_min_version(DPL::FromUTF8String(tmp.str()));
    }
    row.Set_back_supported(widgetConfigurationInfo.backSupported);
    row.Set_access_network(widgetConfigurationInfo.accessNetwork);
    row.Set_pkgname(regInfo.pkgname);

    Try
    {
        DO_INSERT(row, WidgetInfo);
    }
    Catch(DPL::DB::SqlConnection::Exception::Base)
    {
        ReThrowMsg(WidgetDAO::Exception::DatabaseError,
                   "Failed to register widget info.");
    }
}

void WidgetDAO::registerWidgetLocalizedInfo(DbWidgetHandle widgetHandle,
        const WidgetRegisterInfo &regInfo)
{
    using namespace DPL::DB::ORM;
    using namespace DPL::DB::ORM::wrt;

    const ConfigParserData& widgetConfigurationInfo = regInfo.configInfo;

    FOREACH(it, widgetConfigurationInfo.localizedDataSet)
    {
        const DPL::String& locale = it->first;
        const ConfigParserData::LocalizedData& data = it->second;

        LocalizedWidgetInfo::Row row;
        row.Set_app_id(widgetHandle);
        row.Set_widget_locale(locale);
        row.Set_widget_name(data.name);
        row.Set_widget_shortname(data.shortName);
        row.Set_widget_description(data.description);
        row.Set_widget_license(data.license);
        row.Set_widget_license_file(data.licenseFile);
        row.Set_widget_license_href(data.licenseHref);

        DO_INSERT(row, LocalizedWidgetInfo)
    }
}

void WidgetDAO::registerWidgetUserAgentLocales(
        DbWidgetHandle widgetHandle,
        const WidgetRegisterInfo &/*regInfo*/,
        const LanguageTagsList& languageTags)
{
    using namespace DPL::DB::ORM;
    using namespace DPL::DB::ORM::wrt;


    FOREACH(i, languageTags)
    {
        wrt::WidgetUserAgentLocales::Row row;
        row.Set_app_id(widgetHandle);
        row.Set_language_tag(*i);

        DO_INSERT(row, wrt::WidgetUserAgentLocales)
    }
}

void WidgetDAO::registerWidgetIcons(DbWidgetHandle widgetHandle,
        const WidgetRegisterInfo &regInfo)
{
    using namespace DPL::DB::ORM;
    using namespace DPL::DB::ORM::wrt;


    FOREACH(i, regInfo.localizationData.icons)
    {
        wrt::WidgetIcon::icon_id::ColumnType icon_id;
        {
            wrt::WidgetIcon::Row row;
            row.Set_app_id(widgetHandle);
            row.Set_icon_src(i->src);
            row.Set_icon_width(i->width);
            row.Set_icon_height(i->height);

            WRT_DB_INSERT(insert, wrt::WidgetIcon, &WrtDatabase::interface())
            insert->Values(row);
            icon_id = insert->Execute();
        }

        FOREACH(j, i->availableLocales)
        {
            WidgetLocalizedIcon::Row row;
            row.Set_app_id(widgetHandle);
            row.Set_icon_id(icon_id);
            row.Set_widget_locale(*j);
            WRT_DB_SELECT(select, WidgetLocalizedIcon, &WrtDatabase::interface())
            select->Where(And(Equals<WidgetLocalizedIcon::app_id>(widgetHandle),
                              Equals<WidgetLocalizedIcon::widget_locale>(*j)));
            WidgetLocalizedIcon::Select::RowList rows = select->GetRowList();

            bool flag = !rows.empty();

            if(flag == true)
            {
                // already default icon value of same locale exists
                WRT_DB_UPDATE(update, WidgetLocalizedIcon, &WrtDatabase::interface())
                update->Where(And(Equals<WidgetLocalizedIcon::app_id>(widgetHandle),
                                  Equals<WidgetLocalizedIcon::widget_locale>(*j)));
                update->Values(row);
                update->Execute();
            }else{
                // any icon value of same locale doesn't exist
                DO_INSERT(row, WidgetLocalizedIcon)
            }
        }
    }
}

void WidgetDAO::registerWidgetStartFile(DbWidgetHandle widgetHandle,
        const WidgetRegisterInfo &regInfo)
{
    using namespace DPL::DB::ORM;
    using namespace DPL::DB::ORM::wrt;


    FOREACH(i, regInfo.localizationData.startFiles)
    {
        WidgetStartFile::start_file_id::ColumnType startFileID;
        {
            WidgetStartFile::Row row;
            row.Set_app_id(widgetHandle);
            row.Set_src(i->path);

            WRT_DB_INSERT(insert, WidgetStartFile, &WrtDatabase::interface())
            insert->Values(row);
            startFileID = insert->Execute();
        }

        FOREACH(j, i->propertiesForLocales)
        {
            WidgetLocalizedStartFile::Row row;
            row.Set_app_id(widgetHandle);
            row.Set_start_file_id(startFileID);
            row.Set_widget_locale(j->first);
            row.Set_type(j->second.type);
            row.Set_encoding(j->second.encoding);

            DO_INSERT(row, WidgetLocalizedStartFile)
        }
    }
}

void WidgetDAO::registerWidgetFeatures(DbWidgetHandle widgetHandle,
        const WidgetRegisterInfo &regInfo)
{
    using namespace DPL::DB::ORM;
    const ConfigParserData& widgetConfigurationInfo = regInfo.configInfo;


    FOREACH(pWidgetFeature, widgetConfigurationInfo.featuresList)
    {
        wrt::WidgetFeature::Row widgetFeature;
        widgetFeature.Set_app_id(widgetHandle);
        widgetFeature.Set_name(pWidgetFeature->name);
        widgetFeature.Set_required(pWidgetFeature->required);
        widgetFeature.Set_rejected(false);

        wrt::WidgetFeature::widget_feature_id::ColumnType widgetFeatureID;
        {
            WRT_DB_INSERT(insert, wrt::WidgetFeature, &WrtDatabase::interface())
            insert->Values(widgetFeature);
            widgetFeatureID = insert->Execute();
        }

        // Insert into table FeatureParam
        wrt::FeatureParam::Row featureParam;
        featureParam.Set_widget_feature_id(widgetFeatureID);

        ConfigParserData::ParamsList::const_iterator iter;

        FOREACH(iter, pWidgetFeature->paramsList)
        {
            featureParam.Set_name(iter->name);
            featureParam.Set_value(iter->value);

            DO_INSERT(featureParam, wrt::FeatureParam)
        }
    }
}

void WidgetDAO::updateFeatureRejectStatus(const DbWidgetFeature &widgetFeature){
    // This function could be merged with registerWidgetFeature but it requires desing change:
    // 1. Check "ace step" in installer must be done before "update database step"
    // And:
    // ConfigurationParserData shouldn't be called "ParserData" any more.
    using namespace DPL::DB::ORM;

    wrt::ScopedTransaction transaction(&WrtDatabase::interface());
    WRT_DB_SELECT(select, wrt::WidgetFeature, &WrtDatabase::interface())
    select->Where(And(Equals<wrt::WidgetFeature::app_id>(m_widgetHandle),
                      Equals<wrt::WidgetFeature::name>(widgetFeature.name)));

    auto row = select->GetSingleRow();
    row.Set_rejected(widgetFeature.rejected);

    WRT_DB_UPDATE(update, wrt::WidgetFeature, &WrtDatabase::interface())
    update->Where(And(Equals<wrt::WidgetFeature::app_id>(m_widgetHandle),
                      Equals<wrt::WidgetFeature::name>(widgetFeature.name)));
    update->Values(row);
    update->Execute();
    transaction.Commit();
}

void WidgetDAO::registerWidgetWindowModes(DbWidgetHandle widgetHandle,
        const WidgetRegisterInfo &regInfo)
{
    using namespace DPL::DB::ORM;
    using namespace DPL::DB::ORM::wrt;
    const ConfigParserData& widgetConfigurationInfo = regInfo.configInfo;


    FOREACH(i, widgetConfigurationInfo.windowModes)
    {
        wrt::WidgetWindowModes::Row windowMode;
        windowMode.Set_app_id(widgetHandle);
        windowMode.Set_window_mode(*i);

        DO_INSERT(windowMode, wrt::WidgetWindowModes)
    }
}

void WidgetDAO::registerWidgetWarpInfo(DbWidgetHandle widgetHandle,
        const WidgetRegisterInfo &regInfo)
{
    using namespace DPL::DB::ORM;
    using namespace DPL::DB::ORM::wrt;
    const ConfigParserData& widgetConfigurationInfo = regInfo.configInfo;


    FOREACH(AccIt, widgetConfigurationInfo.accessInfoSet)
    {
        WidgetWARPInfo::Row row;
        row.Set_app_id(widgetHandle);
        row.Set_iri(AccIt->m_strIRI);
        row.Set_subdomain_access(static_cast <int>(
                                     AccIt->m_bSubDomainAccess));

        DO_INSERT(row, WidgetWARPInfo)
    }
}

void WidgetDAO::registerWidgetCertificates(DbWidgetHandle widgetHandle,
        const IWacSecurity &wacSecurity)
{
    using namespace DPL::DB::ORM;
    using namespace DPL::DB::ORM::wrt;


    FOREACH(it, wacSecurity.getCertificateList())
    {
        WidgetCertificateFingerprint::Row row;
        row.Set_app_id(widgetHandle);
        row.Set_owner(it->owner);
        row.Set_chainid(it->chainId);
        row.Set_type(it->type);
        row.Set_md5_fingerprint(DPL::FromUTF8String(it->strMD5Fingerprint));
        row.Set_sha1_fingerprint(DPL::FromUTF8String(it->strSHA1Fingerprint));
        row.Set_common_name(it->strCommonName);

        DO_INSERT(row, WidgetCertificateFingerprint)
    }
}

void WidgetDAO::registerLaunchCertificates(DbWidgetHandle widgetHandle,
        const CertificateChainList &certificateChainList)
{
    using namespace DPL::DB::ORM;
    using namespace DPL::DB::ORM::wrt;
    FOREACH(certChain, certificateChainList)
    {
        WidgetCertificate::Row row;
        row.Set_app_id(widgetHandle);
        row.Set_encoded_chain(DPL::FromASCIIString(*certChain));

        DO_INSERT(row, WidgetCertificate);
    }
}

void WidgetDAO::registerWidgetSettings(DbWidgetHandle widgetHandle,
        const WidgetRegisterInfo &regInfo)
{
    using namespace DPL::DB::ORM;
    using namespace DPL::DB::ORM::wrt;

    const ConfigParserData& widgetConfigurationInfo = regInfo.configInfo;

    FOREACH(pWidgetSetting, widgetConfigurationInfo.settingsList)
    {
        SettginsList::Row row;
        row.Set_appId(widgetHandle);
        row.Set_settingName(pWidgetSetting->m_name);
        row.Set_settingValue(pWidgetSetting->m_value);

        DO_INSERT(row, SettginsList)
    }
}

void WidgetDAO::registerAppService(DbWidgetHandle widgetHandle,
                                   const WidgetRegisterInfo &regInfo)
{
    using namespace DPL::DB::ORM;
    using namespace DPL::DB::ORM::wrt;
    const ConfigParserData& widgetConfigurationInfo = regInfo.configInfo;

    FOREACH(ASIt, widgetConfigurationInfo.appServiceList)
    {
        ApplicationServiceInfo::Row row;
        row.Set_app_id(widgetHandle);
        row.Set_src(ASIt->m_src);
        row.Set_operation(ASIt->m_operation);
        row.Set_scheme(ASIt->m_scheme);
        row.Set_mime(ASIt->m_mime);

        DO_INSERT(row, ApplicationServiceInfo)
    }
}

#undef DO_INSERT

void WidgetDAO::unregisterWidget(DbWidgetHandle widgetHandle)
{
    LogDebug("Unregistering widget from DB. Handle: " << widgetHandle);
    SQL_CONNECTION_EXCEPTION_HANDLER_BEGIN
    {
        DPL::DB::ORM::wrt::ScopedTransaction transaction(&WrtDatabase::interface());
        using namespace DPL::DB::ORM;
        using namespace DPL::DB::ORM::wrt;

        CHECK_WIDGET_EXISTENCE(transaction, widgetHandle)

        // Delete from table Widget Info
        {
            WRT_DB_DELETE(del, WidgetInfo, &WrtDatabase::interface())
            del->Where(Equals<WidgetInfo::app_id>(widgetHandle));
            del->Execute();
        }

        // Deleting in other tables is done via "delete cascade" in SQL

        transaction.Commit();
    }
    SQL_CONNECTION_EXCEPTION_HANDLER_END("Failed to unregister widget")
}

#undef SQL_CONNECTION_EXCEPTION_HANDLER_BEGIN
#undef SQL_CONNECTION_EXCEPTION_HANDLER_END
#undef CHECK_WIDGET_EXISTENCE

} // namespace WrtDB
