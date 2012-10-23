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
 * @file       wrt_utility.cpp
 * @version    0.6
 * @author Wei Dong(d.wei@samsung.com)
 * @author Ma Quan(jason.ma@samsung.com)
 * @brief  This file implemented some common functions for widget manager
 */

#include <stdio.h>
#include <dirent.h>
#include <errno.h>
#include <libgen.h>
#include <fts.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dpl/log/log.h>
#include <dpl/utils/wrt_utility.h>

#ifndef MAX_WIDGET_PATH_LENGTH
#define MAX_WIDGET_PATH_LENGTH    1024
#endif

//will be replaced by WrtUtilJoinPaths
bool _WrtUtilSetAbsolutePath(char* absolutePath,
        const char* parentPath,
        const char* fileName)
{
    int len;
    if (NULL == absolutePath || NULL == parentPath || NULL == fileName) {
        return false;
    }
    len = strlen(parentPath);
    if (len > 0) {
        // not check the valid of parentPath and fileName.
        if (parentPath[len - 1] == '/') {
            snprintf(absolutePath,
                     MAX_WIDGET_PATH_LENGTH,
                     "%s%s",
                     parentPath,
                     fileName);
        } else {
            snprintf(absolutePath,
                     MAX_WIDGET_PATH_LENGTH,
                     "%s/%s",
                     parentPath,
                     fileName);
        }
    } else {
        LogDebug("The parent path is null");
        return false;
    }

    //some widget use Windows notation. We need to change '\' to '/'
    for (int i = 0; absolutePath[i] != 0; ++i) {
        if (absolutePath[i] == '\\') {
            absolutePath[i] = '/';
        }
    }

    return true;
}

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

// check it deeply later.
bool _WrtMakeDir (const char *path,
        long mode,
        int flags)
{
    if (NULL == path) {
        return false;
    }

    const int defaultMode = 0777;
    if (!(flags & WRT_FILEUTILS_RECUR)) {
        if (mkdir(path, defaultMode) < 0) {
            LogDebug("Failed to make dir " << path);
            return false;
        }
        if (mode != -1 && chmod(path, mode) < 0) {
            LogDebug("Failed to chmod");
            remove(path);
            return false;
        }
    } else {
        struct stat st;
        if (stat(path, &st) < 0 && errno == ENOENT) {
            bool ret;
            char *buf = NULL;
            char *parent = NULL;
            //            mode_t mask;

            //            mask = umask (0);
            //            umask (mask);

            buf = strdup(path);
            parent = dirname(buf);
            //ret = _WrtMakeDir(parent, (defaultMode & ~mask) | 0300, WRT_FILEUTILS_RECUR);
            ret = _WrtMakeDir(parent, (defaultMode) | 0300, WRT_FILEUTILS_RECUR);
            free(buf);

            if ((!ret) || (!_WrtMakeDir(path, mode, 0))) {
                LogDebug("Failed to _WrtMakeDir");
                return false;
            }
        }
    }

    return true;
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

// will be replaced with the latter function
bool _WrtUtilRemoveDir(const char* path)
{
    DIR* dir = NULL;
    struct dirent* ptr = NULL;
    char childPath[MAX_WIDGET_PATH_LENGTH + 1] = { 0 };
    if (path == NULL) {
        LogWarning("Path is null");
        return false;
    }
    dir = opendir(path);
    if (NULL != dir) {
        while ((ptr = readdir(dir)) != NULL) {
            if ((!strcmp(ptr->d_name, ".")) || (!strcmp(ptr->d_name, ".."))) {
                continue;
            }
            int len = strlen(path);
            if (path[len - 1] == '/') {
                snprintf(childPath,
                         MAX_WIDGET_PATH_LENGTH,
                         "%s%s",
                         path,
                         ptr->d_name);
            } else {
                snprintf(childPath,
                         MAX_WIDGET_PATH_LENGTH,
                         "%s/%s",
                         path,
                         ptr->d_name);
            }
            if (ptr->d_type == DT_DIR) {
                if (!_WrtUtilRemoveDir(childPath)) {
                    closedir(dir);
                    return false;
                }
            } else {
                if (unlink(childPath) != 0) {
                    closedir(dir);
                    LogWarning("Failed to remove file " << childPath);
                    return false;
                }
            }
        }
        closedir(dir);
    } else if (errno == ENOTDIR) {
        if (unlink(path) != 0) {
            LogWarning("Failed to remove file " << path);
            return false;
        }
        return true;
    } else if (errno == ENOENT) { //not exist
        LogWarning("Cannot remove non existent directory " << path);
        return true;
    } else {
        LogWarning("Can't remove directory " << path);
        return false;
    }
    if (rmdir(path) != 0) {
        LogWarning("Removing directory failed :" << path << " errno: " << errno);
        return false;
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

// shall be replaced with the latter function
bool _WrtUtilStringToLower(const char* str, char** lowerStr)
{
    if (!str || !lowerStr) {
        return true;
    }

    char* cur = NULL;
    int length = strlen(str);

    *lowerStr = (char*)malloc(length + 1);

    if (!(*lowerStr)) {
        return false;
    }

    memset(*lowerStr, 0, length + 1);
    strncpy(*lowerStr, str, length);

    cur = *lowerStr;

    while (*str != '\0') {
        *cur++ = tolower(*str++);
        //cur++;
    }

    return true;
}

void WrtUtilStringToLower(std::string &out, const std::string &in)
{
    out.clear();
    for (std::string::const_iterator it=in.begin(); it<in.end(); ++it) {
        out += static_cast<char>(tolower(*it));
    }
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

