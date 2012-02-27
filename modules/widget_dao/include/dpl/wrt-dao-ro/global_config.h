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

namespace WrtDB {
namespace GlobalConfig {
/**
 * WRT database path
 */
inline const char* GetWrtDatabaseFilePath()
{
    return "/opt/dbspace/.wrt.db";
}

/**
 * WRT origin widget interface database path
 */
inline const char* GetWrtWidgetInterfaceDatabaseFilePath()
{
    return "/usr/share/wrt-plugins-common/widget_interface_db.sql";
}

/**
 * WRT device plugin path
 */
inline const char* GetDevicePluginPath()
{
    return "/usr/lib/wrt-plugins";
}

/**
 * WRT factory widgets that are loaded by default
 */
inline const char* GetFactoryInstalledWidgetPath()
{
    return "/opt/apps/widget/system";
}

/**
 * WRT widgets that are downloaded and installed by user
 */
inline const char* GetUserInstalledWidgetPath()
{
    return "/opt/apps";
}

/**
 * WRT widgets that are downloaded and installed by user
 */
inline const char* GetWidgetSrcPath()
{
    return "res/src";
}

/**
 * Directory for WebKit local storage files
 */
inline const char* GetPublicVirtualRootPath()
{
    return "/opt/apps/widget/data/Public";
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
    return "/opt/apps/widget/tests";
}

/**
 * widgets exec path
 */
inline const char* GetUserWidgetExecPath()
{
    return "bin";
}

/**
 * widgets private data path
 */
inline const char* GetWidgetPrivateStoragePath()
{
    return "data";
}


/**
 * widgets desktop files path
 */
inline const char* GetUserWidgetDesktopPath()
{
    return "/opt/share/applications";
}

/**
 * wrt-client exec path
 */
inline const char* GetWrtClientExec()
{
    return "/usr/bin/wrt-client";
}

/**
 * widgets desktop icon path
 */
inline const char* GetUserWidgetDesktopIconPath()
{
    return "res/icons/default/small";
}

/**
 * widgets default icon file
 */
inline const char* GetUserWidgetDefaultIconFile()
{
    return "/usr/share/wrt-engine/wrt_widget_default_icon.png";
}

/**
 * WRT downloaded widgets
 */
// KW inline const char* GetDownloadedWidgetPath()
// KW {
// KW     return "/opt/apps/widget/test-widgets";
// KW }

inline const char* GetSignatureXmlSchema()
{
    //TODO please rename, this filename is not descriptive enough
    return "/usr/share/wrt-engine/schema.xsd";
}

inline const char* GetWAC20TestRootCAFilePath()
{
    return "/usr/share/wrt-engine/WAC2.0TestRootCA.cert";
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
 * WRT device plugins installation required
 * File which indicate that new plugins
 * are available and should be installed
 */
inline const char* GetPluginInstallInitializerName()
{
    return "/opt/apps/widget/plugin-installation-required";
}

/**
 * File with certificate fingerprints list.
 */

inline const char* GetFingerprintListFile()
{
    return "/usr/share/wrt-engine/fingerprint_list.xml";
}

inline const char* GetFingerprintListSchema()
{
    return "/usr/share/wrt-engine/fingerprint_list.xsd";
}

inline const char* GetVCoreDatabaseFilePath()
{
    return "/opt/dbspace/.vcore.db";
}
bool IsOCSPEnabled();
bool IsCRLEnabled();

/**
 * widgets cookie database file name
 */
inline const char* GetCookieDatabaseFile()
{
    return ".cookie.db";
}

/**
 * widget interface database file name
 */
inline const char* GetWidgetInterfaceDatabaseFile()
{
    return ".widget_interface.db";
}

inline const char* GetTmpDirPath()
{
    return "/tmp";
}
} // namespace GlobalConfig
} // namespace WrtDB

#endif // GLOBAL_CONFIG_H
