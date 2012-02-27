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
/*
 * @file        Interfaces.h
 * @author      Bartlomiej Grzelewski (b.grzelewski@samsung.com)
 * @version     1.0
 * @brief       Stubs used by PolicyInformationPoint.h
 */
#ifndef _TEST_ACE_TESTS_INTERFACE_H_
#define _TEST_ACE_TESTS_INTERFACE_H_

#include <string>
#include <dpl/ace/WRT_INTERFACE.h>
#include <map>
#include <vector>

class Request;

class WebRuntimeImp : public IWebRuntime {
public:
    int getAttributesValues(
            const Request &request,
            std::list<ATTRIBUTE> *attribute);
    std::string getSessionId(const Request &) { return std::string(); }
};

class ResourceInformationImp : public IResourceInformation {
public:
    int getAttributesValues(
            const Request &request,
            std::list<ATTRIBUTE> *attribute);
};

class OperationSystemImp : public IOperationSystem {
public:
    int getAttributesValues(
            const Request &request,
            std::list<ATTRIBUTE> *attribute);
};

class FunctionParamImpl : public IFunctionParam
{
public:
    virtual int getAttributesValues(const Request & /* request*/,
      std::list<ATTRIBUTE> *attributes);
    void addAttribute(const std::string &key,
      const std::string &value)
    {
        paramMap.insert(make_pair(key, value));
    }
    virtual ~FunctionParamImpl() {}
private:
    typedef std::multimap<std::string, std::string> ParamMap;
    ParamMap paramMap;
};

#endif //  _TEST_ACE_TESTS_INTERFACE_H_
