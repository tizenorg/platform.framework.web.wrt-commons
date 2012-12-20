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
 * This file contains the declaration of custom handler dao class.
 *
 * @file    custom_handler_dao_read_only.cpp
 * @author  Krzysztof Jackiewicz (k.jackiewicz@samsung.com)
 * @version 1.0
 * @brief   This file contains the declaration of custom handler dao
 */

#include <wrt-commons/custom-handler-dao-ro/custom_handler_dao_read_only.h>
#include <wrt-commons/custom-handler-dao-ro/CustomHandlerDatabase.h>

#include <orm_generator_custom_handler.h>

using namespace DPL::DB::ORM;
using namespace DPL::DB::ORM::custom_handler;

namespace CustomHandlerDB {

namespace {

template <typename T>
CustomHandlerPtr getSingleHandler(std::list<T> row)
{
    CustomHandlerPtr handler;
    if (!row.empty()) {
        // There should be only one
        if (row.size() > 1) {
            ThrowMsg(CustomHandlerDAOReadOnly::Exception::DatabaseError,
                     "More than one handler registered");
        }

        handler.reset(new CustomHandler());
        handler->target = row.front().Get_target();
        handler->base_url = row.front().Get_base_url();
        handler->url = row.front().Get_url();
        handler->title = row.front().Get_title();
        handler->user_allowed = static_cast<bool>(row.front().Get_user_allowed());
        handler->user_decision =
            static_cast<HandlerState>(row.front().Get_user_allowed());
    }
    return handler;
}

} // namespace

CustomHandlerDAOReadOnly::CustomHandlerDAOReadOnly(const DPL::String& pkgName) :
        m_pkgName(pkgName)
{
}

CustomHandlerDAOReadOnly::~CustomHandlerDAOReadOnly()
{
}

CustomHandlerPtr CustomHandlerDAOReadOnly::getProtocolHandler(
        const DPL::String& protocol,
        const DPL::String& url)
{
    LogDebug("Getting protocol handler");
    Try{
        CUSTOM_HANDLER_DB_SELECT(select, ProtocolHandlers);

        select->Where(And(Equals<ProtocolHandlers::app_id>(m_pkgName),
                      And(Equals<ProtocolHandlers::target>(protocol),
                          Equals<ProtocolHandlers::url>(url))));

        std::list<ProtocolHandlers::Row> list = select->GetRowList();
        return getSingleHandler(list);
    } Catch(DPL::DB::SqlConnection::Exception::Base) {
        ReThrowMsg(CustomHandlerDAOReadOnly::Exception::DatabaseError,
                   "Failed to get protocol handler");
    }
}

CustomHandlerPtr CustomHandlerDAOReadOnly::getContentHandler(
        const DPL::String& content,
        const DPL::String& url)
{
    LogDebug("Getting content handler");
    Try{
        CUSTOM_HANDLER_DB_SELECT(select, ContentHandlers);

        select->Where(And(Equals<ContentHandlers::app_id>(m_pkgName),
                      And(Equals<ContentHandlers::target>(content),
                          Equals<ContentHandlers::url>(url))));

        std::list<ContentHandlers::Row> list = select->GetRowList();
        return getSingleHandler(list);
    } Catch(DPL::DB::SqlConnection::Exception::Base) {
        ReThrowMsg(CustomHandlerDAOReadOnly::Exception::DatabaseError,
                   "Failed to get content handler");
    }
}

CustomHandlerPtr CustomHandlerDAOReadOnly::getActivProtocolHandler(
        const DPL::String& protocol)
{
    LogDebug("Getting active protocol handler");
    Try{
        CUSTOM_HANDLER_DB_SELECT(select, ProtocolHandlers);

        select->Where(And(Equals<ProtocolHandlers::app_id>(m_pkgName),
                          Equals<ProtocolHandlers::target>(protocol)));

        std::list<ProtocolHandlers::Row> list = select->GetRowList();
        CustomHandlerPtr handler;

        FOREACH(it, list) {
            if (it->Get_user_allowed() & Agreed) {
                handler.reset(new CustomHandler());
                handler->base_url = it->Get_base_url();
                handler->target = it->Get_target();
                handler->title = it->Get_title();
                handler->url = it->Get_url();
                handler->user_allowed =
                    static_cast<bool>(it->Get_user_allowed());
                handler->user_decision =
                    static_cast<HandlerState>(it->Get_user_allowed());
            }
        }
        return handler;
    } Catch(DPL::DB::SqlConnection::Exception::Base) {
        ReThrowMsg(CustomHandlerDAOReadOnly::Exception::DatabaseError,
                   "Failed to get protocol handler");
    }
}

CustomHandlerPtr CustomHandlerDAOReadOnly::getProtocolHandler(
        const DPL::String& protocol,
        const DPL::String& url,
        const DPL::String& baseURL)
{
    LogDebug("Check if protocol is registered");
    Try{
        CUSTOM_HANDLER_DB_SELECT(select, ProtocolHandlers);

        select->Where(And(Equals<ProtocolHandlers::app_id>(m_pkgName),
                          And(Equals<ProtocolHandlers::target>(protocol),
                              And(Equals<ProtocolHandlers::url>(url),
                                  Equals<ProtocolHandlers::base_url>(baseURL)
                              )
                           )
                       )
                 );

        std::list<ProtocolHandlers::Row> list = select->GetRowList();
        return getSingleHandler(list);
    } Catch(DPL::DB::SqlConnection::Exception::Base) {
        ReThrowMsg(CustomHandlerDAOReadOnly::Exception::DatabaseError,
                   "Failed to get content handler");
    }
}


CustomHandlerPtr CustomHandlerDAOReadOnly::getActivContentHandler(
        const DPL::String& content)
{
    LogDebug("Getting active content handler");
    Try{
        CUSTOM_HANDLER_DB_SELECT(select, ContentHandlers);

        select->Where(And(Equals<ContentHandlers::app_id>(m_pkgName),
                          Equals<ContentHandlers::target>(content)));

        std::list<ContentHandlers::Row> list = select->GetRowList();
        CustomHandlerPtr handler;

        FOREACH(it, list) {
            if (it->Get_user_allowed() & Agreed) {
                handler.reset(new CustomHandler());
                handler->base_url = it->Get_base_url();
                handler->target = it->Get_target();
                handler->title = it->Get_title();
                handler->url = it->Get_url();
                handler->user_allowed =
                    static_cast<bool>(it->Get_user_allowed());
                handler->user_decision =
                    static_cast<HandlerState>(it->Get_user_allowed());
            }
        }
        return handler;
    } Catch(DPL::DB::SqlConnection::Exception::Base) {
        ReThrowMsg(CustomHandlerDAOReadOnly::Exception::DatabaseError,
                   "Failed to get protocol handler");
    }
}

CustomHandlerPtr CustomHandlerDAOReadOnly::getContentHandler(
        const DPL::String& content,
        const DPL::String& url,
        const DPL::String& baseURL)
{
    LogDebug("Check if content is registered");
    Try{
        CUSTOM_HANDLER_DB_SELECT(select, ContentHandlers);

        select->Where(And(Equals<ContentHandlers::app_id>(m_pkgName),
                      And(Equals<ContentHandlers::target>(content),
                      And(Equals<ContentHandlers::url>(url),
                          Equals<ContentHandlers::base_url>(baseURL)))));

        std::list<ContentHandlers::Row> list = select->GetRowList();
        return getSingleHandler(list);
    } Catch(DPL::DB::SqlConnection::Exception::Base) {
        ReThrowMsg(CustomHandlerDAOReadOnly::Exception::DatabaseError,
                   "Failed to get content handler");
    }
}

CustomHandlerPtr CustomHandlerDAOReadOnly::getAllowedProtocolHandler(
        const DPL::String& protocol)
{
    LogDebug("Getting allowed protocol handler");
    Try{
        CUSTOM_HANDLER_DB_SELECT(select, ProtocolHandlers);

        select->Where(And(Equals<ProtocolHandlers::app_id>(m_pkgName),
                      And(Equals<ProtocolHandlers::target>(protocol),
                          Equals<ProtocolHandlers::user_allowed>(true))));

        std::list<ProtocolHandlers::Row> list = select->GetRowList();
        return getSingleHandler(list);
    } Catch(DPL::DB::SqlConnection::Exception::Base) {
        ReThrowMsg(CustomHandlerDAOReadOnly::Exception::DatabaseError,
                   "Failed to get content handler");
    }
}

CustomHandlerPtr CustomHandlerDAOReadOnly::getAllowedContentHandler(
        const DPL::String& protocol)
{
    LogDebug("Getting allowed content handler");
    Try{
        CUSTOM_HANDLER_DB_SELECT(select, ContentHandlers)

        select->Where(And(Equals<ContentHandlers::app_id>(m_pkgName),
                      And(Equals<ContentHandlers::target>(protocol),
                          Equals<ContentHandlers::user_allowed>(true))));

        std::list<ContentHandlers::Row> list = select->GetRowList();
        return getSingleHandler(list);
    } Catch(DPL::DB::SqlConnection::Exception::Base) {
        ReThrowMsg(CustomHandlerDAOReadOnly::Exception::DatabaseError,
                   "Failed to get content handler");
    }
}

} // namespace CustomHandlerDB
