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
 * @file        zip_input.h
 * @author      Przemyslaw Dobrowolski (p.dobrowolsk@samsung.com)
 * @version     1.0
 * @brief       This file is the header file of zip input
 */
#ifndef DPL_ZIP_INPUT_H
#define DPL_ZIP_INPUT_H

#include <dpl/exception.h>
#include <dpl/noncopyable.h>
#include <dpl/abstract_input.h>
#include <utility>
#include <vector>
#include <string>

namespace DPL
{
class ZipInput
    : private Noncopyable
{
public:
    class Exception
    {
    public:
        DECLARE_EXCEPTION_TYPE(DPL::Exception, Base)
        DECLARE_EXCEPTION_TYPE(Base, OpenFailed)
        DECLARE_EXCEPTION_TYPE(Base, ReadGlobalInfoFailed)
        DECLARE_EXCEPTION_TYPE(Base, ReadGlobalCommentFailed)
        DECLARE_EXCEPTION_TYPE(Base, SeekFileFailed)
        DECLARE_EXCEPTION_TYPE(Base, FileInfoFailed)
        DECLARE_EXCEPTION_TYPE(Base, OpenFileFailed)
        DECLARE_EXCEPTION_TYPE(Base, ReadFileFailed)
    };

    typedef std::pair<size_t, size_t> FileHandle;

    struct FileDateTime
    {
        unsigned int second; //< seconds after the minute - [0,59]
        unsigned int minute; //< minutes after the hour - [0,59]
        unsigned int hour;   //< hours since midnight - [0,23]
        unsigned int day;    //< day of the month - [1,31]
        unsigned int month;  //< months since January - [0,11]
        unsigned int year;   //< years - [1980..2044]

        FileDateTime()
            :   second(0),
                minute(0),
                hour(0),
                day(0),
                month(0),
                year(0)
        {
        }

        FileDateTime(unsigned int secondArg,
                     unsigned int minuteArg,
                     unsigned int hourArg,
                     unsigned int dayArg,
                     unsigned int monthArg,
                     unsigned int yearArg)
            :   second(secondArg),
                minute(minuteArg),
                hour(hourArg),
                day(dayArg),
                month(monthArg),
                year(yearArg)
        {
        }
    };

    struct FileInfo
    {
        // File handle
        FileHandle handle;

        // File name and comment
        std::string name;
        std::string comment;

        // File information
        unsigned long version;                //< version made by
        unsigned long versionNeeded;          //< version needed to extract
        unsigned long flag;                   //< general purpose bit flag
        unsigned long compressionMethod;      //< compression method
        unsigned long dosDate;                //< last mod file date in Dos fmt
        unsigned long crc;                    //< crc-32
        off64_t       compressedSize;         //< compressed size
        off64_t       uncompressedSize;       //< uncompressed size
        unsigned long diskNumberStart;        //< disk number start
        unsigned long internalFileAttributes; //< internal file attributes
        unsigned long externalFileAttributes; //< external file attributes

        FileDateTime dateTime;

        FileInfo()
            : handle(),
              name(),
              comment(),
              version(0),
              versionNeeded(0),
              flag(0),
              compressionMethod(0),
              dosDate(0),
              crc(0),
              compressedSize(0),
              uncompressedSize(0),
              diskNumberStart(0),
              internalFileAttributes(0),
              externalFileAttributes(0),
              dateTime()
        {
        }

        FileInfo(const FileHandle &handleArg,
                 const std::string &nameArg,
                 const std::string &commentArg,
                 unsigned long versionArg,
                 unsigned long versionNeededArg,
                 unsigned long flagArg,
                 unsigned long compressionMethodArg,
                 unsigned long dosDateArg,
                 unsigned long crcArg,
                 const off64_t &compressedSizeArg,
                 const off64_t &uncompressedSizeArg,
                 unsigned long diskNumberStartArg,
                 unsigned long internalFileAttributesArg,
                 unsigned long externalFileAttributesArg,
                 const FileDateTime &dateTimeArg)
            : handle(handleArg),
              name(nameArg),
              comment(commentArg),
              version(versionArg),
              versionNeeded(versionNeededArg),
              flag(flagArg),
              compressionMethod(compressionMethodArg),
              dosDate(dosDateArg),
              crc(crcArg),
              compressedSize(compressedSizeArg),
              uncompressedSize(uncompressedSizeArg),
              diskNumberStart(diskNumberStartArg),
              internalFileAttributes(internalFileAttributesArg),
              externalFileAttributes(externalFileAttributesArg),
              dateTime(dateTimeArg)
        {
        }
    };

    class File
        : public DPL::AbstractInput
    {
    private:
        void *m_file;

        friend class ZipInput;
        File(class Device *device, FileHandle handle);

    public:
        ~File();

        virtual DPL::BinaryQueueAutoPtr Read(size_t size);
    };

private:
    class Device *m_device;
    void *m_masterFile;

    size_t m_numberOfFiles;
    size_t m_globalCommentSize;
    std::string m_globalComment;

    // At least cache handles
    typedef std::vector<FileInfo> FileInfoList;
    FileInfoList m_fileInfos;

    void ReadGlobalInfo(void *masterFile);
    void ReadGlobalComment(void *masterFile);
    void ReadInfos(void *masterFile);

public:
    typedef FileInfoList::const_iterator const_iterator;
    typedef FileInfoList::const_reverse_iterator const_reverse_iterator;
    typedef FileInfoList::size_type size_type;

public:
    /**
     * Open zip file from file
     */
    explicit ZipInput(const std::string &fileName);

    /**
     * Destructor
     */
    ~ZipInput();

    // Iterators
    const_iterator begin() const;
    const_iterator end() const;

    const_reverse_iterator rbegin() const;
    const_reverse_iterator rend() const;

    // Size, empty
    size_type size() const;
    bool empty() const;

    /**
     * Open a binary file for given file handle
     *
     * @return file object
     * @param[in] handle Zip file handle to open
     * @exception std::bad_alloc Cannot allocate memory to hold additional data
     * @exception SteamOpenFailed Cannot find file with given handle
     * @see BinaryQueue::BufferDeleterFree
     */
    File *OpenFile(FileHandle handle);

    /**
     * Open a binary file for given file name
     *
     * @return file object
     * @param[in] fileName Zip file name to open
     * @exception std::bad_alloc Cannot allocate memory to hold additional data
     * @exception SteamOpenFailed Cannot find file with given handle
     * @see BinaryQueue::BufferDeleterFree
     */
    File *OpenFile(const std::string &fileName);

    /**
     * Get archive global comment
     *
     * @return Global archive comment
     */
    const std::string &GetGlobalComment() const;
};
} // namespace DPL

#endif // DPL_ZIP_INPUT_H
