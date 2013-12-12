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
#include <wrt-commons/i18n-dao-ro/i18n_database.h>
#include <tzplatform_config.h>

namespace I18n {
namespace DB {
namespace Interface {
namespace {
const char* const I18N_DB_FILE_PATH = tzplatform_mkpath(TZ_USER_DB,".wrt_i18n.db");

DPL::DB::SqlConnection::Flag::Type I18N_DB_FLAGS =
    DPL::DB::SqlConnection::Flag::UseLucene;
}

DPL::Mutex g_dbQueriesMutex;
DPL::DB::ThreadDatabaseSupport g_dbInterface(I18N_DB_FILE_PATH,
                                             I18N_DB_FLAGS);

void attachDatabaseRO()
{
    g_dbInterface.AttachToThread(DPL::DB::SqlConnection::Flag::RO);
}

void detachDatabase()
{
    g_dbInterface.DetachFromThread();
}
} //namespace Interface
} //namespace DB
} //namespace I18n
