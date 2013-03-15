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
 * @file        fast_delegate.h
 * @author      Przemyslaw Dobrowolski (p.dobrowolsk@samsung.com)
 * @version     1.0
 * @brief       This file is the header file of fast delegate
 */
#ifndef DPL_FAST_DELEGATE_H
#define DPL_FAST_DELEGATE_H

#pragma GCC system_header

#define FASTDELEGATE_USESTATICFUNCTIONHACK
#define FASTDELEGATE_ALLOW_FUNCTION_TYPE_SYNTAX

#include <dpl/3rdparty/fastdelegate/FastDelegate.h>
#include <dpl/3rdparty/fastdelegate/FastDelegateBind.h>

namespace DPL {
using namespace fastdelegate;
} // namespace DPL

#endif // DPL_FAST_DELEGATE_H
