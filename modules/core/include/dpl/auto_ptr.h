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
 * @file        auto_ptr.h
 * @author      Bartlomiej Grzelewski (b.grzelewski@samsung.com)
 * @author      Tomasz Swierczek (t.swierczek@samsung.com)
 * @version     1.0
 * @brief       This file contais definictions of specific AutoPtr class.
 */
#ifndef _AUTO_PTR_H_
#define _AUTO_PTR_H_

#include <list>
#include <set>
#include <string>

#include <dpl/availability.h>

namespace DPL {
/*
 * base deleter func
 */
template <typename T>
struct UniversalFree {};

// Template Specialization
#define DECLARE_DELETER(type, function)           \
    template <> \
    struct UniversalFree <type> {           \
        void universal_free(type * ptr){                  \
            if (ptr) {                                      \
                function(ptr); }                           \
        }                                                \
    };

template <typename T>
class AutoPtr
{
  public:
    DPL_DEPRECATED_WITH_MESSAGE("use std::unique_ptr or std::shared_ptr") AutoPtr(T *ptr) :
        m_data(ptr)
    {}

    AutoPtr(const AutoPtr<T> &second)
    {
        m_data = second.m_data;
        second.m_data = 0;
    }

    AutoPtr & operator=(const AutoPtr &second)
    {
        if (this != &second) {
            UniversalFree<T> deleter;
            deleter.universal_free(m_data);
            m_data = second.m_data;
            second.m_data = 0;
        }
        return *this;
    }

    /**
     * overloaded -> operator, so smart ptr could act as ordinary ptr
     */
    T* operator->()
    {
        return m_data;
    }

    ~AutoPtr()
    {
        UniversalFree<T> deleter;
        deleter.universal_free(m_data);
    }

    /**
     * get internal pointer
     */
    T* get(void)
    {
        return m_data;
    }

  private:
    mutable T *m_data;
};
} // namespace DPL

#endif // AUTO_PTR
