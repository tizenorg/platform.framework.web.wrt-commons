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
 * @file        named_input_pipe.h
 * @author      Przemyslaw Dobrowolski (p.dobrowolsk@samsung.com)
 * @version     1.0
 * @brief       This file is the implementation file of named input pipe
 */
#ifndef DPL_NAMED_PIPE_H
#define DPL_NAMED_PIPE_H

#include <dpl/exception.h>
#include <dpl/named_base_pipe.h>
#include <dpl/file_input.h>

namespace DPL {
class NamedInputPipe :
    public NamedBasePipe,
    public FileInput
{};
} // namespace DPL

#endif // DPL_NAMED_PIPE_H
