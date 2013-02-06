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
 * @file        event_support.h
 * @author      Przemyslaw Dobrowolski (p.dobrowolsk@samsung.com)
 * @version     1.0
 * @brief       This file is the implementation file of MVC event support
 */
#ifndef DPL_EVENT_SUPPORT_H
#define DPL_EVENT_SUPPORT_H

#include <dpl/event/event_listener.h>
#include <dpl/event/abstract_event_dispatcher.h>
#include <dpl/event/main_event_dispatcher.h>
#include <dpl/event/thread_event_dispatcher.h>
#include <dpl/event/generic_event_call.h>
#include <dpl/waitable_event.h>
#include <dpl/fast_delegate.h>
#include <memory>
#include <dpl/exception.h>
#include <dpl/thread.h>
#include <dpl/assert.h>
#include <dpl/atomic.h>
#include <dpl/mutex.h>
#include <dpl/foreach.h>
#include <dpl/log/log.h>
#include <vector>
#include <map>
#include <list>

namespace DPL {
namespace Event {
namespace EmitMode {
enum Type
{
    Auto,     ///< If calling thread is the same as receiver's use
              ///< direct call, otherwise call is queued

    Queued,   ///< Call is always queued

    Blocking, ///< If calling thread is the same as receiver's use
              ///< direct call, otherwise call is queued and blocking

