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
 * @file        read_write_mutex.h
 * @author      Przemyslaw Dobrowolski (p.dobrowolsk@samsung.com)
 * @version     1.0
 * @brief       This file is the implementation file of read write mutex
 */
#ifndef DPL_READ_WRITE_MUTEX_H
#define DPL_READ_WRITE_MUTEX_H

#include <dpl/noncopyable.h>
#include <dpl/exception.h>
#include <pthread.h>

namespace DPL
{
class ReadWriteMutex
    : private Noncopyable
{
public:
    class ScopedReadLock
        : private Noncopyable
    {
    private:
        ReadWriteMutex *m_mutex;

    public:
        ScopedReadLock(ReadWriteMutex *mutex);
        virtual ~ScopedReadLock();
    };

    class ScopedWriteLock
        : private Noncopyable
    {
    private:
        ReadWriteMutex *m_mutex;

    public:
        ScopedWriteLock(ReadWriteMutex *mutex);
        virtual ~ScopedWriteLock();
    };

    class Exception
    {
    public:
        DECLARE_EXCEPTION_TYPE(DPL::Exception, Base)
        DECLARE_EXCEPTION_TYPE(Base, CreateFailed)
        DECLARE_EXCEPTION_TYPE(Base, DestroyFailed)
        DECLARE_EXCEPTION_TYPE(Base, ReadLockFailed)
        DECLARE_EXCEPTION_TYPE(Base, WriteLockFailed)
        DECLARE_EXCEPTION_TYPE(Base, UnlockFailed)
    };

private:
    mutable pthread_rwlock_t m_rwlock;

    void ReadLock() const;
    void WriteLock() const;
    void Unlock() const;

public:
    explicit ReadWriteMutex();
    virtual ~ReadWriteMutex();
};

} // namespace DPL

#endif // DPL_READ_WRITE_MUTEX_H
