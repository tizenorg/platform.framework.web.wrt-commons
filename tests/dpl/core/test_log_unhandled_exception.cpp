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
 * @file        test_log_unhandled_exception.cpp
 * @author      Pawel Sikorski (p.marcinkiew@samsung.com)
 * @version     1.0
 * @brief
 */
#include <dpl/test/test_runner.h>
#include <dpl/exception.h>
#include <iostream>

RUNNER_TEST_GROUP_INIT(DPL)

class MyException
{};

class MyDPLException
{
  public:
    DECLARE_EXCEPTION_TYPE(DPL::Exception, Base)
    DECLARE_EXCEPTION_TYPE(Base, MyException)
};

class MySTDException :
    public std::exception
{
  public:
    virtual const char* what() const throw()
    {
        return "my std exception occurred";
    }
};

RUNNER_TEST(Log_Unknown_Exception)
{
    UNHANDLED_EXCEPTION_HANDLER_BEGIN
    {
        //        throw MyException();
    }
    UNHANDLED_EXCEPTION_HANDLER_END
    RUNNER_ASSERT(true);
}

RUNNER_TEST(Log_DPL_Exception)
{
    UNHANDLED_EXCEPTION_HANDLER_BEGIN
    {
        //        Throw(MyDPLException::MyException);
    }
    UNHANDLED_EXCEPTION_HANDLER_END
    RUNNER_ASSERT(true);
}

RUNNER_TEST(Log_STD_Exception)
{
    UNHANDLED_EXCEPTION_HANDLER_BEGIN
    {
        //        throw MySTDException();
    }
    UNHANDLED_EXCEPTION_HANDLER_END
    RUNNER_ASSERT(true);
}
