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
 * @file        recursive_mutex.cpp
 * @author      Przemyslaw Dobrowolski (p.dobrowolsk@samsung.com)
 * @version     1.0
 * @brief       This file is the implementation file of recursive mutex
 */
#include <stddef.h>
#include <dpl/recursive_mutex.h>
#include <dpl/assert.h>

namespace DPL {
RecursiveMutex::RecursiveMutex()
{
    pthread_mutexattr_t attr;

    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);

    if (pthread_mutex_init(&m_mutex, &attr) != 0) {
        Throw(Exception::CreateFailed);
    }
}

RecursiveMutex::~RecursiveMutex()
{
    if (pthread_mutex_destroy(&m_mutex) != 0) {
        Throw(Exception::DestroyFailed);
    }
}

void RecursiveMutex::Lock() const
{
    if (pthread_mutex_lock(&m_mutex) != 0) {
        Throw(Exception::LockFailed);
    }
}

void RecursiveMutex::Unlock() const
{
    if (pthread_mutex_unlock(&m_mutex) != 0) {
        Throw(Exception::UnlockFailed);
    }
}

RecursiveMutex::ScopedLock::ScopedLock(RecursiveMutex *mutex) :
    m_mutex(mutex)
{
    Assert(mutex != NULL);
    m_mutex->Lock();
}

RecursiveMutex::ScopedLock::~ScopedLock()
{
    m_mutex->Unlock();
}
} // namespace DPL
