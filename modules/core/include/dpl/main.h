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
 * @file        main.h
 * @author      Przemyslaw Dobrowolski (p.dobrowolsk@samsung.com)
 * @version     1.0
 * @brief       This file is the implementation file of main for EFL
 */
#ifndef DPL_MAIN_H
#define DPL_MAIN_H

#include <dpl/waitable_handle_watch_support.h>
#include <dpl/exception.h>
#include <dpl/singleton.h>
#include <dpl/workaround.h>
#include <dpl/framework_efl.h>
#include <list>

namespace DPL {
class Main :
    public WaitableHandleWatchSupport
{
  public:
    class Exception
    {
      public:
        DECLARE_EXCEPTION_TYPE(DPL::Exception, Base)
        DECLARE_EXCEPTION_TYPE(Base, CreateFailed)
    };

  protected:
    Ecore_Fd_Handler *m_invokerHandler;

    static Eina_Bool StaticDispatchInvoker(void *data,
                                           Ecore_Fd_Handler *fd_handler);
    static Eina_Bool StaticDispatchReadWatcher(void *data,
                                               Ecore_Fd_Handler *fd_handler);
    static Eina_Bool StaticDispatchWriteWatcher(void *data,
                                                Ecore_Fd_Handler *fd_handler);

    typedef std::list<Ecore_Fd_Handler *> EcoreFdHandlerList;

    EcoreFdHandlerList m_readWatchersList;
    EcoreFdHandlerList m_writeWatchersList;

    void DispatchInvoker();
    void DispatchReadWatcher(WaitableHandle waitableHandle);
    void DispatchWriteWatcher(WaitableHandle waitableHandle);

    void ReloadWatchList();

    // WaitableHandleWatchSupport
    virtual Thread *GetInvokerThread();
    virtual void HandleDirectInvoker();

#ifdef DPL_ENABLE_GLIB_LOOP_INTEGRATION_WORKAROUND
    // GLIB loop intergration workaround
    typedef int (*EcoreSelectType)(int nfds, fd_set *readfds, fd_set *writefds,
                                   fd_set *exceptfds, struct timeval *timeout);
    EcoreSelectType m_oldEcoreSelect;

    static int EcoreSelectInterceptor(int nfds,
                                      fd_set *readfds,
                                      fd_set *writefds,
                                      fd_set *exceptfds,
                                      struct timeval *timeout);
#endif // DPL_ENABLE_GLIB_LOOP_INTEGRATION_WORKAROUND

  public:
    explicit Main();
    virtual ~Main();
};

/**
 * Main singleton
 */
typedef Singleton<Main> MainSingleton;
} // namespace DPL

#endif // DPL_MAIN_H
