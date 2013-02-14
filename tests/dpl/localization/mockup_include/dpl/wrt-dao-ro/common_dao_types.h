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

#ifndef WRT_SRC_CONFIGURATION_COMMON_DAO_TYPES_H_
#define WRT_SRC_CONFIGURATION_COMMON_DAO_TYPES_H_

#include <set>
#include <string>
#include <map>
#include <vector>
#include <list>
#include <dpl/optional_typedefs.h>
#include <memory>

namespace WrtDB {
namespace Powder {
typedef std::set<DPL::String> StringSet;
//! Widget description
struct Description
{
    //!Content level
    typedef enum
    {
        Level0 = 0,
        Level1,
        Level2,
        Level3,
        Level4,
        Level5,
        LevelUnknown
    } LevelEnum;
    struct LevelEntry
    {
        LevelEnum level; //!< content level

        typedef StringSet Context;

        //! POWDER context
        //! xa    This material appears in an artistic context
        //! xb    This material appears in an educational context
        //! xc    This material appears in a medical context
        //! xd    This material appears in a sports context
        //! xe    This material appears in a violent context
        Context context;
        explicit LevelEntry(LevelEnum level = LevelUnknown);
        //! Function checks if context is valid
        //! \param[in] level POWDER content level
        //! \param[in] context POWDER context
        bool isContextValid(LevelEnum level,
                            const DPL::OptionalString& context) const;
    };

    struct CategoryEntry
    {
        //! Levels entries for POWDER description
        typedef std::vector <LevelEntry> LevelsContainer;
        LevelsContainer levels;
        //! Function checks if context is valid
        //! \param[out] reason set if context invalid
        //! \param[in] level POWDER content level
        //! \param[in] context POWDER context
        bool isCategoryValid(LevelEntry& reason,
                             LevelEnum level,
                             const DPL::OptionalString& context) const;
    };

    //! POWDER Category -> Category entry map for Widget
    //!
    //! nu    Nudity
    //! se    Sex
    //! vi    Violence
    //! la    Potentially offensive language
    //! dr    Drug use
    //! ga    Gambling
    //! ha    Hate or harmful activities
    //! ug    Use of user-generated content
    typedef std::map<DPL::String, CategoryEntry> CategoryEntries;

    CategoryEntries categories;

    //! Age rating for widget
    //! If Null not set
    DPL::OptionalInt ageRating;
};
} // namespace Powder

namespace ChildProtection {
//! Blacklist with forbidden URLs
//! It should be stored in WidgetDAO
typedef std::vector<DPL::String> BlackList;

//! Widget Child protection record
//! Record should be stored in WingetDAO
struct Record
{
    //! Child protection enabled
    bool enabled;
    explicit Record(bool enabled) :
        enabled(enabled)
    {}
};

//! Powder processing
struct PowderRules
{
    //! Rule set by parent about forbidden category
    //! Powder category
    //! nu    Nudity
    //! se    Sex
    //! vi    Violence
    //! la    Potentially offensive language
    //! dr    Drug use
    //! ga    Gambling
    //! ha    Hate or harmful activities
    //! ug    Use of user-generated content
    //! Powder context
    //! xa    This material appears in an artistic conteaxt
    //! xb    This material appears in an educational context
    //! xc    This material appears in a medical context
    //! xd    This material appears in a sports context
    //! xe    This material appears in a violent context
    struct CategoryRule
    {
        DPL::String category;
        Powder::Description::LevelEnum level;
        DPL::OptionalString context;
        explicit CategoryRule(
            const DPL::String& category = DPL::String(),
            Powder::Description::LevelEnum level =
                Powder::Description::LevelUnknown,
            const DPL::OptionalString& context =
                DPL::OptionalString());
    };

    struct PowderResult
    {
        //! Reasoning outcome: part of POWDER description used to invalidate
        Powder::Description::LevelEntry invalidDescription;
        //! Reasoning outcome: rule set by parent not full filed by description
        CategoryRule invalidRule;

        //! Reasoning outcome: type of invalidity
        enum InvalidReason
        {
            InvalidRule, //!< One of rules was not fulfilled
            InvalidAge, //!< Age is invalid
            AgeRatingNotSet, //!< Age rating for widget is not set
            Valid //!< Description valid
        };
        InvalidReason reason;
        explicit PowderResult(
            InvalidReason reason = Valid,
            const Powder::Description::LevelEntry&
            invalidDescription =
                Powder::Description::LevelEntry(),
            const CategoryRule& invalidRule = CategoryRule());
    };

    typedef std::pair<bool, PowderResult> ResultPair;

    //! Function checks if rule is fulfilled by description
    //! \param[in] rule checked rule
    //! \param[in] description
    //! \retval true rule is valid
    //! \retval false rule is invalid
    ResultPair isRuleValidForDescription(const CategoryRule& rule,
                                         const Powder::Description& description)
    const;
    //! Function checks if age limit is fulfilled by description
    //! \param[in] description
    //! \retval true age is valid
    //! \retval false age is invalid
    ResultPair isAgeValidForDescription(
        const Powder::Description& description) const;

    //! It is the maximum age rating valid for child
    //! Uniform age is stored in WidgetDAO
    DPL::OptionalInt ageLimit;

    //! Set to true if age rating is required
    //! If ageLimit is not set value is ignored
    bool isAgeRatingRequired;

    //! Set of rules configured by parent
    //! Rules are stored in WidgetDAO and are uniform for all widgets
    typedef std::vector<CategoryRule> RulesContainer;
    RulesContainer rules;

    //! Function check if Widget description is valid for ChildProtection
    //! configuration
    //! \param description widget description
    //! \retval true widget is valid
    //! \retval false widget is invalid
    ResultPair isDescriptionValid(const Powder::Description& description)
    const;

    PowderRules() :
        isAgeRatingRequired(false)
    {}
};
} // namespace ChildProtection

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
    {}

    std::string m_libraryName;
    std::string m_featuresInstallURI;
    std::string m_featuresKeyCN;
    std::string m_featuresRootCN;
    std::string m_featuresRootFingerprint;

    FeatureContainer m_featureContainer;
};

class PluginObjectsDAO
{
  public:
    typedef std::set<std::string> Objects;
    typedef std::shared_ptr<Objects> ObjectsPtr;

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
    {}
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

typedef std::list<WidgetAccessInfo> WidgetAccessInfoList;

typedef std::list<DPL::String> WindowModeList;

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
    DbPluginHandle pluginId;            /// Plugin id that implement this
                                        // feature
    WidgetParamMap params;   /// Widget's params

    DbWidgetFeature() :
        required(false),
        pluginId(INVALID_PLUGIN_HANDLE)
    {}
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

/**
 * @brief Widget specific type
 *
 * Widget type describes belowed in WAC, TIZEN WebApp
 */
enum AppType
{
    APP_TYPE_UNKNOWN = 0, // unknown
    APP_TYPE_WAC20, // WAC 2.0
    APP_TYPE_TIZENWEBAPP, // slp webapp
};

class WidgetType
{
  public:
    WidgetType() :
        appType(APP_TYPE_UNKNOWN)
    {}
    WidgetType(const AppType type) :
        appType(type)
    {}
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
#endif /* WRT_SRC_CONFIGURATION_COMMON_DAO_TYPES_H_ */
