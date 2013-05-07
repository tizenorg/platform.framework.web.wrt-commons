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
#include <stddef.h>
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

WidgetDAO::WidgetDAO(DPL::OptionalString widgetGUID) :
    WidgetDAOReadOnly(WidgetDAOReadOnly::getHandle(widgetGUID))
{}

WidgetDAO::WidgetDAO(DPL::String tzAppId) :
    WidgetDAOReadOnly(WidgetDAOReadOnly::getHandle(tzAppId))
{}

WidgetDAO::WidgetDAO(DbWidgetHandle handle) :
    WidgetDAOReadOnly(handle)
{}

WidgetDAO::~WidgetDAO()
{}

void WidgetDAO::setTizenAppId(const DPL::OptionalString& tzAppId)
{
    SQL_CONNECTION_EXCEPTION_HANDLER_BEGIN
    {
        using namespace DPL::DB::ORM;
        wrt::ScopedTransaction transaction(&WrtDatabase::interface());

        if (!isWidgetInstalled(getHandle())) {
            ThrowMsg(WidgetDAOReadOnly::Exception::WidgetNotExist,
                     "Cannot find widget. Handle: " << getHandle());
        }

        wrt::WidgetInfo::Row row;
        row.Set_tizen_appid(*tzAppId);

        WRT_DB_UPDATE(update, wrt::WidgetInfo, &WrtDatabase::interface())
        update->Where(
            Equals<wrt::WidgetInfo::app_id>(getHandle()));

        update->Values(row);
        update->Execute();
        transaction.Commit();
    }
    SQL_CONNECTION_EXCEPTION_HANDLER_END("Failed to register widget")
}

void WidgetDAO::setSecurityPopupUsage(const SettingsType value)
{
    SQL_CONNECTION_EXCEPTION_HANDLER_BEGIN
    {
        using namespace DPL::DB::ORM;
        using namespace DPL::DB::ORM::wrt;

        ScopedTransaction transaction(&WrtDatabase::interface());
        if (!isWidgetInstalled(getHandle())) {
            ThrowMsg(WidgetDAOReadOnly::Exception::WidgetNotExist,
                     "Cannot find widget. Handle: " << getHandle());
        }

        WidgetSecuritySettings::Row row;
        row.Set_security_popup_usage(value);

        WRT_DB_UPDATE(update, WidgetSecuritySettings, &WrtDatabase::interface())
        update->Where(Equals<WidgetSecuritySettings::app_id>(getHandle()));
        update->Values(row);
        update->Execute();

        transaction.Commit();
    }
    SQL_CONNECTION_EXCEPTION_HANDLER_END("Failed to set security popup usage")
}

void WidgetDAO::setGeolocationUsage(const SettingsType value)
{
    SQL_CONNECTION_EXCEPTION_HANDLER_BEGIN
    {
        using namespace DPL::DB::ORM;
        using namespace DPL::DB::ORM::wrt;

        ScopedTransaction transaction(&WrtDatabase::interface());
        if (!isWidgetInstalled(getHandle())) {
            ThrowMsg(WidgetDAOReadOnly::Exception::WidgetNotExist,
                     "Cannot find widget. Handle: " << getHandle());
        }

        WidgetSecuritySettings::Row row;
        row.Set_geolocation_usage(value);

        WRT_DB_UPDATE(update, WidgetSecuritySettings, &WrtDatabase::interface())
        update->Where(Equals<WidgetSecuritySettings::app_id>(getHandle()));
        update->Values(row);
        update->Execute();

        transaction.Commit();
    }
    SQL_CONNECTION_EXCEPTION_HANDLER_END("Failed to set geolocation usage")
}

