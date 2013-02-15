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
 * @file    feature_model.h
 * @author  Pawel Sikorski (p.sikorski@samgsung.com)
 * @version
 * @brief   This file contains FeatureModel, FeatureHandle definitions.
 */
#ifndef FEATURE_MODEL_H
#define FEATURE_MODEL_H

#include <dpl/event/model.h>
#include <dpl/event/property.h>
#include <memory>
#include <string>
#include <list>
#include <set>
#include <dpl/wrt-dao-ro/common_dao_types.h>

namespace WrtDB {

typedef int FeatureHandle;
typedef std::list<FeatureHandle> FeatureHandleList;
typedef std::shared_ptr<FeatureHandleList> FeatureHandleListPtr;

typedef int FeatureSetHandle;
typedef std::list<FeatureSetHandle> FeatureSetHandleList;

typedef struct {
    std::string     featureName;
    DbPluginHandle  pluginHandle;
} FeatureData;

class FeatureModel : public DPL::Event::Model
{
  public:
    DPL::Event::Property<FeatureHandle, DPL::Event::PropertyReadOnly> FHandle;
    DPL::Event::Property<std::string> Name;

    DPL::Event::Property<std::set<std::string> > DeviceCapabilities;
    DPL::Event::Property<DbPluginHandle> PHandle;

    FeatureModel(FeatureHandle handle) :
        FHandle(this, handle),
        Name(this),
        DeviceCapabilities(this),
        PHandle(this, -1)
    {
    }

    void SetData(const std::string& name, const std::set<std::string>& deviceCapabilities, const DbPluginHandle& pluginHandle)
    {
        Name.SetWithoutLock(name);
        DeviceCapabilities.SetWithoutLock(deviceCapabilities);
        PHandle.SetWithoutLock(pluginHandle);
    }
};

typedef std::shared_ptr<FeatureModel> FeatureModelPtr;

} // namespace WrtDB

#endif // FEATURE_MODEL_H
