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
 * @file        event_delivery_injector.h
 * @author      Przemyslaw Dobrowolski (p.dobrowolsk@samsung.com)
 * @version     1.0
 * @brief       This file is the header file of event delivery injector
 */
#ifndef DPL_EVENT_DELIVERY_INJECTOR_H
#define DPL_EVENT_DELIVERY_INJECTOR_H

#include <dpl/event/event_delivery.h>
#include <dpl/noncopyable.h>
#include <dpl/log/log.h>
#include <utility>
#include <string>
#include <map>

namespace DPL
{
namespace Event
{

class EventDeliverySystemInjector
    : private Noncopyable
{
private:
    EventDeliverySystemInjector()
    {
    }

    typedef void (*MetaCall)(const std::string &metaData);
    typedef std::map<const char *, MetaCall> MetaCallContainer;
    MetaCallContainer m_metaCalls;

public:
    virtual ~EventDeliverySystemInjector()
    {
    }

    static EventDeliverySystemInjector &Instance()
    {
        static EventDeliverySystemInjector instance;
        return instance;
    }

    void PublishMetaType(const char *metaType, const std::string &metaData)
    {
        // Unwrap meta type (indirect Publish meta call)
        m_metaCalls[metaType](metaData);
    }

    template<typename EventMessageType>
    void Publish(const EventMessageType &message)
    {
        EventDeliverySystem::Inject(message);
    }

    void RegisterMetaCall(const char *metaType, MetaCall metaCall)
    {
        m_metaCalls.insert(std::make_pair(metaType, metaCall));
    }
};

}
} // namespace DPL

#endif // DPL_EVENT_DELIVERY_INJECTOR_H
