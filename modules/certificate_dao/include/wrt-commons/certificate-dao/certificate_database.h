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

#ifndef _CERTIFICATE_DATABASE_H_
#define _CERTIFICATE_DATABASE_H_

#include <dpl/thread.h>
#include <dpl/mutex.h>

extern DPL::Mutex g_certificateDBQueriesMutex;

#define CERTIFICATE_DB_INTERNAL(tlsCommand, InternalType, interface)            \
    static DPL::ThreadLocalVariable<InternalType> *tlsCommand##Ptr = NULL; \
    {                                                                        \
        DPL::Mutex::ScopedLock lock(&g_certificateDBQueriesMutex);              \
        if (!tlsCommand##Ptr) {                                            \
            static DPL::ThreadLocalVariable<InternalType> tmp;               \
            tlsCommand##Ptr = &tmp;                                        \
        }                                                                    \
    }                                                                        \
    DPL::ThreadLocalVariable<InternalType> &tlsCommand = *tlsCommand##Ptr; \
    if (tlsCommand.IsNull()) { tlsCommand = InternalType(interface); }

#define CERTIFICATE_DB_SELECT(name, type, interface) \
    CERTIFICATE_DB_INTERNAL(name, type::Select, interface)

#define CERTIFICATE_DB_INSERT(name, type, interface) \
    CERTIFICATE_DB_INTERNAL(name, type::Insert, interface)

#define CERTIFICATE_DB_UPDATE(name, type, interface) \
    CERTIFICATE_DB_INTERNAL(name, type::Update, interface)

#define CERTIFICATE_DB_DELETE(name, type, interface) \
    CERTIFICATE_DB_INTERNAL(name, type::Delete, interface)

#endif // _CERTIFICATE_DATABASE_H_

