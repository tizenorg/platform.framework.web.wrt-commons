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
 * @file        semaphore.cpp
 * @author      Przemyslaw Dobrowolski (p.dobrowolsk@samsung.com)
 * @version     1.0
 * @brief       This file is the implementation file of semaphore
 */
#include <stddef.h>
#include <dpl/semaphore.h>
#include <dpl/assert.h>
#include <dpl/log/log.h>
#include <errno.h>
#include <malloc.h>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>

namespace DPL {
void Semaphore::Remove(const std::string &fileName)
{
    if (sem_unlink(fileName.c_str()) == -1) {
        int error = errno;
        LogPedantic("Failed to unlink semaphore. Errno: " << error);
        ThrowMsg(Exception::RemoveFailed,
                 "Failed to unlink semaphore. Errno: " << error);
    }
}

Semaphore::Semaphore(size_t maxLockCount)
{
    LogPedantic("Allocating unnamed semaphore:");
    LogPedantic("    Maximum lock count: " << maxLockCount);

    if (-1 == sem_init(&m_semaphore.unnamed.handle,
                       0,
                       static_cast<unsigned>(maxLockCount)))
    {
        int error = errno;

        LogPedantic("Failed to create semaphore. Errno: " << error);

        ThrowMsg(Exception::CreateFailed,
                 "Failed to create semaphore. Errno: " << error);
    }

    m_type = Type_Unnamed;
}

Semaphore::Semaphore(const std::string &fileName,
                     bool allowCreate,
                     bool exclusiveCreate,
                     size_t maxLockCount,
                     int permissions,
                     bool unlinkOnDestroy)
{
    LogPedantic("Allocating named semaphore:");
    LogPedantic("    File name: " << fileName);
    LogPedantic("    Maximum lock count: " << maxLockCount);
    LogPedantic("    File name: " << fileName);
    LogPedantic("    Allowed create: " << allowCreate);
    LogPedantic("    Exclusive create: " << exclusiveCreate);
    LogPedantic("    Permissions: " << permissions);
    LogPedantic("    Unlink on destroy: " << unlinkOnDestroy);

    sem_t *semaphore;

    do {
        if (allowCreate) {
            if (exclusiveCreate) {
                semaphore = sem_open(fileName.c_str(),
                                     O_CREAT | O_EXCL,
                                     permissions,
                                     static_cast<unsigned>(maxLockCount));
            } else {
                semaphore = sem_open(fileName.c_str(),
                                     O_CREAT,
                                     permissions,
                                     static_cast<unsigned>(maxLockCount));
            }
        } else {
            semaphore = sem_open(fileName.c_str(), 0);
        }
    } while (semaphore == SEM_FAILED && errno == EINTR);

    if (semaphore == SEM_FAILED) {
        int error = errno;

        LogPedantic("Failed to create semaphore '" << fileName
                                                   << "'. Errno: " << error);

        ThrowMsg(Exception::CreateFailed,
                 "Failed to create semaphore '" << fileName
                                                << "'. Errno: " << error);
    }

    m_semaphore.named.handle = semaphore;

    m_semaphore.named.name = strdup(fileName.c_str()); // May be NULL

    if (m_semaphore.named.name == NULL) {
        LogPedantic("Out of memory while duplicating semaphore name");
    }

    m_semaphore.named.unlinkOnDestroy = unlinkOnDestroy;

    m_type = Type_Named;
}

Semaphore::~Semaphore()
{
    InternalDestroy();
}

sem_t *Semaphore::InternalGet() const
{
    switch (m_type) {
    case Type_Unnamed:
        return &m_semaphore.unnamed.handle;

    case Type_Named:
        return m_semaphore.named.handle;

    default:
        Assert(false && "Invalid type");
    }

    return NULL;
}

void Semaphore::InternalDestroy()
{
    switch (m_type) {
    case Type_Unnamed:
        if (sem_destroy(&m_semaphore.unnamed.handle) == -1) {
            int error = errno;

            LogPedantic("Failed to destroy semaphore. Errno: " << error);
        }
        break;

    case Type_Named:
        if (sem_close(m_semaphore.named.handle) == -1) {
            int error = errno;

            LogPedantic("Failed to close semaphore. Errno: " << error);
        }

        if (m_semaphore.named.name != NULL) {
            // Unlink named semaphore
            if (m_semaphore.named.unlinkOnDestroy &&
                sem_unlink(m_semaphore.named.name) == -1)
            {
                int error = errno;

                LogPedantic("Failed to unlink semaphore. Errno: "
                            << error);
            }

            // Free name
            free(m_semaphore.named.name);
        }
        break;

    default:
        Assert(false && "Invalid type");
    }
}

void Semaphore::Lock() const
{
    if (TEMP_FAILURE_RETRY(sem_wait(InternalGet())) != 0) {
        int error = errno;

        LogPedantic("Failed to lock semaphore. Errno: " << error);

        ThrowMsg(Exception::LockFailed,
                 "Failed to lock semaphore. Errno: " << error);
    }
}

void Semaphore::Unlock() const
{
    if (sem_post(InternalGet()) != 0) {
        int error = errno;

        LogPedantic("Failed to unlock semaphore. Errno: " << error);

        ThrowMsg(Exception::UnlockFailed,
                 "Failed to unlock semaphore. Errno: " << error);
    }
}

Semaphore::ScopedLock::ScopedLock(Semaphore *semaphore) :
    m_semaphore(semaphore)
{
    Assert(semaphore != NULL);
    m_semaphore->Lock();
}

Semaphore::ScopedLock::~ScopedLock()
{
    Try
    {
        m_semaphore->Unlock();
    }
    Catch(Semaphore::Exception::UnlockFailed)
    {
        LogPedantic("Failed to leave semaphore scoped lock");
    }
}
} // namespace DPL
