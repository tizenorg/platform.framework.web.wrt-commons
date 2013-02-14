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
 * @file   TestCases_CustomHandlerDAO.cpp
 * @author  Krzysztof Jackiewicz (k.jackiewicz@samsung.com)
 * @version 1.0
 * @brief   This file contains tests for custom handler dao class.
 */

#include <dpl/test/test_runner.h>
#include <dpl/foreach.h>
#include <wrt-commons/custom-handler-dao-rw/custom_handler_dao.h>
#include <wrt-commons/custom-handler-dao-ro/common_dao_types.h>

using namespace CustomHandlerDB;

RUNNER_TEST_GROUP_INIT(DAO)

namespace {
const DPL::String P_TARGET(L"p_target");
const DPL::String P_BASE_URL(L"p_base_url");
const DPL::String P_URL(L"p_url");
const DPL::String P_TITLE(L"p_title");

const DPL::String C_TARGET(L"c_target");
const DPL::String C_BASE_URL(L"c_base_url");
const DPL::String C_URL(L"c_url");
const DPL::String C_TITLE(L"c_title");

void checkHandlersExistence(CustomHandlerDAOReadOnly& dao,
                            bool protocol,
                            bool content)
{
    CustomHandlerDB::CustomHandlerPtr handler;
    handler = dao.getProtocolHandler(P_TARGET, P_URL);
    RUNNER_ASSERT_MSG((!!handler) == protocol, "Protocol handler check");
    handler = dao.getContentHandler(C_TARGET, C_URL);
    RUNNER_ASSERT_MSG((!!handler) == content, "Content handler check");
}
} // namespace

RUNNER_TEST(custom_handler_empty_db_read)
{
    CustomHandlerDAOReadOnly dao(DPL::String(L"test"));
}

RUNNER_TEST(custom_handlers)
{
    CustomHandlerDAOReadOnly dao_ro(L"test");
    CustomHandlerDAO dao_rw(L"test");

    CustomHandlerDB::CustomHandlerPtr handler;
    CustomHandlerDB::CustomHandler p_handler;
    p_handler.target = P_TARGET;
    p_handler.base_url = P_BASE_URL;
    p_handler.url = P_URL;
    p_handler.title = P_TITLE;
    p_handler.user_decision = Agreed;

    // initial check
    checkHandlersExistence(dao_ro, false, false);

    // Protocol handler registration
    dao_rw.registerProtocolHandler(p_handler);
    checkHandlersExistence(dao_ro, true, false);

    handler = dao_ro.getProtocolHandler(P_TARGET, P_URL);
    RUNNER_ASSERT(handler);
    RUNNER_ASSERT(handler->target == P_TARGET);
    RUNNER_ASSERT(handler->base_url == P_BASE_URL);
    RUNNER_ASSERT(handler->url == P_URL);
    RUNNER_ASSERT(handler->title == P_TITLE);
    RUNNER_ASSERT(handler->user_decision == Agreed);

    // Content handler registration
    CustomHandlerDB::CustomHandler c_handler;
    c_handler.target = C_TARGET;
    c_handler.base_url = C_BASE_URL;
    c_handler.url = C_URL;
    c_handler.title = C_TITLE;
    c_handler.user_decision = DeclinedPermanently;

    dao_rw.registerContentHandler(c_handler);
    checkHandlersExistence(dao_ro, true, true);
    handler = dao_ro.getContentHandler(C_TARGET, C_URL);

    RUNNER_ASSERT(handler);
    RUNNER_ASSERT(handler->target == C_TARGET);
    RUNNER_ASSERT(handler->base_url == C_BASE_URL);
    RUNNER_ASSERT(handler->url == C_URL);
    RUNNER_ASSERT(handler->title == C_TITLE);
    RUNNER_ASSERT(handler->user_decision == DeclinedPermanently);

    // Handler unregistration
    dao_rw.unregisterProtocolHandler(P_TARGET, P_URL);
    checkHandlersExistence(dao_ro, false, true);

    // Nonexistent unregistration
    dao_rw.unregisterContentHandler(L"blah", L"blah");
    checkHandlersExistence(dao_ro, false, true);

    // Cleanup
    dao_rw.unregisterContentHandler(C_TARGET, C_URL);
    checkHandlersExistence(dao_ro, false, false);
}
