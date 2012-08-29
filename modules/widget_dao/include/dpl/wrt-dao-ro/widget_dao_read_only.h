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
 * This file contains the declaration of widget dao class.
 *
 * @file    widget_dao_read_only.h
 * @author  Yang Jie (jie2.yang@samsung.com)
 * @author  Przemyslaw Dobrowolski (p.dobrowolsk@samsung.com)
 * @author  Pawel Sikorski (p.sikorski@samsung.com)
 * @version 1.0
 * @brief   This file contains the declaration of widget dao
 */

#ifndef _WRT_SRC_CONFIGURATION_WIDGET_DAO_READ_ONLY_H_
#define _WRT_SRC_CONFIGURATION_WIDGET_DAO_READ_ONLY_H_

#include <time.h>
#include <list>
#include <string>
#include <dpl/string.h>
#include <dpl/exception.h>
#include <dpl/db/orm.h>
#include <dpl/optional_typedefs.h>
#include <dpl/wrt-dao-ro/config_parser_data.h>
#include <dpl/wrt-dao-ro/property_dao_read_only.h>
#include <dpl/wrt-dao-ro/common_dao_types.h>

namespace WrtDB {

/**
 * Widget's signature enum.
 * This enumerates signature type of widget.
 */
enum WidgetSignatureType
{
    SIGNATURE_TYPE_CARRIER = 0,
    SIGNATURE_TYPE_IDENTIFIED,
    SIGNATURE_TYPE_UNIDENTIFIED
};

typedef std::list<DPL::String> StringList;

struct WidgetLocalizedInfo
{
    DPL::OptionalString name;
    DPL::OptionalString shortName;
    DPL::OptionalString description;
    DPL::OptionalString license;
    DPL::OptionalString licenseHref;
};

/**
 * CertificateData
 * A structure to hold certificate fingerprints.
 */
struct WidgetCertificateData
{
    enum Owner { AUTHOR, DISTRIBUTOR, UNKNOWN };
    enum Type { ROOT, ENDENTITY };

    // type of signature: author/distributor
    Owner owner;
    // indicates whether this is ca certificate
    Type type;

    // chain id number: relative BASE, where BASE is signatureBASE.xml
    int chainId;
    // certificate fingerprint digested by md5
    std::string strMD5Fingerprint;
    // certificate fingerprint digestef by sha1
    std::string strSHA1Fingerprint;
    // Common name field in certificate
    DPL::String strCommonName;

    bool operator== (const WidgetCertificateData& certData) const {
        return certData.chainId == chainId &&
           certData.owner == owner &&
           certData.strCommonName == strCommonName &&
           certData.strMD5Fingerprint == strMD5Fingerprint &&
           certData.strSHA1Fingerprint == strSHA1Fingerprint;
    }
};

typedef std::list<WidgetCertificateData> WidgetCertificateDataList;

typedef DPL::String Locale;
typedef std::set<Locale> LocaleSet;

/**
 * WidgetRegisterInfo
 * A structure to hold widget's information needed to be registered.
 * @see WidgetConfigurationInfo
 */
struct WidgetRegisterInfo
{
    struct LocalizedIcon : public ConfigParserData::Icon
    {
        LocalizedIcon(const ConfigParserData::Icon& icon,
                const LocaleSet& _availableLocales) :
            ConfigParserData::Icon(icon),
            availableLocales(_availableLocales)
        {
        }

        LocaleSet availableLocales;
    };

    struct StartFileProperties
    {
        DPL::String encoding;
        DPL::String type;
    };

    typedef std::map<Locale,
                     StartFileProperties> StartFilePropertiesForLocalesMap;
    struct LocalizedStartFile
    {
        DPL::String path;
        StartFilePropertiesForLocalesMap propertiesForLocales;
    };

    typedef std::list<LocalizedIcon> LocalizedIconList;
    typedef std::list<LocalizedStartFile> LocalizedStartFileList;
    struct LocalizationData
    {
        LocalizedIconList icons;
        LocalizedStartFileList startFiles;
    };

    enum SecurityDomain
    {
        Untrusted,
        Trusted,
        Operator,
        None
    };

    //Constructor
    WidgetRegisterInfo() :
        type(APP_TYPE_UNKNOWN),
        signatureType(SIGNATURE_TYPE_UNIDENTIFIED),
        isFactoryWidget(0),
        isTestWidget(0),
        configInfo(),
        pType(PKG_TYPE_UNKNOWN)
    {
    }

