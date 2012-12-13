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
 *
 * @file    common_dao_types.h
 * @author  Krzysztof Jackiewicz (k.jackiewicz@samsung.com)
 * @version 1.0
 * @brief This file contains the declaration of
 *           common data types for custom handler database.
 */
#ifndef SHARE_COMMON_DAO_TYPES_H_
#define SHARE_COMMON_DAO_TYPES_H_

#include <list>
#include <memory>
#include <dpl/string.h>

namespace CustomHandlerDB {

/**
 * @brief Custom Handler struct
 *
 * Describes custom handler for protocol and content.
 */
struct CustomHandler
{
    DPL::String target;     // protocol/content ("mailto"/"application/x-soup")
    DPL::String base_url;   // base url of registered page
    DPL::String url;        // url used for protocol/content handling
    DPL::String title;      // user friendly handler name
    bool user_allowed;      // true if user has allowed the handler
};

typedef std::shared_ptr<CustomHandler> CustomHandlerPtr;

} // namespace CustomHandlerDB

#endif /* SHARE_COMMON_DAO_TYPES_H_ */
