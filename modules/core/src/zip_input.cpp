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
 * @file        zip_input.cpp
 * @author      Przemyslaw Dobrowolski (p.dobrowolsk@samsung.com)
 * @version     1.0
 * @brief       This file is the implementation file of zip input
 */
#include <dpl/zip_input.h>
#include <dpl/file_input_mapping.h>
#include <dpl/binary_queue.h>
#include <dpl/scoped_free.h>
#include <dpl/scoped_ptr.h>
#include <dpl/scoped_array.h>
#include <dpl/foreach.h>
#include <dpl/log/log.h>
#include <minizip/framework_minizip.h>
#include <new>

namespace DPL
{
namespace // anonymous
{
const size_t EXTRACT_BUFFER_SIZE = 4096;

class ScopedUnzClose
{
private:
    unzFile m_file;

public:
    ScopedUnzClose(unzFile file)
        : m_file(file)
    {
    }

    ~ScopedUnzClose()
    {
        if (!m_file)
            return;

        if (unzClose(m_file) != UNZ_OK)
            LogPedantic("Failed to close zip input file");
    }

    unzFile Release()
    {
        unzFile file = m_file;

        m_file = NULL;

        return file;
    }
};
} // namespace anonymous

/*
 * Seekable multiplexing device
 *
 * Explanation:
 * Minizip library lacks serious support for multithreaded
 * access to zip files. Thus, they cannot be easily extracted
 * simulateously. Here is introduced seekable device which does
 * have a context with seek index for each file. File is mapped to
 * memory and because of that no real synchronization is needed.
 * Memory addresses can be indexed.
 *
 * About generalization:
 * To achieve the same results on abstract input device, there must be
 * provided a mechanism to read data from random address without synchronization.
 * In other words: stateless. As described above, stateless property can be
 * achieved via memory mapping.
 */
class Device
{
private:
    DPL::ScopedPtr<FileInputMapping> m_fileMapping;

    struct File
    {
        off64_t offset;
        Device *device;

        File(Device *d)
            : offset(0),
              device(d)
        {
        }
    };

public:
    Device(const std::string &fileName)
    {
        Try
        {
            LogPedantic("Creating file mapping");
            m_fileMapping.Reset(new FileInputMapping(fileName));
        }
        Catch (FileInputMapping::Exception::Base)
        {
            LogPedantic("Failed to create file mapping");

            ReThrowMsg(ZipInput::Exception::OpenFailed,
                       "Failed to open zip file mapping");
        }

        LogPedantic("File mapping created");
    }

    // zlib_filefunc64_def interface: files
    static voidpf ZCALLBACK open64_file(voidpf opaque,
                                        const void* filename,
                                        int mode)
    {
        (void)filename;
        (void)mode;

        Device *device = static_cast<Device *>(opaque);

        // Open file for master device
        return new File(device);
    }

    static uLong ZCALLBACK read_file(voidpf opaque,
                                     voidpf pstream,
                                     void* buf,
                                     uLong size)
    {
        Device *device = static_cast<Device *>(opaque);
        File *deviceFile = static_cast<File *>(pstream);

        // Check if offset is out of bounds
        if (deviceFile->offset >= device->m_fileMapping->GetSize())
        {
            LogPedantic("Device: read offset out of bounds");
            return -1;
        }

        off64_t bytesLeft = device->m_fileMapping->GetSize() -
                            deviceFile->offset;

        off64_t bytesToRead;

        // Calculate bytes to read
        if (static_cast<off64_t>(size) > bytesLeft)
            bytesToRead = bytesLeft;
        else
            bytesToRead = static_cast<off64_t>(size);

        // Do copy
        memcpy(buf,
               device->m_fileMapping->GetAddress() + deviceFile->offset,
               static_cast<size_t>(bytesToRead));

        // Increment file offset
        deviceFile->offset += bytesToRead;

        // Return bytes that were actually read
        return static_cast<uLong>(bytesToRead);
    }

    static uLong ZCALLBACK write_file(voidpf opaque,
                                      voidpf stream,
                                      const void* buf,
                                      uLong size)
    {
        (void)opaque;
        (void)stream;
        (void)buf;
        (void)size;

        // Not supported by device
        LogPedantic("Unsupported function called!");
        return -1;
    }

