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
 * @file    feature_dao_read_only.h
 * @author  Krzysztof Jackiewicz (k.jackiewicz@samsung.com)
 * @version 1.0
 * @brief   This file contains the declaration of feature dao read only
 */

#ifndef WRT_SRC_CONFIGURATION_FEATURE_DAO_READ_ONLY_H_
#define WRT_SRC_CONFIGURATION_FEATURE_DAO_READ_ONLY_H_

#include <set>
#include <string>
#include <dpl/exception.h>
#include <dpl/wrt-dao-ro/common_dao_types.h>
#include "feature_model.h"
#include <dpl/wrt-dao-ro/common_dao_types.h>
#include <dpl/wrt-dao-ro/WrtDatabase.h>

namespace WrtDB {

class FeatureDAOReadOnly
{
  public:
    /**
     * Exception classes
     */
    class Exception
    {
      public:
        DECLARE_EXCEPTION_TYPE(DPL::Exception, Base)
        DECLARE_EXCEPTION_TYPE(Base, DatabaseError)
        DECLARE_EXCEPTION_TYPE(Base, FeatureNotExist)
    };

    typedef std::set<std::string> DeviceCapabilitiesList;
    typedef std::multimap<FeatureHandle, std::string> DeviceCapabilitiesMap;
    typedef std::map<FeatureHandle, std::string> NameMap;

    static bool isDeviceCapabilityInstalled(const std::string &deviceCapName);

    FeatureDAOReadOnly(FeatureHandle);
    FeatureDAOReadOnly(const std::string &featureName);

    static FeatureHandleListPtr GetFeatureHandleListForPlugin(
            DbPluginHandle pluginHandle);

    static bool isFeatureInstalled(const std::string &featureName);
    static bool isFeatureInstalled(FeatureHandle handle);
    static FeatureHandleList GetHandleList();

    std::string             GetName() const;
    FeatureHandle           GetFeatureHandle() const;
    std::string             GetLibraryPath() const;
    std::string             GetLibraryName() const;
    DeviceCapabilitiesList  GetDeviceCapabilities() const;
    DbPluginHandle          GetPluginHandle() const;

    static NameMap                 GetNames();
    static DeviceCapabilitiesMap   GetDevCapWithFeatureHandle();

  protected:
    FeatureHandle m_featureHandle;
};

} // namespace WrtDB

#endif /* WRT_SRC_CONFIGURATION_FEATURE_DAO_READ_ONLY_H_ */
