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
 * @file    plugin_dao_read_only.h
 * @author  Krzysztof Jackiewicz (k.jackiewicz@samsung.com)
 * @version 1.0
 * @brief   This file contains the declaration of plugin dao read only
 */


#ifndef WRT_SRC_CONFIGURATION_PLUGIN_DAO_READ_ONLY_H_
#define WRT_SRC_CONFIGURATION_PLUGIN_DAO_READ_ONLY_H_

#include <string>
#include <list>
#include <dpl/exception.h>
#include <dpl/shared_ptr.h>
#include <dpl/wrt-dao-ro/common_dao_types.h>

namespace WrtDB {

typedef std::list<DbPluginHandle> PluginHandleList;
typedef std::set<DbPluginHandle> PluginHandleSet;
typedef std::list<std::string> ImplementedObjectsList;
typedef DPL::SharedPtr<PluginHandleSet> PluginHandleSetPtr;

//TODO make it friend to FeatureDAO or inherit
class PluginDAOReadOnly
{
  public:
    enum PluginInstallationState
    {
        INSTALLATION_DEFAULT,
        //when plugin data are up to date and plugin model may be created
        INSTALLATION_COMPLETED,
        //installation is in progress, some data may not be valid
        INSTALLATION_IN_PROGRESS,
        //installation not completed due to missing dependency
        INSTALLATION_WAITING,

        UNKNOWN_ERROR
    };

    static int ToInt(PluginInstallationState state)
    {
        return static_cast<int>(state);
    }

    static PluginInstallationState ToState(int state)
    {
        return static_cast<PluginDAOReadOnly::PluginInstallationState>(state);
    }

    /**
     * PluginDAO Exception classes
     */
    class Exception
    {
      public:
        DECLARE_EXCEPTION_TYPE(DPL::Exception, Base)
        DECLARE_EXCEPTION_TYPE(Base, DatabaseError)
        DECLARE_EXCEPTION_TYPE(Base, PluginNotExist)
        DECLARE_EXCEPTION_TYPE(Base, PluginInstallationNotCompleted)
    };

  public:
    PluginDAOReadOnly(DbPluginHandle pluginHandle);
    PluginDAOReadOnly(const std::string &libraryName);

    static PluginHandleList getPluginHandleList();

    static bool isPluginInstalled(const std::string &libraryName);
    static bool isPluginInstalled(DbPluginHandle pluginHandle);

    static PluginHandleSetPtr getPluginHandleByStatus(
            PluginInstallationState state);

    static DbPluginHandle getPluginHandleForImplementedObject(
            const std::string& objectName);

    static ImplementedObjectsList getImplementedObjectsForPluginHandle(
            DbPluginHandle handle);

    static PluginObjectsDAO::ObjectsPtr getRequiredObjectsForPluginHandle(
            DbPluginHandle handle);

    static PluginInstallationState getInstallationStateForHandle(
            DbPluginHandle handle);

    DbPluginHandle getPluginHandle() const;
    PluginInstallationState getInstallationStatus() const;
    std::string  getLibraryPath() const;
    std::string  getLibraryName() const;
    PluginHandleSetPtr getLibraryDependencies() const;

  private:
    DbPluginHandle m_pluginHandle;

    void checkInstallationCompleted();
};

} // namespace WrtDB

#endif /* WRT_SRC_CONFIGURATION_PLUGIN_DAO_READ_ONLY_H_ */
