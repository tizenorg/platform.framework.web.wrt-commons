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
#include <map>
#include <set>
#include <string>

#include <dpl/string.h>
#include <dpl/exception.h>
#include <dpl/optional_typedefs.h>

#include <dpl/wrt-dao-ro/common_dao_types.h>
//#include "../wrt-dao-ro/common_dao_types.h"

typedef DPL::OptionalString WidgetGUID;

namespace ConfigParserData {

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
} // namespace ConfigParserData
namespace WrtDB {

typedef std::list<DPL::String> StringList;

struct WidgetLocalizedInfo
{
    DPL::OptionalString name;
    DPL::OptionalString shortName;
    DPL::OptionalString description;
    DPL::OptionalString license;
    DPL::OptionalString licenseHref;
};

typedef std::list<DPL::String> LanguageTagList;

class WidgetDAO
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
    WidgetDAO(DbWidgetHandle widgetHandle)
      : m_widgetHandle(widgetHandle)
    {}

    /**
     * Destructor
     */
    virtual ~WidgetDAO(){}

    /**
     * This method returns widget handle(m_widgetHandle).
     *
     * @return widget handle(m_widgetHandle).
     * @exception WRT_CONF_ERR_EMDB_FAILURE - Fail to query DB table.
     * @exception WRT_CONF_ERR_EMDB_NO_RECORD - Can not find matching records in DB table.
     */
    DbWidgetHandle getHandle() const { return m_widgetHandle; }
    DbWidgetHandle getHandle(const WidgetGUID GUID) const;
    static DbWidgetHandle getHandle(const DPL::String tizenAppId);

    /**
     * This method returns the root directory of widget resource.
     *
     * @return path name of root directory.
     * @exception WRT_CONF_ERR_EMDB_FAILURE - Fail to query DB table.
     * @exception WRT_CONF_ERR_EMDB_NO_RECORD - Can not find matching records in
     * DB table.
     */
    DPL::String getPath() const;
    void setPath(const DPL::String &path) const;

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
    void setLocalizedStartFileList(const LocalizedStartFileList &list) const;
    /**
     * This method returns list of start files;
     *
     * @exception WRT_CONF_ERR_EMDB_FAILURE - Fail to query DB table.
     * @exception WRT_CONF_ERR_EMDB_NO_RECORD - Can not find matching records in
     *  DB table.
     */
    WidgetStartFileList getStartFileList() const;
    void setStartFileList(const WidgetStartFileList &list) const;

    WidgetLocalizedInfo getLocalizedInfo(const DPL::String& languageTag) const;
  protected:
    static std::map<DbWidgetHandle,WidgetStartFileList> s_startFileMap;
    static std::map<DbWidgetHandle,LocalizedStartFileList> s_localizedStartFileMap;
    static std::map<DbWidgetHandle,DPL::String> s_pathMap;
};

} // namespace WrtDB

#endif // _WRT_SRC_CONFIGURATION_WIDGET_DAO_READ_ONLY_H_

