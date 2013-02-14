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
 * This file contains the declaration of auto save dao class.
 *
 * @file    auto_save_dao_read_only.cpp
 * @author  Jihoon Chung (jihoon.chung@samsung.com)
 * @version 1.0
 * @brief   This file contains the declaration of auto save dao
 */
#include <stddef.h>
#include <wrt-commons/auto-save-dao-ro/auto_save_dao_read_only.h>
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
        ReThrowMsg(AutoSaveDAOReadOnly::Exception::DatabaseError, \
                   message);                            \
    }

AutoSaveDAOReadOnly::AutoSaveDAOReadOnly()
{}

AutoSaveDAOReadOnly::~AutoSaveDAOReadOnly()
{}

void AutoSaveDAOReadOnly::attachDatabaseRO(void)
{
    m_autoSavedbInterface.AttachToThread(
        DPL::DB::SqlConnection::Flag::RO);
}

void AutoSaveDAOReadOnly::detachDatabase(void)
{
    m_autoSavedbInterface.DetachFromThread();
}

SubmitFormData AutoSaveDAOReadOnly::getAutoSaveSubmitFormData(
    const DPL::String &url)
{
    SQL_CONNECTION_EXCEPTION_HANDLER_BEGIN
    {
        AUTOSAVE_DB_SELECT(select,
                           AutoSaveSubmitFormElement,
                           &m_autoSavedbInterface);
        select->Where(Equals<AutoSaveSubmitFormElement::address>(url));
        AutoSaveSubmitFormElement::Select::RowList rows = select->GetRowList();

        SubmitFormData submitFormData;
        FOREACH(it, rows) {
            SubmitFormElement element;
            element.key = it->Get_key();
            element.value = it->Get_value();
            submitFormData.push_back(element);
        }
        return submitFormData;
    }
    SQL_CONNECTION_EXCEPTION_HANDLER_END("Failed to get autosave data string")
}

#undef SQL_CONNECTION_EXCEPTION_HANDLER_BEGIN
#undef SQL_CONNECTION_EXCEPTION_HANDLER_END
} // namespace AutoSaveDB
