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
 * @file      wrt_global_settings_internal.cpp
 * @version   0.6
 * @author    Lukasz Wrzosek (l.wrzosek@samsung.com)
 */

#include <wrt_global_settings.h>
#include <wrt_global_settings_internal.h>
#include <dpl/assert.h>
#include <dpl/log/log.h>

namespace GlobalSettings {
static IGlobalSettingsFunctions globalSettingsFunctions;

// INTERNAL
void SetPredefinedGlobalSettings(IGlobalSettingsFunctions functions)
{
    globalSettingsFunctions = functions;
    LogDebug("Global settings are set");
}

// PUBLIC
bool GetPopupsEnabledFlag()
{
    Assert(globalSettingsFunctions.getPopupsEnabledFlag &&
           "Global settings are unset");
    return globalSettingsFunctions.getPopupsEnabledFlag();
}
} //namespace GlobalSettings
