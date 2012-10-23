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
 * This file contains the declaration of plugin dao class.
 *
 * @file    plugin_dao.h
 * @author  Pawel Sikorski (p.sikorski@samsung.com)
 * @version 1.0
 * @brief   This file contains the declaration of plugin dao
 */

#ifndef WRT_SRC_CONFIGURATION_PLUGIN_DAO_H_
#define WRT_SRC_CONFIGURATION_PLUGIN_DAO_H_

#include <dpl/wrt-dao-ro/plugin_dao_read_only.h>

namespace WrtDB {

class PluginDAO : public PluginDAOReadOnly
{
  public:
    PluginDAO(DbPluginHandle pluginHandle);
    PluginDAO(const std::string &libraryName);

    static DbPluginHandle registerPlugin(
            const PluginMetafileData& metafile,
            const std::string& pluginPath);

    static void registerPluginImplementedObject(
            const std::string& objectName,
            DbPluginHandle pluginHandle);

    static void registerPluginRequiredObject(
            const std::string& objectName,
            DbPluginHandle pluginHandle);

    static void registerPluginLibrariesDependencies(
            DbPluginHandle plugin,
            const PluginHandleSetPtr& dependencies);

    static void setPluginInstallationStatus(
            DbPluginHandle,
            PluginInstallationState);

    static void unregisterPlugin(DbPluginHandle pluginHandle);
};

} // namespace WrtDB

#endif /* WRT_SRC_CONFIGURATION_PLUGIN_DAO_H_ */
