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
 * @file        shared_ptr.h
 * @author      Przemyslaw Dobrowolski (p.dobrowolsk@samsung.com)
 * @version     1.0
 * @brief       This file is the implementation file of shared pointer RAII
 */
#ifndef DPL_SHARED_PTR_H
#define DPL_SHARED_PTR_H

#include <dpl/noncopyable.h>
#include <dpl/atomic.h>
#include <dpl/bool_operator.h>
#include <stddef.h>
#include <dpl/assert.h>

namespace DPL {
struct StaticPointerCastTag {};
struct ConstPointerCastTag {};
struct DynamicPointerCastTag {};

struct SharedCounter
{
    SharedCounter() :
        ref(1)
    {}

    Atomic ref;
};

template<typename Class>
class EnableSharedFromThis;

template<typename Other>
inline void _Internal_AcceptSharedPtr(SharedCounter *counter,
                                      Other *other,
                                      EnableSharedFromThis<Other> *otherBase)
{
    otherBase->_Internal_AcceptSharedPtr(counter, other);
}

struct AnyPointer
{
    template<typename Other>
    AnyPointer(Other *)
    {}
};

inline void _Internal_AcceptSharedPtr(SharedCounter *, AnyPointer, AnyPointer)
{}

template<typename Class>
class SharedPtr
{
  public:
    typedef Class ValueType;
    typedef SharedPtr<Class> ThisType;

  private:
    SharedCounter *m_counter;
    Class *m_ptr;

    void AttachCounter(const SharedCounter *counter)
    {
        // Attention: R-Value const cast
        m_counter = const_cast<SharedCounter *>(counter);

        if (m_counter != NULL) {
            ++m_counter->ref;
        }
    }

    void DetachCounter()
    {
        if (m_counter) {
            if (!--m_counter->ref) {
                delete m_ptr;
                delete m_counter;
            }

            m_counter = NULL;
            m_ptr = NULL;
        }
    }

  public:
    SharedPtr() :
        m_counter(NULL),
        m_ptr(NULL)
    {}

    explicit SharedPtr(Class *ptr) :
        m_counter(NULL),
        m_ptr(ptr)
    {
        if (m_ptr != NULL) {
            m_counter = new SharedCounter();
            _Internal_AcceptSharedPtr(m_counter, m_ptr, m_ptr);
        }
    }

    SharedPtr(const SharedPtr &other) :
        m_counter(NULL),
        m_ptr(other.m_ptr)
    {
        AttachCounter(other.m_counter);
    }

    SharedPtr(SharedCounter *counter, Class *ptr) :
        m_counter(NULL),
        m_ptr(ptr)
    {
        AttachCounter(counter);
    }

    template<typename Other>
    friend class SharedPtr;

    template<typename Other>
    SharedPtr(const SharedPtr<Other> &other, const StaticPointerCastTag &) :
        m_counter(NULL),
        m_ptr(NULL)
    {
        m_ptr = static_cast<Class *>(other.m_ptr);
        AttachCounter(other.m_counter);
    }

    template<typename Other>
    SharedPtr(const SharedPtr<Other> &other, const ConstPointerCastTag &) :
        m_counter(NULL),
        m_ptr(NULL)
    {
        m_ptr = const_cast<Class *>(other.m_ptr);
        AttachCounter(other.m_counter);
    }

    template<typename Other>
    SharedPtr(const SharedPtr<Other> &other, const DynamicPointerCastTag &) :
        m_counter(NULL),
        m_ptr(NULL)
    {
        Class *ptr = dynamic_cast<Class *>(other.Get());

        if (ptr == NULL) {
            return;
        }

        m_ptr = ptr;
        AttachCounter(other.m_counter);
    }

    virtual ~SharedPtr()
    {
        DetachCounter();
    }

    Class *Get() const
    {
        return m_counter == NULL ? NULL : m_ptr;
    }

    Class *operator->() const
    {
        Assert(m_counter != NULL && "Dereference of shared NULL pointer!");
        return m_ptr;
    }

    Class &operator *() const
    {
        Assert(m_counter != NULL && "Dereference of shared NULL pointer!");
        return *m_ptr;
    }

    void Reset(Class *ptr = NULL)
    {
        DetachCounter();

        if (ptr != NULL) {
            m_ptr = ptr;
            m_counter = new SharedCounter();
            _Internal_AcceptSharedPtr(m_counter, m_ptr, m_ptr);
        }
    }

    SharedPtr &operator=(const SharedPtr &other)
    {
        if (this != &other) {
            DetachCounter();
            m_ptr = other.m_ptr;
            AttachCounter(other.m_counter);
        }

        return *this;
    }

    Atomic::ValueType GetUseCount() const
    {
        if (m_counter == NULL) {
            return Atomic::ValueType(0);
        }

        return m_counter->ref;
    }

    DPL_IMPLEMENT_BOOL_OPERATOR(ValueType, ThisType, m_counter, m_ptr)
};

template<typename Target, typename Source>
SharedPtr<Target> StaticPointerCast(const SharedPtr<Source> &ptr)
{
    return SharedPtr<Target>(ptr, StaticPointerCastTag());
}

template<typename Target, typename Source>
SharedPtr<Target> ConstPointerCast(const SharedPtr<Source> &ptr)
{
    return SharedPtr<Target>(ptr, ConstPointerCastTag());
}

template<typename Target, typename Source>
SharedPtr<Target> DynamicPointerCast(const SharedPtr<Source> &ptr)
{
    return SharedPtr<Target>(ptr, DynamicPointerCastTag());
}

template<typename First, typename Second>
inline bool operator ==(const SharedPtr<First> &first,
                        const SharedPtr<Second> &second)
{
    return first.Get() == second.Get();
}

template<typename First, typename Second>
inline bool operator !=(const SharedPtr<First> &first,
                        const SharedPtr<Second> &second)
{
    return first.Get() != second.Get();
}

template<typename First, typename Second>
inline bool operator <(const SharedPtr<First> &first,
                       const SharedPtr<Second> &second)
{
    return first.Get() < second.Get();
}
template<typename First, typename Second>
inline bool operator >(const SharedPtr<First> &first,
                       const SharedPtr<Second> &second)
{
    return first.Get() > second.Get();
}

template<typename First, typename Second>
inline bool operator <=(const SharedPtr<First> &first,
                        const SharedPtr<Second> &second)
{
    return first.Get() <= second.Get();
}

template<typename First, typename Second>
inline bool operator >=(const SharedPtr<First> &first,
                        const SharedPtr<Second> &second)
{
    return first.Get() >= second.Get();
}
} // namespace DPL

#endif // DPL_SHARED_PTR_H
