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
 * @file    AttributeSetter.h
 * @author  Bartlomiej Grzelewski (b.grzelewski@samsung.com)
 * @version 0.1
 * @brief   Stub for attribute_facade.h
 */
#ifndef _TEST_ACE_TESTS_ATTRIBUTE_SETTER_
#define _TEST_ACE_TESTS_ATTRIBUTE_SETTER_

#include <list>
#include <map>
#include <string>

#include <dpl/singleton.h>
#include <dpl/foreach.h>
#include <dpl/log/log.h>

#include <dpl/ace/Request.h>
#include <dpl/ace/WRT_INTERFACE.h>

class AttributeSetter {
public:
    typedef std::list<std::string> ValueList;
    typedef std::map<std::string, ValueList> AttributeMap;
    typedef std::map<int, AttributeMap> CollectionHandlerMap;

    typedef std::list<ATTRIBUTE> Evil;

    void get(const Request &request,
            const Evil* evil)
    {
        int handle = request.getWidgetHandle();

        CollectionHandlerMap::iterator widgetContextIt =
            m_collectionMap.find(handle);

        if (widgetContextIt == m_collectionMap.end()) {
            LogError("WidgetContext is empty");
            return;
        }

        FOREACH(it, *evil)
        {
            std::string attrName = *(it->first);

            if (attrName == "device-cap") {
                Request::DeviceCapabilitySet devCapSet =
                    request.getDeviceCapabilitySet();
                std::copy(devCapSet.begin(),
                          devCapSet.end(),
                          std::back_inserter(*(it->second)));
                continue;
            }

            AttributeMap::iterator attrIt =
                widgetContextIt->second.find(attrName);

            if (attrIt == widgetContextIt->second.end()) {
                LogError("No attribute: " << attrName <<
                    " in context nr: " << handle);
                const ATTRIBUTE &a = *it;
                const_cast<ATTRIBUTE&>(a).second = NULL;
                continue;
            }

            ValueList valueList = attrIt->second;

            std::copy(valueList.begin(),
                      valueList.end(),
                      std::back_inserter(*(it->second)));
        }
    }

    void addValue(int handler,
                  const std::string &attributeName,
                  const std::string &attributeValue)
    {
        m_collectionMap[handler][attributeName].push_back(attributeValue);
    }

    std::string getValue(int handler, const std::string &attributeName)
    {
        std::list<std::string> lst = m_collectionMap[handler][attributeName];
        if (lst.empty())
            return "fake-device-name-that-should-not-match-with-anything";
        return lst.front();
    }

    void addValue(int handler,
                  const char *attributeName,
                  const char *attributeValue)
    {
        std::string aN = attributeName;
        std::string aV = attributeValue;
        addValue(handler, aN, aV);
    }

    void clear() {
        m_collectionMap.clear();
    }
protected:
    AttributeSetter(){}
    ~AttributeSetter(){}
private:
    CollectionHandlerMap m_collectionMap;
};

typedef DPL::Singleton<AttributeSetter> AttributeSetterSingleton;

#endif // _TEST_ACE_TESTS_ATTRIBUTE_SETTER_
