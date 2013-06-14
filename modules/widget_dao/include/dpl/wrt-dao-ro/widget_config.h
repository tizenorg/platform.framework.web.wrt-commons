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
 * @file    widget_config.h
 * @author  Zbigniew Kostrzewa (z.kostrzewa@samsung.com)
 * @version 1.0
 * @brief   Implementation file for widget config.
 */
#ifndef SRC_DOMAIN_WIDGET_CONFIG_H
#define SRC_DOMAIN_WIDGET_CONFIG_H

#include <string>
#include <dpl/string.h>

#include <dpl/wrt-dao-ro/global_config.h>
#include <dpl/wrt-dao-ro/path_builder.h>
#include <dpl/wrt-dao-ro/common_dao_types.h>

namespace WrtDB {
namespace WidgetConfig {
inline std::string GetWidgetBasePath(DPL::String tzPkgId)
{
    return PathBuilder()
               .Append(GlobalConfig::GetWidgetUserDataPath())
               .Append(DPL::ToUTF8String(tzPkgId))
               .GetFullPath();
}

inline std::string GetWidgetWebLocalStoragePath(DPL::String tzPkgId)
{
    return PathBuilder(GetWidgetBasePath(tzPkgId))
               .Append(GlobalConfig::GetWidgetLocalStoragePath())
               .GetFullPath();
}

inline std::string GetWidgetPersistentStoragePath(DPL::String tzPkgId)
{
    return PathBuilder(GetWidgetBasePath(tzPkgId))
               .Append(GlobalConfig::GetWidgetPrivateStoragePath())
               .GetFullPath();
}

inline std::string GetWidgetTemporaryStoragePath(DPL::String tzPkgId)
{
    return PathBuilder(GetWidgetBasePath(tzPkgId))
               .Append(GlobalConfig::GetWidgetPrivateTempStoragePath())
               .GetFullPath();
}

inline std::string GetWidgetDesktopFilePath(DPL::String tzPkgId)
{
    return PathBuilder()
               .Append(GlobalConfig::GetUserWidgetDesktopPath())
               .Append(DPL::ToUTF8String(tzPkgId))
               .Concat(".desktop")
               .GetFullPath();
}

inline std::string GetWidgetSharedStoragePath(DPL::String tzPkgId)
{
    return PathBuilder()
               .Append(GlobalConfig::GetWidgetUserDataPath())
               .Append(DPL::ToUTF8String(tzPkgId))
               .Concat(GlobalConfig::GetWidgetSharedPath())
               .GetFullPath();
}

inline std::string GetWidgetSharedDataStoragePath(DPL::String tzPkgId)
{
    return PathBuilder(GetWidgetSharedStoragePath(tzPkgId))
               .Concat(GlobalConfig::GetWidgetDataPath())
               .GetFullPath();
}

inline std::string GetWidgetSharedTrustedStoragePath(DPL::String tzPkgId)
{
    return PathBuilder(GetWidgetSharedStoragePath(tzPkgId))
               .Concat(GlobalConfig::GetWidgetTrustedPath())
               .GetFullPath();
}

inline std::string GetWidgetSharedResStoragePath(DPL::String tzPkgId)
{
    return PathBuilder(GetWidgetSharedStoragePath(tzPkgId))
               .Concat(GlobalConfig::GetWidgetResPath())
               .GetFullPath();
}

inline std::string GetWidgetNPRuntimePluginsPath(const DPL::String& tzPkgId)
{
    return PathBuilder(GetWidgetBasePath(tzPkgId))
                .Concat(GlobalConfig::GetWidgetSrcPath())
                .Append(GlobalConfig::GetNPRuntimePluginsPath())
                .GetFullPath();
}
} // namespace WidgetConfig
} // namespace WrtDB

#endif
