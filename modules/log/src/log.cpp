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
 * @file        log.cpp
 * @author      Przemyslaw Dobrowolski (p.dobrowolsk@samsung.com)
 * @version     1.0
 * @brief       This file is the implementation file of log system
 */
#include <dpl/log/log.h>
#include <dpl/singleton_impl.h>

IMPLEMENT_SINGLETON(DPL::Log::LogSystem)

namespace DPL
{
namespace Log
{
namespace // anonymous
{
const char *OLD_STYLE_LOGS_ENV_NAME = "DPL_USE_OLD_STYLE_LOGS";
const char *OLD_STYLE_PEDANTIC_LOGS_ENV_NAME = "DPL_USE_OLD_STYLE_PEDANTIC_LOGS";
const char *OLD_STYLE_LOGS_MASK_ENV_NAME = "DPL_USE_OLD_STYLE_LOGS_MASK";
const char *DPL_LOG_OFF = "DPL_LOG_OFF";
} // namespace anonymous


bool LogSystem::IsLoggingEnabled() const
{
    return m_isLoggingEnabled;
}

LogSystem::LogSystem()
    : m_dlogProvider(NULL),
      m_oldStyleProvider(NULL),
      m_isLoggingEnabled(!getenv(DPL_LOG_OFF))
{
    bool oldStyleLogs = false;
    bool oldStyleDebugLogs = true;
    bool oldStyleInfoLogs = true;
    bool oldStyleWarningLogs = true;
    bool oldStyleErrorLogs = true;
    bool oldStylePedanticLogs = false;

    // Check environment settings about pedantic logs
    const char *value = getenv(OLD_STYLE_LOGS_ENV_NAME);

    if (value != NULL && !strcmp(value, "1"))
        oldStyleLogs = true;

    value = getenv(OLD_STYLE_PEDANTIC_LOGS_ENV_NAME);

    if (value != NULL && !strcmp(value, "1"))
        oldStylePedanticLogs = true;

    value = getenv(OLD_STYLE_LOGS_MASK_ENV_NAME);

    if (value != NULL)
    {
        size_t len = strlen(value);

        if (len >= 1)
        {
            if (value[0] == '0')
                oldStyleDebugLogs = false;
            else if (value[0] == '1')
                oldStyleDebugLogs = true;
        }

        if (len >= 2)
        {
            if (value[1] == '0')
                oldStyleInfoLogs = false;
            else if (value[1] == '1')
                oldStyleInfoLogs = true;
        }

        if (len >= 3)
        {
            if (value[2] == '0')
                oldStyleWarningLogs = false;
            else if (value[2] == '1')
                oldStyleWarningLogs = true;
        }

        if (len >= 4)
        {
            if (value[3] == '0')
                oldStyleErrorLogs = false;
            else if (value[3] == '1')
                oldStyleErrorLogs = true;
        }
    }

    // Setup default DLOG and old style logging
    if (oldStyleLogs)
    {
        // Old style
        m_oldStyleProvider = new OldStyleLogProvider(oldStyleDebugLogs, oldStyleInfoLogs, oldStyleWarningLogs, oldStyleErrorLogs, oldStylePedanticLogs);
        AddProvider(m_oldStyleProvider);
    }
    else
    {
        // DLOG
        m_dlogProvider = new DLOGLogProvider();
        AddProvider(m_dlogProvider);
    }
}

LogSystem::~LogSystem()
{
    // Delete all providers
    for (AbstractLogProviderPtrList::iterator iterator = m_providers.begin(); iterator != m_providers.end(); ++iterator)
        delete *iterator;

    m_providers.clear();

    // And even default providers
    m_dlogProvider = NULL;
    m_oldStyleProvider = NULL;
}

void LogSystem::SetTag(const char* tag)
{
    if (m_dlogProvider != NULL)
        m_dlogProvider->SetTag(tag);
}

void LogSystem::AddProvider(AbstractLogProvider *provider)
{
    ReadWriteMutex::ScopedWriteLock lock(&m_spinLock);
    m_providers.push_back(provider);
}

void LogSystem::RemoveProvider(AbstractLogProvider *provider)
{
    ReadWriteMutex::ScopedWriteLock lock(&m_spinLock);
    m_providers.remove(provider);
}

void LogSystem::Debug(const char *message, const char *filename, int line, const char *function)
{
    ReadWriteMutex::ScopedReadLock lock(&m_spinLock);

    for (AbstractLogProviderPtrList::iterator iterator = m_providers.begin(); iterator != m_providers.end(); ++iterator)
        (*iterator)->Debug(message, filename, line, function);
}

void LogSystem::Info(const char *message, const char *filename, int line, const char *function)
{
    ReadWriteMutex::ScopedReadLock lock(&m_spinLock);

    for (AbstractLogProviderPtrList::iterator iterator = m_providers.begin(); iterator != m_providers.end(); ++iterator)
        (*iterator)->Info(message, filename, line, function);
}

void LogSystem::Warning(const char *message, const char *filename, int line, const char *function)
{
    ReadWriteMutex::ScopedReadLock lock(&m_spinLock);

    for (AbstractLogProviderPtrList::iterator iterator = m_providers.begin(); iterator != m_providers.end(); ++iterator)
        (*iterator)->Warning(message, filename, line, function);
}

void LogSystem::Error(const char *message, const char *filename, int line, const char *function)
{
    ReadWriteMutex::ScopedReadLock lock(&m_spinLock);

    for (AbstractLogProviderPtrList::iterator iterator = m_providers.begin(); iterator != m_providers.end(); ++iterator)
        (*iterator)->Error(message, filename, line, function);
}

void LogSystem::Pedantic(const char *message, const char *filename, int line, const char *function)
{
    ReadWriteMutex::ScopedReadLock lock(&m_spinLock);

    for (AbstractLogProviderPtrList::iterator iterator = m_providers.begin(); iterator != m_providers.end(); ++iterator)
        (*iterator)->Pedantic(message, filename, line, function);
}

}
} // namespace DPL
