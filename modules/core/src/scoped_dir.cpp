/*
 * Copyright (c) 2013 Samsung Electronics Co., Ltd All Rights Reserved
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
 * @file        scoped_dir.cpp
 * @author      Iwanek Tomasz (t.iwanek@samsung.com)
 * @version     1.0
 * @brief       This file is the implementation scoped directory
 */
#include <dpl/scoped_dir.h>
#include <dpl/errno_string.h>
#include <dpl/log/log.h>

#include <fts.h>
#include <sys/types.h>
#include <unistd.h>

namespace {

bool removeRecusive(const char * path)
{
    FTS *fts;
    FTSENT *ftsent;
    bool rv = true;
    char * const paths[] = { const_cast<char * const>(path), NULL };
    if ((fts = fts_open(paths, FTS_PHYSICAL | FTS_NOCHDIR, NULL)) == NULL) {
        return false;
    }
    while ((ftsent = fts_read(fts)) != NULL) {
        switch (ftsent->fts_info) {
        case FTS_D:
            break;
        case FTS_DP:
            if (rmdir(ftsent->fts_accpath) != 0) {
                rv = false;
            }
            break;
        case FTS_DC:
        case FTS_F:
        case FTS_NSOK:
        case FTS_SL:
        case FTS_SLNONE:
        case FTS_DEFAULT:
            if (unlink(ftsent->fts_accpath) != 0) {
                rv = false;
            }
            break;
        case FTS_NS:
            rv = false;
            break;
        case FTS_DOT:
        case FTS_DNR:
        case FTS_ERR:
        default:
            rv = false;
            break;
        }
    }
    if (fts_close(fts) == -1) {
        rv = false;
    }
    return rv;
}

}

namespace DPL {

ScopedDirPolicy::Type ScopedDirPolicy::NullValue()
{
    return std::string();
}

void ScopedDirPolicy::Destroy(Type str)
{
    if(!str.empty())
    {
        bool status = removeRecusive(str.c_str());
        if(!status)
        {
            LogError("Error while removing recursively: " << str);
        }
    }
}

ScopedDir::ScopedDir(const std::string & str, mode_t mode) : BaseType(str)
{
    if(!str.empty())
    {
        if (mkdir(str.c_str(), mode) == -1)
        {
            std::string errstr = DPL::GetErrnoString();
            LogError("Error while creating directory: " << str
                     << " [" << errstr << "]");
        }
    }
}

} // namespace DPL