    static int ZCALLBACK close_file(voidpf opaque,
                                    voidpf stream)
    {
        (void)opaque;
        File *deviceFile = static_cast<File *>(stream);

        // Delete file
        delete deviceFile;

        // Always OK
        return 0;
    }

    static int ZCALLBACK testerror_file(voidpf opaque,
                                        voidpf stream)
    {
        (void)opaque;
        (void)stream;

        // No errors
        return 0;
    }

    static ZPOS64_T ZCALLBACK tell64_file(voidpf opaque,
                                          voidpf stream)
    {
        (void)opaque;
        File *deviceFile = static_cast<File *>(stream);

        return static_cast<ZPOS64_T>(deviceFile->offset);
    }

    static long ZCALLBACK seek64_file(voidpf opaque,
                                      voidpf stream,
                                      ZPOS64_T offset,
                                      int origin)
    {
        Device *device = static_cast<Device *>(opaque);
        File *deviceFile = static_cast<File *>(stream);

        switch (origin)
        {
            case ZLIB_FILEFUNC_SEEK_SET:
                deviceFile->offset = static_cast<off64_t>(offset);

                break;

            case ZLIB_FILEFUNC_SEEK_CUR:
                deviceFile->offset += static_cast<off64_t>(offset);

                break;

            case ZLIB_FILEFUNC_SEEK_END:
                deviceFile->offset =
                    device->m_fileMapping->GetSize() -
                    static_cast<off64_t>(offset);

                break;

            default:
                return -1;
        }

        return 0;
    }
};

ZipInput::ZipInput(const std::string &fileName)
    : m_device(NULL),
      m_numberOfFiles(0),
      m_globalComment(),
      m_fileInfos()
{
    LogPedantic("Zip input file: " << fileName);

    // Create master device
    LogPedantic("Creating master device");
    ScopedPtr<Device> device(new Device(fileName));

    // Open master file
    zlib_filefunc64_def interface;
    interface.zopen64_file = &Device::open64_file;
    interface.zread_file = &Device::read_file;
    interface.zwrite_file = &Device::write_file;
    interface.ztell64_file = &Device::tell64_file;
    interface.zseek64_file = &Device::seek64_file;
    interface.zclose_file = &Device::close_file;
    interface.zerror_file = &Device::testerror_file;
    interface.opaque = device.Get();

    LogPedantic("Opening zip file");
    unzFile file = unzOpen2_64(NULL, &interface);

    if (file == NULL)
    {
        LogPedantic("Failed to open zip file");

         // Some errror occured
        ThrowMsg(Exception::OpenFailed,
                 "Failed to open zip file: " << fileName);
    }

    // Begin scope
    ScopedUnzClose scopedUnzClose(file);

    // Read contents
    ReadGlobalInfo(file);
    ReadGlobalComment(file);
    ReadInfos(file);

    // Release scoped unz close
    m_masterFile = scopedUnzClose.Release();
    m_device = device.Release();

    LogPedantic("Zip file opened");
}

ZipInput::~ZipInput()
{
    // Close zip
    if (unzClose(static_cast<unzFile>(m_masterFile)) != UNZ_OK)
        LogPedantic("Failed to close zip input file");

    // Close device
    delete m_device;
}

void ZipInput::ReadGlobalInfo(void *masterFile)
{
    // Read number of entries and global comment
    unz_global_info globalInfo;

    if (unzGetGlobalInfo(static_cast<unzFile>(masterFile),
                         &globalInfo) != UNZ_OK)
    {
        LogPedantic("Failed to read zip global info");

        ThrowMsg(Exception::ReadGlobalInfoFailed,
                 "Failed to read global info");
    }

    m_numberOfFiles = static_cast<size_t>(globalInfo.number_entry);
    m_globalCommentSize = static_cast<size_t>(globalInfo.size_comment);

    LogPedantic("Number of files: " << m_numberOfFiles);
    LogPedantic("Global comment size: " << m_globalCommentSize);
}

void ZipInput::ReadGlobalComment(void *masterFile)
{
    ScopedArray<char> comment(new char[m_globalCommentSize + 1]);

    if (unzGetGlobalComment(static_cast<unzFile>(masterFile),
                            comment.Get(),
                            m_globalCommentSize + 1) != UNZ_OK)
    {
        LogPedantic("Failed to read zip global comment");

        ThrowMsg(Exception::ReadGlobalCommentFailed,
                 "Failed to read global comment");
    }

    m_globalComment = comment.Get();
    LogPedantic("Global comment: " << m_globalComment);
}

void ZipInput::ReadInfos(void *masterFile)
{
    // Read infos
    m_fileInfos.reserve(m_numberOfFiles);

    if (unzGoToFirstFile(static_cast<unzFile>(masterFile)) != UNZ_OK)
    {
        LogPedantic("Failed to go to first file");
        ThrowMsg(Exception::SeekFileFailed, "Failed to seek first file");
    }

    for (size_t i = 0; i < m_numberOfFiles; ++i)
    {
        unz_file_pos_s filePos;

        if (unzGetFilePos(static_cast<unzFile>(masterFile),
                          &filePos) != UNZ_OK)
        {
            LogPedantic("Failed to get file pos");
            ThrowMsg(Exception::FileInfoFailed, "Failed to get zip file info");
        }

        unz_file_info64 fileInfo;

        if (unzGetCurrentFileInfo64(static_cast<unzFile>(masterFile),
                                    &fileInfo,
                                    NULL,
                                    0,
                                    NULL,
                                    0,
                                    NULL,
                                    0) != UNZ_OK)
        {
            LogPedantic("Failed to get file pos");
            ThrowMsg(Exception::FileInfoFailed, "Failed to get zip file info");
        }

        ScopedArray<char> fileName(new char[fileInfo.size_filename + 1]);
        ScopedArray<char> fileComment(new char[fileInfo.size_file_comment + 1]);

        if (unzGetCurrentFileInfo64(static_cast<unzFile>(masterFile),
                                    &fileInfo,
                                    fileName.Get(),
                                    fileInfo.size_filename + 1,
                                    NULL,
                                    0,
                                    fileComment.Get(),
                                    fileInfo.size_file_comment + 1) != UNZ_OK)
        {
            LogPedantic("Failed to get file pos");
            ThrowMsg(Exception::FileInfoFailed, "Failed to get zip file info");
        }

        m_fileInfos.push_back(
            FileInfo(
                FileHandle(
                    static_cast<size_t>(filePos.pos_in_zip_directory),
                    static_cast<size_t>(filePos.num_of_file)
                ),
                std::string(fileName.Get()),
                std::string(fileComment.Get()),
                static_cast<unsigned long>(fileInfo.version),
                static_cast<unsigned long>(fileInfo.version_needed),
                static_cast<unsigned long>(fileInfo.flag),
                static_cast<unsigned long>(fileInfo.compression_method),
                static_cast<unsigned long>(fileInfo.dosDate),
                static_cast<unsigned long>(fileInfo.crc),
                static_cast<off64_t>(fileInfo.compressed_size),
                static_cast<off64_t>(fileInfo.uncompressed_size),
                static_cast<unsigned long>(fileInfo.disk_num_start),
                static_cast<unsigned long>(fileInfo.internal_fa),
                static_cast<unsigned long>(fileInfo.external_fa),
                FileDateTime(
                    fileInfo.tmu_date.tm_sec,
                    fileInfo.tmu_date.tm_min,
                    fileInfo.tmu_date.tm_hour,
                    fileInfo.tmu_date.tm_mday,
                    fileInfo.tmu_date.tm_mon,
                    fileInfo.tmu_date.tm_year
                )
            )
        );

        // If this is not the last file, go to next one
        if (i != m_numberOfFiles - 1)
        {
            if (unzGoToNextFile(
                    static_cast<unzFile>(masterFile))!= UNZ_OK)
            {
                LogPedantic("Failed to go to next file");

                ThrowMsg(Exception::FileInfoFailed,
                         "Failed to go to next file");
            }
        }
    }
}

ZipInput::const_iterator ZipInput::begin() const
{
    return m_fileInfos.begin();
}

ZipInput::const_iterator ZipInput::end() const
{
    return m_fileInfos.end();
}

ZipInput::const_reverse_iterator ZipInput::rbegin() const
{
    return m_fileInfos.rbegin();
}

ZipInput::const_reverse_iterator ZipInput::rend() const
{
    return m_fileInfos.rend();
}

ZipInput::size_type ZipInput::size() const
{
    return m_fileInfos.size();
}

ZipInput::File *ZipInput::OpenFile(FileHandle handle)
{
    return new File(m_device, handle);
}

ZipInput::File *ZipInput::OpenFile(const std::string &fileName)
{
    FOREACH(iterator, m_fileInfos)
    {
        if (iterator->name == fileName)
        {
            return new File(m_device, iterator->handle);
        }
    }

    ThrowMsg(Exception::OpenFileFailed,
             "Failed to open zip file: " << fileName);
}

ZipInput::File::File(class Device *device, FileHandle handle)
{
    // Open file file
    zlib_filefunc64_def interface;
    interface.zopen64_file = &Device::open64_file;
    interface.zread_file = &Device::read_file;
    interface.zwrite_file = &Device::write_file;
    interface.ztell64_file = &Device::tell64_file;
    interface.zseek64_file = &Device::seek64_file;
    interface.zclose_file = &Device::close_file;
    interface.zerror_file = &Device::testerror_file;
    interface.opaque = device;

    LogPedantic("Opening zip file");
    unzFile file = unzOpen2_64(NULL, &interface);

    if (file == NULL)
    {
        LogPedantic("Failed to open zip file");

         // Some errror occured
        ThrowMsg(ZipInput::Exception::OpenFileFailed,
                 "Failed to open zip file");
    }

    // Begin scope
    ScopedUnzClose scopedUnzClose(file);

    // Look up file handle
    unz64_file_pos filePos =
    {
        static_cast<ZPOS64_T>(handle.first),
        static_cast<ZPOS64_T>(handle.second)
    };

    if (unzGoToFilePos64(file, &filePos) != UNZ_OK)
    {
        LogPedantic("Failed to seek to zip file");

         // Some errror occured
        ThrowMsg(ZipInput::Exception::OpenFileFailed,
                 "Failed to open zip file");
    }

    // Open current file for reading
    if (unzOpenCurrentFile(file) != UNZ_OK)
    {
        LogPedantic("Failed to open current zip file");

         // Some errror occured
        ThrowMsg(ZipInput::Exception::OpenFileFailed,
                 "Failed to open current zip file");
    }

    // Release scoped unz close
    m_file = scopedUnzClose.Release();

    LogPedantic("Zip file opened");
}

ZipInput::File::~File()
{
    // Close current file for reading
    if (unzCloseCurrentFile(static_cast<unzFile>(m_file)) != UNZ_OK)
        LogPedantic("Failed to close current zip input file");

    // Close zip file
    if (unzClose(static_cast<unzFile>(m_file)) != UNZ_OK)
        LogPedantic("Failed to close zip input file");
}

DPL::BinaryQueueAutoPtr ZipInput::File::Read(size_t size)
{
    // Do not even try to unzip if requested zero bytes
    if (size == 0)
        return DPL::BinaryQueueAutoPtr(new DPL::BinaryQueue());

    // Calc data to read
    size_t sizeToRead = size > EXTRACT_BUFFER_SIZE ?
                        EXTRACT_BUFFER_SIZE :
                        size;

    // Extract zip file data (one-copy)
    ScopedFree<void> rawBuffer(malloc(sizeToRead));

    if (!rawBuffer)
        throw std::bad_alloc();

    // Do unpack
    int bytes = unzReadCurrentFile(static_cast<unzFile>(m_file),
                                   rawBuffer.Get(),
                                   sizeToRead);

    // Internal unzipper error
    if (bytes < 0)
    {
        LogPedantic("Extract failed. Error: " << bytes);

        ThrowMsg(ZipInput::Exception::ReadFileFailed,
                 "Failed to extract file with error: " << bytes);
    }

    // Data was read (may be zero bytes)
    DPL::BinaryQueueAutoPtr buffer(new DPL::BinaryQueue());

    buffer->AppendUnmanaged(rawBuffer.Get(),
                            static_cast<size_t>(bytes),
                            &DPL::BinaryQueue::BufferDeleterFree,
                            NULL);

    rawBuffer.Release();

    return buffer;
}

const std::string &ZipInput::GetGlobalComment() const
{
    return m_globalComment;
}

bool ZipInput::empty() const
{
    return m_fileInfos.empty();
}
} // namespace DPL
