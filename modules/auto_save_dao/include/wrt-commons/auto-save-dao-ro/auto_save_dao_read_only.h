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
 * @file    auto_save_dao_read_only.h
 * @author  Jihoon Chung (jihoon.chung@samsung.com)
 * @version 1.0
 * @brief   This file contains the declaration of auto save dao
 */

#ifndef _AUTO_SAVE_DAO_READ_ONLY_H_
#define _AUTO_SAVE_DAO_READ_ONLY_H_

#include <dpl/string.h>
#include <dpl/exception.h>
#include <wrt-commons/auto-save-dao/common_dao_types.h>

namespace AutoSaveDB {

class AutoSaveDAOReadOnly
{
  public:
    /**
     * AutoSaveDAOReadOnly Exception classes
     */
    class Exception
    {
      public:
        DECLARE_EXCEPTION_TYPE(DPL::Exception, Base)
        DECLARE_EXCEPTION_TYPE(Base, DatabaseError)
    };

  public:

    AutoSaveDAOReadOnly();
    virtual ~AutoSaveDAOReadOnly();

    static void attachDatabaseRO(void);
    static void detachDatabase(void);

    /**
     * This method gets Autofill for Webkit
     */
    static DPL::Optional<AutoSaveData> getAutoSaveIdPasswd(
            const DPL::String &url);

};

} // namespace AutoSaveDB

#endif // _AUTO_SAVE_DAO_READ_ONLY_H_

