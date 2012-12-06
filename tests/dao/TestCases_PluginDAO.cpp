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
  * @file   TestCases_PluginDAO.cpp
 * @author  Pawel Sikorski (p.sikorski@samsung.com)
 * @version 1.0
 * @brief   This file contains tests for feature dao class.
 */

#include <list>
#include <dpl/test/test_runner.h>
#include <dpl/foreach.h>
#include <dpl/exception.h>
#include <dpl/wrt-dao-rw/plugin_dao.h>
//#include <plugin_install/plugin_objects.h>
#include <dpl/wrt-dao-ro/wrt_db_types.h>
#include <dpl/log/log.h>

using namespace WrtDB;

#define RUNNER_ASSERT_WHAT_EQUALS(in, test)                   \
    {std::string tmp(in);                                     \
    RUNNER_ASSERT_MSG(tmp == test, "Equals: [" + tmp + "]");}

RUNNER_TEST_GROUP_INIT(DAO)

/*
Name: plugin_dao_test_register_plugins
Description: registers new plugin and check if it was correctly registered
Expected: plugin should be correctly registered
*/
RUNNER_TEST(plugin_dao_test_register_plugins)
{
    {
        std::string libraryPath("np1 lib_path");
        std::string libraryName("np1");

        PluginMetafileData pluginData;
        pluginData.m_libraryName          = libraryName;

        PluginHandle handle = PluginDAO::registerPlugin(pluginData, libraryPath);
        PluginDAO::setPluginInstallationStatus(handle,PluginDAO::INSTALLATION_COMPLETED);
        RUNNER_ASSERT(PluginDAO::isPluginInstalled(libraryName) == true);

        PluginDAO dao(handle);
        std::string tmp;
        tmp = dao.getLibraryPath(); //do for each
        RUNNER_ASSERT_MSG(tmp == libraryPath, "Equals: " + tmp);
    }

    {
        std::string libraryName("np2");

        PluginMetafileData pluginData;
        pluginData.m_libraryName          = libraryName;

        PluginHandle handle = PluginDAO::registerPlugin(pluginData, "");
        PluginDAO::setPluginInstallationStatus(handle,PluginDAO::INSTALLATION_COMPLETED);
        RUNNER_ASSERT(PluginDAO::isPluginInstalled(libraryName) == true);

        PluginDAO dao(handle);
        RUNNER_ASSERT(dao.getLibraryPath() == "");
    }
}

/*
Name: plugin_dao_test_register_plugin_implemented_object
Description: registers new PluginImplementedObject
 and check if it was correctly registered
Expected: plugin dao shoudld be upodated with PluginImplementedObject
*/
RUNNER_TEST(plugin_dao_test_register_plugin_implemented_object)
{
    {
        std::string libraryPath("np3 lib_path");
        std::string libraryName("np3");

        PluginMetafileData pluginData;
        pluginData.m_libraryName          = libraryName;

        PluginHandle handle =
            PluginDAO::registerPlugin(pluginData, libraryPath);
        RUNNER_ASSERT(PluginDAO::isPluginInstalled(libraryName) == true);

        std::string object1("object1");
        std::string object2("object2");
        PluginDAO::registerPluginImplementedObject(object1, handle);
        PluginDAO::registerPluginImplementedObject(object2, handle);

        PluginHandle retHandle1 =
            PluginDAO::getPluginHandleForImplementedObject(object1);
        PluginHandle retHandle2 =
            PluginDAO::getPluginHandleForImplementedObject(object1);
        RUNNER_ASSERT(retHandle1 == handle);
        RUNNER_ASSERT(retHandle2 == handle);
    }
}

/*
Name: plugin_dao_test_register_plugin_implemented_object
Description: registers dependecies for plugins and checks if they were saved
Expected: registered dependecies should be returned from database
*/
RUNNER_TEST(plugin_dao_test_register_library_dependencies)
{
    {
        std::string libraryPath("np4 lib_path");
        std::string libraryName("np4");

        PluginMetafileData pluginData;
        pluginData.m_libraryName          = libraryName;

        PluginHandle handle =
            PluginDAO::registerPlugin(pluginData, libraryPath);
        PluginDAO::setPluginInstallationStatus(handle,PluginDAO::INSTALLATION_COMPLETED);
        RUNNER_ASSERT(PluginDAO::isPluginInstalled(libraryName) == true);

        PluginHandle depHandles[] = {117, 119};

        PluginHandleSetPtr dependencies(new PluginHandleSet);
        dependencies->insert(depHandles[1]);
        dependencies->insert(depHandles[2]);

        PluginDAO::registerPluginLibrariesDependencies(handle, dependencies);

        PluginDAO dao(handle);
        PluginHandleSetPtr retDependencies;
        retDependencies = dao.getLibraryDependencies();

        RUNNER_ASSERT(
            retDependencies->size()==sizeof(depHandles)/sizeof(depHandles[0]));
        RUNNER_ASSERT(
            retDependencies->find(depHandles[1]) != retDependencies->end());
        RUNNER_ASSERT(
            retDependencies->find(depHandles[2]) != retDependencies->end());
    }
}

