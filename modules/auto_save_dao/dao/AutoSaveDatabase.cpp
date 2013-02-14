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
#include <stddef.h>
#include <wrt-commons/auto-save-dao/AutoSaveDatabase.h>

using namespace AutoSaveDB;

namespace {
const char* AUTOSAVE_DB_DATABASE = "/opt/dbspace/.wrt_autosave.db";
DPL::DB::SqlConnection::Flag::Type AUTOSAVE_DB_FLAGS =
    DPL::DB::SqlConnection::Flag::UseLucene;
}

DPL::Mutex g_autoSaveDbQueriesMutex;
DPL::DB::ThreadDatabaseSupport Interface::m_autoSavedbInterface(
    AUTOSAVE_DB_DATABASE, AUTOSAVE_DB_FLAGS);