    Deffered  ///< Call is always queued for a period of time
};
} // namespace EmitMode

template<typename EventType>
class EventSupport :
    private Noncopyable
{
  public:
    typedef EventSupport<EventType> EventSupportType;

    typedef EventListener<EventType> EventListenerType;
    typedef FastDelegate1<const EventType &> DelegateType;

    class EventSupportData; // Forward declaration
    typedef EventSupportData *EventSupportDataPtr;

  private:
    typedef typename GenericEventCall<EventType, EventSupportDataPtr>::
        template Rebind<EventType, EventSupportDataPtr>::
        Other GenericEventCallType;

    // Event listener list
    typedef std::map<EventListenerType *, Thread *> EventListenerList;
    EventListenerList m_eventListenerList;

    // Delegate list
    typedef std::map<DelegateType, Thread *> DelegateList;
    DelegateList m_delegateList;

    // Event support operation mutex
    Mutex m_listenerDelegateMutex;

    // Dedicated instance of thread event dispatcher
    ThreadEventDispatcher m_threadEventDispatcher;

    // Guard destruction of event support in event handler
    Atomic m_guardedCallInProgress;

    // Events created by this support
    typedef std::list<GenericEventCallType *> EventCallList;
    EventCallList m_eventsList;

    // Events list mutex
    Mutex m_eventListMutex;

  public:
    class EventSupportData
    {
      private:
        typedef void (EventSupportType::*ReceiveAbstractEventCallMethod)(
            const EventType &event,
            EventListenerType *eventListener,
            DelegateType delegate,
            WaitableEvent *synchronization);

        EventSupportType *m_eventSupport;
        ReceiveAbstractEventCallMethod m_method;
        typename EventCallList::iterator m_iterator;

        //TODO: Add dispatcher iterator to remove events from
        //      framework/thread's event queue
        WaitableEvent *m_synchronization;

        Mutex m_dataMutex;

      public:
        EventSupportData(EventSupportType *support,
                         ReceiveAbstractEventCallMethod method,
                         WaitableEvent *synchronization) :
            m_eventSupport(support),
            m_method(method),
            m_synchronization(synchronization)
        {}

        ~EventSupportData()
        {
            Mutex::ScopedLock lock(&m_dataMutex);

            if (!m_eventSupport) {
                LogPedantic("EventSupport for this call does not exist");
                return;
            }

            m_eventSupport->RemoveEventCall(m_iterator);
        }

        // TODO: Make private and make EventSupport friend
        void SetIterator(typename EventCallList::iterator iter)
        {
            m_iterator = iter;
        }

        // This method at the end destroys this as it will not be used anymore
        void CallAndDestroy(const EventType &event,
                            EventListenerType *listener,
                            DelegateType delegate)
        {
            {
                Mutex::ScopedLock lock(&m_dataMutex);

                if (m_eventSupport != NULL) {
                    (*m_eventSupport.*m_method)(event,
                                                listener,
                                                delegate,
                                                m_synchronization);
                } else {
                    LogPedantic("EventSupport for this call does not "
                                "exist anymore. Ignored.");
                }

                // releasing mutex lock
            }

            // EventSupportData object is no more used.
            // It can be safely destroyed now.
            delete this;
        }

        void Reset()
        {
            LogPedantic("Reseting my EventSupport");

            Mutex::ScopedLock lock(&m_dataMutex);
            m_eventSupport = NULL;
        }
    };

  private:
    GenericEventCallType *RegisterEventCall(const EventType &event,
                                            EventListenerType *eventListener,
                                            DelegateType delegate,
                                            WaitableEvent *waitableEvent)
    {
        LogPedantic("Create and Register EventCall in EventSupport");

        Mutex::ScopedLock lock(&m_eventListMutex);

        EventSupportDataPtr supportData =
            new EventSupportData(
                this,
                &EventSupportType::ReceiveAbstractEventCall,
                waitableEvent);

        GenericEventCallType *eventCall =
            new GenericEventCallType(supportData, eventListener,
                                     delegate, event);

        typename EventCallList::iterator eventCallIter =
            m_eventsList.insert(m_eventsList.end(), eventCall);

        supportData->SetIterator(eventCallIter);

        return eventCall;
    }

    void RemoveEventCall(typename EventCallList::iterator eventIterator)
    {
        Mutex::ScopedLock lock(&m_eventListMutex);

        LogPedantic("Removing event call from EventSupport");

        m_eventsList.erase(eventIterator);
    }

    // Note: Reentrant metod
    void GuardedEventCall(const EventType &event,
                          EventListenerType *eventListener)
    {
        LogPedantic("Guarded event listener call...");

        ++m_guardedCallInProgress;

        UNHANDLED_EXCEPTION_HANDLER_BEGIN
        {
            eventListener->OnEventReceived(event);
        }
        UNHANDLED_EXCEPTION_HANDLER_END

        -- m_guardedCallInProgress;

        LogPedantic("Guarded event listener finished");
    }

    // Note: Reentrant metod
    void GuardedEventCall(const EventType &event,
                          DelegateType delegate)
    {
        LogPedantic("Guarded delegate call...");

        ++m_guardedCallInProgress;

        UNHANDLED_EXCEPTION_HANDLER_BEGIN
        {
            delegate(event);
        }
        UNHANDLED_EXCEPTION_HANDLER_END

        -- m_guardedCallInProgress;

        LogPedantic("Guarded delegate call finished");
    }

    void ReceiveAbstractEventCall(const EventType &event,
                                  EventListenerType *eventListener,
                                  DelegateType delegate,
                                  WaitableEvent *synchronization)
    {
        LogPedantic("Received abstract event call method");

        Thread *targetThread;

        // Listener might have been removed, ensure that it still exits
        if (eventListener != NULL) {
            Mutex::ScopedLock lock(&m_listenerDelegateMutex);

            typename EventListenerList::iterator iterator =
                m_eventListenerList.find(eventListener);

            if (iterator == m_eventListenerList.end()) {
                LogPedantic("Abstract event call listener disappeared."
                            "Event ignored.");

                // Even though, synchronize caller if needed
                if (synchronization != NULL) {
                    synchronization->Signal();
                }

                return;
            }

            // Get target thread id
            targetThread = iterator->second;
        } else {
            // Delegate might have been removed, ensure that it still exits
            Mutex::ScopedLock lock(&m_listenerDelegateMutex);

            typename DelegateList::iterator iterator =
                m_delegateList.find(delegate);

            if (iterator == m_delegateList.end()) {
                LogPedantic("Abstract event call delegate disappeared."
                            "Event ignored.");

                // Even though, synchronize caller if needed
                if (synchronization != NULL) {
                    synchronization->Signal();
                }

                return;
            }

            // Get target thread id
            targetThread = iterator->second;
        }

        // Ensure that we are now in proper thread now
        if (targetThread != Thread::GetCurrentThread()) {
            LogPedantic("Detected event dispatching ping-pong scenario");

            // Retry if it was not synchronized
            if (synchronization == NULL) {
                // Cheat with event delivery
                EmitEvent(event, EmitMode::Queued);

                LogPedantic("Ping-Pong: Resent as queued event");
            } else {
                // There is a problem
                // Developer did something nasty, and we will not clean up his
                // mess
                synchronization->Signal();

                LogPedantic("### Ping-Pong: Failed to deliver synchronized"
                            "event in ping-pong scenario!");
            }

            return;
        }

        // Guard listener code for exceptions
        if (eventListener != NULL) {
            GuardedEventCall(event, eventListener);
        } else {
            GuardedEventCall(event, delegate);
        }

        // Release caller if synchronizing
        if (synchronization != NULL) {
            synchronization->Signal();
        }
    }

  protected:
    void EmitEvent(const EventType &event,
                   EmitMode::Type mode = EmitMode::Queued,
                   double dueTime = 0.0)
    {
        // Emit event, and retrieve later in current context to dispatch
        std::unique_ptr<Mutex::ScopedLock> lock(
            new Mutex::ScopedLock(&m_listenerDelegateMutex));

        // Show some info
        switch (mode) {
        case EmitMode::Auto:
            LogPedantic("Emitting AUTO event...");
            break;

        case EmitMode::Queued:
            LogPedantic("Emitting QUEUED event...");
            break;

        case EmitMode::Blocking:
            LogPedantic("Emitting BLOCKING event...");
            break;

        case EmitMode::Deffered:
            LogPedantic("Emitting DEFFERED event...");
            break;

        default:
            break;
        }

        // In some configurations there is a barrier
        std::vector<WaitableEvent *> synchronizationBarrier;

        // Emit to all listeners
        FOREACH(iterator, m_eventListenerList)
        {
            // Switch to proper dispatcher and emit event
            AbstractEventDispatcher *dispatcher = NULL;

            if (iterator->second == NULL) {
                // Send to main thread
                LogPedantic("Sending event to main dispatcher");
                dispatcher = &GetMainEventDispatcherInstance();
            } else {
                // Setup thread dispatcher, and send to proper thread
                LogPedantic("Sending event to thread dispatcher");
                m_threadEventDispatcher.SetThread(iterator->second);
                dispatcher = &m_threadEventDispatcher;
            }

            // Dispatch event to abstract dispatcher
            WaitableEvent *synchronization;

            // TODO: Pool synchronization objects
            switch (mode) {
            case EmitMode::Auto:
                // Check thread
                if (iterator->second == Thread::GetCurrentThread()) {
                    // Guard listener code for exceptions
                    GuardedEventCall(event, iterator->first);
                } else {
                    // Handle non-synchronized event
                    dispatcher->AddEventCall(
                        RegisterEventCall(event, iterator->first,
                                          DelegateType(), NULL));
                }
                break;

            case EmitMode::Queued:
                // Handle non-synchronized event
                dispatcher->AddEventCall(
                    RegisterEventCall(event, iterator->first,
                                      DelegateType(), NULL));

                break;

            case EmitMode::Blocking:
                // Check thread
                if (iterator->second == Thread::GetCurrentThread()) {
                    // Guard listener code for exceptions
                    GuardedEventCall(event, iterator->first);
                } else {
                    // New synchronization object is needed
                    synchronization = new WaitableEvent();

                    // Handle synchronized event
                    dispatcher->AddEventCall(
                        RegisterEventCall(event, iterator->first,
                                          DelegateType(), synchronization));

                    // Add to barrier
                    synchronizationBarrier.push_back(synchronization);
                }
                break;

            case EmitMode::Deffered:
                // Handle deffered events
                Assert(dueTime >= 0.0 && "Due time must be non-negative");

                dispatcher->AddTimedEventCall(
                    RegisterEventCall(event, iterator->first,
                                      DelegateType(), NULL), dueTime);

                break;

            default:
                Assert("Invalid emit mode");
            }
        }

        LogPedantic("Added event to dispatchers");

        // Emit to all delegates
        FOREACH(iterator, m_delegateList)
        {
            // Switch to proper dispatcher and emit event
            AbstractEventDispatcher *dispatcher = NULL;

            if (iterator->second == NULL) {
                // Send to main thread
                LogPedantic("Sending event to main dispatcher");
                dispatcher = &GetMainEventDispatcherInstance();
            } else {
                // Setup thread dispatcher, and send to proper thread
                LogPedantic("Sending event to thread dispatcher");
                m_threadEventDispatcher.SetThread(iterator->second);
                dispatcher = &m_threadEventDispatcher;
            }

            // Dispatch event to abstract dispatcher
            WaitableEvent *synchronization;

            // TODO: Pool synchronization objects
            switch (mode) {
            case EmitMode::Auto:
                // Check thread
                if (iterator->second == Thread::GetCurrentThread()) {
                    // Guard listener code for exceptions
                    GuardedEventCall(event, iterator->first);
                } else {
                    // Handle non-synchronized event
                    dispatcher->AddEventCall(
                        RegisterEventCall(event,
                                          NULL,
                                          iterator->first,
                                          NULL));
                }
                break;

            case EmitMode::Queued:
                // Handle non-synchronized event
                dispatcher->AddEventCall(
                    RegisterEventCall(event,
                                      NULL,
                                      iterator->first,
                                      NULL));

                break;

            case EmitMode::Blocking:
                // Check thread
                if (iterator->second == Thread::GetCurrentThread()) {
                    // Guard listener code for exceptions
                    GuardedEventCall(event, iterator->first);
                } else {
                    // New synchronization object is needed
                    synchronization = new WaitableEvent();

                    // Handle synchronized event
                    dispatcher->AddEventCall(
                        RegisterEventCall(event,
                                          NULL,
                                          iterator->first,
                                          synchronization));

                    // Add to barrier
                    synchronizationBarrier.push_back(synchronization);
                }
                break;

            case EmitMode::Deffered:
                // Handle deffered events
                Assert(dueTime >= 0.0 && "Due time must be non-negative");

                dispatcher->AddTimedEventCall(
                    RegisterEventCall(event,
                                      NULL,
                                      iterator->first,
                                      NULL), dueTime);

                break;

            default:
                Assert("Invalid emit mode");
            }
        }

        LogPedantic("Added event to dispatchers");

        // Leave listeners lock in case of blocking call
        if (!synchronizationBarrier.empty()) {
            LogPedantic("Leaving lock due to existing barrier");
            lock.reset();
        }

        LogPedantic("Size of barrier: " << synchronizationBarrier.size());

        // Synchronize with barrier
        // TODO: Implement generic WaitForAllMultipleHandles call
        while (!synchronizationBarrier.empty()) {
            // Get barrier waitable handles
            WaitableHandleList barrierHandles;

            FOREACH(iterator, synchronizationBarrier)
            barrierHandles.push_back((*iterator)->GetHandle());

            // Await more events
            WaitableHandleIndexList indexList =
                WaitForMultipleHandles(barrierHandles);

            // Remove all awaited handles
            // TODO: Return handles to pool
            FOREACH(iterator, indexList)
            {
                // Delete object
                delete synchronizationBarrier[*iterator];

                // Zero out place
                synchronizationBarrier[*iterator] = NULL;
            }

            // Now clean up
            std::vector<WaitableEvent *> clearedSynchronizationBarrier;

            FOREACH(iterator, synchronizationBarrier)
            {
                if (*iterator == NULL) {
                    continue;
                }

                clearedSynchronizationBarrier.push_back(*iterator);
            }

            synchronizationBarrier.swap(clearedSynchronizationBarrier);

            LogPedantic("Reduced size of barrier: "
                        << synchronizationBarrier.size());
        }

        LogPedantic("Event emitted");
    }

  public:
    EventSupport() :
        m_guardedCallInProgress(false)
    {}

    virtual ~EventSupport()
    {
        Assert(m_guardedCallInProgress == false);

        m_eventListenerList.clear();
        m_delegateList.clear();

        Mutex::ScopedLock lock(&m_eventListMutex);

        LogPedantic("Disabling events for EventSupport");

        FOREACH(iterator, m_eventsList)
            (*iterator)->DisableEvent();
    }

    void AddListener(EventListenerType *eventListener)
    {
        Mutex::ScopedLock lock(&m_listenerDelegateMutex);

        // Listener must not be NULL
        Assert(eventListener != NULL);

        // Listener must not already exists
        Assert(m_eventListenerList.find(eventListener)
               == m_eventListenerList.end());

        // Add new listener, inherit dispatcher from current context
        m_eventListenerList.insert(
            std::make_pair(eventListener, Thread::GetCurrentThread()));

        // Done
        LogPedantic("Listener registered");
    }

    void AddListener(DelegateType delegate)
    {
        Mutex::ScopedLock lock(&m_listenerDelegateMutex);

        // Delegate must not be empty
        Assert(!delegate.empty());

        // Delegate must not already exists
        Assert(m_delegateList.find(delegate) == m_delegateList.end());

        // Add new delegate, inherit dispatcher from current context
        m_delegateList.insert(
            std::make_pair(delegate, Thread::GetCurrentThread()));

        // Done
        LogPedantic("Delegate registered");
    }

    void RemoveListener(EventListenerType *eventListener)
    {
        Mutex::ScopedLock lock(&m_listenerDelegateMutex);

        // Listener must not be NULL
        Assert(eventListener != NULL);

        // Listener must exist
        typename EventListenerList::iterator iterator =
            m_eventListenerList.find(eventListener);

        Assert(iterator != m_eventListenerList.end());

        // Remove listener from list
        m_eventListenerList.erase(iterator);
        LogPedantic("Listener unregistered");
    }

    void RemoveListener(DelegateType delegate)
    {
        Mutex::ScopedLock lock(&m_listenerDelegateMutex);

        // Delegate must not be empty
        Assert(!delegate.empty());

        // Delegate must exist
        typename DelegateList::iterator iterator =
            m_delegateList.find(delegate);

        Assert(iterator != m_delegateList.end());

        // Remove delegate from list
        m_delegateList.erase(iterator);
        LogPedantic("Delegate unregistered");
    }

    void SwitchListenerToThread(EventListenerType *eventListener,
                                Thread *thread)
    {
        Mutex::ScopedLock lock(&m_listenerDelegateMutex);

        // Listener must not be NULL
        Assert(eventListener != NULL);

        // Listener must exist
        typename EventListenerList::iterator iterator =
            m_eventListenerList.find(eventListener);

        Assert(iterator != m_eventListenerList.end());

        // Set listener thread
        iterator->second = thread;

        LogPedantic("Listener switched");
    }

    void SwitchListenerToThread(DelegateType delegate,
                                Thread *thread)
    {
        Mutex::ScopedLock lock(&m_listenerDelegateMutex);

        // Delegate must not be empty
        Assert(!delegate.empty());

        // Delegate must exist
        typename EventListenerList::iterator iterator =
            m_delegateList.find(delegate);

        Assert(iterator != m_delegateList.end());

        // Set delegate thread
        iterator->second = thread;

        LogPedantic("Delegate switched");
    }

    void SwitchAllListenersToThread(Thread *thread)
    {
        Mutex::ScopedLock lock(&m_listenerDelegateMutex);

        // Switch all listeners and delegates
        FOREACH(iterator, m_eventListenerList)
        iterator->second = thread;

        FOREACH(iterator, m_delegateList)
        iterator->second = thread;

        LogPedantic("All listeners and delegates switched");
    }
};
}
} // namespace DPL

#endif // DPL_EVENT_SUPPORT_H
