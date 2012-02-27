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
 * @file    webruntime_database.cpp
 * @author  Przemyslaw Dobrowolski (p.dobrowolsk@samsung.com)
 * @version 1.0
 * @brief   This file contains the definition of webruntime database
 */
#include "Database.h"

#include <string>
#include <dpl/log/log.h>
#include <dpl/db/sql_connection.h>

#include "Config.h"

const char* VCoreDatabaseConnectionTraits::Address()
{
    /* This function should be fixed but some changes in
     * dpl are required.
     */
//  return ValidationCore::VCConfigSingleton::Instance().
//  getDatabasePath().c_str();

    LogWarning(
      "VCore will use hardcoded path for database: /opt/dbspace/.vcore.db");
    return "/opt/dbspace/.vcore.db";
}

DPL::Mutex g_vcoreDbQueriesMutex;
