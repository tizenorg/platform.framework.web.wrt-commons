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
 * @file    bash_utils.h
 * @author  Iwanek Tomasz
 * @version 1.0
 */

#ifndef BASH_UTILS_H
#define BASH_UTILS_H

#include <string>

namespace BashUtils {
/**
 * Escapes bash special characters in string and return string in double quotes
 * @param source string to be escaped
 * @return escaped string
 */
std::string escape_arg(const std::string & source);
}

#endif // BASH_UTILS_H
