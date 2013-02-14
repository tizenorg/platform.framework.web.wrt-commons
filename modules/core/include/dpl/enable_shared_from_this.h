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
 * @file        enable_shared_from_this.h
 * @author      Przemyslaw Dobrowolski (p.dobrowolsk@samsung.com)
 * @version     1.0
 * @brief       This file is the implementation file of shared pointer RAII
 */
#ifndef DPL_ENABLE_SHARED_FROM_THIS_H
#define DPL_ENABLE_SHARED_FROM_THIS_H

#include <dpl/shared_ptr.h>
#include <dpl/noncopyable.h>
#include <dpl/assert.h>

namespace DPL {
template<typename Class>
class EnableSharedFromThis : private Noncopyable
{
  private:
    // A weak pointer to shared counter
    SharedCounter *m_counter;
    Class *m_ptr;

  public:
    DPL::SharedPtr<Class> SharedFromThis()
    {
        Assert(m_counter != NULL && "Pointer is not shared!");
        return SharedPtr<Class>(m_counter, m_ptr);
    }

    DPL::SharedPtr<const Class> SharedFromThis() const
    {
        Assert(m_counter != NULL && "Pointer is not shared!");
        return SharedPtr<Class>(m_counter, m_ptr);
    }

    // For internal SharedPtr usage only. Do not call directly.
    void _Internal_AcceptSharedPtr(SharedCounter *counter, Class *ptr)
    {
        m_counter = counter;
        m_ptr = ptr;
    }

    EnableSharedFromThis() :
        m_counter(NULL),
        m_ptr(NULL)
    {}

    virtual ~EnableSharedFromThis()
    {}
};
} // namespace DPL

#endif // DPL_ENABLE_SHARED_FROM_THIS_H
