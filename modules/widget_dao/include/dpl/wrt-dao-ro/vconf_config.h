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
 * @file    vconf_config.h
 * @author  Jihoon Chung (jihoon.chung@samsung.com)
 * @version 1.0
 * @brief   Implementation file for vconf key config.
 */
#ifndef SRC_DOMAIN_VCONF_CONFIG_H
#define SRC_DOMAIN_VCONF_CONFIG_H

#include <string>
#include <dpl/string.h>

#include <dpl/wrt-dao-ro/global_config.h>
#include <dpl/wrt-dao-ro/path_builder.h>

namespace WrtDB {
namespace VconfConfig {
inline std::string GetVconfKeyRootPath(DPL::String pkgName)
{
    return PathBuilder()
               .Append(GlobalConfig::GetVconfKeyPrefixPath())
               .Append(DPL::ToUTF8String(pkgName))
               .GetFullPath();
}

inline std::string GetVconfKeyPopupUsage(DPL::String pkgName)
{
    return PathBuilder()
               .Append(GlobalConfig::GetVconfKeyPrefixPath())
               .Append(DPL::ToUTF8String(pkgName))
               .Concat(GlobalConfig::GetVconfKeyPopupUsagePath())
               .GetFullPath();
}

inline std::string GetVconfKeyGeolocationUsage(DPL::String pkgName)
{
    return PathBuilder()
               .Append(GlobalConfig::GetVconfKeyPrefixPath())
               .Append(DPL::ToUTF8String(pkgName))
               .Concat(GlobalConfig::GetVconfKeyGeolocationUsagePath())
               .GetFullPath();
}

inline std::string GetVconfKeyWebNotificationUsage(DPL::String pkgName)
{
    return PathBuilder()
               .Append(GlobalConfig::GetVconfKeyPrefixPath())
               .Append(DPL::ToUTF8String(pkgName))
               .Concat(GlobalConfig::GetVconfKeyWebNotificationUsagePath())
               .GetFullPath();
}

inline std::string GetVconfKeyWebDatabaseUsage(DPL::String pkgName)
{
    return PathBuilder()
               .Append(GlobalConfig::GetVconfKeyPrefixPath())
               .Append(DPL::ToUTF8String(pkgName))
               .Concat(GlobalConfig::GetVconfKeyWebDatabaseUsagePath())
               .GetFullPath();
}

inline std::string GetVconfKeyFilesystemUsage(DPL::String pkgName)
{
    return PathBuilder()
               .Append(GlobalConfig::GetVconfKeyPrefixPath())
               .Append(DPL::ToUTF8String(pkgName))
               .Concat(GlobalConfig::GetVconfKeyFilesystemUsagePath())
               .GetFullPath();
}

inline std::string GetVconfKeyMemorySavingMode(DPL::String pkgName)
{
    return PathBuilder()
               .Append(GlobalConfig::GetVconfKeyPrefixPath())
               .Append(DPL::ToUTF8String(pkgName))
               .Concat(GlobalConfig::GetVconfKeyMemorySavingModePath())
               .GetFullPath();
}

} // namespace VconfConfig
} // namespace WrtDB

#endif
