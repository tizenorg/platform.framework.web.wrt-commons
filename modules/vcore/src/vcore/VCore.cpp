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
 * @file        VCore.cpp
 * @author      Bartlomiej Grzelewski (b.grzelewski@samsung.com)
 * @brief
 */

#include <vcore/VCorePrivate.h>
#include <vcore/Config.h>
#include <vcore/Database.h>
#include <openssl/ssl.h>
#include <database_checksum_vcore.h>
#include <glib.h>
#include <glib-object.h>

namespace {
DPL::DB::ThreadDatabaseSupport threadInterface(
    VCoreDatabaseConnectionTraits::Address(),
    VCoreDatabaseConnectionTraits::Flags());
} // namespace anonymous

namespace ValidationCore {

void AttachToThread(void){
    threadInterface.AttachToThread();
}

void DetachFromThread(void){
    threadInterface.DetachFromThread();
}

DPL::DB::ThreadDatabaseSupport& ThreadInterface(void) {
    return threadInterface;
}

bool VCoreInit(const std::string& configFilePath,
               const std::string& configSchemaPath,
               const std::string& databasePath)
{
    SSL_library_init();
    g_thread_init(NULL);
    g_type_init();

    LogDebug("Initializing VCore");
    Config &globalConfig = ConfigSingleton::Instance();
    bool returnValue = globalConfig.setXMLConfigPath(configFilePath) &&
        globalConfig.setXMLSchemaPath(configSchemaPath) &&
        globalConfig.setDatabasePath(databasePath);

    Assert(ThreadInterface().CheckTableExist(DB_CHECKSUM_STR) &&
           "Not a valid vcore database version");

    return returnValue;
}

} // namespace ValidationCore

