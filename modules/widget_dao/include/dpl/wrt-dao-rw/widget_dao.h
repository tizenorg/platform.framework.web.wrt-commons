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
 * @file    widget_dao.h
 * @author  Yang Jie (jie2.yang@samsung.com)
 * @author  Przemyslaw Dobrowolski (p.dobrowolsk@samsung.com)
 * @author  Pawel Sikorski (p.sikorski@samsung.com)
 * @version 1.0
 * @brief   This file contains the declaration of widget dao
 */
#ifndef WIDGET_DAO_H
#define WIDGET_DAO_H

#include <dpl/wrt-dao-ro/widget_dao_read_only.h>
#include <list>
#include <string>
#include <sys/time.h>
#include <ctime>
#include <cstdlib>
#include <dpl/exception.h>
#include <dpl/db/orm.h>
#include <dpl/wrt-dao-ro/config_parser_data.h>
#include <dpl/wrt-dao-rw/property_dao.h>

namespace WrtDB {

class WidgetDAO : public WidgetDAOReadOnly
{
  public:
    typedef std::list<DPL::String> LanguageTagsList;

    WidgetDAO(DbWidgetHandle handle);
    WidgetDAO(DPL::OptionalString widgetGUID);
    WidgetDAO(DPL::String tzAppId);

    /**
     * Destructor
     */
    virtual ~WidgetDAO();

    /**
     * This method registers the widget information in the DB when it is installed.
     *
     * @see WidgetRegisterInfo
     * @see UnRegisterWidget()
     * @param[in] TizenAppId Widget app id that will be registered.
     * @param[in] pWidgetRegisterInfo    Specified the widget's information needed to be registered.
     * @param[in] wacSecurity   Widget's security certificates.
     */
    static void registerWidget(
            const TizenAppId& tzAppId,
            const WidgetRegisterInfo &widgetRegInfo,
            const IWacSecurity &wacSecurity);

    static DbWidgetHandle registerWidget(
                const WidgetRegisterInfo &pWidgetRegisterInfo,
                const IWacSecurity &wacSecurity) __attribute__((deprecated));

    /**
     * @brief registerWidgetGenerateTizenId Registers widget with auto-generated tizen id
     *
     * This function is disadviced and should be used only in tests.
     * Function is not thread-safe.
     *
     * @param pWidgetRegisterInfo registeration information
     * @param wacSecurity Widget's security certificates.
     * @return tzAppId generated
     */
    static TizenAppId registerWidgetGeneratePkgId(
                const WidgetRegisterInfo &pWidgetRegisterInfo,
                const IWacSecurity &wacSecurity);

    /**
     * This method re-registers the widget information to the DB when it is installed.
     *
     * It performs unregistration and new registration of widget in db in one transaction.
     *
     * @see WidgetRegisterInfo
     * @param[in] tzAppId  Widget tizen app id that will be registered.
     * @param[in] pWidgetRegisterInfo    Specified the widget's information needed to be registered.
     * @param[in] wacSecurity   Widget's security certificates.
     */
    static void registerOrUpdateWidget(
            const TizenAppId & tzAppId,
            const WidgetRegisterInfo &widgetRegInfo,
            const IWacSecurity &wacSecurity);

    /**
     * This method removes a widget's information from EmDB.
     *
     * @see RegisterWidget()
     * @param[in] tzAppId widgets name to be unregistered
     */
    static void unregisterWidget(const TizenAppId & tzAppId);

    static void unregisterWidget(WrtDB::DbWidgetHandle handle) __attribute__((deprecated));

    /* This method removes widget property
     */
    void removeProperty(const PropertyDAOReadOnly::WidgetPropertyKey &key);

    /**
     * @brief registerExternalLocations Removes rows from WidgetExternalLocations
     */
    void unregisterAllExternalLocations();

    /* This method sets widget property
     */
    void setProperty(const PropertyDAOReadOnly::WidgetPropertyKey &key,
            const PropertyDAOReadOnly::WidgetPropertyValue &value,
            bool readOnly = false);

    /* set tzAppId
     */
    void setTizenAppId(const DPL::OptionalString& tzAppId);

    /* This function will update of api-feature status.
     * If status is true (feature rejected) plugin connected with this
     * api feature mustn't be loaded durign widget launch.
     */
    void updateFeatureRejectStatus(const DbWidgetFeature &widgetFeature);

     /*
      * This method change security settings value
      */
    void setSecurityPopupUsage(const SettingsType value);
    void setGeolocationUsage(const SettingsType value);
    void setWebNotificationUsage(const SettingsType value);
    void setWebDatabaseUsage(const SettingsType value);
    void setFileSystemUsage(const SettingsType value);

  private:
    //Methods used during widget registering
    static DbWidgetHandle registerWidgetInfo(
            const TizenAppId & widgetName,
            const WidgetRegisterInfo &regInfo,
            const IWacSecurity &wacSecurity,
            const DPL::Optional<DbWidgetHandle> handle = DPL::Optional<DbWidgetHandle>());
    static void registerWidgetExtendedInfo(
            DbWidgetHandle widgetHandle,
            const WidgetRegisterInfo &regInfo);
    static void registerWidgetLocalizedInfo(
            DbWidgetHandle widgetHandle,
            const WidgetRegisterInfo &regInfo);
    static void registerWidgetIcons(
            DbWidgetHandle widgetHandle,
            const WidgetRegisterInfo &regInfo);
    static void registerWidgetStartFile(
            DbWidgetHandle widgetHandle,
            const WidgetRegisterInfo &regInfo);
    static void registerWidgetPreferences(
            DbWidgetHandle widgetHandle,
            const WidgetRegisterInfo &regInfo);
    static void registerWidgetFeatures(
            DbWidgetHandle widgetHandle,
            const WidgetRegisterInfo &regInfo);
    static void registerWidgetPrivilege(
            DbWidgetHandle widgetHandle,
            const WidgetRegisterInfo &regInfo);
    static void registerWidgetWindowModes(
            DbWidgetHandle widgetHandle,
            const WidgetRegisterInfo &regInfo);
    static void registerWidgetWarpInfo(
            DbWidgetHandle widgetHandle,
            const WidgetRegisterInfo &regInfo);
    static void registerWidgetCertificates(
            DbWidgetHandle widgetHandle,
            const IWacSecurity &wacSecurity);
    static void registerCertificatesChains(
            DbWidgetHandle widgetHandle,
            CertificateSource certificateSource,
            const CertificateChainList &list);
    static void registerWidgetSettings(
            DbWidgetHandle widgetHandle,
            const WidgetRegisterInfo &regInfo);
    static void registerAppService(
            DbWidgetHandle widgetHandle,
            const WidgetRegisterInfo &regInfo);
    static void registerEncryptedResouceInfo(
            DbWidgetHandle widgetHandle,
            const WidgetRegisterInfo &regInfo);
    /**
     * @brief registerExternalLocations Inserts new rows to WidgetExternalLocations
     * @param externals list of files
     */
    static void registerExternalLocations(DbWidgetHandle widgetHandle,
                                const ExternalLocationList & externals);
    static void registerWidgetSecuritySettings(DbWidgetHandle widgetHandle);


    static void registerWidgetInternal(
            const TizenAppId & tzAppId,
            const WidgetRegisterInfo &widgetRegInfo,
            const IWacSecurity &wacSecurity,
            const DPL::Optional<DbWidgetHandle> handle = DPL::Optional<DbWidgetHandle>());
    static void unregisterWidgetInternal(
            const TizenAppId & tzAppId);
};

} // namespace WrtDB

#endif // WIDGET_DAO_H
