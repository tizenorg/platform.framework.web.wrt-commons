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
 * @file    webruntime_database.h
 * @author  Przemyslaw Dobrowolski (p.dobrowolsk@samsung.com)
 * @version 1.0
 * @brief   This file contains the declaration of webruntime database
 */
#ifndef WRT_ENGINE_SRC_CONFIGURATION_WEBRUNTIME_DATABASE_H
#define WRT_ENGINE_SRC_CONFIGURATION_WEBRUNTIME_DATABASE_H

#include <dpl/thread.h>
#include <dpl/mutex.h>

extern DPL::Mutex g_wrtDbQueriesMutex;

#define WRT_DB_INTERNAL(tlsCommand, InternalType, interface)                 \
    static DPL::ThreadLocalVariable<InternalType> *tlsCommand ## Ptr = NULL; \
    {                                                                        \
        DPL::Mutex::ScopedLock lock(&g_wrtDbQueriesMutex);                   \
        if (!tlsCommand ## Ptr) {                                            \
            static DPL::ThreadLocalVariable<InternalType> tmp;               \
            tlsCommand ## Ptr = &tmp;                                        \
        }                                                                    \
    }                                                                        \
    DPL::ThreadLocalVariable<InternalType> &tlsCommand = *tlsCommand ## Ptr; \
    if (tlsCommand.IsNull()) { tlsCommand = InternalType(interface); }

#define WRT_DB_SELECT(name, type, interface) WRT_DB_INTERNAL(name, type::Select, interface)

#define WRT_DB_INSERT(name, type, interface) WRT_DB_INTERNAL(name, type::Insert, interface)

#define WRT_DB_UPDATE(name, type, interface) WRT_DB_INTERNAL(name, type::Update, interface)

#define WRT_DB_DELETE(name, type, interface) WRT_DB_INTERNAL(name, type::Delete, interface)

#endif // WRT_ENGINE_SRC_CONFIGURATION_WEBRUNTIME_DATABASE_H
