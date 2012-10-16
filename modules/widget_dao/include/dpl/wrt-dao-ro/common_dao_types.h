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
 *
 * @file    common_dao_types.h
 * @author  Michal Ciepielski (m.ciepielski@samsung.com)
 * @version 1.0
 * @brief   This file contains the declaration of common data types for wrtdb
 */

#ifndef WRT_WIDGET_DAO_COMMON_DAO_TYPES_H_
#define WRT_WIDGET_DAO_COMMON_DAO_TYPES_H_

#include <set>
#include <string>
#include <map>
#include <vector>
#include <list>
#include <dpl/optional_typedefs.h>
#include <dpl/shared_ptr.h>
#include <memory>

namespace WrtDB {
class PluginMetafileData
{
  public:
    struct Feature
    {
        std::string m_name;
        std::set<std::string> m_deviceCapabilities;

        bool operator< (const Feature& obj) const
        {
            return m_name < obj.m_name;
        }
    };
    typedef std::set<Feature> FeatureContainer;

  public:

    PluginMetafileData()
    {
    }

    std::string m_libraryName;
    FeatureContainer m_featureContainer;
};

class PluginObjectsDAO
{
  public:
    typedef std::set<std::string> Objects;
    typedef DPL::SharedPtr<Objects> ObjectsPtr;

  public:
    explicit PluginObjectsDAO() {}

  protected:
    ObjectsPtr m_implemented;
    ObjectsPtr m_dependent;
};

/**
 * @brief Widget id describes web-runtime global widget identifier.
 *
 * Notice that only up to one widget can exist at the same time.
 * DbWidgetHandle can be translated into corresponding WidgetModel by invoking
 * FindWidgetModel routine.
 */
typedef int DbWidgetHandle;
typedef DPL::String WidgetPkgName;

/**
 * Value of invalid widget handle
 */
enum {
    INVALID_WIDGET_HANDLE = -1
};

/**
 * @brief Structure to hold the information of widget's size
 */
struct DbWidgetSize
{
    DPL::OptionalInt width;
    DPL::OptionalInt height;

    DbWidgetSize(DPL::OptionalInt w = DPL::OptionalInt::Null,
            DPL::OptionalInt h = DPL::OptionalInt::Null) :
        width(w),
        height(h)
    {
    }
};

inline bool operator ==(const DbWidgetSize &objA, const DbWidgetSize &objB)
{
    if (!objA.height || !objA.width || !objB.width || !objB.height) {
        return false;
    } else {
        return *objA.height == *objB.height && *objA.width == *objB.width;
    }
}

/**
 * Widget [G]lobal [U]nique [ID]entifier
 * Orginated from appstore ID
 */
typedef DPL::OptionalString WidgetGUID;

struct WidgetAccessInfo
{
    DPL::String strIRI;                /* origin iri */
    bool bSubDomains;                  /* do we want access to subdomains ? */

    bool operator ==(const WidgetAccessInfo& info) const
    {
        return info.strIRI == strIRI &&
               info.bSubDomains == bSubDomains;
    }
};

struct EncryptedFileInfo
{
    DPL::String fileName;
    int fileSize;

    bool operator==(const EncryptedFileInfo& info) const
    {
        return fileName == info.fileName;
    }

    bool operator==(const DPL::String& file) const
    {
        return fileName == file;
    }

    bool operator< (const EncryptedFileInfo& info) const
    {
        return fileName < info.fileName;
    }
};

typedef std::list<WidgetAccessInfo> WidgetAccessInfoList;

typedef std::list<DPL::String> WindowModeList;

typedef std::set<EncryptedFileInfo> EncryptedFileList;

/**
 * @brief Widget configuration parameter key
 */
typedef DPL::String WidgetParamKey;

/**
 * @brief Widget configuration parameter value
 */
typedef DPL::String WidgetParamValue;

/**
 * @brief A map of widget configuration parameters.
 *
 * Widget configuration parameters are read from database and are stored
 * along with feature that they describe.
 */
typedef std::multimap<WidgetParamKey, WidgetParamValue> WidgetParamMap;

/**
 * @brief Widget feature host information about possible javascript extensions
 *        that widget may use
 *
 * Widget features are declared in configuration file in widget installation
 * package. Each declared special feature is contained in some wrt-plugin that
 * declares to implement it. After widget launch wrt searches for proper plugin
 * libraries and load needed features.
 *
 * Widget features can be required or optional. It is possible to start widget
 * without missing feature. When required feature cannot be loaded widget will
 * not start.
 */

enum {
    INVALID_PLUGIN_HANDLE = -1
};
typedef int DbPluginHandle;

struct DbWidgetFeature
{
    DPL::String name;        /// Feature name
    bool required;           /// Whether feature is required
    bool rejected;           /// Api feature was rejected by ace
    DbPluginHandle pluginId; /// Plugin id that implement this feature
    WidgetParamMap params;   /// Widget's params

