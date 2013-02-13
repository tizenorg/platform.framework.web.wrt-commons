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
 * @file        main.cpp
 * @author      Przemyslaw Dobrowolski (p.dobrowolsk@samsung.com)
 * @version     1.0
 * @brief       This file is the implementation file of main for EFL
 */
#include <stddef.h>
#include <dpl/main.h>
#include <dpl/log/log.h>
#include <sys/select.h>
#include <fcntl.h>
#include <errno.h>
#include <dpl/assert.h>
#include <dpl/singleton_impl.h>

IMPLEMENT_SINGLETON(DPL::Main)

namespace DPL {
namespace // anonymous
{
// Late EFL event handling
Main *g_lateMain = NULL;
} // namespace anonymous

Main::Main()
#ifdef DPL_ENABLE_GLIB_LOOP_INTEGRATION_WORKAROUND
// GLIB loop intergration workaround
    : m_oldEcoreSelect(NULL)
#endif // DPL_ENABLE_GLIB_LOOP_INTEGRATION_WORKAROUND
{
    // Late EFL event handling
    Assert(g_lateMain == NULL);
    g_lateMain = this;

    // Increment ECORE init count to ensure we have all
    // subsystems correctly set-up until main dispatcher dtor
    // This is especially important when MainEventDispatcher
    // is a global object destroyed no earlier than crt destroy routine
    ecore_init();

#ifdef DPL_ENABLE_GLIB_LOOP_INTEGRATION_WORKAROUND
    // GLIB loop intergration workaround
    union ConvertPointer
    {
        Ecore_Select_Function pointer;
        EcoreSelectType function;
    } convert;

    convert.pointer = ecore_main_loop_select_func_get();
    m_oldEcoreSelect = convert.function;

    ecore_main_loop_select_func_set(&EcoreSelectInterceptor);
#endif // DPL_ENABLE_GLIB_LOOP_INTEGRATION_WORKAROUND

    // Register event invoker
    m_invokerHandler = ecore_main_fd_handler_add(
            WaitableHandleWatchSupport::WaitableInvokerHandle(),
            ECORE_FD_READ,
            &StaticDispatchInvoker,
            this,
            NULL,
            NULL);

    if (m_invokerHandler == NULL) {
        ThrowMsg(Exception::CreateFailed, "Failed to register invoker handler!");
    }

    // It is impossible that there exist watchers at this time
    // No need to add watchers
    LogPedantic("ECORE event handler registered");
}

Main::~Main()
{
    // Remove any watchers
    for (EcoreFdHandlerList::iterator iterator = m_readWatchersList.begin();
         iterator != m_readWatchersList.end();
         ++iterator)
    {
        ecore_main_fd_handler_del(*iterator);
    }

    m_readWatchersList.clear();

    for (EcoreFdHandlerList::iterator iterator = m_writeWatchersList.begin();
         iterator != m_writeWatchersList.end();
         ++iterator)
    {
        ecore_main_fd_handler_del(*iterator);
    }

    m_writeWatchersList.clear();

    // Remove event invoker
    ecore_main_fd_handler_del(m_invokerHandler);
    m_invokerHandler = NULL;

    //set old ecore select function, because after ecore_shutdown() call,
    //it is being called once again and it may crash.
    ecore_main_loop_select_func_set(m_oldEcoreSelect);
    // Decrement ECORE init count
    // We do not need ecore routines any more
    ecore_shutdown();

    // Late EFL event handling
    Assert(g_lateMain == this);
    g_lateMain = NULL;
}

Eina_Bool Main::StaticDispatchInvoker(void *data, Ecore_Fd_Handler *fd_handler)
{
    LogPedantic("Static ECORE dispatch invoker");

    Main *This = static_cast<Main *>(data);
    (void)fd_handler;

    Assert(This != NULL);

    // Late EFL event handling
    if (g_lateMain == NULL) {
        LogPedantic("WARNING: Late EFL invoker dispatch!");
    } else {
        This->DispatchInvoker();
    }

    return ECORE_CALLBACK_RENEW;
}

Eina_Bool Main::StaticDispatchReadWatcher(void* data,
                                          Ecore_Fd_Handler* fd_handler)
{
    LogPedantic("Static ECORE dispatch read watcher");

    Main *This = static_cast<Main *>(data);

    Assert(This != NULL);

    // Late EFL event handling
    if (g_lateMain == NULL) {
        LogPedantic("WARNING: Late EFL read watcher dispatch!");
    } else {
        This->DispatchReadWatcher(static_cast<WaitableHandle>(
                                      ecore_main_fd_handler_fd_get(fd_handler)));
    }

    return ECORE_CALLBACK_RENEW;
}

Eina_Bool Main::StaticDispatchWriteWatcher(void* data,
                                           Ecore_Fd_Handler* fd_handler)
{
    LogPedantic("Static ECORE dispatch write watcher");

    Main *This = static_cast<Main *>(data);

    Assert(This != NULL);

    // Late EFL event handling
    if (g_lateMain == NULL) {
        LogPedantic("WARNING: Late EFL write watcher dispatch!");
    } else {
        This->DispatchWriteWatcher(static_cast<WaitableHandle>(
                                       ecore_main_fd_handler_fd_get(fd_handler)));
    }

    return ECORE_CALLBACK_RENEW;
}

void Main::DispatchInvoker()
{
    LogPedantic("Dispatching invoker...");

    // Reload watch list
    ReloadWatchList();

    // Handle base invoker
    WaitableHandleWatchSupport::InvokerFinished();

    LogPedantic("Invoker dispatched");
}

void Main::ReloadWatchList()
{
    LogPedantic(
        "Reloading watch list... (" << m_readWatchersList.size() << " + " <<
        m_writeWatchersList.size() << ")");

    // Reload list of watchers
    WaitableHandleListEx waitableWatcherHandles =
        WaitableHandleWatchSupport::WaitableWatcherHandles();

    // Remove not existing read watchers
    EcoreFdHandlerList::iterator watchersIterator = m_readWatchersList.begin();
    WaitableHandleListEx::iterator handlesIterator;

    while (watchersIterator != m_readWatchersList.end()) {
        bool found = false;

        for (handlesIterator = waitableWatcherHandles.begin();
             handlesIterator != waitableWatcherHandles.end();
             ++handlesIterator)
        {
            if (handlesIterator->second == WaitMode::Read &&
                handlesIterator->first ==
                static_cast<WaitableHandle>(ecore_main_fd_handler_fd_get(*
                                                                         watchersIterator)))
            {
                found = true;
                break;
            }
        }

        if (!found) {
            // Unregister handler
            ecore_main_fd_handler_del(*watchersIterator);

            // Remove iterator
            EcoreFdHandlerList::iterator next = watchersIterator;
            ++next;

            m_readWatchersList.erase(watchersIterator);
            watchersIterator = next;
        } else {
            ++watchersIterator;
        }
    }

    // Remove not existing write watchers
    watchersIterator = m_writeWatchersList.begin();

    while (watchersIterator != m_writeWatchersList.end()) {
        bool found = false;

        for (handlesIterator = waitableWatcherHandles.begin();
             handlesIterator != waitableWatcherHandles.end();
             ++handlesIterator)
        {
            if (handlesIterator->second == WaitMode::Write &&
                handlesIterator->first ==
                static_cast<WaitableHandle>(ecore_main_fd_handler_fd_get(*
                                                                         watchersIterator)))
            {
                found = true;
                break;
            }
        }

        if (!found) {
            // Unregister handler
            ecore_main_fd_handler_del(*watchersIterator);

            // Remove iterator
            EcoreFdHandlerList::iterator next = watchersIterator;
            ++next;

            m_writeWatchersList.erase(watchersIterator);
            watchersIterator = next;
        } else {
            ++watchersIterator;
        }
    }

    // Add new read/write watchers
    for (handlesIterator = waitableWatcherHandles.begin();
         handlesIterator != waitableWatcherHandles.end();
         ++handlesIterator)
    {
        if (handlesIterator->second == WaitMode::Read) {
            bool found = false;

            for (watchersIterator = m_readWatchersList.begin();
                 watchersIterator != m_readWatchersList.end();
                 ++watchersIterator)
            {
                if (handlesIterator->first ==
                    static_cast<WaitableHandle>(ecore_main_fd_handler_fd_get(*
                                                                             watchersIterator)))
                {
                    found = true;
                    break;
                }
            }

            if (!found) {
                Ecore_Fd_Handler *handler = ecore_main_fd_handler_add(
                        handlesIterator->first,
                        ECORE_FD_READ,
                        &StaticDispatchReadWatcher,
                        this,
                        NULL,
                        NULL);
                if (handler == NULL) {
                    ThrowMsg(Exception::CreateFailed,
                             "Failed to register read watcher handler!");
                }

                // Push new watcher to list
                m_readWatchersList.push_back(handler);
            }
        } else if (handlesIterator->second == WaitMode::Write) {
            bool found = false;

            for (watchersIterator = m_writeWatchersList.begin();
                 watchersIterator != m_writeWatchersList.end();
                 ++watchersIterator)
            {
                if (handlesIterator->first ==
                    static_cast<WaitableHandle>(ecore_main_fd_handler_fd_get(*
                                                                             watchersIterator)))
                {
                    found = true;
                    break;
                }
            }

            if (!found) {
                Ecore_Fd_Handler *handler = ecore_main_fd_handler_add(
                        handlesIterator->first,
                        ECORE_FD_WRITE,
                        &StaticDispatchWriteWatcher,
                        this,
                        NULL,
                        NULL);
                if (handler == NULL) {
                    ThrowMsg(Exception::CreateFailed,
                             "Failed to register write watcher handler!");
                }

                // Push new watcher to list
                m_writeWatchersList.push_back(handler);
            }
        } else {
            Assert(0);
        }
    }

    LogPedantic(
        "Watch list reloaded  (" << m_readWatchersList.size() << " + " <<
        m_writeWatchersList.size() << ")");
}

void Main::DispatchReadWatcher(WaitableHandle waitableHandle)
{
    LogPedantic("Dispatching read watcher...");

    // Handle watcher
    WaitableHandleWatchSupport::HandleWatcher(waitableHandle, WaitMode::Read);

    LogPedantic("Watcher dispatched");
}

void Main::DispatchWriteWatcher(WaitableHandle waitableHandle)
{
    LogPedantic("Dispatching write watcher...");

    // Handle watcher
    WaitableHandleWatchSupport::HandleWatcher(waitableHandle, WaitMode::Write);

    LogPedantic("Watcher dispatched");
}

Thread *Main::GetInvokerThread()
{
    return NULL;
}

void Main::HandleDirectInvoker()
{
    // Handle direct invoker
    ReloadWatchList();
}

#ifdef DPL_ENABLE_GLIB_LOOP_INTEGRATION_WORKAROUND
// GLIB loop intergration workaround
int Main::EcoreSelectInterceptor(int nfds,
                                 fd_set *readfds,
                                 fd_set *writefds,
                                 fd_set *exceptfds,
                                 struct timeval *timeout)
{
    // We have to check error code here and make another try because of some
    // glib error's.
    fd_set rfds, wfds, efds;
    memcpy(&rfds, readfds, sizeof(fd_set));
    memcpy(&wfds, writefds, sizeof(fd_set));
    memcpy(&efds, exceptfds, sizeof(fd_set));

