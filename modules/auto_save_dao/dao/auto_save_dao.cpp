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
 * @file    auto_save_dao.cpp
 * @author  Jihoon Chung (jihoon.chung@samsung.com)
 * @version 1.0
 * @brief    This file contains the definition of auto save dao class.
 */

#include <wrt-commons/auto-save-dao-rw/auto_save_dao.h>
#include <wrt-commons/auto-save-dao/AutoSaveDatabase.h>
#include <orm_generator_autosave.h>
#include <dpl/foreach.h>

using namespace DPL::DB::ORM;
using namespace DPL::DB::ORM::autosave;
using namespace AutoSaveDB::Interface;

namespace AutoSaveDB {

#define SQL_CONNECTION_EXCEPTION_HANDLER_BEGIN          Try

#define SQL_CONNECTION_EXCEPTION_HANDLER_END(message)   \
    Catch(DPL::DB::SqlConnection::Exception::Base) {       \
        LogError(message);                              \
        ReThrowMsg(AutoSaveDAO::Exception::DatabaseError, \
                   message);                            \
    }

AutoSaveDAO::AutoSaveDAO() :
    AutoSaveDAOReadOnly()
{
}

AutoSaveDAO::~AutoSaveDAO()
{
}

void AutoSaveDAO::attachDatabaseRW(void)
{
    m_autoSavedbInterface.AttachToThread(
        DPL::DB::SqlConnection::Flag::RW);
}

void AutoSaveDAO::detachDatabase(void)
{
    m_autoSavedbInterface.DetachFromThread();
}

void AutoSaveDAO::setAutoSaveSubmitFormData(const DPL::String &url,
                                      const SubmitFormData &submitFormData)
{
    SQL_CONNECTION_EXCEPTION_HANDLER_BEGIN
    {
        ScopedTransaction transaction(&m_autoSavedbInterface);

        SubmitFormData submitData = getAutoSaveSubmitFormData(url);
        if (!submitData.empty()) {
            AUTOSAVE_DB_DELETE(del,
                               AutoSaveSubmitFormElement,
                               &m_autoSavedbInterface)
            del->Where(Equals<AutoSaveSubmitFormElement::address>(url));
            del->Execute();
        }

        FOREACH(pSubmitFormData, submitFormData) {
            AutoSaveSubmitFormElement::Row row;
            row.Set_address(url);
            row.Set_key(pSubmitFormData->key);
            row.Set_value(pSubmitFormData->value);

            AUTOSAVE_DB_INSERT(
                insert, AutoSaveSubmitFormElement, &m_autoSavedbInterface);
            insert->Values(row);
            insert->Execute();
        }

        transaction.Commit();
    }
    SQL_CONNECTION_EXCEPTION_HANDLER_END(
        "Fail to register id, passwd for autosave")
}
#undef SQL_CONNECTION_EXCEPTION_HANDLER_BEGIN
#undef SQL_CONNECTION_EXCEPTION_HANDLER_END

} // namespace AutoSaveDB
