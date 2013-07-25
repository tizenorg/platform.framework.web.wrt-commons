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
/**
 * This file contains the declaration of i18n dao namespace.
 *
 * @file    i18n_dao_read_only.h
 * @author  Zbigniew Kostrzewa (z.kostrzewa@samsung.com)
 * @version 1.0
 * @brief   This file contains the declaration of i18n dao.
 */

#ifndef _I18N_DAO_READ_ONLY_H_
#define _I18N_DAO_READ_ONLY_H_

#include <dpl/string.h>

namespace I18n {
namespace DB {
namespace I18nDAOReadOnly {
bool IsValidSubTag(const DPL::String& tag, int type);
} // namespace I18nDAOReadOnly
} // namespace DB
} // namespace I18n

#endif // _I18N_DAO_READ_ONLY_H_