/*
Name: plugin_dao_test_register_required_object
Description: registers required plugin objects for plugins and checks if they were saved
Expected: registered required plugin objects should be returned from database
*/
RUNNER_TEST(plugin_dao_test_register_required_object)
{
    {
        std::string libraryPath("np5 lib_path");
        std::string libraryName("np5");

        PluginMetafileData pluginData;
        pluginData.m_libraryName          = libraryName;

        PluginHandle handle =
            PluginDAO::registerPlugin(pluginData, libraryPath);
        PluginDAO::setPluginInstallationStatus(handle,PluginDAO::INSTALLATION_COMPLETED);
        RUNNER_ASSERT(PluginDAO::isPluginInstalled(libraryName) == true);

        const size_t numObjects =2;
        std::string objectReq [numObjects];
        objectReq[0] = std::string("object1.req");
        objectReq[1] = std::string("object2.req");
        PluginDAO::registerPluginRequiredObject(objectReq[0], handle);
        PluginDAO::registerPluginRequiredObject(objectReq[1], handle);

        WrtDB::PluginObjectsDAO::ObjectsPtr objects =
            PluginDAO::getRequiredObjectsForPluginHandle(handle);

        RUNNER_ASSERT(objects->size() == numObjects
                    && objects->find(objectReq[0]) != objects->end()
                    && objects->find(objectReq[1]) != objects->end());
    }
}

/*
Name: plugin_dao_test_is_library_installed
Description: tests if plugin isntallation/registrartion works
Expected: only registered plugins should be reported as installed
*/
RUNNER_TEST(plugin_dao_test_is_library_installed)
{
    {
        //exist
        std::list<const char *> preinstalled;
        preinstalled.push_back("plugin1");
        preinstalled.push_back("plugin2");
        preinstalled.push_back("plugin3");
        preinstalled.push_back("p4");
        preinstalled.push_back("p5");

        FOREACH(it, preinstalled)
            RUNNER_ASSERT_MSG(PluginDAO::isPluginInstalled(*it),
                              std::string("Not found: ") + *it);
    }

    {
        //does not exist
        RUNNER_ASSERT_MSG(
                PluginDAO::isPluginInstalled("not_installed1") == false,
                "Found not_installed1");
        RUNNER_ASSERT_MSG(PluginDAO::isPluginInstalled("p 4") == false,
                          "Found p 4");
        RUNNER_ASSERT_MSG(PluginDAO::isPluginInstalled("") == false,
                          "Found <empty>");
        RUNNER_ASSERT_MSG(PluginDAO::isPluginInstalled("p33") == false,
                          "Found p33");
        RUNNER_ASSERT_MSG(PluginDAO::isPluginInstalled("feature1") == false,
                          "Found feature1");
    }
}

/*
Name: plugin_dao_test_get_plugin_handle_list
Description: test of returning plugin handle list
Expected: returned list should be no less than number of registered plugins
*/
RUNNER_TEST(plugin_dao_test_get_plugin_handle_list)
{
    PluginHandleList handles = PluginDAO::getPluginHandleList();
    RUNNER_ASSERT(handles.size() >= 5);
}

/*
Name: plugin_dao_test_constructor_name
Description: tests construction of plugin dao based on plugin name
Expected: Instance of dao should be constructed only
 if there is given plugin in database
*/
RUNNER_TEST(plugin_dao_test_constructor_name)
{
    {
        //exist
        std::list<const char *> preinstalled;
        preinstalled.push_back("plugin1");
        preinstalled.push_back("plugin2");
        preinstalled.push_back("plugin3");
        preinstalled.push_back("p4");
        preinstalled.push_back("p5");

        FOREACH(it, preinstalled)
        {
            PluginDAO dao(*it);
            RUNNER_ASSERT_WHAT_EQUALS(dao.getLibraryName(), *it);
        }
    }

    {
        //does not exist
        std::list<const char *> not_installed;
        not_installed.push_back("plugin 1");
        not_installed.push_back("");
        not_installed.push_back("p 3");

        FOREACH(it, not_installed)
        {
            Try {
                //Plugin not exist
                PluginDAO dao(*it);
                RUNNER_ASSERT_MSG(false, "should not be found");
            }
            Catch (PluginDAO::Exception::PluginNotExist) {
                continue;
            }
        }
    }

}