    DbWidgetFeature() :
        required(false),
        pluginId(INVALID_PLUGIN_HANDLE)
    {
    }
};

inline bool operator < (const DbWidgetFeature &objA,
        const DbWidgetFeature &objB)
{
    return objA.name.compare(objB.name) < 0;
}

inline bool operator==(const DbWidgetFeature &featureA,
        const DbWidgetFeature &featureB)
{
    return featureA.name == featureB.name &&
           featureA.required == featureB.required &&
           featureA.pluginId == featureB.pluginId;
}

/**
 * @brief Default container for features list
 */
typedef std::multiset<DbWidgetFeature> DbWidgetFeatureSet;

/**
 * @brief Default container with DbWidgetHandle's
 */
typedef std::list<DbWidgetHandle> DbWidgetHandleList;

class WidgetDAOReadOnly; //forward declaration
typedef std::shared_ptr<WidgetDAOReadOnly> WidgetDAOReadOnlyPtr;
/**
 * @brief Default container with WidgetDAOReadOnly
 */
typedef std::list<WidgetDAOReadOnlyPtr> DbWidgetDAOReadOnlyList;

/**
 * @brief Widget specific type
 *
 * Widget type describes belowed in WAC, TIZEN WebApp
 */
enum AppType
{
    APP_TYPE_UNKNOWN = 0, // unknown
    APP_TYPE_WAC20, // WAC 2.0
    APP_TYPE_TIZENWEBAPP, // Tizen webapp
};

class WidgetType
{
  public:
    WidgetType()
    :appType(APP_TYPE_UNKNOWN)
    {
    }
    WidgetType(const AppType type)
    :appType(type)
    {
    }
    bool operator== (const AppType& other) const
    {
        return appType == other;
    }
    std::string getApptypeToString()
    {
        switch (appType) {
#define X(x) case x: return #x;
        X(APP_TYPE_UNKNOWN)
        X(APP_TYPE_WAC20)
        X(APP_TYPE_TIZENWEBAPP)
#undef X
        default:
            return "UNKNOWN";
        }
    }

    AppType appType;
};

/**
 * @brief Package specific type
 *
 * Package type describes belowed in Tizen webapp, C++ service App
 */
enum PackagingType
{
    PKG_TYPE_UNKNOWN = 0, // unknown
    PKG_TYPE_TIZEN_WEBAPP, // Tizen webapp
    PKG_TYPE_TIZEN_WITHSVCAPP, // Tizen webapp with C++ service app
};

class PkgType
{
  public:
    PkgType()
    :pkgType(PKG_TYPE_UNKNOWN)
    {
    }
    PkgType(const PackagingType type)
    :pkgType(type)
    {
    }
    bool operator== (const PackagingType& other) const
    {
        return pkgType == other;
    }
    std::string getPkgtypeToString()
    {
        switch (pkgType) {
#define X(x) case x: return #x;
        X(PKG_TYPE_UNKNOWN)
        X(PKG_TYPE_TIZEN_WEBAPP)
        X(PKG_TYPE_TIZEN_WITHSVCAPP)
#undef X
        default:
            return "UNKNOWN";
        }
    }

    PackagingType pkgType;
};

} // namespace WrtDB

struct WidgetSetting
{
    DPL::String settingName;
    DPL::String settingValue;

    bool operator ==(const WidgetSetting& info) const
    {
        return (info.settingName == settingName &&
               info.settingValue == settingValue);
    }
    bool operator !=(const WidgetSetting& info) const
    {
        return (info.settingName != settingName ||
               info.settingValue != settingValue);
    }
};

typedef std::list<WidgetSetting> WidgetSettings;

/**
 * @brief Widget Application Service
 *
 * Application sercvice describes details of behaviour
 * when widget receives aul bundle data.
 */
struct WidgetApplicationService
{
  public:
    DPL::String src;       /* start uri */
    DPL::String operation; /* service name */
    DPL::String scheme;    /* scheme type*/
    DPL::String mime;      /* mime type */

    bool operator== (const WidgetApplicationService& other) const
    {
        return src == other.src &&
        operation == other.operation &&
        scheme == other.scheme &&
        mime == other.mime;
    }
};

typedef std::list<WidgetApplicationService> WidgetApplicationServiceList;

#endif /* WRT_WIDGET_DAO_COMMON_DAO_TYPES_H_ */
