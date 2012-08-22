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
/**
 * @file        wrt_global_settings.cpp
 * @version     1.0
 * @author      Pawel Sikorski(p.sikorski@samsung.com)
 * @author      Lukasz Wrzosek (l.wrzosek@samsung.com)
 * @brief       runtime
 */
#include <cstdlib>
#include <cstring>
#include <string>
#include <sys/utsname.h>
#include <dpl/utils/wrt_global_settings.h>

namespace GlobalSettings {

namespace {
const char* MACHINE_NAME_EMUL = "emulated"; // "arch_emulated"
enum MachineType
{
    MACHINE_TYPE_TARGET,
    MACHINE_TYPE_EMULATOR,
    MACHINE_TYPE_UNKNOWN
};

struct Settings {
    bool testMode;
    bool isEmulator;

    Settings()
    : testMode(false),
    isEmulator(false)
    {}
};

Settings gSettings;

bool initializeGlobalSettings();
bool initHelper = initializeGlobalSettings();

MachineType getMachineType()
{
    // get current machine name
    struct utsname u;
    if (0 == uname(&u)) {
        if ((!u.machine) || (0 == strlen(u.machine))) {
            return MACHINE_TYPE_UNKNOWN;
        } else {
            // If current machine is emul,
            // machine name include "<arch>_emulated"
            std::string machine = u.machine;
            // find "emulated" string in the u.machine
            if (std::string::npos != machine.find(MACHINE_NAME_EMUL)) {
                return MACHINE_TYPE_EMULATOR;
            } else {
                return MACHINE_TYPE_TARGET;
            }
        }
    }

    return MACHINE_TYPE_UNKNOWN;
}

bool initializeGlobalSettings()
{
    (void)initHelper;

    // ignore environment variables if this flag is not set
#ifdef GLOBAL_SETTINGS_CONTROL
    const char *env = getenv("WRT_TEST_MODE");
    gSettings.testMode = (env != NULL && 0 == strncmp(env, "1", 1));
    // TODO other settings initialization

#endif
    gSettings.isEmulator = (MACHINE_TYPE_EMULATOR == getMachineType());
    return false;
}
} // namespace

bool TestModeEnabled()
{
    return gSettings.testMode;
}

bool IsEmulator()
{
    return gSettings.isEmulator;
}

} // GlobalSettings
