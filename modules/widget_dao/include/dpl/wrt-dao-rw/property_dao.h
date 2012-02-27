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
 * @file    property_dao.h
 * @author  Pawel Sikorski (p.sikorski@samsung.com)
 * @version 1.0
 * @brief   This file contains the declaration of property dao
 */

#ifndef WRT_SRC_CONFIGURATION_PROPERTY_DAO_H_
#define WRT_SRC_CONFIGURATION_PROPERTY_DAO_H_

#include <dpl/wrt-dao-ro/property_dao_read_only.h>

namespace WrtDB {

struct WidgetRegisterInfo; //forward declaration

namespace PropertyDAO {

void RemoveProperty(DbWidgetHandle widgetHandle,
                    const PropertyDAOReadOnly::WidgetPropertyKey &key);

/* This method sets widget property
 */
void SetProperty(DbWidgetHandle widgetHandle,
                 const PropertyDAOReadOnly::WidgetPropertyKey &key,
                 const PropertyDAOReadOnly::WidgetPropertyValue &value,
                 bool readOnly = false);

/* This method registers properties for widget.
 * Properties unregistering is done via "delete cascade" mechanism in SQL
 */
void RegisterProperties(DbWidgetHandle widgetHandle,
                        const WidgetRegisterInfo &regInfo);

} // namespace PropertyDAO
} // namespace WrtDB

#endif /* WRT_SRC_CONFIGURATION_PROPERTY_DAO_H_ */
