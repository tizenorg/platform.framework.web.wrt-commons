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
 * @file        thread.cpp
 * @author      Przemyslaw Dobrowolski (p.dobrowolsk@samsung.com)
 * @version     1.0
 * @brief       This file is the implementation file of thread
 */
#include <stddef.h>
#include <dpl/thread.h>
#include <dpl/log/log.h>
#include <sys/time.h>
#include <algorithm>
#include <dpl/assert.h>
#include <errno.h>
#include <time.h>
#include <string.h>

namespace // anonymous
{
static const size_t NANOSECONDS_PER_SECOND =
    static_cast<uint64_t>(1000 * 1000 * 1000);

static const size_t NANOSECONDS_PER_MILISECOND =
    static_cast<uint64_t>(1000 * 1000);

static const size_t NANOSECONDS_PER_MICROSECOND =
    static_cast<uint64_t>(1000);

static const pthread_t g_mainThread = pthread_self();

class ThreadSpecific
{
public:
    pthread_key_t threadSpecific;

    ThreadSpecific()
        : threadSpecific(0)
    {
        threadSpecific=0;
        pthread_key_create(&threadSpecific, NULL);
    }

    virtual ~ThreadSpecific()
    {
        pthread_key_delete(threadSpecific);
    }
};

static ThreadSpecific g_threadSpecific;
} // namespace anonymous

