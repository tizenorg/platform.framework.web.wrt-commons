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
 * This file contains the declaration of custom handler dao class.
 *
 * @file    custom_handler_dao_read_only.h
 * @author  Krzysztof Jackiewicz (k.jackiewicz@samsung.com)
 * @version 1.0
 * @brief   This file contains the declaration of custom handler dao
 */

#ifndef _CUSTOM_HANDLER_DAO_READ_ONLY_H_
#define _CUSTOM_HANDLER_DAO_READ_ONLY_H_

#include <dpl/string.h>
#include <dpl/exception.h>
#include "common_dao_types.h"

namespace CustomHandlerDB {

class CustomHandlerDAOReadOnly
{
  public:
    /**
     * CustomHandlerDAOReadOnly Exception classes
     */
    class Exception
    {
      public:
        DECLARE_EXCEPTION_TYPE(DPL::Exception, Base)
        DECLARE_EXCEPTION_TYPE(Base, DatabaseError)
    };

  public:
    explicit CustomHandlerDAOReadOnly(const DPL::String& pkgName);
    virtual ~CustomHandlerDAOReadOnly();

    /**
     * Returns protocol handler
     */
    CustomHandlerPtr getProtocolHandler(const DPL::String& protocol,
                                        const DPL::String& url);

    /**
     * Returns content handler
     */
    CustomHandlerPtr getContentHandler(const DPL::String& content,
                                       const DPL::String& url);

    /**
     * Returns allowed handler for given protocol
     */
    CustomHandlerPtr getAllowedProtocolHandler(const DPL::String& protocol);

    /**
     * Returns allowed handler for given content
     */
    CustomHandlerPtr getAllowedContentHandler(const DPL::String& protocol);

  protected:
    DPL::String m_pkgName;
};

} // namespace CustomHandlerDB

#endif // _CUSTOM_HANDLER_DAO_READ_ONLY_H_

