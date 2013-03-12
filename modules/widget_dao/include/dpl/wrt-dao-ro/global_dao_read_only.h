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
 * @file    global_dao_read_only.h
 * @author  Pawel Sikorski (p.sikorski@samsung.com)
 * @version 1.0
 * @brief   This file contains the declaration of global dao
 */

#ifndef WRT_SRC_CONFIGURATION_GLOBAL_DAO_READ_ONLY_H_
#define WRT_SRC_CONFIGURATION_GLOBAL_DAO_READ_ONLY_H_

#include <list>
#include <set>
#include <string>

#include <dpl/string.h>
#include <dpl/exception.h>

#include <dpl/wrt-dao-ro/common_dao_types.h>

namespace WrtDB {

typedef std::list<DPL::String> WidgetPackageList;
typedef std::set<DPL::String> DeviceCapabilitySet;

//#ifdef USE_BROWSER_SETTING
//typedef DPL::DB::ORM::wrt::UserAgents::key_value::ColumnType UserAgent;
//#endif //USE_BROWSER_SETTING

class GlobalDAOReadOnly
{
  public:
    /**
     * GlobalDAOReadOnly Exception classes
     */
    class Exception
    {
      public:
        DECLARE_EXCEPTION_TYPE(DPL::Exception, Base)
        DECLARE_EXCEPTION_TYPE(Base, DatabaseError)
    };

  public:

    static bool IsValidSubTag(const DPL::String& tag, int type);

    static bool GetDeveloperMode();

    static bool GetSecureByDefault();

    static bool getComplianceMode();

    static std::string getComplianceFakeImei();

    static std::string getComplianceFakeMeid();

    static WidgetAccessInfoList GetWhiteURIList();

    static bool GetCookieSharingMode();

    enum NetworkAccessMode
    {
        NEVER_CONNECT,
        ALWAYS_ASK,
        CONNECT_AUTOMATICALLY
    };

    /**
     * This method returns network access mode for home network.
     *
     * @return Access mode for home network.
     */
    static NetworkAccessMode GetHomeNetworkDataUsage();

    /**
     * This method returns network access mode while roaming is enabled.
     *
     * @return Access mode for home network.
     */
    static NetworkAccessMode GetRoamingDataUsage();

    static DPL::String GetUserAgentValue(const DPL::String &key);

    /**
     * This method returns set of device capabilities used by apifeature.
     */
    static DeviceCapabilitySet GetDeviceCapability(
            const DPL::String &apifeature);

  protected:
    GlobalDAOReadOnly()
    {
    }
};

} // namespace WrtDB

#endif // WRT_SRC_CONFIGURATION_GLOBAL_DAO_READ_ONLY_H_
