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
 * @file        read_write_mutex.cpp
 * @author      Przemyslaw Dobrowolski (p.dobrowolsk@samsung.com)
 * @version     1.0
 * @brief       This file is the implementation file of read write mutex
 */
#include <stddef.h>
#include <dpl/read_write_mutex.h>
#include <dpl/assert.h>

namespace DPL
{
ReadWriteMutex::ReadWriteMutex()
{
    if (pthread_rwlock_init(&m_rwlock, NULL) != 0)
        Throw(Exception::CreateFailed);
}

ReadWriteMutex::~ReadWriteMutex()
{
    if (pthread_rwlock_destroy(&m_rwlock) != 0)
        Throw(Exception::DestroyFailed);
}

void ReadWriteMutex::ReadLock() const
{
    if (pthread_rwlock_rdlock(&m_rwlock) != 0)
        Throw(Exception::ReadLockFailed);
}

void ReadWriteMutex::WriteLock() const
{
    if (pthread_rwlock_wrlock(&m_rwlock) != 0)
        Throw(Exception::WriteLockFailed);
}

void ReadWriteMutex::Unlock() const
{
    if (pthread_rwlock_unlock(&m_rwlock) != 0)
        Throw(Exception::UnlockFailed);
}

ReadWriteMutex::ScopedReadLock::ScopedReadLock(ReadWriteMutex *mutex)
    : m_mutex(mutex)
{
    Assert(mutex != NULL);
    m_mutex->ReadLock();
}

ReadWriteMutex::ScopedReadLock::~ScopedReadLock()
{
    m_mutex->Unlock();
}

ReadWriteMutex::ScopedWriteLock::ScopedWriteLock(ReadWriteMutex *mutex)
    : m_mutex(mutex)
{
    Assert(mutex != NULL);
    m_mutex->WriteLock();
}

ReadWriteMutex::ScopedWriteLock::~ScopedWriteLock()
{
    m_mutex->Unlock();
}
} // namespace DPL