    WidgetType type;
    DPL::OptionalString guid;
    DPL::OptionalString version;
    DPL::OptionalString minVersion;
    std::string shareHref;
    std::string baseFolder;
    WidgetSignatureType signatureType;
    int isFactoryWidget;
    int isTestWidget;
    ConfigParserData configInfo;
    LocalizationData localizationData;
    DPL::OptionalString pkgname;
    time_t installedTime;
    PkgType pType;
    EncryptedFileList encryptedFiles;
};

typedef std::list<std::string> CertificateChainList;
class IWacSecurity
{
  public:
    virtual ~IWacSecurity();

    virtual const WidgetCertificateDataList& getCertificateList() const = 0;

    virtual bool isRecognized() const = 0;

    virtual bool isDistributorSigned() const = 0;

    virtual bool isWacSigned() const = 0;

    virtual void getCertificateChainList(CertificateChainList& list) const = 0;
};

/**
 * WidgetAuthorInfo.
 * Structure to hold the information of widget's author.
 */
struct WidgetAuthorInfo
{
    DPL::OptionalString name;
    DPL::OptionalString email;
    DPL::OptionalString href;
};

/**
 * Widget update policy
 */
enum WidgetUpdatePolicy
{
    WIDGET_UPDATE_MONTHLY = 0,  //< monthly update
    WIDGET_UPDATE_WEEKLY,       //< weekly update
    WIDGET_UPDATE_DAILY,        //< daily update
    WIDGET_UPDATE_STARTUP,      //< update when cell phone boots
    WIDGET_UPDATE_NEVER         //< never update
};

typedef std::list <std::string> WidgetCertificateCNList;
typedef std::list<DPL::String> LanguageTagList;
typedef std::list<std::string> HostList;
typedef std::list<std::string> FingerPrintList;
typedef std::list<std::string> ResourceAttributeList;

class WidgetDAOReadOnly
{
  public:
    /**
     * WidgetDAO Exception classes
     */
    class Exception
    {
      public:
        DECLARE_EXCEPTION_TYPE(DPL::Exception, Base)
        DECLARE_EXCEPTION_TYPE(Base, DatabaseError)
        DECLARE_EXCEPTION_TYPE(Base, ReadOnlyProperty)
        DECLARE_EXCEPTION_TYPE(Base, GUIDisNull)
        DECLARE_EXCEPTION_TYPE(Base, UnexpectedEmptyResult)
        DECLARE_EXCEPTION_TYPE(Base, WidgetNotExist)
        DECLARE_EXCEPTION_TYPE(Base, AlreadyRegistered)
    };

  protected:
    DbWidgetHandle m_widgetHandle;

  public:
    struct WidgetLocalizedIconRow
    {
        int appId;
        int iconId;
        DPL::String widgetLocale;
    };
    typedef std::list<WidgetLocalizedIconRow> WidgetLocalizedIconList;

    struct WidgetIconRow
    {
        int iconId;
        int appId;
        DPL::String iconSrc;
        DPL::OptionalInt iconWidth;
        DPL::OptionalInt iconHeight;
    };
    typedef std::list<WidgetIconRow> WidgetIconList;

    struct WidgetStartFileRow
    {
        int startFileId;
        int appId;
        DPL::String src;
    };
    typedef std::list<WidgetStartFileRow> WidgetStartFileList;

    struct WidgetLocalizedStartFileRow
    {
        int startFileId;
        int appId;
        DPL::String widgetLocale;
        DPL::String type;
        DPL::String encoding;
    };
    typedef std::list<WidgetLocalizedStartFileRow> LocalizedStartFileList;


    /**
     * This is a constructor.
     *
     * @param[in] widgetHandle application id of widget.
     */
    WidgetDAOReadOnly(DbWidgetHandle widgetHandle);
    WidgetDAOReadOnly(DPL::OptionalString widgetGUID);

    /**
     * Destructor
     */
    virtual ~WidgetDAOReadOnly();

    /**
     * This method returns widget handle(m_widgetHandle).
     *
     * @return widget handle(m_widgetHandle).
     * @exception WRT_CONF_ERR_EMDB_FAILURE - Fail to query DB table.
     * @exception WRT_CONF_ERR_EMDB_NO_RECORD - Can not find matching records in DB table.
     */
    DbWidgetHandle getHandle() const;
    static DbWidgetHandle getHandle(const WidgetGUID GUID);
    static DbWidgetHandle getHandle(const DPL::String pkgName);