void WidgetDAO::setWebNotificationUsage(const SettingsType value)
{
    SQL_CONNECTION_EXCEPTION_HANDLER_BEGIN
    {
        using namespace DPL::DB::ORM;
        using namespace DPL::DB::ORM::wrt;

        ScopedTransaction transaction(&WrtDatabase::interface());
        if (!isWidgetInstalled(getHandle())) {
            ThrowMsg(WidgetDAOReadOnly::Exception::WidgetNotExist,
                     "Cannot find widget. Handle: " << getHandle());
        }

        WidgetSecuritySettings::Row row;
        row.Set_web_notification_usage(value);

        WRT_DB_UPDATE(update, WidgetSecuritySettings, &WrtDatabase::interface())
        update->Where(Equals<WidgetSecuritySettings::app_id>(getHandle()));
        update->Values(row);
        update->Execute();

        transaction.Commit();
    }
    SQL_CONNECTION_EXCEPTION_HANDLER_END("Failed to set web notification usage")
}

void WidgetDAO::setWebDatabaseUsage(const SettingsType value)
{
    SQL_CONNECTION_EXCEPTION_HANDLER_BEGIN
    {
        using namespace DPL::DB::ORM;
        using namespace DPL::DB::ORM::wrt;

        ScopedTransaction transaction(&WrtDatabase::interface());
        if (!isWidgetInstalled(getHandle())) {
            ThrowMsg(WidgetDAOReadOnly::Exception::WidgetNotExist,
                     "Cannot find widget. Handle: " << getHandle());
        }

        WidgetSecuritySettings::Row row;
        row.Set_web_database_usage(value);

        WRT_DB_UPDATE(update, WidgetSecuritySettings, &WrtDatabase::interface())
        update->Where(Equals<WidgetSecuritySettings::app_id>(getHandle()));
        update->Values(row);
        update->Execute();

        transaction.Commit();
    }
    SQL_CONNECTION_EXCEPTION_HANDLER_END("Failed to set web database usage")
}

void WidgetDAO::setFileSystemUsage(const SettingsType value)
{
    SQL_CONNECTION_EXCEPTION_HANDLER_BEGIN
    {
        using namespace DPL::DB::ORM;
        using namespace DPL::DB::ORM::wrt;

        ScopedTransaction transaction(&WrtDatabase::interface());
        if (!isWidgetInstalled(getHandle())) {
            ThrowMsg(WidgetDAOReadOnly::Exception::WidgetNotExist,
                     "Cannot find widget. Handle: " << getHandle());
        }

        WidgetSecuritySettings::Row row;
        row.Set_file_system_usage(value);

        WRT_DB_UPDATE(update, WidgetSecuritySettings, &WrtDatabase::interface())
        update->Where(Equals<WidgetSecuritySettings::app_id>(getHandle()));
        update->Values(row);
        update->Execute();

        transaction.Commit();
    }
    SQL_CONNECTION_EXCEPTION_HANDLER_END("Failed to set filesystem usage")
}

void WidgetDAO::registerWidget(
    const TizenAppId & tzAppId,
    const WidgetRegisterInfo &widgetRegInfo,
    const IWacSecurity &wacSecurity)
{
    LogDebug("Registering widget");
    SQL_CONNECTION_EXCEPTION_HANDLER_BEGIN
    {
        DPL::DB::ORM::wrt::ScopedTransaction transaction(
            &WrtDatabase::interface());
        registerWidgetInternal(tzAppId, widgetRegInfo, wacSecurity);
        transaction.Commit();
    }
    SQL_CONNECTION_EXCEPTION_HANDLER_END("Failed to register widget")
}

DbWidgetHandle WidgetDAO::registerWidget(
    const WidgetRegisterInfo &pWidgetRegisterInfo,
    const IWacSecurity &wacSecurity)
{
    //make it more precise due to very fast tests
    struct timeval tv;
    gettimeofday(&tv, NULL);
    DbWidgetHandle widgetHandle;
    unsigned int seed = time(NULL);
    do {
        widgetHandle = rand_r(&seed);
    } while (isWidgetInstalled(widgetHandle));

    registerWidget(*pWidgetRegisterInfo.configInfo.tizenAppId,
                   pWidgetRegisterInfo,
                   wacSecurity);
    return widgetHandle;
}

