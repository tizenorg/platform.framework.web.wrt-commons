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
#ifndef MIME_TYPE_UTILS_H
#define MIME_TYPE_UTILS_H

#include <dpl/string.h>
#include <dpl/optional_typedefs.h>

class MimeTypeUtils
{
  public:
    class Exception
    {
      public:
        DECLARE_EXCEPTION_TYPE(DPL::Exception, Base)
        DECLARE_EXCEPTION_TYPE(Base, InvalidFileName)
    };

  private:
    //TODO use hash_map if possible
    static const std::set<DPL::String>& getMimeTypesSupportedForIcon();
    static const std::set<DPL::String>& getMimeTypesSupportedForStartFile();

    typedef std::map<DPL::String, DPL::String> FileIdentificationMap;

    static DPL::String getFileNameFromPath(const DPL::String& path);
    static const FileIdentificationMap& getFileIdentificationMap();
    static DPL::String stripMimeParameters(const DPL::String& mimeType);

  public:
    typedef std::map<DPL::String, DPL::String> MimeAttributes;
    static bool isValidIcon(const DPL::String& path);
    static bool isValidStartFile(const DPL::String& path,
                                 const DPL::OptionalString& providedMimeType);
    static bool isMimeTypeSupportedForStartFile(const DPL::String& mimeType);
    static bool isMimeTypeSupportedForIcon(const DPL::String& mimeType);
    static MimeAttributes getMimeAttributes(const DPL::String& mimeType);
    ///implements 9.1.10. (Rule for Identifying the Media Type of a File)
    ///from W3C packaging specification
    static DPL::String identifyFileMimeType(const DPL::String& path);
};

#endif  /* MIME_TYPE_UTILS_H */

