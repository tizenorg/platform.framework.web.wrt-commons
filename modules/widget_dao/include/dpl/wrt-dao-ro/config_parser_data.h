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
void NormalizeString(DPL::OptionalString& txt, bool isTrimSpace = false);
void NormalizeString(DPL::String& str);
DPL::String GetSingleAttributeValue(const DPL::String value);
void NormalizeAndTrimSpaceString(DPL::OptionalString& txt);

class WidgetConfigurationManager;

class ConfigParserData
{
  public:
    struct Feature
    {
        Feature(const DPL::String& _name) : name(_name)
        {}
        DPL::String name;

        bool operator==(const Feature&) const;
        bool operator!=(const Feature&) const;
        bool operator >(const Feature&) const;
        bool operator>=(const Feature&) const;
        bool operator <(const Feature&) const;
        bool operator<=(const Feature&) const;
    };
    typedef std::set<Feature> FeaturesList;

    struct Privilege
    {
        Privilege(const DPL::String& _name) : name(_name)
        {}
        DPL::String name;

        bool operator==(const Privilege&) const;
        bool operator!=(const Privilege&) const;
        bool operator >(const Privilege&) const;
        bool operator>=(const Privilege&) const;
        bool operator <(const Privilege&) const;
        bool operator<=(const Privilege&) const;
    };
    typedef std::set<Privilege> PrivilegeList;

    struct Icon
    {
        Icon(const DPL::String& _src) : src(_src)
        {}
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
        Preference(const DPL::String& _name,
                   bool _readonly = false) :
            name(_name),
            value(),
            readonly(_readonly)
        {}
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
        {}

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
        {}
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

    struct AppControlInfo
    {
        enum class Disposition {
            WINDOW = 0,
            INLINE
        };
        AppControlInfo(const DPL::String& operation) :
            m_operation(operation),
            m_index(0)
        {}
        DPL::String m_src;
        DPL::String m_operation;
        std::set <DPL::String> m_uriList;
        std::set <DPL::String> m_mimeList;
        Disposition m_disposition;
        unsigned m_index;

        bool operator==(const AppControlInfo&) const;
        bool operator!=(const AppControlInfo&) const;
    };

    typedef std::list<AppControlInfo> AppControlInfoList;

    typedef std::list<std::pair<DPL::String, DPL::String> > BoxSizeList;

    struct LiveboxInfo
    {
        LiveboxInfo() { }

        struct BoxContent
        {
            DPL::String m_boxSrc;
            DPL::String m_boxMouseEvent;
            DPL::String m_boxTouchEffect;
            BoxSizeList m_boxSize;
            DPL::String m_pdSrc;
            DPL::String m_pdWidth;
            DPL::String m_pdHeight;
            DPL::String m_pdFastOpen;
        };
        typedef BoxContent BoxContentInfo;

        DPL::String m_label;
        DPL::String m_icon;
        DPL::String m_liveboxId;
        DPL::String m_primary;
        DPL::String m_type;
        DPL::String m_autoLaunch;
        DPL::String m_updatePeriod;
        BoxContentInfo m_boxInfo;

        bool operator==(const LiveboxInfo&) const;
        bool operator!=(const LiveboxInfo&) const;
        bool operator >(const LiveboxInfo&) const;
        bool operator>=(const LiveboxInfo&) const;
        bool operator <(const LiveboxInfo&) const;
        bool operator<=(const LiveboxInfo&) const;
    };
    typedef std::list<DPL::Optional<LiveboxInfo> > LiveboxList;

    enum IconSectionType
    {
        DefaultIcon =0,
        SmallIcon
    };

    typedef std::set<std::pair<IconSectionType, DPL::String>> IconSet;
    typedef std::list<DPL::String> CapabilityList;
    typedef std::set<std::pair<DPL::String, DPL::String>> DisplayNameSet;

    struct AccountProvider
    {
        AccountProvider() :
            m_multiAccountSupport(false)
        { }

        bool m_multiAccountSupport;
        IconSet m_iconSet;
        DisplayNameSet m_displayNameSet;
        CapabilityList m_capabilityList;
    };

    typedef std::list<DPL::OptionalString> DependsPkgList;
    typedef std::set<DPL::String> CategoryList;

    struct AllowNavigationInfo
    {
        AllowNavigationInfo(DPL::String scheme,
                            DPL::String host) :
            m_scheme(scheme),
            m_host(host)
        { }
        DPL::String m_scheme;
        DPL::String m_host;
    };
    typedef std::list<AllowNavigationInfo> AllowNavigationInfoList;

    struct Metadata
    {
        Metadata(const DPL::String& _key,
                 const DPL::String& _value) :
            key(_key),
            value(_value)
        {}
        DPL::String key;
        DPL::String value;

        bool operator==(const Metadata&) const;
        bool operator!=(const Metadata&) const;
    };
    typedef std::list<Metadata> MetadataList;

    enum class SecurityModelVersion {
        SECURITY_MODEL_V1 = 0, // WARP
        SECURITY_MODEL_V2      // CSP, allow-navigation
    };

    LiveboxList m_livebox;
    StringsList nameSpaces;

    LocalizedDataSet localizedDataSet;

    DPL::OptionalString authorName;
    DPL::OptionalString authorHref;
    DPL::OptionalString authorEmail;

    FeaturesList featuresList;
    PrivilegeList privilegeList;

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

    bool backSupported;
    bool accessNetwork;

    // Unlocalized data, to be processed by WidgetConfigurationManager
    bool startFileEncountered;
    DPL::OptionalString startFile;
    DPL::OptionalString startFileEncoding;
    DPL::OptionalString startFileContentType;
    IconsList iconsList;

    // tizen id / required platform min version for TIZEN webapp
    DPL::OptionalString tizenMinVersionRequired;
    DPL::OptionalString tizenPkgId;
    DPL::OptionalString tizenAppId;

    // allow-navigation
    AllowNavigationInfoList allowNavigationInfoList;

    //csp polic for widget
    DPL::OptionalString cspPolicy;
    DPL::OptionalString cspPolicyReportOnly;

    //AppControl model list
    AppControlInfoList appControlList;

    // For link shared directory
    DependsPkgList dependsPkgList;
    // Splash image path
    DPL::OptionalString splashImgSrc;
    // Background page filename
    DPL::OptionalString backgroundPage;
    // For category
    CategoryList categoryList;
    // For Account
    AccountProvider accountProvider;
<<<<<<< HEAD
    // security model version
    SecurityModelVersion securityModelVersion;
    // security model version
    MetadataList metadataList;
=======
>>>>>>> cbb1e3b... Add Account parser.

    ConfigParserData() :
        flashNeeded(false),
        minVersionRequired(),
        backSupported(false),
        accessNetwork(false),
        startFileEncountered(false),
        securityModelVersion(SecurityModelVersion::SECURITY_MODEL_V1)
    {}
};
} // namespace WrtDB

#endif  //CONFIG_PARSER_DATA_H_
