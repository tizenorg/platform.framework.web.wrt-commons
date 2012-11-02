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
 * @file        single_instance.cpp
 * @author      Przemyslaw Dobrowolski (p.dobrowolsk@samsung.com)
 * @version     1.0
 * @brief       This file is the implementation file of single instance
 */
#include <stddef.h>
#include <dpl/single_instance.h>
#include <dpl/log/log.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <dpl/assert.h>

namespace DPL
{
namespace // anonumous
{
const char *LOCK_PREFIX_PATH = "/tmp/dpl_single_instance_";
}
SingleInstance::SingleInstance()
    : m_locked(false),
      m_fdLock(-1)
{
}

SingleInstance::~SingleInstance()
{
    Assert(!m_locked && "Single instance must be released before exit!");
}

bool SingleInstance::TryLock(const std::string &lockName)
{
    LogPedantic("Locking single instance: " << lockName);

    struct flock lock;

    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 1;

    // Open lock file
    m_fdLock = TEMP_FAILURE_RETRY(open((std::string(LOCK_PREFIX_PATH) + lockName).c_str(), O_WRONLY | O_CREAT, 0666));

    if (m_fdLock == -1)
        ThrowMsg(Exception::LockError, "Cannot open single instance lock file!");

    // Lock file
    int result = TEMP_FAILURE_RETRY(fcntl(m_fdLock, F_SETLK, &lock));

    // Was the instance successfuly locked ?
    if (result == 0)
    {
        LogPedantic("Instance locked: " << lockName);

        // It is locked now
        m_locked = true;

        // Done
        return true;
    }

    if (errno == EACCES || errno == EAGAIN)
    {
        LogPedantic("Instance is already running: " << lockName);
        return false;
    }

    // This is lock error
    ThrowMsg(Exception::LockError, "Cannot lock single instance lock file!");
}

void SingleInstance::Release()
{
    if (!m_locked)
        return;

    LogPedantic("Unlocking single instance");

    // Unlock file
    struct flock lock;

    lock.l_type = F_UNLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 1;

    int result = TEMP_FAILURE_RETRY(fcntl(m_fdLock, F_SETLK, &lock));

    // Was the instance successfuly unlocked ?
    if (result == -1)
        ThrowMsg(Exception::LockError, "Cannot unlock single instance lock file!");

    // Close lock file
    if (TEMP_FAILURE_RETRY(close(m_fdLock)) == -1)
        ThrowMsg(Exception::LockError, "Cannot close single instance lock file!");

    m_fdLock = -1;

    // Done
    m_locked = false;
    LogPedantic("Instance unlocked");
}
} // namespace DPL
