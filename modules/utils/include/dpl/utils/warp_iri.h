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
#ifndef _WARPIRI_H_
#define _WARPIRI_H_

#include <vector>

#include <dpl/string.h>
#include <dpl/log/log.h>

class WarpIRI
{
    static const unsigned int UNKNOWN_PORT = 0;
  public:
    WarpIRI();

    void set(const char *iri,
            bool domain);
    void set(const DPL::String &iristring,
            bool domain);

    /* It also checks port and schema */
    bool isSubDomain(const WarpIRI &second) const;
    bool isAccessDefinition() const;
    bool getSubDomain() const;

    static bool isIRISchemaIgnored(const char *iri);

    bool operator ==(const WarpIRI &other) const
    {
        return m_domain == other.m_domain &&
               m_host == other.m_host &&
               m_schema == other.m_schema &&
               m_port == other.m_port &&
               m_isAccessDefinition == other.m_isAccessDefinition &&
               m_isIRIValid == other.m_isIRIValid;
    }

  private:
    unsigned int getPort(const DPL::String &schema) const;

    bool m_domain;
    std::vector<DPL::String> m_host;
    DPL::String m_schema;
    unsigned int m_port;
    bool m_isAccessDefinition;
    bool m_isIRIValid;
};

#endif // _WarpIRI_H_
