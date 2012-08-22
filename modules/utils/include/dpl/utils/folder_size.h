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
 *
 * @file        folder_size.h
 * @author      Jaroslaw Osmanski (j.osmanski@samsung.com)
 * @version     1.0
 * @brief       Declaration for function calculating directory size
 */

#ifndef SRC_COMMON_FOLDER_SIZE_H_
#define SRC_COMMON_FOLDER_SIZE_H_

#include <string>

#include <dpl/string.h>

namespace Utils {

size_t getFolderSize(const std::string& path);

DPL::String fromFileSizeString(size_t fileSize);

}

#endif /* SRC_COMMON_FOLDER_SIZE_H_ */
