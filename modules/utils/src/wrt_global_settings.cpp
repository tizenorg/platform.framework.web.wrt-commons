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
#include <dpl/assert.h>
#include <dpl/log/log.h>
#include <string.h>
#include <stdlib.h>
#include <wrt_global_settings_internal.h>

namespace {
bool GetPopupsEnabledFlag()
{
    //TODO : env var. will be removed after UX guide for POWDER is enabled.
    const char *env = getenv("WRT_POPUP_ENABLE");
    if (env && 0 == strcmp(env, "1")) {
        return true;
    } else {
        return false;
    }
}

static bool initializeGlobalSettings();

static bool initHelper = initializeGlobalSettings();

bool initializeGlobalSettings()
{
    (void)initHelper;
    LogDebug("Initializing globall settings");
    GlobalSettings::IGlobalSettingsFunctions functions;
    functions.getPopupsEnabledFlag = &GetPopupsEnabledFlag;
    GlobalSettings::SetPredefinedGlobalSettings(functions);
    return false;
}
}