    /**
     * This method returns the root directory of widget resource.
     *
     * @return path name of root directory.
     * @exception WRT_CONF_ERR_EMDB_FAILURE - Fail to query DB table.
     * @exception WRT_CONF_ERR_EMDB_NO_RECORD - Can not find matching records in
     * DB table.
     */
    DPL::String getPath() const;

    DPL::String getFullPath() const;

    /**
     * This method returns the preferred size of the widget,
     * including width and height.
     *
     * @see DbWidgetSize
     * @return An structure type variable to hold widget's size.
     * @exception WRT_CONF_ERR_EMDB_FAILURE - Fail to query DB table.
     * @exception WRT_CONF_ERR_EMDB_NO_RECORD - Can not find matching
     *                                          DB table.
     */
    DbWidgetSize getPreferredSize() const;

    /**
     * This method returns the type of the widget.
     *
     * @return WidgetType
     * @exception WRT_CONF_ERR_EMDB_FAILURE - Fail to query DB table.
     * @exception WRT_CONF_ERR_EMDB_NO_RECORD - Can not find matching
                                                records in DB table.
     */
    WidgetType getWidgetType() const;

    /**
     * This method returns the id of the widget.
     *
     * @return widget id
     * @exception WRT_CONF_ERR_EMDB_FAILURE - Fail to query DB table.
     * @exception WRT_CONF_ERR_EMDB_NO_RECORD - Can not find matching records in
     *  DB table.
     */
    WidgetGUID getGUID() const;

    /**
     * This method returns the Package name of the widget.
     *
     * @return pkgname
     * @exception WRT_CONF_ERR_EMDB_FAILURE - Fail to query DB table.
     * @exception WRT_CONF_ERR_EMDB_NO_RECORD - Can not find matching records in DB table.
     */
    DPL::OptionalString getPkgname() const;

    /**
     * This method returns the defaultlocale for the widget.
     *
     * @return defaultlocale
     * @exception WRT_CONF_ERR_EMDB_FAILURE - Fail to query DB table.
     * @exception WRT_CONF_ERR_EMDB_NO_RECORD - Can not find matching records in
     *  DB table.
     */
    DPL::OptionalString getDefaultlocale() const;

    /**
     * This method returns list of localized icons files;
     *
     * @exception WRT_CONF_ERR_EMDB_FAILURE - Fail to query DB table.
     * @exception WRT_CONF_ERR_EMDB_NO_RECORD - Can not find matching records in
     *  DB table.
     */
    WidgetLocalizedIconList getLocalizedIconList() const;

    /**
     * This method returns list of icons files;
     *
     * @exception WRT_CONF_ERR_EMDB_FAILURE - Fail to query DB table.
     * @exception WRT_CONF_ERR_EMDB_NO_RECORD - Can not find matching records in
     *  DB table.
     */
    WidgetIconList getIconList() const;

    /**
     * This method returns list of localized start files;
     *
     * @exception WRT_CONF_ERR_EMDB_FAILURE - Fail to query DB table.
     * @exception WRT_CONF_ERR_EMDB_NO_RECORD - Can not find matching records in
     *  DB table.
     */
    LocalizedStartFileList getLocalizedStartFileList() const;

    /**
     * This method returns list of start files;
     *
     * @exception WRT_CONF_ERR_EMDB_FAILURE - Fail to query DB table.
     * @exception WRT_CONF_ERR_EMDB_NO_RECORD - Can not find matching records in
     *  DB table.
     */
    WidgetStartFileList getStartFileList() const;

    /**
     * @param[out] outAccessInfoList list filled with access info structures
     */
    void getWidgetAccessInfo(WidgetAccessInfoList& outAccessInfoList) const;

    /**
     * WAC 2.0 extension
     * @return recognized status
     */
    bool isRecognized() const;

    /**
     * WAC 2.0 extension
     * @return
     */
    bool isWacSigned() const;

    /**
     * WAC 2.0 extension
     * @return
     */
    bool isDistributorSigned() const;

    /**
     * WAC 2.0 extension
     * @return trusted status
     */
    bool isTrusted() const;

    /**
     * WAC 2.0 extension
     * @return is WAC test widget
     */
    bool isTestWidget() const;

    /**
     * This method returns window mode of widget.
     *
     * @return window modes of widget
     * @exception WRT_CONF_ERR_EMDB_FAILURE - Fail to query DB table.
     * @exception WRT_CONF_ERR_EMDB_NO_RECORD - Can not find matching records in
     *  DB table.
     */
    WindowModeList getWindowModes() const;

