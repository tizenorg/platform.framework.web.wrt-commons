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
 * @file        main_event_dispatcher.cpp
 * @author      Przemyslaw Dobrowolski (p.dobrowolsk@samsung.com)
 * @version     1.0
 * @brief       This file is the implementation file of main event dispatcher
 * for EFL
 */
#include <stddef.h>
#include <dpl/event/main_event_dispatcher.h>
#include <dpl/log/log.h>
#include <dpl/assert.h>
#include <dpl/singleton_impl.h>

namespace DPL {
IMPLEMENT_SINGLETON(Event::MainEventDispatcher)

namespace Event {
typedef Singleton<Event::MainEventDispatcher> MainEventDispatcherSingleton;

namespace // anonymous
{
static const pthread_t g_threadMain = pthread_self();

// Late EFL event handling
MainEventDispatcher *g_lateMainEventDispatcher = NULL;
} // namespace anonymous

MainEventDispatcher::MainEventDispatcher()
{
    // Late EFL event handling
    Assert(g_lateMainEventDispatcher == NULL);
    g_lateMainEventDispatcher = this;

    // Increment ECORE init count to ensure we have all
    // subsystems correctly set-up until main dispatcher dtor
    // This is especially important when MainEventDispatcher
    // is a global object destroyed no earlier than crt destroy routine
    ecore_init();

    // Add new global ECORE event
    m_eventId = ecore_event_type_new();

    LogPedantic("ECORE event class registered: " << m_eventId);

    // Register event class handler
    if ((m_eventCallHandler =
             ecore_event_handler_add(m_eventId, &StaticDispatchEvent,
                                     this)) == NULL)
    {
        ThrowMsg(Exception::CreateFailed, "Failed to register event handler!");
    }

    // Allocate WaitableEvent
    m_crossEventCallInvoker = new WaitableEvent();

    // Register cross event handler
    m_crossEventCallHandler = ecore_main_fd_handler_add(
            m_crossEventCallInvoker->GetHandle(),
            ECORE_FD_READ,
            &StaticDispatchCrossInvoker,
            this,
            NULL,
            NULL);

    if (m_crossEventCallHandler == NULL) {
        ThrowMsg(Exception::CreateFailed,
                 "Failed to register cross event handler!");
    }

    LogPedantic("ECORE cross-event handler registered");
}

MainEventDispatcher::~MainEventDispatcher()
{
    // Remove cross event handler
    ecore_main_fd_handler_del(m_crossEventCallHandler);
    m_crossEventCallHandler = NULL;
    LogPedantic("ECORE cross-event handler unregistered");

    // Remove m_crossEventCallInvoker
    delete m_crossEventCallInvoker;
    m_crossEventCallInvoker = NULL;

    // Remove event class handler
    ecore_event_handler_del(m_eventCallHandler);
    m_eventCallHandler = NULL;

    // Decrement ECORE init count
    // We do not need ecore routines any more
    ecore_shutdown();

    // Late EFL event handling
    Assert(g_lateMainEventDispatcher == this);
    g_lateMainEventDispatcher = NULL;
}

void MainEventDispatcher::ResetCrossEventCallHandler()
{
    // Remove cross event handler
    ecore_main_fd_handler_del(m_crossEventCallHandler);
    m_crossEventCallHandler = NULL;
    LogPedantic("ECORE cross-event handler unregistered");

    // Re-allocate WaitableEvent
    delete m_crossEventCallInvoker;
    m_crossEventCallInvoker = new WaitableEvent();

    // Register cross event handler
    m_crossEventCallHandler =
        ecore_main_fd_handler_add(m_crossEventCallInvoker->GetHandle(),
                                  ECORE_FD_READ,
                                  &StaticDispatchCrossInvoker,
                                  this,
                                  NULL,
                                  NULL);

    if (m_crossEventCallHandler == NULL) {
        ThrowMsg(Exception::CreateFailed,
                 "Failed to register cross event handler!");
    }

    LogPedantic("ECORE cross-event handler re-registered");
}

void MainEventDispatcher::StaticDeleteEvent(void *data, void *event)
{
    LogPedantic("Static ECORE delete event handler");

    MainEventDispatcher *This = static_cast<MainEventDispatcher *>(data);
    AbstractEventCall *abstractEventCall =
        static_cast<AbstractEventCall *>(event);

    Assert(This != NULL);
    Assert(abstractEventCall != NULL);

    // Late EFL event handling
    if (g_lateMainEventDispatcher == NULL) {
        LogPedantic("WARNING: Late EFL event delete!");
        delete abstractEventCall;
    } else {
        This->DeleteEvent(abstractEventCall);
    }
}

Eina_Bool MainEventDispatcher::StaticDispatchEvent(void *data,
                                                   int type,
                                                   void *event)
{
    LogPedantic("Static ECORE dispatch event");

    MainEventDispatcher *This = static_cast<MainEventDispatcher *>(data);
    AbstractEventCall *abstractEventCall =
        static_cast<AbstractEventCall *>(event);
    (void)type;

    Assert(This != NULL);
    Assert(abstractEventCall != NULL);

    // Late EFL event handling
    if (g_lateMainEventDispatcher == NULL) {
        LogPedantic("WARNING: Late EFL event dispatch!");
    } else {
        This->DispatchEvent(abstractEventCall);
    }

    // Continue to handler other ECORE events
    return ECORE_CALLBACK_RENEW;
}

Eina_Bool MainEventDispatcher::StaticDispatchTimedEvent(void *data)
{
    LogPedantic("Static ECORE dispatch timed event");

    TimedEventStruct *timedEventStruct = static_cast<TimedEventStruct *>(data);
    MainEventDispatcher *This = timedEventStruct->This;
    AbstractEventCall *abstractEventCall = timedEventStruct->abstractEventCall;
    delete timedEventStruct;

    Assert(This != NULL);
    Assert(abstractEventCall != NULL);

    // Late EFL event handling
    if (g_lateMainEventDispatcher == NULL) {
        LogPedantic("WARNING: Late EFL timed event dispatch!");
    } else {
        // Dispatch timed event
        This->DispatchEvent(abstractEventCall);
    }

    // And delete manually event, because ECORE does not
    // use delete handler for timers
    StaticDeleteEvent(static_cast<void *>(This),
                      static_cast<void *>(abstractEventCall));

    // Do not continue timed event handlers
    // This also releases ECORE timer
    return ECORE_CALLBACK_CANCEL;
}

Eina_Bool MainEventDispatcher::StaticDispatchCrossInvoker(
    void *data,
    Ecore_Fd_Handler *
    fd_handler)
{
    LogPedantic("Static ECORE dispatch cross invoker");

    MainEventDispatcher *This = static_cast<MainEventDispatcher *>(data);
    (void)fd_handler;

    Assert(This != NULL);

    // Late EFL event handling
    if (g_lateMainEventDispatcher == NULL) {
        LogPedantic("WARNING: Late EFL cross invoker dispatch!");
    } else {
        This->DispatchCrossInvoker();
    }

    return ECORE_CALLBACK_RENEW;
}

void MainEventDispatcher::DeleteEvent(AbstractEventCall *abstractEventCall)
{
    LogPedantic("ECORE delete event");
    delete abstractEventCall;
}

void MainEventDispatcher::DispatchEvent(AbstractEventCall *abstractEventCall)
{
    LogPedantic("ECORE dispatch event");

    // Call event handler
    abstractEventCall->Call();
}

void MainEventDispatcher::DispatchTimedEvent(
    AbstractEventCall *abstractEventCall)
{
    LogPedantic("ECORE dispatch timed event");

    // Call event handler
    abstractEventCall->Call();
}

void MainEventDispatcher::DispatchCrossInvoker()
{
    LogPedantic("ECORE dispatch cross invoker");

    // Steal cross events list
    WrappedEventCallList stolenCrossEvents;

    // Critical section
    {
        m_crossEventCallInvoker->Reset();
        Mutex::ScopedLock lock(&m_crossEventCallMutex);
        m_wrappedCrossEventCallList.swap(stolenCrossEvents);
    }

    LogPedantic(
        "Cross-thread event list stolen. Number of events: " <<
        stolenCrossEvents.size());

    // Repush all stolen events
    WrappedEventCallList::const_iterator eventIterator;

    for (eventIterator = stolenCrossEvents.begin();
         eventIterator != stolenCrossEvents.end();
         ++eventIterator)
    {
        // Unwrap events
        LogPedantic("Dispatching event from invoker");
        InternalAddEvent(eventIterator->abstractEventCall,
                         eventIterator->timed,
                         eventIterator->dueTime);
    }

    LogPedantic("Cross-thread events dispatched");
}

void MainEventDispatcher::AddEventCall(AbstractEventCall *abstractEventCall)
{
    if (pthread_equal(pthread_self(), g_threadMain)) {
        LogPedantic("Main thread ECORE event push");
        InternalAddEvent(abstractEventCall, false, 0.0);
    } else {
        LogPedantic("Cross-thread ECORE event push");

        // Push event to cross event list
        {
            Mutex::ScopedLock lock(&m_crossEventCallMutex);
            m_wrappedCrossEventCallList.push_back(WrappedEventCall(
                                                      abstractEventCall, false,
                                                      0.0));
            m_crossEventCallInvoker->Signal();
        }

        LogPedantic("Event pushed to cross-thread event list");
    }
}

void MainEventDispatcher::AddTimedEventCall(
    AbstractEventCall *abstractEventCall,
    double dueTime)
{
    if (pthread_equal(pthread_self(), g_threadMain)) {
        LogPedantic("Main thread timed ECORE event push");
        InternalAddEvent(abstractEventCall, true, dueTime);
    } else {
        LogPedantic("Cross-thread timed ECORE event push");

        // Push event to cross event list
        {
            Mutex::ScopedLock lock(&m_crossEventCallMutex);
            m_wrappedCrossEventCallList.push_back(WrappedEventCall(
                                                      abstractEventCall, true,
                                                      dueTime));
            m_crossEventCallInvoker->Signal();
        }

        LogPedantic("Event pushed to cross-thread event list");
    }
}

void MainEventDispatcher::InternalAddEvent(AbstractEventCall *abstractEventCall,
                                           bool timed,
                                           double dueTime)
{
    LogPedantic("Adding base event");

    if (timed == true) {
        // Push timed event onto ecore stack
        TimedEventStruct* eventData = new TimedEventStruct(abstractEventCall,
                                                           this);
        Ecore_Timer *timedEvent = ecore_timer_add(dueTime,
                                                  &StaticDispatchTimedEvent,
                                                  eventData);

        if (timedEvent == NULL) {
            delete eventData;
            delete abstractEventCall;
            ThrowMsg(Exception::AddTimedEventFailed,
                     "Failed to add ECORE timed event");
        }

        LogPedantic("Timed wrapped event added");
    } else {
        // Push immediate event onto ecore stack
        Ecore_Event *event = ecore_event_add(m_eventId,
                                             abstractEventCall,
                                             &StaticDeleteEvent,
                                             this);

        if (event == NULL) {
            delete abstractEventCall;
            ThrowMsg(Exception::AddEventFailed, "Failed to add ECORE event");
        }

        LogPedantic("Wrapped event added");
    }
}

MainEventDispatcher& GetMainEventDispatcherInstance()
{
    return MainEventDispatcherSingleton::Instance();
}
}
} // namespace DPL