    int ret = MainSingleton::Instance().m_oldEcoreSelect(nfds,
                                                         readfds,
                                                         writefds,
                                                         exceptfds,
                                                         timeout);

    if (ret == -1) {
        // Check each descriptor to see if it is valid
        for (int i = 0; i < nfds; i++) {
            if (FD_ISSET(i,
                         readfds) ||
                FD_ISSET(i, writefds) || FD_ISSET(i, exceptfds))
            {
                // Try to get descriptor flags
                int result = fcntl(i, F_GETFL);

                if (result == -1) {
                    if (errno == EBADF) {
                        // This a bad descriptor. Remove all occurrences of it.
                        if (FD_ISSET(i, readfds)) {
                            LogPedantic(
                                "GLIB_LOOP_INTEGRATION_WORKAROUND: Bad read descriptor: "
                                << i);
                            FD_CLR(i, readfds);
                        }

                        if (FD_ISSET(i, writefds)) {
                            LogPedantic(
                                "GLIB_LOOP_INTEGRATION_WORKAROUND: Bad write descriptor: "
                                << i);
                            FD_CLR(i, writefds);
                        }

                        if (FD_ISSET(i, exceptfds)) {
                            LogPedantic(
                                "GLIB_LOOP_INTEGRATION_WORKAROUND: Bad exception descriptor: "
                                << i);
                            FD_CLR(i, exceptfds);
                        }
                    } else {
                        // Unexpected error
                        Assert(0);
                    }
                }
            }
        }

        LogPedantic(
            "GLIB_LOOP_INTEGRATION_WORKAROUND: Bad read descriptor. Retrying with default select.");

        //Retry with old values and return new error
        memcpy(readfds, &rfds, sizeof(fd_set));
        memcpy(writefds, &wfds, sizeof(fd_set));
        memcpy(exceptfds, &efds, sizeof(fd_set));

        // Trying to do it very short
        timeval tm;
        tm.tv_sec = 0;
        tm.tv_usec = 10;

        if (timeout) {
            ret = select(nfds, readfds, writefds, exceptfds, &tm);
        } else {
            ret = select(nfds, readfds, writefds, exceptfds, NULL);
        }
    }

    return ret;
}
#endif // DPL_ENABLE_GLIB_LOOP_INTEGRATION_WORKAROUND
} // namespace DPL
