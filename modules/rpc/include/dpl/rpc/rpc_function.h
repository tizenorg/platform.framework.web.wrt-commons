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
 * @file        rpc_function.h
 * @author      Przemyslaw Dobrowolski (p.dobrowolsk@samsung.com)
 * @version     1.0
 * @brief       This file is the header file for RPC function
 */
#ifndef DPL_RPC_FUNCTION_H
#define DPL_RPC_FUNCTION_H

#include <dpl/exception.h>
#include <dpl/binary_queue.h>
#include <dpl/scoped_array.h>
#include <dpl/string.h>
#include <dpl/serialization.h>
#include <string>

namespace DPL
{
namespace RPC
{

class RPCFunction : public IStream
{
protected:
    BinaryQueue m_buffer; ///< Serialized RPC function call as a binary queue

public:
    class Exception
    {
    public:
        DECLARE_EXCEPTION_TYPE(DPL::Exception, Base)
        DECLARE_EXCEPTION_TYPE(Base, ParseFailed)
    };

    /**
     * Constructor
     */
    RPCFunction()
    {
    }

    /**
     * Constructor
     *
     * @param buffer Binary queue to copy initialization data from
     */
    RPCFunction(const BinaryQueue &buffer)
    {
        m_buffer.AppendCopyFrom(buffer);
    }

    /**
     * Destructor
     */
    virtual ~RPCFunction()
    {
    }

    /**
     * Append argument to call
     *
     * @param[in] arg Template based argument to append
     * @return none
     * @warning Carefully add any pointers to buffer because of template nature of this method
     */
    template<typename Type>
    void AppendArg(const Type &arg)
    {
        size_t argSize = sizeof(arg);
        m_buffer.AppendCopy(&argSize, sizeof(argSize));
        m_buffer.AppendCopy(&arg, sizeof(arg));
    }

    /**
     * Append @a std::string argument to call
     *
     * @param[in] arg String to append to function call
     * @return none
     */
    void AppendArg(const std::string &arg)
    {
        size_t argSize = arg.size();
        m_buffer.AppendCopy(&argSize, sizeof(argSize));
        m_buffer.AppendCopy(arg.c_str(), argSize);
    }

    /**
     * Append @a DPL::String argument to call
     *
     * @param[in] arg String to append to function call
     * @return none
     */
    void AppendArg(const String &arg)
    {
        std::string localStdString = ToUTF8String(arg);
        AppendArg(localStdString);
    }

    /**
     * Consume argument from call. Arguments are retrieved in non-reversed order
     * (same as they were pushed onto RPC function argument stack)
     *
     * @param[out] arg Reference to output template based argument
     * @warning Carefully add any pointers to buffer because of template nature of this method
     * @return none
     */
    template<typename Type>
    void ConsumeArg(Type &arg)
    {
        Try
        {
            size_t argSize = sizeof(arg);
            m_buffer.FlattenConsume(&argSize, sizeof(argSize));

            if (argSize != sizeof(arg))
                ThrowMsg(Exception::ParseFailed, "Stream parse CRC failed");

            m_buffer.FlattenConsume(&arg, sizeof(arg));
        }
        Catch (BinaryQueue::Exception::OutOfData)
        {
            ReThrowMsg(Exception::ParseFailed, "Unexpected end of stream");
        }
    }

    /**
     * Consume @a std::string argument from call. Arguments are retrieved in non-reversed order
     * (same as they were pushed onto RPC function argument stack)
     *
     * @param[out] arg Reference to output @a std::string argument
     * @return none
     */
    void ConsumeArg(std::string &arg)
    {
        Try
        {
            std::string::size_type size;
            m_buffer.FlattenConsume(&size, sizeof(size));
            ScopedArray<char> str(new char[size]);
            m_buffer.FlattenConsume(str.Get(), size);
            arg = std::string(str.Get(), str.Get() + size);
        }
        Catch (BinaryQueue::Exception::OutOfData)
        {
            ReThrowMsg(Exception::ParseFailed, "Unexpected end of stream");
        }
    }

    /**
     * Consume @a DPL::String argument from call. Arguments are converted to UTF-8 string
     *
     * @param[out] arg Reference to output @a DPL::String argument
     * @return none
     */
    void ConsumeArg(String &arg)
    {
        std::string consumedStdString;
        ConsumeArg(consumedStdString);
        arg = FromUTF8String(consumedStdString);
    }

    /**
     * Serialize all function parameters into single binary queue
     *
     * @return Serialized binary queue representation of RPC function
     */
    BinaryQueue Serialize() const
    {
        return m_buffer;
    }

    /**
     * Reads binary data from serialized stream
     *
     * @param num number of bytes to read
     * @param bytes buffer for read data
     */
    virtual void Read(size_t num, void * bytes)
    {
        m_buffer.FlattenConsume(bytes, num);
    }

    /**
     * Writes binary data to serialized stream
     *
     * @param num number of bytes to write to serialization buffer
     * @param bytes buffer for data to write
     */
    virtual void Write(size_t num, const void * bytes)
    {
        m_buffer.AppendCopy(bytes, num);
    }
};

}
} // namespace DPL

#endif // DPL_RPC_FUNCTION_H
