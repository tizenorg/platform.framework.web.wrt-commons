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
/**
 * @file        wrt_utility.cpp
 * @version     0.8
 * @author      Janusz Majnert <j.majnert@samsung.com>
 * @brief       Implementation of some common utility functions
 */
#include <stddef.h>
#include <fts.h>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <ctype.h>
#include <dpl/log/log.h>
#include <dpl/utils/wrt_utility.h>

void WrtUtilJoinPaths(std::string &joined, const std::string &parent, const std::string &child)
{
    size_t parent_len = parent.length();;
    joined=parent;
    joined+=child;
    //In case someone used windows-style paths
    std::replace(joined.begin(), joined.end(), '\\', '/');

    if (parent_len != 0 && child.length() != 0) {
        if (joined[parent_len-1] != '/' && joined[parent_len] != '/')
            joined.insert(parent_len, "/");
        else if (joined[parent_len-1] == '/' && joined[parent_len] == '/')
            joined.erase(parent_len, 1);
    }
}

bool WrtUtilMakeDir(const std::string &newpath, mode_t mode)
{
    size_t pos = 0;
    int error;

    if (newpath.length() == 0) return false;

    std::string path=newpath;

    if (*(path.rbegin()) != '/') path += '/';

    while ((pos = path.find('/', pos+1)) != std::string::npos) {
        if (mkdir(path.substr(0, pos).c_str(), mode) != 0) {
            error=errno;
            if (error == EEXIST) continue;
            LogWarning(__PRETTY_FUNCTION__ << ": failed to create directory "
                        << path.substr(0,pos)
                        << ". Error: "
                        << strerror(error));
            return false;
        }
    }
    return true;
}

bool WrtUtilRemove(const std::string &path)
{
    FTS *fts;
    FTSENT *ftsent;
    bool rv = true;
    int error = 0;
    char * const paths[] = {const_cast<char * const>(path.c_str()), NULL};

    if ((fts = fts_open(paths, FTS_PHYSICAL|FTS_NOCHDIR, NULL)) == NULL) {
        //ERROR
        error = errno;
        LogWarning(__PRETTY_FUNCTION__ << ": fts_open failed with error: "
                    << strerror(error));
        return false;
    }

    while ((ftsent = fts_read(fts)) != NULL) {
        switch (ftsent->fts_info) {
            case FTS_D:
                //directory in preorder - do nothing
                break;
            case FTS_DP:
                //directory in postorder - remove
                if (rmdir(ftsent->fts_accpath) != 0) {
                    error = errno;
                    LogWarning(__PRETTY_FUNCTION__
                                << ": rmdir failed with error: "
                                << strerror(error));
                    rv = false;
                }
                break;
            case FTS_DC:
            case FTS_F:
            case FTS_NSOK:
            case FTS_SL:
            case FTS_SLNONE:
            case FTS_DEFAULT:
                //regular files and other objects that can safely be removed
                if (unlink(ftsent->fts_accpath) != 0) {
                    error = errno;
                    LogWarning(__PRETTY_FUNCTION__
                                << ": unlink failed with error: "
                                << strerror(error));
                    rv = false;
                }
                break;
            case FTS_NS:
                LogWarning(__PRETTY_FUNCTION__
                            << ": couldn't get stat info for file: "
                            << ftsent->fts_path
                            << ". The error was: "
                            << strerror(ftsent->fts_errno));
                rv = false;
                break;
            case FTS_DOT:
            case FTS_DNR:
            case FTS_ERR:
            default:
                LogWarning(__PRETTY_FUNCTION__
                            << ": traversal failed with error: "
                            << strerror(ftsent->fts_errno));
                rv = false;
                break;
        }
    }

    if (fts_close(fts) == -1) {
        error = errno;
        LogWarning(__PRETTY_FUNCTION__ << ": fts_close failed with error: "
                    << strerror(error));
        rv = false;
    }
    return rv;
}

bool WrtUtilFileExists(const std::string &path)
{
    struct stat tmp;
    if (stat(path.c_str(),&tmp) == 0) {
        return S_ISREG(tmp.st_mode);
    } else {
        return false;
    }
}

bool WrtUtilDirExists(const std::string &path)
{
    struct stat tmp;
    if (stat(path.c_str(),&tmp) == 0) {
        return S_ISDIR(tmp.st_mode);
    } else {
        return false;
    }
}

