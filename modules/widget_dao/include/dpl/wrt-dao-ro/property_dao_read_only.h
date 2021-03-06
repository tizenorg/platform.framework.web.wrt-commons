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
 * property_dao_read_only.h
 *
 *  Created on: Nov 16, 2011
 *      Author: Krzysztof Jackiewicz(k.jackiewicz@samsung.com)
 */

#ifndef PROPERTY_DAO_READ_ONLY_H_
#define PROPERTY_DAO_READ_ONLY_H_

#include <list>
#include <dpl/string.h>
#include <dpl/exception.h>
#include <dpl/db/orm.h>
#include <dpl/wrt-dao-ro/common_dao_types.h>

namespace WrtDB {
namespace PropertyDAOReadOnly {

typedef DPL::String WidgetPropertyKey;
typedef DPL::OptionalString WidgetPropertyValue;

typedef std::list<WidgetPropertyKey> WidgetPropertyKeyList;

struct WidgetPreferenceRow {
    int                 app_id;
    WidgetPropertyKey   key_name;
    WidgetPropertyValue key_value;
    DPL::OptionalInt    readonly;
};

typedef std::list<WidgetPreferenceRow> WidgetPreferenceList;

/**
 * PropertyDAO Exception classes
 */
class Exception
{
  public:
    DECLARE_EXCEPTION_TYPE(DPL::Exception, Base)
    DECLARE_EXCEPTION_TYPE(Base, DatabaseError)
    DECLARE_EXCEPTION_TYPE(Base, ReadOnlyProperty)
};

/* This method checks read only flag for given property
 */
DPL::OptionalInt CheckPropertyReadFlag(DbWidgetHandle widgetHandle,
                                  const WidgetPropertyKey &key);

/* This method gets widget property key list
 */
WidgetPropertyKeyList GetPropertyKeyList(DbWidgetHandle widgetHandle);

/* This method gets widget property list
 */
WidgetPreferenceList GetPropertyList(DbWidgetHandle widgetHandle);

/* This method get widget property value
 */
WidgetPropertyValue GetPropertyValue(DbWidgetHandle widgetHandle,
                                     const WidgetPropertyKey &key);

} // PropertyDAOReadOnly
} // namespace WrtDB

#endif /* PROPERTY_DAO_READ_ONLY_H_ */
