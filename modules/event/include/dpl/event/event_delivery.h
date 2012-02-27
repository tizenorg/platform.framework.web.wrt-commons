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
 * @file        event_delivery.h
 * @author      Przemyslaw Dobrowolski (p.dobrowolsk@samsung.com)
 * @version     1.0
 * @brief       This file is the header file of event delivery class
 */
#ifndef DPL_EVENT_DELIVERY_H
#define DPL_EVENT_DELIVERY_H

#include <dpl/event/event_delivery_detail.h>
#include <dpl/event/event_delivery_messages.h>
#include <dpl/event/event_support.h>
#include <dpl/noncopyable.h>
#include <dpl/shared_ptr.h>
#include <dpl/exception.h>
#include <dpl/assert.h>
#include <dpl/foreach.h>
#include <list>
#include <map>
#include <typeinfo>
#include <string>

/*
 * Event delivery system
 *
 * Sample usages:
 *
 * I. Listening for standard predefined notifications
 *
 *   class MyClass
 *       : public EventListener<EventMessages::RoamingChanged>
 *   {
 *       void OnEventReceived(const EventMessages::RoamingChanged& event)
 *       {
 *           std::cout << "Roaming is now " << event.GetRoamingEnabled() ? "ENABLED" : "DISABLED" << std::endl;
 *       }
 *
 *   public:
 *       MyClass()
 *       {
 *           EventDeliverySystem::AddListener<EventMessages::RoamingChanged>(this);
 *       }
 *
 *       virtual ~MyClass()
 *       {
 *           EventDeliverySystem::RemoveListener<EventMessages::RoamingChanged>(this);
 *       }
 *
 *       void SendSampleSignal()
 *       {
 *           EventMessages::RoamingChanged roamingMessage(true);
 *           EventDeliverySystem::Publish(roamingMessage);
 *       }
 *   }
 *
 *  II. Creation of custom generic events, and listening for them
 *
 *   EVENT_DELIVERY_DECLARE_EVENT_MESSAGE_2(MyEvent, int, float);
 *
 *   class MyClass
 *       : public EventListener<MyEvent>
 *   {
 *       void OnEventReceived(const MyEvent &event)
 *       {
 *           std::cout << "Event contents: " << event.GetArg0() << ", " << event.GetArg1() << std::endl;
 *       }
 *
 *   public:
 *       MyClass()
 *       {
 *           EventDeliverySystem::AddListener<MyEvent>(this);
 *       }
 *
 *       virtual ~MyClass()
 *       {
 *           EventDeliverySystem::RemoveListener<MyEvent>(this);
 *       }
 *
 *       void SendSampleSignal()
 *       {
 *           MyEvent myEvent(5, 3.14f);
 *           EventDeliverySystem::Publish(myEvent);
 *       }
 *   }
 *
 *  In source file, one must add implementation of event delivery message:
 *
 *  EVENT_DELIVERY_IMPLEMENT_EVENT_MESSAGE(MyEvent)
 *
 */
