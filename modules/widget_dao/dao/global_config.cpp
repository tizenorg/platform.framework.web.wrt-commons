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
#include <dpl/wrt-dao-ro/global_config.h>

#include <cstdlib>
#include <cstring>

namespace WrtDB {
namespace GlobalConfig {
bool IsOCSPEnabled()
{
    static const char *val = getenv("WRT_OCSP_ENABLE");
    static bool enableOCSP = val && (strcmp(val, "1") == 0);
    return enableOCSP;
}

bool IsCRLEnabled()
{
    static const char *val = getenv("WRT_CRL_ENABLE");
    static bool enableOCSP = val && (strcmp(val, "1") == 0);
    return enableOCSP;
}

} // namespace GlobalConfig
} // namespace WrtDB

