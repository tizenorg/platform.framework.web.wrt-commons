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
 * @file        generic_event_call.h
 * @author      Przemyslaw Dobrowolski (p.dobrowolsk@samsung.com)
 * @version     1.0
 * @brief       This file is the implementation file of generic event call
 */
#ifndef DPL_GENERIC_EVENT_CALL_H
#define DPL_GENERIC_EVENT_CALL_H

#include <dpl/event/abstract_event_call.h>
#include <dpl/event/event_listener.h>
#include <dpl/noncopyable.h>
#include <dpl/fast_delegate.h>
#include <dpl/log/log.h>
#include <dpl/assert.h>

namespace DPL {
namespace Event {
template<typename EventType, typename SupportDataType>
class GenericEventCall :
    public AbstractEventCall
{
  public:
    typedef EventListener<EventType> EventListenerType;
    typedef FastDelegate1<const EventType &> DelegateType;

  protected:
    SupportDataType m_supportData;
    EventListenerType *m_eventListener;
    DelegateType m_delegate;
    EventType m_event;

  public:
    template<typename OtherEventType, typename OtherSupportType>
    struct Rebind
    {
        typedef GenericEventCall<OtherEventType, OtherSupportType> Other;
    };

    GenericEventCall(SupportDataType supportData,
                     EventListenerType *eventListener,
                     DelegateType delegate,
                     const EventType &event) :
        m_supportData(supportData),
        m_eventListener(eventListener),
        m_delegate(delegate),
        m_event(event)
    {}

    virtual ~GenericEventCall()
    {
        Assert(m_supportData == NULL &&
               "Call method hasn't been called"
               " (support data wasn't destroyed)");
    }

    virtual void Call()
    {
        LogPedantic("Calling generic event call");

        m_supportData->CallAndDestroy(m_event, m_eventListener, m_delegate);

        // Now m_supportData points to invalid object. Marking it as NULL.
        m_supportData = NULL;

        LogPedantic("Generic event called");
    }

    virtual void DisableEvent()
    {
        LogPedantic("Disabling this EventCall");
        m_supportData->Reset();

        // TODO: In the future, event should be completely removed
        // from the event queue (not just marked "disabled")
    }
};
}
} // namespace DPL

#endif // DPL_GENERIC_EVENT_CALL_H