TizenAppId WidgetDAO::registerWidgetGeneratePkgId(
    const WidgetRegisterInfo &pWidgetRegisterInfo,
    const IWacSecurity &wacSecurity)
{
    TizenAppId tzAppId = generatePkgId();
    registerWidget(tzAppId, pWidgetRegisterInfo, wacSecurity);
    return tzAppId;
}

void WidgetDAO::registerWidgetInternal(
    const TizenAppId & tzAppId,
    const WidgetRegisterInfo &widgetRegInfo,
    const IWacSecurity &wacSecurity,
    const DPL::Optional<DbWidgetHandle> handle)
{
    //Register into WidgetInfo has to be first
    //as all other tables depend upon that
    DbWidgetHandle widgetHandle = registerWidgetInfo(tzAppId,
                                                     widgetRegInfo,
                                                     wacSecurity,
                                                     handle);

    registerWidgetExtendedInfo(widgetHandle, widgetRegInfo);

    registerWidgetLocalizedInfo(widgetHandle, widgetRegInfo);

    registerWidgetIcons(widgetHandle, widgetRegInfo);

    registerWidgetStartFile(widgetHandle, widgetRegInfo);

    PropertyDAO::RegisterProperties(tzAppId, widgetRegInfo);

    registerWidgetFeatures(widgetHandle, widgetRegInfo);

    registerWidgetPrivilege(widgetHandle, widgetRegInfo);

    registerWidgetWindowModes(widgetHandle, widgetRegInfo);

    registerWidgetWarpInfo(widgetHandle, widgetRegInfo);

    registerWidgetAllowNavigationInfo(widgetHandle, widgetRegInfo);

    registerWidgetCertificates(widgetHandle, wacSecurity);

    CertificateChainList list;
    wacSecurity.getCertificateChainList(list, SIGNATURE_DISTRIBUTOR);
    registerCertificatesChains(widgetHandle, SIGNATURE_DISTRIBUTOR, list);

    list.clear();
    wacSecurity.getCertificateChainList(list, SIGNATURE_AUTHOR);
    registerCertificatesChains(widgetHandle, SIGNATURE_AUTHOR, list);

    registerWidgetSettings(widgetHandle, widgetRegInfo);

    registerAppControl(widgetHandle, widgetRegInfo);

    registerEncryptedResouceInfo(widgetHandle, widgetRegInfo);

    registerExternalLocations(widgetHandle, widgetRegInfo.externalLocations);

    registerWidgetSecuritySettings(widgetHandle);
}

void WidgetDAO::registerOrUpdateWidget(
    const TizenAppId & widgetName,
    const WidgetRegisterInfo &widgetRegInfo,
    const IWacSecurity &wacSecurity)
{
    LogDebug("Reregistering widget");
    SQL_CONNECTION_EXCEPTION_HANDLER_BEGIN
    {
        DPL::DB::ORM::wrt::ScopedTransaction transaction(
            &WrtDatabase::interface());

        unregisterWidgetInternal(widgetName);
        registerWidgetInternal(widgetName, widgetRegInfo, wacSecurity);
        transaction.Commit();
    }
    SQL_CONNECTION_EXCEPTION_HANDLER_END("Failed to reregister widget")
}

void WidgetDAO::backupAndUpdateWidget(
    const TizenAppId & oldAppId,
    const TizenAppId & newAppId,
    const WidgetRegisterInfo &widgetRegInfo,
    const IWacSecurity &wacSecurity)
{
    LogDebug("Backup and Register widget");
    SQL_CONNECTION_EXCEPTION_HANDLER_BEGIN
    {
        DPL::DB::ORM::wrt::ScopedTransaction transaction(
            &WrtDatabase::interface());

        updateWidgetAppIdInternal(newAppId, oldAppId);
        registerWidgetInternal(newAppId, widgetRegInfo, wacSecurity);
        transaction.Commit();
    }
    SQL_CONNECTION_EXCEPTION_HANDLER_END("Failed to reregister widget")
}

