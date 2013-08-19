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
/**
 * @file    path.h
 * @author  Tomasz Iwanek (t.iwanek@samsung.com)
 * @version 1.0
 */
#ifndef PATH_H
#define PATH_H

#include <dirent.h>
#include <sys/stat.h>

#include <string>
#include <sstream>
#include <iterator>
#include <memory>
#include <vector>

#include <dpl/exception.h>
#include <dpl/string.h>

namespace DPL {
namespace Utils {
class Path;
}
}

std::ostream & operator<<(std::ostream & str, const DPL::Utils::Path & path);

namespace DPL {
namespace Utils {
/**
 * @brief The Path class path abstraction
 *
 * Class for expressing paths not limited not existing ones.
 * It's possible to check if path exists, remove it or iterate it if it's directory
 *
 * Created Path object allways contains absolute path, never relative path.
 * Simplifies common usage cases:
 * - path construction (with /= and / operators)
 * - directory iterator (begin(), end(), iterator construction)
 * - receiving filenames and directory names of given paths
 * - checking what is pointed by path (Exists(), IsFile(), IsDir())
 *
             * Check tests for details of usage.
 */
class Path
{
public:
    DECLARE_EXCEPTION_TYPE(DPL::Exception, BaseException)
    DECLARE_EXCEPTION_TYPE(BaseException, AlreadyExists)            //path already exists
    DECLARE_EXCEPTION_TYPE(BaseException, NotPrefix)                //given path is not prefix of this path
    DECLARE_EXCEPTION_TYPE(BaseException, NotExists)                //file not exists
    DECLARE_EXCEPTION_TYPE(BaseException, NotDirectory)             //directory nto exists
    DECLARE_EXCEPTION_TYPE(BaseException, OperationFailed)          //operation failed due to system error(permission etc..)
    DECLARE_EXCEPTION_TYPE(BaseException, EmptyPath)                //object cannot be constructed with empty path
    DECLARE_EXCEPTION_TYPE(BaseException, InternalError)            //internal error / wrong path usage
    DECLARE_EXCEPTION_TYPE(BaseException, CannotCopy)               //cannot make copy
    DECLARE_EXCEPTION_TYPE(BaseException, RootDirectoryError)       //operation cannot be done with root diretory

    class Iterator : public std::iterator<std::input_iterator_tag, Path>
    {
    public:
        Iterator();
        Iterator(const char *);
        Iterator& operator++();
        Iterator operator++(int);
        bool operator==(const Iterator& rhs) const;
        bool operator!=(const Iterator& rhs) const;
        const Path & operator*();
        const Path * operator->();
    private:
        void ReadNext();

        std::shared_ptr<DIR> m_dir;
        std::shared_ptr<Path> m_path;
        std::shared_ptr<Path> m_root;
    };

    explicit Path(const DPL::String & str);
    explicit Path(const std::string & str);
    explicit Path(const char * str);
    Path();

    /**
     * @brief DirectoryPath shell's dirname equivalent as path
     * @return directory path
     */
    Path DirectoryPath() const;
    /**
     * @brief DirectoryName shell's dirname equivalent
     * @return directory name of given path
     */
    std::string DirectoryName() const;
    /**
     * @brief Basename shell's basename equivalent
     * @return base name of given path
     */
    std::string Filename() const;
    /**
     * @brief Fullpath fullpath based on current working diretory
     * @return full path
     */
    std::string Fullpath() const;
    /**
     * @brief Extension
     * @return extension
     */
    std::string Extension() const;

    bool Exists() const;
    bool IsDir() const;
    bool IsFile() const;
    bool ExistsAndIsFile() const;
    bool ExistsAndIsDir() const;
    bool IsSymlink() const;
    std::size_t Size() const;
    /**
     * @brief isSubPath Returns relative path to given base
     * @param prefix base path
     * @return reltive path
     *
     * @throws If prefix does not match to this path object
     */
    bool isSubPath(const Path & other) const;
    bool hasExtension(const std::string& extension) const;

    bool operator==(const Path & other) const;
    bool operator!=(const Path & other) const;

    //appending to path
    Path operator/(const DPL::String& part) const;
    Path operator/(const std::string& part) const;
    Path operator/(const char * part) const;

    Path & operator/=(const DPL::String& part);
    Path & operator/=(const std::string& part);
    Path & operator/=(const char * part);

    //foreach
    Iterator begin() const;
    Iterator end() const;

    //root error - throws error on root directory
    void RootGuard() const;

private:

    void Append(const std::string& part);
    void Construct(const std::string & src);

    std::vector<std::string> m_parts;

    friend std::ostream & ::operator<<(std::ostream & str, const DPL::Utils::Path & path);
};

/**
 * @brief MkDir creates 'current path' as directory
 * @param path path
 * @param mode mode
 */
void MakeDir(const Path & path, mode_t mode = 0755);

/**
 * @brief MkFile creates 'current path' as empty file
 * @param path path
 */
void MakeEmptyFile(const Path & path);

/**
 * @brief Remove removes 'current path'
 * @param path path to remove
 */
void Remove(const Path & path);

/**
 * @brief TryRemove tries to remvoe path
 * @param path returns status of removal
 */
bool TryRemove(const Path & path);

/**
 * @brief Rename renames(moves) current path
 *
 * If you uses this method string to path is internally change
 * and this object will store new path not only anymore
 * @param from source path
 * @param to target path
 */
void Rename(const Path & from, const Path & to);

/**
 * @brief Exists Checks if given path exists
 * @param path path
 * @return true if path exists
 */
bool Exists(const Path & path);

/**
 * @brief Copy file
 *
 * @param from source path
 * @param to target path
 */
void CopyFile(const Path & from, const Path & to);

/**
 * @brief Copy directory recursively
 *
 * @param from source directory path
 * @param to target directory path
 */
void CopyDir(const Path & from, const Path & to);

}

}

//TODO: uncomment when user defiend literals are supported
///Path operator"" p(const char * str);

#endif // PATH_H