    /**
     * This method returns the version of the widget.
     *
     * @return version of widget
     * @exception WRT_CONF_ERR_EMDB_FAILURE - Fail to query DB table.
     * @exception WRT_CONF_ERR_EMDB_NO_RECORD - Can not find matching records in
     *  DB table.
     */
    DPL::OptionalString getVersion() const;

    /**
     * This method returns list filed with Common Name entries from certificate.
     *
     * @return Common Name of Distribuotor End Entity certificate.
     * @exception WRT_CONF_ERR_EMDB_FAILURE - Fail to query DB table.
     * @exception WRT_CONF_ERR_EMDB_NO_RECORD - Can not find matching records in
     *  DB table.
     */
    WidgetCertificateCNList getKeyCommonNameList(
            WidgetCertificateData::Owner owner,
            WidgetCertificateData::Type type) const;

    /**
     * given a certificate owner (author / distributor) and type of certificate
     * (end entity / ca)
     * function returns list of matching fingerprints
     */
    FingerPrintList getKeyFingerprints(
            WidgetCertificateData::Owner owner,
            WidgetCertificateData::Type type) const;

    /*
     *  This method gets certificate data list for a widget from database.
     */
    WidgetCertificateDataList getCertificateDataList() const;

    /**
     * This method returns a list of widget features.
     *
     * @see WidgetFeature
     * @see FreeFeatureList()
     * @return list of widget features, type of list element is structure
     *  WidgetFeature
     * @exception WRT_CONF_ERR_EMDB_FAILURE - Fail to query DB table.
     * @exception WRT_CONF_ERR_EMDB_NO_RECORD - Can not find matching records in
     *  DB table.
     */
    DbWidgetFeatureSet getFeaturesList() const;

    static WidgetParamMap getFeatureParams(int widgetFeatureId);
    /**
     * This method checks whether widget has specified feature.
     *
     * @return true if has, false if has not
     * @exception WRT_CONF_ERR_EMDB_FAILURE - Fail to query DB table.
     */
    bool hasFeature(const std::string& featureName) const;

    /**
     * This method gets host list that widget can connect to.
     *
     * @return See above comment
     * @exception WRT_CONF_ERR_EMDB_FAILURE - Fail to query DB table.
     */
    HostList getAccessHostList() const;

    /**
     * This method gets widget's access on network: true or false.
     *
     * @return true: widget can access network; false: widget can not access
     *  network.
     * @exception WRT_CONF_ERR_EMDB_FAILURE - Fail to query DB table.
     * @exception WRT_CONF_ERR_EMDB_NO_RECORD - Can not find matching records in
     *  DB table.
     */
    bool getAccessNetworkMode() const;

    /**
     * This method gets if widget needs webkit plugins enabled
     *
     * @return true: widget needs webkit plugins enabled
     * @exception WRT_CONF_ERR_EMDB_FAILURE - Fail to query DB table.
     * @exception WRT_CONF_ERR_EMDB_NO_RECORD - Can not find matching records in
     *  DB table.
     */
    bool getWebkitPluginsRequired() const;

    /**
     * This method returns a list of all the installed widgets.
     *
     * @return list of installed widgets' app id.
     * @exception WRT_CONF_ERR_EMDB_FAILURE - Fail to query DB table.
     * @exception WRT_CONF_ERR_EMDB_NO_RECORD - Can not find matching records in
     *  DB table.
     */
    static DbWidgetHandleList getHandleList();

   /**
     * This method removes a widget's information from EmDB.
     *
     * @see RegisterWidget()
     * @param[in] widgetHandle    widget's app id
     * @return true if succeed, false if fail.
     */
    static void unregisterWidget(DbWidgetHandle widgetHandle);

    /**
     * This method gets author's infomation of a widget which is parsed from
     *  configiration document.
     *
     * @see WidgetAuthorInfo
     * @param[out] pAuthorInfo
     * @return true if succeed, false if fail.
     */
    WidgetAuthorInfo getAuthorInfo() const;

    /**
     * This method gets author's name of a widget which is parsed from
     *  configiration document.
     *
     * @param[out] pAuthorInfo
     * @return author's name.
     * @exception WRT_CONF_ERR_EMDB_FAILURE - Fail to query DB table.
     * @exception WRT_CONF_ERR_EMDB_NO_RECORD - Can not find matching records in
     *  DB table.
     */
    DPL::OptionalString getAuthorName() const;