void WidgetDAO::restoreUpdateWidget(
    const TizenAppId & oldAppId,
    const TizenAppId & newAppId)
{
    LogDebug("restore widget");
    SQL_CONNECTION_EXCEPTION_HANDLER_BEGIN
    {
        DPL::DB::ORM::wrt::ScopedTransaction transaction(
            &WrtDatabase::interface());

        unregisterWidgetInternal(newAppId);
        updateWidgetAppIdInternal(oldAppId, newAppId);
        transaction.Commit();
    }
    SQL_CONNECTION_EXCEPTION_HANDLER_END("Failed to reregister widget")
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
    row.Set_test_widget(regInfo.isTestWidget);
    row.Set_install_time(regInfo.installedTime);
    row.Set_splash_img_src(regInfo.configInfo.splashImgSrc);
    row.Set_background_page(regInfo.configInfo.backgroundPage);
    row.Set_installed_path(regInfo.widgetInstalledPath);

    DO_INSERT(row, WidgetExtendedInfo)
}

DbWidgetHandle WidgetDAO::registerWidgetInfo(
    const TizenAppId & tzAppId,
    const WidgetRegisterInfo &regInfo,
    const IWacSecurity &wacSecurity,
    const DPL::Optional<DbWidgetHandle> handle)
{
    using namespace DPL::DB::ORM;
    using namespace DPL::DB::ORM::wrt;
    const ConfigParserData& widgetConfigurationInfo = regInfo.configInfo;

    // TODO in wrt_db all Columns in WidgetInfo have DEFAULT VALUE set.
    // Because of that, "Optional" is not used there

    WidgetInfo::Row row;
    if (!!handle) {
        row.Set_app_id(*handle);
    }

    row.Set_widget_type(regInfo.webAppType.appType);

    row.Set_widget_id(widgetConfigurationInfo.widget_id);
    row.Set_defaultlocale(widgetConfigurationInfo.defaultlocale);
    row.Set_widget_version(widgetConfigurationInfo.version);
    row.Set_widget_width(widgetConfigurationInfo.width);
    row.Set_widget_height(widgetConfigurationInfo.height);
    row.Set_author_name(widgetConfigurationInfo.authorName);
    row.Set_author_email(widgetConfigurationInfo.authorEmail);
    row.Set_author_href(widgetConfigurationInfo.authorHref);
    row.Set_csp_policy(widgetConfigurationInfo.cspPolicy);
    row.Set_csp_policy_report_only(widgetConfigurationInfo.cspPolicyReportOnly);
    row.Set_base_folder(DPL::FromUTF8String(regInfo.baseFolder));
    row.Set_webkit_plugins_required(widgetConfigurationInfo.flashNeeded);
    row.Set_recognized(wacSecurity.isRecognized());
    row.Set_wac_signed(wacSecurity.isWacSigned());
    row.Set_distributor_signed(wacSecurity.isDistributorSigned());
    row.Set_tizen_appid(tzAppId);
    row.Set_tizen_pkgid(regInfo.tzPkgid);
    {
        std::stringstream tmp;
        tmp << regInfo.minVersion;
        row.Set_min_version(DPL::FromUTF8String(tmp.str()));
    }
    row.Set_back_supported(widgetConfigurationInfo.backSupported);
    row.Set_access_network(widgetConfigurationInfo.accessNetwork);
    row.Set_pkg_type(regInfo.packagingType.pkgType);
    row.Set_security_model_version(
        static_cast<int>(widgetConfigurationInfo.securityModelVersion));

    Try
    {
        DO_INSERT(row, WidgetInfo);
    }
    Catch(DPL::DB::SqlConnection::Exception::Base)
    {
        ReThrowMsg(WidgetDAO::Exception::DatabaseError,
                   "Failed to register widget info.");
    }

    if (!handle) {
        //get autoincremented value of widgetHandle
        WRT_DB_SELECT(select, WidgetInfo, &WrtDatabase::interface())
        select->Where(Equals<WidgetInfo::tizen_appid>(tzAppId));
        return select->GetSingleValue<WidgetInfo::app_id>();
    } else {
        return *handle;
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
            icon_id = static_cast<int>(insert->Execute());
        }

        FOREACH(j, i->availableLocales)
        {
            WidgetLocalizedIcon::Row row;
            row.Set_app_id(widgetHandle);
            row.Set_icon_id(icon_id);
            row.Set_widget_locale(*j);
            DO_INSERT(row, WidgetLocalizedIcon)
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
            startFileID = static_cast<int>(insert->Execute());
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
        widgetFeature.Set_rejected(false);

        DO_INSERT(widgetFeature, wrt::WidgetFeature)
    }
}

