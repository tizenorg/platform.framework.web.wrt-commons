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
#include <wrt-commons/custom-handler-dao-ro/custom_handler_dao_read_only.h>

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
    row.Set_user_allowed(handler.user_decision);
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
        if (handler.user_decision & Agreed) {
            //need to disable all previous, agreed entries
            CUSTOM_HANDLER_DB_SELECT(select, ContentHandlers);
            select->Where(And(Equals<ContentHandlers::target>(handler.target),
                          Or(Equals<ContentHandlers::user_allowed>(Agreed),
                             Equals<ContentHandlers::user_allowed>(AgreedPermanently))
                          ));
            ContentHandlers::Select::RowList rows = select->GetRowList();
            if (rows.size() > 1) {
                //more than one activ content handler - not good. Remove all.
                //this should never happen
                LogError("Database data incoherent.");
                CUSTOM_HANDLER_DB_DELETE(deleteContent, ContentHandlers);
                deleteContent->Where(And(Equals<ContentHandlers::target>(handler.target),
                         Or(Equals<ContentHandlers::user_allowed>(Agreed),
                            Equals<ContentHandlers::user_allowed>(AgreedPermanently))));
                deleteContent->Execute();
                //all content handlers removed. New one can be inserted
            } else if (!rows.empty()) {
                //one active handler. Can be updaed
                LogDebug("Activ content handler exist. Update");
                CUSTOM_HANDLER_DB_UPDATE(update, ContentHandlers);
                update->Where(And(Equals<ContentHandlers::target>(handler.target),
                              Or(Equals<ContentHandlers::user_allowed>(Agreed),
                                 Equals<ContentHandlers::user_allowed>(AgreedPermanently))
                              ));
                ContentHandlers::Row rowToUpdate = rows.front();

                if (handler.user_decision & DecisionSaved) {
                    //do not ask about previous one
                    rowToUpdate.Set_user_allowed(DeclinedPermanently);
                } else {
                    //ask for next time
                    rowToUpdate.Set_user_allowed(Declined);
                }
                update->Values(rowToUpdate);
                update->Execute();
            }
        }
        LogDebug("Inserting new content handler");
        ContentHandlers::Row row;
        fillRow(row, handler, m_pkgName);
        if (getContentHandler(handler.target, handler.url, handler.base_url)) {
            LogDebug("Content handler exist. Update its state");
            CUSTOM_HANDLER_DB_UPDATE(updateRow, ContentHandlers);
            updateRow->Where(And(Equals<ContentHandlers::app_id>(m_pkgName),
                             And(Equals<ContentHandlers::target>(handler.target),
                             And(Equals<ContentHandlers::url>(handler.url),
                                 Equals<ContentHandlers::base_url>(handler.base_url)))));
            updateRow->Values(row);
            updateRow->Execute();
            LogDebug("updated");
            return;
        }
        CUSTOM_HANDLER_DB_INSERT(insert, ContentHandlers);
        insert->Values(row);
        insert->Execute();
        LogDebug("insterted");
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

        if (handler.user_decision & Agreed) {
            //need to disable all previous, agreed entries
            CUSTOM_HANDLER_DB_SELECT(select, ProtocolHandlers);
            select->Where(And(Equals<ProtocolHandlers::target>(handler.target),
                          Or(Equals<ProtocolHandlers::user_allowed>(Agreed),
                             Equals<ProtocolHandlers::user_allowed>(AgreedPermanently))
                          ));
            ProtocolHandlers::Select::RowList rows = select->GetRowList();
            if (rows.size() > 1) {
                //more than one activ protocol handler - not good. Remove all.
                //this should never happen
                LogError("Database data incoherent.");
                CUSTOM_HANDLER_DB_DELETE(deleteProtocol, ProtocolHandlers);
                deleteProtocol->Where(And(Equals<ProtocolHandlers::target>(handler.target),
                         Or(Equals<ProtocolHandlers::user_allowed>(Agreed),
                            Equals<ProtocolHandlers::user_allowed>(AgreedPermanently))));
                deleteProtocol->Execute();
                //all protocol handlers removed. New one can be inserted
            } else if (!rows.empty()) {
                //one active handler. Can be updaed
                CUSTOM_HANDLER_DB_UPDATE(update, ProtocolHandlers);
                update->Where(And(Equals<ProtocolHandlers::target>(handler.target),
                              Or(Equals<ProtocolHandlers::user_allowed>(Agreed),
                                 Equals<ProtocolHandlers::user_allowed>(AgreedPermanently))
                              ));
                ProtocolHandlers::Row rowToUpdate = rows.front();

                if (handler.user_decision & DecisionSaved) {
                    //do not ask about previous one
                    rowToUpdate.Set_user_allowed(DeclinedPermanently);
                } else {
                    //ask for next time
                    rowToUpdate.Set_user_allowed(Declined);
                }
                update->Values(rowToUpdate);
                update->Execute();
            }
        }
        LogDebug("Inserting new protocol handler");
        ProtocolHandlers::Row row;
        fillRow(row, handler, m_pkgName);
        if (getProtocolHandler(handler.target, handler.url, handler.base_url)) {
            LogDebug("Protocol handler exist. Update its state");
            CUSTOM_HANDLER_DB_UPDATE(updateRow, ProtocolHandlers);
            updateRow->Where(And(Equals<ProtocolHandlers::app_id>(m_pkgName),
                             And(Equals<ProtocolHandlers::target>(handler.target),
                             And(Equals<ProtocolHandlers::url>(handler.url),
                                 Equals<ProtocolHandlers::base_url>(handler.base_url)))));
            updateRow->Values(row);
            updateRow->Execute();
            LogDebug("updated");
            return;
        }
        CUSTOM_HANDLER_DB_INSERT(insert, ProtocolHandlers);
        insert->Values(row);
        insert->Execute();
        LogDebug("insterted");
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

void CustomHandlerDAO::unregisterContentHandler(const DPL::String& target,
                                                const DPL::String& url,
                                                const DPL::String& baseURL)
{
    LogDebug("Removing content handler " << target << " " << url);
    Try {
       CUSTOM_HANDLER_DB_DELETE(deleteFrom, ContentHandlers);
       deleteFrom->Where(And(Equals<ContentHandlers::app_id>(m_pkgName),
                         And(Equals<ContentHandlers::target>(target),
                         And(Equals<ContentHandlers::url>(url),
                             Equals<ContentHandlers::base_url>(baseURL)))));
       deleteFrom->Execute();
    }
    Catch(DPL::DB::SqlConnection::Exception::Base) {
       ReThrowMsg(CustomHandlerDAO::Exception::DatabaseError,
                  "Failed to remove content handler");
    }
}

void CustomHandlerDAO::unregisterProtocolHandler(const DPL::String& target,
                                                 const DPL::String& url,
                                                 const DPL::String& baseURL)
{
    LogDebug("Removing protocol handler " << target << " " << url);
    Try {
       CUSTOM_HANDLER_DB_DELETE(deleteFrom, ProtocolHandlers);
       deleteFrom->Where(And(Equals<ProtocolHandlers::app_id>(m_pkgName),
                         And(Equals<ProtocolHandlers::target>(target),
                         And(Equals<ProtocolHandlers::url>(url),
                             Equals<ProtocolHandlers::base_url>(baseURL)))));
       deleteFrom->Execute();
    }
    Catch(DPL::DB::SqlConnection::Exception::Base) {
       ReThrowMsg(CustomHandlerDAO::Exception::DatabaseError,
                  "Failed to remove content handler");
    }

}

} // namespace CustomHandlerDB