namespace DPL
{
namespace Event
{

class AbstractEventDeliverySystemPublisher
{
public:
    virtual ~AbstractEventDeliverySystemPublisher() = 0;
};

template<typename EventType>
class EventDeliverySystemPublisher
    : private EventSupport<EventType>
    , public AbstractEventDeliverySystemPublisher
{
public:
    typedef typename EventSupport<EventType>::EventListenerType EventListenerType;

private:
    friend class EventDeliverySystem;

    EventDeliverySystemPublisher(EventListenerType *eventListener)
        : m_eventListener(eventListener)
    {
        EventSupport<EventType>::AddListener(m_eventListener);
    }

    EventListenerType *m_eventListener;

public:
    ~EventDeliverySystemPublisher()
    {
        EventSupport<EventType>::RemoveListener(m_eventListener);
    }

    void PublishEvent(const EventType &event)
    {
        EmitEvent(event, EmitMode::Queued);
    }

    EventListenerType *GetListener() const
    {
        return m_eventListener;
    }
};

template<typename Type>
class EventDeliverySystemTraits
{
public:
    typedef Type EventType;
    typedef EventDeliverySystemPublisher<EventType> PublisherType;
    typedef typename PublisherType::EventListenerType EventListenerType;
    typedef SharedPtr<PublisherType> PublisherPtrType;
    typedef std::list<PublisherPtrType> PublisherPtrContainerType;
};

class EventDeliverySystem
    : private Noncopyable
{
private:
    typedef SharedPtr<AbstractEventDeliverySystemPublisher> AbstractPublisherPtrType;
    typedef std::list<AbstractPublisherPtrType> AbstractPublisherPtrContainerType;
    typedef AbstractPublisherPtrContainerType::const_iterator AbstractPublisherPtrContainerTypeConstIterator;
    typedef AbstractPublisherPtrContainerType::iterator AbstractPublisherPtrContainerTypeIterator;
    typedef std::map<std::string, AbstractPublisherPtrContainerType> AbstractPublisherPtrContainerMapType;

    template<class EventType>
    class CrtDeleteCheck
    {
    private:
        const AbstractPublisherPtrContainerType &m_publisherContainer;

    public:
        CrtDeleteCheck(const AbstractPublisherPtrContainerType &publisherContainer)
            : m_publisherContainer(publisherContainer)
        {
        }

        virtual ~CrtDeleteCheck()
        {
            Assert(m_publisherContainer.empty() && "All event delivery listeners must be removed before exit!");
        }
    };

    // Map containing publishers for all registered events
    static AbstractPublisherPtrContainerMapType m_publishers;

    // Detail implementation: lazy initialized
    static EventDeliverySystemDetail m_detailSystem;

    EventDeliverySystem()
    {
    }

    template<typename EventType>
    static AbstractPublisherPtrContainerType &GetPublisherContainerRef()
    {
        std::string typeId = typeid(EventType).name();

        //FIXME: problem with linking per compilation unit won't hurt this check?
        static CrtDeleteCheck<EventType> crtDeleteCheck(m_publishers[typeId]);
        (void)crtDeleteCheck;

        return m_publishers[typeId];
    }

    template<typename EventType>
    static typename EventDeliverySystemTraits<EventType>::PublisherPtrContainerType GetPublisherContainer()
    {
        std::string typeId = typeid(EventType).name();
        typedef typename EventDeliverySystemTraits<EventType>::PublisherPtrContainerType PublisherContainerType;
        typedef typename EventDeliverySystemTraits<EventType>::PublisherType PublisherType;
        typedef typename EventDeliverySystemTraits<EventType>::PublisherPtrType PublisherPtrType;

        AbstractPublisherPtrContainerType eventPublishers = m_publishers[typeId];
        PublisherContainerType publisherContainer;

        FOREACH(iterator, eventPublishers)
        {
            PublisherPtrType publisher =
                    StaticPointerCast<PublisherType>(*iterator);
            publisherContainer.push_back(publisher);
        }

        return publisherContainer;
    }

    // Detail access
    template<typename EventType>
    static void Inject(const EventType &event)
    {
        typedef typename EventDeliverySystemTraits<EventType>::PublisherPtrContainerType PublisherContainerType;
        typedef typename EventDeliverySystemTraits<EventType>::PublisherPtrType PublisherPtrType;

        PublisherContainerType publisherContainer = GetPublisherContainer<EventType>();
        typename PublisherContainerType::iterator iterator;

        for (iterator = publisherContainer.begin(); iterator != publisherContainer.end(); ++iterator)
        {
            PublisherPtrType &publisher = *iterator;
            publisher->PublishEvent(event);
        }
    }

    template<typename EventType>
    class PublisherPtrContainerListenerPredicate
    {
    public:
        typedef EventListener<EventType> EventListenerType;

    private:
        EventListenerType *m_listener;

    public:
        PublisherPtrContainerListenerPredicate(EventListenerType *listener)
            : m_listener(listener)
        {
        }

        bool operator()(const AbstractPublisherPtrType &publisher) const
        {
            return StaticPointerCast<typename EventDeliverySystemTraits<EventType>::PublisherType>(publisher)->GetListener() == m_listener;
        }
    };

    friend class EventDeliverySystemInjector;

public:
    virtual ~EventDeliverySystem()
    {
    }

    template<typename EventType>
    static void AddListener(EventListener<EventType> *listener)
    {
        typedef typename EventDeliverySystemTraits<EventType>::PublisherType PublisherType;
        typedef typename EventDeliverySystemTraits<EventType>::PublisherPtrType PublisherPtrType;
        typedef typename EventDeliverySystemTraits<EventType>::PublisherPtrContainerType PublisherPtrContainerType;

        typedef PublisherPtrContainerListenerPredicate<EventType> PublisherPtrContainerListenerPredicateType;

        AbstractPublisherPtrContainerType &publisherContainer = GetPublisherContainerRef<EventType>();
        AbstractPublisherPtrContainerTypeConstIterator iterator =
                std::find_if(publisherContainer.begin(), publisherContainer.end(), PublisherPtrContainerListenerPredicateType(listener));

        Assert(iterator == publisherContainer.end() && "Listener already exists");

        // Add new publisher
        publisherContainer.push_back(AbstractPublisherPtrType(new PublisherType(listener)));

        // When first listener is inserted, start to listen for events
        if (publisherContainer.size() == 1) {
            m_detailSystem.Listen(EventType());
        }
    }

    template<typename EventType>
    static void RemoveListener(EventListener<EventType> *listener)
    {
        typedef typename EventDeliverySystemTraits<EventType>::PublisherType PublisherType;
        typedef typename EventDeliverySystemTraits<EventType>::PublisherPtrType PublisherPtrType;
        typedef typename EventDeliverySystemTraits<EventType>::PublisherPtrContainerType PublisherPtrContainerType;

        typedef PublisherPtrContainerListenerPredicate<EventType> PublisherPtrContainerListenerPredicateType;

        AbstractPublisherPtrContainerType &publisherContainer = GetPublisherContainerRef<EventType>();

        AbstractPublisherPtrContainerTypeIterator iterator =
                std::find_if(publisherContainer.begin(), publisherContainer.end(), PublisherPtrContainerListenerPredicateType(listener));

        Assert(iterator != publisherContainer.end() && "Listener does not exist");

        // Remove publisher
        publisherContainer.erase(iterator);

        // When last listener was removed, stop to listen for events
        if (publisherContainer.empty())
            m_detailSystem.Unlisten(EventType());
    }

    template<typename EventType>
    static void Publish(const EventType &event)
    {
        m_detailSystem.Publish(event);
    }
};

}
} // namespace DPL

#endif // DPL_EVENT_DELIVERY_H
