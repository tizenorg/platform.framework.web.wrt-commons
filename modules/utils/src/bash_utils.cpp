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
 * @file    bash_utils.cpp
 * @author  Iwanek Tomasz
 * @version 1.0
 */

#include<dpl/utils/bash_utils.h>
#include<dpl/log/log.h>

#include<string>

namespace BashUtils {

std::string escape_arg(const std::string & source)
{
    static const std::string special("!$`\\\"");
    std::string ret = "\"";
    for(std::string::const_iterator iter = source.begin(); iter != source.end(); ++iter)
    {
        if(special.find(*iter) != std::string::npos)
        {
            ret += std::string("\\") + *iter;
        }
        else
        {
            ret += *iter;
        }
    }
    return ret + "\"";
}

}
