/*
 * Copyright (c) 2012 Samsung Electronics Co., Ltd All Rights Reserved
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
 * @file    custom_handler_dao.cpp
 * @author  Krzysztof Jackiewicz (k.jackiewicz@samsung.com)
 * @version 1.0
 * @brief    This file contains the definition of custom handler dao class.
 */

#include <wrt-commons/custom-handler-dao-rw/custom_handler_dao.h>
#include <wrt-commons/custom-handler-dao-ro/CustomHandlerDatabase.h>
#include <orm_generator_custom_handler.h>

using namespace DPL::DB::ORM;
using namespace DPL::DB::ORM::custom_handler;

namespace CustomHandlerDB {

namespace {

template <typename T>
void fillRow(T& row, const CustomHandler& handler, const DPL::String& pkgName)
{
    row.Set_app_id(pkgName);
    row.Set_target(handler.target);
    row.Set_base_url(handler.base_url);
    row.Set_url(handler.url);
    row.Set_title(handler.title);
    row.Set_user_allowed(handler.user_allowed);
}

} // namespace

CustomHandlerDAO::CustomHandlerDAO(const DPL::String& pkgName) :
    CustomHandlerDAOReadOnly(pkgName)
{
}

CustomHandlerDAO::~CustomHandlerDAO()
{
}

void CustomHandlerDAO::registerContentHandler(const CustomHandler& handler)
{
    LogDebug("Registering content handler " << handler.target << " " <<
                handler.base_url);
    Try {
        ContentHandlers::Row row;
        fillRow(row, handler, m_pkgName);

        // TODO remove previous if necessary
        CUSTOM_HANDLER_DB_INSERT(insert, ContentHandlers);
        insert->Values(row);
        insert->Execute();
    }
    Catch(DPL::DB::SqlConnection::Exception::Base){
        ReThrowMsg(CustomHandlerDAO::Exception::DatabaseError,
                   "Failed to register custom handler");
    }
}

void CustomHandlerDAO::registerProtocolHandler(const CustomHandler& handler)
{
    LogDebug("Registering protocol handler " << handler.target << " " <<
            handler.base_url);
    Try {
        ProtocolHandlers::Row row;
        fillRow(row, handler, m_pkgName);

        // TODO remove previous if necessary
        CUSTOM_HANDLER_DB_INSERT(insert, ProtocolHandlers);
        insert->Values(row);
        insert->Execute();
    }
    Catch(DPL::DB::SqlConnection::Exception::Base){
        ReThrowMsg(CustomHandlerDAO::Exception::DatabaseError,
                   "Failed to register custom handler");
    }
}

void CustomHandlerDAO::unregisterContentHandler(const DPL::String& target,
                                                const DPL::String& url)
{
    LogDebug("Removing content handler " << target << " " << url);
    Try {
       CUSTOM_HANDLER_DB_DELETE(deleteFrom, ContentHandlers);
       deleteFrom->Where(And(Equals<ContentHandlers::app_id>(m_pkgName),
                         And(Equals<ContentHandlers::target>(target),
                             Equals<ContentHandlers::url>(url))));
       deleteFrom->Execute();
    }
    Catch(DPL::DB::SqlConnection::Exception::Base) {
       ReThrowMsg(CustomHandlerDAO::Exception::DatabaseError,
                  "Failed to remove content handler");
    }
}

void CustomHandlerDAO::unregisterProtocolHandler(const DPL::String& target,
                                                 const DPL::String& url)
{
    LogDebug("Removing protocol handler " << target << " " << url);
    Try {
       CUSTOM_HANDLER_DB_DELETE(deleteFrom, ProtocolHandlers);
       deleteFrom->Where(And(Equals<ProtocolHandlers::app_id>(m_pkgName),
                         And(Equals<ProtocolHandlers::target>(target),
                             Equals<ProtocolHandlers::url>(url))));
       deleteFrom->Execute();
    }
    Catch(DPL::DB::SqlConnection::Exception::Base) {
       ReThrowMsg(CustomHandlerDAO::Exception::DatabaseError,
                  "Failed to remove content handler");
    }

}

} // namespace CustomHandlerDB
