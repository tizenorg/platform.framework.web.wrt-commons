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

    /**
     * This is a constructor.
     *
     * @param[in] widgetHandle application id of widget.
     * @param[in] widgetGUID application guid of widget.
     */
    WidgetDAO(DbWidgetHandle widgetHandle);
    WidgetDAO(DPL::OptionalString widgetGUID);

    /**
     * Destructor
     */
    virtual ~WidgetDAO();

    /**
     * This method registers the widget information to the DB when it is installed.
     *
     * @see WidgetRegisterInfo
     * @see UnRegisterWidget()
     * @param[in] widgetHandle  Widget ID that will be registered.
     * @param[in] pWidgetRegisterInfo    Specified the widget's information needed to be registered.
     * @param[in] wacSecurity   Widget's security certificates.
     */
    static void registerWidget(
            const DbWidgetHandle& widgetHandle,
            const WidgetRegisterInfo &pWidgetRegisterInfo,
            const IWacSecurity &wacSecurity);

    static DbWidgetHandle registerWidget(
            const WidgetRegisterInfo &pWidgetRegisterInfo,
            const IWacSecurity &wacSecurity) __attribute__((deprecated))
    {
        //make it more precise due to very fast tests
        struct timeval tv;
        gettimeofday(&tv, NULL);
        srand(time(NULL) + tv.tv_usec);
        DbWidgetHandle widgetHandle;
        do {
            widgetHandle = rand();
        } while (isWidgetInstalled(widgetHandle));

        registerWidget(widgetHandle, pWidgetRegisterInfo, wacSecurity);
        return widgetHandle;
    }

    /**
     * This method removes a widget's information from EmDB.
     *
     * @see RegisterWidget()
     * @param[in] widgetHandle    widget's app id
     * @return true if succeed, false if fail.
     */
    static void unregisterWidget(DbWidgetHandle widgetHandle);

    /* This method removes widget property
     */
    void removeProperty(const PropertyDAOReadOnly::WidgetPropertyKey &key);

    /* This method sets widget property
     */
    void setProperty(const PropertyDAOReadOnly::WidgetPropertyKey &key,
            const PropertyDAOReadOnly::WidgetPropertyValue &value,
            bool readOnly = false);

    /* set PkgName
     */
    void setPkgName(const DPL::OptionalString& pkgName);

    /* This function will update of api-feature status.
     * If status is true (feature rejected) plugin connected with this
     * api feature mustn't be loaded durign widget launch.
     */
    void updateFeatureRejectStatus(const DbWidgetFeature &widgetFeature);

  private:
    //Methods used during widget registering
    static void registerWidgetInfo(
            const DbWidgetHandle& widgetHandle,
            const WidgetRegisterInfo &regInfo,
            const IWacSecurity &wacSecurity);
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
    static void registerWidgetWindowModes(
            DbWidgetHandle widgetHandle,
            const WidgetRegisterInfo &regInfo);
    static void registerWidgetWarpInfo(
            DbWidgetHandle widgetHandle,
            const WidgetRegisterInfo &regInfo);
    static void registerWidgetCertificates(
            DbWidgetHandle widgetHandle,
            const IWacSecurity &wacSecurity);
    static void registerLaunchCertificates(
            DbWidgetHandle widgetHandle,
            const CertificateChainList &list);
    static void registerWidgetSettings(
            DbWidgetHandle widgetHandle,
            const WidgetRegisterInfo &regInfo);
    static void registerAppService(
            DbWidgetHandle widgetHandle,
            const WidgetRegisterInfo &regInfo);
};

} // namespace WrtDB

#endif // WIDGET_DAO_H
