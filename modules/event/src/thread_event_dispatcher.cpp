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
 * @file        thread_event_dispatcher.cpp
 * @author      Przemyslaw Dobrowolski (p.dobrowolsk@samsung.com)
 * @version     1.0
 * @brief       This file is the implementation file of thread event dispatcher
 */
#include <dpl/event/thread_event_dispatcher.h>
#include <dpl/log/log.h>
#include <dpl/assert.h>

namespace DPL
{
namespace Event
{

ThreadEventDispatcher::ThreadEventDispatcher()
    : m_thread(NULL)
{
}

ThreadEventDispatcher::~ThreadEventDispatcher()
{
}

void ThreadEventDispatcher::SetThread(Thread *thread)
{
    m_thread = thread;
}

void ThreadEventDispatcher::StaticEventDelete(void *event, void *userParam)
{
    AbstractEventCall *abstractEventCall = static_cast<AbstractEventCall *>(event);
    ThreadEventDispatcher *This = static_cast<ThreadEventDispatcher *>(userParam);

    LogPedantic("Received static event delete from thread");

    Assert(abstractEventCall != NULL);
    Assert(This != NULL);

    This->EventDelete(abstractEventCall);
}

void ThreadEventDispatcher::StaticEventDispatch(void *event, void *userParam)
{
    AbstractEventCall *abstractEventCall = static_cast<AbstractEventCall *>(event);
    ThreadEventDispatcher *This = static_cast<ThreadEventDispatcher *>(userParam);

    LogPedantic("Received static event dispatch from thread");

    Assert(abstractEventCall != NULL);
    Assert(This != NULL);

    This->EventDispatch(abstractEventCall);
}

void ThreadEventDispatcher::EventDelete(AbstractEventCall *abstractEventCall)
{
    LogPedantic("Deleting event");
    delete abstractEventCall;
}

void ThreadEventDispatcher::EventDispatch(AbstractEventCall *abstractEventCall)
{
    LogPedantic("Dispatching event to event support");
    abstractEventCall->Call();
}

void ThreadEventDispatcher::AddEventCall(AbstractEventCall *abstractEventCall)
{
    // Thread must be set prior to call
    Assert(m_thread != NULL);

    LogPedantic("Adding event to thread event loop");

    // Call abstract event call in dedicated thread
    m_thread->PushEvent(abstractEventCall, &StaticEventDispatch, &StaticEventDelete, this);
}

void ThreadEventDispatcher::AddTimedEventCall(AbstractEventCall *abstractEventCall, double dueTime)
{
    // Thread must be set prior to call
    Assert(m_thread != NULL);

    LogPedantic("Adding timed event to thread event loop");

    // Call abstract event call in dedicated thread
    m_thread->PushTimedEvent(abstractEventCall, dueTime, &StaticEventDispatch, &StaticEventDelete, this);
}

}
} // namespace DPL
