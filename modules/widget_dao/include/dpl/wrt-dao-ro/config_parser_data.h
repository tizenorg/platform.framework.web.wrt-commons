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
 * @file        config_parser_data.h
 * @author      Lukasz Wrzosek (l.wrzosek@samsung.com)
 * @version     0.1
 * @brief
 */
#ifndef CONFIG_PARSER_DATA_H_
#define CONFIG_PARSER_DATA_H_

#include <string>
#include <set>
#include <list>
#include <map>
#include <dpl/optional_typedefs.h>
#include <dpl/string.h>
#include <ctype.h>

namespace WrtDB {

void NormalizeString(DPL::OptionalString& txt);
void NormalizeString(DPL::String& str);
DPL::String GetSingleAttributeValue(const DPL::String value);

class WidgetConfigurationManager;

class ConfigParserData
{
  public:
    struct Param
    {
        Param(const DPL::String& name) : name(name)
        {
        }
        DPL::String name;
        DPL::String value;
        bool operator==(const Param&) const;
        bool operator!=(const Param&) const;
        bool operator >(const Param&) const;
        bool operator>=(const Param&) const;
        bool operator <(const Param&) const;
        bool operator<=(const Param&) const;
    };
    typedef std::set<Param> ParamsList;

    struct Feature
    {
        Feature(const DPL::String& name,
                bool required = true) : name(name),
            required(required)
        {
        }
        DPL::String name;
        bool required;
        ParamsList paramsList;

        bool operator==(const Feature&) const;
        bool operator!=(const Feature&) const;
        bool operator >(const Feature&) const;
        bool operator>=(const Feature&) const;
        bool operator <(const Feature&) const;
        bool operator<=(const Feature&) const;
    };
    typedef std::set<Feature> FeaturesList;

    struct Icon
    {
        Icon(const DPL::String& src) : src(src)
        {
        }
        DPL::String src;
        DPL::OptionalInt width;
        DPL::OptionalInt height;
        bool operator==(const Icon&) const;
        bool operator!=(const Icon&) const;
        bool operator >(const Icon&) const;
        bool operator>=(const Icon&) const;
        bool operator <(const Icon&) const;
        bool operator<=(const Icon&) const;
    };
    typedef std::list<Icon> IconsList;

    struct LocalizedData
    {
        DPL::OptionalString name;
        DPL::OptionalString shortName;

        DPL::OptionalString description;

        DPL::OptionalString license;
        DPL::OptionalString licenseFile;
        DPL::OptionalString licenseHref;
    };
    typedef std::map<DPL::String, LocalizedData> LocalizedDataSet;

    struct Preference
    {
        Preference(const DPL::String& name,
                bool readonly = false) :
            name(name),
            value(),
            readonly(readonly)
        {
        }
        DPL::String name;
        DPL::OptionalString value;
        bool readonly;
        bool operator==(const Preference&) const;
        bool operator!=(const Preference&) const;
        bool operator >(const Preference&) const;
        bool operator>=(const Preference&) const;
        bool operator <(const Preference&) const;
        bool operator<=(const Preference&) const;
    };
    typedef std::set<Preference> PreferencesList;
    typedef std::set<DPL::String> StringsList;

    struct AccessInfo
    {
        AccessInfo(const DPL::String& strIRI,
                bool bSubdomainAccess) : m_strIRI(strIRI),
            m_bSubDomainAccess(bSubdomainAccess)
        {
        }

        bool operator==(const AccessInfo&) const;
        bool operator!=(const AccessInfo&) const;
        bool operator <(const AccessInfo&) const;

        DPL::String m_strIRI;
        bool m_bSubDomainAccess;
    };
    typedef std::set<AccessInfo> AccessInfoSet;

    struct Setting
    {
        Setting(const DPL::String& name,
                const DPL::String& value) :
                m_name(name),
                m_value(value)
        {
        }
        DPL::String m_name;
        DPL::String m_value;

        bool operator==(const Setting&) const;
        bool operator!=(const Setting&) const;
        bool operator >(const Setting&) const;
        bool operator>=(const Setting&) const;
        bool operator <(const Setting&) const;
        bool operator<=(const Setting&) const;
    };

    typedef std::set<Setting> SettingsList;

    struct ServiceInfo
    {
        ServiceInfo(
            const DPL::String& src,
            const DPL::String& operation,
            const DPL::String& scheme,
            const DPL::String& mime) :
            m_src(src),
            m_operation(operation),
            m_scheme(scheme),
            m_mime(mime)
        {
        }
        DPL::String m_src;
        DPL::String m_operation;
        DPL::String m_scheme;
        DPL::String m_mime;

        bool operator==(const ServiceInfo&) const;
        bool operator!=(const ServiceInfo&) const;
    };
    typedef std::list<ServiceInfo> ServiceInfoList;
    typedef std::list<DPL::OptionalString> DependsPkgList;

    StringsList nameSpaces;

    LocalizedDataSet localizedDataSet;

    DPL::OptionalString authorName;
    DPL::OptionalString authorHref;
    DPL::OptionalString authorEmail;

    FeaturesList featuresList;

    SettingsList settingsList;

    DPL::OptionalInt width;
    DPL::OptionalInt height;

    DPL::OptionalString widget_id;
    DPL::OptionalString defaultlocale;

    PreferencesList preferencesList;

    DPL::OptionalString version;
    StringsList windowModes;

    AccessInfoSet accessInfoSet;

    bool flashNeeded;

    DPL::OptionalString minVersionRequired;
    StringsList powderDescriptionLinks;

    bool backSupported;
    bool accessNetwork;

    // Unlocalized data, to be processed by WidgetConfigurationManager
    bool startFileEncountered;
    DPL::OptionalString startFile;
    DPL::OptionalString startFileEncoding;
    DPL::OptionalString startFileContentType;
    IconsList iconsList;

    // tizen id / required platform min version for TIZEN webapp
    DPL::OptionalString tizenId;
    DPL::OptionalString tizenMinVersionRequired;

    //Application service model list
    ServiceInfoList appServiceList;
    // For link shared directory
    DependsPkgList dependsPkgList;
    // Splash image path
    DPL::OptionalString splashImgSrc;

    ConfigParserData() :
        flashNeeded(false),
        minVersionRequired(),
        backSupported(false),
        accessNetwork(false),
        startFileEncountered(false)
    {
    }
};

} // namespace WrtDB

#endif  //CONFIG_PARSER_DATA_H_
