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
 * @file        scoped_ptr.h
 * @author      Przemyslaw Dobrowolski (p.dobrowolsk@samsung.com)
 * @version     1.0
 * @brief       This file is the implementation file of scoped pointer RAII
 */
#ifndef DPL_SCOPED_PTR_H
#define DPL_SCOPED_PTR_H

#include <cstddef>

#include <dpl/scoped_resource.h>
#include <dpl/assert.h>

namespace DPL
{
template<typename Class>
struct ScopedPtrPolicy
{
    typedef Class* Type;
    static Type NullValue() { return NULL; }
    static void Destroy(Type ptr) { delete ptr; }
};

template<typename Class, typename ClassPolicy = ScopedPtrPolicy<Class> >
class ScopedPtr : public ScopedResource<ClassPolicy>
{
    typedef ClassPolicy Policy;
    typedef ScopedResource<Policy> BaseType;

  public:
    explicit ScopedPtr(Class *ptr = Policy::NullValue()) : BaseType(ptr) { }

    Class *operator->() const throw()
    {
        Assert(this->m_value != Policy::NullValue() &&
               "Dereference of scoped NULL pointer!");
        return this->m_value;
    }

    Class &operator *() const throw()
    {
        Assert(this->m_value != Policy::NullValue() &&
               "Dereference of scoped NULL pointer!");
        return *this->m_value;
    }
};
} // namespace DPL

#endif // DPL_SCOPED_PTR_H
