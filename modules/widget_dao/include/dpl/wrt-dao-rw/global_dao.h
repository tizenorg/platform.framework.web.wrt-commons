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
 * @file    global_dao.h
 * @author  Pawel Sikorski (p.sikorski@samsung.com)
 * @version 1.0
 * @brief   This file contains the declaration of global dao
 */

#ifndef WRT_SRC_CONFIGURATION_GLOBAL_DAO_H_
#define WRT_SRC_CONFIGURATION_GLOBAL_DAO_H_

#include <dpl/wrt-dao-ro/global_dao_read_only.h>

namespace WrtDB {

class GlobalDAO : public GlobalDAOReadOnly
{
  public:
    /**
     * Set new Parental mode status
     */
    static void SetParentalMode(bool parental_status);

    /**
     * Retrieve Parental mode maximal allowed age
     *
     * @return NULL if allowed age not set, else pointer value is allowed age
     */
    static void SetParentalAllowedAge(const DPL::OptionalInt& age);

    static void AddCategoryRule(
            const ChildProtection::PowderRules::CategoryRule& powder);

    static void RemoveCategoryRule(
            const ChildProtection::PowderRules::CategoryRule& powder);

    static void UpdateCategoryRule(
            const ChildProtection::PowderRules::CategoryRule& oldRule,
            const ChildProtection::PowderRules::CategoryRule& newRule);

    static void AddAdultBlackListElement(const DPL::String &url);

    static void RemoveAdultBlackListElement(const DPL::String &url);

    static void UpdateAdultBlackList(const DPL::String &oldUrl,
                                     const DPL::String &newUrl);

    /**
     * Add deffered widget packages to be installed
     */
    static void AddDefferedWidgetPackageInstallation(const DPL::String &path);

    /**
     * Remove deffered widget packages to be installed
     */
    static void RemoveDefferedWidgetPackageInstallation(const DPL::String &arg);

    static void SetDeveloperMode(bool mode);

    static void SetSecureByDefault(bool secureByDefault);

    static void setComplianceMode(bool mode);

    static void setComplianceFakeImei(const std::string &imei);

    static void setComplianceFakeMeid(const std::string &meid);

    static void AddWhiteURI(const std::string &value, bool subDomain);

    static void RemoveWhiteURI(const std::string &uri);

    /**
     * This method changes network access mode while roaming is enabled.
     *
     */
    static void SetHomeNetworkDataUsage(NetworkAccessMode newMode);

    /**
     * This method changes network access mode while roaming is enabled.
     *
     */
    static void SetRoamingDataUsage(NetworkAccessMode newMode);

    /**
     * This method sets Autofill for Webkit
     */
    static void SetAutoSaveIdPasswd(
            const DPL::String &url, const AutoSaveData &saveData);


  private:
    GlobalDAO()
    {
    }
};

} // namespace WrtDB

#endif /* WRT_SRC_CONFIGURATION_GLOBAL_DAO_H_ */
