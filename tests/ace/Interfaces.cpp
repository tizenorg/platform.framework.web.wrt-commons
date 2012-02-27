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
 * @file        Interfaces.cpp
 * @author      Bartlomiej Grzelewski (b.grzelewski@samsung.com)
 * @version     1.0
 * @brief       Stubs used by PolicyInformationPoint.h
 */
#include "Interfaces.h"

#include <dpl/log/log.h>

#include "AttributeSetter.h"

int WebRuntimeImp::getAttributesValues(
        const Request &request,
        std::list<ATTRIBUTE> *attribute)
{
    LogError("Running stub");
    AttributeSetterSingleton::Instance().get(request, attribute);
    return 0;
}

int ResourceInformationImp::getAttributesValues(
        const Request &request,
        std::list<ATTRIBUTE> *attribute)
{
    LogError("Running stub");
    AttributeSetterSingleton::Instance().get(request, attribute);
    return 0;
}

int OperationSystemImp::getAttributesValues(
        const Request &request,
        std::list<ATTRIBUTE> *attribute)
{
    LogError("Running stub");
    AttributeSetterSingleton::Instance().get(request, attribute);
    return 0;
}

int FunctionParamImpl::getAttributesValues(const Request & /* request*/,
        std::list<ATTRIBUTE> *attributes)
{
    LogError("Running stub");
    FOREACH(iter, *attributes)
    {
        std::string attributeName = *(iter->first);

        ParamMap::const_iterator i;
        std::pair<ParamMap::const_iterator, ParamMap::const_iterator> jj =
          paramMap.equal_range(attributeName);

        for (i = jj.first; i != jj.second; ++i) {
            iter->second->push_back(i->second);
            LogDebug("Attribute: " << attributeName << " Value: " <<
              i->second);
        }
    }
    return 0;
}

