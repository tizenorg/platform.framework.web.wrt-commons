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
  * @file   TestCases_FeatureDAO.cpp
 * @author  Pawel Sikorski (p.sikorski@samsung.com)
 * @version 1.0
 * @brief   This file contains tests for feature dao class.
 */

#include <dpl/test/test_runner.h>
#include <dpl/foreach.h>
#include <dpl/wrt-dao-rw/feature_dao.h>
#include <dpl/wrt-dao-rw/plugin_dao.h>
#include <dpl/wrt-dao-ro/wrt_db_types.h>

using namespace WrtDB;

#define RUNNER_ASSERT_WHAT_EQUALS(in, test)                   \
    {std::string tmp(in);                                     \
    RUNNER_ASSERT_MSG(tmp == test, "Equals: [" + tmp + "]");}

RUNNER_TEST_GROUP_INIT(DAO)

/*
Name: feature_dao_test_register_features
Description: Checks if plugin registeration performs features registration
Expected: registrartion should succeed
*/
RUNNER_TEST(feature_dao_test_register_features)
{
    PluginHandle plHandle;
    {
        std::string libraryPath("nfp1 lib_path");
        std::string libraryName("nfp1");

        PluginMetafileData pluginData;
        pluginData.m_libraryName          = libraryName;

        plHandle = PluginDAO::registerPlugin(pluginData, libraryPath);
        RUNNER_ASSERT(PluginDAO::isPluginInstalled(libraryName) == true);

        FeatureHandleList old = FeatureDAOReadOnly::GetHandleList();
        PluginMetafileData::Feature f;
        f.m_name = std::string("new_f1");

        FeatureHandle handle = FeatureDAO::RegisterFeature(f,plHandle);
        RUNNER_ASSERT_MSG(handle != -1, "Already registered");
        RUNNER_ASSERT_MSG(old.size() < FeatureDAOReadOnly::GetHandleList().size(),
                          "New feature should be saved");

        FeatureDAOReadOnly dao(handle);

        RUNNER_ASSERT_WHAT_EQUALS(dao.GetName(),"new_f1");
        plHandle = dao.GetPluginHandle();
    }

    {
        FeatureHandleList old = FeatureDAOReadOnly::GetHandleList();

        PluginMetafileData::Feature f;
        f.m_name = std::string("new_f2");

        FeatureHandle handle = FeatureDAO::RegisterFeature(f,plHandle);
        RUNNER_ASSERT_MSG(handle != -1, "Already registered");
        RUNNER_ASSERT_MSG(old.size() < FeatureDAOReadOnly::GetHandleList().size(),
                          "New feature should be saved");

        FeatureDAOReadOnly dao(handle);

        RUNNER_ASSERT_MSG(plHandle == dao.GetPluginHandle(),
                          "New plugin registered (should be old used)");
    }
}

/*
Name: feature_dao_test_get_feature_properties
Description: Checks properties of inserted features
Expected: properties of features should match values inserted to database directly
*/
RUNNER_TEST(feature_dao_test_get_feature_properties)
{
    {
        FeatureDAOReadOnly dao("feature1");
        RUNNER_ASSERT_WHAT_EQUALS(dao.GetName(), "feature1");
        RUNNER_ASSERT_WHAT_EQUALS(dao.GetLibraryName(), "plugin1");
        RUNNER_ASSERT_WHAT_EQUALS(dao.GetLibraryPath(), "");
    }

    {
        FeatureDAOReadOnly dao("feature2");
        RUNNER_ASSERT_WHAT_EQUALS(dao.GetName(), "feature2");
        RUNNER_ASSERT_WHAT_EQUALS(dao.GetLibraryName(), "p4");
        RUNNER_ASSERT_WHAT_EQUALS(dao.GetLibraryPath(), "path_to_p4");
    }

    {
        FeatureDAOReadOnly dao("feature3");
        RUNNER_ASSERT_WHAT_EQUALS(dao.GetName(), "feature3");
        RUNNER_ASSERT_WHAT_EQUALS(dao.GetLibraryName(), "p4");
        RUNNER_ASSERT_WHAT_EQUALS(dao.GetLibraryPath(), "path_to_p4");
    }
}

