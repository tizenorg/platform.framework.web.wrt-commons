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
 * @file    bind_to_dao.h
 * @author  Grzegorz Krawczyk (g.krawczyk@samsung.com)
 * @version 1.0
 * @brief
 */

#ifndef WRT_SRC_CONFIGURATION_BIND_TO_DAO_H_
#define WRT_SRC_CONFIGURATION_BIND_TO_DAO_H_

namespace WrtDB {

/**
 * @param ObjectType type of object used as delegate argument
 * @param RetType Type returned from the external function
 * @param ExtArg Type of argument required by external fun
 * @param getterFun Object Type method which returns value of type ExtArg
 * used as argument for external function
 * */
//STATIC FUNCTION
template <
    typename ObjectType,
    typename ValueType,
    typename ExtArg,
    ExtArg(ObjectType::*argGetter) () const,
    ValueType(*externalGetter) (ExtArg)
    >
struct BindToDAO_Static
{
    static ValueType Get(DPL::Model* obj)
    {
        ObjectType* instance = static_cast<ObjectType*>(obj);

        return externalGetter((instance->*argGetter)());
    }
};

template <
    typename ObjectType,
    typename ValueType,
    typename ExtArg,
    typename ExtObject,
    ExtArg(ObjectType::*argGetter) () const,
    ValueType(ExtObject::*externalGetter) () const
    >
struct BindToDAO
{
    static ValueType Get(DPL::Model* obj)
    {
        ObjectType* instance = static_cast<ObjectType*>(obj);
        ExtObject extObject((instance->*argGetter)());
        return (extObject.*externalGetter)();
    }
};

} // namespace WrtDB

#endif
