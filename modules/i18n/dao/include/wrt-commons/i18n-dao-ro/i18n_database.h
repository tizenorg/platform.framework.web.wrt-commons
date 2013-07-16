/*
 * Copyright (c) 2012 Samsung Electronics Co., Ltd All Rights Reserved
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

#ifndef _I18N_DATABASE_H_
#define _I18N_DATABASE_H_

#include <dpl/thread.h>
#include <dpl/mutex.h>
#include <dpl/db/thread_database_support.h>

namespace I18n {
namespace DB {
namespace Interface {
void attachDatabaseRO();
void detachDatabase();

extern DPL::Mutex g_dbQueriesMutex;
extern DPL::DB::ThreadDatabaseSupport g_dbInterface;
} // namespace Interface
} // namespace DB
} // namespace I18n

#define I18N_DB_INTERNAL(tlsCommand, InternalType)                             \
    static DPL::ThreadLocalVariable<InternalType> *tlsCommand##Ptr = NULL;     \
    {                                                                          \
        DPL::Mutex::ScopedLock lock(                                           \
            &I18n::DB::Interface::g_dbQueriesMutex);                           \
        if (!tlsCommand##Ptr) {                                                \
            static DPL::ThreadLocalVariable<InternalType> tmp;                 \
            tlsCommand##Ptr = &tmp;                                            \
        }                                                                      \
    }                                                                          \
    DPL::ThreadLocalVariable<InternalType> &tlsCommand = *tlsCommand##Ptr;     \
    if (tlsCommand.IsNull())                                                   \
    {                                                                          \
        tlsCommand = InternalType(&I18n::DB::Interface::g_dbInterface);        \
    }

#define I18N_DB_SELECT(name, type) \
    I18N_DB_INTERNAL(name, type::Select)

#endif /* _I18N_DATABASE_H_ */

