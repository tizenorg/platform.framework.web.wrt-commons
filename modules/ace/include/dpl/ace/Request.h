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
//
//
//
//  @ Project : Access Control Engine
//  @ File Name : Request.h
//  @ Date : 2009-05-06
//  @ Author : Samsung
//
//

#ifndef _REQUEST_H_
#define _REQUEST_H_

#include <set>
#include <string>
#include <vector>

#include <dpl/ace-dao-ro/IRequest.h>

#include <dpl/ace/WRT_INTERFACE.h>

class Request : public AceDB::IRequest
{
  public:
    typedef std::string DeviceCapability;
    typedef std::set<DeviceCapability> DeviceCapabilitySet;

    Request(WidgetHandle widgetHandle,
            WidgetExecutionPhase phase,
            IFunctionParam *functionParam = 0)
      : m_widgetHandle(widgetHandle)
      , m_phase(phase)
      , m_functionParam(functionParam)
    {}

    WidgetHandle getWidgetHandle() const
    {
        return m_widgetHandle;
    }

    WidgetExecutionPhase getExecutionPhase() const
    {
        return m_phase;
    }

    IFunctionParam *getFunctionParam() const
    {
        return m_functionParam;
    }

    void addDeviceCapability(const std::string& device)
    {
        m_devcapSet.insert(device);
    }

    DeviceCapabilitySet getDeviceCapabilitySet() const
    {
        return m_devcapSet;
    }

  private:
    WidgetHandle m_widgetHandle;
    WidgetExecutionPhase m_phase;
    //! \brief list of function param (only for intercept)
    IFunctionParam *m_functionParam;
    //! \brief Set of defice capabilities
    DeviceCapabilitySet m_devcapSet;
};

typedef std::vector <Request> Requests;

#endif  //_REQUEST_H_
