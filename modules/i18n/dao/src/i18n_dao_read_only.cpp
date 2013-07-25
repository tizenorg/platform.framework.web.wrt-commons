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
 * This file contains the definition of i18n dao namespace.
 *
 * @file    i18n_dao_read_only.cpp
 * @author  Zbigniew Kostrzewa (z.kostrzewa@samsung.com)
 * @version 1.0
 * @brief   This file contains the definition of i18n dao
 */

#include <wrt-commons/i18n-dao-ro/i18n_dao_read_only.h>
#include <wrt-commons/i18n-dao-ro/i18n_database.h>

#include <orm_generator_i18n.h>

using namespace DPL::DB::ORM;
using namespace DPL::DB::ORM::i18n;

namespace I18n {
namespace DB {
namespace I18nDAOReadOnly {
bool IsValidSubTag(const DPL::String& tag, int type)
{
  I18N_DB_SELECT(select, iana_records)
  select->Where(And(Equals<iana_records::SUBTAG>(tag),
                    Equals<iana_records::TYPE>(type)));
  return !select->GetRowList().empty();
}
}
}
}