    /**
     * This method gets author's email of a widget which is parsed from
     *  configiration document.
     *
     * @param[out] pAuthorInfo
     * @return author's email.
     * @exception WRT_CONF_ERR_EMDB_FAILURE - Fail to query DB table.
     * @exception WRT_CONF_ERR_EMDB_NO_RECORD - Can not find matching records in
     *  DB table.
     */
    DPL::OptionalString getAuthorEmail() const;

    /**
     * This method gets author's email of a widget which is parsed from
     *  configiration document.
     *
     * @param[out] pAuthorInfo
     * @return author's email.
     * @exception WRT_CONF_ERR_EMDB_FAILURE - Fail to query DB table.
     * @exception WRT_CONF_ERR_EMDB_NO_RECORD - Can not find matching records in
     *  DB table.
     */
    DPL::OptionalString getAuthorHref() const;

    /**
     * This method returns minimum version of WAC that WRT has to be compliant
     *  to to run this widget
     *
     * @return Minimum version
     * @exception WRT_CONF_ERR_EMDB_FAILURE - Fail to query DB table.
     * @exception WRT_CONF_ERR_EMDB_NO_RECORD - Can not find matching records in
     *  DB table.
     */
    DPL::OptionalString getMinimumWacVersion() const;

    /* This method get widget' share href.
     *
     * @return widget's share href
     */
    std::string getShareHref() const;

    /**
     * This method checks whether specified widget is a factory widget.
     *
     * @param[in] widgetHandle    widget's app id
     * @return true if yes, false if no.
     */
    bool isFactory() const;

    /**
     * This method get widget installed time
     *
     * @return time_t : return widget's install time
     */
    time_t getInstallTime() const;

    /**
     * This method gets widget base folder.
     *
     * @return widget base folder.
     * @exception WRT_CONF_ERR_EMDB_FAILURE - Fail to query DB table.
     * @exception WRT_CONF_ERR_EMDB_NO_RECORD - Can not find matching records in
     *  DB table.
     */
    std::string getBaseFolder() const;

    /**
     * This method gets deletable property of widget.
     *
     * @return true: can be deleted; false: can not be deleted
     * @exception WRT_CONF_ERR_GCONF_FAILURE
     * @exception WRT_CONF_ERR_EMDB_FAILURE
     * @exception WRT_CONF_ERR_EMDB_NO_RECORD
     */
    bool isDeletable() const;

    /* This method gets the parameter list for resource.
     */
    ResourceAttributeList getResourceAttribute(
            const std::string &resourceId) const;

    /* This method checks read only flag for given property
     */
    DPL::OptionalInt checkPropertyReadFlag(
            const PropertyDAOReadOnly::WidgetPropertyKey &key) const;

    /* This method gets widget property key list
     */
    PropertyDAOReadOnly::WidgetPropertyKeyList getPropertyKeyList() const;

    /* This method gets widget property list
     */
    PropertyDAOReadOnly::WidgetPreferenceList getPropertyList() const;

    /* This method get widget property value
     */
    PropertyDAOReadOnly::WidgetPropertyValue getPropertyValue(
            const PropertyDAOReadOnly::WidgetPropertyKey &key) const;

    LanguageTagList getLanguageTags() const;
    LanguageTagList getIconLanguageTags() const;

    WidgetLocalizedInfo getLocalizedInfo(const DPL::String& languageTag) const;
    std::string getCookieDatabasePath() const;
    // Local storage
    std::string getPrivateLocalStoragePath() const;

    bool getBackSupported() const;

    static bool isWidgetInstalled(DbWidgetHandle handle);
    static bool isWidgetInstalled(DPL::String pkgName);

    /* This method get path of the splash image.
     *
     * @return path of the widget's splash image
     */
    DPL::OptionalString getSplashImgSrc() const;

    CertificateChainList getWidgetCertificate() const;

    void getWidgetSettings(WidgetSettings& outWidgetSettings) const;

    /**
     * This method gets application service list that define AUL value
     *
     * @return See above comment
     * @exception WRT_CONF_ERR_EMDB_FAILURE - Fail to query DB table.
     */
    void getAppServiceList(
            WidgetApplicationServiceList& outAppServiceList) const;

    /**
     * This method returns the type of the package.
     *
     * @return PkgType
     * @exception WRT_CONF_ERR_EMDB_FAILURE - Fail to query DB table.
     * @exception WRT_CONF_ERR_EMDB_NO_RECORD - Can not find matching
                                                records in DB table.
     */
    PkgType getPkgType() const;

    void getEncryptedFileList(EncryptedFileList& filesList) const;
};

} // namespace WrtDB

#endif // _WRT_SRC_CONFIGURATION_WIDGET_DAO_READ_ONLY_H_

