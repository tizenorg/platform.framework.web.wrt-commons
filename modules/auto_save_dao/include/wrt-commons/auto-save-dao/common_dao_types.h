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
 *
 * @file    common_dao_types.h
 * @author  Jihoon Chung (jihoon.chung@samsung.com)
 * @version 1.0
 * @brief This file contains the declaration of
 *           common data types for auto save database.
 */
#ifndef SHARE_COMMON_DAO_TYPES_H_
#define SHARE_COMMON_DAO_TYPES_H_

#include <list>
#include <dpl/string.h>

namespace AutoSaveDB {
struct SubmitFormElement
{
    DPL::String key;
    DPL::String value;

    SubmitFormElement()
    {}

    SubmitFormElement(
        const DPL::String& keyData,
        const DPL::String& valueData) :
        key(keyData),
        value(valueData)
    {}

    bool operator== (const SubmitFormElement& other) const
    {
        return (!DPL::StringCompare(key, other.key) &&
                !DPL::StringCompare(value, other.value));
    }

    bool operator!= (const SubmitFormElement& other) const
    {
        return !(*this == other);
    }
};
typedef std::list<SubmitFormElement> SubmitFormData;
} // namespace AutoSaveDB

#endif /* SHARE_COMMON_DAO_TYPES_H_ */