void WidgetDAO::registerWidgetPrivilege(DbWidgetHandle widgetHandle,
                                        const WidgetRegisterInfo &regInfo)
{
    using namespace DPL::DB::ORM;
    const ConfigParserData& widgetConfigurationInfo = regInfo.configInfo;

    FOREACH(it, widgetConfigurationInfo.privilegeList)
    {
        wrt::WidgetPrivilege::Row widgetPrivilege;
        widgetPrivilege.Set_app_id(widgetHandle);
        widgetPrivilege.Set_name(it->name);

        DO_INSERT(widgetPrivilege, wrt::WidgetPrivilege)
    }
}

void WidgetDAO::updateFeatureRejectStatus(const DbWidgetFeature &widgetFeature)
{
    // This function could be merged with registerWidgetFeature but it requires
    // desing change:
    // 1. Check "ace step" in installer must be done before "update database
    // step"
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

void WidgetDAO::registerWidgetAllowNavigationInfo(DbWidgetHandle widgetHandle,
                                                  const WidgetRegisterInfo &regInfo)
{
    using namespace DPL::DB::ORM;
    using namespace DPL::DB::ORM::wrt;
    const ConfigParserData& widgetConfigurationInfo = regInfo.configInfo;

    FOREACH(allowNaviIt, widgetConfigurationInfo.allowNavigationInfoList)
    {
        WidgetAllowNavigation::Row row;
        row.Set_app_id(widgetHandle);
        row.Set_scheme(allowNaviIt->m_scheme);
        row.Set_host(allowNaviIt->m_host);
        DO_INSERT(row, WidgetAllowNavigation)
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

void WidgetDAO::registerCertificatesChains(
    DbWidgetHandle widgetHandle,
    CertificateSource certificateSource,
    const CertificateChainList &
    certificateChainList)
{
    using namespace DPL::DB::ORM;
    using namespace DPL::DB::ORM::wrt;
    FOREACH(certChain, certificateChainList)
    {
        WidgetCertificate::Row row;
        row.Set_app_id(widgetHandle);
        row.Set_cert_source(certificateSource);
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
        SettingsList::Row row;
        row.Set_appId(widgetHandle);
        row.Set_settingName(pWidgetSetting->m_name);
        row.Set_settingValue(pWidgetSetting->m_value);

        DO_INSERT(row, SettingsList)
    }
}

void WidgetDAO::insertAppControlInfo(DbWidgetHandle handle,
                                             DPL::String src,
                                             DPL::String operation,
                                             DPL::String uri,
                                             DPL::String mime,
                                             unsigned index,
                                             unsigned disposition)
{
    using namespace DPL::DB::ORM;
    using namespace DPL::DB::ORM::wrt;

    AppControlInfo::Row row;

    row.Set_app_id(handle);
    row.Set_execute_index(index);
    row.Set_src(src);
    row.Set_operation(operation);
    row.Set_uri(uri);
    row.Set_mime(mime);
    row.Set_disposition(disposition);

    DO_INSERT(row, AppControlInfo);
}

void WidgetDAO::registerAppControl(DbWidgetHandle widgetHandle,
                                   const WidgetRegisterInfo &regInfo)
{
    using namespace DPL::DB::ORM;
    using namespace DPL::DB::ORM::wrt;
    const ConfigParserData& widgetConfigurationInfo = regInfo.configInfo;

    // appControlList
    FOREACH(appControl_it, widgetConfigurationInfo.appControlList)
    {
        DPL::String src       = appControl_it->m_src;
        DPL::String operation = appControl_it->m_operation;
        unsigned index        = appControl_it->m_index;
        unsigned disposition  = appControl_it->m_disposition ==
            ConfigParserData::AppControlInfo::Disposition::INLINE ? 1 : 0;

        if (!appControl_it->m_uriList.empty())
        {
            FOREACH(uri_it, appControl_it->m_uriList)
            {
                DPL::String uri = *uri_it;

                if (!appControl_it->m_mimeList.empty())
                {
                    FOREACH(mime_it, appControl_it->m_mimeList)
                    {
                        DPL::String mime = *mime_it;

                        insertAppControlInfo(widgetHandle, src, operation, uri, mime, index, disposition);
                    }
                }
                else
                {
                    DPL::String mime = L"";

                    insertAppControlInfo(widgetHandle, src, operation, uri, mime, index, disposition);
                }
            }
        }
        else
        {
            DPL::String uri = L"";

            if (!appControl_it->m_mimeList.empty())
            {
                FOREACH(mime_it, appControl_it->m_mimeList)
                {
                    DPL::String mime = *mime_it;

                    insertAppControlInfo(widgetHandle, src, operation, uri, mime, index, disposition);
                }
            }
            else
            {
                DPL::String mime = L"";

                insertAppControlInfo(widgetHandle, src, operation, uri, mime, index, disposition);
            }
        }
    }
}

void WidgetDAO::registerEncryptedResouceInfo(DbWidgetHandle widgetHandle,
                                             const WidgetRegisterInfo &regInfo)
{
    using namespace DPL::DB::ORM;
    using namespace DPL::DB::ORM::wrt;

    FOREACH(it, regInfo.encryptedFiles)
    {
        EncryptedResourceList::Row row;
        row.Set_app_id(widgetHandle);
        row.Set_resource(it->fileName);
        row.Set_size(it->fileSize);

        DO_INSERT(row, EncryptedResourceList)
    }
}

void WidgetDAO::registerExternalLocations(
    DbWidgetHandle widgetHandle,
    const ExternalLocationList &
    externals)
{
    SQL_CONNECTION_EXCEPTION_HANDLER_BEGIN
    {
        using namespace DPL::DB::ORM;
        using namespace DPL::DB::ORM::wrt;
        DPL::DB::ORM::wrt::ScopedTransaction transaction(
            &WrtDatabase::interface());
        LogDebug("Inserting external files for widgetHandle: " << widgetHandle);
        FOREACH(it, externals)
        {
            WidgetExternalLocations::Row row;
            row.Set_app_id(widgetHandle);
            row.Set_path(DPL::FromUTF8String(*it));

            DO_INSERT(row, WidgetExternalLocations)
        }
        transaction.Commit();
    }
    SQL_CONNECTION_EXCEPTION_HANDLER_END("Failed to register external files");
}

void WidgetDAO::registerWidgetSecuritySettings(DbWidgetHandle widgetHandle)
{
    using namespace DPL::DB::ORM;
    using namespace DPL::DB::ORM::wrt;
    WidgetSecuritySettings::Row row;
    row.Set_app_id(widgetHandle);
    row.Set_security_popup_usage(SETTINGS_TYPE_ON);
    row.Set_geolocation_usage(SETTINGS_TYPE_ON);
    row.Set_web_notification_usage(SETTINGS_TYPE_ON);
    row.Set_web_database_usage(SETTINGS_TYPE_ON);
    row.Set_file_system_usage(SETTINGS_TYPE_ON);

    DO_INSERT(row, WidgetSecuritySettings)
}

void WidgetDAO::unregisterAllExternalLocations()
{
    using namespace DPL::DB::ORM;
    using namespace DPL::DB::ORM::wrt;
    LogDebug("Deleting external files for widgetHandle: " << m_widgetHandle);
    WRT_DB_DELETE(del, WidgetExternalLocations, &WrtDatabase::interface());
    del->Where(Equals<WidgetExternalLocations::app_id>(m_widgetHandle));
    del->Execute();
}

void WidgetDAO::unregisterWidget(const TizenAppId & tzAppId)
{
    LogDebug("Unregistering widget from DB. tzAppId: " << tzAppId);
    SQL_CONNECTION_EXCEPTION_HANDLER_BEGIN
    {
        DPL::DB::ORM::wrt::ScopedTransaction transaction(
            &WrtDatabase::interface());
        unregisterWidgetInternal(tzAppId);
        transaction.Commit();
    }
    SQL_CONNECTION_EXCEPTION_HANDLER_END("Failed to unregister widget")
}

void WidgetDAO::unregisterWidget(WrtDB::DbWidgetHandle handle)
{
    LogDebug("Unregistering widget from DB. Handle: " << handle);
    SQL_CONNECTION_EXCEPTION_HANDLER_BEGIN
    {
        using namespace DPL::DB::ORM;
        using namespace DPL::DB::ORM::wrt;
        ScopedTransaction transaction(&WrtDatabase::interface());

        // Delete from table Widget Info
        WRT_DB_DELETE(del, WidgetInfo, &WrtDatabase::interface())
        del->Where(Equals<WidgetInfo::app_id>(handle));
        del->Execute();

        transaction.Commit();
    }
    SQL_CONNECTION_EXCEPTION_HANDLER_END("Failed to unregister widget")
}

void WidgetDAO::unregisterWidgetInternal(
    const TizenAppId & tzAppId)
{
    using namespace DPL::DB::ORM;
    using namespace DPL::DB::ORM::wrt;

    DbWidgetHandle handle = getHandle(tzAppId);

    // Delete from table Widget Info
    WRT_DB_DELETE(del, WidgetInfo, &WrtDatabase::interface())
    del->Where(Equals<WidgetInfo::app_id>(handle));
    del->Execute();

    // Deleting in other tables is done via "delete cascade" in SQL
}

void WidgetDAO::updateWidgetAppIdInternal(
    const TizenAppId & fromAppId,
    const TizenAppId & toAppId)
{
    SQL_CONNECTION_EXCEPTION_HANDLER_BEGIN
    {
        using namespace DPL::DB::ORM;
        using namespace DPL::DB::ORM::wrt;

        ScopedTransaction transaction(&WrtDatabase::interface());
        if (!isWidgetInstalled(fromAppId)) {
            ThrowMsg(WidgetDAOReadOnly::Exception::WidgetNotExist,
                     "Cannot find widget. tzAppId: " << fromAppId);
        }

        WidgetInfo::Row row;
        row.Set_tizen_appid(toAppId);

        WRT_DB_UPDATE(update, WidgetInfo, &WrtDatabase::interface())
        update->Where(Equals<WidgetInfo::tizen_appid>(fromAppId));
        update->Values(row);
        update->Execute();

        transaction.Commit();
    }
    SQL_CONNECTION_EXCEPTION_HANDLER_END("Failed to update appid")
}

#undef DO_INSERT

#undef SQL_CONNECTION_EXCEPTION_HANDLER_BEGIN
#undef SQL_CONNECTION_EXCEPTION_HANDLER_END
} // namespace WrtDB
