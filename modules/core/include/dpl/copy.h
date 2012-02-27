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
 * @file        copy.h
 * @author      Przemyslaw Dobrowolski (p.dobrowolsk@samsung.com)
 * @version     1.0
 * @brief       This file is the implementation file of copy
 */
#ifndef DPL_COPY_H
#define DPL_COPY_H

#include <dpl/abstract_waitable_input.h>
#include <dpl/abstract_waitable_output.h>
#include <dpl/exception.h>

namespace DPL
{
/**
 * Copy failed exception
 */
DECLARE_EXCEPTION_TYPE(Exception, CopyFailed)

/**
 * Copy all bytes abstract waitable input to abstract waitable output
 *
 * @param[in] input Abstract waitable input to copy from
 * @param[in] output Abstract waitable output to copy to
 * @throw CopyFailed An error occurred while copying. Look into exception trace for details.
 */
void Copy(AbstractWaitableInput *input, AbstractWaitableOutput *output);

/**
 * Copy exactly totalBytes bytes abstract waitable input to abstract waitable output
 *
 * @param[in] input Abstract waitable input to copy from
 * @param[in] output Abstract waitable output to copy to
 * @throw CopyFailed An error occurred while copying. Look into exception trace for details.
 */
void Copy(AbstractWaitableInput *input, AbstractWaitableOutput *output, size_t totalBytes);
} // namespace DPL

#endif // DPL_COPY_H
