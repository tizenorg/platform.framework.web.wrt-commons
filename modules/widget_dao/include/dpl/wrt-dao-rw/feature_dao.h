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
 * This file contains the declaration of feature dao class.
 *
 * @file    feature_dao.h
 * @author  Jaroslaw Osmanski (j.osmanski@samsung.com)
 * @author  Pawel Sikorski (p.sikorski@samsung.com)
 * @version 1.0
 * @brief   This file contains the declaration of feature dao
 */
#ifndef _FEATURE_DAO_H
#define _FEATURE_DAO_H

#include <dpl/wrt-dao-ro/feature_dao_read_only.h>

namespace WrtDB {
namespace FeatureDAO {
    FeatureHandle RegisterFeature(const PluginMetafileData::Feature &feature,
                                  const DbPluginHandle pluginHandle);
} // namespace FeatureDB
} // namespace WrtDB

#endif    /* _FEATURE_DAO_H */