/*
Name: plugin_dao_test_get_plugin_properties
Description: tests reading plugin properties from database
Expected: Data, inserted into database, should be accessible via dao
*/
RUNNER_TEST(plugin_dao_test_get_plugin_properties)
{
    {
        PluginDAO dao("p4");
        RUNNER_ASSERT(dao.getPluginHandle() == 4);
        RUNNER_ASSERT_WHAT_EQUALS(dao.getLibraryName(), "p4");
        RUNNER_ASSERT_WHAT_EQUALS(dao.getLibraryPath(), "path_to_p4");
    }

    {
        PluginDAO dao(5);
        RUNNER_ASSERT(dao.getPluginHandle() == 5);
        RUNNER_ASSERT_WHAT_EQUALS(dao.getLibraryName(), "p5");
        RUNNER_ASSERT_WHAT_EQUALS(dao.getLibraryPath(), "path_to_p5");
    }

    {
        PluginDAO dao(2);
        RUNNER_ASSERT(dao.getPluginHandle() == 2);
        RUNNER_ASSERT_WHAT_EQUALS(dao.getLibraryName(), "plugin2");
        RUNNER_ASSERT_WHAT_EQUALS(dao.getLibraryPath(), "path_to_plugin2");
    }

    {
        PluginDAO dao(1);
        RUNNER_ASSERT(dao.getPluginHandle() == 1);
        RUNNER_ASSERT_WHAT_EQUALS(dao.getLibraryName(), "plugin1");
        RUNNER_ASSERT_WHAT_EQUALS(dao.getLibraryPath(), "");
    }
}

/*
Name: plugin_dao_test_get_implemented_objects_for_plugin_handle_1
Description: tests receiving from dao Implemented Objects
Expected: returned object is size 0
*/
RUNNER_TEST(plugin_dao_test_get_implemented_objects_for_plugin_handle_1)
{
    {
        const int handle = 1;
        PluginDAOReadOnly dao(handle);
        auto dbHandle = dao.getPluginHandle();
        RUNNER_ASSERT(dbHandle == handle);
        auto objects = dao.getImplementedObjectsForPluginHandle(dbHandle);

        RUNNER_ASSERT(objects.size() == 0);
    }
}

/*
Name: plugin_dao_test_get_implemented_objects_for_plugin_handle_2
Description: tests receiving from dao Implemented Objects
Expected: returned object is size as it was inserted
*/
RUNNER_TEST(plugin_dao_test_get_implemented_objects_for_plugin_handle_2)
{
    {
        std::set< std::string > preinstalled =
        {
            ""
        };

        PluginDAOReadOnly dao(2);
        auto dbHandle = dao.getPluginHandle();
        auto objects = dao.getImplementedObjectsForPluginHandle(dbHandle);

        //LogError("\n" << objects.size() << " " << preinstalled.size() << "\n");

        RUNNER_ASSERT(objects.size() == preinstalled.size());

        FOREACH(dbObject, objects)
        {
            RUNNER_ASSERT(preinstalled.find(*dbObject) != preinstalled.end());
        }
    }
}

/*
Name: plugin_dao_test_get_implemented_objects_for_plugin_handle_3
Description: tests receiving from dao Implemented Objects
Expected: returned objects list has preinserted object
*/
RUNNER_TEST(plugin_dao_test_get_implemented_objects_for_plugin_handle_3)
{
    {
        std::set< std::string > preinstalled =
        {
            "Plugin_3_Object_A"
        };

        PluginDAOReadOnly dao(3);
        auto dbHandle = dao.getPluginHandle();
        auto objects = dao.getImplementedObjectsForPluginHandle(dbHandle);
        RUNNER_ASSERT(objects.size() == preinstalled.size());

        FOREACH(dbObject, objects)
        {
            RUNNER_ASSERT(preinstalled.find(*dbObject) != preinstalled.end());
        }
    }
}

/*
Name: plugin_dao_test_get_implemented_objects_for_plugin_handle_4
Description: tests receiving from dao Implemented Objects
Expected: returned objects list has all preinserted objects
*/
RUNNER_TEST(plugin_dao_test_get_implemented_objects_for_plugin_handle_4)
{
    {
        std::set< std::string > preinstalled =
        {
            "Plugin_4_Object_A",
            "Plugin_4_Object_B",
            "Plugin_4_Object_C",
        };

        PluginDAOReadOnly dao(4);
        auto dbHandle = dao.getPluginHandle();
        auto objects = dao.getImplementedObjectsForPluginHandle(dbHandle);
        RUNNER_ASSERT(objects.size() == preinstalled.size());

        FOREACH(dbObject, objects)
        {
            RUNNER_ASSERT(preinstalled.find(*dbObject) != preinstalled.end());
        }
    }
}

/*
Name: plugin_dao_test_get_implemented_objects_for_plugin_handle_5
Description: tests receiving from dao Implemented Objects
Expected: returned objects list do not have object that was not inserted
*/
RUNNER_TEST(plugin_dao_test_get_implemented_objects_for_plugin_handle_5)
{
    {
        std::set< std::string > preinstalled =
        {
            "Plugin_5_Object_B",
        };

        PluginDAOReadOnly dao(5);
        auto dbHandle = dao.getPluginHandle();
        auto objects = dao.getImplementedObjectsForPluginHandle(dbHandle);
        RUNNER_ASSERT(objects.size() == preinstalled.size());

        FOREACH(dbObject, objects)
        {
            RUNNER_ASSERT(preinstalled.find(*dbObject) == preinstalled.end());
        }
    }
}


#undef RUNNER_ASSERT_WHAT_EQUALS
