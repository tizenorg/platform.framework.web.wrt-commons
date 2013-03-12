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
 * This file  have been implemented in compliance with  W3C WARP SPEC.
 * but there are some patent issue between  W3C WARP SPEC and APPLE.
 * so if you want to use this file, refer to the README file in root directory
 */
#include <stddef.h>
#include <list>
#include <set>
#include <string>
#include <dpl/utils/warp_iri.h>
#include <dpl/string.h>
#include <dpl/auto_ptr.h>
#include <dpl/foreach.h>
#include <idna.h>
#include <istream>
#include <iri.h>
//#include <ValidatorCommon.h>

namespace {
// All schemes which are supported by external application should be ignored
// by WARP engine.
//
// Warp specification require from iri to have host element. File protocol
// does not contain host element so it's always denied by warp.
// Unfortunatly all widgets are using file protocol to load its data from
// hard drive. What's why we cannot check any iri with file schema.

const char *IRI_IGNORED_SCHEME[] = { "file://", "widget://", "tel:", "sms:",
                                    "mmsto:", "mailto:", "data:", "blob:", 0 };

const DPL::String SCHEMA_HTTP = DPL::FromUTF8String("http");
const DPL::String SCHEMA_HTTPS = DPL::FromUTF8String("https");
const DPL::String SCHEMA_FTP = DPL::FromUTF8String("ftp");
} // namespace anonymous

// This will create AutoPtr deleter for iri_struct.
// Deleter must be in the same namespace as definition of AutoPtr.

namespace DPL{
DECLARE_DELETER(iri_struct, iri_destroy)
} // namespace DPL

WarpIRI::WarpIRI() :
    m_domain(false),
    m_port(UNKNOWN_PORT),
    m_isAccessDefinition(false),
    m_isIRIValid(false)
{
}

void WarpIRI::set(const char *p_iri,
        bool domain)
{
    if (!p_iri) {
        m_isAccessDefinition = m_isIRIValid = false;
        return;
    }

    m_domain = domain;
    m_isAccessDefinition = true;
    m_isIRIValid = true;
    m_host.clear();

    if (strcmp(p_iri, "*") == 0) {
        return;
    }

    DPL::AutoPtr<iri_struct> iri(iri_parse(p_iri));

    if (!iri.get()) {
        LogError("Error in iri_parse!");
        m_isIRIValid = false;
        m_isAccessDefinition = false;
        return;
    }

    if (iri->scheme == NULL || iri->host == NULL) {
        m_isIRIValid = false;
        m_isAccessDefinition = false;
        return;
    }

    // all of this must be NULL in WARP definition
    if (iri->user || iri->path || iri->query || iri->anchor) {
        m_isAccessDefinition = false;
    }

    m_schema = DPL::FromASCIIString(std::string(iri->scheme));
    m_port = static_cast<unsigned int>(iri->port);

    if (m_port == 0) {
        m_port = getPort(m_schema);
        if (m_port == UNKNOWN_PORT) {
            m_isAccessDefinition = false;
            return;
        }
    }

    DPL::String str = DPL::FromASCIIString(std::string(iri->host));

    std::string utf8host = iri->host;
    std::list<std::string> hostTokenList;
    DPL::Tokenize(utf8host, ".", std::front_inserter(hostTokenList));

    if (SCHEMA_HTTP == m_schema || SCHEMA_HTTPS == m_schema) {
        FOREACH(i, hostTokenList) {
            char *output = NULL;
            int rc = idna_to_ascii_8z(i->c_str(),
                                      &output,
                                      IDNA_USE_STD3_ASCII_RULES);

            if (IDNA_SUCCESS != rc) {
                LogWarning("libidn error: " << rc << " " <<
                           idna_strerror((Idna_rc)rc));
                m_isIRIValid = false;
                m_isAccessDefinition = false;
            } else {
                std::string token(output);
                std::transform(token.begin(),
                               token.end(),
                               token.begin(),
                                   ::tolower);
                m_host.push_back(DPL::FromUTF8String(token));
            }
            free(output);
        }
    } else {
        FOREACH(i, hostTokenList){
            m_host.push_back(DPL::FromUTF8String(*i));
        }
    }
}

void WarpIRI::set(const DPL::String &iristring,
        bool domain)
{
    set(DPL::ToUTF8String(iristring).c_str(), domain);
}

unsigned int WarpIRI::getPort(const DPL::String &schema) const
{
    unsigned int port = UNKNOWN_PORT;
    if (schema == SCHEMA_HTTP) {
        port = 80;
    } else if (schema == SCHEMA_HTTPS) {
        port = 443;
    } else if (schema == SCHEMA_FTP) {
        port = 21;
    }
    return port;
}

bool WarpIRI::isSubDomain(const WarpIRI &second) const
{
    if (!m_isAccessDefinition || !second.m_isIRIValid) { return false; }
    if (m_schema != second.m_schema) { return false; }
    if (m_port != second.m_port) { return false; }

    size_t size = m_host.size() < second.m_host.size() ?
        m_host.size() : second.m_host.size();

    if (m_host.size() > second.m_host.size()) {
        return false;
    }

    if (!m_domain && (m_host.size() != second.m_host.size())) {
        return false;
    }

    for (size_t i = 0; i < size; ++i) {
        if (DPL::StringCompare(m_host[i], second.m_host[i])) {
            return false;
        }
    }
    return true;
}

bool WarpIRI::isAccessDefinition() const
{
    return m_isAccessDefinition;
}

bool WarpIRI::getSubDomain() const
{
    return m_domain;
}

bool WarpIRI::isIRISchemaIgnored(const char *iri)
{
    for (int i = 0; IRI_IGNORED_SCHEME[i]; ++i) {
        if (0 ==
            strncmp(iri, IRI_IGNORED_SCHEME[i],
                    strlen(IRI_IGNORED_SCHEME[i]))) {
            return true;
        }
    }
    return false;
}
