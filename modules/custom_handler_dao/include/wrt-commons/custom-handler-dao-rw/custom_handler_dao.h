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
 * @file    custom_handler_dao.h
 * @author  Krzysztof Jackiewicz (k.jackiewicz@samsung.com)
 * @version 1.0
 * @brief   This file contains the declaration of custom handler dao
 */
#ifndef _CUSTOM_HANDLER_DAO_H_
#define _CUSTOM_HANDLER_DAO_H_

#include <wrt-commons/custom-handler-dao-ro/custom_handler_dao_read_only.h>

namespace CustomHandlerDB {

class CustomHandlerDAO : public CustomHandlerDAOReadOnly
{
  public:
    explicit CustomHandlerDAO(const DPL::String& pkgName);
    virtual ~CustomHandlerDAO();

    /**
     * Registers custom content handler
     */
    void registerContentHandler(const CustomHandler& handler);

    /**
     * Registers custom protocol handler
     */
    void registerProtocolHandler(const CustomHandler& handler);

    /**
     * Unregisters custom content handler
     */
    void unregisterContentHandler(const DPL::String& target,
                                  const DPL::String& burl);
    /**
     * Unregisters custom protocol handler
     */
    void unregisterProtocolHandler(const DPL::String& target,
                                   const DPL::String& url);
};

} // namespace CustomHandlerDB

#endif // _CUSTOM_HANDLER_DAO_H_
