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
 * @file   TestCases_PropertyDAO.cpp
 * @author  Pawel Sikorski (p.sikorski@samsung.com)
 * @version 1.0
 * @brief   This file contains tests for property dao class.
 */

#include <list>
#include <map>
#include <dpl/test/test_runner.h>
#include <dpl/foreach.h>
#include <dpl/exception.h>
#include <dpl/wrt-dao-rw/property_dao.h>
#include <dpl/wrt-dao-ro/wrt_db_types.h>

using namespace WrtDB;
using namespace WrtDB::PropertyDAOReadOnly;

// Widgets used 2000, 2001, 2002, 2003(saved by wrt_dao_tests_prepare_db.sh)

#define RUNNER_ASSERT_WHAT_EQUALS(in, test)                   \
    {std::string tmp(in);                                     \
    RUNNER_ASSERT_MSG(tmp == test, "Equals: [" + tmp + "]");}

#define RUNNER_ASSERT_WHAT_EQUALS_OPTIONAL(in, test)          \
        {                                                     \
            if(in.IsNull()) RUNNER_ASSERT_MSG(false, "NULL"); \
            else RUNNER_ASSERT_WHAT_EQUALS(DPL::ToUTF8String(*in),test);\
        }

RUNNER_TEST_GROUP_INIT(DAO)

/*
Name: property_dao_get_lists
Description: tests returning list of properties for given id
Expected: data received should match those, which were inserted in prepare script
*/
RUNNER_TEST(property_dao_get_lists)
{
    {//property list
        std::map<WidgetHandle, size_t> prefsMap;
        prefsMap.insert(std::pair<WidgetHandle, size_t>(2000, 2));
        prefsMap.insert(std::pair<WidgetHandle, size_t>(2001, 1));
        prefsMap.insert(std::pair<WidgetHandle, size_t>(2002, 2));
        prefsMap.insert(std::pair<WidgetHandle, size_t>(1, 0)); //no widget

        FOREACH(it, prefsMap) {
            PropertyDAOReadOnly::WidgetPreferenceList prefs =
                    PropertyDAOReadOnly::GetPropertyList(it->first);
            RUNNER_ASSERT(prefs.size() == it->second);
        }
    }

    {//property key list
        WidgetPropertyKeyList orig_2000;
        orig_2000.push_back(DPL::FromUTF8String("key1_for_2000"));
        orig_2000.push_back(DPL::FromUTF8String("key2_for_2000"));

        WidgetPropertyKeyList orig_2001;
        orig_2001.push_back(DPL::FromUTF8String("key1_for_2001"));

        WidgetPropertyKeyList orig_2002;
        orig_2002.push_back(DPL::FromUTF8String("key1_for_2002"));
        orig_2002.push_back(DPL::FromUTF8String("key2_for_2002"));

        std::map<WidgetHandle, WidgetPropertyKeyList *> prefsKeyMap;
        prefsKeyMap.insert(std::pair<WidgetHandle, WidgetPropertyKeyList *>(
                2000, &orig_2000));
        prefsKeyMap.insert(std::pair<WidgetHandle, WidgetPropertyKeyList *>(
                2001, &orig_2001));
        prefsKeyMap.insert(std::pair<WidgetHandle, WidgetPropertyKeyList *>(
                2002, &orig_2002));

        FOREACH(it_out, prefsKeyMap) {
            WidgetPropertyKeyList got = PropertyDAOReadOnly::GetPropertyKeyList(
                    it_out->first);
            RUNNER_ASSERT(got.size() == it_out->second->size());
            //TODO
            //            FOREACH(it_in, got)
            //            {
            //                RUNNER_ASSERT(it_out->second.
            //            }
        }
    }
}

/*
Name: property_dao_set_update_remove
Description: tests set new property for widget, updating property and removing it
Expected: given operation should works
*/
RUNNER_TEST(property_dao_set_update_remove)
{
    WidgetPropertyKeyList keys = PropertyDAOReadOnly::GetPropertyKeyList(2000);

    //ADD
    PropertyDAO::SetProperty(2000,
                             DPL::FromUTF8String("new_key"),
                             DPL::FromUTF8String("new_value1"));

    RUNNER_ASSERT_MSG(
        keys.size() + 1 == PropertyDAOReadOnly::GetPropertyKeyList(2000).size(),
        "new property not added");
    RUNNER_ASSERT_WHAT_EQUALS_OPTIONAL(
        PropertyDAOReadOnly::GetPropertyValue(2000,
                                              DPL::FromUTF8String("new_key")),
        "new_value1");

    //UPDATE
    PropertyDAO::SetProperty(2000,
                             DPL::FromUTF8String("new_key"),
                             DPL::FromUTF8String("new_value2"));
    RUNNER_ASSERT_MSG(
        keys.size() + 1 == PropertyDAOReadOnly::GetPropertyKeyList(2000).size(),
        "new property not added");
    RUNNER_ASSERT_WHAT_EQUALS_OPTIONAL(
        PropertyDAOReadOnly::GetPropertyValue(2000,
                                              DPL::FromUTF8String("new_key")),
        "new_value2");

    //REMOVE
    PropertyDAO::RemoveProperty(2000, DPL::FromUTF8String("new_key"));

    RUNNER_ASSERT_MSG(
        keys.size() == PropertyDAOReadOnly::GetPropertyKeyList(2000).size(),
        "property not removed");

}

/*
Name: property_dao_get_value
Description: tests if properties can be received from database
Expected: value, which were inserted before test, should be present
*/
RUNNER_TEST(property_dao_get_value)
{
    RUNNER_ASSERT_WHAT_EQUALS_OPTIONAL(
            PropertyDAOReadOnly::GetPropertyValue(
                    2000, DPL::FromUTF8String("key1_for_2000")),
                "value_for_key1_2000");

    RUNNER_ASSERT_WHAT_EQUALS_OPTIONAL(
            PropertyDAOReadOnly::GetPropertyValue(
                    2000, DPL::FromUTF8String("key2_for_2000")),
                "value_for_key2_2000");
}

#undef RUNNER_ASSERT_WHAT_EQUALS
