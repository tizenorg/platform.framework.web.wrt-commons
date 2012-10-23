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
#include <sstream>
#include <sys/utsname.h>
#include <dpl/utils/wrt_global_settings.h>

namespace GlobalSettings {

namespace {
const int ROAMING_TEST = 0x00000001;
const int POPUPS_TEST =  0x00000002;
const int OCSP_TEST = 0x00000004;
const int WARP_TEST = 0x00000008;
const int CRL_TEST = 0x00000010;
const int SCREEN_SHOT_TEST = 0x00000020;
const int ALL_TEST = (ROAMING_TEST | POPUPS_TEST | OCSP_TEST | WARP_TEST
                      | CRL_TEST | SCREEN_SHOT_TEST);
const char* WRT_TEST_MODE = "WRT_TEST_MODE";
const char* MACHINE_NAME_EMUL = "emulated"; // "arch_emulated"
enum MachineType
{
    MACHINE_TYPE_TARGET,
    MACHINE_TYPE_EMULATOR,
    MACHINE_TYPE_UNKNOWN
};

struct Settings {
    int testModes;
    bool isEmulator;

    Settings()
    : isEmulator(false), testModes(0)
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
    char * envStr = getenv(WRT_TEST_MODE);
    int testMode = 0;
    if (NULL != envStr) {
        std::string env = envStr;
        if ("1" == env) {
            testMode = ALL_TEST;
        } else {
            std::istringstream str(envStr);
            while (std::getline(str, env, '|')) {
                if ("popups" == env) {
                        testMode |= POPUPS_TEST;
                } else if ("roaming" == env) {
                        testMode |= ROAMING_TEST;;
                } else if ("ocsp" == env) {
                        testMode |= OCSP_TEST;;
                } else if ("warp" == env) {
                        testMode |= WARP_TEST;;
                } else if ("crl" == env) {
                        testMode |= CRL_TEST;
                } else if ("screen" == env) {
                        testMode |= SCREEN_SHOT_TEST;;
                }
            }
        }
        gSettings.testModes = testMode;
    }
    // TODO other settings initialization

#endif
    gSettings.isEmulator = (MACHINE_TYPE_EMULATOR == getMachineType());
    return false;
}
} // namespace

bool TestModeEnabled()
{
    return ((gSettings.testModes & ALL_TEST) == ALL_TEST);
}

bool PopupsTestModeEnabled() {
    return (gSettings.testModes & POPUPS_TEST);
}
bool WarpTestModeEnabled() {
    return (gSettings.testModes & WARP_TEST);
}
bool RoamingTestModeEnabled() {
    return (gSettings.testModes & ROAMING_TEST);
}
bool OCSPTestModeEnabled() {
    return (gSettings.testModes & OCSP_TEST);
}
bool CrlTestModeEnabled() {
    return (gSettings.testModes & CRL_TEST);
}
bool MakeScreenTestModeEnabled() {
    return (gSettings.testModes & SCREEN_SHOT_TEST);
}

bool IsEmulator()
{
    return gSettings.isEmulator;
}

} // GlobalSettings
