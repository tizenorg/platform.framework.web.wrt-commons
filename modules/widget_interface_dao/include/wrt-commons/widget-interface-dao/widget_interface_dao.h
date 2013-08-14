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
/*
 * @author      Lukasz Marek (l.marek@samsung.com)
 * @version     0.1
 * @brief
 */

#ifndef _WIDGET_INTERFACE_DAO_H_
#define _WIDGET_INTERFACE_DAO_H_

#include <string>
#include <dpl/db/thread_database_support.h>
#include <dpl/db/orm_interface.h>

namespace WidgetInterfaceDB {
class WidgetInterfaceDAO
{
  public:

    /**
     * WidgetInterfaceDAO Exception classes
     */
    class Exception
    {
      public:
        DECLARE_EXCEPTION_TYPE(DPL::Exception, Base)
        DECLARE_EXCEPTION_TYPE(Base, DatabaseError)
        DECLARE_EXCEPTION_TYPE(Base, LocalStorageValueNoModifableException)
        DECLARE_EXCEPTION_TYPE(Base, InvalidArgumentException)
    };
    WidgetInterfaceDAO(int widgetHandle);
    virtual ~WidgetInterfaceDAO();

    static std::string databaseFileName(int widgetHandle);
    void setItem(const std::string& key,
                 const std::string& value,
                 bool readOnly,
                 bool fromConfigXml);
    void setItem(const std::string& key,
                 const std::string& value,
                 bool readOnly);
    void removeItem(const std::string& key);
    DPL::Optional<std::string> getValue(const std::string& key) const;
    void clear(bool removeReadOnly);
    size_t getStorageSize() const;
    std::string getKeyByIndex(size_t index) const;

  protected:
    int m_widgetHandle;
    mutable DPL::DB::ThreadDatabaseSupport m_databaseInterface;

  private:
    void checkDatabase();
    void copyPropertiesFromWrtDatabase();
};
typedef std::unique_ptr<WidgetInterfaceDAO> WidgetInterfaceDAOPtr;
}
#endif //_WIDGET_INTERFACE_DAO_H_
