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
 * @file    auto_save_dao.h
 * @author  Jihoon Chung (jihoon.chung@samsung.com)
 * @version 1.0
 * @brief   This file contains the declaration of auto save dao
 */
#ifndef _AUTO_SAVE_DAO_H_
#define _AUTO_SAVE_DAO_H_

#include <wrt-commons/auto-save-dao-ro/auto_save_dao_read_only.h>

namespace AutoSaveDB {

class AutoSaveDAO : public AutoSaveDAOReadOnly
{
  public:

    AutoSaveDAO();
    virtual ~AutoSaveDAO();

    static void attachDatabaseRW(void);
    static void detachDatabase(void);

    /**
     * This method sets Autofill for Webkit
     */
    static void setAutoSaveSubmitFormData(
            const DPL::String& url, const SubmitFormData &submitFormData);
};

} // namespace AutoSaveDB

#endif // _AUTO_SAVE_DAO_H_
