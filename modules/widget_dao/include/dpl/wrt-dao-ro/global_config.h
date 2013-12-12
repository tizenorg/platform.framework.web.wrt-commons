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
 * @file    global_config.h
 * @author  Yang Jie (jie2.yang@samsung.com)
 * @author  Przemyslaw Dobrowolski (p.dobrowolsk@samsung.com)
 * @version 1.0
 * @brief   This file contains global WRT config
 */
#ifndef GLOBAL_CONFIG_H
#define GLOBAL_CONFIG_H

#include <string>
#include <list>
#include <tzplatform_config.h>

namespace WrtDB {
namespace GlobalConfig {
/**
 * WRT database path
 */
inline const char* GetWrtDatabaseFilePath()
{
    return tzplatform_mkpath(TZ_SYS_DB,".wrt.db");
}

/**
 * WRT device plugin path
 */
inline const char* GetDevicePluginPath()
{
    return LIBDIR_PREFIX "/wrt-plugins";
}

/**
 * WRT widgets that are downloaded and installed by user
 */
inline const char* GetUserInstalledWidgetPath()
{
    return  tzplatform_getenv(TZ_USER_APP);
}

/**
 * WRT widgets that are preloaded
 */
inline const char* GetUserPreloadedWidgetPath()
{
    return tzplatform_getenv(TZ_SYS_RO_APP);
}

/**
 * WRT widgets that are downloaded and installed by user
 */
inline const char* GetWidgetUserDataPath()
{
    return tzplatform_getenv(TZ_USER_APP);
}

/**
 * WRT widgets that are downloaded and installed by user
 */
inline const char* GetWidgetSrcPath()
{
    return "/res/wgt";
}

/**
 * Directory for WebKit local storage files
 */
inline const char* GetPublicVirtualRootPath()
{
    return tzplatform_mkpath(TZ_SYS_RW_WIDGET,"data/Public");
}

/**
 * Directory for WebKit local storage files
 */
inline const char* GetWidgetLocalStoragePath()
{
    return "data/localStorage";
}

/**
 * Directory for tests data (such as test widgets wgt)
 */
inline const char* GetTestsDataPath()
{
    return  tzplatform_mkpath(TZ_SYS_RW_WIDGET,"tests");
}

/**
 * widgets exec path
 */
inline const char* GetUserWidgetExecPath()
{
    return "/bin";
}

/**
 * widgets private data path
 */
inline const char* GetWidgetPrivateStoragePath()
{
    return "data";
}

/**
 * widgets private temp data path
 */
inline const char* GetWidgetPrivateTempStoragePath()
{
    return "tmp";
}

/**
 * widgets desktop files path
 */
inline const char* GetUserWidgetDesktopPath()
{
    return tzplatform_getenv(TZ_SYS_RW_DESKTOP_APP);
}

/**
 * wrt-client exec path
 */
inline const char* GetWrtClientExec()
{
    return  tzplatform_mkpath(TZ_SYS_BIN,"wrt-client");
}

/**
 * widgets desktop icon path
 */
inline const char* GetUserWidgetDesktopIconPath()
{
    return tzplatform_mkpath(TZ_SYS_SHARE,"icons/default/small");
}

/**
 * widgets default icon file
 */
inline const char* GetUserWidgetDefaultIconFile()
{
    return tzplatform_mkpath(TZ_SYS_RO_WRT_ENGINE,"wrt_widget_default_icon.png");
}

inline const char* GetSignatureXmlSchema()
{
    //TODO please rename, this filename is not descriptive enough
    return  tzplatform_mkpath(TZ_SYS_RO_WRT_ENGINE,"schema.xsd");
}

/**
 * Name of the w3c geolocation feature
 */
inline const char* GetW3CGeolocationFeatureName()
{
    return "http://www.w3.org/TR/geolocation-API/";
}

/**
 * Prefix of package name for widgets
 */
inline const char* GetPkgnamePrefix()
{
    return "org.tizen.";
}

/**
 * Plugin Configuration Metafile name
 */
inline const char* GetPluginMetafileName()
{
    return "config.xml";
}

/**
 * Plugin .so prefix
 */
inline const char* GetPluginPrefix()
{
    return "libwrt-plugins-";
}

/**
 * Plugin .so suffix
 */
inline const char* GetPluginSuffix()
{
    return ".so";
}

/**
 * WRT device plugins installation required
 * File which indicate that new plugins
 * are available and should be installed
 */
inline const char* GetPluginInstallInitializerName()
{
    return tzplatform_mkpath(TZ_SYS_RW_WIDGET,"plugin-installation-required");
}

/**
 * File with certificate fingerprints list.
 */

inline const char* GetFingerprintListFile()
{
    return tzplatform_mkpath(TZ_SYS_RO_WRT_ENGINE,"fingerprint_list.xml");
}

inline const char* GetFingerprintListSchema()
{
    return tzplatform_mkpath(TZ_SYS_RO_WRT_ENGINE,"fingerprint_list.xsd");
}

inline const char* GetVCoreDatabaseFilePath()
{
    return tzplatform_mkpath(TZ_SYS_DB,".cert_svc_vcore.db");
}

/**
 * widgets cookie database file name
 */
inline const char* GetCookieDatabaseFile()
{
    return ".cookie.db";
}

inline const char* GetTmpDirPath()
{
    return "/tmp";
}

inline const char* GetTizenVersion()
{
    return "2.2";
}

inline const char* GetShareDirectoryPath()
{
    return tzplatform_getenv(TZ_SYS_SHARE);
}

inline const char* GetTempInstallInfoPath()
{
    return tzplatform_mkpath(TZ_SYS_RW_WIDGET,"temp_info");
}

inline const char* GetWidgetSharedPath()
{
    return "/shared";
}

inline const char* GetWidgetDataPath()
{
    return "/data";
}

inline const char* GetWidgetTrustedPath()
{
    return "/trusted";
}

inline const char* GetWidgetResPath()
{
    return "/res";
}

inline const char* GetNPRuntimePluginsPath()
{
#ifdef __arm__
    return "plugins/arm";
#else
    return "plugins/x86";
#endif
}

inline const char* GetBackupDatabaseSuffix()
{
    return ".backup";
}

inline const char* GetManifestPath()
{
    return tzplatform_getenv(TZ_SYS_RW_PACKAGES);
}

inline const char* GetPreloadManifestPath()
{
    return tzplatform_getenv(TZ_SYS_RO_PACKAGES);
}

inline const char* GetRecoveryStatusPath()
{
    return tzplatform_mkpath(TZ_SYS_RO_PACKAGES,".recovery/wgt");
}
} // namespace GlobalConfig
} // namespace WrtDB

#endif // GLOBAL_CONFIG_H
