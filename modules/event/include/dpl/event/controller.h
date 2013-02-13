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
 * @file        controller.h
 * @author      Przemyslaw Dobrowolski (p.dobrowolsk@samsung.com)
 * @version     1.0
 * @brief       This file is the implementation file of MVC controller
 */
#ifndef DPL_CONTROLLER_H
#define DPL_CONTROLLER_H

#include <dpl/event/event_support.h>
#include <dpl/event/event_listener.h>
#include <dpl/type_list.h>
#include <dpl/thread.h>
#include <dpl/assert.h>

namespace DPL {
namespace Event {
template<typename EventType>
class ControllerEventHandler :
    public EventListener<EventType>,
    private EventSupport<EventType>
{
  private:
    bool m_touched;

  public:
    ControllerEventHandler() :
        m_touched(false)
    {
        EventSupport<EventType>::AddListener(this);
    }

    virtual ~ControllerEventHandler()
    {
        EventSupport<EventType>::RemoveListener(this);
    }

    void PostEvent(const EventType &event)
    {
        Assert(
            m_touched &&
            "Default context not inherited. Call Touch() to inherit one.");
        EventSupport<EventType>::EmitEvent(event, EmitMode::Queued);
    }

    void PostTimedEvent(const EventType &event, double dueTime)
    {
        Assert(
            m_touched &&
            "Default context not inherited. Call Touch() to inherit one.");
        EventSupport<EventType>::EmitEvent(event, EmitMode::Deffered, dueTime);
    }

    void PostSyncEvent(const EventType &event)
    {
        Assert(
            m_touched &&
            "Default context not inherited. Call Touch() to inherit one.");

        // Check calling context
        EventSupport<EventType>::EmitEvent(event, EmitMode::Blocking);
    }

    void SwitchToThread(Thread *thread)
    {
        Assert(
            m_touched &&
            "Default context not inherited. Call Touch() to inherit one.");
        EventSupport<EventType>::SwitchListenerToThread(this, thread);
    }

    void Touch()
    {
        m_touched = true;
        EventSupport<EventType>::SwitchListenerToThread(
            this,
            Thread::
                GetCurrentThread());
    }
};

template<typename EventTypeList>
class Controller :
    public Controller<typename EventTypeList::Tail>,
    public ControllerEventHandler<typename EventTypeList::Head>
{
  public:
    typedef typename EventTypeList::Head EventType;

  public:
    Controller()
    {}

    virtual ~Controller()
    {}

    virtual void SwitchToThread(Thread *thread)
    {
        ControllerEventHandler<EventType>::SwitchToThread(thread);
        Controller<typename EventTypeList::Tail>::SwitchToThread(thread);
    }

    virtual void Touch()
    {
        ControllerEventHandler<EventType>::Touch();
        Controller<typename EventTypeList::Tail>::Touch();
    }
};

template<>
class Controller<TypeListDecl<>::Type>
{
  public:
    Controller()
    {}

    virtual ~Controller()
    {}

    virtual void SwitchToThread(Thread *thread)
    {
        (void)thread;
    }

    virtual void Touch()
    {}
};
}
} // namespace DPL

// Utilities
#define CONTROLLER_POST_EVENT(Name, \
                              EventArg) Name##Singleton::Instance().DPL::Event \
        ::ControllerEventHandler< \
        __typeof__ EventArg>::PostEvent(EventArg)
#define CONTROLLER_POST_TIMED_EVENT(Name, EventArg, \
                                    DueTime) Name##Singleton::Instance().DPL:: \
        Event::ControllerEventHandler< \
        __typeof__ EventArg>::PostTimedEvent(EventArg, DueTime)
#define CONTROLLER_POST_SYNC_EVENT(Name, \
                                   EventArg) Name##Singleton::Instance().DPL:: \
        Event::ControllerEventHandler< \
        __typeof__ EventArg>::PostSyncEvent(EventArg)

#endif // DPL_CONTROLLER_H