/*
Name: feature_dao_test_feature_constructor_name
Description: -
Expected: -

TODO: test
*/
RUNNER_TEST(feature_dao_test_feature_constructor_name)
{
    std::list<const char *> preinstalled;
    preinstalled.push_back("feature1");
    preinstalled.push_back("feature2");
    preinstalled.push_back("feature3");
    preinstalled.push_back("feature4");

    FOREACH(it, preinstalled)
    {
        FeatureDAOReadOnly dao(*it);
        RUNNER_ASSERT_WHAT_EQUALS(dao.GetName(), *it);
    }

    //TODO check exception that may occur (feature does not exist)
}

/*
Name: feature_dao_test_feature_handle_list
Description: Checks if list of installed features is returend correctly
Expected: list size should be at last equal number of preinserted features
*/
RUNNER_TEST(feature_dao_test_feature_handle_list)
{
    FeatureHandleList handles = FeatureDAOReadOnly::GetHandleList();
    RUNNER_ASSERT(handles.size() >= 4);
}

/*
Name: feature_dao_test_is_feature_installed
Description: Checks if installed features are showed correctly.
Expected: correct installed features should be present
*/
RUNNER_TEST(feature_dao_test_is_feature_installed)
{
    //installed
    {
        std::list<const char *> preinstalled;
        preinstalled.push_back("feature1");
        preinstalled.push_back("feature2");
        preinstalled.push_back("feature3");
        preinstalled.push_back("feature4");

        FOREACH(it, preinstalled)
            RUNNER_ASSERT(FeatureDAOReadOnly::isFeatureInstalled(*it));
    }

    //not installed
    {
        RUNNER_ASSERT(FeatureDAOReadOnly::isFeatureInstalled("not_installed1")==false);
        RUNNER_ASSERT(FeatureDAOReadOnly::isFeatureInstalled("plugin1") == false);
        RUNNER_ASSERT(FeatureDAOReadOnly::isFeatureInstalled("") == false);
        RUNNER_ASSERT(FeatureDAOReadOnly::isFeatureInstalled("ff") == false);
    }
}

/*
Name: feature_dao_test_get_device_capab
Description: -
Expected: -

TODO: fix test
*/
RUNNER_TEST(feature_dao_test_get_device_capab)
{
#if 0
    //TODO do more tests

    {//check deviceCaps
        Feature f;
        f.setName("new_dev_f4");
        f.setLibraryName("new_dev_f4 lib_name");

        FeatureDAOReadOnly::DeviceCapabilitiesList devList;
        devList.insert("new_dev_f4 devcap1");
        devList.insert("new_dev_f4 devcap2");

        FOREACH(it, devList)
            f.addDeviceCapability(*it);

        FeatureHandle handle = FeatureDAO::RegisterFeature(f,"new_dev_f4 path");

        FeatureDAOReadOnly dao(handle);

        FeatureDAOReadOnly::DeviceCapabilitiesList gotList =
                dao.GetDeviceCapabilities();
        RUNNER_ASSERT_MSG(gotList.size() == devList.size(),
                          "deviceCaps wrong");
    }
#endif
}

/*
Name: feature_dao_test_is_device_capab_installed
Description: Checks if FeatureDAOReadOnly::isDeviceCapabilityInstalled works correctly.
Expected: correct capabilities should be present
*/
RUNNER_TEST(feature_dao_test_is_device_capab_installed)
{
    //installed
    std::list<const char *> preinstalled;
    preinstalled.push_back("devicecap1");
    preinstalled.push_back("devicecap2");
    preinstalled.push_back("devicecap3");
    preinstalled.push_back("devicecap4");

    FOREACH(it, preinstalled)
        RUNNER_ASSERT(FeatureDAOReadOnly::isDeviceCapabilityInstalled(*it));

    //not installed
    std::list<const char *> notinstalled;
    notinstalled.push_back("notinstalled1");
    notinstalled.push_back("plugin1");
    notinstalled.push_back("");
    notinstalled.push_back("ff");

    FOREACH(it, notinstalled)
        RUNNER_ASSERT(!FeatureDAOReadOnly::isDeviceCapabilityInstalled(*it));
}

#undef RUNNER_ASSERT_WHAT_EQUALS