namespace DPL
{
bool g_TLSforMainCreated = false;

Thread::Thread()
    : m_thread(0),
      m_abandon(false),
      m_running(false),
      m_directInvoke(false)
{
}

Thread::~Thread()
{
    // Ensure that we quit thread
    // Always wait thread by yourself; if thread is still running
    // this may be sometimes very bad. When derived, some resources
    // may leak or be doubly freed
    Quit();

    // Remove any remainig events
    // Thread proc is surely not running now
    for (InternalEventList::iterator iterator = m_eventList.begin(); iterator != m_eventList.end(); ++iterator)
        iterator->eventDeleteProc(iterator->event, iterator->userParam);

    m_eventList.clear();
}

bool Thread::IsMainThread()
{
    return (pthread_equal(pthread_self(), g_mainThread));
}

Thread *Thread::GetCurrentThread()
{
    if (pthread_equal(pthread_self(), g_mainThread))
        return NULL;

    void *threadSpecific = pthread_getspecific(g_threadSpecific.threadSpecific);

    // Is this a managed thread ?
    if (threadSpecific == NULL)
        Throw(Exception::UnmanagedThread);

    return static_cast<Thread *>(threadSpecific);
}

void *Thread::StaticThreadEntry(void *param)
{
    LogPedantic("Entered static thread entry");

    // Retrieve context
    Thread *This = static_cast<Thread *>(param);
    Assert(This != NULL);

    // Set thread specific
    int result =  pthread_setspecific(g_threadSpecific.threadSpecific, This);

    if (result!=0)
    {
        LogError("Failed to set threadSpecific. Error: " << strerror(result));
    }

    // Enter thread proc
    // Do not allow exceptions to hit pthread core
    UNHANDLED_EXCEPTION_HANDLER_BEGIN
    {
        This->ThreadEntry();
    }
    UNHANDLED_EXCEPTION_HANDLER_END

    // Critical section
    {
        // Leave running state
        Mutex::ScopedLock lock(&This->m_stateMutex);

        This->m_running = false;

        // Abandon thread
        if (This->m_abandon)
        {
            LogPedantic("Thread was abandoned");
            pthread_detach(This->m_thread);
        }
        else
        {
            LogPedantic("Thread is joinable");
        }
    }

    return NULL;
}

int Thread::ThreadEntry()
{
    LogPedantic("Entered default thread entry");
    return Exec();
}

void Thread::ProcessEvents()
{
    LogPedantic("Processing events");
    
    // Steal current event list
    InternalEventList stolenEvents;

    // Enter event list critical section
    {
        Mutex::ScopedLock lock(&m_eventMutex);
        m_eventList.swap(stolenEvents);
        m_eventInvoker.Reset();
    }

    // Process event list
    LogPedantic("Stolen " << stolenEvents.size() << " internal events");

    for (InternalEventList::iterator iterator = stolenEvents.begin(); iterator != stolenEvents.end(); ++iterator)
    {
        // Dispatch immediate event
        iterator->eventDispatchProc(iterator->event, iterator->userParam);

        // Delete event
        iterator->eventDeleteProc(iterator->event, iterator->userParam);
    }
}

void Thread::ProcessTimedEvents()
{
    // Critical section on timed events mutex
    {
        Mutex::ScopedLock lock(&m_timedEventMutex);

        // Get current time
        unsigned long currentTimeMiliseconds = GetCurrentTimeMiliseconds();

        // Info
        LogPedantic("Processing timed events. Time now: " << currentTimeMiliseconds << " ms");

        // All timed events are sorted chronologically
        // Emit timed out events
        while (!m_timedEventVector.empty() &&
               currentTimeMiliseconds >= m_timedEventVector.begin()->registerTimeMiliseconds + m_timedEventVector.begin()->dueTimeMiliseconds)
        {
            // Info
            LogPedantic("Transforming timed event into immediate event. Absolute due time: " <<
                        (m_timedEventVector.begin()->registerTimeMiliseconds + m_timedEventVector.begin()->dueTimeMiliseconds) << " ms");

            // Emit immediate event
            PushEvent(m_timedEventVector.begin()->event,
                      m_timedEventVector.begin()->eventDispatchProc,
                      m_timedEventVector.begin()->eventDeleteProc,
                      m_timedEventVector.begin()->userParam);

            // Remove timed eventand fix heap
            std::pop_heap(m_timedEventVector.begin(), m_timedEventVector.end());
            m_timedEventVector.pop_back();
        }
    }
}

unsigned long Thread::GetCurrentTimeMiliseconds() const
{
    timeval tv;
    gettimeofday(&tv, NULL);
    return static_cast<unsigned long>(tv.tv_sec) * 1000 + static_cast<unsigned long>(tv.tv_usec) / 1000;
}

int Thread::Exec()
{
    LogPedantic("Executing thread event processing");

    const std::size_t MIN_HANDLE_LIST_SIZE = 4;

    // Start processing of events
    WaitableHandleListEx handleList;

    // index 0: Quit waitable event handle
    handleList.push_back(std::make_pair(m_quitEvent.GetHandle(), WaitMode::Read));

    // index 1: Event occurred event handle
    handleList.push_back(std::make_pair(m_eventInvoker.GetHandle(), WaitMode::Read));

    // index 2: Timed event occurred event handle
    handleList.push_back(std::make_pair(m_timedEventInvoker.GetHandle(), WaitMode::Read));

    // index 3: Waitable handle watch support invoker
    handleList.push_back(std::make_pair(WaitableHandleWatchSupport::WaitableInvokerHandle(), WaitMode::Read));

    //
    // Watch list might have been initialized before threaded started
    // Need to fill waitable event watch list in this case
    //
    {
        WaitableHandleListEx waitableHandleWatchHandles = WaitableHandleWatchSupport::WaitableWatcherHandles();
        std::copy(waitableHandleWatchHandles.begin(), waitableHandleWatchHandles.end(), std::back_inserter(handleList));
    }

    // Quit flag
    bool quit = false;

    while (!quit)
    {
        // Retrieve minimum wait time, according to timed events list
        unsigned long minimumWaitTime;

        // Critical section on timed events mutex
        {
            Mutex::ScopedLock lock(&m_timedEventMutex);

            if (!m_timedEventVector.empty())
            {
                unsigned long currentTimeMiliseconds = GetCurrentTimeMiliseconds();
                unsigned long destinationTimeMiliseconds = m_timedEventVector.begin()->registerTimeMiliseconds + m_timedEventVector.begin()->dueTimeMiliseconds;

                // Are we already late with timed event ?
                if (currentTimeMiliseconds > destinationTimeMiliseconds)
                    minimumWaitTime = 0;
                else
                    minimumWaitTime = destinationTimeMiliseconds - currentTimeMiliseconds;
            }
            else
                minimumWaitTime = 0xFFFFFFFF; // Infinity
        }

        // Info
        LogPedantic("Thread loop minimum wait time: " << minimumWaitTime << " ms");

        // Do thread waiting
        WaitableHandleIndexList waitableHandleIndexList = WaitForMultipleHandles(handleList, minimumWaitTime);

        if (waitableHandleIndexList.empty())
        {
            // Timeout occurred. Process timed events.
            LogPedantic("Timed event list elapsed invoker");
            ProcessTimedEvents();
            continue;
        }

        // Go through each index
        for (WaitableHandleIndexList::const_iterator waitableHandleIndexIterator = waitableHandleIndexList.begin();
             waitableHandleIndexIterator != waitableHandleIndexList.end();
             ++waitableHandleIndexIterator)
        {
            size_t index = *waitableHandleIndexIterator;

            LogPedantic("Event loop triggered with index: " << index);

            switch (index)
            {
                case 0:
                    // Quit waitable event handle
                    quit = true;
                    break;

                case 1:
                    // Event occurred event handle
                    ProcessEvents();

                    // Handle direct invoker
                    if (m_directInvoke)
                    {
                        m_directInvoke = false;

                        LogPedantic("Handling direct invoker");

                        // Update list
                        while (handleList.size() > MIN_HANDLE_LIST_SIZE)
                            handleList.pop_back();

                        // Insert current waitable event handles instead
                        {
                            WaitableHandleListEx waitableHandleWatchHandles = WaitableHandleWatchSupport::WaitableWatcherHandles();
                            std::copy(waitableHandleWatchHandles.begin(), waitableHandleWatchHandles.end(), std::back_inserter(handleList));
                        }
                    }

                    // Done
                    break;

                case 2:
                    // Timed event list changed
                    LogPedantic("Timed event list changed invoker");
                    ProcessTimedEvents();

                    // Reset timed event invoker
                    m_timedEventInvoker.Reset();

                    // Done
                    break;

                case 3:
                    // Waitable handle watch support invoker
                    LogPedantic("Waitable handle watch invoker event occurred");

                    // First, remove all previous handles
                    while (handleList.size() > MIN_HANDLE_LIST_SIZE)
                        handleList.pop_back();

                    // Insert current waitable event handles instead
                    {
                        WaitableHandleListEx waitableHandleWatchHandles = WaitableHandleWatchSupport::WaitableWatcherHandles();
                        std::copy(waitableHandleWatchHandles.begin(), waitableHandleWatchHandles.end(), std::back_inserter(handleList));
                    }

                    // Handle invoker in waitable watch support
                    WaitableHandleWatchSupport::InvokerFinished();

                    LogPedantic("Waitable handle watch invoker event handled");

                    // Done
                    break;

                default:
                    // Waitable event watch list
                    LogPedantic("Waitable handle watch event occurred");

                    // Handle event in waitable handle watch
                    {
                        std::pair<WaitableHandle, WaitMode::Type> handle = handleList[index];
                        WaitableHandleWatchSupport::HandleWatcher(handle.first, handle.second);
                    }

                    if (m_directInvoke)
                    {
                        m_directInvoke = false;

                        LogPedantic("Handling direct invoker");

                        // Update list
                        while (handleList.size() > MIN_HANDLE_LIST_SIZE)
                            handleList.pop_back();

                        // Insert current waitable event handles instead
                        {
                            WaitableHandleListEx waitableHandleWatchHandles = 
                                    WaitableHandleWatchSupport::
                                        WaitableWatcherHandles();
                            std::copy(waitableHandleWatchHandles.begin(),
                                      waitableHandleWatchHandles.end(),
                                      std::back_inserter(handleList));
                        }
                    }

                    LogPedantic("Waitable handle watch event handled");

                    // Done
                    break;
            }
        }
    }

    LogPedantic("Leaving thread event processing");
    return 0;
}

void Thread::Run()
{
    LogPedantic("Running thread");

    // Critical section
    {
        Mutex::ScopedLock lock(&m_stateMutex);

        if (m_running)
            return;

        // Try to create new thread
        if (pthread_create(&m_thread, NULL, &StaticThreadEntry, this) != 0)
            Throw(Exception::RunFailed);

        // At default, we abandon thread
        m_abandon = true;

        // Enter running state
        m_running = true;
    }

    LogPedantic("Thread run");
}

void Thread::Quit()
{
    pthread_t joinableThread;

    // Critical section
    {
        Mutex::ScopedLock lock(&m_stateMutex);

        // Is thread running ?
        if (!m_running)
            return;

        LogPedantic("Quitting thread...");

        // Do not abandon thread, we will join
        m_abandon = false;

        // Singal quit waitable event
        m_quitEvent.Signal();

        // Copy joinable thread identifier, because
        // we are leaving critical section
        joinableThread = m_thread;
    }

    // Wait for joinable thread
    void *result;

    if (pthread_join(joinableThread, &result) != 0)
        Throw(Exception::QuitFailed);

    LogPedantic("Thread quit");
}

void Thread::PushEvent(void *event, EventDispatchProc eventDispatchProc, EventDeleteProc eventDeleteProc, void *userParam)
{
    // Enter event list critical section
    Mutex::ScopedLock lock(&m_eventMutex);

    // Push new event
    m_eventList.push_back(InternalEvent(event, userParam, eventDispatchProc, eventDeleteProc));

    // Trigger invoker
    m_eventInvoker.Signal();

    LogPedantic("Event pushed and invoker signaled");
}

void Thread::PushTimedEvent(void *event, double dueTimeSeconds, EventDispatchProc eventDispatchProc, EventDeleteProc eventDeleteProc, void *userParam)
{
    // Check for developer errors
    Assert(dueTimeSeconds >= 0.0);

    // Enter timed event list critical section
    Mutex::ScopedLock lock(&m_timedEventMutex);

    // Get current time
    unsigned long currentTimeMiliseconds = GetCurrentTimeMiliseconds();

    // Convert to miliseconds
    unsigned long dueTimeMiliseconds = static_cast<unsigned long>(1000.0 * dueTimeSeconds);

    // Push new timed event
    m_timedEventVector.push_back(InternalTimedEvent(event, userParam, dueTimeMiliseconds, currentTimeMiliseconds, eventDispatchProc, eventDeleteProc));

    // Heapify timed events
    std::make_heap(m_timedEventVector.begin(), m_timedEventVector.end());

    // Trigger invoker
    m_timedEventInvoker.Signal();

    LogPedantic("Timed event pushed and invoker signaled: due time: " << dueTimeMiliseconds << " ms, absolute due time: " << currentTimeMiliseconds + dueTimeMiliseconds << " ms");
}

Thread *Thread::GetInvokerThread()
{
    return this;
}

void Thread::HandleDirectInvoker()
{
    // We must be in ProcessEvents call stack
    // Mark that situation to handle direct invoker
    m_directInvoke = true;
}

void Thread::Sleep(uint64_t seconds)
{
   NanoSleep(seconds * NANOSECONDS_PER_SECOND);
}

void Thread::MiliSleep(uint64_t miliseconds)
{
    NanoSleep(miliseconds * NANOSECONDS_PER_MILISECOND);
}

void Thread::MicroSleep(uint64_t microseconds)
{
    NanoSleep(microseconds * NANOSECONDS_PER_MICROSECOND);
}

void Thread::NanoSleep(uint64_t nanoseconds)
{
    timespec requestedTime =
    {
        static_cast<time_t>(
            nanoseconds / NANOSECONDS_PER_SECOND),

        static_cast<long>(
            nanoseconds % NANOSECONDS_PER_SECOND)
    };

    timespec remainingTime;

    for (;;)
    {
        if (nanosleep(&requestedTime, &remainingTime) == 0)
            break;

        int error = errno;
        Assert(error == EINTR);

        requestedTime = remainingTime;
    }
}
} // namespace DPL
